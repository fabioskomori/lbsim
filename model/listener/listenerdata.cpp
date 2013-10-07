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

#include "listenerdata.h"

ListenerData::ListenerData(int width, int height, int length, long iterations, double particles, double deltaP, double deltaDeltaP, double averageP) :
        width(width), height(height), length(length), iterations(iterations), particles(particles), deltaP(deltaP), deltaDeltaP(deltaDeltaP), averageP(averageP) {
}

ListenerData ListenerData::createIterations(long iterations, double particles, double deltaP, double deltaDeltaP, double averageP) {
    return ListenerData(-1, -1, -1, iterations, particles, deltaP, deltaDeltaP, averageP);
}

ListenerData ListenerData::createSize(int width, int height, int length) {
    return ListenerData(width, height, length, -1, -1, -1, -1, -1);
}

long ListenerData::getIterations() const {
    return iterations;
}

double ListenerData::getParticles() const {
    return particles;
}

int ListenerData::getWidth() const {
    return width;
}

int ListenerData::getHeight() const {
    return height;
}

int ListenerData::getLength() const {
    return length;
}

double ListenerData::getDeltaP() const {
    return deltaP;
}

double ListenerData::getDeltaDeltaP() const {
    return deltaDeltaP;
}

double ListenerData::getAverageP() const {
    return averageP;
}
