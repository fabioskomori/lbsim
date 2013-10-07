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

#include "nextcellevalutor.h"
#include "velocityevaluator.h"
#include "../../../model/math/myvector3d.h"
#include "nearestpointvelocityevaluator.h"

NextCellEvalutor::NextCellEvalutor() {
    ve = new NearestPointVelocityEvaluator();
}

NextCellEvalutor::~NextCellEvalutor() {
    delete ve;
}

MyVector3D NextCellEvalutor::nextPoint(MyVector3D point, Grid *grid) {
    MyVector3D v = ve->evaluate(point, grid);
    int px = point.getX();
    int py = point.getY();
    int pz = point.getZ();
    MyVector3D n = point + v;
    int count = 0;
    while ((int)n.getX() == px && (int)n.getY() == py && (int)n.getZ() == pz && ++count < 1000) {
        n = n + v;
    }
    return n;
}
