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

#include "depositioncell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/math/vector3i.h"
#include "../../../model/math/myvector3d.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/deposition/depositionwall.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>

//TODO2: implement deposition in 3d

DepositionCell::DepositionCell() {
    deposited = 0;
    reset(0, 0);
    calcConcentration();
}

void DepositionCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i ) {
    double surroundingConcentration = 0;
    for (int i = 0; i < 9; i++) {
        BaseCell* neighbor = neighbors[i];
        if (neighbor != 0) {
            double newF = (1 - 1 / epsilon[1]) * getF(i, 1) + LBUtil::W[9][i] * concentration / epsilon[1];
            neighbor->setNextF(i, newF, 1);
            if (i != 0) {
                double neighborConcentration = 0;
                if (neighbor->isFluid()) {
                    neighborConcentration = neighbor->getP(0);
                } else if (dynamic_cast<DepositionWall*>(neighbor) != 0) {
                    neighborConcentration = 1;
                }
                surroundingConcentration += neighborConcentration * 9 / 5 * LBUtil::W[9][i];
            }
        }
    }
    deposited += surroundingConcentration * concentration * grid->getConfig()->getDepositionRate();
    if (deposited > 1) {
        deposited = 1;
    }
}

void DepositionCell::preUpdate2(double [], BaseCell* [], Grid *, Vector3i ) {
}

void DepositionCell::update() {
    for (int i = 0; i < 9; i++) {
        g[i] = nextG[i];
    }
    calcConcentration();
}

void DepositionCell::postUpdate(Grid *grid, Vector3i position) {
    for (int i = 0; i < 9; i++) {
        Vector3i neighbor = position + LBUtil::C[9][i];
        nextG[i] = g[i] + deposited * (grid->getGrid(neighbor.getY(), neighbor.getX(), neighbor.getZ())->getF(LBUtil::OPPOSITE[9][i], 1) - g[i]);
    }
    for (int i = 0; i < 9; i++) {
        g[i] = nextG[i];
    }
}

void DepositionCell::reset(double , double p) {
    deposited = 0;
    if (p >= 0) {
        initialConcentration = p;
    }
    LBUtil::f_eq(MyVector3D(), initialConcentration, 9, g);
    calcConcentration();
}

double DepositionCell::getF(int index, int component) {
    if (component == 1) {
        return g[index];
    }
    return 0;
}

void DepositionCell::setNextF(int index, double value, int component) {
    if (component == 1) {
        nextG[index] = value;
    }
}

void DepositionCell::setMulticomponent(int) {
}

void DepositionCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "deposition");
    writer.writeAttribute("initialConcentration", QString::number(initialConcentration));
    writer.writeAttribute("deposited", QString::number(deposited));
    for (int i = 0; i < 9; i++) {
        writer.writeAttribute(QString("g").append(QString::number(i)), QString::number(g[i]));
    }
}

void DepositionCell::activate(QXmlStreamReader &reader, Grid*) {
    initialConcentration = reader.attributes().value("initialConcentration").toString().toDouble();
    deposited = reader.attributes().value("deposited").toString().toDouble();
    for (int i = 0; i < 9; i++) {
        g[i] = reader.attributes().value(QString("g").append(QString::number(i))).toString().toDouble();
    }
    calcConcentration();
}

double DepositionCell::getP(int index) {
    switch (index) {
    case 0:
        return deposited;
    case 1:
        return concentration;
    }
    return 0;
}

MyVector3D DepositionCell::getU(int ) {
    return MyVector3D();
}

double DepositionCell::deltaP() {
    return 0;
}

double DepositionCell::getLastDeltaP() {
    return 0;
}

double DepositionCell::getNextF(int index, int component) {
    if (component == 1) {
        return nextG[index];
    }
    return 0;
}

MyVector3D DepositionCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool DepositionCell::isFluid() {
    return true;
}

int DepositionCell::getOpenCLType() {
    return -1;
}

void DepositionCell::updateFromOpenCL(double*) {
}

void DepositionCell::calcConcentration() {
    concentration = 0;
    for (int i = 0; i < 9; i++) {
        concentration += g[i];
    }
}
