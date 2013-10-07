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

#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include <list>
#include <map>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
class MovingCell;
class MovingParticle;
class Grid;

class ParticleManager {
public:
    ParticleManager(Grid *grid);
    ~ParticleManager();
    void preUpdate();
    void update();
    void transformSmallParticles();
    void onLatticeAdded(int i, int j, int k, MovingCell *lattice);
    void onLatticeRemoved(int i, int j, int k, MovingCell *lattice);
    std::list<MovingParticle*>* getParticles();
    double getElectrostaticCoefficient();
    void setElectrostaticCoefficient(double electrostaticCoefficient);
    double getGravityCoefficient();
    void setGravityCoefficient(double gravityCoefficient);
    void reset();
    void setFrozen(bool frozen);
    double getTemp();
    void setNormalizeVelocities(bool normalizeVelocities);
    bool getNormalizeVelocities();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    int getParticlesMap(MovingParticle *particle);
    MovingParticle* getParticlesMap(int position);
private:
    Grid *grid;
    std::list<MovingParticle*> *particles;
    std::list<MovingParticle*> *smallParticles;
    double electrostaticCoefficient;
    double gravityCoefficient;
    double temp;
    bool removeOrphanParticles;
    bool normalizeVelocities;
};

#endif // PARTICLEMANAGER_H
