/*
 * Copyright (C) 2013-2017 Stephen Merrony
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QDebug>

#include <QDesktopServices>
#include <QFileDialog>
#include <QFontDatabase>
#include <QIcon>
#include <QImage>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QSizePolicy>
#include <QStatusBar>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#include "mainwindow.h"

MainWindow::MainWindow(QString hostArg, QWidget *parent) : QMainWindow(parent) {

    setWindowTitle( APP_NAME );

    QIcon icon( ":/icons/DGlogoOrange.png" );
    setWindowIcon( icon );

    fromHostQ = new QQueue<unsigned char>();

    status = new Status();
    settings = new QSettings( ORG_NAME, APP_NAME );
    history = new History();

    terminal = new Terminal( status, history );

    crt = new Crt( this, terminal );
    crt->setMinimumSize( terminal->visible_cols * Crt::CHAR_WIDTH, terminal->visible_lines * Crt::CHAR_HEIGHT * crt->zoom ); // was 800x576

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
    updateCrtTimer->start( CRT_REFRESH_MS ); // 17ms ~ 60Hz

    // alternate the blink state every half-second
    QTimer *blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ), this, SLOT( updateBlinkState() ) );
    blinkTimer->start( BLINK_TIMER_MS );

    // status bar
    setupStatusBar();


    connect( keyHandler, SIGNAL(localPrintRequest()), this, SLOT(localPrintRequest()) );
    connect( keyHandler, SIGNAL(startHoldingSignal()), terminal, SLOT(startBuffering()) );
    connect( keyHandler, SIGNAL(stopHoldingSignal()), terminal, SLOT(stopBuffering()) );

    if (hostArg.length() > 0) {

        QStringList hl = hostArg.split( ':' );
        qDebug()  << "Will attempt to connect to " + hl.at(0) + " port " +hl.at(1);
        openRemote( hl.at(0), hl.at(1) );
    } else {
        qDebug() << "No remote host specified";
    }

}

void MainWindow::focusInEvent( QFocusEvent * ) {
    crt->installEventFilter( keyHandler );
}

void MainWindow::focusOutEvent( QFocusEvent * ) {
    crt->removeEventFilter( keyHandler );
}

void MainWindow::updateCrtIfDirty() {
    if (status->dirty) {
        crt->update();
        status->dirty = false;
    }
}

void MainWindow::updateBlinkState() {
    terminal->blinkState = ! terminal->blinkState;
    status->dirty = true;
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

void MainWindow::viewHistory() {
    QDialog *historyDialog = new QDialog( this );
    historyDialog->setWindowTitle( "DasherQ Terminal History" );
    historyDialog->setMinimumWidth( 600 );
    historyDialog->setMinimumHeight( 400 );
    QVBoxLayout *dLayout = new QVBoxLayout( historyDialog );
    QTextEdit *historyArea = new QTextEdit();
    historyArea->setPlainText( history->fetchAllAsQString() );
    historyArea->setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont) );
    historyArea->setReadOnly( true );
    dLayout->addWidget( historyArea );
    historyDialog->exec();
    delete historyDialog;
}

void MainWindow::setD210emulation() {
    status->emulation = Status::D210;
}

void MainWindow::setD211emulation() {
    status->emulation = Status::D211;
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

void MainWindow::openRemote( QString host, QString port) {
    telnetConnection = new TelnetConnection( this );
    if (telnetConnection->openTelnetConnection( host,
                                                port.toInt() )) {
        // stop local echoing
        disconnect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
        disconnect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );

        connect( terminal, SIGNAL(keySignal(char)), telnetConnection, SLOT(writeCharTelnet(char)) );
        connect( keyHandler, SIGNAL(keySignal(char)), telnetConnection, SLOT(writeCharTelnet(char)) );
        connect( telnetConnection, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
        openNetworkAction->setEnabled( false );
        closeNetworkAction->setEnabled( true );
        restartNetworkAction->setEnabled( true );
        selfTestAction->setEnabled( false );
        serialMenu->setEnabled( false );
        status->connection = Status::TELNET_CONNECTED;
        status->remoteHost = host;
        status->remotePort = port;
        settings->setValue( LAST_HOST_SETTING, host );
        settings->setValue( LAST_PORT_SETTING, port );
    } else {
        QMessageBox::critical( this, "Error", "Unable to connect to telnet server");
    }
}

void MainWindow::openNetworkPort() {

    NetworkConnectDialog *d = new NetworkConnectDialog( settings, this );
    if (d->exec()) {
        openRemote( d->hostLineEdit->text(), d->portLineEdit->text() );
    }
    delete d;
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
    restartNetworkAction->setEnabled( false );
    serialMenu->setEnabled( true );
    selfTestAction->setEnabled( true );
    // resume local echoing
    connect( keyHandler, SIGNAL(keySignal(char)), this, SLOT(localEcho(char)) );
    connect( this, SIGNAL(hostDataSignal(QByteArray)), terminal, SLOT(processHostData(QByteArray)) );
    QMessageBox::information( this, "DasherQ", "Host Disconnected" );
}

void MainWindow::restartNetworkPort() {
    if (!telnetConnection->restartTelnetConnection()) closeNetworkPort();
}

void MainWindow::showExternalHelp() {
    QDesktopServices::openUrl( QUrl( HELP_URL ) );
}

void MainWindow::showAboutDialog() {
    QMessageBox::about( this,
                        "About DasherQ",
                        "<center><b>DasherQ</b><br<br>"
                        "&copy; 2013-2017 Steve Merrony<br><br>"
                        "Version " + QString::number(VERSION) + "<br><br>"
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

    menu = menuBar()->addMenu( "View" );
    historyAction = menu->addAction( "View History" );
    connect( historyAction, SIGNAL( triggered() ), this, SLOT( viewHistory() ) );

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

    action = menu->addAction( "D211" );
    action->setCheckable( true );
    action->setChecked( false );
    connect( action, SIGNAL( triggered() ), this, SLOT( setD211emulation() ) );
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
    action = menu->addSeparator();
    restartNetworkAction = networkMenu->addAction( "&Restart" );
    connect( restartNetworkAction, SIGNAL( triggered() ), this, SLOT( restartNetworkPort() ) );
    restartNetworkAction->setEnabled( false );

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
        connectionStatusLabel->setText( status->remoteHost + ":" + status->remotePort );
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
