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

#ifndef SCLATTICE_H
#define SCLATTICE_H

#include "../../model/math/myvector3d.h"
#include <list>
#include "basecell.h"
class Force;

class SCCell : public BaseCell {
public:
    SCCell(double p0);
    ~SCCell();
    void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    void postUpdate(Grid *grid, Vector3i position);
    void reset(double p0, double p1);
    double getNextF(int index, int component);
    void setNextF(int index, double value, int component);
    double getF(int index, int component);
    double getF0(int index, double epsilon, BaseCell* neighbors[], Grid *grid);
    double getP(int index);
    MyVector3D getU(int index);
    double deltaP();
    double getLastDeltaP();
    virtual void passivate(QXmlStreamWriter &writer);
    void passivateWithoutType(QXmlStreamWriter &writer);
    virtual void activate(QXmlStreamReader &reader, Grid *grid);
    void setF(int index, double value);
    void zero();
    void config(MyVector3D velocity, double p);
    MyVector3D getForcesVelocity(int component);
    void setMulticomponent(int multicomponent);
    bool isFluid();
    virtual int getOpenCLType();
    void updateFromOpenCL(double *data);
    BaseCell* clone();
    void setConstantConcentration(double constantConcentration);
    void setForcesVelocity(MyVector3D forcesVelocity);
protected:
    double gamma(double h, double cacheF0[], double min, double max, int step);
    double *f, *nextF;
    MyVector3D u, forcesVelocity;
    double p;
    double p0;
    double lastDeltaP;
    double constantConcentration;
};

#endif // SCLATTICE_H
