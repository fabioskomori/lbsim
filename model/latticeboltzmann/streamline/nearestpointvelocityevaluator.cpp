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

#include "nearestpointvelocityevaluator.h"
#include "../../../model/math/myvector3d.h"
#include <cmath>
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/basecell.h"

NearestPointVelocityEvaluator::NearestPointVelocityEvaluator() {
}

MyVector3D NearestPointVelocityEvaluator::evaluate(MyVector3D point, Grid *grid) {
    int x = (int)ceil(point.getX());
    if (point.getX() - floor(point.getX()) < ceil(point.getX()) - point.getX()) {
        x = floor(point.getX());
    }
    int y = (int)ceil(point.getY());
    if (point.getY() - floor(point.getY()) < ceil(point.getY()) - point.getY()) {
        y = floor(point.getY());
    }
    int z = (int)ceil(point.getZ());
    if (point.getZ() - floor(point.getZ()) < ceil(point.getZ()) - point.getZ()) {
        z = floor(point.getZ());
    }
    BaseCell *cell = grid->getGrid(y, x, z);
    if (cell == 0) {
        return MyVector3D();
    }
    return cell->getU(0);
}
