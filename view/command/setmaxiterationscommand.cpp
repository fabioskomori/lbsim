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

#include "setmaxiterationscommand.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../view/lbmainwindow.h"
#include "../../view/results.h"
#include "ui_results.h"
#include <QMutex>
#include <QWaitCondition>

SetMaxIterationsCommand::SetMaxIterationsCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetMaxIterationsCommand::name() {
    return "setMaxIterations";
}

QString SetMaxIterationsCommand::help() {
    return "setMaxIterations <max>";
}

QString SetMaxIterationsCommand::execute() {
    mainWindow->getResults()->getUI()->maxSteps->setText(parameters.at(0));
    mainWindow->getResults()->on_maxSteps_returnPressed();
    QMutex mutex;
    QWaitCondition waitCondition;
    mutex.lock();
    waitCondition.wait(&mutex, 1000);
    mutex.unlock();
    return "";
}

Command* SetMaxIterationsCommand::clone() {
    return (new SetMaxIterationsCommand(grid, mainWindow))->copy(this);
}
