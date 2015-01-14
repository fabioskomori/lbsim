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

#include "sccell.h"
#include "basecell.h"
#include "lbutil.h"
#include "lbgrid.h"
#include "gridconfig.h"
#include "force/forcelist.h"
#include "force/force.h"
#include <typeinfo>
#include <QDebug>
#include <cmath>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../model/util/shared.h"

int SCCell::getOpenCLType() {
    return 0;
}

SCCell::SCCell(double p0) : p0(p0) {
    f = new double[Shared::instance()->getGridConfig()->getModel()];
    nextF = new double[Shared::instance()->getGridConfig()->getModel()];
    reset(p0, -1);
    constantConcentration = 0;
}

BaseCell* SCCell::clone() {
    SCCell *result = new SCCell(p0);
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        result->setF(i, f[i]);
    }
    return result;
}

SCCell::~SCCell() {
    delete [] f;
    delete [] nextF;
}

void SCCell::config(MyVector3D velocity, double p) {
    int model = Shared::instance()->getGridConfig()->getModel();
    LBUtil::f_eq(velocity, p, model, f);
    LBUtil::calc_pu(f, model, &p, &u);
}

void SCCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i pos) {
    forcesVelocity = MyVector3D();
    std::list<Force*>* forces = Shared::instance()->getGridConfig()->getForces();
    for (std::list<Force*>::iterator force = forces->begin(); force != forces->end(); force++) {
        MyVector3D du = ((*force)->computeForce(this, neighbors, 0, pos.getX(), pos.getY(), pos.getZ(), grid) ^ (epsilon[0] / p));
        //forcesVelocity = forcesVelocity + du.limit(0.1);
        forcesVelocity = forcesVelocity + du;
    }
    int model = Shared::instance()->getGridConfig()->getModel();
    double *cacheF0 = new double[model];
    for (int i = 0; i < model; i++) {
        cacheF0[i] = getF0(i, epsilon[0], neighbors, grid);
    }
    double h = 0, halfGamma = 1;
    if (grid->getConfig()->getEntropic()) {
        for (int i = 0; i < model; i++) {
            h += f[i] * std::log(f[i] / LBUtil::W[model][i]);
        }
        halfGamma = gamma(h, cacheF0, 0, 2, 0) / 2;
    }
    double tau = epsilon[0];
    if (grid->getConfig()->getSmagorinsky() > 0) {
        double Qxx = 0, Qxy = 0, Qyx = 0, Qyy = 0;
        for (int i = 0; i < model; i++) {
            Qxx += LBUtil::C[model][i].getX() * LBUtil::C[model][i].getX() * (f[i] - cacheF0[i]);
            Qxy += LBUtil::C[model][i].getX() * LBUtil::C[model][i].getY() * (f[i] - cacheF0[i]);
            Qyx += LBUtil::C[model][i].getY() * LBUtil::C[model][i].getX() * (f[i] - cacheF0[i]);
            Qyy += LBUtil::C[model][i].getY() * LBUtil::C[model][i].getY() * (f[i] - cacheF0[i]);
        }
        double Q = Qxx * Qxx + Qxy * Qxy + Qyx * Qyx + Qyy * Qyy;
        tau = 0.5 * (std::sqrt(tau * tau + model * grid->getConfig()->getSmagorinsky() * grid->getConfig()->getSmagorinsky() * std::sqrt(8 * Q) / p) + tau);
    }
    for (int i = 0; i < model; i++) {
        BaseCell* neighbor = neighbors[i];
        if (neighbor != 0) {
            double newF = (1 - halfGamma / tau) * getF(i, -1) + cacheF0[i] * halfGamma / tau;
            neighbor->setNextF(i, newF, 0);
        }
    }
    delete[] cacheF0;
    //LBUtil::update(model, epsilon, neighbors, nextF);
}

void SCCell::setForcesVelocity(MyVector3D forcesVelocity) {
    this->forcesVelocity = forcesVelocity;
}

void SCCell::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void SCCell::update() {
    if (constantConcentration == 0) {
        std::swap(f, nextF);
        LBUtil::calc_pu(f, Shared::instance()->getGridConfig()->getModel(), &p, &u);
    } else {
        reset(constantConcentration, 0);
    }
}

void SCCell::updateFromOpenCL(double *data) {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        f[i] = data[i];
    }
    LBUtil::calc_pu(f, model, &p, &u);
}

void SCCell::setF(int index, double value) {
    int model = Shared::instance()->getGridConfig()->getModel();
    f[index] = value;
    LBUtil::calc_pu(f, model, &p, &u);
}

void SCCell::postUpdate(Grid*, Vector3i) {
}

void SCCell::reset(double newP, double) {
    if (newP >= 0) {
        p0 = newP;
    }
    int model = Shared::instance()->getGridConfig()->getModel();
    LBUtil::f_eq(MyVector3D(), p0, model, f);
    LBUtil::calc_pu(f, model, &p, &u);
}

void SCCell::zero() {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        f[i] = 0;
    }
    LBUtil::calc_pu(f, model, &p, &u);
}

double SCCell::gamma(double h, double cacheF0[], double min, double max, int step) {
    double middle = (min + max) / 2;
    double newH = 0;
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        double newF = (1 - middle) * f[i] + middle * cacheF0[i];
        newH += newF * std::log(newF / LBUtil::W[model][i]);
    }
    step++;
    if (step >= 100 || std::abs((newH - h) / h) < 1e-5) {
        if (step >= 100) {
        }
        return middle;
    }
    return gamma(h, cacheF0, newH > h ? middle : min, newH < h ? max : middle, step);
}

double SCCell::getNextF(int index, int) {
    return nextF[index];
}

double SCCell::getF(int index, int) {
    return f[index];
}

double SCCell::getF0(int index, double , BaseCell*[], Grid *grid) {
    int model = Shared::instance()->getGridConfig()->getModel();
    if (LBUtil::W[model][index] > 1e-100) {
        MyVector3D u2 = u + forcesVelocity;
        double scalar = LBUtil::C[model][index] * u2;
        double factor = 1;
        if (grid->getConfig()->getVelocityOrder() > 0) {
            factor += 3 * scalar;
        }
        if (grid->getConfig()->getVelocityOrder() > 1) {
            factor += 4.5 * scalar * scalar - 1.5 * (u2 * u2);
        }
        return p * LBUtil::W[model][index] * factor;
    }
    return 0;
}

double SCCell::getP(int) {
    return p;
}

MyVector3D SCCell::getU(int) {
    return u;
}

double SCCell::deltaP() {
    double result = 0;
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        result += std::abs(nextF[i] - f[i]);
    }
    lastDeltaP = result / Shared::instance()->getGridConfig()->getModel();
    return lastDeltaP;
}

double SCCell::getLastDeltaP() {
    return lastDeltaP;
}

void SCCell::setNextF(int index, double value, int component) {
    if (component != 1) {
        nextF[index] = value;
    }
}

void SCCell::passivate(QXmlStreamWriter &writer) {
    int model = Shared::instance()->getGridConfig()->getModel();
    writer.writeAttribute("type", "singleComponent");
    writer.writeAttribute("p0", QString::number(p0));
    writer.writeAttribute("lastDeltaP", QString::number(lastDeltaP));
    for (int i = 0; i < model; i++) {
        writer.writeAttribute(QString("f").append(QString::number(i)), QString::number(f[i]));
    }
}

void SCCell::passivateWithoutType(QXmlStreamWriter &writer) {
    int model = Shared::instance()->getGridConfig()->getModel();
    writer.writeAttribute("p0", QString::number(p0));
    writer.writeAttribute("lastDeltaP", QString::number(lastDeltaP));
    for (int i = 0; i < model; i++) {
        writer.writeAttribute(QString("f").append(QString::number(i)), QString::number(f[i]));
    }
}

void SCCell::activate(QXmlStreamReader &reader, Grid *) {
    p0 = reader.attributes().value("p0").toString().toDouble();
    lastDeltaP = reader.attributes().value("lastDeltaP").toString().toDouble();
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        f[i] = reader.attributes().value(QString("f").append(QString::number(i))).toString().toDouble();
    }
    LBUtil::calc_pu(f, Shared::instance()->getGridConfig()->getModel(), &p, &u);
}

MyVector3D SCCell::getForcesVelocity(int) {
    return forcesVelocity;
}

void SCCell::setMulticomponent(int) {
}

bool SCCell::isFluid() {
    return true;
}

void SCCell::setConstantConcentration(double constantConcentration) {
    this->constantConcentration = constantConcentration;
}
