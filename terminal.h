#ifndef SCREENTHREAD_H
#define SCREENTHREAD_H

#include <QByteArray>
#include <QObject>

#include "cell.h"
#include "status.h"

class Terminal : public QObject {
    Q_OBJECT
public:
    explicit Terminal( Status * );

    static const int VISIBLE_ROWS = 24;
    static const int VISIBLE_COLS = 80;

    static const unsigned char NUL			=  0;
    static const unsigned char PRINT_FORM	=  1;

    static const unsigned char BLINK_ENABLE =  3; // for the whole screen
    static const unsigned char BLINK_DISABLE =  4; // for the whole screen
    static const unsigned char READ_WINDOW_ADDR =  5; // REQUIRES RESPONSE

    static const unsigned char BELL 		=  7;
    static const unsigned char HOME			=  8; // window home
    static const unsigned char TAB 			=  9;
    static const unsigned char NL 			=  10;
    static const unsigned char ERASE_EOL 	=  11;
    static const unsigned char ERASE_WINDOW =  12; // implies window home too
    static const unsigned char CR 			=  13;
    static const unsigned char BLINK_ON 	=  14;
    static const unsigned char BLINK_OFF 	=  15;
    static const unsigned char WRITE_WINDOW_ADDR =  16;
    static const unsigned char PRINT_WINDOW	=  17;
    static const unsigned char ROLL_ENABLE  =  18; // R
    static const unsigned char ROLL_DISABLE =  19; // S (!)
    static const unsigned char UNDERSCORE_ON = 20;
    static const unsigned char UNDERSCORE_OFF = 21;

    static const unsigned char CURSOR_UP 	=  23;
    static const unsigned char CURSOR_RIGHT =  24;
    static const unsigned char CURSOR_LEFT 	=  25;
    static const unsigned char CURSOR_DOWN 	=  26;

    static const unsigned char DIM_ON 		=  28;
    static const unsigned char DIM_OFF 		=  29;

    static const unsigned char CMD			=  30;

    static const unsigned char SPACE 		=  32;

    int cursorX, cursorY;
    bool roll_enabled, blinking_enabled, blinkState, protection_enabled;

    Cell display[VISIBLE_ROWS][VISIBLE_COLS];

private:
    Status *status;
    bool inCommand, inExtendedCommand,
         readingWindowAddressX, readingWindowAddressY,
         blinking, dimmed, reversedVideo, underscored, protectd;
    bool inTelnetCommand, gotTelnetDo, gotTelnetWill;
    unsigned char telnetCmd, doAction, willAction;
    int newXaddress, newYaddress;
    QDataStream *loggingStream;
    QByteArray bufferByteArray;

public:
    void clearLine( int line );
    void clearScreen();
    void eraseUnprotectedToEndOfScreen();
    void scrollUp( int rows );


signals:
    void keySignal( char );
    void logCharSignal( char );
    
public slots:
    void processHostData( QByteArray );
    void startBuffering();
    void stopBuffering();
    void selfTest();

};

#endif // SCREENTHREAD_H
