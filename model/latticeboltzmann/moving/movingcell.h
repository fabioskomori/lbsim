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

#ifndef MOVINGLATTICE_H
#define MOVINGLATTICE_H

class Grid;
#include "../../../model/latticeboltzmann/basecell.h"
class Grid;
class MovingParticle;
class SomethingMoving;

class MovingCell : public BaseCell {
public:
    MovingCell(double p0, int charge, Grid *grid);
    ~MovingCell();
    void init();
    void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    void postUpdate(Grid *grid, Vector3i position);
    void reset(double p0, double p1);
    double getNextF(int index, int component);
    double getF(int index, int);
    SomethingMoving* getParticle();
    void setParticle(SomethingMoving *particle);
    int getCharge();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
    void setNextF(int index, double value, int component);
    void setMulticomponent(int multicomponent);
    double getP(int index);
    MyVector3D getU(int index);
    double deltaP();
    double getLastDeltaP();
    MyVector3D getForcesVelocity(int component);
    bool isFluid();
    int getOpenCLType();
    BaseCell* clone();
private:
    SomethingMoving* particle;
    Grid *grid;
    int charge;
    double *nextF;
    double p0;
};

#endif // MOVINGLATTICE_H
