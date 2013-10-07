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

#include "rkcell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QDebug>
#include <cmath>
#include "../../../model/util/shared.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

int RKCell::getOpenCLType() {
    return 0;
}

void RKCell::updateFromOpenCL(double *data) {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        red[i] = data[i];
        blue[i] = data[i + model];
    }
    updatePU();
}

RKCell::RKCell() {
    int model = Shared::instance()->getGridConfig()->getModel();
    red = new double[model];
    blue = new double[model];
    nextRed = new double[model];
    nextBlue = new double[model];
    GridConfig *config = Shared::instance()->getGridConfig();
    reset(config->randomP(), config->randomP());
}

RKCell::~RKCell() {
    delete[] red;
    delete[] blue;
    delete[] nextRed;
    delete[] nextBlue;
}

void RKCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "multi_rk");
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        writer.writeAttribute(QString("red").append(QString::number(i)), QString::number(red[i]));
        writer.writeAttribute(QString("blue").append(QString::number(i)), QString::number(blue[i]));
    }
}

void RKCell::activate(QXmlStreamReader &reader, Grid*) {
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        red[i] = reader.attributes().value(QString("red").append(QString::number(i))).toString().toDouble();
        blue[i] = reader.attributes().value(QString("blue").append(QString::number(i))).toString().toDouble();
    }
    updatePU();
}

void RKCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    int model = Shared::instance()->getGridConfig()->getModel();
    MyVector3D colorGradient = MyVector3D();
    for (int a = 0; a < model; a++) {
        Vector3i neighbor = position + LBUtil::C[model][a];
        RKCell *cell = dynamic_cast<RKCell*>(grid->getGrid(neighbor.getY(), neighbor.getX(), neighbor.getZ()));
        if (cell != 0) {
            colorGradient = colorGradient + (LBUtil::C[model][a] ^ cell->getColor());
        }
    }
    for (int i = 0; i < model; i++) {
        double fi = red[i] + blue[i];
        double p = redP + blueP;
        double nextF = fi + 1.0 / epsilon[0] * (LBUtil::f_eq(u, p, model, i) - fi);
        double cos = (LBUtil::C[model][i] * colorGradient);
        if (colorGradient.norm() > 1e-10 && LBUtil::C[model][i].norm() > 1e-10) {
            cos = cos / (LBUtil::C[model][i].norm() * colorGradient.norm());
        } else {
            //cos = std::sqrt(2.0) / 2;
            //cos = 1;
            cos = 0;
        }
        GridConfig *config = Shared::instance()->getGridConfig();
        nextF += config->getRkA() * colorGradient.norm() * (2 * cos * cos - 1);
        double diff = config->getRkBeta() * redP * blueP / (p * p) * LBUtil::W[model][i] * p * cos;
        //double diff = 0;
        double newNextRed = redP / p * nextF + diff;
        double newNextBlue = blueP / p * nextF - diff;
        if (neighbors[i] != 0) {
            neighbors[i]->setNextF(i, newNextRed, 0);
            neighbors[i]->setNextF(i, newNextBlue, 1);
        }
    }
}

void RKCell::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void RKCell::update() {
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        red[i] = nextRed[i];
        blue[i] = nextBlue[i];
    }
    updatePU();
}

void RKCell::updatePU() {
    redP = blueP = 0;
    u = MyVector3D();
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        redP += red[i];
        blueP += blue[i];
        u = u + (LBUtil::C[model][i] ^ (red[i] + blue[i]));
    }
    u = u ^ (1.0 / (redP + blueP));
}

void RKCell::postUpdate(Grid*, Vector3i) {
}

void RKCell::reset(double p0, double p1) {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        red[i] = LBUtil::f_eq(MyVector3D(), p0, model, i);
        blue[i] = LBUtil::f_eq(MyVector3D(), p1, model, i);
    }
    updatePU();
}

double RKCell::getF(int index, int component) {
    switch (component) {
    case 0:
        return red[index];
    case 1:
        return blue[index];
    }
    return 0;
}

void RKCell::setNextF(int index, double value, int component) {
    switch (component) {
    case 0:
        nextRed[index] = value;
        break;
    case 1:
        nextBlue[index] = value;
        break;
    }
}

void RKCell::setMulticomponent(int) {
}

double RKCell::getP(int index) {
    if (index == 0) {
        return redP;
    } else if (index == 1) {
        return blueP;
    }
    return redP + blueP;
}

MyVector3D RKCell::getU(int) {
    return u;
}

double RKCell::deltaP() {
    return 0;
}

double RKCell::getLastDeltaP() {
    return 0;
}

double RKCell::getNextF(int, int) {
    return 0;
}

MyVector3D RKCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool RKCell::isFluid() {
    return true;
}

double RKCell::getColor() {
    return redP - blueP;
}
