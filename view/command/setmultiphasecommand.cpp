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

#include "setmultiphasecommand.h"
#include "../../view/lbmainwindow.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetMultiphaseCommand::SetMultiphaseCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetMultiphaseCommand::name() {
    return "setMultiphase";
}

QString SetMultiphaseCommand::help() {
    return "setMultiphase <g> <psi0> <p0> <gads>";
}

QString SetMultiphaseCommand::execute() {
    mainWindow->getParameters()->getUI()->mpG->setText(parameters.at(0));
    mainWindow->getParameters()->getUI()->mpPsi0->setText(parameters.at(1));
    mainWindow->getParameters()->getUI()->mpP0->setText(parameters.at(2));
    mainWindow->getParameters()->getUI()->mpGads->setText(parameters.at(3));
    mainWindow->getParameters()->on_mpG_returnPressed();
    return "";
}

Command* SetMultiphaseCommand::clone() {
    return (new SetMultiphaseCommand(grid, mainWindow))->copy(this);
}
