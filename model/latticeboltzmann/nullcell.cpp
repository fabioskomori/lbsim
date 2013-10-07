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

#include "nullcell.h"
#include <QDebug>
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/math/vector3i.h"
#include "../../model/math/myvector3d.h"
#include <QXmlStreamWriter>

int NullCell::getOpenCLType() {
    return 10;
}

NullCell::NullCell() {
}

void NullCell::preUpdate(double[], BaseCell*[], Grid*, Vector3i) {
}

void NullCell::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void NullCell::update() {
}

void NullCell::postUpdate(Grid*, Vector3i) {
}

void NullCell::reset(double, double) {
}

double NullCell::getF(int, int) {
    return 0;
}

void NullCell::setNextF(int, double, int) {
}

void NullCell::setMulticomponent(int) {
}

void NullCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "null");
}

double NullCell::getP(int) {
    return 0;
}

MyVector3D NullCell::getU(int) {
    return MyVector3D();
}

double NullCell::deltaP() {
    return 0;
}

double NullCell::getLastDeltaP() {
    return 0;
}

double NullCell::getNextF(int, int) {
    return 0;
}

MyVector3D NullCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool NullCell::isFluid() {
    return false;
}
