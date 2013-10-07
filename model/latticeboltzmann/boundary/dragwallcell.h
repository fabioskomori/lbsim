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

#ifndef DRAGWALLCELL_H
#define DRAGWALLCELL_H

#include "wallcell.h"
#include "../../../model/math/myvector3d.h"

class DragWallCell : public WallCell {
public:
    DragWallCell();
    void activate(QXmlStreamReader &reader, Grid *grid);
    void passivate(QXmlStreamWriter &writer);
    void setNextF(int index, double value, int component);
    void update();
    MyVector3D getDragForce(Grid *grid, Vector3i position);
    void setIndex(int index);
    int getIndex();
private:
    int index;
};

#endif // DRAGWALLCELL_H
