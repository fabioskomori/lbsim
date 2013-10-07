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

#include "simplecontroller.h"
#include "../../view/lbmainwindow.h"
#include "../../view/lbwidget.h"
#include "../../view/painter/painter.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridsimulation.h"
#include "parametercontrol.h"
#include "../../view/parameter/parameter.h"

SimpleController::SimpleController(int period, double firstVariation, double p) : period(period), firstVariation(firstVariation), p(p), step(0) {
}

void SimpleController::process() {
    if (mainWindow->getLB()->getPainter()->getGrid()->getSimulation()->getIterations() % period == 0) {
        newError = standardDeviation();
        newValue = 0;
        if (step == 0) {
            newValue = (parameters.front())->getInitialValue();
        } else if (step == 1) {
            newValue = firstVariation;
        } else {
            newValue = (lastValue - p * (lastChange / (newError - lastError)) * newError);
            if (newValue > (parameters.front())->getMaxValue()) {
                newValue = (parameters.front())->getMaxValue();
            } else if (newValue < (parameters.front())->getMinValue()) {
                newValue = (parameters.front())->getMinValue();
            }
        }
        (parameters.front())->getParameter()->set(newValue);
        lastChange = newValue - lastValue;
        lastValue = newValue;
        lastError = newError;
        step++;
        notify();
    }
}
