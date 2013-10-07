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

#include "setgravitycommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetGravityCommand::SetGravityCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetGravityCommand::name() {
    return "setGravity";
}

QString SetGravityCommand::help() {
    return "setGravity <gravity>";
}

QString SetGravityCommand::execute() {
    mainWindow->getParameters()->getUI()->gravityEnable->setChecked(true);
    mainWindow->getParameters()->on_gravityEnable_clicked();
    mainWindow->getParameters()->getUI()->gravity->setText(parameters.at(0));
    mainWindow->getParameters()->on_gravity_returnPressed();
    return "";
}

Command* SetGravityCommand::clone() {
    return (new SetGravityCommand(grid, mainWindow))->copy(this);
}
