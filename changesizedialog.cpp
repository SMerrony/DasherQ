#include <QDialogButtonBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>

#include "changesizedialog.h"

ChangeSizeDialog::ChangeSizeDialog(QWidget *parent) : QDialog( parent )
{
    QFormLayout *fLayout = new QFormLayout;

    colsComboBox = new QComboBox;
    colsComboBox->addItem( "80", 80 );
    colsComboBox->addItem( "81", 81 );
    colsComboBox->addItem( "120", 120 );
    colsComboBox->addItem( "132", 132 );
    colsComboBox->addItem( "135", 135 );

    fLayout->addRow( "Columns:", colsComboBox );

    rowsComboBox = new QComboBox;
    rowsComboBox->addItem( "24", 24 );
    rowsComboBox->addItem( "25", 25 );
    rowsComboBox->addItem( "36", 36 );
//    rowsComboBox->addItem( "48", 48 );

    fLayout->addRow( "Columns:", rowsComboBox );

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    fLayout->addWidget( buttonBox );

    setLayout( fLayout );


}



