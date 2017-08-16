/*
 * Copyright (C) 2013-2017 Stephen Merrony
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main( int argc, char *argv[] ) {

    QApplication a( argc, argv );
    a.setApplicationName( APP_NAME );
    a.setApplicationVersion( QString::number(VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription( "DASHER Terminal Emulator" );
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption hostOption(QStringList() << "host" << "host:port","Connect to specified host", "host:port");
    parser.addOption( hostOption );
    parser.process(a);

    MainWindow w( parser.value( hostOption ) );
    w.show();
    
    return a.exec();
}
