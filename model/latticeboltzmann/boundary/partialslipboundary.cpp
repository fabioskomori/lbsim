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

#include "partialslipboundary.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

int PartialSlipBoundary::getOpenCLType() {
    return -1;
}

PartialSlipBoundary::PartialSlipBoundary() {
}

//TODO2: implement partial slip in 3d
void PartialSlipBoundary::preUpdate(double[], BaseCell* neighbors[], Grid*, Vector3i) {
    GridConfig *config = Shared::instance()->getGridConfig();
    if (neighbors[1] != 0 && neighbors[1]->isFluid()) {
        neighbors[2]->setNextF(2, neighbors[2]->getF(6, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[8]->getF(4, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[8]->setNextF(8, neighbors[8]->getF(4, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[2]->getF(6, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[1]->setNextF(1, neighbors[1]->getF(5, 0), 0);
    }
    if (neighbors[3] != 0 && neighbors[3]->isFluid()) {
        neighbors[4]->setNextF(4, neighbors[4]->getF(8, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[2]->getF(6, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[2]->setNextF(2, neighbors[2]->getF(6, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[4]->getF(8, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[3]->setNextF(3, neighbors[3]->getF(7, 0), 0);
    }
    if (neighbors[5] != 0 && neighbors[5]->isFluid()) {
        neighbors[4]->setNextF(4, neighbors[4]->getF(8, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[6]->getF(2, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[6]->setNextF(6, neighbors[6]->getF(2, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[4]->getF(8, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[5]->setNextF(5, neighbors[5]->getF(1, 0), 0);
    }
    if (neighbors[7] != 0 && neighbors[7]->isFluid()) {
        neighbors[6]->setNextF(6, neighbors[6]->getF(2, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[8]->getF(4, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[8]->setNextF(8, neighbors[8]->getF(4, 0) * (1 - config->getSlipWallCoefficient()) + neighbors[6]->getF(2, 0) * config->getSlipWallCoefficient(), 0);
        neighbors[7]->setNextF(7, neighbors[7]->getF(3, 0), 0);
    }
}

void PartialSlipBoundary::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void PartialSlipBoundary::update() {
}

void PartialSlipBoundary::postUpdate(Grid*, Vector3i) {
}

void PartialSlipBoundary::reset(double, double) {
}

double PartialSlipBoundary::getF(int) {
    return 0;
}

void PartialSlipBoundary::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "partialSlip");
}

void PartialSlipBoundary::setMulticomponent(int) {
}

double PartialSlipBoundary::getF(int, int) {
    return 0;
}

double PartialSlipBoundary::getF0(int, double, BaseCell*[], int) {
    return 0;
}

void PartialSlipBoundary::setNextF(int, double, int) {
}

void PartialSlipBoundary::injectNeighbors(BaseCell**) {
}

double PartialSlipBoundary::getP(int) {
    return 0;
}

MyVector3D PartialSlipBoundary::getU(int) {
    return MyVector3D();
}

double PartialSlipBoundary::deltaP() {
    return 0;
}

double PartialSlipBoundary::getLastDeltaP() {
    return 0;
}

double PartialSlipBoundary::getNextF(int, int) {
    return 0;
}

MyVector3D PartialSlipBoundary::getForcesVelocity(int) {
    return MyVector3D();
}

bool PartialSlipBoundary::isFluid() {
    return false;
}
