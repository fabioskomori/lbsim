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

#include "shallowcell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QDebug>
#include <cmath>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

//TODO2: implement shallow cell in 3d

int ShallowCell::getOpenCLType() {
    return -1;
}

ShallowCell::ShallowCell() {
    reset(1, 1);
}

void ShallowCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i) {
    GridConfig *config = Shared::instance()->getGridConfig();
    int model = config->getModel();
    double feq[9];
    for (int i = 1; i <= 7; i += 2) {
        feq[i] = h * 1.0 / 3 * (config->getShallowG() * h / 2 + (LBUtil::C[model][i] * u) + 3 * (LBUtil::C[model][i] * u) * (LBUtil::C[model][i] * u) / 2 - (u * u) / 2);
    }
    for (int i = 2; i <= 8; i += 2) {
        feq[i] = h * 1.0 / 12 * (config->getShallowG() * h / 2 + (LBUtil::C[model][i] * u) + 3 * (LBUtil::C[model][i] * u) * (LBUtil::C[model][i] * u) / 2 - (u * u) / 2);
    }
    double sum = 0;
    for (int i = 1; i < 9; i++) {
        sum += feq[i];
    }
    feq[0] = h - sum;
    MyVector3D sfriction = u ^ (-grid->getConfig()->getShallowG() * grid->getConfig()->getShallowNB() * grid->getConfig()->getShallowNB() * (std::pow(h, -1.0 / 3)) * u.norm());
    for (int i = 0; i < 9; i++) {
        if (neighbors[i] != 0) {
            neighbors[i]->setNextF(i, f[i] + 1.0 / epsilon[0] * (feq[i] - f[i]) + (LBUtil::C[model][i] * sfriction) / 6, -1);
        }
    }
}

void ShallowCell::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void ShallowCell::update() {
    for (int i = 0; i < 9; i++) {
        f[i] = nextF[i];
    }
    calc();
}

void ShallowCell::postUpdate(Grid*, Vector3i) {
}

void ShallowCell::reset(double newP0, double) {
    GridConfig *config = Shared::instance()->getGridConfig();
    if (newP0 >= 0) {
        p0 = newP0;
    }
    for (int i = 1; i <= 7; i += 2) {
        f[i] = p0 * 1.0 / 3 * (config->getShallowG() * p0 / 2);
    }
    for (int i = 2; i <= 8; i += 2) {
        f[i] = p0 * 1.0 / 12 * (config->getShallowG() * p0 / 2);
    }
    double sum = 0;
    for (int i = 1; i < 9; i++) {
        sum += f[i];
    }
    f[0] = p0 - sum;
    calc();
}

double ShallowCell::getF(int index, int) {
    return f[index];
}

void ShallowCell::setNextF(int index, double value, int) {
    nextF[index] = value;
}

void ShallowCell::setMulticomponent(int) {
}

void ShallowCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "shallow");
    for (int i = 0; i < 9; i++) {
        writer.writeAttribute(QString("f").append(QString::number(i)), QString::number(f[i]));
    }
    writer.writeAttribute("p0", QString::number(p0));
}

void ShallowCell::activate(QXmlStreamReader &reader, Grid *) {
    for (int i = 0; i < 9; i++) {
        f[i] = reader.attributes().value(QString("f").append(QString::number(i))).toString().toDouble();
    }
    p0 = reader.attributes().value("p0").toString().toDouble();
    calc();
}

double ShallowCell::getP(int) {
    return h;
}

MyVector3D ShallowCell::getU(int) {
    return u;
}

void ShallowCell::calc() {
    h = 0;
    u = MyVector3D();
    for (int i = 0; i < 9; i++) {
        h += f[i];
        u = u + (LBUtil::C[Shared::instance()->getGridConfig()->getModel()][i] ^ f[i]);
    }
    u = u ^ (1.0 / h);
}

double ShallowCell::deltaP() {
    return 0;
}

double ShallowCell::getLastDeltaP() {
    return 0;
}

double ShallowCell::getNextF(int, int) {
    return 0;
}

MyVector3D ShallowCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool ShallowCell::isFluid() {
    return true;
}
