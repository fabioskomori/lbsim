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

#ifndef BASELATTICE_H
#define BASELATTICE_H

class QXmlStreamReader;
class QXmlStreamWriter;
class Vector3i;
class MyVector3D;
class Force;
class Grid;
#include <typeinfo>

class BaseCell {
public:
    virtual ~BaseCell();
    virtual void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) = 0;
    virtual void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) = 0;
    virtual void update() = 0;
    virtual void postUpdate(Grid *grid, Vector3i position) = 0;
    virtual void reset(double p0, double p1) = 0;
    virtual double getF(int index, int component) = 0;
    virtual void setNextF(int index, double value, int component) = 0;
    virtual void setMulticomponent(int multicomponent) = 0;
    virtual void passivate(QXmlStreamWriter &writer);
    virtual void activate(QXmlStreamReader &reader, Grid *grid);
    virtual double getP(int index) = 0;
    virtual MyVector3D getU(int index) = 0;
    virtual double deltaP() = 0;
    virtual double getLastDeltaP() = 0;
    virtual double getNextF(int index, int component) = 0;
    virtual MyVector3D getForcesVelocity(int component) = 0;
    virtual bool isFluid() = 0;
    virtual int getOpenCLType() = 0;
    virtual void updateFromOpenCL(double *data);
    virtual BaseCell* clone();
};

#endif // BASELATTICE_H
