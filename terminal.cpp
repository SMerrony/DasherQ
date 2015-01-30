#include <QApplication>
#include <QDebug>

#include <string.h>

#include "terminal.h"
#include "telnetconnection.h"

Terminal::Terminal( Status *pStatus ){
    // get the shared objects
    status = pStatus;

    cursorX = 0;
    cursorY = 0;
    roll_enabled = true;
    blinking_enabled = true;
    blinkState = false;
    protection_enabled = false;
    inCommand = false; inExtendedCommand = false;
    inTelnetCommand = false; gotTelnetDo = false; gotTelnetWill = false;
    readingWindowAddressX = false;
    readingWindowAddressY = false;
    blinking = false;
    dimmed = false;
    reversedVideo = false;
    underscored = false;
    protectd = false;

    clearScreen();
    display[12][39].charValue = 'O';
    display[12][40].charValue = 'K';
}

void Terminal::clearLine( int line ) {
    for (int cc = 0; cc < VISIBLE_COLS; cc++) {
        display[line][cc].clearToSpace();
    }
    inCommand = false;
    readingWindowAddressX = false;
    readingWindowAddressY = false;
    blinking = false;
    dimmed = false;
    reversedVideo = false;
    underscored = false;
}

void Terminal::clearScreen() {
    for (int row = 0; row < VISIBLE_ROWS; row++){
        clearLine( row );
    }
}

void Terminal::eraseUnprotectedToEndOfScreen() {
    // clear remainder of line
    for (int x = cursorX; x < VISIBLE_COLS; x++) {
        display[cursorY][x].clearToSpaceIfUnprotected();
    }
    // clear all lines below
    for (int y = cursorY + 1; y < VISIBLE_ROWS; y++) {
        for (int x = 0; x < VISIBLE_COLS; x++) {
            display[y][x].clearToSpaceIfUnprotected();
        }
    }
}

void Terminal::scrollUp( int rows ) {
    for (int times = 0; times < rows; times++) {
        // move each char up a row
        for (int r = 1; r < VISIBLE_ROWS; r++) {
            for (int c = 0; c < VISIBLE_COLS; c++) {
                display[r-1][c].copy( display[r][c] );
            }
        }
        // clear the bottom row
        clearLine( VISIBLE_ROWS - 1 );
    }
}

void Terminal::selfTest() {

    char testlineHRule1[] = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
    char testlineHRule2[] = "         1         2         3         4         5         6         7         8";
    char testline1[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567489!\"$%^.";
    char testlineN[] = "Normal  : ";
    char testlineD[] = "Dim     : ";
    char testlineB[] = "Blink   : ";
    char testlineU[] = "Under   : ";
    char testlineR[] = "Reverse : ";

    QByteArray qba;

    emit keySignal( ERASE_WINDOW );
    qba.append(ERASE_WINDOW);

    qba.append( testlineHRule1 );
    qba.append( NL );

    qba.append( testlineHRule2 );
    qba.append( NL );

    qba.append( testlineN );
    qba.append( testline1 );
    qba.append( NL );

    qba.append( testlineD );
    qba.append( DIM_ON );
    qba.append( testline1 );
    qba.append( DIM_OFF );
    qba.append( NL );

    qba.append( testlineU );
    qba.append( UNDERSCORE_ON );
    qba.append( testline1 );
    qba.append( UNDERSCORE_OFF );
    qba.append( NL );

    qba.append( testlineB );
    qba.append( BLINK_ON );
    qba.append( testline1 );
    qba.append( BLINK_OFF );
    qba.append( NL );

    qba.append( testlineR );
    qba.append( CMD );
    qba.append( 'D' );
    qba.append( testline1 );
    qba.append( CMD );
    qba.append( 'E' );
    qba.append( NL );

    processHostData( qba );
}

void Terminal::processHostData( QByteArray hostDataBA ) {

    bool skipChar;
    unsigned char ch;

    if (status->holding) {
        for (int ix = 0; ix < hostDataBA.size(); ix++) {
            bufferByteArray.append( hostDataBA.at( ix ) );
        }
    } else {

        for (int ix = 0; ix < hostDataBA.size(); ix++) {

            ch = hostDataBA.at( ix );

            // qDebug() << "Terminal got " << ch;

            skipChar = false;


            // check for Telnet command
            if (status->connection == Status::TELNET_CONNECTED && ch == TelnetConnection::CMD_IAC) {
                if (inTelnetCommand) {
                    // special case - host really wants to send a 255 - let it through
                    inTelnetCommand = false;
                } else {
                    inTelnetCommand = true;
                    skipChar = true;
                    continue;
                }
            }

            if (status->connection == Status::TELNET_CONNECTED && inTelnetCommand ) {

                // get command unsigned char
                telnetCmd = ch;
                switch (telnetCmd) {
                case TelnetConnection::CMD_DO:
                    gotTelnetDo = true;
                    skipChar = true;
                    break;
                case TelnetConnection::CMD_WILL:
                    gotTelnetWill = true;
                    skipChar = true;
                    break;
                case TelnetConnection::CMD_AO:
                case TelnetConnection::CMD_AYT:
                case TelnetConnection::CMD_BRK:
                case TelnetConnection::CMD_DM:
                case TelnetConnection::CMD_DONT:
                case TelnetConnection::CMD_EC: // ??? Erase Char - should we map this to BS/CurLeft?
                case TelnetConnection::CMD_EL:
                case TelnetConnection::CMD_GA:
                case TelnetConnection::CMD_IP:
                case TelnetConnection::CMD_NOP:
                case TelnetConnection::CMD_SB: // should probably skip to SE...
                case TelnetConnection::CMD_SE:
                    skipChar = true;
                    break;
                default:
                    break;
                }

            }
            if (skipChar) { continue; }


            if (status->connection == Status::TELNET_CONNECTED && gotTelnetDo) {
                // whatever the host asks us to do we will refuse
                doAction = ch;
                emit keySignal( TelnetConnection::CMD_IAC );
                emit keySignal( TelnetConnection::CMD_WONT );
                emit keySignal( doAction );
                gotTelnetDo = false;
                inTelnetCommand = false;
                skipChar = true;
            }

            if (status->connection == Status::TELNET_CONNECTED && gotTelnetWill) {
                // whatever the host offers to do we will refuse
                willAction = ch;
                emit keySignal( TelnetConnection::CMD_IAC );
                emit keySignal( TelnetConnection::CMD_DONT );
                emit keySignal( willAction );
                gotTelnetWill = false;
                inTelnetCommand = false;
                skipChar = true;
            }

            if (skipChar) { continue; }

            if (readingWindowAddressX) {
                newXaddress = (int) ch & 0x7f;
                if (newXaddress >= VISIBLE_COLS) {
                    newXaddress -= VISIBLE_COLS;
                }
                if (newXaddress == 127) {
                    // special case - x stays the same - see D410 User Manual p.3-25
                    newXaddress = cursorX;
                }
                readingWindowAddressX = false;
                readingWindowAddressY = true;
                skipChar = true;
                continue;
            }

            if (readingWindowAddressY) {
                newYaddress = (int) ch & 0x7f;
                cursorX = newXaddress;
                cursorY = newYaddress;
                if (newYaddress == 127) {
                    // special case - y stays the same - see D410 User Manual p.3-25
                    newYaddress = cursorY;
                }
                if (cursorY >= VISIBLE_ROWS) {
                    // see end of p.3-24 in D410 User Manual
                    if (roll_enabled) {
                        scrollUp( cursorY - (VISIBLE_ROWS - 1));
                    }
                    cursorY -= VISIBLE_ROWS;
                }
                readingWindowAddressY = false;
                skipChar = true;
                continue;
            }

            // logging output chars
            if (status->logging) {
                emit logCharSignal( ch );
            }

            // D200 commands
            if (inCommand) {
                switch (ch) {
                case 'C':	 	// REQUIRES RESPONSE
                    // read model ID
                    emit keySignal( 036 );
                    emit keySignal( 0157 );
                    emit keySignal( 043 );  // model report
                    emit keySignal( 041 );  // D100/D200
                    emit keySignal( 74 ); // 0b01001010 ); // see p.2-7 of D100/D200 User Manual
                    emit keySignal( 003 );  // firmware code
                    skipChar = true;
                    break;
                case 'D':
                    reversedVideo = true;
                    skipChar = true;
                    break;
                case 'E':
                    reversedVideo = false;
                    skipChar = true;
                    break;
                default:
                    // System->out.printf( "Screen: Warning - Unrecognised Break-CMD code '%s'\n", ch );
                    break;
                }

                // D210 commands
                if (status->emulation >= 210 && ch == 'F') {
                    inExtendedCommand = true;
                    skipChar = true;
                }

                if (status->emulation >= 210 && inExtendedCommand) {
                    switch (ch) {
                    case 'F':
                        eraseUnprotectedToEndOfScreen();
                        skipChar = true;
                        inExtendedCommand = false;
                        break;
                    }
                }

                // D211 commands
                //            if (status->emulation >= 211) {
                //                unsigned char extCmd;
                //                switch (ch) {
                //                case 'F': // extended commands...
                //                    extCmd = fromHostQ->dequeue();
                //                    switch (extCmd) {
                //                    case 'S': // Select Char set
                //                        break;
                //                    }
                //                    break;
                //                case 'N': // shift in
                //                    break;
                //                case 'O': // shift out
                //                    break;
                //                }
                //            }

                // D400 commands
                //            if (status->emulation >= 400) {
                //                unsigned char extCmd;
                //                switch (ch) {
                //                case 'F': // extended commands...
                //                    // get extended command unsigned char
                //                    extCmd = fromHostQ->dequeue();
                //                    switch (extCmd) {
                //                    case 'I': // delete line
                //                        break;
                //                    case '\\': // <134> delete line between margins
                //                        break;
                //                    case 'E': // Erase screen
                //                        break;
                //                    case ']': // Horizontal scroll disable
                //                        break;
                //                    case '^': // Horizontal scroll enable
                //                        break;
                //                    case 'H': // Insert line
                //                        break;
                //                    case '[': // Insert line between margins
                //                        break;
                //                    case 'a': // Print pass thru off
                //                        break;
                //                    case '\'': // Print pass thru on
                //                        break;
                //                    case 'W': // Protect disable
                //                        break;
                //                    case 'V': // Protect enable
                //                        break;
                //                    case 'O': // Read horizontal scroll offset
                //                        break;
                //                    case 'b': // Read screen address
                //                        break;
                //                    case 'A': // RESET
                //                        break;
                //                    case 'Z': // Restore normal margins
                //                        break;
                //                    case 'G': // Screen home
                //                        break;
                //                    case 'C': // Scroll left n
                //                        break;
                //                    case 'D': // Scroll right n
                //                        break;
                //                    case 'K': // Select compressed spacing
                //                        break;
                //                    case 'J': // Select normal spacing
                //                        break;
                //                    case 'Y': // Set alternate margins i,j,k
                //                        break;
                //                    case 'Q': // Set cursor type n
                //                        break;
                //                    case 'X': // Set margins i,j,k
                //                        break;
                //                    case 'T': // Set scroll rate n
                //                        break;
                //                    case 'B': // Set windows ....
                //                        break;
                //                    case '_': // Show columns i,j
                //                        break;
                //                    case '?': // Window bit dump ('5')
                //                        break;
                //                    case 'P': // Write screen address
                //                        break;
                //                    }
                //                    break;
                //                case 'H': // scroll up
                //                    break;
                //                case 'I': // scroll down
                //                    break;
                //                case 'J': // insert char
                //                    break;
                //                case 'K': // delete char
                //                    break;
                //                }
                //            }

                inCommand = false;
                continue;
            }

            if (skipChar) { continue; }

            switch (ch) {
            case NUL:
                skipChar = true;
                break;
            case BELL:
                QApplication::beep();
                skipChar = true;
                break;
            case BLINK_DISABLE:
                blinking_enabled = false;
                skipChar = true;
                break;
            case BLINK_ENABLE:
                blinking_enabled = true;
                skipChar = true;
                break;
            case BLINK_OFF:
                blinking = false;
                skipChar = true;
                break;
            case BLINK_ON:
                blinking = true;
                skipChar = true;
                break;
            case CURSOR_UP:
                if (cursorY > 0) { cursorY--; } else { cursorY = VISIBLE_ROWS - 1; }
                skipChar = true;
                break;
            case CURSOR_DOWN:
                if (cursorY < VISIBLE_ROWS - 2 ) { cursorY++; } else { cursorY = 0; }
                skipChar = true;
                break;
            case CURSOR_RIGHT:
                if (cursorX < VISIBLE_COLS - 2) { cursorX++; } else {
                    cursorX = 0;
                    if (cursorY < VISIBLE_ROWS - 2 ) { cursorY++; } else { cursorY = 0; }
                }
                skipChar = true;
                break;
            case CURSOR_LEFT:
                if (cursorX > 0) {
                    cursorX--;
                } else {
                    cursorX = VISIBLE_COLS - 1;
                    if (cursorY > 0) { cursorY--; } else { cursorY = VISIBLE_ROWS - 1; }
                }
                skipChar = true;
                break;
            case DIM_ON:
                dimmed = true;
                skipChar = true;
                break;
            case DIM_OFF:
                dimmed = false;
                skipChar = true;
                break;
            case HOME:
                cursorX = 0; cursorY = 0;
                skipChar = true;
                break;
            case ERASE_EOL:
                for (int col = cursorX; col < VISIBLE_COLS; col++) {
                    display[cursorY][col].clearToSpace();
                }
                skipChar = true;
                break;
            case ERASE_WINDOW:
                clearScreen();
                cursorX = 0; cursorY = 0;
                skipChar = true;
                break;
            case ROLL_DISABLE:
                roll_enabled = false;
                skipChar = true;
                break;
            case READ_WINDOW_ADDR: 	 // REQUIRES RESPONSE - see D410 User Manual p.3-18
                emit keySignal(31 );
                emit keySignal( cursorX );
                emit keySignal( cursorY );
                skipChar = true;
                break;
            case ROLL_ENABLE:
                roll_enabled = true;
                skipChar = true;
                break;
            case UNDERSCORE_ON:
                underscored = true;
                skipChar = true;
                break;
            case UNDERSCORE_OFF:
                underscored = false;
                skipChar = true;
                break;
            case WRITE_WINDOW_ADDR:
                readingWindowAddressX = true;
                skipChar = true;
                break;
            case CMD:
                inCommand = true;
                skipChar = true;
                break;
            }

            if (skipChar) { continue; }

            // wrap due to hitting margin or new line?
            if (cursorX == VISIBLE_COLS || ch == NL) {
                if (cursorY == VISIBLE_ROWS - 1) { // hit bottom of screen
                    if (roll_enabled) {
                        scrollUp( 1 );
                    } else {
                        cursorY = 0;
                        clearLine( cursorY );
                    }
                } else {
                    cursorY++;
                    if (!roll_enabled) {
                        clearLine( cursorY );
                    }
                }
                cursorX = 0;
            }

            // CR?
            if (ch == CR || ch == NL ) {
                cursorX = 0;
                continue;
            }

            // finally, put the character in the displayable character matrix
            // it will get picked up on next refresh by Crt
            if (ch > 0) {
                display[cursorY][cursorX].set(  ch, blinking, dimmed, reversedVideo, underscored, protectd );
            } else {
                qDebug() << "Terminal: Warning - Ignoring character with code " << ch;
            }

            cursorX++;
        } // end-for each char in bytearray

    } // end-if buffering

    // something has arrived at the terminal - so set the screen status to dirty so that the crt will
    // redraw on its next cycle
    status->dirty = true;
}

/* once this signal is received all data is held in a QByteArray for later processing */
void Terminal::startBuffering() {

    status->holding = true;
    bufferByteArray.resize( 0 );
}

/* on receipt of this signal 'play back' the buffer into the terminal */
void Terminal::stopBuffering() {

    status->holding = false;
    processHostData( bufferByteArray );
    bufferByteArray.resize( 0 );
}
