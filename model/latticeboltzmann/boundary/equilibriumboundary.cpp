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

#include "equilibriumboundary.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "wallcell.h"
#include "../../../model/latticeboltzmann/multi/mccell.h"
#include "../../../model/latticeboltzmann/gridsimulation.h"
#include <typeinfo>
#include <QDebug>
#include <cmath>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

#define PI 3.14159265

int EquilibriumBoundary::getOpenCLType() {
    return -1;
}

EquilibriumBoundary::EquilibriumBoundary() {
    pressure = velocity = -1;
    multiComponentIndex = 0;
}

EquilibriumBoundary::~EquilibriumBoundary() {
}

void EquilibriumBoundary::preUpdate(double[], BaseCell* neighbors[], Grid *grid, Vector3i) {
    BaseCell *empty = 0;
    MyVector3D vector = MyVector3D();
    int is[9];
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
    if (empty != 0) {
        double periodic = grid->getConfig()->getSourcePeriod(0) == 0 ? 0 : std::sin(2 * PI * (grid->getSimulation()->getIterations() % grid->getConfig()->getSourcePeriod(0)) / grid->getConfig()->getSourcePeriod(0));
        double temp1, temp2, temp3;
        int mcQtt, type;
        grid->getConfig()->getMulticomponent(&mcQtt, &temp1, &temp2, &temp3, &type);
        for (int i = 0; i < 9; i++) {
            if (is[i] != -1) {
                if (pressure > 0) {
                    empty->setNextF(is[i], LBUtil::f_eq(empty->getU(-1), pressure * (1 + periodic * grid->getConfig()->getSourceAmplitude(0)), Shared::instance()->getGridConfig()->getModel(), is[i]), 0);
                    if (type == MC_PS) {
                        //empty->setNextF(indexes[i], empty->getF(LBUtil::OPPOSITE[indexes[i]], 1), 1);
                        empty->setNextF(is[i], LBUtil::f_eq(empty->getU(-1), empty->getP(1), Shared::instance()->getGridConfig()->getModel(), is[i]), 1);
                    } else if (mcQtt == 2 && type == MC_SC) {
                        empty->setNextF(is[i], LBUtil::f_eq(MyVector3D(), empty->getP(0), Shared::instance()->getGridConfig()->getModel(), is[i]), 0);
                        empty->setNextF(is[i], LBUtil::f_eq(MyVector3D(), empty->getP(1), Shared::instance()->getGridConfig()->getModel(), is[i]), 1);
                    }
                } else {
                    empty->setNextF(is[i], LBUtil::f_eq(vector ^ (1 + periodic * grid->getConfig()->getSourceAmplitude(0)), empty->getP(0), Shared::instance()->getGridConfig()->getModel(), is[i]), 0);
                    if (type == MC_PS) {
                        empty->setNextF(is[i], LBUtil::f_eq(empty->getU(-1), grid->getConfig()->getSourcePressure(1), Shared::instance()->getGridConfig()->getModel(), is[i]), 1);
                    } else if (mcQtt == 2 && type == MC_SC) {
                        empty->setNextF(is[i], LBUtil::f_eq(vector, grid->getConfig()->getSourcePressure(0), Shared::instance()->getGridConfig()->getModel(), is[i]), 0);
                        empty->setNextF(is[i], 0, 1);
                    }
                }
            }
        }
    }
}

void EquilibriumBoundary::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void EquilibriumBoundary::update() {
}

void EquilibriumBoundary::postUpdate(Grid*, Vector3i) {
}

void EquilibriumBoundary::reset(double, double) {
}

double EquilibriumBoundary::getF(int, int) {
    return 0;
}

void EquilibriumBoundary::configPressure(double pressure) {
    this->pressure = pressure;
    this->velocity = -1;
}

void EquilibriumBoundary::configVelocity(double velocity) {
    this->velocity = velocity;
    this->pressure = -1;
}

double EquilibriumBoundary::getVelocity() {
    return velocity;
}

double EquilibriumBoundary::getPressure() {
    return pressure;
}

void EquilibriumBoundary::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "equilibrium");
    writer.writeAttribute("pressure", QString::number(pressure));
    writer.writeAttribute("velocity", QString::number(velocity));
}

void EquilibriumBoundary::activate(QXmlStreamReader &reader, Grid*) {
    pressure = reader.attributes().value("pressure").toString().toDouble();
    velocity = reader.attributes().value("velocity").toString().toDouble();
}

void EquilibriumBoundary::setNextF(int, double, int) {

}

void EquilibriumBoundary::setMultiComponentIndex(int multiComponentIndex) {
    this->multiComponentIndex = multiComponentIndex;
}

void EquilibriumBoundary::setMulticomponent(int) {
}

double EquilibriumBoundary::getP(int) {
    return 0;
}

MyVector3D EquilibriumBoundary::getU(int) {
    return MyVector3D();
}

double EquilibriumBoundary::deltaP() {
    return 0;
}

double EquilibriumBoundary::getLastDeltaP() {
    return 0;
}

double EquilibriumBoundary::getNextF(int, int) {
    return 0;
}

MyVector3D EquilibriumBoundary::getForcesVelocity(int) {
    return MyVector3D();
}

bool EquilibriumBoundary::isFluid() {
    return false;
}
