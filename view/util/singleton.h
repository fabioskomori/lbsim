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

#ifndef SINGLETON_H
#define SINGLETON_H

#include <QString>
class QMutex;
class ConsoleDialog;
class Parameters;
class PainterConfig;

class Singleton {
public:
    static Singleton* instance();
    static QString adjustPath(QString path);
    void setConsole(ConsoleDialog *console);
    ConsoleDialog* getConsole();
    QMutex* getRenderMutex();
    void setParameters(Parameters *parameters);
    Parameters* getParameters();
    void setFolder(QString folder);
    QString getFolder();
    void setPainterConfig(PainterConfig *painterConfig);
    PainterConfig* getPainterConfig();
private:
    Singleton();
    ConsoleDialog *console;
    QMutex *renderMutex;
    Parameters *parameters;
    PainterConfig *painterConfig;
    QString folder;
};

#endif // SINGLETON_H
