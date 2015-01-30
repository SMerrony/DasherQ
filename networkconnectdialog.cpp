#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

#include "networkconnectdialog.h"

NetworkConnectDialog::NetworkConnectDialog(QWidget *parent) :
    QDialog(parent)
{
    QFormLayout *fLayout = new QFormLayout;
    hostLineEdit = new QLineEdit;
    fLayout->addRow( "Host:", hostLineEdit );

    portLineEdit = new QLineEdit;
    portLineEdit->setText( "23" );
    fLayout->addRow( "Port:", portLineEdit );

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    fLayout->addWidget( buttonBox );

    setLayout( fLayout );

}
