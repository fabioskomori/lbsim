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

#include "setresultscommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/results.h"
#include "ui_results.h"

SetResultsCommand::SetResultsCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetResultsCommand::name() {
    return "setResults";
}

QString SetResultsCommand::help() {
    return "setResults <interval> <images, 0 or 1> <v/p, 0 or 1> <simulation, 0 or 1>";
}

QString SetResultsCommand::execute() {
    mainWindow->getResults()->getUI()->interval->setText(parameters.at(0));
    mainWindow->getResults()->getUI()->images->setChecked(parameters.at(1) == "1");
    mainWindow->getResults()->getUI()->velocities->setChecked(parameters.at(2) == "1");
    mainWindow->getResults()->getUI()->reportSimulation->setChecked(parameters.at(3) == "1");
    return "";
}

Command* SetResultsCommand::clone() {
    return (new SetResultsCommand(grid, mainWindow))->copy(this);
}
