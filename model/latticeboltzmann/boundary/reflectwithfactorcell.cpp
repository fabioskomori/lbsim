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

#include "reflectwithfactorcell.h"
#include "../../util/shared.h"
#include "../../latticeboltzmann/gridconfig.h"
#include "../../latticeboltzmann/lbutil.h"
#include <QXmlStreamWriter>

ReflectWithFactorCell::ReflectWithFactorCell() {
}

int ReflectWithFactorCell::getOpenCLType() {
    return 7;
}

void ReflectWithFactorCell::setNextF(int index, double value, int component) {
    int model = Shared::instance()->getGridConfig()->getModel();
    if (neighbors[LBUtil::OPPOSITE[model][index]] != 0) {
        neighbors[LBUtil::OPPOSITE[model][index]]->setNextF(LBUtil::OPPOSITE[model][index], value * (component < 10 ? 1 : Shared::instance()->getGridConfig()->getReflectWithFactor()), component);
    }
}

void ReflectWithFactorCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "reflectWithFactor");
}
