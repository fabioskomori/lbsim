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

#include "movingcell.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/sccell.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "movingparticle.h"
#include "movingwall.h"
#include <typeinfo>
#include <QDebug>
#include "../../../model/util/shared.h"

int MovingCell::getOpenCLType() {
    return 4;
}

MovingCell::MovingCell(double p0, int charge, Grid *grid) {
    nextF = new double[grid->getConfig()->getModel()];
    this->charge = charge;
    this->grid = grid;
    particle = 0;
    this->p0 = p0;
    reset(-1, -1);
}

BaseCell* MovingCell::clone() {
    MovingCell* result = new MovingCell(p0, charge, grid);
    result->setParticle(particle->clone());
    return result;
}

MovingCell::~MovingCell() {
    delete nextF;
}

SomethingMoving* MovingCell::getParticle() {
    return particle;
}

void MovingCell::setParticle(SomethingMoving *particle) {
    this->particle = particle;
}

void MovingCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    particle->preUpdate(epsilon, neighbors, grid, position);
}

void MovingCell::preUpdate2(double[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    MyVector3D d = particle->getD();
    for (int i = 1; i < grid->getConfig()->getModel(); i++) {
        Vector3i neighbor = position + LBUtil::C[grid->getConfig()->getModel()][i];
        BaseCell *cell = grid->getGrid(neighbor.getY(), neighbor.getX(), neighbor.getZ());
        if (cell != 0 && cell->isFluid()) {
            //double delta = 0.5 + (d * LBUtil::C[grid->getConfig()->getModel()][LBUtil::OPPOSITE[grid->getConfig()->getModel()][i]]);
            //double faw = cell->getNextF(LBUtil::OPPOSITE[grid->getConfig()->getModel()][i], 0) + delta * (nextF[LBUtil::OPPOSITE[grid->getConfig()->getModel()][i]] - cell->getNextF(LBUtil::OPPOSITE[grid->getConfig()->getModel()][i], 0));
            double faw = nextF[LBUtil::OPPOSITE[grid->getConfig()->getModel()][i]];
            faw += 6 * LBUtil::W[Shared::instance()->getGridConfig()->getModel()][LBUtil::OPPOSITE[grid->getConfig()->getModel()][i]] * cell->getP(-1) * (particle->getVelocity() * LBUtil::C[grid->getConfig()->getModel()][i]);
            double nextF = faw;
            /*Vector3i newPosition = position + (LBUtil::C[grid->getConfig()->getModel()][i] ^ 2);
            BaseCell *secondNeighbor = grid->getGrid(newPosition.getY(), newPosition.getX(), newPosition.getZ());
            if (secondNeighbor != 0 && secondNeighbor->isFluid()) {
                nextF += (delta / (1 + delta)) * (secondNeighbor->getNextF(i, 0) - faw);
            }*/
            cell->setNextF(i, nextF, -1);
        }
    }
    int mcQuantity, mcType;
    double mcG, mcGads1, mcGads2;
    grid->getConfig()->getMulticomponent(&mcQuantity, &mcG, &mcGads1, &mcGads2, &mcType);
    if (mcType == MC_PS) {
        BaseCell *empty = 0;
        int is[9];
        int m = Shared::instance()->getGridConfig()->getModel();
        switch (m) {
        case 9:
            if (neighbors[1] != 0 && neighbors[1]->isFluid()) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 2; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[5] != 0 && neighbors[5]->isFluid()) {
                empty = neighbors[5];
                is[0] = 4; is[1] = 5; is[2] = 6; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[3] != 0 && neighbors[3]->isFluid()) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 3; is[2] = 4; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[7] != 0 && neighbors[7]->isFluid()) {
                empty = neighbors[7];
                is[0] = 6; is[1] = 7; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            }
            break;
        case 15:
            if (neighbors[1] != 0 && neighbors[1]->isFluid()) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 6; is[2] = 9; is[3] = 11; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[3] != 0 && neighbors[3]->isFluid()) {
                empty = neighbors[5];
                is[0] = 3; is[1] = 7; is[2] = 8; is[3] = 12; is[4] = 13; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[2] != 0 && neighbors[2]->isFluid()) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 6; is[2] = 7; is[3] = 11; is[4] = 12; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[4] != 0 && neighbors[4]->isFluid()) {
                empty = neighbors[7];
                is[0] = 4; is[1] = 8; is[2] = 9; is[3] = 13; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            }
            break;
        case 19:
            if (neighbors[1] != 0 && neighbors[1]->isFluid()) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 15; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[5] != 0 && neighbors[5]->isFluid()) {
                empty = neighbors[5];
                is[0] = 4; is[1] = 5; is[2] = 6; is[3] = 12; is[4] = 17; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[3] != 0 && neighbors[3]->isFluid()) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 3; is[2] = 4; is[3] = 11; is[4] = 16; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[7] != 0 && neighbors[7]->isFluid()) {
                empty = neighbors[7];
                is[0] = 6; is[1] = 7; is[2] = 8; is[3] = 13; is[4] = 18; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            }
            break;
        case 27:
            if (neighbors[1] != 0 && neighbors[1]->isFluid()) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 11; is[5] = 17; is[6] = 19; is[7] = 20; is[8] = 26;
            } else if (neighbors[5] != 0 && neighbors[5]->isFluid()) {
                empty = neighbors[5];
                is[0] = 4; is[1] = 5; is[2] = 6; is[3] = 13; is[4] = 14; is[5] = 15; is[6] = 22; is[7] = 23; is[8] = 24;
            } else if (neighbors[3] != 0 && neighbors[3]->isFluid()) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 3; is[2] = 4; is[3] = 11; is[4] = 12; is[5] = 13; is[6] = 20; is[7] = 21; is[8] = 22;
            } else if (neighbors[7] != 0 && neighbors[7]->isFluid()) {
                empty = neighbors[7];
                is[0] = 6; is[1] = 7; is[2] = 8; is[3] = 15; is[4] = 16; is[5] = 17; is[6] = 24; is[7] = 25; is[8] = 26;
            }
            break;
        }
        if (empty != 0) {
            double residual = 0;
            double weightSum = 0;
            for (int i = 0; i < 9; i++) {
                if (is[i] != -1) {
                    residual += empty->getNextF(LBUtil::OPPOSITE[m][is[i]], 1);
                    weightSum += LBUtil::W[m][is[i]];
                }
            }
            residual /= weightSum;
            for (int i = 0; i < 9; i++) {
                if (is[i] != -1) {
                    empty->setNextF(is[i], LBUtil::W[m][is[i]] * residual, 1);
                }
            }
        }
    }
}

void MovingCell::update() {
    particle->update();
}

void MovingCell::postUpdate(Grid*, Vector3i) {
}

void MovingCell::reset(double, double) {
}

double MovingCell::getNextF(int index, int) {
    return nextF[index];
}

double MovingCell::getF(int, int) {
    return 0;
}

int MovingCell::getCharge() {
    return charge;
}

void MovingCell::activate(QXmlStreamReader &reader, Grid*) {
    charge = reader.attributes().value("charge").toString().toInt();
    p0 = reader.attributes().value("p0").toString().toDouble();
    for (int i = 0; i < grid->getConfig()->getModel(); i++) {
        nextF[i] = reader.attributes().value(QString("nextF").append(QString::number(i))).toString().toDouble();
    }
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    SomethingMoving *something = 0;
    if (reader.attributes().value("type").toString() == "wall") {
        something = new MovingWall(MyVector3D());
        if (something != 0) {
            something->activate(reader);
            particle = something;
        }
    } else if (reader.attributes().value("type").toString() == "particle") {
        particle = grid->getParticleManager()->getParticlesMap(reader.attributes().value("index").toString().toInt());
    } else {
        qWarning() << "moving cell error";
    }
}

void MovingCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "moving");
    writer.writeAttribute("charge", QString::number(charge));
    writer.writeAttribute("p0", QString::number(p0));
    for (int i = 0; i < grid->getConfig()->getModel(); i++) {
        writer.writeAttribute(QString("nextF").append(QString::number(i)), QString::number(nextF[i]));
    }
    writer.writeStartElement("something");
    MovingParticle *moving = dynamic_cast<MovingParticle*>(particle);
    if (moving != 0) {
        writer.writeAttribute("type", "particle");
        writer.writeAttribute("index", QString::number(grid->getParticleManager()->getParticlesMap(moving)));
    } else {
        particle->passivate(writer);
    }
    writer.writeEndElement();
}

void MovingCell::setNextF(int index, double value, int) {
    nextF[index] = value;
}

void MovingCell::setMulticomponent(int) {
}

double MovingCell::getP(int) {
    return 0;
}

MyVector3D MovingCell::getU(int) {
    return particle->getVelocity();
}

double MovingCell::deltaP() {
    return 0;
}

double MovingCell::getLastDeltaP() {
    return 0;
}

MyVector3D MovingCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool MovingCell::isFluid() {
    return false;
}
