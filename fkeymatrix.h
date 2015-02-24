#ifndef FKEYMATRIX_H
#define FKEYMATRIX_H

#include <QDockWidget>
#include <QLabel>

#include "keyboardhandler.h"

class FkeyMatrix : public QDockWidget
{
  Q_OBJECT
public:
  explicit FkeyMatrix( QWidget *parent, KeyBoardHandler *keyHandler );
  ~FkeyMatrix();

signals:

public slots:   
    bool loadTemplate();

private:
    QString templateTitle;
    QLabel *templateLabel, *templateLabel2;
    QString fKeyStrings[4][15];
    QLabel *fKeyLabels[4][15];

};

#endif // FKEYMATRIX_H
