#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>

#include <QAction>
#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QMainWindow>
#include <QScrollArea>
#include <QSettings>
#include <QTextStream>

#include "crt.h"
#include "fkeymatrix.h"
#include "keyboardhandler.h"
#include "changesizedialog.h"
#include "networkconnectdialog.h"
#include "serialconnectdialog.h"
#include "serialconnection.h"
#include "status.h"
#include "telnetconnection.h"
#include "terminal.h"

const QString HELP_URL = "http://stephenmerrony.co.uk/dg/doku.php?id=software:newsoftware:dasherq";
const QString APP_NAME = "DasherQ";
const QString ORG_NAME = "SMerrony";
const double VERSION = 1.3;
const int BLINK_TIMER_MS = 500;
const int CRT_REFRESH_MS = 50;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *centerWidget;
    QMenu *mainMenu, *emulationMenu;
    
public:
    explicit MainWindow( QString hostArg, QWidget *parent = 0);
    ~MainWindow();

protected:
    void focusInEvent( QFocusEvent * );
    void focusOutEvent( QFocusEvent * );

private slots:
    void viewHistory();
    void setD200emulation();
    void setD210emulation();
    void setD211emulation();
    void resize();
    void selfTest();
    void toggleLogging();
    void openSerialPort();
    void closeSerialPort();
    void openRemote( QString host, QString port);
    void openNetworkPort();
    void closeNetworkPort();
    void restartNetworkPort();
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
    QScrollArea *scrollArea;
    KeyBoardHandler *keyHandler;

    Terminal *terminal;

    void setupMenuBar();
    void setupStatusBar();

    // some menus/actions that we need access to
    QMenu *serialMenu, *networkMenu;
    QAction *loggingAction, *historyAction, *resizeAction, *selfTestAction, *loadTemplateAction,
            *openSerialAction, *closeSerialAction,
            *openNetworkAction, *closeNetworkAction, *restartNetworkAction;

    QLabel *onlineStatusLabel, *connectionStatusLabel, *emulationStatusLabel;

    QFile loggingFile;
    QTextStream loggingStream;

    SerialConnection *serialConnection;
    TelnetConnection *telnetConnection;

    QSettings *settings;
    History *history;

};

#endif // MAINWINDOW_H
