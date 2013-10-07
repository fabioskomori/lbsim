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

#include "passivescalarforce.h"
#include "../../../model/math/myvector3d.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridsimulation.h"
#include "../../../model/latticeboltzmann/basecell.h"
#include "../../../model/latticeboltzmann/passivescalar/passivescalarcell.h"
#include <QDebug>
#include <cmath>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#define PI 3.14159265

PassiveScalarForce::PassiveScalarForce(double strengthX, double strengthY, int minx, int maxx, int miny, int maxy, int period) : strengthX(strengthX), strengthY(strengthY), minx(minx), maxx(maxx), miny(miny), maxy(maxy), period(period) {
}

MyVector3D PassiveScalarForce::computeForce(BaseCell*, BaseCell*[], int, int x, int y, int z, Grid *grid) {
    if (x >= minx && x <= maxx && y >= miny && y <= maxy) {
        double weight = 0;
        PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(grid->getGrid(y, x, z));
        if (psc != 0) {
            weight = psc->getP(10);
        }
        return MyVector3D(weight * strengthX * (period == 0 ? 1 : std::sin(2 * PI * (grid->getSimulation()->getIterations() % period) / period)), weight * strengthY * (period == 0 ? 1 : std::sin(2 * PI * (grid->getSimulation()->getIterations() % period) / period)), 0);
    }
    return MyVector3D();
}

double PassiveScalarForce::getStrengthX() {
    return strengthX;
}

double PassiveScalarForce::getStrengthY() {
    return strengthY;
}

int PassiveScalarForce::getMinX() {
    return minx;
}

int PassiveScalarForce::getMaxX() {
    return maxx;
}

int PassiveScalarForce::getMinY() {
    return miny;
}

int PassiveScalarForce::getMaxY() {
    return maxy;
}

int PassiveScalarForce::getPeriod() {
    return period;
}

bool PassiveScalarForce::hasEffect() {
    return true;
}

void PassiveScalarForce::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("strengthX", QString::number(strengthX));
    writer.writeAttribute("strengthY", QString::number(strengthY));
    writer.writeAttribute("minx", QString::number(minx));
    writer.writeAttribute("maxx", QString::number(maxx));
    writer.writeAttribute("miny", QString::number(miny));
    writer.writeAttribute("maxy", QString::number(maxy));
    writer.writeAttribute("period", QString::number(period));
}

void PassiveScalarForce::activate(QXmlStreamReader &reader, Grid*) {
    strengthX = reader.attributes().value("strengthX").toString().toDouble();
    strengthY = reader.attributes().value("strengthY").toString().toDouble();
    minx = reader.attributes().value("minx").toString().toInt();
    maxx = reader.attributes().value("maxx").toString().toInt();
    miny = reader.attributes().value("miny").toString().toInt();
    maxy = reader.attributes().value("maxy").toString().toInt();
    period = reader.attributes().value("period").toString().toInt();
}
