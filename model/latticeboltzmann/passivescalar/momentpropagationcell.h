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

#ifndef MOMENTPROPAGATIONCELL_H
#define MOMENTPROPAGATIONCELL_H

#include "passivescalarcell.h"

class MomentPropagationCell : public PassiveScalarCell {
public:
    MomentPropagationCell(BaseCell *cell, double fixedConcentration, int index);
    void initializeDistribution();
    void calc();
    void reset(int id);
    void reset(double p0, double p1);
    void preUpdate(double epsilon[], BaseCell *neighbors[], Grid *grid, Vector3i position);
    void preUpdate2(double epsilon[], BaseCell *neighbors[], Grid *grid, Vector3i position);
    void update();
    double getF(int index, int component);
    void setNextF(int index, double value, int component);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
};

#endif // MOMENTPROPAGATIONCELL_H
