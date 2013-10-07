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

#include "setp0command.h"
#include "../../view/lbmainwindow.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetP0Command::SetP0Command(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetP0Command::name() {
    return "setP0";
}

QString SetP0Command::help() {
    return "setP0 <min> <max>";
}

QString SetP0Command::execute() {
    mainWindow->getParameters()->getUI()->minP0->setText(parameters.at(0));
    mainWindow->getParameters()->on_minP0_returnPressed();
    mainWindow->getParameters()->getUI()->maxP0->setText(parameters.at(1));
    mainWindow->getParameters()->on_maxP0_returnPressed();
    return "";
}

Command* SetP0Command::clone() {
    return (new SetP0Command(grid, mainWindow))->copy(this);
}
