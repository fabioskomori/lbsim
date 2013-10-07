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

#include "multiphase.h"
#include "../../../model/latticeboltzmann/force/force.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/porouscell.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <cmath>
#include <QDebug>
#include "../../../model/util/shared.h"

MultiPhase::MultiPhase() {
}

MyVector3D MultiPhase::computeForce(BaseCell *lattice, BaseCell* neighbors[], int, int, int, int, Grid *grid) {
    GridConfig *config = Shared::instance()->getGridConfig();
    double mpG, mpPsi0, mpP0, mpGads;
    config->getMultiphase(&mpG, &mpPsi0, &mpP0, &mpGads);
    MyVector3D multiphase = MyVector3D(0, 0, 0);
    // interparticle
    for (int i = 1; i < config->getModel(); i++) {
        if (neighbors[i] != 0) {
            multiphase = multiphase + (LBUtil::C[config->getModel()][i] ^ (LBUtil::W[config->getModel()][i] * computePsi0(neighbors[i], grid)));
        }
    }
    multiphase = multiphase ^ (-mpG * computePsi0(lattice, grid));
    // adhesive
    MyVector3D adhesive = MyVector3D(0, 0, 0);
    for (int i = 1; i < config->getModel(); i++) {
        if (neighbors[i] != 0) {
            WallCell *wall = dynamic_cast<WallCell*>(neighbors[i]);
            PorousCell *porous = dynamic_cast<PorousCell*>(neighbors[i]);
            if (wall != 0 || porous != 0) {
                adhesive = adhesive + (LBUtil::C[config->getModel()][i] ^ (LBUtil::W[config->getModel()][i] * (porous != 0 ? grid->getConfig()->getSolidDensity() : 1)));
            }
        }
    }
    adhesive = adhesive ^ (-mpGads * computePsi0(lattice, grid));
    return multiphase + adhesive;
}

double MultiPhase::computePsi0(BaseCell *lattice, Grid*) {
    if (lattice != 0 && lattice->isFluid()) {
        GridConfig *config = Shared::instance()->getGridConfig();
        double mpG, mpPsi0, mpP0, mpGads;
        config->getMultiphase(&mpG, &mpPsi0, &mpP0, &mpGads);
        PorousCell *porous = dynamic_cast<PorousCell*>(lattice);
        return mpPsi0 * exp(-mpP0 / ((lattice->getP(-1) / (porous != 0 ? 1 : 1))));
        //return mpP0 * (1 - exp(-empty->getP() / mpP0));
    }
    return 0;
}

bool MultiPhase::hasEffect() {
    GridConfig *config = Shared::instance()->getGridConfig();
    double mpG, mpPsi0, mpP0, mpGads;
    config->getMultiphase(&mpG, &mpPsi0, &mpP0, &mpGads);
    return abs(mpG) > 1e-100;
}

void MultiPhase::passivate(QXmlStreamWriter&) {
}

void MultiPhase::activate(QXmlStreamReader&, Grid *) {
}
