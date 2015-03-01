#include <QDebug>

#include <QPainter>

#include "crt.h"


Crt::Crt(QWidget *parent, Terminal *pTerminal ) : QWidget( parent ) {

    terminal = pTerminal;

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

    painter.setWindow(0,0, terminal->visible_cols * CHAR_WIDTH, terminal->visible_rows * CHAR_HEIGHT );
    painter.setPen( fgColor );

    for (int y = 0; y < terminal->visible_rows; y++) {
        for (int x = 0; x < terminal->visible_cols; x++) {

            // first fill the cell with the right background colour, then set the right foreground colour
            if (terminal->display[y][x].reverse) {
                painter.fillRect( x * CHAR_WIDTH, y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, fgColor );
                currColor = bgColor;
            } else {
                painter.fillRect( x * CHAR_WIDTH, y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, bgColor );
                currColor = fgColor;
            }

            // draw the character - but handle blinking
            if (terminal->blinking_enabled && terminal->blinkState && terminal->display[y][x].blink) {
                painter.fillRect( x * CHAR_WIDTH, (y + 1) * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, bgColor );
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
                painter.drawLine( x * CHAR_WIDTH, (y + 1) * CHAR_HEIGHT, (x + 1) * CHAR_WIDTH, (y + 1) * CHAR_HEIGHT );
            }

        } // end for x
    } // end for y

    // draw the cursor - if on-screen
    if (terminal->cursorX < terminal->visible_cols && terminal->cursorY < terminal->visible_rows) {
        painter.fillRect( terminal->cursorX * CHAR_WIDTH, terminal->cursorY * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, fgColor );
        // draw inverted character if present
        if (terminal->display[terminal->cursorY][terminal->cursorX].charValue != ' ') {
            drawReverseChar( &painter, terminal->cursorX, terminal->cursorY, terminal->display[terminal->cursorY][terminal->cursorX].charValue );
        }
    }
}

inline void Crt::drawChar( QPainter *painter, int x, int y, unsigned char charValue ) {
    if ( bdfFont->map[charValue].loaded) {
        painter->drawPixmap( x * CHAR_WIDTH , y * CHAR_HEIGHT, *(bdfFont->map[charValue].pixmap) );
    }
}

inline void Crt::drawDimChar( QPainter *painter, int x, int y, unsigned char charValue ) {
    if ( bdfFont->map[charValue].loaded) {
        painter->drawPixmap( x * CHAR_WIDTH , y * CHAR_HEIGHT, *(bdfFont->map[charValue].dimPixmap) );
    }
}

inline void Crt::drawReverseChar( QPainter *painter, int x, int y, unsigned char charValue ) {
    if ( bdfFont->map[charValue].loaded) {
        painter->drawPixmap( x * CHAR_WIDTH , y * CHAR_HEIGHT, *(bdfFont->map[charValue].reversePixmap) );
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

    for (int y = 0; y < terminal->visible_rows; y++) {
        for (int x = 0; x < terminal->visible_cols; x++) {

            // first fill the cell with the right background colour, then set the right foreground colour
            if (terminal->display[y][x].reverse) {
                pPainter.fillRect( x * CHAR_WIDTH, y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, fgColor );
            }

            if (terminal->display[y][x].charValue > 31  && terminal->display[y][x].charValue < 128) {
                if (terminal->display[y][x].reverse) {
                    pPainter.setPen( bgColor );
                    pPainter.drawText( x * CHAR_WIDTH, y * CHAR_HEIGHT, QString( terminal->display[y][x].charValue ) );
                } else if (terminal->display[y][x].dim) {
                    pPainter.setPen( Qt::gray );
                    pPainter.drawText( x * CHAR_WIDTH, y * CHAR_HEIGHT, QString( terminal->display[y][x].charValue ) );
                } else {
                    pPainter.setPen( Qt::black );
                    pPainter.drawText( x * CHAR_WIDTH, y * CHAR_HEIGHT, QString( terminal->display[y][x].charValue ) );
                }
            }

            // underscore
            if (terminal->display[y][x].underscore) {
                pPainter.drawLine( x * CHAR_WIDTH, (y + 1) * CHAR_HEIGHT, (x + 1) * CHAR_WIDTH, (y + 1) * CHAR_HEIGHT );
            }

        } // end for x
    } // end for y

    pPainter.end();
}
