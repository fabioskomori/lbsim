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

#include "zouheboundary.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/math/myvector3d.h"
#include <QDebug>
#include <typeinfo>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

int ZouHeBoundary::getOpenCLType() {
    return -1;
}

ZouHeBoundary::ZouHeBoundary() {
    pressure = 0;
    velocity = 0;
}

void ZouHeBoundary::preUpdate(double[], BaseCell*[], Grid*, Vector3i) {
}

void ZouHeBoundary::preUpdate2(double[], BaseCell* neighbors[], Grid *grid, Vector3i) {
    int i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0, i7 = 0, i8 = 0;
    double ux = 0, uy = 0;
    BaseCell *cell = 0;
    ux = i0 = 0;
    if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
        cell = neighbors[1];
        uy = velocity;
        i1 = 1;
        i2 = 3;
        i3 = 5;
        i4 = 7;
        i5 = 2;
        i6 = 4;
        i7 = 6;
        i8 = 8;
    } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
        cell = neighbors[5];
        uy = velocity;
        i1 = 5;
        i2 = 3;
        i3 = 1;
        i4 = 7;
        i5 = 4;
        i6 = 2;
        i7 = 8;
        i8 = 6;
    } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
        cell = neighbors[3];
        uy = velocity;
        i1 = 3;
        i2 = 5;
        i3 = 7;
        i4 = 1;
        i5 = 4;
        i6 = 6;
        i7 = 8;
        i8 = 2;
    } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
        cell = neighbors[7];
        uy = velocity;
        i1 = 7;
        i2 = 1;
        i3 = 3;
        i4 = 5;
        i5 = 8;
        i6 = 2;
        i7 = 4;
        i8 = 6;
    }
    if (pressure > 0) {
        double ux = 1 - (cell->getNextF(i0, 0) + cell->getNextF(i2, 0) + cell->getNextF(i4, 0) + 2 * (cell->getNextF(i3, 0) + cell->getNextF(i6, 0) + cell->getNextF(i7, 0))) / pressure;
        cell->setNextF(i1, cell->getNextF(i3, 0) + 2.0 / 3 * pressure * ux, 0);
        cell->setNextF(i5, cell->getNextF(i7, 0) - 1.0 / 2 * (cell->getNextF(i2, 0) - cell->getNextF(i4, 0)) + 1.0 / 6 * pressure * ux, 0);
        cell->setNextF(i8, cell->getNextF(i6, 0) + 1.0 / 2 * (cell->getNextF(i2, 0) - cell->getNextF(i4, 0)) + 1.0 / 6 * pressure * ux, 0);
        cell->setNextF(i1, cell->getF(i1, 1), 1);
        cell->setNextF(i5, cell->getF(i5, 1), 1);
        cell->setNextF(i8, cell->getF(i8, 1), 1);
        double temp1, temp2, temp3;
        int temp0, type;
        grid->getConfig()->getMulticomponent(&temp0, &temp1, &temp2, &temp3, &type);
        if (type == MC_PS) {
            cell->setNextF(i1, LBUtil::W[9][i1] * cell->getP(1) * (1 + 3 * (LBUtil::C[9][i1] * cell->getU(0))), 1);
            cell->setNextF(i5, LBUtil::W[9][i5] * cell->getP(1) * (1 + 3 * (LBUtil::C[9][i5] * cell->getU(0))), 1);
            cell->setNextF(i8, LBUtil::W[9][i8] * cell->getP(1) * (1 + 3 * (LBUtil::C[9][i8] * cell->getU(0))), 1);
        }
    } else {
        double rho = (1.0) / (1.0 - velocity) * (cell->getNextF(i0, 0) + cell->getNextF(i4, 0) + cell->getNextF(i2, 0) + 2 * (cell->getNextF(i3, 0) + cell->getNextF(i6, 0) + cell->getNextF(i7, 0)));
        cell->setNextF(i1, cell->getNextF(i3, 0) + 2.0 / 3 * rho * uy, 0);
        cell->setNextF(i8, cell->getNextF(i6, 0) - 0.5 * (cell->getNextF(i4, 0) - cell->getNextF(i2, 0)) + 0.5 * rho * ux + 1.0 / 6 * rho * uy, 0);
        cell->setNextF(i5, cell->getNextF(i7, 0) + 0.5 * (cell->getNextF(i4, 0) - cell->getNextF(i2, 0)) - 0.5 * rho * ux + 1.0 / 6 * rho * uy, 0);
        double temp1, temp2, temp3;
        int temp0, type;
        grid->getConfig()->getMulticomponent(&temp0, &temp1, &temp2, &temp3, &type);
        if (type == MC_PS) {
            cell->setNextF(i1, LBUtil::f_eq(MyVector3D(), grid->getConfig()->getSourcePressure(1), 9, i1), 1);
            cell->setNextF(i5, LBUtil::f_eq(MyVector3D(), grid->getConfig()->getSourcePressure(1), 9, i5), 1);
            cell->setNextF(i8, LBUtil::f_eq(MyVector3D(), grid->getConfig()->getSourcePressure(1), 9, i8), 1);
        }
    }
}

void ZouHeBoundary::update() {
}

void ZouHeBoundary::postUpdate(Grid*, Vector3i) {
}

void ZouHeBoundary::reset(double, double) {
}

double ZouHeBoundary::getF(int, int) {
    return 0;
}

void ZouHeBoundary::configPressure(double pressure) {
    velocity = -1;
    this->pressure = pressure;
}

void ZouHeBoundary::configVelocity(double velocity) {
    pressure = -1;
    this->velocity = velocity;
}

double ZouHeBoundary::getVelocity() {
    return velocity;
}

double ZouHeBoundary::getPressure() {
    return pressure;
}

void ZouHeBoundary::activate(QXmlStreamReader &reader, Grid*) {
    pressure = reader.attributes().value("pressure").toString().toDouble();
    velocity = reader.attributes().value("velocity").toString().toDouble();
}

void ZouHeBoundary::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "zouhe");
    writer.writeAttribute("pressure", QString::number(pressure));
    writer.writeAttribute("velocity", QString::number(velocity));
}

void ZouHeBoundary::setNextF(int , double , int) {
}

void ZouHeBoundary::setMulticomponent(int) {
}

double ZouHeBoundary::getP(int) {
    return 0;
}

MyVector3D ZouHeBoundary::getU(int) {
    return MyVector3D();
}

double ZouHeBoundary::deltaP() {
    return 0;
}

double ZouHeBoundary::getLastDeltaP() {
    return 0;
}

double ZouHeBoundary::getNextF(int, int) {
    return 0;
}

MyVector3D ZouHeBoundary::getForcesVelocity(int) {
    return MyVector3D();
}

bool ZouHeBoundary::isFluid() {
    return false;
}
