#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <QBuffer>
#include <QEvent>
#include <QObject>
#include <QQueue>

#include "status.h"

class KeyBoardHandler : public QObject
{
    Q_OBJECT
public:
    explicit KeyBoardHandler(QObject *parent, Status *pStatus );
    
signals:
    void keySignal( char );
    void startHoldingSignal();
    void stopHoldingSignal();
    void localPrintRequest();
    
public slots:
    void fKeyEventHandler();

protected:
    bool eventFilter( QObject *obj, QEvent *event );

private:
    Status *status;
    //QQueue<unsigned char> *fromKbdQ;
    QBuffer *fromKbdBuffer;

    void keyPressed( QEvent *event );
    void keyReleased( QEvent *event );
    
};

#endif // KEYBOARDHANDLER_H
