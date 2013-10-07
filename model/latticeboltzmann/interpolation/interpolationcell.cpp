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

#include "interpolationcell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/basecell.h"
#include "../../../model/math/myvector3d.h"
#include "../../../model/math/vector3i.h"
#include <QXmlStreamWriter>
#include <cmath>
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridsimulation.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/util/shared.h"

#define PI 3.14159265

InterpolationCell::InterpolationCell(double density, double variation, int period) : density(density), variation(variation), period(period) {
}

void InterpolationCell::preUpdate(double[], BaseCell *neighbors[], Grid *grid, Vector3i) {
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        BaseCell* neighbor = neighbors[i];
        if (neighbor != 0) {
            double newDensity = density;
            if (period != 0) {
                newDensity += variation * std::sin(2 * PI * ((grid->getSimulation()->getIterations() % period) / (double)period));
            }
            neighbor->setNextF(i, LBUtil::f_eq(MyVector3D(), newDensity, Shared::instance()->getGridConfig()->getModel(), i), 0);
        }
    }
}

void InterpolationCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "interpolation");
    writer.writeAttribute("density", QString::number(density));
    writer.writeAttribute("variation", QString::number(variation));
    writer.writeAttribute("period", QString::number(period));
}

void InterpolationCell::activate(QXmlStreamReader &reader, Grid*) {
    density = reader.attributes().value("density").toString().toDouble();
    if (reader.attributes().hasAttribute("variation")) {
        variation = reader.attributes().value("variation").toString().toDouble();
        period = reader.attributes().value("period").toString().toInt();
    }
}

void InterpolationCell::preUpdate2(double[], BaseCell*[], Grid*, Vector3i) {
}

void InterpolationCell::setNextF(int, double, int) {
}

int InterpolationCell::getOpenCLType() {
    return -1;
}
