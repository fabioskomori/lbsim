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

#include "resetcommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/lbwidget.h"
#include "../../view/painter/painter.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include <QMutex>
#include <QWaitCondition>

ResetCommand::ResetCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString ResetCommand::name() {
    return "reset";
}

QString ResetCommand::help() {
    return "reset";
}

QString ResetCommand::execute() {
    //QMetaObject::invokeMethod(mainWindow, "on_actionReset_triggered");
    mainWindow->getLB()->reset();
    mainWindow->getLB()->getPainter()->getGrid()->reset();
    return "";
}

Command* ResetCommand::clone() {
    return (new ResetCommand(grid, mainWindow))->copy(this);
}
