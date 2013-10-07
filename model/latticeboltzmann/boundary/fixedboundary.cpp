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

#include "fixedboundary.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/multi/mccell.h"
#include "../../../model/latticeboltzmann/gridsimulation.h"
#include "wallcell.h"
#include <typeinfo>
#include <QDebug>
#include <cmath>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

#define PI 3.14159265

int FixedBoundary::getOpenCLType() {
    return -1;
}

FixedBoundary::FixedBoundary() {
    pressure = 1;
    velocity = 0.02;
    multiComponentIndex = 0;
}

void FixedBoundary::preUpdate(double[], BaseCell*[], Grid*, Vector3i) {
}

void FixedBoundary::preUpdate2(double[], BaseCell* neighbors[], Grid *grid, Vector3i) {
    BaseCell *empty = 0;
    MyVector3D vector = MyVector3D();
    int is[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    int m = Shared::instance()->getGridConfig()->getModel();
    switch (m) {
    case 9:
        if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
            empty = neighbors[1];
            vector = MyVector3D(velocity, 0, 0);
            is[0] = 1; is[1] = 2; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
            empty = neighbors[5];
            vector = MyVector3D(-velocity, 0, 0);
            is[0] = 4; is[1] = 5; is[2] = 6; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
            empty = neighbors[3];
            vector = MyVector3D(0, velocity, 0);
            is[0] = 2; is[1] = 3; is[2] = 4; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
            empty = neighbors[7];
            vector = MyVector3D(0, -velocity, 0);
            is[0] = 6; is[1] = 7; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        }
        break;
    case 15:
        if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[1]->isFluid())) {
            empty = neighbors[1];
            vector = MyVector3D(velocity, 0, 0);
            is[0] = 1; is[1] = 6; is[2] = 9; is[3] = 11; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[3]->isFluid())) {
            empty = neighbors[5];
            vector = MyVector3D(-velocity, 0, 0);
            is[0] = 3; is[1] = 7; is[2] = 8; is[3] = 12; is[4] = 13; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[4] == 0 || (!neighbors[4]->isFluid() && neighbors[2]->isFluid())) {
            empty = neighbors[3];
            vector = MyVector3D(0, velocity, 0);
            is[0] = 2; is[1] = 6; is[2] = 7; is[3] = 11; is[4] = 12; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[2] == 0 || (!neighbors[2]->isFluid() && neighbors[4]->isFluid())) {
            empty = neighbors[7];
            vector = MyVector3D(0, -velocity, 0);
            is[0] = 4; is[1] = 8; is[2] = 9; is[3] = 13; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        }
        break;
    case 19:
        if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
            empty = neighbors[1];
            vector = MyVector3D(velocity, 0, 0);
            is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 15; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
            empty = neighbors[5];
            vector = MyVector3D(-velocity, 0, 0);
            is[0] = 4; is[1] = 5; is[2] = 6; is[3] = 12; is[4] = 17; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
            empty = neighbors[3];
            vector = MyVector3D(0, velocity, 0);
            is[0] = 2; is[1] = 3; is[2] = 4; is[3] = 11; is[4] = 16; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
            empty = neighbors[7];
            vector = MyVector3D(0, -velocity, 0);
            is[0] = 6; is[1] = 7; is[2] = 8; is[3] = 13; is[4] = 18; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        }
        break;
    case 27:
        if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
            empty = neighbors[1];
            vector = MyVector3D(velocity, 0, 0);
            is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 11; is[5] = 17; is[6] = 19; is[7] = 20; is[8] = 26;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
            empty = neighbors[5];
            vector = MyVector3D(-velocity, 0, 0);
            is[0] = 4; is[1] = 5; is[2] = 6; is[3] = 13; is[4] = 14; is[5] = 15; is[6] = 22; is[7] = 23; is[8] = 24;
        } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
            empty = neighbors[3];
            vector = MyVector3D(0, velocity, 0);
            is[0] = 2; is[1] = 3; is[2] = 4; is[3] = 11; is[4] = 12; is[5] = 13; is[6] = 20; is[7] = 21; is[8] = 22;
        } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
            empty = neighbors[7];
            vector = MyVector3D(0, -velocity, 0);
            is[0] = 6; is[1] = 7; is[2] = 8; is[3] = 15; is[4] = 16; is[5] = 17; is[6] = 24; is[7] = 25; is[8] = 26;
        }
        break;
    }
    double pressure2 = pressure;
    if (grid->getConfig()->getSourcePeriod(multiComponentIndex) != 0) {
        pressure2 = pressure + grid->getConfig()->getSourceAmplitude(multiComponentIndex) * std::sin(2 * PI * ((grid->getSimulation()->getIterations() % grid->getConfig()->getSourcePeriod(multiComponentIndex)) / (double)grid->getConfig()->getSourcePeriod(multiComponentIndex)));
    }
    double temp1, temp2, temp3;
    int temp0, type;
    grid->getConfig()->getMulticomponent(&temp0, &temp1, &temp2, &temp3, &type);
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < 9; i++) {
        if (is[i] != -1) {
            empty->setNextF(is[i], 0, multiComponentIndex == 1 ? 0 : 1);
            if (type == MC_PS) {
                empty->setNextF(is[i], LBUtil::f_eq(vector, grid->getConfig()->getSourcePressure(1), model, is[i]), multiComponentIndex == 1 ? 0 : 1);
            }
            //empty->setNextF(is[i], LBUtil::f_eq(vector, pressure2, model, is[i]) + (nextF[LBUtil::OPPOSITE[model][is[i]]] - LBUtil::f_eq(vector, pressure2, model, LBUtil::OPPOSITE[model][is[i]])), multiComponentIndex);
            empty->setNextF(is[i], LBUtil::f_eq(vector, pressure2, model, is[i]), multiComponentIndex);
        }
    }
}

void FixedBoundary::update() {
}

void FixedBoundary::postUpdate(Grid*, Vector3i) {
}

void FixedBoundary::reset(double, double) {
}

void FixedBoundary::setNextF(int index, double value, int component) {
    if (component == 0) {
        nextF[index] = value;
    }
}

double FixedBoundary::getF(int, int) {
    return 0.0;
}

void FixedBoundary::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "fixed");
    writer.writeAttribute("velocity", QString::number(velocity));
    writer.writeAttribute("pressure", QString::number(pressure));
    writer.writeAttribute("multiComponentIndex", QString::number(multiComponentIndex));
}

void FixedBoundary::activate(QXmlStreamReader &reader, Grid *) {
    velocity = reader.attributes().value("velocity").toString().toDouble();
    pressure = reader.attributes().value("pressure").toString().toDouble();
    multiComponentIndex = reader.attributes().value("multiComponentIndex").toString().toInt();
}

void FixedBoundary::configPressure(double pressure) {
    this->pressure = pressure;
}

void FixedBoundary::configVelocity(double velocity) {
    this->velocity = velocity;
}

double FixedBoundary::getVelocity() {
    return velocity;
}

double FixedBoundary::getPressure() {
    return pressure;
}

void FixedBoundary::setMultiComponentIndex(int multiComponentIndex) {
    this->multiComponentIndex = multiComponentIndex;
}

void FixedBoundary::setMulticomponent(int) {
}

double FixedBoundary::getP(int) {
    return 0;
}

MyVector3D FixedBoundary::getU(int) {
    return MyVector3D();
}

double FixedBoundary::deltaP() {
    return 0;
}

double FixedBoundary::getLastDeltaP() {
    return 0;
}

double FixedBoundary::getNextF(int, int) {
    return 0;
}

MyVector3D FixedBoundary::getForcesVelocity(int) {
    return MyVector3D();
}

bool FixedBoundary::isFluid() {
    return false;
}
