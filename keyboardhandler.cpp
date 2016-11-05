#include <QDebug>
#include <QKeyEvent>
#include <QGuiApplication>

#include "keyboardhandler.h"

KeyBoardHandler::KeyBoardHandler( QObject *parent, Status *pStatus )
    : QObject( parent ) {
    myParent = (QWidget *)parent;
    status = pStatus;
    // qDebug() << "KeyboardHandler created";
}

void KeyBoardHandler::fKeyEventHandler( QString text )  {

    unsigned char modifier = 0;
    QString fKeyLabel;

    // if we are called with an explicit (non-empty) string then use it
    if (text.compare( "" ) == 0) {
        fKeyLabel = sender()->property("text").toString();
    } else {
        fKeyLabel = text;
    }

    if (QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier)) { modifier -= 16; }
    if (QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier)) { modifier -= 64; }

    // qDebug() << "KeyboardHandler received fKeyEvent " << fKeyLabel << " with modifier " << modifier;

    if (fKeyLabel.compare( "Break" ) == 0) {
        qDebug() << "Break requested by user";
        emit breakSignal(); // special CMD_BREAK indicator (trapped by connection handler)
    } else if (fKeyLabel.compare( "F1" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 113 + modifier );
    } else if (fKeyLabel.compare( "F2" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 114 + modifier );
    } else if (fKeyLabel.compare( "F3" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 115 + modifier );
    } else if (fKeyLabel.compare( "F4" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 116 + modifier );
    } else if (fKeyLabel.compare( "F5" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 117 + modifier );
    } else if (fKeyLabel.compare( "F6" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 118 + modifier );
    } else if (fKeyLabel.compare( "F7" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 119 + modifier );
    } else if (fKeyLabel.compare( "F8" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 120 + modifier );
    } else if (fKeyLabel.compare( "F9" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 121 + modifier );
    } else if (fKeyLabel.compare( "F10" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 122 + modifier );
    } else if (fKeyLabel.compare( "F11" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 123 + modifier );
    } else if (fKeyLabel.compare( "F12" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 124 + modifier );
    } else if (fKeyLabel.compare( "F13" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 125 + modifier );
    } else if (fKeyLabel.compare( "F14" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 126 + modifier );
    } else if (fKeyLabel.compare( "F15" ) == 0) {
        emit keySignal( 30 );
        emit keySignal( 112 + modifier );
    } else if (fKeyLabel.compare( "Er Pg" ) == 0) {
        emit keySignal( 12 );
    } else if (fKeyLabel.compare( "CR" ) == 0) {
        emit keySignal( 13 );
    } else if (fKeyLabel.compare( "ErEOL" ) == 0) {
        emit keySignal( 11 );
    } else if (fKeyLabel.compare( "LocPr" ) == 0) {
        emit localPrintRequest(); // signal that a print is requested
    } else if (fKeyLabel.compare( "Hold" ) == 0) {
        if (status->holding) {
            status->holding = false;
            emit( stopHoldingSignal() );
        } else {
            status->holding = true;
            emit( startHoldingSignal() );
        }
    } else {
        qDebug() << "KeyboardHandler: Warning - Unknown key event received " << fKeyLabel;
    }

    myParent->setFocus();
}

bool KeyBoardHandler::eventFilter( QObject *, QEvent *event ) {
    // qDebug() << "Some event caught at " << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:zzz");
    if (event->type() == QEvent::KeyRelease) {
        // qDebug() << "KeyRelease trapped";
        keyReleased( event );
        return true;
    } else if (event->type() == QEvent::KeyPress) {
        // keyPressed( event );
        return true;
    }
    return false;  // let the system handle other events
}

/***
 * For Dasher compatibility some keys need to be remapped to send DG rather than
 * ANSI-standard codes.  Also, the Function keys are trapped here and passed on
 * the the fKeyEventHandler.
 *
 ***/
void KeyBoardHandler::keyReleased( QEvent *event ) {

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    switch( keyEvent->key() ) {
    case Qt::Key_Down:
        if (keyEvent->modifiers() & Qt::ShiftModifier) emit keySignal( 30 );
        emit keySignal( 26 );
        break;
    case Qt::Key_End:
        emit keySignal( 30 );	// Shift C3 on Dasher
        emit keySignal( 90 );
        break;
    case Qt::Key_Home:
        emit keySignal( 30 );	// Shift C1 on Dasher
        emit keySignal( 88 );
        break;
    case Qt::Key_Left:
        if (keyEvent->modifiers() & Qt::ShiftModifier) emit keySignal( 30 );
        emit keySignal( 25 );
        break;
    case Qt::Key_PageDown:
        emit keySignal( 30 );	// Shift C4 on Dasher
        emit keySignal( 91 );
        break;
    case Qt::Key_PageUp:
        emit keySignal( 30 );	// Shift C2 on Dasher
        emit keySignal( 89 );
        break;
    case Qt::Key_Right:
        if (keyEvent->modifiers() & Qt::ShiftModifier) emit keySignal( 30 );
        emit keySignal( 24 );
        break;
    case Qt::Key_Up:
        if (keyEvent->modifiers() & Qt::ShiftModifier) emit keySignal( 30 );
        emit keySignal( 23 );
        break;
    case Qt::Key_F1:
        fKeyEventHandler( "F1" );
        break;
    case Qt::Key_F2:
        fKeyEventHandler( "F2" );
        break;
    case Qt::Key_F3:
        fKeyEventHandler( "F3" );
        break;
    case Qt::Key_F4:
        fKeyEventHandler( "F4" );
        break;
    case Qt::Key_F5:
        fKeyEventHandler( "F5" );
        break;
    case Qt::Key_F6:
        fKeyEventHandler( "F6" );
        break;
    case Qt::Key_F7:
        fKeyEventHandler( "F7" );
        break;
    case Qt::Key_F8:
        fKeyEventHandler( "F8" );
        break;
    case Qt::Key_F9:
        fKeyEventHandler( "F9" );
        break;
    case Qt::Key_F10:
        fKeyEventHandler( "F10" );
        break;
    case Qt::Key_F11:
        fKeyEventHandler( "F11" );
        break;
    case Qt::Key_F12:
        fKeyEventHandler( "F12" );
        break;

        // modifiers
        // swallow these...
    case Qt::Key_CapsLock:
    case Qt::Key_Control:
    case Qt::Key_NumLock:
    case Qt::Key_Shift:
        return;
        break;
    default:
        unsigned char ch = keyEvent->text().toLatin1()[0];
        emit keySignal( ch );
        break;
    }

}
