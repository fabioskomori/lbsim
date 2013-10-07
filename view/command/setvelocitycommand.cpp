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

#include "setvelocitycommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetVelocityCommand::SetVelocityCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetVelocityCommand::name() {
    return "setVelocity";
}

QString SetVelocityCommand::help() {
    return "setVelocity <v1> <v2>";
}

QString SetVelocityCommand::execute() {
    mainWindow->getParameters()->getUI()->inletOrder->setCurrentIndex(0);
    mainWindow->getParameters()->getUI()->sourceVelocity->setText(parameters.at(0));
    mainWindow->getParameters()->on_sourceVelocity_returnPressed();
    mainWindow->getParameters()->getUI()->inletOrder->setCurrentIndex(1);
    mainWindow->getParameters()->getUI()->sourceVelocity->setText(parameters.at(1));
    mainWindow->getParameters()->on_sourceVelocity_returnPressed();
    return "";
}

Command* SetVelocityCommand::clone() {
    return (new SetVelocityCommand(grid, mainWindow))->copy(this);
}
