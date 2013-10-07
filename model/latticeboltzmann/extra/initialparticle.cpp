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

#include "initialparticle.h"
#include "particle.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/basecell.h"
#include "velocityfield.h"
#include <cstdlib>

InitialParticle::InitialParticle(Vector3i position, Vector3i color, int id) : position(position), color(color), id(id) {
    streakLines = new std::list<Particle*>();
    streakLines->push_back(new Particle(position.toMyVector3D(), color, id, position.toMyVector3D()));
}

InitialParticle::~InitialParticle() {
    delete streakLines;
}

std::list<Particle*>* InitialParticle::getStreakLines() {
    return streakLines;
}

void InitialParticle::process(Grid *grid) {
    std::list<Particle*> *newPathLines = new std::list<Particle*>();
    newPathLines->push_back(new Particle(position.toMyVector3D(), color, id, position.toMyVector3D()));
    while (!streakLines->empty()) {
        Particle* pathLine = streakLines->front();
        MyVector3D current = pathLine->getPosition();
        BaseCell *lattice = grid->getGrid(current.getY(), current.getX(), current.getZ());
        if (lattice != 0 && lattice->isFluid()) {
            double randX = (((double)rand()) / RAND_MAX * 2 - 1) * grid->getVelocityField()->getDiffusionEffect();
            double randY = (((double)rand()) / RAND_MAX * 2 - 1) * grid->getVelocityField()->getDiffusionEffect();
            Particle *newParticle = new Particle(MyVector3D(current.getX() + lattice->getU(-1).getX() * grid->getVelocityField()->getAcceleration() + randX,
                                                          current.getY() + lattice->getU(-1).getY() * grid->getVelocityField()->getAcceleration() + randY,
                                                          current.getZ() + lattice->getU(-1).getZ() * grid->getVelocityField()->getAcceleration()), pathLine->getColor(), pathLine->getId(), current);
            newPathLines->push_back(newParticle);
        }
        delete pathLine;
        streakLines->pop_front();
    }
    delete streakLines;
    streakLines = newPathLines;
}

void InitialParticle::reset() {
    streakLines->clear();
    streakLines->push_back(new Particle(position.toMyVector3D(), color, id, position.toMyVector3D()));
}

int InitialParticle::getId() {
    return id;
}
