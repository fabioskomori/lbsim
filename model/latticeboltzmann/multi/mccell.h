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

#ifndef MCLATTICE_H
#define MCLATTICE_H

#include "../../../model/latticeboltzmann/basecell.h"
#include "../../../model/math/myvector3d.h"
class SCCell;
class GridConfig;
class MultiComponent;
#include <list>
class GridConfig;

class MCCell : public BaseCell {
public:
    MCCell(SCCell *from = 0);
    ~MCCell();
    void preUpdate(double epsilon[], BaseCell *neighbors[], Grid *grid, Vector3i position);
    void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    void postUpdate(Grid *grid, Vector3i position);
    void reset(double p1, double p2);
    double getF(int index);
    double getF0(int index, double epsilon, BaseCell *neighbors[]);
    double deltaP();
    double getLastDeltaP();
    void config(unsigned int quantity, SCCell *from = 0);
    MyVector3D getCompositeVelocity();
    void setNextF(int index, double value, int component);
    double getP(int index);
    MyVector3D getU(int index);
    double getF(int index, int component);
    double getF0(int index, double epsilon, BaseCell *neighbors[], int component);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
    void setF(int index, double value);
    void setMulticomponent(int multicomponent);
    MultiComponent* getMultiForce();
    MyVector3D getForcesVelocity(int component);
    double getNextF(int index, int component);
    bool isFluid();
    int getOpenCLType();
    void updateFromOpenCL(double *data);
    void setConstantConcentration(int index, double value);
    SCCell* getLattice(int index);
private:
    void calcCompositeVelocity();
    std::list<SCCell*> lattices;
    double *epsilons;
    MyVector3D compositeVelocity, *forcesVelocity;
    double lastDeltaP;
    MultiComponent *force;
    double *constantConcentration;
};

#endif // MCLATTICE_H
