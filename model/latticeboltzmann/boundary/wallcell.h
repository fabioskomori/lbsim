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

#ifndef WALLLATTICE_H
#define WALLLATTICE_H

#include "../basecell.h"

class WallCell : public BaseCell {
public:
    WallCell();
    ~WallCell();
    virtual void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    virtual void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    virtual void update();
    virtual void postUpdate(Grid *grid, Vector3i position);
    virtual void reset(double p0, double p1);
    virtual double getF(int index);
    virtual void passivate(QXmlStreamWriter &writer);
    virtual void setMulticomponent(int multicomponent);
    virtual double getF(int index, int component);
    virtual double getF0(int index, double epsilon, BaseCell* neighbors[], int component);
    virtual void setNextF(int index, double value, int component);
    virtual void injectNeighbors(BaseCell** neighbors);
    virtual double getP(int index);
    virtual MyVector3D getU(int index);
    virtual double deltaP();
    virtual double getLastDeltaP();
    virtual double getNextF(int index, int component);
    virtual MyVector3D getForcesVelocity(int component);
    virtual bool isFluid();
    virtual int getOpenCLType();
    BaseCell** getNeighbors();
    BaseCell* clone();
protected:
    /*void updateF(int model, int multicomponent);
    int model;
    double f[54], nextF[54];
    double p0;
    int multicomponent;*/
    BaseCell** neighbors;
};

#endif // WALLLATTICE_H
