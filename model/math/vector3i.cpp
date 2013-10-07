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

#include "vector3i.h"
#include "myvector3d.h"
#include <cmath>
#include <cstdlib>

Vector3i::Vector3i() {
}

Vector3i::Vector3i(int x, int y, int z) : x(x), y(y), z(z) {
}

int Vector3i::getX() const {
    return x;
}

int Vector3i::getY() const {
    return y;
}

int Vector3i::getZ() const {
    return z;
}

double Vector3i::norm() {
    return std::sqrt((double)(x * x + y * y + z * z));
}

bool Vector3i::isNeighbor(const Vector3i *vector) const {
    return abs(x - vector->x) <= 1 && abs(y - vector->y) <= 1 && abs(z - vector->z) <= 1;
}

bool Vector3i::operator==(const Vector3i &a) const {
    return a.x == x && a.y == y && a.z == z;
}

void Vector3i::addD(const int dx, const int dy, const int dz, bool periodic, int mx, int my, int mz) {
    x += dx;
    y += dy;
    z += dz;
    if (periodic) {
        x = (x + mx) % mx;
        y = (y + my) % my;
        z = (z + mz) % mz;
    }
}

MyVector3D Vector3i::operator^(const double a) const {
    return MyVector3D(a * x, a * y, a * z);
}

Vector3i Vector3i::operator^(const int a) const {
    return Vector3i(a * x, a * y, a * z);
}

double Vector3i::operator*(const MyVector3D &v) const {
    return x * v.getX() + y * v.getY() + z * v.getZ();
}

Vector3i Vector3i::operator+(const Vector3i &v) const {
    return Vector3i(x + v.x, y + v.y, z + v.z);
}

Vector3i Vector3i::operator-(const Vector3i &v) const {
    return Vector3i(x - v.x, y - v.y, z - v.z);
}

MyVector3D Vector3i::toMyVector3D() {
    return MyVector3D(x, y, z);
}

Vector3i Vector3i::reflect(int width, int height, int length) {
    int newX(x);
    if (newX < 0) {
        newX = width - 1;
    } else if (newX > width - 1) {
        newX = 0;
    }
    int newY(y);
    if (newY < 0) {
        newY = height - 1;
    } else if (newY > height - 1) {
        newY = 0;
    }
    int newZ(z);
    if (newZ < 0) {
        newZ = length - 1;
    } else if (newZ > length - 1) {
        newZ = 0;
    }
    return Vector3i(newX, newY, newZ);
}

bool Vector3i::bounds(const int width, const int height, const int length) const {
    return x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < length;
}
