#include <QDebug>

#include <QDesktopServices>
#include <QFileDialog>
#include <QIcon>
#include <QImage>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QSizePolicy>
#include <QStatusBar>
#include <QThread>
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

//    scrollArea = new QScrollArea();
//    scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
//    scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    crt = new Crt( this, terminal );
    crt->setMinimumSize( terminal->visible_cols * Crt::CHAR_WIDTH, terminal->visible_lines * Crt::CHAR_HEIGHT * crt->zoom ); // was 800x576

    //    scrollArea->setWidget( crt );
    //    setCentralWidget( scrollArea );
    //    scrollArea->verticalScrollBar()->setValue( scrollArea->verticalScrollBar()->maximum() );
    //    // lock size of CRT widget
    //    int sbWidth = style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;
    //    scrollArea->setFixedSize( (terminal->visible_cols * Crt::CHAR_WIDTH) + sbWidth, terminal->visible_lines * Crt::CHAR_HEIGHT * 1.750 );

    setCentralWidget( crt );

    // install our keyboard handler
    keyHandler = new KeyBoardHandler( this, status );
    crt->installEventFilter( keyHandler );
    this->setFocusProxy( crt );             // pass focus to crt if mainwindow gets it
    crt->setFocusPolicy( Qt::ClickFocus );

    fKeyMatrix = new FkeyMatrix( this, keyHandler );
    fKeyMatrix->setFeatures( 0 );
    addDockWidget( Qt::TopDockWidgetArea, fKeyMatrix);

    setupMenuBar();

    // start in local mode
    connect( this, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
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

void MainWindow::resize() {

    ChangeSizeDialog *d = new ChangeSizeDialog( this );
    if (d->exec()) {
        int newLines  = d->linesComboBox->currentData().toInt();
        int newCols   = d->colsComboBox->currentData().toInt();
        float newZoom = d->scaleComboBox->currentData().toFloat();
        terminal->resize( newLines, newCols );
        //crt->rowOffset = Terminal::TOTAL_LINES - terminal->visible_lines;
        crt->zoom = newZoom;
        crt->setMinimumSize( terminal->visible_cols * Crt::CHAR_WIDTH,
                             terminal->visible_lines * Crt::CHAR_HEIGHT * newZoom ); // was 800x576
        // lock size of CRT widget
//        int sbWidth = style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;
//        scrollArea->setFixedSize( (terminal->visible_cols * Crt::CHAR_WIDTH) + sbWidth, terminal->visible_lines * Crt::CHAR_HEIGHT * newZoom );
        this->adjustSize();
//        scrollArea->verticalScrollBar()->setValue( scrollArea->verticalScrollBar()->maximum() );
    }
    delete d;
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
            disconnect( this, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
            disconnect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
            disconnect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );

            connect( this, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
            connect( terminal, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
            connect( keyHandler, SIGNAL(keySignal(char)), serialConnection, SLOT(writeCharSerial(char)) );
            // break handling is 'special', and they only originate from the keyboard...
            connect( keyHandler, SIGNAL(breakSignal()), serialConnection, SLOT(sendBreak()) );

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
    disconnect( keyHandler, SIGNAL(breakSignal()), serialConnection, SLOT(sendBreak()) );
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
                        "&copy; 2013-2015 Steve Merrony<br><br>"
                        "Version 0.9<br><br>"
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
    menu->addAction( "Send File", this, SLOT(sendFile()));
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

    resizeAction = menu->addAction( "Resize" );
    connect( resizeAction, SIGNAL( triggered() ), this, SLOT( resize() ) );
    action = menu->addSeparator();

    selfTestAction = menu->addAction( "Self-Test" );
    connect( selfTestAction, SIGNAL( triggered() ), this, SLOT( selfTest() ) );

    action = menu->addSeparator();

    loadTemplateAction = menu->addAction( "Load Template" );
    connect( loadTemplateAction, SIGNAL( triggered() ), fKeyMatrix, SLOT( loadTemplate() ) );

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

    emulationStatusLabel->setText( 'D' + QString::number( status->emulation ) +
                                   " (" + QString::number( status->visLines ) +
                                   'x' + QString::number( status->visCols ) +
                                   ')' );

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

/* slot to send a local file to the host */
void MainWindow::sendFile() {

    QString fileName = QFileDialog::getOpenFileName( this, "Choose File to Send" );

    if (fileName.isEmpty())
        return;
    else {
        QFile file( fileName );
        if (!file.open( QIODevice::ReadOnly )) {
            QMessageBox::information( this, "Unable to open file", file.errorString() );
            return;
        }
        QByteArray blob = file.readAll();
        for (int i = 0; i < blob.size(); i++) {
            char this_byte = blob.at( i );

            emit keySignal( this_byte );
        }
        // send EOF marker - we will use two letter Zs which would not otherwise appear
        emit keySignal( 4 );
        file.close();
    }

}
