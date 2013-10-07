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

#ifndef DATAPOINT_H
#define DATAPOINT_H

#include "../../model/math/vector3i.h"
#include "../../model/math/myvector3d.h"

class DataPoint {
public:
    DataPoint(int x, int y, int z, double density, int densityEnabled, double velocityX, double velocityY, double velocityZ, int velocityXenabled, int velocityYenabled, int velocityZenabled);
    Vector3i getPoint();
    double getDensity();
    int getDensityEnabled();
    MyVector3D getVelocity();
    Vector3i getVelocityEnabled();
private:
    Vector3i point;
    double density;
    int densityEnabled;
    MyVector3D velocity;
    Vector3i velocityEnabled;
};

#endif // DATAPOINT_H
