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

#include "movingwall.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/math/vector3i.h"
#include <cmath>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

MovingWall::MovingWall(MyVector3D velocity) {
    this->velocity = velocity;
    period = current = 0;
}

void MovingWall::preUpdate(double[], BaseCell*[], Grid*, Vector3i) {
}

void MovingWall::update() {
    if (period != 0) {
        current = (current + 1) % period;
    }
}

MyVector3D MovingWall::getD() {
    return MyVector3D(0, 0, 0);
}

MyVector3D MovingWall::getVelocity() {
    static double pi(std::atan(1.0) * 4.0);
    if (period != 0) {
        MyVector3D result = velocity ^ (std::sin(2.0 * pi * current / period));
        return result;
    }
    return velocity;
}

void MovingWall::configVelocity(MyVector3D velocity) {
    this->velocity = velocity;
}

void MovingWall::configPeriod(int period) {
    this->period = period;
}

void MovingWall::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "wall");
    writer.writeAttribute("velocityX", QString::number(velocity.getX()));
    writer.writeAttribute("velocityY", QString::number(velocity.getY()));
    writer.writeAttribute("velocityZ", QString::number(velocity.getZ()));
    writer.writeAttribute("period", QString::number(period));
    writer.writeAttribute("current", QString::number(current));
}

void MovingWall::activate(QXmlStreamReader &reader) {
    velocity = MyVector3D(reader.attributes().value("velocityX").toString().toDouble(), reader.attributes().value("velocityY").toString().toDouble(), reader.attributes().value("velocityZ").toString().toDouble());
    period = reader.attributes().value("period").toString().toInt();
    current = reader.attributes().value("current").toString().toInt();
}

SomethingMoving* MovingWall::clone() {
    return new MovingWall(velocity);
}
