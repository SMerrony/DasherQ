#ifndef NETWORKCONNECTDIALOG_H
#define NETWORKCONNECTDIALOG_H

#include <QDialog>
#include <QLineEdit>

class NetworkConnectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NetworkConnectDialog(QWidget *parent = 0);

    QLineEdit *hostLineEdit, *portLineEdit;
    
signals:
    
public slots:
    
};

#endif // NETWORKCONNECTDIALOG_H
