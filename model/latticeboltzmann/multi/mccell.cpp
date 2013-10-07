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

#include "mccell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/force/multicomponent.h"
#include "../../../model/latticeboltzmann/sccell.h"
#include "../../../model/latticeboltzmann/porouscell.h"
#include <typeinfo>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

int MCCell::getOpenCLType() {
    return 0;
}

MCCell::MCCell(SCCell *from) : constantConcentration(0), forcesVelocity(0), epsilons(0) {
    config(2, from);
    reset(-1, -1);
    compositeVelocity = MyVector3D();
    force = new MultiComponent();
}

MCCell::~MCCell() {
    delete force;
    delete[] forcesVelocity;
    delete[] epsilons;
    delete[] constantConcentration;
}

void MCCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    for (int comp = 0; comp < lattices.size(); comp++) {
        forcesVelocity[comp] = MyVector3D();
        double p = getLattice(comp)->getP(-1);
        if (p > 1e-3) {
            forcesVelocity[comp] = (force->computeForce(this, neighbors, comp, position.getX(), position.getY(), position.getZ(), grid) ^ (epsilon[comp] / p));
            SCCell *cell = getLattice(comp);
            std::list<Force*> *forces = Shared::instance()->getGridConfig()->getForces();
            for (std::list<Force*>::iterator force2 = forces->begin(); force2 != forces->end(); force2++) {
                forcesVelocity[comp] = forcesVelocity[comp] + ((*force2)->computeForce(cell, neighbors, comp, position.getX(), position.getY(), position.getZ(), grid) ^ (epsilon[comp] / p));
            }
        }
    }
    GridConfig *gridConfig = Shared::instance()->getGridConfig();
    for (int i = 0; i < gridConfig->getModel(); i++) {
        BaseCell *neighbor = neighbors[i];
        if (neighbor != 0) {
            for (unsigned int j = 0; j < lattices.size(); j++) {
                neighbor->setNextF(i, (1 - 1 / epsilon[j]) * getF(i, j) + 1 / epsilon[j] * getF0(i, epsilon[j], neighbors, j), j);
            }
        } else {
            qWarning() << "multicomponent error";
        }
    }
    for (int i = 0; i < lattices.size(); i++) {
        epsilons[i] = epsilon[i];
    }
}

void MCCell::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void MCCell::update() {
    int index = 0;
    for (std::list<SCCell*>::iterator lattice = lattices.begin(); lattice != lattices.end(); lattice++) {
        (*lattice)->update();
        if (constantConcentration[index] != 0) {
            (*lattice)->reset(constantConcentration[index], 0);
        }
        index++;
    }
    //calcCompositeVelocity();
}

void MCCell::setF(int, double) {
}

void MCCell::postUpdate(Grid *grid, Vector3i v) {
    for (std::list<SCCell*>::iterator lattice = lattices.begin(); lattice != lattices.end(); lattice++) {
        (*lattice)->postUpdate(grid, v);
    }
    calcCompositeVelocity();
}

void MCCell::calcCompositeVelocity() {
    int index = 0;
    double densitySum = 0;
    compositeVelocity = MyVector3D();
    int model = Shared::instance()->getGridConfig()->getModel();
    for (std::list<SCCell*>::iterator lattice = lattices.begin(); lattice != lattices.end(); lattice++) {
        MyVector3D fe = MyVector3D();
        for (int i = 0; i < model; i++) {
            if (LBUtil::W[model][i] > 1e-100) {
                fe = fe + (LBUtil::C[model][i] ^ (*lattice)->getF(i, -1));
            }
        }
        compositeVelocity = compositeVelocity + (fe ^ (1.0 / epsilons[index]));
        densitySum += (*lattice)->getP(-1) / epsilons[index];
        index++;
    }
    if (densitySum > 1e-100) {
        compositeVelocity = compositeVelocity ^ (1.0 / densitySum);
    } else {
        compositeVelocity = MyVector3D();
    }
}

void MCCell::reset(double p1, double p2) {
    compositeVelocity = MyVector3D();
    getLattice(0)->reset(p1, -1);
    getLattice(1)->reset(p2, -1);
}

double MCCell::getNextF(int, int) {
    return 0;
}

double MCCell::getF(int index) {
    double result = 0;
    for (std::list<SCCell*>::iterator lattice = lattices.begin(); lattice != lattices.end(); lattice++) {
        result += (*lattice)->getF(index, -1);
    }
    return result;
}

double MCCell::getF0(int, double, BaseCell*[]) {
    return 0;
}

double MCCell::getF(int index, int component) {
    return getLattice(component)->getF(index, -1);
}

double MCCell::getF0(int index, double , BaseCell*[], int component) {
    int model = Shared::instance()->getGridConfig()->getModel();
    GridConfig *gridConfig = Shared::instance()->getGridConfig();
    if (LBUtil::W[model][index] > 1e-100) {
        MyVector3D u2 = compositeVelocity + forcesVelocity[component];
        double scalar = LBUtil::C[model][index] * u2;
        return getLattice(component)->getP(-1) * LBUtil::W[gridConfig->getModel()][index] * (1 + 3 * scalar +
                4.5 * scalar * scalar - 1.5 * (u2 * u2));
    }
    return 0;
}

double MCCell::deltaP() {
    /*lastDeltaP = 0;
    for (std::list<SCCell*>::iterator lattice = lattices.begin(); lattice != lattices.end(); lattice++) {
        lastDeltaP += (*lattice)->getLastDeltaP();
    }
    lastDeltaP /= lattices.size();
    return lastDeltaP;*/
    return 0;
}

double MCCell::getP(int index) {
    if (index < 0) {
        double sumP = 0;
        int numP = 0;
        for (std::list<SCCell*>::iterator lattice = lattices.begin(); lattice != lattices.end(); lattice++) {
            sumP += (*lattice)->getP(-1);
            numP++;
        }
        return sumP / numP;
    }
    SCCell *cell = getLattice(index);
    if (cell == 0) return 0;
    return cell->getP(-1);
}

MyVector3D MCCell::getU(int index) {
    if (index < 0) {
        return compositeVelocity;
    }
    SCCell *cell = getLattice(index);
    if (cell == 0) return MyVector3D();
    return getLattice(index)->getU(-1);
}

double MCCell::getLastDeltaP() {
    return lastDeltaP;
}

void MCCell::config(unsigned int quantity, SCCell *from) {
    while (lattices.size() != quantity) {
        if (lattices.size() < quantity) {
            SCCell *lattice = 0;
            if (from != 0) {
                lattice = dynamic_cast<SCCell*>(from->clone());
            } else {
                lattice = new SCCell(Shared::instance()->getGridConfig()->randomP());
            }
            PorousCell *porous = dynamic_cast<PorousCell*>(lattice);
            if (porous != 0) {
                porous->setIndex(lattices.size());
            }
            lattices.push_back(lattice);
        } else {
            lattices.pop_back();
        }
    }
    if (constantConcentration != 0) {
        delete[] constantConcentration;
    }
    if (forcesVelocity != 0) {
        delete[] forcesVelocity;
    }
    if (epsilons != 0) {
        delete[] epsilons;
    }
    constantConcentration = new double[lattices.size()];
    forcesVelocity = new MyVector3D[lattices.size()];
    epsilons = new double[lattices.size()];
    for (int i = 0; i < lattices.size(); i++) {
        constantConcentration[i] = 0;
    }
}

MyVector3D MCCell::getCompositeVelocity() {
    return compositeVelocity;
}

void MCCell::setNextF(int index, double value, int component) {
    SCCell *lattice = getLattice(component);
    if (lattice != 0) {
        lattice->setNextF(index, value, -1);
    }
}

void MCCell::activate(QXmlStreamReader &reader, Grid *grid) {
    compositeVelocity = MyVector3D(reader.attributes().value("compVelX").toString().toDouble(), reader.attributes().value("compVelY").toString().toDouble(), reader.attributes().value("compVelZ").toString().toDouble());
    lastDeltaP = reader.attributes().value("lastDeltaP").toString().toDouble();
    lattices.clear();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    int size = reader.attributes().value("cellsSize").toString().toInt();
    for (int i = 0; i < size; i++) {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        SCCell *cell = dynamic_cast<SCCell*>(grid->activateCell(reader));
        cell->activate(reader, grid);
        lattices.push_back(cell);
    }
}

void MCCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "multiComponent");
    writer.writeAttribute("compVelX", QString::number(compositeVelocity.getX()));
    writer.writeAttribute("compVelY", QString::number(compositeVelocity.getY()));
    writer.writeAttribute("compVelZ", QString::number(compositeVelocity.getZ()));
    writer.writeAttribute("lastDeltaP", QString::number(lastDeltaP));
    writer.writeStartElement("cells");
    writer.writeAttribute("cellsSize", QString::number(lattices.size()));
    for (std::list<SCCell*>::iterator cell = lattices.begin(); cell != lattices.end(); cell++) {
        writer.writeStartElement("cell");
        (*cell)->passivate(writer);
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

SCCell* MCCell::getLattice(int index) {
    std::list<SCCell*>::iterator lattice = lattices.begin();
    for (int i = 0; i < index; i++) {
        lattice++;
        if (lattice == lattices.end()) {
            return 0;
        }
    }
    return *lattice;
}

void MCCell::setMulticomponent(int quantity) {
    config(quantity);
}

MultiComponent* MCCell::getMultiForce() {
    return force;
}

MyVector3D MCCell::getForcesVelocity(int component) {
    if (component == -1) {
        return forcesVelocity[0] + forcesVelocity[1];
    }
    return forcesVelocity[component];
}

void MCCell::updateFromOpenCL(double *data) {
    int model = Shared::instance()->getGridConfig()->getModel();
    getLattice(0)->updateFromOpenCL(data);
    getLattice(1)->updateFromOpenCL(&data[model]);
    calcCompositeVelocity();
}

bool MCCell::isFluid() {
    return true;
}

void MCCell::setConstantConcentration(int index, double value) {
    constantConcentration[index] = value;
}
