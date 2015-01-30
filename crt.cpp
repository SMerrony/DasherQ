#include <QDebug>

#include <QPainter>

#include "crt.h"


Crt::Crt(QWidget *parent, Terminal *pTerminal ) : QWidget( parent ) {

    terminal = pTerminal;

    charWidth = 10;
    charHeight = 12;

    setGreenColours();

    bdfFont = new BDFfont();
    if (!bdfFont->load( ":/D410-b-12.bdf" )) {
        qDebug() << "Crt: Fatal error - could not load custom Dasher font";
        exit( 1 );
    }
}

void Crt::setGreenColours() {
    bgColor = QColor( 0, 0, 0, 255 );
    fgColor = QColor( 0, 255, 0, 255 );
    dimColor = QColor( 0, 127, 0, 255 );
}
void Crt::setWhiteColours() {
    bgColor = QColor( 0, 0, 0, 255 );
    fgColor = QColor( 255, 255, 255, 255 );
    dimColor = QColor( 127, 127, 127, 255 );
}

void Crt::paintEvent( QPaintEvent * ) {

    QPainter painter( this );

    QColor currColor;

    painter.setWindow(0,0, 800,288);
    painter.setPen( fgColor );

    for (int y = 0; y < Terminal::VISIBLE_ROWS; y++) {
        for (int x = 0; x < Terminal::VISIBLE_COLS; x++) {

            // first fill the cell with the right background colour, then set the right foreground colour
            if (terminal->display[y][x].reverse) {
                painter.fillRect( x * charWidth, y * charHeight, charWidth, charHeight, fgColor );
                currColor = bgColor;
            } else {
                painter.fillRect( x * charWidth, y * charHeight, charWidth, charHeight, bgColor );
                currColor = fgColor;
            }

            // draw the character - but handle blinking
            if (terminal->blinking_enabled && terminal->blinkState && terminal->display[y][x].blink) {
                painter.fillRect( x * charWidth, (y + 1) * charHeight, charWidth, charHeight, bgColor );
                continue; // nothing else to do for this cell if in a blanked blink cycle
            } else {
                if (terminal->display[y][x].charValue > 31  && terminal->display[y][x].charValue < 128) {
                    if (terminal->display[y][x].reverse) {
                        drawReverseChar( &painter, x,y , terminal->display[y][x].charValue );
                        currColor = bgColor;
                    } else if (terminal->display[y][x].dim) {
                        drawDimChar( &painter, x,y , terminal->display[y][x].charValue );
                        currColor = dimColor;
                    } else {
                        drawChar( &painter, x,y , terminal->display[y][x].charValue );
                        currColor = fgColor;
                    }
                }
            }

            // underscore
            if (terminal->display[y][x].underscore) {
                painter.setPen( currColor );
                painter.drawLine( x * charWidth, (y + 1) * charHeight, (x + 1) * charWidth, (y + 1) * charHeight );
            }

        } // end for x
    } // end for y

    // draw the cursor - if on-screen
    if (terminal->cursorX < terminal->VISIBLE_COLS && terminal->cursorY < terminal->VISIBLE_ROWS) {
        painter.fillRect( terminal->cursorX * charWidth, terminal->cursorY * charHeight, charWidth, charHeight, fgColor );
        // draw inverted character if present
        if (terminal->display[terminal->cursorY][terminal->cursorX].charValue != ' ') {
            drawReverseChar( &painter, terminal->cursorX, terminal->cursorY, terminal->display[terminal->cursorY][terminal->cursorX].charValue );
        }
    }
}

inline void Crt::drawChar( QPainter *painter, int x, int y, unsigned char charValue ) {
    if ( bdfFont->map[charValue].loaded) {
        painter->drawPixmap( x * charWidth , y * charHeight, *(bdfFont->map[charValue].pixmap) );
    }
}

inline void Crt::drawDimChar( QPainter *painter, int x, int y, unsigned char charValue ) {
    if ( bdfFont->map[charValue].loaded) {
        painter->drawPixmap( x * charWidth , y * charHeight, *(bdfFont->map[charValue].dimPixmap) );
    }
}

inline void Crt::drawReverseChar( QPainter *painter, int x, int y, unsigned char charValue ) {
    if ( bdfFont->map[charValue].loaded) {
        painter->drawPixmap( x * charWidth , y * charHeight, *(bdfFont->map[charValue].reversePixmap) );
    }
}

void Crt::print( QPrinter *printer ) {

    printer->setResolution( QPrinter::HighResolution );
    QPainter pPainter;
    pPainter.begin( printer );

    double xscale = printer->pageRect().width()/double( 880 );
    double yscale = printer->pageRect().height()/double( 310 );
    double scale = qMin( xscale, yscale );
    pPainter.translate( printer->paperRect().x() + printer->pageRect().width()/2,
                        printer->paperRect().y() + printer->pageRect().height()/2);
    pPainter.scale( scale, scale );
    pPainter.translate( -width()/2, -height()/2 );

    for (int y = 0; y < Terminal::VISIBLE_ROWS; y++) {
        for (int x = 0; x < Terminal::VISIBLE_COLS; x++) {

            // first fill the cell with the right background colour, then set the right foreground colour
            if (terminal->display[y][x].reverse) {
                pPainter.fillRect( x * charWidth, y * charHeight, charWidth, charHeight, fgColor );
            }

            if (terminal->display[y][x].charValue > 31  && terminal->display[y][x].charValue < 128) {
                if (terminal->display[y][x].reverse) {
                    pPainter.setPen( bgColor );
                    pPainter.drawText( x * charWidth, y * charHeight, QString( terminal->display[y][x].charValue ) );
                } else if (terminal->display[y][x].dim) {
                    pPainter.setPen( Qt::gray );
                    pPainter.drawText( x * charWidth, y * charHeight, QString( terminal->display[y][x].charValue ) );
                } else {
                    pPainter.setPen( Qt::black );
                    pPainter.drawText( x * charWidth, y * charHeight, QString( terminal->display[y][x].charValue ) );
                }
            }

            // underscore
            if (terminal->display[y][x].underscore) {
                pPainter.drawLine( x * charWidth, (y + 1) * charHeight, (x + 1) * charWidth, (y + 1) * charHeight );
            }

        } // end for x
    } // end for y

    pPainter.end();
}
