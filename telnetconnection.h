#ifndef TELNETCONNECTION_H
#define TELNETCONNECTION_H

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

class TelnetConnection : public QObject
{
    Q_OBJECT
public:
    explicit TelnetConnection( QObject *pparent );
    bool connected;
    struct Settings {
        QString host;
        int     port;
    };
    Settings settings;

    static const unsigned char CMD_SE = 240;
    static const unsigned char CMD_NOP = 241;
    static const unsigned char CMD_DM = 242;
    static const unsigned char CMD_BRK = 243;
    static const unsigned char CMD_IP = 244;
    static const unsigned char CMD_AO = 245;
    static const unsigned char CMD_AYT = 246;
    static const unsigned char CMD_EC = 247;
    static const unsigned char CMD_EL = 248;
    static const unsigned char CMD_GA = 249;
    static const unsigned char CMD_SB = 250;
    static const unsigned char CMD_WILL = 251;
    static const unsigned char CMD_WONT = 252;
    static const unsigned char CMD_DO = 253;
    static const unsigned char CMD_DONT = 254;
    static const unsigned char CMD_IAC = 255;

    static const unsigned char OPT_BIN = 0;
    static const unsigned char OPT_ECHO = 1;
    static const unsigned char OPT_RECON = 2;
    static const unsigned char OPT_SGA = 3;
    static const unsigned char OPT_STATUS = 5;
    static const unsigned char OPT_COLS = 8;
    static const unsigned char OPT_ROWS = 9;
    static const unsigned char OPT_EASCII = 17;
    static const unsigned char OPT_LOGOUT = 18;
    static const unsigned char OPT_TTYPE = 24;
    static const unsigned char OPT_NAWS = 31; // window size
    static const unsigned char OPT_TSPEED = 32;
    static const unsigned char OPT_XDISP = 35;
    static const unsigned char OPT_NEWENV = 39;

    //static const char WILL_NAWS[] = {CMD_IAC, CMD_WILL, OPT_NAWS};
    //static const unsigned char WILL_TSPEED[] = {CMD_IAC, CMD_WILL, OPT_TSPEED};
    //static const unsigned char WILL_TTYPE[] = {CMD_IAC, CMD_WILL, OPT_TTYPE};
    //static const unsigned char DO_ECHO[] = {CMD_IAC, CMD_DO, OPT_ECHO};



signals:
    void hostDataSignal( QByteArray );
    void telnetClosed();
    
public slots:
    bool openTelnetConnection( QString hostName, int portNum );
    void readTelnetData();
    void writeCharTelnet( char );
    void closeTelnetConnection();
    void handleTelnetError( QAbstractSocket::SocketError );

private:
    QObject *parent;
    QTcpSocket *tcpSocket;
};

#endif // TELNETCONNECTION_H
