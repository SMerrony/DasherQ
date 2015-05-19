#ifndef CRT_H
#define CRT_H

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPrinter>
#include <QWidget>

#include "bdffont.h"
#include "terminal.h"

class Crt : public QWidget
{
    Q_OBJECT
public:
    explicit Crt(QWidget *parent, Terminal * );

    static const int CHAR_WIDTH = 10;
    static const int CHAR_HEIGHT = 12;
    int rowOffset;
    float zoom;

    void paintEvent(QPaintEvent *);
    void print( QPrinter * );

signals:
    
public slots:


private:
    BDFfont *bdfFont;
    Terminal *terminal;

    QColor bgColor, fgColor, dimColor;

    void setGreenColours();
    void setWhiteColours();
    void drawChar( QPainter *painter, int x, int y, unsigned char charValue );
    void drawDimChar( QPainter *painter, int x, int y, unsigned char charValue );
    void drawReverseChar( QPainter *painter, int x, int y, unsigned char charValue );
};

#endif // CRT_H
