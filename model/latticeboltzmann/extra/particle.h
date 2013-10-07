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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "../../../model/math/vector3i.h"
#include "../../../model/math/myvector3d.h"
class QXmlStreamReader;
class QXmlStreamWriter;

class Particle {
public:
    Particle(MyVector3D position, Vector3i color, int id, MyVector3D previousPosition);
    ~Particle();
    Particle* clone();
    MyVector3D getPosition();
    Vector3i getColor();
    MyVector3D getPreviousPosition();
    int getId();
private:
    MyVector3D position;
    Vector3i color;
    int id;
    MyVector3D previousPosition;
};

#endif // PARTICLE_H
