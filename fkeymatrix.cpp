#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "fkeymatrix.h"

FkeyMatrix::FkeyMatrix(QWidget *parent, KeyBoardHandler *keyHandler) : QDockWidget( parent ) {

    QWidget *dockWidget = new QWidget();
    QGridLayout *layout = new QGridLayout();

    QPushButton *locPrButton = new QPushButton( "LocPr" );
    connect( locPrButton, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( locPrButton, 0, 0 );
    QPushButton *brkButton = new QPushButton( "Break" );
    connect( brkButton, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( brkButton, 4, 0 );

    QPushButton *f1Button = new QPushButton( "F1" );
    connect( f1Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f1Button, 4, 2 );
    QPushButton *f2Button = new QPushButton( "F2" );
    connect( f2Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f2Button, 4, 3 );
    QPushButton *f3Button = new QPushButton( "F3" );
    connect( f3Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f3Button, 4, 4 );
    QPushButton *f4Button = new QPushButton( "F4" );
    connect( f4Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f4Button, 4, 5 );
    QPushButton *f5Button = new QPushButton( "F5" );
    connect( f5Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f5Button, 4, 6 );

    QLabel *shiftLabel2 = new QLabel( "Shift" );
    QLabel *ctrlLabel2 = new QLabel( "Ctrl" );
    QLabel *ctrlShiftLabel2 = new QLabel( "Ctrl\nShift" );
    layout->addWidget( ctrlShiftLabel2, 0, 7 );
    layout->addWidget( ctrlLabel2,      1, 7 );
    layout->addWidget( shiftLabel2,     2, 7 );

    QPushButton *f6Button = new QPushButton( "F6" );
    connect( f6Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f6Button, 4, 8 );
    QPushButton *f7Button = new QPushButton( "F7" );
    connect( f7Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f7Button, 4, 9 );
    QPushButton *f8Button = new QPushButton( "F8" );
    connect( f8Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f8Button, 4, 10 );
    QPushButton *f9Button = new QPushButton( "F9" );
    connect( f9Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f9Button, 4, 11 );
    QPushButton *f10Button = new QPushButton( "F10" );
    connect( f10Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f10Button, 4, 12 );

    QLabel *shiftLabel3 = new QLabel( "Shift" );
    QLabel *ctrlLabel3 = new QLabel( "Ctrl" );
    QLabel *ctrlShiftLabel3 = new QLabel( "Ctrl\nShift" );
    layout->addWidget( ctrlShiftLabel3, 0, 13 );
    layout->addWidget( ctrlLabel3,      1, 13 );
    layout->addWidget( shiftLabel3,     2, 13 );

    QPushButton *f11Button = new QPushButton( "F11" );
    connect( f11Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f11Button, 4, 14 );
    QPushButton *f12Button = new QPushButton( "F12" );
    connect( f12Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f12Button, 4, 15 );
    QPushButton *f13Button = new QPushButton( "F13" );
    connect( f13Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f13Button, 4, 16 );
    QPushButton *f14Button = new QPushButton( "F14" );
    connect( f14Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f14Button, 4, 17 );
    QPushButton *f15Button = new QPushButton( "F15" );
    connect( f15Button, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( f15Button, 4, 18 );

    QPushButton *holdButton = new QPushButton( "Hold" );
    connect( holdButton, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( holdButton, 0, 19 );
    QPushButton *erPageButton = new QPushButton( "Er Pg" );
    connect( erPageButton, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( erPageButton, 1, 19 );
    QPushButton *crButton = new QPushButton( "CR" );
    connect( crButton, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( crButton, 2, 19 );
    QPushButton *erEOLButton = new QPushButton( "ErEOL" );
    connect( erEOLButton, SIGNAL(released()), keyHandler, SLOT( fKeyEventHandler()));
    layout->addWidget( erEOLButton, 3, 19 );

    layout->setMargin( 1 ); // minimize space around grid
    layout->setSpacing( 1 );

    this->setStyleSheet( "QPushButton { max-width: 35px }"
                         "QLabel { max-width: 45px; qproperty-alignment: AlignCenter; }" );

    dockWidget->setLayout( layout );
    setTitleBarWidget( dockWidget );  // Qt5 - cannot remove titlebar, so fill it with our contents to conceal it
}

FkeyMatrix::~FkeyMatrix()
{

}

