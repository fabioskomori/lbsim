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

#include "setmovingwallscommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

SetMovingWallsCommand::SetMovingWallsCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetMovingWallsCommand::name() {
    return "setMovingWalls";
}

QString SetMovingWallsCommand::help() {
    return "setMovingWalls <vx> <vy>";
}

QString SetMovingWallsCommand::execute() {
    mainWindow->getParameters()->getUI()->movingVelocity->setText(parameters.at(0));
    mainWindow->getParameters()->on_movingVelocity_returnPressed();
    mainWindow->getParameters()->getUI()->movingVelocityY->setText(parameters.at(1));
    mainWindow->getParameters()->on_movingVelocityY_returnPressed();
    return "";
}

Command* SetMovingWallsCommand::clone() {
    return (new SetMovingWallsCommand(grid, mainWindow))->copy(this);
}
