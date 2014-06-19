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

#ifndef LOADERFROMIMAGE_H
#define LOADERFROMIMAGE_H

class QString;
class Grid;
#include <QColor>

class LoaderFromImage {
public:
    static void load(Grid *grid, QString &fileName, QString flags = "");
    static void save(Grid *grid, QString &fileName, int scale = 1);
    static QColor null;
    static QColor porous;
    static QColor moving;
    static QColor open;
    static QColor source;
    static QColor movingWall;
    static QColor wall;
    static QColor negative;
    static QColor positive;
    static QColor shallow;
    static QColor point;
    static QColor thermal;
    static QColor dragWall;
    static QColor slipWall;
    static QColor depositionWall;
    static QColor reflectWithFactor;
    static QColor kornerSolid, kornerLiquid, kornerGas, kornerInterface, kornerWall;
};

#endif // LOADERFROMIMAGE_H
