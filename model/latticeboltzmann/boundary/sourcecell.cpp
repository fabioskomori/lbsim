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

#include "sourcecell.h"
#include "../../../model/math/myvector3d.h"
#include "../../../model/math/vector3i.h"
#include "zouheboundary.h"
#include "equilibriumboundary.h"
#include "fixedboundary.h"
#include "pointboundary.h"
#include <typeinfo>
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

int SourceCell::getOpenCLType() {
    return (index == 0 ? 3 : 8);
}

SourceCell* SourceCell::createNull() {
    SourceCell *result = new SourceCell();
    result->setImpl(0);
    return result;
}

SourceCell* SourceCell::createEquilibrium(double velocity) {
    SourceCell *result = new SourceCell();
    EquilibriumBoundary *equilibrium = dynamic_cast<EquilibriumBoundary*>(result->getImpl());
    equilibrium->configVelocity(velocity);
    return result;
}

SourceCell* SourceCell::createZouHe(double velocity) {
    SourceCell *result = new SourceCell();
    ZouHeBoundary *zouhe = new ZouHeBoundary();
    zouhe->configVelocity(velocity);
    result->setImpl(zouhe);
    return result;
}

SourceCell* SourceCell::createFixed(double velocity, double p) {
    SourceCell *result = new SourceCell();
    FixedBoundary *fixed = new FixedBoundary();
    fixed->configVelocity(velocity);
    fixed->configPressure(p);
    result->setImpl(fixed);
    return result;
}

SourceCell* SourceCell::createPoint(double p) {
    SourceCell *result = new SourceCell();
    PointBoundary *boundary = new PointBoundary();
    boundary->configPressure(p);
    result->setImpl(boundary);
    return result;
}

SourceCell::SourceCell() {
    boundary = new EquilibriumBoundary();
}

SourceCell::~SourceCell() {
    delete boundary;
}

void SourceCell::preUpdate(double epsilon[], BaseCell *neighbors[], Grid *grid, Vector3i position) {
    boundary->preUpdate(epsilon, neighbors, grid, position);
}

void SourceCell::preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    boundary->preUpdate2(epsilon, neighbors, grid, position);
}

double SourceCell::getF(int index) {
    return boundary->getF(index, -1);
}

void SourceCell::update() {
    boundary->update();
}

void SourceCell::postUpdate(Grid*, Vector3i) {
}

void SourceCell::reset(double p0, double p1) {
    boundary->reset(p0, p1);
}

void SourceCell::setIndex(int index) {
    this->index = index;
    EquilibriumBoundary *equilibrium = dynamic_cast<EquilibriumBoundary*>(boundary);
    if (equilibrium != 0) {
        equilibrium->setMultiComponentIndex(index);
    }
    FixedBoundary *fixed = dynamic_cast<FixedBoundary*>(boundary);
    if (fixed != 0) {
        fixed->setMultiComponentIndex(index);
    }
}

int SourceCell::getIndex() {
    return index;
}

Boundary* SourceCell::getImpl() {
    return boundary;
}

void SourceCell::setImpl(Boundary *boundary) {
    if (this->boundary != 0) {
        delete this->boundary;
    }
    this->boundary = boundary;
}

void SourceCell::activate(QXmlStreamReader &reader, Grid *grid) {
    index = reader.attributes().value("index").toString().toInt();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    if (reader.attributes().value("type").toString() == "equilibrium") {
        boundary = new EquilibriumBoundary();
    } else if (reader.attributes().value("type").toString() == "fixed") {
        boundary = new FixedBoundary();
    } else if (reader.attributes().value("type").toString() == "point") {
        boundary = new PointBoundary();
    } else if (reader.attributes().value("type").toString() == "zouhe") {
        boundary = new ZouHeBoundary();
    } else {
        qWarning() << "inlet error";
    }
    boundary->activate(reader, grid);
}

void SourceCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "source");
    writer.writeAttribute("index", QString::number(index));
    writer.writeStartElement("boundary");
    boundary->passivate(writer);
    writer.writeEndElement();
}

double SourceCell::getF(int, int) {
    return 0;
}

double SourceCell::getF0(int, double, BaseCell*[], int) {
    return 0;
}

void SourceCell::setMulticomponent(int) {
}

void SourceCell::setNextF(int index, double value, int component) {
    boundary->setNextF(index, value, component);
}

double SourceCell::getP(int) {
    return 0;
}

MyVector3D SourceCell::getU(int) {
    return MyVector3D();
}

double SourceCell::deltaP() {
    return 0;
}

double SourceCell::getLastDeltaP() {
    return 0;
}

double SourceCell::getNextF(int, int) {
    return 0;
}

MyVector3D SourceCell::getForcesVelocity(int) {
    return MyVector3D();
}

bool SourceCell::isFluid() {
    return false;
}
