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

#ifndef INITIALPARTICLE_H
#define INITIALPARTICLE_H

#include "../../../model/math/vector3i.h"
#include <list>
class Particle;
class Grid;

class InitialParticle {
public:
    InitialParticle(Vector3i position, Vector3i color, int id);
    ~InitialParticle();
    int getId();
    void process(Grid *grid);
    void reset();
    std::list<Particle*>* getStreakLines();
private:
    Vector3i position;
    Vector3i color;
    int id;
    std::list<Particle*> *streakLines;
};

#endif // INITIALPARTICLE_H
