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

#ifndef VECTOR_H
#define VECTOR_H

class Vector3i;
class QString;
#include <string>

class MyVector3D {
public:
    MyVector3D();
    MyVector3D(double x, double y, double z);
    MyVector3D projection(bool x, bool y, bool z);
    MyVector3D projection(Vector3i &v);
    MyVector3D periodic(double w, double h, double l);
    MyVector3D operator^(const double scalar) const;
    MyVector3D operator^(const MyVector3D &a) const;
    double operator*(const MyVector3D &a) const;
    double operator*(const Vector3i &a) const;
    MyVector3D operator+(const MyVector3D &a) const;
    MyVector3D operator-(const MyVector3D &a) const;
    bool operator==(MyVector3D &a) const;
    MyVector3D limit(double limit);
    MyVector3D normalize();
    MyVector3D log();
    Vector3i toVector3i();
    double distance2(MyVector3D v);
    double norm();
    double norm2();
    double getX() const;
    double getY() const;
    double getZ() const;
    std::string toString();
    QString qtString();
private:
    double x;
    double y;
    double z;
};

#endif // VECTOR_H
