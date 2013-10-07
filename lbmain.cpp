/*
    LBSim: a fluid dynamics simulator using the lattice Boltzmann method
    Copyright (C) 2013 Fabio Sussumu Komori - NDS/GNMD/LME/PSI/EP/USP

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include "view/lbmainwindow.h"
#include <ctime>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include <iostream>

void myMessageHandler(QtMsgType type, const char *msg) {
    QString txt;
    switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(msg);
            break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(msg);
            break;
        case QtFatalMsg:
            txt = QString("Fatal: %1").arg(msg);
            break;
    }
    QFile outFile("lbsim.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " " << txt << "\r\n";
    if (type == QtFatalMsg) {
        abort();
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LBMainWindow *w = new LBMainWindow();
    w->showMaximized();
    qsrand(time(0));
    return a.exec();
}
