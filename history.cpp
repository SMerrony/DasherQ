#include "history.h"

History::History()
{
    buffer = new QList<QString>();
}

void History::addLine( QString line ) {
    if (buffer->length() == MAX_HISTORY_LINES) buffer->removeFirst();
    buffer->append( line );
}

QString History::fetchAllAsQString() {
    QString text;
    for (int l = 0; l < buffer->size(); l++) text += (buffer->at(l) + "\n");
    return text ;
}
