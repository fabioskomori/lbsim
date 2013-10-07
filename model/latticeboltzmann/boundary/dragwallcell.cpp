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

#include "dragwallcell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/math/vector3i.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/basecell.h"

DragWallCell::DragWallCell() {
    index = -1;
}

void DragWallCell::activate(QXmlStreamReader &reader, Grid *) {
    index = reader.attributes().value("index").toString().toInt();
}

void DragWallCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "dragWall");
    writer.writeAttribute("index", QString::number(index));
}

void DragWallCell::setNextF(int index, double value, int component) {
    WallCell::setNextF(index, value, component);
}

void DragWallCell::update() {
}

MyVector3D DragWallCell::getDragForce(Grid *grid, Vector3i position) {
    MyVector3D drag;
    BaseCell **neighbors = grid->getNeighbors(position.getX(), position.getY(), position.getZ());
    for (int i = 0; i < grid->getConfig()->getModel(); i++) {
        if (neighbors[i] != 0) {
            drag = drag + (LBUtil::C[grid->getConfig()->getModel()][i] ^ (2 * neighbors[i]->getF(LBUtil::OPPOSITE[grid->getConfig()->getModel()][i], 0)));
        }
    }
    return drag;
}

int DragWallCell::getIndex() {
    return index;
}

void DragWallCell::setIndex(int index) {
    this->index = index;
}
