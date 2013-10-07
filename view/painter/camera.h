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

#ifndef CAMERA_H
#define CAMERA_H

#include "../../model/math/myquaternion.h"
#include "../../model/math/myvector3d.h"

class Camera {
public:
    Camera();
    void resize(int w, int h, double dz = 0);
    void rotate(const MyVector3D from, const MyVector3D to);
    void translate(const MyVector3D d);
    void prePaint();
    void postPaint();
    void reset();
    double getZoom();
    void setZoom(double zoom);
    void setEuler(int roll, int pitch, int yaw);
    void setTranslation(const MyVector3D d);
    void setTranslation(double x, double y, double z);
    MyVector3D getTranslation();
    MyQuaternion getRotation();
private:
    MyQuaternion rotation;
    double zoom;
    int w, h;
    MyVector3D translation;
};

#endif // CAMERA_H
