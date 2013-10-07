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

#include "setbackgroundcommand.h"
#include "../../view/lbmainwindow.h"
#include "../../view/lbwidget.h"
#include "../../view/painter/painter.h"

SetBackgroundCommand::SetBackgroundCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString SetBackgroundCommand::name() {
    return "setBackground";
}

QString SetBackgroundCommand::help() {
    return "setBackground <color rgb code, e.g., 2F8B4C>";
}

QString SetBackgroundCommand::execute() {
    bool ok;
    int rgb = parameters.at(0).toInt(&ok, 16);
    int b = rgb % 256;
    rgb /= 256;
    int g = rgb % 256;
    rgb /= 256;
    int r = rgb % 256;
    mainWindow->getLB()->getPainter()->setBackgroundColor(QColor(r, g, b));
    mainWindow->getLB()->repaint();
    return "";
}

Command* SetBackgroundCommand::clone() {
    return (new SetBackgroundCommand(grid, mainWindow))->copy(this);
}
