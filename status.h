#ifndef STATUS_H
#define STATUS_H

#include <QString>

class Status {

public:
    enum ConnectionType { DISCONNECTED, SERIAL_CONNECTED, TELNET_CONNECTED };
    enum EmulationType { D200 = 200,
                         D210 = 210,
                         D211 = 211
                       };
    int visLines, visCols;
    QString serialPort, remoteHost, remotePort;
    bool logging;
    bool holding, dirty;
    ConnectionType connection;
    EmulationType emulation;

    Status();
};

#endif // STATUS_H
