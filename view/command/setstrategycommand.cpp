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

#include "setstrategycommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetStrategyCommand::SetStrategyCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetStrategyCommand::name() {
    return "setStrategy";
}

QString SetStrategyCommand::help() {
    return "setStrategy <1: equilibrium, 2: fixed flux, 3: zou he>";
}

QString SetStrategyCommand::execute() {
    mainWindow->getParameters()->getUI()->outletStrategy->setCurrentIndex(parameters.at(0).toInt());
    mainWindow->getParameters()->on_outletStrategy_currentIndexChanged(parameters.at(0).toInt());
    return "";
}

Command* SetStrategyCommand::clone() {
    return (new SetStrategyCommand(grid, mainWindow))->copy(this);
}
