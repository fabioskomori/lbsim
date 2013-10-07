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

#include "densitycommand.h"
#include "../../model/latticeboltzmann/basecell.h"
#include "../../model/latticeboltzmann/lbgrid.h"

DensityCommand::DensityCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString DensityCommand::name() {
    return "density";
}

QString DensityCommand::help() {
    return "density <x> <y> <z>";
}

QString DensityCommand::execute() {
    BaseCell *cell = grid->getGrid(parameters.at(1).toInt() - 1, parameters.at(0).toInt() - 1, parameters.at(2).toInt() - 1);
    if (cell != 0) {
        return (QString("p = ").append(QString::number(cell->getP(-1))));
    }
    return "";
}

Command* DensityCommand::clone() {
    return (new DensityCommand(grid, mainWindow))->copy(this);
}
