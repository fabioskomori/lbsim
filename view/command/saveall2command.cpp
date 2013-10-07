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

#include "saveall2command.h"
#include "../../view/lbmainwindow.h"

SaveAll2Command::SaveAll2Command(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SaveAll2Command::name() {
    return "saveAll2";
}

QString SaveAll2Command::help() {
    return "saveAll2 <filename>";
}

QString SaveAll2Command::execute() {
    mainWindow->saveAll2(parameters.at(0));
    return "";
}

Command* SaveAll2Command::clone() {
    return (new SaveAll2Command(grid, mainWindow))->copy(this);
}
