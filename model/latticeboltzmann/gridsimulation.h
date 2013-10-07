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

#ifndef GRIDSIMULATION_H
#define GRIDSIMULATION_H

class QXmlStreamWriter;
class QXmlStreamReader;

class GridSimulation {
public:
    GridSimulation();
    void resetIterations();
    long addIteration(int steps = 1);
    long getIterations();
    void addDeltasP();
    void removeDeltasP();
    int getDeltasP();
    void resetDeltasP();
    void resetDeltaP();
    void addDeltaP(double deltaP);
    void calcDeltaP();
    double getDeltaP();
    void resetTotalP();
    void addTotalP(double totalP);
    double getTotalP();
    void setPerformance(double performance);
    double getPerformance();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
private:
    long iterations;
    int deltasP;
    double deltaP;
    double totalP;
    double performance;
};

#endif // GRIDSIMULATION_H
