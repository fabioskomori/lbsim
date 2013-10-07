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

#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <list>
class QString;
class QXmlStreamReader;
class Command;
class Grid;
class LBMainWindow;

class CommandPool {
public:
    CommandPool();
    static void init(Grid *grid, LBMainWindow *mainWindow);
    static std::list<Command*>* list();
    static Command* commandByName(QString name);
    static Command* activate(QXmlStreamReader &reader);
private:
    static Grid *grid;
    static LBMainWindow *mainWindow;
};

#endif // COMMANDPOOL_H
