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

#include "streamlinegenerator.h"
#include "streamlinealgorithm.h"

StreamLineGenerator::StreamLineGenerator(Grid *grid) : grid(grid) {
}

std::map<QString, QString> StreamLineGenerator::getProperties() {
    return properties;
}

void StreamLineGenerator::setProperty(QString name, QString value) {
    properties[name] = value;
    propertySetted(name, value);
}

void StreamLineGenerator::propertySetted(QString , QString ) {
}

void StreamLineGenerator::injectAlgorithm(StreamLineAlgorithm *algorithm) {
    this->algorithm = algorithm;
}
