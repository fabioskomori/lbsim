#include "meltingsolidificationcell.h"
#include "../../math/vector3i.h"
#include "../lbgrid.h"
#include "../gridconfig.h"
#include "../../util/shared.h"
#include "../lbutil.h"
#include "kornerimplementation.h"
#include <QDebug>
#include <QXmlStreamWriter>

#define EPSILON 0.01

MeltingSolidificationCell::MeltingSolidificationCell(KornerImplementation *ki) {
    this->ki = ki;
}

void MeltingSolidificationCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    int model = Shared::instance()->getGridConfig()->getModel();
    switch (type) {
    case 'I':
        nextCellMass = cellMass;
        for (int i = 1; i < model; i++) {
            MeltingSolidificationCell* cell = dynamic_cast<MeltingSolidificationCell*>(neighbors[i]);
            if (cell != 0) {
                switch (cell->type) {
                case 'L':
                    nextCellMass += cell->f[LBUtil::OPPOSITE[9][i]] - f[i];
                    break;
                case 'I':
                    nextCellMass += 0.5 * (cellMass / p + cell->cellMass / cell->p) * (cell->f[LBUtil::OPPOSITE[9][i]] - f[i]);
                    break;
                }
            }
        }
    case 'L':
    case 'G':
        for (int i = 0; i < model; i++) {
            MeltingSolidificationCell* cell = dynamic_cast<MeltingSolidificationCell*>(neighbors[LBUtil::OPPOSITE[model][i]]);
            if (cell != 0) {
                MyVector3D g2 = MyVector3D(0, -grid->getConfig()->getPassiveScalarGravity(), 0)/* ^ (grid->getConfig()->getNanoBeta() * grid->getConfig()->getNanoEpsilon0() * (cell->T) / (cell->p * grid->getConfig()->getNanoDensity()))*/;
                MyVector3D u2 = cell->u;
                if (cell->type == 'L') u2 = cell->u + g2;
                switch (cell->type) {
                /*case 'L':
                case 'I':
                case 'G':
                    switch (type) {
                    case 'I':
                        nextF[i] = cell->f[i] * (1 - 1.0 / grid->getConfig()->getNanoEpsilon0()) + 1.0 / grid->getConfig()->getNanoEpsilon0() * LBUtil::f_eq(u2, cell->p, model, i);
                        break;
                    case 'L':
                        break;
                    case 'G':
                        break;
                    }
                    break;*/
                case 'S':
                case 'W':
                    nextF[i] = f[LBUtil::OPPOSITE[model][i]];
                    break;
                default:
                    if ((type == 'I' && (cell->type == 'I' || cell->type == 'L')) || (type == 'L' && (cell->type == 'L' || cell->type == 'I')) || (type == 'G' && cell->type == 'G')) {
                        nextF[i] = cell->f[i] * (1 - 1.0 / grid->getConfig()->getNanoEpsilon0()) + 1.0 / grid->getConfig()->getNanoEpsilon0() * LBUtil::f_eq(u2, cell->p, model, i);
                    } else if (type == 'G' && cell->type == 'I') {
                        nextF[i] = f[LBUtil::OPPOSITE[model][i]];
                    } else if (type == 'I' && cell->type == 'G') {
                        // DUVIDA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        nextF[i] = 2 * LBUtil::f_eq(u2, cell->p, 9, i) - f[LBUtil::OPPOSITE[model][i]];
                        //nextF[i] = f[LBUtil::OPPOSITE[model][i]];
                    } else {
                        qDebug() << "erro: de " << cell->type << " para " << type;
                    }
                }
            }
        }
        break;
    case 'S':
    case 'W':
        // does nothing
        for (int i = 0; i < model; i++) {
            nextF[i] = f[i];
        }
        break;
    }
    if (type == 'W') {
        for (int i = 0; i < model; i++) {
            nextG[i] = g[i];
        }
    } else {
        double tau = 1;
        switch (type) {
        case 'I':
            tau = (cellMass / p) * ki->getLiquidTau() + (1 - cellMass / p) * ki->getGasTau();
            //tau = ki->getLiquidTau();
            break;
        case 'L':
            tau = ki->getLiquidTau();
            break;
        case 'S':
            tau = ki->getSolidTau();
            break;
        case 'G':
            tau = ki->getGasTau();
            break;
        }
        for (int i = 0; i < 9; i++) {
            MeltingSolidificationCell* cell = dynamic_cast<MeltingSolidificationCell*>(neighbors[LBUtil::OPPOSITE[model][i]]);
            if (cell != 0) {
                switch (cell->getType()) {
                case 'W':
                    nextG[i] = g[LBUtil::OPPOSITE[model][i]];
                    break;
                case 'I':
                case 'L':
                case 'S':
                case 'G':
                    nextG[i] = cell->g[i] * (1 - 1.0 / tau) + 1.0 / tau * LBUtil::f_eq(cell->u, cell->T, model, i);
                    break;
                }
            }
        }
    }
}

void MeltingSolidificationCell::preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    double energy = 0;
    for (int i = 0; i < 9; i++) {
        energy += g[i];
    }
    if (type == 'I') {
        double p = 0;
        for (int i = 0; i < 9; i++) {
            p += f[i];
        }
        if (nextCellMass < -EPSILON || nextCellMass > p + EPSILON) {
            if (nextCellMass < -EPSILON) {
                type = 'G';
                nextCellMass = 0;
            } else {
                type = 'L';
                nextCellMass = p;
            }
            for (int i = 1; i < 9; i++) {
                MeltingSolidificationCell *msc = dynamic_cast<MeltingSolidificationCell*>(neighbors[i]);
                if (msc != 0) {
                    msc->convertToInterface(type);
                }
            }
        }
    } else if (type == 'S') {
        if (energy > ki->getLiquidEnergy()) {
            bool anyGas = false;
            for (int i = 1; i < 9; i++) {
                MeltingSolidificationCell *msc = dynamic_cast<MeltingSolidificationCell*>(neighbors[i]);
                if (msc != 0) {
                    if (msc->getType() == 'G') {
                        anyGas = true;
                        break;
                    }
                }
            }
            if (anyGas) {
                type = 'I';
            } else {
                type = 'L';
            }
            nextCellMass = 0.95 * p;
        }
    } else if (type == 'L') {
        // solidification
    }
}

void MeltingSolidificationCell::update() {
    if (type == 'I') {
        cellMass = nextCellMass;
    }
    int model = Shared::instance()->getGridConfig()->getModel();
    for (int i = 0; i < model; i++) {
        f[i] = nextF[i];
        g[i] = nextG[i] + LBUtil::W[9][i] * absorbedEnergy;
    }
    LBUtil::calc_pu(g, model, &T, &u);
    LBUtil::calc_pu(f, model, &p, &u);
}

void MeltingSolidificationCell::postUpdate(Grid *grid, Vector3i position) {
}

void MeltingSolidificationCell::reset(double p0, double p1) {
    LBUtil::f_eq(MyVector3D(), p1 != -1 ? p1 : 1, 9, g);
    LBUtil::calc_pu(g, 9, &T, &u);
    /*if (type == 'G') {
        LBUtil::f_eq(MyVector3D(), 0.9, 9, f);
    } else {*/
        LBUtil::f_eq(MyVector3D(), p0 != -1 ? p0 : 1, 9, f);
    //}
    LBUtil::calc_pu(f, 9, &p, &u);
}

double MeltingSolidificationCell::getF(int index, int component) {
    return component == 1 ? g[index] : f[index];
}

void MeltingSolidificationCell::setNextF(int index, double value, int component) {
}

void MeltingSolidificationCell::setMulticomponent(int) {
}

void MeltingSolidificationCell::passivate(QXmlStreamWriter &writer) {
}

void MeltingSolidificationCell::activate(QXmlStreamReader &reader, Grid *grid) {
}

double MeltingSolidificationCell::getP(int index) {
    if (index == 1) {
        return T;
    }
    switch (type) {
    case 'G':
    case 'L':
    case 'S':
        return p;
    case 'I':
        return cellMass;
    }
    return 0;
}

MyVector3D MeltingSolidificationCell::getU(int index) {
    return u;
}

double MeltingSolidificationCell::deltaP() {
    return 0;
}

double MeltingSolidificationCell::getLastDeltaP() {
    return 0;
}

double MeltingSolidificationCell::getNextF(int index, int component) {
    return component == 1 ? nextG[index] : nextF[index];
}

MyVector3D MeltingSolidificationCell::getForcesVelocity(int component) {
    return MyVector3D();
}

bool MeltingSolidificationCell::isFluid() {
    return true;
}

int MeltingSolidificationCell::getOpenCLType() {
    return -1;
}

void MeltingSolidificationCell::updateFromOpenCL(double *data) {
}

BaseCell* MeltingSolidificationCell::clone() {
    return new MeltingSolidificationCell(ki);
}

void MeltingSolidificationCell::setType(char type) {
    this->type = type;
    if (type == 'G') {
        cellMass = 0;
    } else if (type == 'L') {
        cellMass = 0;
    } else if (type == 'S') {
        cellMass = 0;
    } else if (type == 'I') {
        cellMass = 0.5;
    } else if (type == 'W') {
        cellMass = 0;
    }
}

void MeltingSolidificationCell::convertToInterface(char newType) {
    if (type == 'G' && type != newType) {
        type = 'I';
        nextCellMass = 0;
    }
    if (type == 'L' && type != newType) {
        type = 'I';
        nextCellMass = p;
    }
}

char MeltingSolidificationCell::getType() {
    return type;
}

double MeltingSolidificationCell::getCellMass() {
    return cellMass;
}

double MeltingSolidificationCell::getBeamEnergy() {
    return beamEnergy;
}

void MeltingSolidificationCell::setBeamEnergy(double beamEnergy) {
    this->beamEnergy = beamEnergy;
}

double MeltingSolidificationCell::getAbsorbedEnergy() {
    return absorbedEnergy;
}

void MeltingSolidificationCell::setAbsorbedEnergy(double absorbedEnergy) {
    this->absorbedEnergy = absorbedEnergy;
}
