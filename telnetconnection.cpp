#include <QMessageBox>
#include <QTimer>

#include "telnetconnection.h"

TelnetConnection::TelnetConnection( QObject *pparent ) :
    QObject(pparent) {
    parent = pparent;
    tcpSocket = new QTcpSocket();
    connected = false;
}

bool TelnetConnection::openTelnetConnection( QString hostName, int portNum ) {

    tcpSocket->connectToHost( hostName, portNum );
    if (tcpSocket->waitForConnected( 10000 )) {
        connect( tcpSocket, SIGNAL( readyRead() ), this, SLOT( readTelnetData() ) );
        connect( tcpSocket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( handleTelnetError( QAbstractSocket::SocketError ) ) );
        connect( this, SIGNAL( telnetClosed() ), parent, SLOT( closeNetworkPort() ) );
        host = hostName; port = portNum;
        connected = true;

    } else {
        return false;
    }
    return true;
}

void TelnetConnection::readTelnetData() {

    qint64 bytesReady = tcpSocket->bytesAvailable();

    if (bytesReady > 0) {
        // qDebug() << "TelnetConnection received " << bytesReady << " bytes ready from host";
        QByteArray data = tcpSocket->read( bytesReady );
        emit hostDataSignal( data );
    }
}

void TelnetConnection::writeCharTelnet( char b ) {

    tcpSocket->write( QByteArray( 1, b ) );
}

void TelnetConnection::closeTelnetConnection() {

    if (tcpSocket->isValid()) {
        tcpSocket->abort();
        tcpSocket->deleteLater();
    }

}

bool TelnetConnection::restartTelnetConnection() {
    if (!connected) return false;
    closeTelnetConnection();
    return openTelnetConnection( host, port );
}

void TelnetConnection::handleTelnetError( QAbstractSocket::SocketError socketError ) {

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "TelnetConnection: Remote host closed connection";
        //closeTelnetConnection();
        emit telnetClosed();
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "The connection was refused by the peer. Please check the host name and port settings.";
        break;
    default:
        qDebug() << "The following error occurred: " << tcpSocket->errorString();
    }


}
