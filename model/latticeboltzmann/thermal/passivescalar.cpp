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

#include "passivescalar.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/util/shared.h"

int PassiveScalar::getOpenCLType() {
    return 0;
}

void PassiveScalar::updateFromOpenCL(double *data) {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        f[i] = data[i];
        g[i] = data[model + i];
    }
    calc();
}

BaseCell* PassiveScalar::clone() {
    PassiveScalar *result = new PassiveScalar();
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        result->f[i] = f[i];
        result->g[i] = g[i];
    }
    result->lastDeltaP = lastDeltaP;
    result->calc();
    return result;
}

void PassiveScalar::config(double p1, MyVector3D v, double p2) {
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        f[i] = LBUtil::f_eq(v, p1, Shared::instance()->getGridConfig()->getModel(), i);
        g[i] = LBUtil::f_eq(MyVector3D(), p2, Shared::instance()->getGridConfig()->getModel(), i);
    }
    calc();
}

PassiveScalar::PassiveScalar() {
    f = new double[Shared::instance()->getGridConfig()->getModel()];
    g = new double[Shared::instance()->getGridConfig()->getModel()];
    nextF = new double[Shared::instance()->getGridConfig()->getModel()];
    nextG = new double[Shared::instance()->getGridConfig()->getModel()];
    reset(1, 0);
}

PassiveScalar::~PassiveScalar() {
    delete[] f;
    delete[] g;
    delete[] nextF;
    delete[] nextG;
}

void PassiveScalar::calc() {
    p[0] = p[1] = 0;
    u = MyVector3D();
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        p[0] += f[i];
        p[1] += g[i];
        u = u + (LBUtil::C[Shared::instance()->getGridConfig()->getModel()][i] ^ f[i]);
    }
    u = u ^ (1.0 / p[0]);
}

void PassiveScalar::preUpdate(double[], BaseCell* neighbors[], Grid *grid, Vector3i) {
    //MyVector3D g2 = MyVector3D(0, -grid->getConfig()->getPassiveScalarGravity(), 0) ^ (epsilon[0] * (p[0] + p[1]) / p[0]);
    MyVector3D g2 = MyVector3D(0, -grid->getConfig()->getPassiveScalarGravity(), 0) ^ (grid->getConfig()->getNanoBeta() * grid->getConfig()->getNanoEpsilon0() * (p[1]) / (p[0] * grid->getConfig()->getNanoDensity()));
    MyVector3D u2 = u + g2;
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        neighbors[i]->setNextF(i, g[i] * (1 - 1.0 / grid->getConfig()->getNanoEpsilon1()) + 1.0 / grid->getConfig()->getNanoEpsilon1() * (LBUtil::W[model][i] * p[1] * (1 + 3 * (LBUtil::C[model][i] * u2))), 1);
        neighbors[i]->setNextF(i, f[i] * (1 - 1.0 / grid->getConfig()->getNanoEpsilon0()) + 1.0 / grid->getConfig()->getNanoEpsilon0() * LBUtil::f_eq(u2, p[0], model, i), 0);
    }
}

void PassiveScalar::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {

}

void PassiveScalar::update() {
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        f[i] = nextF[i];
        g[i] = nextG[i];
    }
    calc();
}

void PassiveScalar::postUpdate(Grid*, Vector3i) {

}

void PassiveScalar::reset(double newP0, double newP1) {
    if (newP0 >= 0) {
        p0[0] = newP0;
    }
    if (newP1 >= 0) {
        p0[1] = newP1;
    }
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        f[i] = LBUtil::f_eq(MyVector3D(), p0[0], model, i);
        g[i] = LBUtil::f_eq(MyVector3D(), p0[1], model, i);
    }
    calc();
}

double PassiveScalar::getF(int index, int component) {
    switch (component) {
    case 0:
        return f[index];
    case 1:
        return g[index];
    }
    return 0;
}

void PassiveScalar::setNextF(int index, double value, int component) {
    switch (component) {
    case 0:
        nextF[index] = value;
        break;
    case 1:
        nextG[index] = value;
        break;
    }
}

void PassiveScalar::setMulticomponent(int) {

}

void PassiveScalar::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "passivescalar");
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        writer.writeAttribute(QString("f").append(QString::number(i)), QString::number(f[i]));
        writer.writeAttribute(QString("g").append(QString::number(i)), QString::number(g[i]));
    }
    writer.writeAttribute("p00", QString::number(p0[0]));
    writer.writeAttribute("p01", QString::number(p0[1]));
}

void PassiveScalar::activate(QXmlStreamReader &reader, Grid *) {
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        f[i] = reader.attributes().value(QString("f").append(QString::number(i))).toString().toDouble();
        g[i] = reader.attributes().value(QString("g").append(QString::number(i))).toString().toDouble();
    }
    p0[0] = reader.attributes().value("p00").toString().toDouble();
    p0[1] = reader.attributes().value("p01").toString().toDouble();
    calc();
}

double PassiveScalar::getP(int index) {
    if (index == 0 || index == 1) {
        return p[index];
    }
    return p[0] + p[1];
}

MyVector3D PassiveScalar::getU(int) {
    return u;
}

double PassiveScalar::deltaP() {
    double result0 = 0, result1 = 0;
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        result0 += nextF[i];
        result1 += nextG[i];
    }
    lastDeltaP = ((result0 - p[0]) >= 0 ? result0 - p[0] : p[0] - result0) + ((result1 - p[1]) >= 0 ? result1 - p[1] : p[1] - result1);
    return lastDeltaP;
}

double PassiveScalar::getLastDeltaP() {
    return lastDeltaP;
}

double PassiveScalar::getNextF(int index, int component) {
    if (component == 1) {
        return nextG[index];
    }
    return nextF[index];
}

MyVector3D PassiveScalar::getForcesVelocity(int) {
    return MyVector3D();
}

bool PassiveScalar::isFluid() {
    return true;
}
