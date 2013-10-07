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

#include "myvector3d.h"
#include "vector3i.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <QDebug>

MyVector3D::MyVector3D(): x(0), y(0), z(0) {
}

MyVector3D::MyVector3D(double x, double y, double z) : x(x), y(y), z(z) {
}

double MyVector3D::distance2(MyVector3D v) {
    return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z);
}

MyVector3D MyVector3D::periodic(double w, double h, double l) {
    double nx = x, ny = y, nz = z;
    if (x < 0) {
        nx += w;
    } else if (x > w) {
        nx -= w;
    }
    if (y < 0) {
        ny += h;
    } else if (y > h) {
        ny -= h;
    }
    if (z < 0) {
        nz += l;
    } else if (z > l) {
        nz -= l;
    }
    return MyVector3D(nx, ny, nz);
}

MyVector3D MyVector3D::projection(bool hasx, bool hasy, bool hasz) {
    return MyVector3D(x * (hasx ? 1 : 0), y * (hasy ? 1 : 0), z * (hasz ? 1 : 0));
}

MyVector3D MyVector3D::projection(Vector3i &v) {
    return MyVector3D(x * v.getX(), y * v.getY(), z * v.getZ());
}

MyVector3D MyVector3D::operator^(const double scalar) const {
    return MyVector3D(x * scalar, y * scalar, z * scalar);
}

MyVector3D MyVector3D::operator^(const MyVector3D &a) const {
    return MyVector3D(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
}

double MyVector3D::operator*(const MyVector3D &a) const {
    return a.x * x + a.y * y + a.z * z;
}

double MyVector3D::operator*(const Vector3i &a) const {
    return a.getX() * x + a.getY() * y + a.getZ()* z;
}

MyVector3D MyVector3D::operator+(const MyVector3D &a) const {
    return MyVector3D(a.x + x, a.y + y, a.z + z);
}

MyVector3D MyVector3D::operator-(const MyVector3D &a) const {
    return MyVector3D(x - a.x, y - a.y, z - a.z);
}

bool MyVector3D::operator==(MyVector3D &a) const {
    return a.x == x && a.y == y && a.z == z;
}

MyVector3D MyVector3D::limit(double limit) {
    double norma = norm();
    if (norma > limit) {
        return MyVector3D(x * limit / norma, y * limit / norma, z * limit / norma);
    }
    return MyVector3D(x, y, z);
}

double MyVector3D::norm() {
    return std::sqrt(x * x + y * y + z * z);
}

double MyVector3D::norm2() {
    return x * x + y * y + z * z;
}

MyVector3D MyVector3D::log() {
    return MyVector3D(std::log(x), std::log(y), std::log(z));
}

MyVector3D MyVector3D::normalize() {
    double norma = norm();
    return MyVector3D(x / norma, y / norma, z / norma);
}

Vector3i MyVector3D::toVector3i() {
    return Vector3i(x, y, z);
}

double MyVector3D::getX() const{
    return x;
}

double MyVector3D::getY() const{
    return y;
}

double MyVector3D::getZ() const{
    return z;
}

std::string MyVector3D::toString() {
	std::ostringstream result;
    result << "(";
    result << x;
    result << ", ";
    result << y;
    result << ", ";
    result << z;
    result << ")";
    return result.str();
}

QString MyVector3D::qtString() {
    return QString(toString().c_str());
}
