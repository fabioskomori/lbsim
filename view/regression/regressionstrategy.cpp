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

#include "regressionstrategy.h"
#include "datapoint.h"
#include "../../view/lbmainwindow.h"
#include "../../view/lbwidget.h"
#include "../../view/painter/painter.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/basecell.h"
#include <QDebug>
#include <cmath>

RegressionStrategy::RegressionStrategy() {
    listener = 0;
}

double RegressionStrategy::getNewError() {
    return newError;
}

double RegressionStrategy::getNewValue() {
    return newValue;
}

void RegressionStrategy::addDataPoint(DataPoint dataPoint) {
    dataPoints.push_back(dataPoint);
}

void RegressionStrategy::addParameter(ParameterControl *parameter) {
    parameters.push_back(parameter);
}

void RegressionStrategy::injectMainWindow(LBMainWindow *mainWindow) {
    this->mainWindow = mainWindow;
}

void RegressionStrategy::setListener(Listener *listener) {
    this->listener = listener;
}

double RegressionStrategy::standardDeviation() {
    return std::sqrt(squaredResiduals() / dataPoints.size());
}

double RegressionStrategy::squaredResiduals() {
    double result = 0;
    for (std::list<DataPoint>::iterator dataPoint = dataPoints.begin(); dataPoint != dataPoints.end(); dataPoint++) {
        if ((*dataPoint).getDensityEnabled() == 1) {
            double error = (*dataPoint).getDensity() - mainWindow->getLB()->getPainter()->getGrid()->getGrid((*dataPoint).getPoint().getY(), (*dataPoint).getPoint().getX(), (*dataPoint).getPoint().getZ())->getP(0);
            result += error * error;
        }
        if ((*dataPoint).getVelocityEnabled().getX() == 1) {
            double error = (*dataPoint).getVelocity().getX() - mainWindow->getLB()->getPainter()->getGrid()->getGrid((*dataPoint).getPoint().getY(), (*dataPoint).getPoint().getX(), (*dataPoint).getPoint().getZ())->getU(0).getX();
            result += error * error;
        }
        if ((*dataPoint).getVelocityEnabled().getY() == 1) {
            double error = (*dataPoint).getVelocity().getY() - mainWindow->getLB()->getPainter()->getGrid()->getGrid((*dataPoint).getPoint().getY(), (*dataPoint).getPoint().getX(), (*dataPoint).getPoint().getZ())->getU(0).getY();
            result += error * error;
        }
        if ((*dataPoint).getVelocityEnabled().getZ() == 1) {
            double error = (*dataPoint).getVelocity().getZ() - mainWindow->getLB()->getPainter()->getGrid()->getGrid((*dataPoint).getPoint().getY(), (*dataPoint).getPoint().getX(), (*dataPoint).getPoint().getZ())->getU(0).getZ();
            result += error * error;
        }
    }
    return result;
}

void RegressionStrategy::callback(int event) {
    if (event == LISTENER_EVENT_GRIDUPDATE) {
        process();
    }
}

void RegressionStrategy::notify() {
    if (listener != 0) {
        listener->callback(0);
    }
}
