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

#ifndef VECTOR3I_H
#define VECTOR3I_H

class MyVector3D;

class Vector3i {
public:
    Vector3i();
    Vector3i(int x, int y, int z);
    int getX() const;
    int getY() const;
    int getZ() const;
    double norm();
    void addD(int x, int y, int z, bool periodic, int mx, int my, int mz);
    bool isNeighbor(const Vector3i *vector) const;
    bool operator==(const Vector3i &a) const;
    Vector3i operator+(const Vector3i &v) const;
    Vector3i operator-(const Vector3i &v) const;
    MyVector3D operator^(const double a) const;
    MyVector3D toMyVector3D();
    Vector3i operator^(const int a) const;
    double operator*(const MyVector3D &v) const;
    Vector3i reflect(int width, int height, int length);
    bool bounds(const int width, const int height, const int length) const;
private:
    int x, y, z;
};

#endif // VECTOR3I_H
