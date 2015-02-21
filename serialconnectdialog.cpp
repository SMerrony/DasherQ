#include <QDialogButtonBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "serialconnectdialog.h"

SerialConnectDialog::SerialConnectDialog(QWidget *parent) :
    QDialog(parent)
{
    QFormLayout *fLayout = new QFormLayout;

    portsComboBox = new QComboBox;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts() ) {
        portsComboBox->addItem( info.portName() );
    }

    fLayout->addRow( "Port:", portsComboBox );

    baudComboBox = new QComboBox;
    baudComboBox->addItem( "1200", QSerialPort::Baud1200 );
    baudComboBox->addItem( "2400", QSerialPort::Baud2400 );
    baudComboBox->addItem( "4800", QSerialPort::Baud4800 );
    baudComboBox->addItem( "9600", QSerialPort::Baud9600 );
    baudComboBox->addItem( "19200", QSerialPort::Baud19200 );
    baudComboBox->setCurrentIndex( 3 );
    fLayout->addRow( "Baud Rate:", baudComboBox );

    dataComboBox = new QComboBox;
    dataComboBox->addItem( "7", QSerialPort::Data7 );
    dataComboBox->addItem( "8", QSerialPort::Data8 );
    dataComboBox->setCurrentIndex( 1 );
    fLayout->addRow( "Data bits:", dataComboBox );

    parityComboBox = new QComboBox;
    parityComboBox->addItem( "None", QSerialPort::NoParity );
    parityComboBox->addItem( "Odd", QSerialPort::OddParity );
    parityComboBox->addItem( "Even", QSerialPort::EvenParity );
    parityComboBox->addItem( "Mark", QSerialPort::MarkParity );
    parityComboBox->addItem( "Space", QSerialPort::SpaceParity );
    fLayout->addRow( "Parity:", parityComboBox );

    stopComboBox = new QComboBox;
    stopComboBox->addItem( "1", QSerialPort::OneStop );
    stopComboBox->addItem( "1.5", QSerialPort::OneAndHalfStop );
    stopComboBox->addItem( "2", QSerialPort::TwoStop );
    fLayout->addRow( "Stop bits:", stopComboBox );

    flowComboBox = new QComboBox;
    flowComboBox->addItem( "None", QSerialPort::NoFlowControl );
    flowComboBox->addItem( "Hardware (RTS/CTS)", QSerialPort::HardwareControl );
    flowComboBox->addItem( "Software (XON/XOFF)", QSerialPort::SoftwareControl );
    fLayout->addRow( "Flow Control:", flowComboBox );

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    fLayout->addWidget( buttonBox );

    setLayout( fLayout );

}
