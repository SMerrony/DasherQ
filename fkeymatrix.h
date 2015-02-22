#ifndef FKEYMATRIX_H
#define FKEYMATRIX_H

#include <QDockWidget>

#include "keyboardhandler.h"

class FkeyMatrix : public QDockWidget
{
  Q_OBJECT
public:
  explicit FkeyMatrix( QWidget *parent, KeyBoardHandler *keyHandler );
  ~FkeyMatrix();

signals:

public slots:

private:


};

#endif // FKEYMATRIX_H
