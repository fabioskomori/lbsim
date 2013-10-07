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

#ifndef FORCE_H
#define FORCE_H

class MyVector3D;
class BaseCell;
class Grid;
class QXmlStreamWriter;
class QXmlStreamReader;

class Force {
public:
    virtual MyVector3D computeForce(BaseCell *lattice, BaseCell* neighbors[], int index, int x, int y, int z, Grid *grid) = 0;
    virtual bool hasEffect() = 0;
    virtual void passivate(QXmlStreamWriter &writer) = 0;
    virtual void activate(QXmlStreamReader &reader, Grid *grid) = 0;
};

#endif // FORCE_H
