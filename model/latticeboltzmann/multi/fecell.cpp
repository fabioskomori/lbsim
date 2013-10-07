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

#include "fecell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/math/vector3i.h"
#include "../../../model/util/shared.h"
#include "../../../model/latticeboltzmann/gridconfig.h"

FECell::FECell() {
    int model = Shared::instance()->getGridConfig()->getModel();
    f = new double[model];
    g = new double[model];
}

FECell::~FECell() {
    delete[] f;
    delete[] g;
}

void FECell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *, Vector3i ) {
    int model = Shared::instance()->getGridConfig()->getModel();
    double muPhi = 0; //?
    double A0 = 9 * rho / 4 - 15.0 / 4 * (phi * muPhi + rho / 3);
    double Ai = 3 * (phi * muPhi + rho / 3);
    for (int i = 0; i < model; i++) {
        if (neighbors[i] != 0) {
            /*double scalar = LBUtil::C[model][i] * u;
            double feq = LBUtil::W[model][i] * ((i == 0 ? A0 : Ai) + rho * (3 * scalar + 9.0 / 2 * scalar * scalar - 3.0 / 2 * (u * u)));
            double Falpha = 0; //???
            double deltaPhiAlpha = 0; //???
            double Si = 3 * (LBUtil::W[model][i] * LBUtil::C[model][i]) * (Falpha + muPhi * deltaPhiAlpha);
            neighbors[i]->setNextF(i, f[i] + 1.0 / epsilon[0] * (feq - f[i]) + Si);*/
        }
    }
}

void FECell::preUpdate2(double [], BaseCell* [], Grid *, Vector3i ) {
}

void FECell::update() {
    calc();
}

void FECell::postUpdate(Grid *, Vector3i ) {
}

void FECell::reset(double newP0, double newP1) {
    if (newP0 >= 0) {
        p0 = newP0;
    }
    if (newP1 >= 0) {
        p1 = newP1;
    }
}

double FECell::getF(int , int ) {
    return 0;
}

void FECell::setNextF(int , double , int ) {
}

void FECell::setMulticomponent(int ) {
}

double FECell::getP(int ) {
    return 0;
}

MyVector3D FECell::getU(int ) {
    return u;
}

double FECell::deltaP() {
    return 0;
}

double FECell::getLastDeltaP() {
    return 0;
}

double FECell::getNextF(int, int) {
    return 0;
}

MyVector3D FECell::getForcesVelocity(int) {
    return MyVector3D();
}

bool FECell::isFluid() {
    return true;
}

void FECell::calc() {
    rho = phi = 0;
    u = MyVector3D();
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        rho += f[i];
        phi += g[i];
        u = u + (LBUtil::C[Shared::instance()->getGridConfig()->getModel()][i] ^ f[i]);
    }
    u = u ^ (1.0 / rho);
}
