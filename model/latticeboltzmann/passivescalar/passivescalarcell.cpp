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

#include "passivescalarcell.h"
#include "passivescalarsingleton.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/math/vector3i.h"
#include "../../../model/math/myvector3d.h"
#include "../../../model/util/shared.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/gridsimulation.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/boundary/opencell.h"
#include "../../../model/latticeboltzmann/boundary/reflectwithfactorcell.h"
#include "../../../model/latticeboltzmann/nullcell.h"
#include <QDebug>
#include <QXmlStreamWriter>

PassiveScalarCell::PassiveScalarCell(BaseCell *cell, double fixedConcentration, int index) : cell(cell), fixedConcentration(fixedConcentration), index(index) {
    f = 0;
    nextF = 0;
    p = 0;
    initializeDistribution();
}

BaseCell* PassiveScalarCell::getCell() {
    return cell;
}

PassiveScalarCell::~PassiveScalarCell() {
    clearMemory();
}

void PassiveScalarCell::clearMemory() {
    if (f != 0) {
        delete[] f;
        f = 0;
    }
    if (nextF != 0) {
        delete[] nextF;
        nextF = 0;
    }
    if (p != 0) {
        delete[] p;
        p = 0;
    }
}

void PassiveScalarCell::initializeDistribution() {
    clearMemory();
    int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
    int model = Shared::instance()->getGridConfig()->getModel();
    f = new double[colors * model];
    for (int i = 0; i < colors * model; i++) {
        f[i] = 0;
    }
    nextF = new double[colors * model];
    for (int i = 0; i < colors * model; i++) {
        nextF[i] = 0;
    }
    p = new double[colors];
    for (int i = 0; i < colors; i++) {
        p[i] = 0;
    }
}

void PassiveScalarCell::calc() {
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        double newP = 0;
        for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
            newP += f[a * Shared::instance()->getGridConfig()->getModel() + i];
        }
        p[a] = newP;
    }
}

void PassiveScalarCell::setFixedConcentration(double fixedConcentration) {
    this->fixedConcentration = fixedConcentration;
}

void PassiveScalarCell::setIndex(int index) {
    this->index = index;
}

void PassiveScalarCell::reset(int) {
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
            /*if (index == a) {
                f[a * Shared::instance()->getGridConfig()->getModel() + i] = LBUtil::f_eq2(MyVector3D(), fixedConcentration, Shared::instance()->getGridConfig()->getModel(), i);
            } else {*/
                f[a * Shared::instance()->getGridConfig()->getModel() + i] = LBUtil::f_eq2(MyVector3D(), PassiveScalarSingleton::getInstance()->getInitialConcentration(), Shared::instance()->getGridConfig()->getModel(), i);
            //}
        }
    }
    calc();
}

void PassiveScalarCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    cell->preUpdate(epsilon, neighbors, grid, position);
    if (dynamic_cast<NullCell*>(cell) == 0) {
        int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
        int model = grid->getConfig()->getModel();
        for (int a = 0; a < colors; a++) {
            if (fixedConcentration >= 0 && a == index) {
                for (int i = 0; i < model; i++) {
                    if (neighbors[i] != 0) {
                        neighbors[i]->setNextF(i, LBUtil::f_eq2(cell->getU(0), (PassiveScalarSingleton::getInstance()->getInjectionPeriod() == 0 || grid->getSimulation()->getIterations() % PassiveScalarSingleton::getInstance()->getInjectionPeriod() < PassiveScalarSingleton::getInstance()->getInjectionTime()) ? (fixedConcentration/* + PassiveScalarSingleton::getInstance()->getEpsilon(a) * (cell->getP(10 + a) - fixedConcentration)*/) : 0, model, i), 10 + a);
                    }
                }
            } else {
                WallCell *wall = dynamic_cast<WallCell*>(cell);
                if (wall == 0) {
                    double newEpsilon = PassiveScalarSingleton::getInstance()->getEpsilon(a);
                    for (int i = 0; i < model; i++) {
                        if (neighbors[i] != 0) {
                            //neighbors[i]->setNextF(i, f[a * model + i] * (1 - 1.0 / newEpsilon) + 1.0 / newEpsilon * (LBUtil::W[model][i] * p[a] * (1 + 3 * (LBUtil::C[model][i] * cell->getU(0)))), 10 + a);
                            neighbors[i]->setNextF(i, f[a * model + i] * (1 - 1.0 / newEpsilon) + 1.0 / newEpsilon * LBUtil::f_eq2(cell->getU(0), p[a], model, i), 10 + a);
                        }
                    }
                }
            }
        }
    }
}

void PassiveScalarCell::preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    cell->preUpdate2(epsilon, neighbors, grid, position);
    OpenCell *open = dynamic_cast<OpenCell*>(cell);
    if (open != 0) {
        BaseCell *empty = 0;
        int is[9];
        int m = Shared::instance()->getGridConfig()->getModel();
        switch (m) {
        case 9:
            if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 2; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
                empty = neighbors[5];
                is[0] = 4; is[1] = 5; is[2] = 6; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 3; is[2] = 4; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
                empty = neighbors[7];
                is[0] = 6; is[1] = 7; is[2] = 8; is[3] = -1; is[4] = -1; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            }
            break;
        case 15:
            if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[1]->isFluid())) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 6; is[2] = 9; is[3] = 11; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[3]->isFluid())) {
                empty = neighbors[5];
                is[0] = 3; is[1] = 7; is[2] = 8; is[3] = 12; is[4] = 13; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[4] == 0 || (!neighbors[4]->isFluid() && neighbors[2]->isFluid())) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 6; is[2] = 7; is[3] = 11; is[4] = 12; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[2] == 0 || (!neighbors[2]->isFluid() && neighbors[4]->isFluid())) {
                empty = neighbors[7];
                is[0] = 4; is[1] = 8; is[2] = 9; is[3] = 13; is[4] = 14; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            }
            break;
        case 19:
            if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 15; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
                empty = neighbors[5];
                is[0] = 4; is[1] = 5; is[2] = 6; is[3] = 12; is[4] = 17; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 3; is[2] = 4; is[3] = 11; is[4] = 16; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
                empty = neighbors[7];
                is[0] = 6; is[1] = 7; is[2] = 8; is[3] = 13; is[4] = 18; is[5] = -1; is[6] = -1; is[7] = -1; is[8] = -1;
            }
            break;
        case 27:
            if (neighbors[5] == 0 || (!neighbors[5]->isFluid() && neighbors[1]->isFluid())) {
                empty = neighbors[1];
                is[0] = 1; is[1] = 2; is[2] = 8; is[3] = 10; is[4] = 11; is[5] = 17; is[6] = 19; is[7] = 20; is[8] = 26;
            } else if (neighbors[1] == 0 || (!neighbors[1]->isFluid() && neighbors[5]->isFluid())) {
                empty = neighbors[5];
                is[0] = 4; is[1] = 5; is[2] = 6; is[3] = 13; is[4] = 14; is[5] = 15; is[6] = 22; is[7] = 23; is[8] = 24;
            } else if (neighbors[7] == 0 || (!neighbors[7]->isFluid() && neighbors[3]->isFluid())) {
                empty = neighbors[3];
                is[0] = 2; is[1] = 3; is[2] = 4; is[3] = 11; is[4] = 12; is[5] = 13; is[6] = 20; is[7] = 21; is[8] = 22;
            } else if (neighbors[3] == 0 || (!neighbors[3]->isFluid() && neighbors[7]->isFluid())) {
                empty = neighbors[7];
                is[0] = 6; is[1] = 7; is[2] = 8; is[3] = 15; is[4] = 16; is[5] = 17; is[6] = 24; is[7] = 25; is[8] = 26;
            }
            break;
        }
        if (empty != 0) {
            for (int i = 0; i < 9; i++) {
                if (is[i] != -1) {
                    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
                        empty->setNextF(is[i], LBUtil::f_eq2(empty->getU(0), empty->getP(10 + a), Shared::instance()->getGridConfig()->getModel(), is[i]), 10 + a);
                    }
                }
            }
        }
    }
}

void PassiveScalarCell::update() {
    cell->update();
    if (dynamic_cast<NullCell*>(cell) == 0) {
        int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
        int model = Shared::instance()->getGridConfig()->getModel();
        for (int a = 0; a < colors; a++) {
            for (int i = 0; i < model; i++) {
                f[a * model + i] = nextF[a * model + i];
            }
        }
        calc();
    }
}

void PassiveScalarCell::postUpdate(Grid *grid, Vector3i position) {
    cell->postUpdate(grid, position);
}

void PassiveScalarCell::reset(double p0, double p1) {
    cell->reset(p0, p1);
}

double PassiveScalarCell::getF(int index, int component) {
    return component < 10 ? cell->getF(index, component) : f[(component - 10) * Shared::instance()->getGridConfig()->getModel() + index];
}

void PassiveScalarCell::setNextF(int index, double value, int component) {
    if (component < 10) {
        cell->setNextF(index, value, component);
    } else {
        WallCell *wall = dynamic_cast<WallCell*>(cell);
        if (wall != 0 && index != 0) {
            double factor = 1;
            if (dynamic_cast<ReflectWithFactorCell*>(wall) != 0) {
                factor = Shared::instance()->getGridConfig()->getReflectWithFactor();
            }
            int model = Shared::instance()->getGridConfig()->getModel();
            BaseCell** neighbors = wall->getNeighbors();
            if (neighbors[LBUtil::OPPOSITE[model][index]] != 0) {
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(neighbors[LBUtil::OPPOSITE[model][index]]);
                if (psc != 0) {
                    WallCell *wall2 = dynamic_cast<WallCell*>(psc->getCell());
                    if (wall2 == 0) {
                        neighbors[LBUtil::OPPOSITE[model][index]]->setNextF(LBUtil::OPPOSITE[model][index], value * factor, component);
                    }
                }
            }
        } else {
            nextF[(component - 10) * Shared::instance()->getGridConfig()->getModel() + index] = value;
        }
    }
}

void PassiveScalarCell::setMulticomponent(int multicomponent) {
    cell->setMulticomponent(multicomponent);
}

void PassiveScalarCell::passivate(QXmlStreamWriter &writer) {
    int model = Shared::instance()->getGridConfig()->getModel();
    writer.writeAttribute("type", "passiveScalarCell");
    writer.writeAttribute("fixedConcentration", QString::number(fixedConcentration));
    writer.writeAttribute("index", QString::number(index));
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        for (int i = 0; i < model; i++) {
            writer.writeAttribute(QString("f").append(QString::number(a * model + i)), QString::number(f[a * model + i]));
        }
    }
    writer.writeStartElement("cell");
    cell->passivate(writer);
    writer.writeEndElement();
}

void PassiveScalarCell::activate(QXmlStreamReader &reader, Grid *grid) {
    int model = Shared::instance()->getGridConfig()->getModel();
    fixedConcentration = reader.attributes().value("fixedConcentration").toString().toDouble();
    index = reader.attributes().value("index").toString().toInt();
    initializeDistribution();
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        for (int i = 0; i < model; i++) {
            f[a * model + i] = reader.attributes().value(QString("f").append(QString::number(a * model + i))).toString().toDouble();
        }
    }
    calc();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    cell = grid->activateCell(reader);
    cell->activate(reader, grid);
}

double PassiveScalarCell::getP(int index) {
    if (index < 10) {
        return cell->getP(index);
    } else {
        return p[index - 10];
    }
}

MyVector3D PassiveScalarCell::getU(int index) {
    return cell->getU(index);
}

double PassiveScalarCell::deltaP() {
    return cell->deltaP();
}

double PassiveScalarCell::getLastDeltaP() {
    return cell->getLastDeltaP();
}

double PassiveScalarCell::getNextF(int index, int component) {
    return cell->getNextF(index, component);
}

MyVector3D PassiveScalarCell::getForcesVelocity(int component) {
    return cell->getForcesVelocity(component);
}

bool PassiveScalarCell::isFluid() {
    return cell->isFluid();
}

int PassiveScalarCell::getOpenCLType() {
    if (fixedConcentration >= 0) {
        return -(index + 1);
    }
    return cell->getOpenCLType();
}

void PassiveScalarCell::updateFromOpenCL(double *data) {
    cell->updateFromOpenCL(data);
    int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int a = 0; a < colors; a++) {
        for (int i = 0; i < model; i++) {
            f[a * model + i] = data[model + a * model + i];
        }
    }
    calc();
}
