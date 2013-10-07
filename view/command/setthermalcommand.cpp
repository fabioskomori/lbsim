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

#include "setthermalcommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetThermalCommand::SetThermalCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetThermalCommand::name() {
    return "setThermal";
}

QString SetThermalCommand::help() {
    return "setThermal <wall1> <wall2> <wall3> <gravity>";
}

QString SetThermalCommand::execute() {
    mainWindow->getParameters()->getUI()->thermalIndex->setCurrentIndex(0);
    mainWindow->getParameters()->getUI()->thermal1->setText(parameters.at(0));
    mainWindow->getParameters()->on_thermal1_returnPressed();
    mainWindow->getParameters()->getUI()->thermalIndex->setCurrentIndex(1);
    mainWindow->getParameters()->getUI()->thermal1->setText(parameters.at(1));
    mainWindow->getParameters()->on_thermal1_returnPressed();
    mainWindow->getParameters()->getUI()->thermalIndex->setCurrentIndex(2);
    mainWindow->getParameters()->getUI()->thermal1->setText(parameters.at(2));
    mainWindow->getParameters()->on_thermal1_returnPressed();
    mainWindow->getParameters()->getUI()->thermalGravity->setText(parameters.at(3));
    mainWindow->getParameters()->on_thermalGravity_returnPressed();
    return "";
}

Command* SetThermalCommand::clone() {
    return (new SetThermalCommand(grid, mainWindow))->copy(this);
}
