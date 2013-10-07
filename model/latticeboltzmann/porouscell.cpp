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

#include "porouscell.h"
#include "lbutil.h"
#include "gridconfig.h"
#include "lbgrid.h"
#include "boundary/wallcell.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../model/util/shared.h"

int PorousCell::getOpenCLType() {
    return 9;
}

PorousCell::PorousCell(double p0) : SCCell(p0) {
    solidDensity = Shared::instance()->getGridConfig()->getSolidDensity();
}

void PorousCell::postUpdate(Grid *grid, Vector3i position) {
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        Vector3i neighbor = position + LBUtil::C[model][i];
        BaseCell *neighborCell = grid->getGrid(neighbor.getY(), neighbor.getX(), neighbor.getZ());
        if (dynamic_cast<WallCell*>(neighborCell) == 0) {
            f[i] = f[i] + solidDensity * (neighborCell->getF(LBUtil::OPPOSITE[model][i], index) - f[i]);
        }
    }
    LBUtil::calc_pu(f, model, &p, &u);
}

void PorousCell::passivate(QXmlStreamWriter &writer) {
    SCCell::passivate(writer);
    writer.writeAttribute("type", "porous");
    writer.writeAttribute("solidDensity", QString::number(solidDensity));
}

void PorousCell::activate(QXmlStreamReader &reader, Grid *grid) {
    SCCell::activate(reader, grid);
    solidDensity = reader.attributes().value("solidDensity").toString().toDouble();
}

double PorousCell::getSolidDensity() {
    return solidDensity;
}

void PorousCell::setSolidDensity(double solidDensity) {
    this->solidDensity = solidDensity;
}

BaseCell* PorousCell::clone() {
    PorousCell *result = new PorousCell(p0);
    for (int i = 0; i < Shared::instance()->getGridConfig()->getModel(); i++) {
        result->setF(i, f[i]);
    }
    return result;
}

void PorousCell::setIndex(int index) {
    this->index = index;
}
