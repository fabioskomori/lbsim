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

#include "camera.h"
#include "../../model/math/myvector3d.h"
#include <QGLWidget>
#include <cmath>

Camera::Camera() {
    reset();
}

void Camera::reset() {
    rotation = MyQuaternion(1, 0, 0, 0);
    zoom = 1;
    translation = MyVector3D();
}

void Camera::resize(int w, int h, double dz) {
    this->w = w;
    this->h = h;
    if (dz > 0) {
        zoom = zoom * (1.0 + dz);
    } else {
        zoom = zoom / (1.0 - dz);
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h) {
        double plus = (double)(w - h) / h;
        glOrtho(zoom * (-0.5 - plus / 2), zoom * (0.5 + plus / 2), zoom * (-0.5), zoom * (0.5), -2, 2);
    } else {
        double plus = (double)(h - w) / w;
        glOrtho(zoom * (-0.5), zoom * (0.5), zoom * (-0.5 - plus / 2), zoom * (0.5 + plus / 2), -2, 2);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Camera::rotate(const MyVector3D from, const MyVector3D to) {
    MyVector3D axis = MyVector3D(from.getY() * to.getZ() - from.getZ() * to.getY(), from.getZ() * to.getX() - from.getX() * to.getZ(), from.getX() * to.getY() - from.getY() * to.getX()).normalize();
    double cosineHalfAngleSquared = ((from.getX() * to.getX() + from.getY() * to.getY() + from.getZ() * to.getZ()) + 1.0) * 0.5;
    double cosineHalfAngle = sqrt(cosineHalfAngleSquared);
    double sineHalfAngle = sqrt(1.0 - cosineHalfAngleSquared);
    rotation = (rotation * MyQuaternion(-cosineHalfAngle, axis.getX() * sineHalfAngle, axis.getY() * sineHalfAngle, axis.getZ() * sineHalfAngle)).normalize();
}

void Camera::setEuler(int roll, int pitch, int yaw) {
    static double pi(3.14159);
    rotation = MyQuaternion(1, 0, 0, 0);
    rotation = MyQuaternion(std::cos(roll * pi / 180.0 / 2), std::sin(roll * pi / 180.0 / 2), 0, 0) * rotation;
    rotation = MyQuaternion(std::cos(pitch * pi / 180.0 / 2), 0, std::sin(pitch * pi / 180.0 / 2), 0) * rotation;
    rotation = MyQuaternion(std::cos(yaw * pi / 180.0 / 2), 0, 0, std::sin(yaw * pi / 180.0 / 2)) * rotation;
    rotation = rotation.normalize();
}

void Camera::translate(const MyVector3D d) {
    translation = translation + (d ^ zoom);
}

void Camera::setTranslation(const MyVector3D d) {
    translation = d ^ zoom;
}

void Camera::setTranslation(double x, double y, double z) {
    translation = MyVector3D(x, y, z);
}

MyVector3D Camera::getTranslation() {
    return translation;
}

void Camera::prePaint() {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(translation.getX(), translation.getY(), translation.getZ());
    float rotationMatrix[16];
    rotation.rotationMatrix(rotationMatrix);
    glMultMatrixf(rotationMatrix);
}

void Camera::postPaint() {
    glPopMatrix();
}

double Camera::getZoom() {
    return zoom;
}

void Camera::setZoom(double zoom) {
    this->zoom = zoom;
    resize(w, h, 0);
}

MyQuaternion Camera::getRotation() {
    return rotation;
}
