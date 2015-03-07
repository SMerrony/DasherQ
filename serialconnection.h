#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class SerialConnection : public QObject
{
    Q_OBJECT
public:
    explicit SerialConnection( QObject *parent );

    bool connected;
    struct Settings {
        QString name;
        QSerialPort::BaudRate baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    Settings settings;
    
signals:
    void hostDataSignal( QByteArray );
    
public slots:
    bool openSerialPort( QString portName,
                         QSerialPort::BaudRate baud,
                         QSerialPort::DataBits data,
                         QSerialPort::Parity parity,
                         QSerialPort::StopBits stop,
                         QSerialPort::FlowControl flow );
    void readSerialData();
    void writeCharSerial( char );
    void sendBreak();
    void closeSerialPort();
    void handleError(QSerialPort::SerialPortError error);
    QString getCurrentBaudRate();
    QString getCurrentParity();

private:
    QSerialPort *serialPort;
    
};

#endif // SERIALCONNECTION_H
