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

#ifndef STARTINGPARTICLE_H
#define STARTINGPARTICLE_H

#include "../../../model/math/vector3i.h"
class Particle;
#include <list>
class QXmlStreamWriter;
class QXmlStreamReader;
class InitialParticle;

class StartingParticle {
public:
    StartingParticle(int id, Vector3i from, Vector3i to, Vector3i color, Vector3i color2);
    std::list<InitialParticle*> createParticles(int gridSpacing);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    int getId();
    Vector3i getFrom();
    Vector3i getTo();
    Vector3i getColor();
private:
    int id;
    Vector3i from, to, color, color2;
};

#endif // STARTINGPARTICLE_H
