#ifndef BDFFONT_H
#define BDFFONT_H

#include <QBitArray>
#include <QPixmap>
#include <QString>

class BDFfont
{
    struct CharBitMap {
        bool loaded;
        QPixmap *pixmap; // this is to replace the  QBitArray
        QPixmap *dimPixmap;
        QPixmap *reversePixmap;
    };

public:
    BDFfont();
    bool load( QString fontFileName );

    int charCount;
    CharBitMap map[128];

};

#endif // BDFFONT_H
