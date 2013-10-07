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

#ifndef PASSIVESCALARFORCE_H
#define PASSIVESCALARFORCE_H

#include "../../../model/latticeboltzmann/force/force.h"

class PassiveScalarForce : public Force {
public:
    PassiveScalarForce(double strengthX, double strengthY, int minx, int maxx, int miny, int maxy, int period);
    MyVector3D computeForce(BaseCell *lattice, BaseCell* neighbors[], int index, int x, int y, int z, Grid *grid);
    bool hasEffect();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
    double getStrengthX();
    double getStrengthY();
    int getMinX();
    int getMaxX();
    int getMinY();
    int getMaxY();
    int getPeriod();
private:
    double strengthX, strengthY;
    int minx, maxx, miny, maxy;
    int period;
};

#endif // PASSIVESCALARFORCE_H
