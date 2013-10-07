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

#ifndef VELOCITYFIELD_H
#define VELOCITYFIELD_H

class Grid;
class Particle;
class StartingParticle;
#include <list>
class QXmlStreamWriter;
class QXmlStreamReader;
class Grid;
class InitialParticle;

class VelocityField {
public:
    VelocityField(Grid *grid);
    ~VelocityField();
    void addStartingPathLine(StartingParticle* pathLine);
    void remove(int id);
    void removeAll();
    void reset(int id);
    void resetAll();
    void setAddNew(bool addNew);
    StartingParticle* getStartingPathLine(int id);
    std::list<StartingParticle*>* getStartingPathLines();
    std::list<InitialParticle*>* getInitialParticles();
    void processPathLines();
    int getAcceleration();
    void setAcceleration(int acceleration);
    double getDiffusionEffect();
    void setDiffusionEffect(double diffusionEffect);
    int getGridSpacing();
    void setGridSpacing(int gridSpacing);
    int getInjectionPeriod();
    void setInjectionPeriod(int injectionPeriod);
    int getInjectionTime();
    void setInjectionTime(int injectionTime);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
private:
    void rebuildInitial();
    Grid *grid;
    std::list<StartingParticle*> *startingPathLines;
    std::list<InitialParticle*> *initialParticles;
    int acceleration;
    double diffusionEffect;
    int gridSpacing, injectionPeriod, injectionTime;
};

#endif // VELOCITYFIELD_H
