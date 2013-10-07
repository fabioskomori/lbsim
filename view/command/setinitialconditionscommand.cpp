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

#include "setinitialconditionscommand.h"
#include "../../view/lbmainwindow.h"
#include <QMutex>
#include <QWaitCondition>
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetInitialConditionsCommand::SetInitialConditionsCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetInitialConditionsCommand::name() {
    return "setInitialConditions";
}

QString SetInitialConditionsCommand::help() {
    return "setInitialConditions <minx> <maxx> <miny> <maxy> <minz> <maxz> <p1> <p2>";
}

QString SetInitialConditionsCommand::execute() {
    mainWindow->getParameters()->getUI()->minX->setText(parameters.at(0));
    mainWindow->getParameters()->getUI()->maxX->setText(parameters.at(1));
    mainWindow->getParameters()->getUI()->minY->setText(parameters.at(2));
    mainWindow->getParameters()->getUI()->maxY->setText(parameters.at(3));
    mainWindow->getParameters()->getUI()->minZ->setText(parameters.at(4));
    mainWindow->getParameters()->getUI()->maxZ->setText(parameters.at(5));
    mainWindow->getParameters()->getUI()->density1->setText(parameters.at(6));
    mainWindow->getParameters()->getUI()->density2->setText(parameters.at(7));
    mainWindow->getParameters()->on_updateInitial_clicked();
    QMutex mutex;
    mutex.lock();
    QWaitCondition wait;
    wait.wait(&mutex, 1000);
    mutex.unlock();
    return "";
}

Command* SetInitialConditionsCommand::clone() {
    return (new SetInitialConditionsCommand(grid, mainWindow))->copy(this);
}
