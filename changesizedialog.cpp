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

    linesComboBox = new QComboBox;
    linesComboBox->addItem( "24", 24 );
    linesComboBox->addItem( "25", 25 );
    linesComboBox->addItem( "36", 36 );
    linesComboBox->addItem( "48", 48 );
    linesComboBox->addItem( "66", 66 );  // Standard Line-Printer size

    fLayout->addRow( "Lines:", linesComboBox );

    scaleComboBox = new QComboBox;
    scaleComboBox->addItem( "Normal", 1.75f );
    scaleComboBox->addItem( "Smaller", 1.4f );
    scaleComboBox->addItem( "Tiny", 1.0f );

    fLayout->addRow( "Zoom:", scaleComboBox );

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    fLayout->addWidget( buttonBox );


    setLayout( fLayout );


}



