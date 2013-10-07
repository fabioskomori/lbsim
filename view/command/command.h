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

#ifndef COMMAND_H
#define COMMAND_H

class QString;
#include <QStringList>
class QXmlStreamWriter;
class Grid;
class LBMainWindow;

class Command {
public:
    Command(Grid *grid, LBMainWindow *mainWindow);
    void addParameter(QString parameter);
    void setVariable(QString variable, QString value);
    void passivate(QXmlStreamWriter &writer);
    QString toString();
    void die();
    void live();
    bool dead();
    Command* copy(Command *from);
    virtual QString name() = 0;
    virtual QString help() = 0;
    virtual QString execute() = 0;
    virtual Command* clone() = 0;
protected:
    QStringList parameters;
    Grid *grid;
    LBMainWindow *mainWindow;
    bool canceled;
};

#endif // COMMAND_H
