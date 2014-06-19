#include "kornerimplementation.h"
#include "../lbgrid.h"
#include "../gridconfig.h"
#include "../gridsimulation.h"
#include "meltingsolidificationcell.h"
#include <cmath>
#include <QDebug>

#define PI 3.14159265

KornerImplementation::KornerImplementation(Grid *grid) {
    this->grid = grid;
    power = 0.5;
    standardDeviation = 0.1;
    currentX = startX = 0;
    velocity = 0.01;
    timesteps = 10000;
    absorptionCoefficient = 0.5;
    solidTau = 0.65;
    liquidTau = 0.7;
    gasTau = 1;
    solidEnergy = 1.5;
    liquidEnergy = 2;
}

void KornerImplementation::preUpdate() {
    if (grid->getConfig()->getKorner()) {
        for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
            for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
                MeltingSolidificationCell *msc = dynamic_cast<MeltingSolidificationCell*>(grid->getGrid(y, x, 0));
                if (msc != 0) {
                    msc->setBeamEnergy(0);
                    msc->setAbsorbedEnergy(0);
                }
            }
        }
        if (grid->getSimulation()->getIterations() < timesteps) {
            currentX += velocity;
            for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
                for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
                    MeltingSolidificationCell *msc = dynamic_cast<MeltingSolidificationCell*>(grid->getGrid(y, x, 0));
                    if (msc != 0) {
                        double beamEnergy = power / (std::sqrt(2 * PI) * standardDeviation) * std::exp(-1.0 / (2 * standardDeviation * standardDeviation) * ((x - currentX) * (x - currentX)));
                        msc->setBeamEnergy(beamEnergy);
                    }
                }
            }
            for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
                double absorbed = -1;
                for (int y = grid->getConfig()->getHeight() - 1; y >= 0; y--) {
                    MeltingSolidificationCell *msc = dynamic_cast<MeltingSolidificationCell*>(grid->getGrid(y, x, 0));
                    if (msc != 0) {
                        msc->setAbsorbedEnergy(0);
                        if (msc->getType() == 'S' || msc->getType() == 'L' || msc->getType() == 'I') {
                            if (absorbed < 0) {
                                absorbed = msc->getBeamEnergy();
                            }
                            double factor = 1;
                            if (msc->getType() == 'I') {
                                factor = msc->getCellMass() / msc->getP(0);
                            }
                            double newAbsorbed = absorbed * absorptionCoefficient;
                            msc->setAbsorbedEnergy(factor * newAbsorbed);
                            absorbed -= newAbsorbed;
                        }
                    }
                }
            }
        }
    }
}

void KornerImplementation::configure(double power, double standardDeviation, int startX, double velocity, int timesteps, double absorptionCoefficient) {
    this->power = power;
    this->standardDeviation = standardDeviation;
    this->currentX = startX;
    this->startX = startX;
    this->velocity = velocity;
    this->timesteps = timesteps;
    this->absorptionCoefficient = absorptionCoefficient;
}

void KornerImplementation::configureThermal(double solidTau, double liquidTau, double gasTau) {
    this->solidTau = solidTau;
    this->liquidTau = liquidTau;
    this->gasTau = gasTau;
}

double KornerImplementation::getSolidTau() {
    return solidTau;
}

double KornerImplementation::getLiquidTau() {
    return liquidTau;
}

double KornerImplementation::getGasTau() {
    return gasTau;
}

void KornerImplementation::configureEnergy(double solidEnergy, double liquidEnergy) {
    this->solidEnergy = solidEnergy;
    this->liquidEnergy = liquidEnergy;
}

double KornerImplementation::getSolidEnergy() {
    return solidEnergy;
}

double KornerImplementation::getLiquidEnergy() {
    return liquidEnergy;
}
