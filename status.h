#ifndef STATUS_H
#define STATUS_H

#include <QString>

class Status {

public:
    enum ConnectionType { DISCONNECTED, SERIAL_CONNECTED, TELNET_CONNECTED };
    enum EmulationType { D200 = 200,
                         D210 = 210
                       };
    QString serialPort, remoteHost;
    bool logging;
    bool control_pressed, shift_pressed, holding, dirty;
    ConnectionType connection;
    EmulationType emulation;

    Status();
};

#endif // STATUS_H
