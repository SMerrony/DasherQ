#include <QDebug>
#include <QFile>
#include <QImage>
#include <QStringList>

#include "bdffont.h"

BDFfont::BDFfont() {

    charCount = 0;

    for (int i = 0; i < 128; i++) {
        map[i].loaded = false;
        map[i].pixmap = new QPixmap( 10, 12 );
        map[i].pixmap->fill( Qt::black );
        map[i].dimPixmap = new QPixmap( 10, 12 );
        map[i].dimPixmap->fill( Qt::black );
        map[i].reversePixmap = new QPixmap( 10, 12 );
        map[i].reversePixmap->fill( QColor( 0, 255, 0, 255 ) );
    }

}

bool BDFfont::load(QString fontFileName) {

    QFile fontFile( fontFileName );

    if (!fontFile.open( QFile::ReadOnly )) {
        qDebug() << "Could not open BDF font file";
        return false;
    }

    QTextStream in( &fontFile );

    while (!in.readLine().startsWith( "ENDPROPERTIES" )); // skip over header
    QString charCountLine( in.readLine() );
    if (!charCountLine.startsWith( "CHARS" )) return false;
    charCount = charCountLine.section( " ", 1 ).toInt();
    QImage tmpImage( 10, 12, QImage::Format_RGB16 );
    QImage tmpDimImage( 10, 12, QImage::Format_RGB16 );
    QImage tmpRevImage( 10, 12, QImage::Format_RGB16 );

    for (int cc = 0; cc < charCount; cc++) {
        tmpImage.fill( Qt::black );
        tmpDimImage.fill( Qt::black );
        tmpRevImage.fill( QColor( 0, 255, 0, 255 ) );

        // skip to start of character
        while (!in.readLine().startsWith( "STARTCHAR" ));
        QString encodingLine = in.readLine();
        if (!encodingLine.startsWith( "ENCODING" )) return false;
        int asciiCode = encodingLine.section( " ", 1 ).toInt();
        // qDebug() << "BDFfont: Debug - loading character " << asciiCode;

        // skip two lines (SWIDTH & DWIDTH)
        in.readLine(); in.readLine();

        // decode the BBX line
        QString bbxLine = in.readLine();
        if (!bbxLine.startsWith( "BBX" )) return false;
        QStringList bbxTokens = bbxLine.split( " " );
        unsigned int pixWidth = bbxTokens.at(1).toInt();
        int pixHeight = bbxTokens.at(2).toInt();
        int xOffset = bbxTokens.at(3).toInt();
        int yOffset = bbxTokens.at(4).toInt();

        // skip the BITMAP line
        in.readLine();

        // load the actual bitmap for this char a row at a time from the top down
        bool ok;
        for (int bitMapLine = pixHeight - 1; bitMapLine >= 0; bitMapLine--) {
            QString lineStr = in.readLine();
            unsigned char lineByte = lineStr.toInt( &ok, 16 );
            for (unsigned int i=0; i < pixWidth; i++) {
                bool pix = ((lineByte & 0x80) >> 7) == 1; // test the MSB
                if (pix) {
                    tmpImage.setPixel( xOffset + i, bitMapLine + yOffset, qRgb( 0, 255, 0 ) );
                    tmpDimImage.setPixel( xOffset + i, bitMapLine + yOffset, qRgb( 0, 128, 0 ) );
                    tmpRevImage.setPixel( xOffset + i, bitMapLine + yOffset, qRgb( 0, 0, 0 ) );
                }
                lineByte = (lineByte << 1);
            }
        }
        map[asciiCode].pixmap->convertFromImage( tmpImage.mirrored() ); //  flip vertically
        map[asciiCode].dimPixmap->convertFromImage( tmpDimImage.mirrored() );
        map[asciiCode].reversePixmap->convertFromImage( tmpRevImage.mirrored() );
        map[asciiCode].loaded = true;
    } // end for each char

    fontFile.close();

    qDebug() << "BDFfont: Loaded " << charCount << " characters";

    return true;
}
