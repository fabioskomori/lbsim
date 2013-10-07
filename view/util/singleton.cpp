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

#include "singleton.h"
#include <QMutex>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

Singleton::Singleton() {
    renderMutex = new QMutex();
}

QString Singleton::adjustPath(QString path) {
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/") + path;
#else
    const QString pathInInstallDir = QCoreApplication::applicationDirPath()
        + QLatin1String("/../") + path;
    if (pathInInstallDir.contains(QLatin1String("opt"))
            && pathInInstallDir.contains(QLatin1String("bin"))
            && QFileInfo(pathInInstallDir).exists()) {
        return pathInInstallDir;
    }
#endif
#endif
    return path;
}

Singleton* Singleton::instance() {
    static Singleton *newSingleton = 0;
    if (!newSingleton) {
        newSingleton = new Singleton();
    }
    return newSingleton;
}

void Singleton::setConsole(ConsoleDialog *console) {
    this->console = console;
}

ConsoleDialog* Singleton::getConsole() {
    return console;
}

void Singleton::setParameters(Parameters *parameters) {
    this->parameters = parameters;
}

Parameters* Singleton::getParameters() {
    return parameters;
}

QMutex* Singleton::getRenderMutex() {
    return renderMutex;
}

void Singleton::setFolder(QString folder) {
    this->folder = folder;
}

QString Singleton::getFolder() {
    return folder;
}

void Singleton::setPainterConfig(PainterConfig *painterConfig) {
    this->painterConfig = painterConfig;
}

PainterConfig* Singleton::getPainterConfig() {
    return painterConfig;
}
