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

#include "pointboundary.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/gridsimulation.h"
#include <cmath>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

#define PI 3.14159265

int PointBoundary::getOpenCLType() {
    return -1;
}

PointBoundary::PointBoundary() {
}

void PointBoundary::preUpdate(double[], BaseCell* neighbors[], Grid *grid, Vector3i) {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        if (neighbors[i] != 0) {
            double pressure2 = pressure;
            if (grid->getConfig()->getSourcePeriod(0) != 0) {
                pressure2 = pressure + grid->getConfig()->getSourceAmplitude(0) * std::sin(2 * PI * ((grid->getSimulation()->getIterations() % grid->getConfig()->getSourcePeriod(0)) / (double)grid->getConfig()->getSourcePeriod(0)));
            }
            neighbors[i]->setNextF(i, LBUtil::f_eq(MyVector3D(), pressure2, model, i), 0);
        }
    }
}

void PointBoundary::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void PointBoundary::update() {
}

void PointBoundary::postUpdate(Grid*, Vector3i) {
}

void PointBoundary::reset(double, double) {
}

double PointBoundary::getF(int, int) {
    return 0;
}

void PointBoundary::setNextF(int, double, int) {
}

void PointBoundary::setMulticomponent(int) {
}

void PointBoundary::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "point");
    writer.writeAttribute("pressure", QString::number(pressure));
}

void PointBoundary::activate(QXmlStreamReader &reader, Grid *) {
    pressure = reader.attributes().value("pressure").toString().toDouble();
}

double PointBoundary::getP(int) {
    return 0;
}

MyVector3D PointBoundary::getU(int) {
    return MyVector3D();
}

double PointBoundary::deltaP() {
    return 0;
}

double PointBoundary::getLastDeltaP() {
    return 0;
}

double PointBoundary::getNextF(int, int) {
    return 0;
}

MyVector3D PointBoundary::getForcesVelocity(int) {
    return MyVector3D();
}

bool PointBoundary::isFluid() {
    return false;
}

void PointBoundary::configPressure(double pressure) {
    this->pressure = pressure;
}

double PointBoundary::getPressure() {
    return pressure;
}

void PointBoundary::configVelocity(double) {
}

double PointBoundary::getVelocity() {
    return 0;
}
