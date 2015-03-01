#ifndef CHANGESIZEDIALOG_H
#define CHANGESIZEDIALOG_H

#include <QComboBox>
#include <QDialog>

class ChangeSizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChangeSizeDialog( QWidget *parent = 0 );

    QComboBox *colsComboBox, *rowsComboBox;

};

#endif // CHANGESIZEDIALOG_H
