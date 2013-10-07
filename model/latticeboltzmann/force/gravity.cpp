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

#include "gravity.h"
#include <cmath>
#include "../../../model/math/myvector3d.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/basecell.h"
#include "../../../model/util/shared.h"

Gravity::Gravity() {
}

MyVector3D Gravity::computeForce(BaseCell *empty, BaseCell*[], int index, int, int, int, Grid*) {
    GridConfig *config = Shared::instance()->getGridConfig();
    return MyVector3D(0, -config->getGravity(), 0) ^ (empty->getP(-1) * (index == 0 ? config->getDensity1() : config->getDensity2()));
}

bool Gravity::hasEffect() {
    GridConfig *config = Shared::instance()->getGridConfig();
    return std::abs(config->getGravity()) > 1e-100;
}

void Gravity::passivate(QXmlStreamWriter&) {
}

void Gravity::activate(QXmlStreamReader&, Grid*) {
}
