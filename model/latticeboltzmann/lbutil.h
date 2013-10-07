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

#ifndef LBUTIL_H
#define LBUTIL_H

class MyVector3D;
class Vector3i;
class BaseCell;

class LBUtil {
public:
    static void f_eq(const MyVector3D &u, double p, int model, double f[]);
    static double f_eq(const MyVector3D &u, double p, int model, int index);
    static double f_eq2(const MyVector3D &u, double p, int model, int index);
    static void calc_pu(double f[], int model, double *p, MyVector3D *u);
    static void stream(int model, BaseCell* neighbors[], double nextF[]);
    static Vector3i C[28][27];
    static double W[28][27];
    static int OPPOSITE[28][27];
};

#endif // LBUTIL_H
