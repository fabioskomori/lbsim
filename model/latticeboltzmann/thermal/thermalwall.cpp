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

#include "thermalwall.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

int ThermalWall::getOpenCLType() {
    switch (index) {
    case 0:
        return 5;
    case 1:
        return 6;
    }
    return -1;
}

ThermalWall::ThermalWall() {
    index = 0;
}

BaseCell* ThermalWall::clone() {
    ThermalWall *result = new ThermalWall();
    result->setIndex(index);
    return result;
}

void ThermalWall::preUpdate(double[], BaseCell*[], Grid*, Vector3i) {
}

void ThermalWall::preUpdate2(double[], BaseCell* neighbors[], Grid*, Vector3i) {
    int missing[3] = {0, 0, 0};
    if (neighbors[1] != 0 && neighbors[1]->isFluid()) {
        missing[0] = 2;
        missing[1] = 1;
        missing[2] = 8;
    } else if (neighbors[3] != 0 && neighbors[3]->isFluid()) {
        missing[0] = 2;
        missing[1] = 3;
        missing[2] = 4;
    } else if (neighbors[5] != 0 && neighbors[5]->isFluid()) {
        missing[0] = 4;
        missing[1] = 5;
        missing[2] = 6;
    } else if (neighbors[7] != 0 && neighbors[7]->isFluid()) {
        missing[0] = 6;
        missing[1] = 7;
        missing[2] = 8;
    } else {
        return;
    }
    double residual = 0;
    for (int i = 0; i < 9; i++) {
        if (i != missing[0] && i != missing[1] && i != missing[2]) {
            residual += neighbors[missing[1]]->getNextF(i, 1);
        }
    }
    residual = (Shared::instance()->getGridConfig()->getThermal(index) - residual) * 6;
    neighbors[missing[1]]->setNextF(missing[0], residual / 36, 1);
    neighbors[missing[1]]->setNextF(missing[1], residual / 9, 1);
    neighbors[missing[1]]->setNextF(missing[2], residual / 36, 1);
    /*int is[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    int m = Shared::instance()->getGridConfig()->getModel();
    switch (m) {
    case 9:
        if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
            is[0] = 1; is[1] = 2; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
            is[0] = 5; is[1] = 4; is[2] = 6; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
            is[0] = 3; is[1] = 2; is[2] = 4; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
            is[0] = 7; is[1] = 6; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        }
        break;
    case 15:
        if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[1]->isFluid())) {
            is[0] = 1; is[1] = 6; is[2] = 9; is[3] = 11; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[3]->isFluid())) {
            is[0] = 3; is[1] = 7; is[2] = 8; is[3] = 12; is[4] = 13; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[4] == 0 || (!neighbors[4]->isFluid() && neighbors[2]->isFluid())) {
            is[0] = 2; is[1] = 6; is[2] = 7; is[3] = 11; is[4] = 12; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[2] == 0 || (!neighbors[2]->isFluid() && neighbors[4]->isFluid())) {
            is[0] = 4; is[1] = 8; is[2] = 9; is[3] = 13; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        }
        break;
    case 19:
        if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
            is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 15; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
            is[0] = 5; is[1] = 4; is[2] = 6; is[3] = 12; is[4] = 17; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
            is[0] = 3; is[1] = 2; is[2] = 4; is[3] = 11; is[4] = 16; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
            is[0] = 7; is[1] = 6; is[2] = 8; is[3] = 13; is[4] = 18; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
        }
        break;
    case 27:
        if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
            is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 11; is[5] = 17; is[6] = 19; is[7] = 20; is[8] = 26;
        } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
            is[0] = 5; is[1] = 4; is[2] = 6; is[3] = 13; is[4] = 14; is[5] = 15; is[6] = 22; is[7] = 23; is[8] = 24;
        } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
            is[0] = 3; is[1] = 2; is[2] = 4; is[3] = 11; is[4] = 12; is[5] = 13; is[6] = 20; is[7] = 21; is[8] = 22;
        } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
            is[0] = 7; is[1] = 6; is[2] = 8; is[3] = 15; is[4] = 16; is[5] = 17; is[6] = 24; is[7] = 25; is[8] = 26;
        }
        break;
    }
    double residual = 0;
    for (int i = 0; i < m; i++) {
        bool equal = false;
        for (int a = 0; a < 9; a++) {
            if (i == is[a]) {
                equal = true;
            }
        }
        if (!equal) {
            residual += neighbors[is[0]]->getNextF(i, 1);
        }
    }
    double weightSum = 0;
    for (int i = 0; i < 9; i++) {
        if (is[i] != -1) {
            weightSum += LBUtil::W[m][is[i]];
        }
    }
    residual = (Shared::instance()->getGridConfig()->getThermal(index) - residual) / weightSum;
    for (int i = 0; i < 9; i++) {
        if (is[i] != -1) {
            neighbors[is[0]]->setNextF(is[i], residual * LBUtil::W[m][is[i]], 1);
        }
    }*/
}

void ThermalWall::update() {
}

void ThermalWall::postUpdate(Grid*, Vector3i) {
}

void ThermalWall::reset(double, double) {
}

double ThermalWall::getF(int, int) {
    return 0;
}

void ThermalWall::setNextF(int index, double value, int component) {
    int model = Shared::instance()->getGridConfig()->getModel();
    if (component == 0 && neighbors[LBUtil::OPPOSITE[model][index]] != 0) {
        neighbors[LBUtil::OPPOSITE[model][index]]->setNextF(LBUtil::OPPOSITE[model][index], value, component);
    }
}

void ThermalWall::setMulticomponent(int) {
}

void ThermalWall::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "thermalWall");
    writer.writeAttribute("index", QString::number(index));
}

void ThermalWall::activate(QXmlStreamReader &reader, Grid *) {
    index = reader.attributes().value("index").toString().toInt();
}

double ThermalWall::getP(int) {
    return 0;
}

MyVector3D ThermalWall::getU(int) {
    return MyVector3D();
}

double ThermalWall::deltaP() {
    return 0;
}

double ThermalWall::getLastDeltaP() {
    return 0;
}

double ThermalWall::getNextF(int, int) {
    return 0;
}

MyVector3D ThermalWall::getForcesVelocity(int) {
    return MyVector3D();
}

bool ThermalWall::isFluid() {
    return false;
}

void ThermalWall::injectNeighbors(BaseCell** neighbors) {
    this->neighbors = neighbors;
}

int ThermalWall::getIndex() {
    return index;
}

void ThermalWall::setIndex(int index) {
    this->index = index;
}
