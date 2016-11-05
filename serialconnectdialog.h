#ifndef SERIALCONNECTDIALOG_H
#define SERIALCONNECTDIALOG_H

#include <QComboBox>
#include <QDialog>

const QString LAST_SERIAL_SETTING = "LAST_SERIAL";
const QString LAST_BAUD_SETTING   = "LAST_BAUD";

class SerialConnectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SerialConnectDialog(QWidget *parent = 0);

    QComboBox *portsComboBox,
              *baudComboBox,
              *dataComboBox,
              *parityComboBox,
              *stopComboBox,
              *flowComboBox;
    
signals:
    
public slots:
    
};

#endif // SERIALCONNECTDIALOG_H
