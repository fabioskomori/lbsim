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

#ifndef MOVINGPARTICLE_H
#define MOVINGPARTICLE_H

#include <list>
#include "../../../model/math/myvector3d.h"
#include "../../../model/math/vector3i.h"
#include "particlemanager.h"
#include "movingcell.h"
#include "somethingmoving.h"
class ParticleManager;
class MovingCell;

class MovingParticle : public SomethingMoving {
public:
    MovingParticle(ParticleManager *particleManager);
    ~MovingParticle();
    void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    bool addPoint(Vector3i *point, int charge);
    void addBorderEffect(BaseCell* neighbors[], MovingCell *moving);
    void processMovement();
    void processSmallParticle(Grid *grid);
    bool tryMerge(MovingParticle *moving);
    std::list<Vector3i*>* getPoints();
    MyVector3D getD();
    void updateD(int x, int y, int z, Grid *grid, bool updatePoints = false);
    void needsGridChange(int *dx, int *dy, int *dz);
    void setRelativeDensity(double relativeDensity);
    double getRelativeDensity();
    int getCharge();
    void processCharge();
    int getTotalCharge();
    MyVector3D getVelocity();
    void setVelocity(MyVector3D velocity);
    MyVector3D getMassCenter();
    MyVector3D getElectrostaticAcceleration();
    MyVector3D getLastMovementQuantity();
    void setTemperatureCoefficient(double temperatureCoefficient);
    double getTemperatureCoefficient();
    void setFrozen(bool frozen);
    bool getFrozen();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    double getElectricPotential();
    SomethingMoving* clone();
private:
    std::list<Vector3i*> *points;
    double dx, dy, dz;
    int gridx, gridy, gridz;
    MyVector3D movementQuantity, lastMovementQuantity;
    MyVector3D electrostaticAcceleration;
    double relativeDensity;
    int charge;
    ParticleManager *particleManager;
    bool chargeProcessed;
    bool movementProcessed;
    double temperatureCoefficient;
    MyVector3D velocity;
    bool frozen;
    double electricPotential;
};

#endif // MOVINGPARTICLE_H
