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

#ifndef LISTENERDATA_H
#define LISTENERDATA_H

class ListenerData {
public:
    ListenerData(int width, int height, int length, long iterations, double particles, double deltaP, double deltaDeltaP, double averageP);
    static ListenerData createIterations(long iterations, double particles, double deltaP, double deltaDeltaP, double averageP);
    static ListenerData createSize(int width, int height, int length);
    long getIterations() const;
    double getParticles() const;
    int getWidth() const;
    int getHeight() const;
    int getLength() const;
    double getDeltaP() const;
    double getDeltaDeltaP() const;
    double getAverageP() const;
private:
    int width, height, length;
    long iterations;
    double particles;
    double deltaP, deltaDeltaP;
    double averageP;
};

#endif // LISTENERDATA_H
