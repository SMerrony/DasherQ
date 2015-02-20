#include <QDebug>

#include "serialconnection.h"

SerialConnection::SerialConnection( QObject *parent ) : QObject(parent) {

    serialPort = new QSerialPort( this );
    connected = false;
}

/***
 * openSerialPort - attempt to open asynch port with all given parameters
 *
 ***/
bool SerialConnection::openSerialPort( QString portName,
                                       QSerialPort::BaudRate baud,
                                       QSerialPort::DataBits data,
                                       QSerialPort::Parity parity,
                                       QSerialPort::StopBits stop,
                                       QSerialPort::FlowControl flow ) {
    serialPort->setPortName( portName );
    if (serialPort->open( QIODevice::ReadWrite )) {
        if ( serialPort->setBaudRate( baud ) &&
             serialPort->setDataBits( data ) &&
             serialPort->setFlowControl( flow ) &&
             serialPort->setParity( parity ) &&
             serialPort->setStopBits( stop )  &&
             serialPort->setRequestToSend( true )
           ) {

            connect( serialPort, SIGNAL( error( QSerialPort::SerialPortError )),
                     this, SLOT( handleError( QSerialPort::SerialPortError ) ) );
            connect( serialPort, SIGNAL( readyRead() ), this, SLOT( readSerialData() ) );

            connected = true;
        } else {
            qDebug() << "Error setting serial port parameters " << serialPort->errorString();
            return false;
        }

    } else {
        qDebug() << "Error opening serial port: " << serialPort->errorString();
        return false;
    }
    return true;
}

// slot to read data from serial port and signal it all onwards
void SerialConnection::readSerialData() {

    // qDebug() << "SerialConnection: readSerialData invoked";
    if (serialPort->bytesAvailable() > 0) {
        QByteArray data = serialPort->readAll();
        emit hostDataSignal( data );
    }
}

// slot to write char to serial port
void SerialConnection::writeCharSerial( char b ) {

    if (b == 2) {
        serialPort->sendBreak( 110 );
    } else {
        serialPort->write( QByteArray( 1, b ) );
    }
}

void SerialConnection::closeSerialPort() {

    serialPort->close();
    connected = false;
}

void SerialConnection::handleError(QSerialPort::SerialPortError error) {
    qDebug() << "Serial I/O Error: " << error << " : " << serialPort->errorString();
}

QString SerialConnection::getCurrentBaudRate() {
    QString cbr = "(Unset)";
    cbr = QString::number(serialPort->baudRate());
    return cbr;
}

QString SerialConnection::getCurrentParity() {
    QString cp = "(Unset)";
    int p = serialPort->parity();
    switch(p) {
    case QSerialPort::NoParity: cp = "No"; break;
    case QSerialPort::EvenParity: cp = "Even"; break;
    case QSerialPort::OddParity: cp = "Odd"; break;
    case QSerialPort::SpaceParity: cp = "Space"; break;
    case QSerialPort::MarkParity: cp = "Mark"; break;
    }

    return cp;
}
