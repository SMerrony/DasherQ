#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>

#include "networkconnectdialog.h"

NetworkConnectDialog::NetworkConnectDialog( QSettings *settings, QWidget *parent ) :
    QDialog(parent)
{
    QFormLayout *fLayout = new QFormLayout;
    hostLineEdit = new QLineEdit;
    if (settings->contains( LAST_HOST_SETTING )) {
        hostLineEdit->setText( settings->value( LAST_HOST_SETTING ).toString() );
    } else {
        hostLineEdit->setText( "localhost" );
    }

    fLayout->addRow( "Host:", hostLineEdit );

    portLineEdit = new QLineEdit;
    if (settings->contains( LAST_PORT_SETTING )) {
        portLineEdit->setText( settings->value( LAST_PORT_SETTING ).toString() );
    } else {
        portLineEdit->setText( "23" );
    }
    fLayout->addRow( "Port:", portLineEdit );

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    fLayout->addWidget( buttonBox );

    setLayout( fLayout );

}
