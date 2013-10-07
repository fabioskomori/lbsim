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

#ifndef PASSIVESCALARCELL_H
#define PASSIVESCALARCELL_H

#include "../../../model/latticeboltzmann/basecell.h"

class PassiveScalarCell : public BaseCell {
public:
    PassiveScalarCell(BaseCell *cell, double fixedConcentration, int index);
    ~PassiveScalarCell();
    virtual void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    virtual void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    virtual void update();
    void postUpdate(Grid *grid, Vector3i position);
    virtual void reset(double p0, double p1);
    virtual double getF(int index, int component);
    virtual void setNextF(int index, double value, int component);
    void setMulticomponent(int multicomponent);
    virtual void passivate(QXmlStreamWriter &writer);
    virtual void activate(QXmlStreamReader &reader, Grid *grid);
    double getP(int index);
    MyVector3D getU(int index);
    double deltaP();
    double getLastDeltaP();
    double getNextF(int index, int component);
    MyVector3D getForcesVelocity(int component);
    bool isFluid();
    int getOpenCLType();
    void updateFromOpenCL(double *data);
    virtual void initializeDistribution();
    void setFixedConcentration(double fixedConcentration);
    void setIndex(int index);
    BaseCell* getCell();
    virtual void reset(int id);
protected:
    virtual void calc();
    void clearMemory();
    BaseCell *cell;
    double *f, *nextF, *p;
    double fixedConcentration;
    int index;
};

#endif // PASSIVESCALARCELL_H
