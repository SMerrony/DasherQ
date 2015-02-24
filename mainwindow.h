#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>

#include <QAction>
#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QMainWindow>
#include <QTextStream>

#include "crt.h"
#include "fkeymatrix.h"
#include "keyboardhandler.h"
#include "networkconnectdialog.h"
#include "serialconnectdialog.h"
#include "serialconnection.h"
#include "status.h"
#include "telnetconnection.h"
#include "terminal.h"

const QString HELP_URL = "http://stephenmerrony.co.uk/dg/software/new-software/dasherq-terminal-emulator.html";

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *centerWidget;
    QMenu *mainMenu, *emulationMenu;
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void setD200emulation();
    void setD210emulation();
    void selfTest();
    void toggleLogging();
    void openSerialPort();
    void closeSerialPort();
    void openNetworkPort();
    void closeNetworkPort();
    void showExternalHelp();
    void showAboutDialog();
    void updateCrtIfDirty();
    void updateBlinkState();
    void updateStatusBar();
    void localPrintRequest();
    void localEcho( char );
    void logChar( char );
    void sendFile();

signals:
    void hostDataSignal( QByteArray );
    void keySignal( char );

private:
    QQueue<unsigned char> *fromHostQ;

    Status *status;

    FkeyMatrix *fKeyMatrix;
    Crt *crt;
    KeyBoardHandler *keyHandler;

    Terminal *terminal;

    void setupMenuBar();
    void setupStatusBar();

    // some menus/actions that we need access to
    QMenu *serialMenu, *networkMenu;
    QAction *loggingAction, *selfTestAction, *loadTemplateAction, *openSerialAction, *closeSerialAction, *openNetworkAction, *closeNetworkAction;

    QLabel *onlineStatusLabel, *connectionStatusLabel, *emulationStatusLabel;

    QFile loggingFile;
    QTextStream loggingStream;

    SerialConnection *serialConnection;
    TelnetConnection *telnetConnection;

};

#endif // MAINWINDOW_H
