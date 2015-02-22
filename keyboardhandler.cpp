#include <QDebug>
#include <QKeyEvent>

#include "keyboardhandler.h"

KeyBoardHandler::KeyBoardHandler( QObject *parent, Status *pStatus )
    : QObject( parent ) {
    myParent = (QWidget *)parent;
    status = pStatus;
    qDebug() << "KeyboardHandler created";
}

void KeyBoardHandler::fKeyEventHandler() {

    unsigned char modifier = 0;

    if (status->control_pressed && status->shift_pressed) { modifier = -80; }  // Ctrl-Shift
    if (status->control_pressed && !status->shift_pressed) { modifier = -64; } // Ctrl
    if (!status->control_pressed && status->shift_pressed) { modifier = -16; } // Shift

    QString fKeyLabel = sender()->property("text").toString();

    // qDebug() << "KeyboardHandler received fKeyEvent " << fKeyLabel;

    if (fKeyLabel.compare( "Break" ) == 0) {
        qDebug() << "Break requested by user";
        emit keySignal( 0 ); // special CMD_BREAK indicator (trapped by connection handler)
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

bool KeyBoardHandler::eventFilter( QObject *obj, QEvent *event ) {
    // qDebug() << "Some event caught at " << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:zzz");
    if (event->type() == QEvent::KeyRelease) {
        // qDebug() << "KeyRelease trapped";
        keyReleased( event );
        return true;
    } else if (event->type() == QEvent::KeyPress) {
        keyPressed( event );
        return true;
    }
    return false;  // let the system handle other events
}

void KeyBoardHandler::keyPressed( QEvent *event ) {

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    switch( keyEvent->key() ) {
    case Qt::ControlModifier:
        status->control_pressed = true;
        return;
        break;
    case Qt::ShiftModifier:
        status->shift_pressed = true;
        return;
        break;
    }
}

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
        // modifiers
    case Qt::Key_Control:
        status->control_pressed = false;
        return;
        break;
    case Qt::Key_Shift:
        status->shift_pressed = false;
        return;
        break;
    default:
        unsigned char ch = keyEvent->text().toLatin1()[0];
        emit keySignal( ch );
        break;
    }

}
