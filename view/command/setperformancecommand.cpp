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

#include "setperformancecommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/info.h"
#include "ui_info.h"

SetPerformanceCommand::SetPerformanceCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetPerformanceCommand::name() {
    return "setPerformance";
}

QString SetPerformanceCommand::help() {
    return "setPerformance <multicore, 0 or 1> <disable charts, 0 or 1>";
}

QString SetPerformanceCommand::execute() {
    mainWindow->getInfo()->getUI()->multicoreSupport->setChecked(parameters.at(0) == "1");
    mainWindow->getInfo()->on_multicoreSupport_clicked();
    mainWindow->getInfo()->getUI()->disableCharts->setChecked(parameters.at(1) == "1");
    mainWindow->getInfo()->on_disableCharts_clicked();
    return "";
}

Command* SetPerformanceCommand::clone() {
    return (new SetPerformanceCommand(grid, mainWindow))->copy(this);
}
