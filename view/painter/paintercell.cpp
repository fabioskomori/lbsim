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

#include "paintercell.h"

PainterCell::PainterCell(BaseCell *cell, int i, int j, int k, int c, int orientation) : cell(cell), i(i), j(j), k(k), c(c), orientation(orientation) {
}

BaseCell* PainterCell::getCell() {
    return cell;
}

int PainterCell::getI() {
    return i;
}

int PainterCell::getJ() {
    return j;
}

int PainterCell::getK() {
    return k;
}

int PainterCell::getC() {
    return c;
}

int PainterCell::getOrientation() {
    return orientation;
}
