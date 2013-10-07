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

#ifndef SOMETHINGMOVING_H
#define SOMETHINGMOVING_H

#include "../../../model/math/myvector3d.h"
#include "../../../model/latticeboltzmann/basecell.h"

class SomethingMoving {
public:
    virtual void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) = 0;
    virtual void update() = 0;
    virtual MyVector3D getD() = 0;
    virtual MyVector3D getVelocity() = 0;
    virtual void passivate(QXmlStreamWriter &writer) = 0;
    virtual void activate(QXmlStreamReader &reader) = 0;
    virtual SomethingMoving* clone() = 0;
};

#endif // SOMETHINGMOVING_H
