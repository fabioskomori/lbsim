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

#include "setzoomcommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/lbwidget.h"
#include "../../view/view.h"
#include "ui_view.h"
#include "../../view/painter/painter.h"
#include "../../view/painter/camera.h"

SetZoomCommand::SetZoomCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetZoomCommand::name() {
    return "setZoom";
}

QString SetZoomCommand::help() {
    return "setZoom <zoom>";
}

QString SetZoomCommand::execute() {
    mainWindow->getView()->getUI()->zoom->setText(parameters.at(0));
    mainWindow->getView()->on_zoom_returnPressed();
    return "";
}

Command* SetZoomCommand::clone() {
    return (new SetZoomCommand(grid, mainWindow))->copy(this);
}
