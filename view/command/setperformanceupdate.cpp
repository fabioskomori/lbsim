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

#include "setperformanceupdate.h"
#include "../../view/lbmainwindow.h"
#include "../../view/info.h"
#include "ui_info.h"
//#include "ui_opencl.h"

SetPerformanceUpdate::SetPerformanceUpdate(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetPerformanceUpdate::name() {
    return "setPerformanceUpdate";
}

QString SetPerformanceUpdate::help() {
    return "setPerformanceUpdate <render> <transfer>";
}

QString SetPerformanceUpdate::execute() {
    mainWindow->getInfo()->getUI()->renderUpdate->setText(parameters.at(0));
    //mainWindow->getOpenCL()->getUI()->openclIterations->setText(parameters.at(1));
    //mainWindow->getOpenCL()->on_openclIterations_returnPressed();
    return "";
}

Command* SetPerformanceUpdate::clone() {
    return (new SetPerformanceUpdate(grid, mainWindow))->copy(this);
}
