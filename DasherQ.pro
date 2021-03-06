#-------------------------------------------------
#
# Project created by QtCreator 2013-07-24T21:45:08
#
#-------------------------------------------------

QT       += core gui serialport network printsupport

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

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
    terminal.cpp \
    fkeymatrix.cpp \
    changesizedialog.cpp \
    history.cpp

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
    terminal.h \
    fkeymatrix.h \
    changesizedialog.h \
    history.h

FORMS    +=

OTHER_FILES += \
    DGlogoOrange.png \
    D410-b-12.bdf \
    SED_template.txt \
    SMI_template.txt

RESOURCES += \
    dqIcon.qrc

RC_FILE = DasherQ.rc

DISTFILES += \
    DGlogoOrange.ico \
    DasherQ.rc \
    BROWSE_template.txt \
    MEMACS_template.txt
