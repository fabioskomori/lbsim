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

#include "opencell.h"
#include "zouheboundary.h"
#include "equilibriumboundary.h"
#include "extrapolationboundary.h"
#include "../../../model/math/vector3i.h"
#include "../../../model/math/myvector3d.h"
#include <typeinfo>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

int OpenCell::getOpenCLType() {
    return 2;
}

OpenCell* OpenCell::createNull() {
    OpenCell *result = new OpenCell();
    result->setImpl(0);
    return result;
}

OpenCell* OpenCell::createZouHePressure(double pressure) {
    OpenCell *result = new OpenCell();
    ZouHeBoundary *zouHe = new ZouHeBoundary();
    zouHe->configPressure(pressure);
    result->setImpl(zouHe);
    return result;
}

OpenCell* OpenCell::createEquilibrium(double pressure, int ) {
    OpenCell *result = new OpenCell();
    EquilibriumBoundary *equilibrium = new EquilibriumBoundary();
    equilibrium->configPressure(pressure);
    result->setImpl(equilibrium);
    return result;
}

OpenCell* OpenCell::createExtrapolation(int ) {
    OpenCell *result = new OpenCell();
    ExtrapolationBoundary *extrapolation = new ExtrapolationBoundary();
    result->setImpl(extrapolation);
    return result;
}

OpenCell::OpenCell() {
    boundary = 0;
}

OpenCell::~OpenCell() {
    delete boundary;
}

void OpenCell::preUpdate(double epsilon[], BaseCell *neighbors[], Grid *grid, Vector3i position) {
    boundary->preUpdate(epsilon, neighbors, grid, position);
}

void OpenCell::preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    boundary->preUpdate2(epsilon, neighbors, grid, position);
}

void OpenCell::update() {
    boundary->update();
}

void OpenCell::postUpdate(Grid*, Vector3i) {
}

void OpenCell::reset(double p0, double p1) {
    boundary->reset(p0, p1);
}

double OpenCell::getF(int index) {
    return boundary->getF(index, -1);
}

Boundary* OpenCell::getImpl() {
    return boundary;
}

void OpenCell::setImpl(Boundary *boundary) {
    if (this->boundary != 0) {
        delete this->boundary;
    }
    this->boundary = boundary;
}

void OpenCell::activate(QXmlStreamReader &reader, Grid *grid) {
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    if (reader.attributes().value("type").toString() == "equilibrium") {
        boundary = new EquilibriumBoundary();
    } else if (reader.attributes().value("type").toString() == "extrapolation") {
        boundary = new ExtrapolationBoundary();
    } else if (reader.attributes().value("type").toString() == "zouhe") {
        boundary = new ZouHeBoundary();
    } else {
        qWarning() << "outlet error";
    }
    boundary->activate(reader, grid);
}

void OpenCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "open");
    writer.writeStartElement("boundary");
    boundary->passivate(writer);
    writer.writeEndElement();
}

void OpenCell::setNextF(int, double) {

}

double OpenCell::getF(int, int) {
    return 0;
}

double OpenCell::getF0(int, double, BaseCell*[], int) {
    return 0;
}

void OpenCell::setMulticomponent(int) {
}

void OpenCell::setNextF(int index, double value, int component) {
    boundary->setNextF(index, value, component);
}

double OpenCell::getP(int) {
    return 0;
}

MyVector3D OpenCell::getU(int) {
    return MyVector3D();
}

double OpenCell::deltaP() {
    return 0;
}

double OpenCell::getLastDeltaP() {
    return 0;
}

double OpenCell::getNextF(int, int) {
    return 0;
}

MyVector3D OpenCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool OpenCell::isFluid() {
    return false;
}
