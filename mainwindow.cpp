#include <QDebug>

#include <QDesktopServices>
#include <QFileDialog>
#include <QIcon>
#include <QImage>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include <QUrl>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    setWindowTitle( "DasherQ" );

    QIcon icon( ":/icons/DGlogoOrange.png" );
    setWindowIcon( icon );

    fromHostQ = new QQueue<unsigned char>();

    status = new Status();

    terminal = new Terminal( status );

    crt = new Crt( this, terminal );
    crt->setMinimumSize( 800, 576 ); // was 800x576

    setCentralWidget( crt );

    setupMenuBar();

    // install our keyboard handler
    keyHandler = new KeyBoardHandler( this, status );
    crt->installEventFilter( keyHandler );
    // crt->grabKeyboard();
    crt->setFocusPolicy( Qt::ClickFocus );

    setupToolbar();

    // start in local mode
    connect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
    connect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );

    // refresh the "crt" regularly
    QTimer *updateCrtTimer = new QTimer( this );
    connect( updateCrtTimer, SIGNAL( timeout() ), this, SLOT( updateCrtIfDirty() ) );
    updateCrtTimer->start( 34 ); // 17ms ~ 60Hz

    // alternate the blink state every half-second
    QTimer *blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ), this, SLOT( updateBlinkState() ) );
    blinkTimer->start( 500 );

    // status bar
    setupStatusBar();

    connect( keyHandler, SIGNAL(localPrintRequest()), this, SLOT(localPrintRequest()) );
    connect( keyHandler, SIGNAL(startHoldingSignal()), terminal, SLOT(startBuffering()) );
    connect( keyHandler, SIGNAL(stopHoldingSignal()), terminal, SLOT(stopBuffering()) );

}

void MainWindow::updateCrtIfDirty() {
    if (status->dirty) {
        crt->update();
        status->dirty = false;
    }
}

void MainWindow::updateBlinkState() {

    terminal->blinkState = ! terminal->blinkState;
    crt->update();
}

void MainWindow::toggleLogging() {
    if (status->logging) {
        // stop logging
        disconnect( terminal, SIGNAL(logCharSignal(char)), this, SLOT(logChar(char)) );
        // loggingStream.unsetDevice();
        loggingFile.close();
        loggingAction->setChecked( false );
        status->logging = false;
    } else {
        // start logging
        QString logfileNameString = QFileDialog::getSaveFileName( this, "DasherQ Logfile", NULL, "Logfiles (*.log *.txt)" );
        if (logfileNameString != NULL) {
            loggingFile.setFileName( logfileNameString );
            if (!loggingFile.open( QIODevice::Append )) {
                QMessageBox::warning( this, "Warning", "Unable to open chosen log file" );
            } else {
                loggingStream.setDevice( &loggingFile );
                connect( terminal, SIGNAL(logCharSignal(char)), this, SLOT(logChar(char)) );
                loggingAction->setChecked( true );
                status->logging = true;
            }
        }

    }
}

MainWindow::~MainWindow() {
}

void MainWindow::setD200emulation() {
    status->emulation = Status::D200;
}

void MainWindow::setD210emulation() {
    status->emulation = Status::D210;
}

void MainWindow::selfTest() {
    terminal->selfTest();
}

void MainWindow::openSerialPort() {

    SerialConnectDialog *d = new SerialConnectDialog( this );
    if (d->exec()) {
        qDebug() << "Chosen port: " << d->portsComboBox->currentText();
        serialConnection = new SerialConnection( this );
        if (serialConnection->openSerialPort(
                    d->portsComboBox->currentText(),
                    static_cast<QSerialPort::BaudRate>(d->baudComboBox->currentData().toInt()),
                    static_cast<QSerialPort::DataBits>(d->dataComboBox->currentData().toInt()),
                    static_cast<QSerialPort::Parity>(d->parityComboBox->currentData().toInt()),
                    static_cast<QSerialPort::StopBits>(d->stopComboBox->currentData().toInt()),
                    static_cast<QSerialPort::FlowControl>(d->flowComboBox->currentData().toInt())
                                             )
            ){
            // stop local echoing
            disconnect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
            disconnect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );

            connect( terminal, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
            connect( keyHandler, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
            connect( serialConnection, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
            openSerialAction->setEnabled( false );
            closeSerialAction->setEnabled( true );
            selfTestAction->setEnabled( false );
            networkMenu->setEnabled( false );
            status->connection = Status::SERIAL_CONNECTED;
            status->serialPort = d->portsComboBox->currentText();

        } else {
            QMessageBox::critical( this, "Error", "Unable to open Serial Port" );
        }
    }
    delete d;
}

void MainWindow::closeSerialPort() {

    serialConnection->closeSerialPort();
    disconnect( keyHandler, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
    disconnect( serialConnection, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
    disconnect( terminal, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
    delete serialConnection;
    status->connection = Status::DISCONNECTED;
    status->serialPort = "";
    openSerialAction->setEnabled( true );
    closeSerialAction->setEnabled( false );
    selfTestAction->setEnabled( true );
    networkMenu->setEnabled( true );
    // resume local echoing
    connect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
    connect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
    QMessageBox::information( this, "DasherQ", "Connection Terminated" );
}

void MainWindow::openNetworkPort() {

    NetworkConnectDialog *d = new NetworkConnectDialog( this );
    crt->releaseKeyboard();
    if (d->exec()) {
        telnetConnection = new TelnetConnection( this );
        if (telnetConnection->openTelnetConnection( d->hostLineEdit->text(),
                                                    d->portLineEdit->text().toInt() )) {
            // stop local echoing
            disconnect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
            disconnect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );

            connect( terminal, SIGNAL(keySignal(char)), telnetConnection, SLOT(writeCharTelnet(char)) );
            connect( keyHandler, SIGNAL(keySignal(char)), telnetConnection, SLOT(writeCharTelnet(char)) );
            connect( telnetConnection, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
            openNetworkAction->setEnabled( false );
            closeNetworkAction->setEnabled( true );
            selfTestAction->setEnabled( false );
            serialMenu->setEnabled( false );
            status->connection = Status::TELNET_CONNECTED;
            status->remoteHost = d->hostLineEdit->text();
        }
    } else {
        QMessageBox::critical( this, "Error", "Unable to connect to telnet server");
    }
    delete d;
    crt->grabKeyboard();
}

void MainWindow::closeNetworkPort() {

    telnetConnection->closeTelnetConnection();
    disconnect( keyHandler, SIGNAL(keySignal(char)), telnetConnection, SLOT(writeCharTelnet(char)) );
    disconnect( terminal, SIGNAL(keySignal(char)), telnetConnection, SLOT(writeCharTelnet(char)) );
    disconnect( telnetConnection, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
    telnetConnection->deleteLater();
    status->connection = Status::DISCONNECTED;
    status->remoteHost = "";
    openNetworkAction->setEnabled( true );
    closeNetworkAction->setEnabled( false );
    serialMenu->setEnabled( true );
    selfTestAction->setEnabled( true );
    // resume local echoing
    connect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
    connect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
    QMessageBox::information( this, "DasherQ", "Host Disconnected" );
}

void MainWindow::showExternalHelp() {
    QDesktopServices::openUrl( QUrl( HELP_URL ) );
}

void MainWindow::showAboutDialog() {
    QMessageBox::about( this,
                        "About DasherQ",
                        "<center><b>DasherQ</b><br<br>"
                        "&copy; 2015 Steve Merrony<br><br>"
                        "Version 0.8<br><br>"
                        "Please see<br>"
                        "<a href='http://www.stephenmerrony.co.uk/dg/'>http://www.stephenmerrony.co.uk/dg/</a><br>"
                        "for more information</center>" );
}

void MainWindow::setupMenuBar() {

    QMenu *menu;
    QAction *action;
    QActionGroup *emulationGroup = new QActionGroup( this );

    menu = menuBar()->addMenu( "&File" );
    loggingAction = menu->addAction( "Logging" );
    loggingAction->setCheckable( true );
    loggingAction->setChecked( false );
    connect( loggingAction, SIGNAL( triggered() ), this, SLOT( toggleLogging() ) );
    action = menu->addSeparator();
    menu->addAction( "&Quit", this, SLOT(close()));

    menu = menuBar()->addMenu( "Emulation" );

    action = menu->addAction( "D200" );
    action->setCheckable( true );
    action->setChecked( true );
    connect( action, SIGNAL( triggered() ), this, SLOT( setD200emulation() ) );
    action->setActionGroup( emulationGroup );

    action = menu->addAction( "D210" );
    action->setCheckable( true );
    action->setChecked( false );
    connect( action, SIGNAL( triggered() ), this, SLOT( setD210emulation() ) );
    action->setActionGroup( emulationGroup );

    action = menu->addSeparator();

    selfTestAction = menu->addAction( "Self-Test" );
    connect( selfTestAction, SIGNAL( triggered() ), this, SLOT( selfTest() ) );

    serialMenu = menuBar()->addMenu( "&Serial" );
    openSerialAction = serialMenu->addAction( "&Connect" );
    connect( openSerialAction, SIGNAL( triggered() ), this, SLOT( openSerialPort() ) );
    closeSerialAction = serialMenu->addAction( "&Disconnect" );
    connect( closeSerialAction, SIGNAL( triggered() ), this, SLOT( closeSerialPort() ) );
    closeSerialAction->setEnabled( false );

    networkMenu = menuBar()->addMenu( "&Network" );
    openNetworkAction = networkMenu->addAction( "&Connect" );
    connect( openNetworkAction, SIGNAL( triggered() ), this, SLOT( openNetworkPort() ) );
    closeNetworkAction = networkMenu->addAction( "&Disconnect" );
    connect( closeNetworkAction, SIGNAL( triggered() ), this, SLOT( closeNetworkPort() ) );
    closeNetworkAction->setEnabled( false );

    menu = menuBar()->addMenu( "&Help" );
    action = menu->addAction( "Online &Help" );
    connect( action, SIGNAL( triggered() ), this, SLOT( showExternalHelp() ) );
    action = menu->addSeparator();
    action = menu->addAction( "&About" );
    connect( action, SIGNAL( triggered() ), this, SLOT( showAboutDialog() ) );

}

void MainWindow::setupStatusBar() {

    onlineStatusLabel = new QLabel( "Uninitialised", this );
    onlineStatusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addPermanentWidget( onlineStatusLabel, 1 );

    connectionStatusLabel = new QLabel( "", this );
    connectionStatusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addPermanentWidget( connectionStatusLabel, 1 );

    emulationStatusLabel = new QLabel( "", this );
    emulationStatusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addPermanentWidget( emulationStatusLabel, 1 );

    // update the statusBar independently every half-second
    QTimer *updateStatusTimer = new QTimer( this );
    connect( updateStatusTimer, SIGNAL( timeout() ), this, SLOT( updateStatusBar() ) );
    updateStatusTimer->start( 500 );
}

void MainWindow::updateStatusBar() {
    switch (status->connection) {
    case Status::DISCONNECTED:
        onlineStatusLabel->setText( "Offline" );
        connectionStatusLabel->setText( "" );
        break;
    case Status::SERIAL_CONNECTED:
        onlineStatusLabel->setText( "Online (Serial)" );
        connectionStatusLabel->setText( status->serialPort + " @ " +
                                        serialConnection->getCurrentBaudRate() + "Baud, " +
                                        serialConnection->getCurrentParity() + " Parity" );
        break;
    case Status::TELNET_CONNECTED:
        onlineStatusLabel->setText( "Online (Telnet)" );
        connectionStatusLabel->setText( status->remoteHost );
        break;
    }

    if (status->logging) {
        onlineStatusLabel->setText( onlineStatusLabel->text() + " Logging" );
    }

    if (status->holding) {
        onlineStatusLabel->setText( onlineStatusLabel->text() + " [Hold]");
    }

    emulationStatusLabel->setText( 'D' + QString::number( status->emulation ) );
}

void MainWindow::setupToolbar() {

    QToolBar *toolBar;
    QAction *action;

    toolBar = addToolBar( "Functions" );
    action = toolBar->addAction( "Brk" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    toolBar->addSeparator();
    action = toolBar->addAction( "F1" );
    action->setShortcut( Qt::Key_F1 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F2" );
    action->setShortcut( Qt::Key_F2 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F3" );
    action->setShortcut( Qt::Key_F3 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F4" );
    action->setShortcut( Qt::Key_F4 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F5" );
    action->setShortcut( Qt::Key_F5 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    toolBar->addSeparator();
    action = toolBar->addAction( "F6" );
    action->setShortcut( Qt::Key_F6 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F7" );
    action->setShortcut( Qt::Key_F7 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F8" );
    action->setShortcut( Qt::Key_F8 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F9" );
    action->setShortcut( Qt::Key_F9 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F10" );
    action->setShortcut( Qt::Key_F10 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    toolBar->addSeparator();
    action = toolBar->addAction( "F11" );
    action->setShortcut( Qt::Key_F12 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F12" );
    action->setShortcut( Qt::Key_F12 );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F13" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F14" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "F15" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    toolBar->addSeparator();
    action = toolBar->addAction( "Erase Page" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "CR" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "Erase EOL" );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    toolBar->addSeparator();
    action = toolBar->addAction( "Loc Pr" );
    action->setShortcut( Qt::Key_Print );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
    action = toolBar->addAction( "Hold" );
    action->setShortcut( Qt::Key_Pause );
    connect( action, SIGNAL( triggered() ), keyHandler, SLOT( fKeyEventHandler( ) ) );
}

void MainWindow::localPrintRequest() {

    QPrinter printer;

    QPrintDialog pDialog( &printer, this );
    pDialog.setWindowTitle( "Print Screen" );

    if (pDialog.exec() == QDialog::Accepted) {
        crt->print( &printer );
    } else {
        return;
    }
}

/* slot to act as a local echo-er */
void MainWindow::localEcho( char c ) {
    emit hostDataSignal( QByteArray( 1, c ) );
}

/* slot to log a character */
void MainWindow::logChar( char ch ) {
    loggingStream << ch;
}
