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

#include "tauparameter.h"
#include "../../view/lbmainwindow.h"
#include "../../view/lbwidget.h"
#include "../../view/painter/painter.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../view/parameters2.h"
#include "ui_parameters2.h"

TauParameter::TauParameter(LBMainWindow *mainWindow) : Parameter(mainWindow) {
}

void TauParameter::set(double value) {
    mainWindow->getParameters()->getUI()->epsilon->setText(QString::number(value));
    mainWindow->getParameters()->on_epsilon_returnPressed();
}

double TauParameter::get() {
    return mainWindow->getLB()->getPainter()->getGrid()->getConfig()->getEpsilons()[0];
}

QString TauParameter::name() {
    return "relaxation parameter";
}
