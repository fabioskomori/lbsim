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

#ifndef SOURCELATTICE_H
#define SOURCELATTICE_H

#include "../../../model/latticeboltzmann/basecell.h"
class Boundary;

class SourceCell : public BaseCell {
public:
    static SourceCell* createEquilibrium(double velocity);
    static SourceCell* createFixed(double velocity, double p);
    static SourceCell* createPoint(double p);
    static SourceCell* createZouHe(double velocity);
    static SourceCell* createNull();
    void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    void postUpdate(Grid *grid, Vector3i position);
    void reset(double p0, double p1);
    double getF(int index);
    ~SourceCell();
    void setIndex(int index);
    int getIndex();
    Boundary* getImpl();
    void setImpl(Boundary *boundary);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
    double getF(int index, int component);
    double getF0(int index, double epsilon, BaseCell* neighbors[], int component);
    void setMulticomponent(int multicomponent);
    void setNextF(int index, double value, int component);
    double getP(int index);
    MyVector3D getU(int index);
    double deltaP();
    double getLastDeltaP();
    double getNextF(int index, int component);
    MyVector3D getForcesVelocity(int component);
    bool isFluid();
    int getOpenCLType();
private:
    SourceCell();
    Boundary *boundary;
    int index;
};

#endif // SOURCELATTICE_H
