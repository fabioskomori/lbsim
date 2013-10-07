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

#include "myquaternion.h"
#include <cmath>

MyQuaternion::MyQuaternion() : a(0.0f), b(0.0f), c(0.0f), d(0.0f) {
}

MyQuaternion::MyQuaternion(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {
}

void MyQuaternion::operator=(const MyQuaternion &operand) {
    a = operand.a;
    b = operand.b;
    c = operand.c;
    d = operand.d;
}

MyQuaternion MyQuaternion::normalize() {
    double temp = std::sqrt(a * a + b * b + c * c + d * d);
    return MyQuaternion(a / temp, b / temp, c / temp, d / temp);
}

MyQuaternion MyQuaternion::operator*(const MyQuaternion &operand) const {
    return MyQuaternion(a * operand.a - b * operand.b - c * operand.c - d * operand.d,
                    a * operand.b + b * operand.a + c * operand.d - d * operand.c,
                    a * operand.c - b * operand.d + c * operand.a + d * operand.b,
                    a * operand.d + b * operand.c - c * operand.b + d * operand.a);
}

MyQuaternion MyQuaternion::complement() const {
    return MyQuaternion(a, -b, -c, -d);
}

float MyQuaternion::value(unsigned int index) {
    switch (index) {
    case 0:
        return a;
    case 1:
        return b;
    case 2:
        return c;
    case 3:
        return d;
    default:
        return 0.0f;
    }
}

MyQuaternion MyQuaternion::rotate(const MyQuaternion &rotation) const {
    return rotation * (*this) * rotation.complement();
}

void MyQuaternion::rotationMatrix(float matrix[16]) {
    matrix[0] = (a * a + b * b - c * c - d * d);
    matrix[1] = (2.0f * b * c - 2.0f * a * d);
    matrix[2] = (2.0f * b * d + 2.0f * a * c);
    matrix[3] = 0.0f;
    matrix[4] = (2.0f * b * c + 2.0f * a * d);
    matrix[5] = (a * a - b * b + c * c - d * d);
    matrix[6] = (2.0f * c * d - 2.0f * a * b);
    matrix[7] = 0.0f;
    matrix[8] = (2.0f * b * d - 2.0f * a * c);
    matrix[9] = (2.0f * c * d + 2.0f * a * b);
    matrix[10] = (a * a - b * b - c * c + d * d);
    matrix[11] = 0.0f;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = 0.0;
    matrix[15] = 1.0;
}

double MyQuaternion::getRoll() {
    static double pi(3.14159);
    return std::atan2(2 * (a * b + c * d), 1 - 2 * (b * b + c * c)) * 180 / pi;
}

double MyQuaternion::getPitch() {
    static double pi(3.14159);
    return std::asin(2 * (a * c - b * d)) * 180 / pi;
}

double MyQuaternion::getYaw() {
    static double pi(3.14159);
    return std::atan2(2 * (a * d + b * c), 1 - 2 * (c * c + d * d)) * 180 / pi;
}
