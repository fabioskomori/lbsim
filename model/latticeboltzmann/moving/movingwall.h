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

#ifndef MOVINGWALL_H
#define MOVINGWALL_H

#include "somethingmoving.h"

class MovingWall : public SomethingMoving {
public:
    MovingWall(MyVector3D velocity);
    void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    MyVector3D getD();
    MyVector3D getVelocity();
    void configVelocity(MyVector3D velocity);
    void configPeriod(int period);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    SomethingMoving* clone();
private:
    MyVector3D velocity;
    int period, current;
};

#endif // MOVINGWALL_H
