#ifndef NETWORKCONNECTDIALOG_H
#define NETWORKCONNECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSettings>

const QString LAST_HOST_SETTING = "LAST_HOST";
const QString LAST_PORT_SETTING = "LAST_PORT";

class NetworkConnectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NetworkConnectDialog( QSettings *settings, QWidget *parent = 0 );

    QLineEdit *hostLineEdit, *portLineEdit;
    
signals:
    
public slots:
    
};

#endif // NETWORKCONNECTDIALOG_H
