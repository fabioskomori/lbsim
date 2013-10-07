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

#include "multicomponent.h"
#include "../../../model/latticeboltzmann/multi/mccell.h"
#include "../../../model/latticeboltzmann/porouscell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QDebug>
#include "../../../model/util/shared.h"

MultiComponent::MultiComponent() {
}

MyVector3D MultiComponent::computeForce(BaseCell *lattice, BaseCell* neighbors[], int index, int, int, int, Grid *grid) {
    int quantity, type;
    double g, gads1, gads2;
    GridConfig *config = Shared::instance()->getGridConfig();
    config->getMulticomponent(&quantity, &g, &gads1, &gads2, &type);
    MyVector3D resultG = MyVector3D();
    MyVector3D resultGads = MyVector3D();
    if (lattice->getP(index) > 1e-3 && (g > 1e-3 || g < -1e-3)) {
        for (int i = 1; i < config->getModel(); i++) {
            if (neighbors[i] != 0) {
                MCCell *mc = dynamic_cast<MCCell*>(neighbors[i]);
                PorousCell *porous = 0;
                if (mc != 0) {
                    porous = dynamic_cast<PorousCell*>(mc->getLattice(0));
                }
                for (int a = 0; a < quantity; a++) {
                    if (a != index) {
                        resultG = resultG + (LBUtil::C[config->getModel()][i] ^ (LBUtil::W[config->getModel()][i] * neighbors[i]->getP(a) / (porous != 0 ? grid->getConfig()->getSolidDensity() : 1)));
                    }
                }
            }
        }
        resultG = resultG ^ (-g * lattice->getP(index));
    }
    for (int i = 1; i < config->getModel(); i++) {
        WallCell *neighbor = dynamic_cast<WallCell*>(neighbors[i]);
        MCCell *mc = dynamic_cast<MCCell*>(neighbors[i]);
        PorousCell *porous = 0;
        if (mc != 0) {
            porous = dynamic_cast<PorousCell*>(mc->getLattice(0));
        }
        if (neighbor != 0 || porous != 0) {
            resultGads = resultGads + (LBUtil::C[config->getModel()][i] ^ (LBUtil::W[config->getModel()][i] * (porous != 0 ? grid->getConfig()->getSolidDensity() : 1)));
        }
    }
    resultGads = resultGads ^ (-(index == 0 ? gads1 : (index == 1 ? gads2 : 0)) * lattice->getP(index));
    return resultG + resultGads;
}

bool MultiComponent::hasEffect() {
    return true;
}

void MultiComponent::passivate(QXmlStreamWriter&) {
}

void MultiComponent::activate(QXmlStreamReader&, Grid*) {
}
