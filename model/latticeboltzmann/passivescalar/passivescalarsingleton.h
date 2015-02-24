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

#ifndef PASSIVESCALARSINGLETON_H
#define PASSIVESCALARSINGLETON_H

class Vector3i;
class Grid;
#include <list>
class Configuration;
class QXmlStreamWriter;
class QXmlStreamReader;

class PassiveScalarSingleton {
public:
    static PassiveScalarSingleton* getInstance();
    void inject(Grid *grid);
    void remove(int id);
    void removeAll();
    void add(int id, int minx, int maxx, int miny, int maxy, int minz, int maxz, Vector3i color, int type);
    Configuration* get(int id);
    std::list<Configuration*> listPSS();
    void reset(int id);
    void resetAll();
    int getColorsQuantity();
    Vector3i* getColors();
    double getEpsilon(int index);
    void setEpsilon(int index, double value);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
    void setInjectionPeriod(int injectionPeriod);
    int getInjectionPeriod();
    void setInjectionTime(int injectionTime);
    int getInjectionTime();
    int getLastType();
    void setInitialConcentration(double initialConcentration);
    double getInitialConcentration();
    void setFixedConcentration(double fixedConcentration);
    double getFixedConcentration();
private:
    PassiveScalarSingleton();
    void rebuild(int type = 1);
    Grid *grid;
    std::list<Configuration*> pss;
    Vector3i *colors;
    double epsilon;
    int injectionPeriod, injectionTime, lastType;
    double initialConcentration, fixedConcentration;
};

#endif // PASSIVESCALARSINGLETON_H
