#-------------------------------------------------
#
# Project created by QtCreator 2013-07-24T21:45:08
#
#-------------------------------------------------

QT       += core gui serialport network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DasherQ
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    status.cpp \
    cell.cpp \
    keyboardhandler.cpp \
    bdffont.cpp \
    crt.cpp \
    serialconnection.cpp \
    serialconnectdialog.cpp \
    networkconnectdialog.cpp \
    telnetconnection.cpp \
    terminal.cpp

HEADERS  += mainwindow.h \
    status.h \
    cell.h \
    keyboardhandler.h \
    bdffont.h \
    crt.h \
    serialconnection.h \
    serialconnectdialog.h \
    networkconnectdialog.h \
    telnetconnection.h \
    terminal.h

FORMS    +=

OTHER_FILES += \
    DGlogoOrange.png \
    D410-b-12.bdf

RESOURCES += \
    dqIcon.qrc
