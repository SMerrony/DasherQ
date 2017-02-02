#ifndef HISTORY_H
#define HISTORY_H

#include <QList>
#include <QString>

const int MAX_HISTORY_LINES = 2000;

class History
{
public:
    History();
    void addLine( QString );
    QString fetchAllAsQString();

    QList<QString> *buffer;

};

#endif // HISTORY_H
