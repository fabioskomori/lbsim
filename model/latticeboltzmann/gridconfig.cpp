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

#include "gridconfig.h"
#include "sccell.h"
#include "lbgrid.h"
#include "multi/mccell.h"
#include "multi/rkcell.h"
#include "shallow/shallowcell.h"
#include "porouscell.h"
#include "thermal/passivescalar.h"
#include "deposition/depositioncell.h"
#include "force/gravity.h"
#include "force/multiphase.h"
#include "moving/movingcell.h"
#include "moving/movingwall.h"
#include "boundary/sourcecell.h"
#include "boundary/opencell.h"
#include "boundary/extrapolationboundary.h"
#include "boundary/zouheboundary.h"
#include "boundary/fixedboundary.h"
#include "boundary/equilibriumboundary.h"
#include "passivescalar/passivescalarcell.h"
#include "moving/movingparticle.h"
#include <cstdlib>
#include <cmath>
#include <QDebug>
#include "force/forcelist.h"
#include "meltingsolidification/meltingsolidificationcell.h"

GridConfig::GridConfig(Grid *grid) {
    HEIGHT = 10, WIDTH = 10, LENGTH = 10;
    model = 9;
    epsilons[0] = epsilons[1] = epsilons[2] = 1;
    maxIterations = 0;
    minP0 = 1;
    maxP0 = 1;
    sourceVelocity[0] = sourceVelocity[1] = sourceVelocity[2] = 0.02;
    sourcePressure[0] = sourcePressure[1] = sourcePressure[2] = 1;
    sourcePeriod[0] = sourcePeriod[1] = sourcePeriod[2] = 0;
    sourceAmplitude[0] = sourceAmplitude[1] = sourceAmplitude[2] = 0;
    wallVelocity = MyVector3D(0.02, 0, 0);
    processNeighbors = true;
    periodicBoundaries = true;
    this->grid = grid;
    gravity = 0;
    mpG = 0, mpPsi0 = 4, mpP0 = 200, mpGads = -190;
    mcQuantity = 1;
    mcType = MC_SC;
    mcG = 0.05;
    open = OPEN_EQUILIBRIUM;
    source = SOURCE_EQUILIBRIUM;
    wallPeriod = 0;
    solidDensity = 0.5;
    rkA = 0.00001;
    rkBeta = 0.5;
    shallowG = 1;
    shallowNB = 1;
    density1 = density2 = 1;
    entropic = false;
    thermal[0] = thermal[1] = thermal[2] = 1;
    passiveScalarGravity = 0;
    multicoreSupport = true;
    smagorinsky = 0;
    stopCriterion = 0;
    slipWallCoefficient = 0;
    disableCharts = true;
    thermalExpansion = 1;
    depositionRate = 1;
    velocityOrder = 2;
    forces = new std::list<Force*>();
    reflectWithFactor = 1;
    nanoFraction = 0;
    nanoDensity = nanoThermalExpansion = nanoThermalConductivity = nanoHeatCapacitance = 1;
    nanoEpsilon0 = epsilons[0];
    nanoEpsilon1 = epsilons[1];
    nanoBeta = thermalExpansion;
    korner = false;
}

GridConfig::~GridConfig() {
    delete forces;
}

int GridConfig::getWidth() {
    return WIDTH;
}

int GridConfig::getHeight() {
    return HEIGHT;
}

int GridConfig::getLength() {
    return LENGTH;
}

bool GridConfig::contains(double x, double y, double z) {
    return x >= -0.5 && y >= -0.5 && z >= -0.5 && x <= WIDTH - 0.5 && y <= HEIGHT - 0.5 && z < LENGTH - 0.5;
}

int GridConfig::getTotalSites() {
    return WIDTH * HEIGHT * LENGTH;
}

void GridConfig::setDimension(int width, int height, int length) {
    WIDTH = width;
    HEIGHT = height;
    LENGTH = length;
}

int GridConfig::getModel() {
    return model;
}

void GridConfig::setModel(int model) {
    if (model != this->model) {
        this->model = model;
        processNeighbors = true;
    }
}

double* GridConfig::getEpsilons() {
    return epsilons;
}

void GridConfig::setEpsilon(int index, double epsilon) {
    epsilons[index] = epsilon;
    updateNano();
}

double GridConfig::getMinP0() {
    return minP0;
}

void GridConfig::setMinP0(double minP0) {
    this->minP0 = minP0;
    grid->reset();
}

double GridConfig::getMaxP0() {
    return maxP0;
}

void GridConfig::setMaxP0(double maxP0) {
    this->maxP0 = maxP0;
    grid->reset();
}

double GridConfig::randomP() {
    double result = (double)rand() / RAND_MAX * (maxP0 - minP0) + minP0;
    return result;
}

int GridConfig::getMaxIterations() {
    return maxIterations;
}

void GridConfig::setMaxIterations(int maxIterations) {
    this->maxIterations = maxIterations;
}

bool GridConfig::getProcessNeighbors() {
    return processNeighbors;
}

void GridConfig::setProcessNeighbors(bool processNeighbors) {
    this->processNeighbors = processNeighbors;
}

bool GridConfig::getPeriodicBoundaries() {
    return periodicBoundaries;
}

void GridConfig::setPeriodicBoundaries(bool periodicBoundaries) {
    this->periodicBoundaries = periodicBoundaries;
    processNeighbors = true;
}

std::list<Force*>* GridConfig::getForces() {
    return forces;
}

void GridConfig::addForce(Force *newForce) {
    for (std::list<Force*>::iterator force = forces->begin(); force != forces->end(); force++) {
        if (typeid(**force) == typeid(*newForce)) {
            force = forces->erase(force);
        }
    }
    if (newForce->hasEffect()) {
        forces->push_back(newForce);
    }
}

void GridConfig::removeForce(std::type_info toRemove) {
    for (std::list<Force*>::iterator force = forces->begin(); force != forces->end(); force++) {
        if (typeid(**force) == toRemove) {
            force = forces->erase(force);
        }
    }
}

double GridConfig::getGravity() {
    return gravity;
}

void GridConfig::setGravity(double newGravity) {
    this->gravity = newGravity;
    Gravity *gravity = new Gravity();
    addForce(gravity);
}

void GridConfig::getMultiphase(double *g, double *psi0, double *p0, double *gads) {
    *g = mpG;
    *psi0 = mpPsi0;
    *p0 = mpP0;
    *gads = mpGads;
}

void GridConfig::setMultiphase(double g, double psi0, double p0, double gads) {
    mpG = g;
    mpPsi0 = psi0;
    mpP0 = p0;
    mpGads = gads;
    MultiPhase *mp = new MultiPhase();
    addForce(mp);
}

void GridConfig::getMulticomponent(int *quantity, double *g, double *gads1, double *gads2, int *type) {
    *quantity = mcQuantity;
    *g = mcG;
    *gads1 = mcGads1;
    *gads2 = mcGads2;
    *type = mcType;
}

void GridConfig::setMulticomponent(int quantity, double g, double gads1, double gads2, int type) {
    this->mcQuantity = quantity;
    this->mcG = g;
    this->mcGads1 = gads1;
    this->mcGads2 = gads2;
    this->mcType = type;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                BaseCell *cell = grid->getGrid(i, j, k);
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                if (psc != 0) {
                    cell = psc->getCell();
                }
                if (cell->isFluid()) {
                    if (quantity == 1) {
                        if (dynamic_cast<SCCell*>(cell) == 0 && dynamic_cast<ShallowCell*>(cell) == 0 && dynamic_cast<PorousCell*>(cell) == 0 && dynamic_cast<MeltingSolidificationCell*>(cell) == 0) {
                            grid->setGrid(i, j, k, new SCCell(randomP()));
                        }
                    } else {
                        switch (type) {
                        case MC_SC:
                            if (dynamic_cast<MCCell*>(cell) == 0) {
                                grid->setGrid(i, j, k, new MCCell(dynamic_cast<SCCell*>(cell)));
                            }
                            break;
                        case MC_RK:
                            if (dynamic_cast<RKCell*>(cell) == 0) {
                                grid->setGrid(i, j, k, new RKCell());
                            }
                            break;
                        case MC_PS:
                            if (dynamic_cast<PassiveScalar*>(cell) == 0) {
                                grid->setGrid(i, j, k, new PassiveScalar());
                            }
                            break;
                        case MC_DEPOSITION:
                            if (dynamic_cast<DepositionCell*>(cell) == 0) {
                                grid->setGrid(i, j, k, new DepositionCell());
                            }
                            break;
                        }
                    }
                }
                cell = grid->getGrid(i, j, k);
                cell->setMulticomponent(quantity);
            }
        }
    }
}

void GridConfig::setParticleRelativeDensity(double relativeDensity) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid(i, j, k));
                if (moving != 0) {
                    MovingParticle *particle = dynamic_cast<MovingParticle*>(moving->getParticle());
                    if (particle != 0) {
                        particle->setRelativeDensity(relativeDensity);
                    }
                }
            }
        }
    }
}

void GridConfig::setParticleTemperatureCoefficient(double temperatureCoefficient) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid(i, j, k));
                if (moving != 0) {
                    MovingParticle *particle = dynamic_cast<MovingParticle*>(moving->getParticle());
                    if (particle != 0) {
                        particle->setTemperatureCoefficient(temperatureCoefficient);
                    }
                }
            }
        }
    }
}

double GridConfig::getSourceVelocity(int index) {
    if (index < 3) {
        return sourceVelocity[index];
    }
    return sourceVelocity[0];
}

double GridConfig::getSourcePressure(int index) {
    return sourcePressure[index];
}

int GridConfig::getSourcePeriod(int index) {
    return sourcePeriod[index];
}

void GridConfig::setSourcePeriod(int index, int period) {
    sourcePeriod[index] = period;
}

double GridConfig::getSourceAmplitude(int index) {
    return sourceAmplitude[index];
}

void GridConfig::setSourceAmplitude(int index, double amplitude) {
    sourceAmplitude[index] = amplitude;
}

void GridConfig::setSourceVelocity(int index, double velocity) {
    sourceVelocity[index] = velocity;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                BaseCell *cell = grid->getGrid(i, j, k);
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(grid->getGrid(i, j, k));
                if (psc != 0) {
                    cell = psc->getCell();
                }
                SourceCell *source = dynamic_cast<SourceCell*>(cell);
                if (source != 0 && source->getIndex() == index) {
                    source->getImpl()->configVelocity(velocity);
                }
            }
        }
    }
}

void GridConfig::setWallVelocity(MyVector3D wallVelocity) {
    this->wallVelocity = wallVelocity;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid(i, j, k));
                if (moving != 0) {
                    MovingWall *wall = dynamic_cast<MovingWall*>(moving->getParticle());
                    if (wall != 0) {
                        wall->configVelocity(wallVelocity);
                    }
                }
            }
        }
    }
}

void GridConfig::setWallPeriod(int wallPeriod) {
    this->wallPeriod = wallPeriod;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid(i, j, k));
                if (moving != 0) {
                    MovingWall *wall = dynamic_cast<MovingWall*>(moving->getParticle());
                    if (wall != 0) {
                        wall->configPeriod(wallPeriod);
                    }
                }
            }
        }
    }
}

void GridConfig::setSourcePressure(int index, double pressure) {
    sourcePressure[index] = pressure;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                SourceCell *source = dynamic_cast<SourceCell*>(grid->getGrid(i, j, k));
                if (source != 0 && source->getIndex() == index) {
                    //double velocity = source->getImpl()->getVelocity();
                    //source->setImpl(new FixedBoundary());
                    //source->getImpl()->configVelocity(velocity);
                    source->getImpl()->configPressure(pressure);
                }
            }
        }
    }
    /*for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                OpenCell *open = dynamic_cast<OpenCell*>(grid->getGrid(i, j, k));
                if (open != 0) {
                    open->setImpl(new Equilibrium2Boundary());
                }
            }
        }
    }*/
}

void GridConfig::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("model", QString::number(model));
    writer.writeAttribute("epsilon0", QString::number(epsilons[0]));
    writer.writeAttribute("epsilon1", QString::number(epsilons[1]));
    writer.writeAttribute("epsilon2", QString::number(epsilons[2]));
    writer.writeAttribute("minP0", QString::number(minP0));
    writer.writeAttribute("maxP0", QString::number(maxP0));
    writer.writeAttribute("sourceVelocity0", QString::number(sourceVelocity[0]));
    writer.writeAttribute("sourceVelocity1", QString::number(sourceVelocity[1]));
    writer.writeAttribute("sourceVelocity2", QString::number(sourceVelocity[2]));
    writer.writeAttribute("sourcePressure0", QString::number(sourcePressure[0]));
    writer.writeAttribute("sourcePressure1", QString::number(sourcePressure[1]));
    writer.writeAttribute("sourcePeriod0", QString::number(sourcePeriod[0]));
    writer.writeAttribute("sourcePeriod1", QString::number(sourcePeriod[1]));
    writer.writeAttribute("sourceAmplitude0", QString::number(sourceAmplitude[0]));
    writer.writeAttribute("sourceAmplitude1", QString::number(sourceAmplitude[1]));
    //writer.writeAttribute("maxIterations", QString::number(maxIterations));
    writer.writeAttribute("processNeighbors", processNeighbors ? "1" : "0");
    writer.writeAttribute("periodicBoundaries", periodicBoundaries ? "1" : "0");
    writer.writeAttribute("gravity", QString::number(gravity));
    writer.writeAttribute("mcQuantity", QString::number(mcQuantity));
    writer.writeAttribute("mcType", QString::number(mcType));
    writer.writeAttribute("mcG", QString::number(mcG));
    writer.writeAttribute("mcGads", QString::number(mcGads1));
    writer.writeAttribute("mcGads2", QString::number(mcGads2));
    writer.writeAttribute("mpG", QString::number(mpG));
    writer.writeAttribute("mpPsi0", QString::number(mpPsi0));
    writer.writeAttribute("mpP0", QString::number(mpP0));
    writer.writeAttribute("mpGads", QString::number(mpGads));
    writer.writeAttribute("porousSolidDensity", QString::number(solidDensity));
    writer.writeAttribute("wallVelocityX", QString::number(wallVelocity.getX()));
    writer.writeAttribute("wallVelocityY", QString::number(wallVelocity.getY()));
    writer.writeAttribute("wallVelocityZ", QString::number(wallVelocity.getZ()));
    writer.writeAttribute("wallPeriod", QString::number(wallPeriod));
    writer.writeAttribute("density1", QString::number(density1));
    writer.writeAttribute("density2", QString::number(density2));
    writer.writeAttribute("shallowG", QString::number(shallowG));
    writer.writeAttribute("shallowNB", QString::number(shallowNB));
    writer.writeAttribute("passiveScalarGravity", QString::number(passiveScalarGravity));
    writer.writeAttribute("thermal0", QString::number(thermal[0]));
    writer.writeAttribute("thermal1", QString::number(thermal[1]));
    writer.writeAttribute("thermal2", QString::number(thermal[2]));
    writer.writeAttribute("smagorinsky", QString::number(smagorinsky));
    writer.writeAttribute("entropic", entropic ? "1" : "0");
    writer.writeAttribute("slipWallCoefficient", QString::number(slipWallCoefficient));
    writer.writeAttribute("sourceStrategy", QString::number(source));
    writer.writeAttribute("openStrategy", QString::number(open));
    writer.writeAttribute("thermalExpansion", QString::number(thermalExpansion));
    writer.writeAttribute("depositionRate", QString::number(depositionRate));
    writer.writeAttribute("velocityOrder", QString::number(velocityOrder));
    writer.writeAttribute("reflectWithFactor", QString::number(reflectWithFactor));
    writer.writeAttribute("nanoFraction", QString::number(nanoFraction));
    writer.writeAttribute("nanoDensity", QString::number(nanoDensity));
    writer.writeAttribute("nanoThermalExpansion", QString::number(nanoThermalExpansion));
    writer.writeAttribute("nanoThermalConductivity", QString::number(nanoThermalConductivity));
    writer.writeAttribute("nanoHeatCapacitance", QString::number(nanoHeatCapacitance));
    ForceList::passivate(forces, writer);
}

void GridConfig::activate(QXmlStreamReader &reader) {
    model = reader.attributes().value("model").toString().toInt();
    epsilons[0] = reader.attributes().value("epsilon0").toString().toDouble();
    epsilons[1] = reader.attributes().value("epsilon1").toString().toDouble();
    epsilons[2] = reader.attributes().value("epsilon2").toString().toDouble();
    minP0 = reader.attributes().value("minP0").toString().toDouble();
    maxP0 = reader.attributes().value("maxP0").toString().toDouble();
    sourceVelocity[0] = reader.attributes().value("sourceVelocity0").toString().toDouble();
    sourceVelocity[1] = reader.attributes().value("sourceVelocity1").toString().toDouble();
    sourceVelocity[2] = reader.attributes().value("sourceVelocity2").toString().toDouble();
    if (reader.attributes().hasAttribute("sourcePressure0")) {
        sourcePressure[0] = reader.attributes().value("sourcePressure0").toString().toDouble();
        sourcePressure[1] = reader.attributes().value("sourcePressure1").toString().toDouble();
    }
    maxIterations = 0;
    processNeighbors = true;
    periodicBoundaries = reader.attributes().value("periodicBoundaries") == "1";
    gravity = reader.attributes().value("gravity").toString().toDouble();
    if (reader.attributes().hasAttribute("mcQuantity")) {
        mcQuantity = reader.attributes().value("mcQuantity").toString().toInt();
        mcG = reader.attributes().value("mcG").toString().toDouble();
        mcGads1 = 0;
        if (reader.attributes().hasAttribute("mcGads")) {
            mcGads1 = reader.attributes().value("mcGads").toString().toDouble();
        }
        mcGads2 = 0;
        if (reader.attributes().hasAttribute("mcGads2")) {
            mcGads2 = reader.attributes().value("mcGads2").toString().toDouble();
        }
        mpG = reader.attributes().value("mpG").toString().toDouble();
        mpPsi0 = reader.attributes().value("mpPsi0").toString().toDouble();
        mpP0 = reader.attributes().value("mpP0").toString().toDouble();
        mpGads = reader.attributes().value("mpGads").toString().toDouble();
        setMulticomponent(mcQuantity, mcG, mcGads1, mcGads2, reader.attributes().hasAttribute("mcType") ? reader.attributes().value("mcType").toString().toInt() : MC_SC);
    }
    solidDensity = 0.5;
    if (reader.attributes().hasAttribute("porousSolidDensity")) {
        solidDensity = reader.attributes().value("porousSolidDensity").toString().toDouble();
    }
    if (reader.attributes().hasAttribute("wallPeriod")) {
        wallPeriod = reader.attributes().value("wallPeriod").toString().toInt();
        wallVelocity = MyVector3D(reader.attributes().value("wallVelocityX").toString().toDouble(), reader.attributes().value("wallVelocityY").toString().toDouble(), reader.attributes().value("wallVelocityZ").toString().toDouble());
    }
    density1 = density2 = 1;
    if (reader.attributes().hasAttribute("density1")) {
        density1 = reader.attributes().value("density1").toString().toDouble();
        density2 = reader.attributes().value("density2").toString().toDouble();
    }
    sourcePeriod[0] = sourcePeriod[1] = 0;
    sourceAmplitude[0] = sourceAmplitude[1] = 0;
    if (reader.attributes().hasAttribute("sourcePeriod0")) {
        sourcePeriod[0] = reader.attributes().value("sourcePeriod0").toString().toInt();
        sourcePeriod[1] = reader.attributes().value("sourcePeriod1").toString().toInt();
        sourceAmplitude[0] = reader.attributes().value("sourceAmplitude0").toString().toDouble();
        sourceAmplitude[1] = reader.attributes().value("sourceAmplitude1").toString().toDouble();
    }
    if (reader.attributes().hasAttribute("shallowG")) {
        shallowG = reader.attributes().value("shallowG").toString().toDouble();
        shallowNB = reader.attributes().value("shallowNB").toString().toDouble();
    }
    if (reader.attributes().hasAttribute("passiveScalarGravity")) {
        passiveScalarGravity = reader.attributes().value("passiveScalarGravity").toString().toDouble();
        thermal[0] = reader.attributes().value("thermal0").toString().toDouble();
        thermal[1] = reader.attributes().value("thermal1").toString().toDouble();
    }
    if (reader.attributes().hasAttribute("thermal2")) {
        thermal[2] = reader.attributes().value("thermal2").toString().toDouble();
    }
    if (reader.attributes().hasAttribute("smagorinsky")) {
        smagorinsky = reader.attributes().value("smagorinsky").toString().toDouble();
        entropic = reader.attributes().value("entropic") == "1";
    }
    if (reader.attributes().hasAttribute("slipWallCoefficient")) {
        slipWallCoefficient = reader.attributes().value("slipWallCoefficient").toString().toDouble();
    }
    if (reader.attributes().hasAttribute("sourceStrategy")) {
        source = reader.attributes().value("sourceStrategy").toString().toInt();
        open = reader.attributes().value("openStrategy").toString().toInt();
    }
    thermalExpansion = 1;
    if (reader.attributes().hasAttribute("thermalExpansion")) {
        thermalExpansion = reader.attributes().value("thermalExpansion").toString().toDouble();
    }
    depositionRate = 1;
    if (reader.attributes().hasAttribute("depositionRate")) {
        depositionRate = reader.attributes().value("depositionRate").toString().toDouble();
    }
    velocityOrder = 2;
    if (reader.attributes().hasAttribute("velocityOrder")) {
        velocityOrder = reader.attributes().value("velocityOrder").toString().toInt();
    }
    reflectWithFactor = 1;
    if (reader.attributes().hasAttribute("reflectWithFactor")) {
        reflectWithFactor = reader.attributes().value("reflectWithFactor").toString().toInt();
    }
    nanoFraction = 0;
    nanoDensity = nanoThermalExpansion = nanoThermalConductivity = nanoHeatCapacitance = 1;
    if (reader.attributes().hasAttribute("nanoFraction")) {
        nanoFraction = reader.attributes().value("nanoFraction").toString().toDouble();
        nanoDensity = reader.attributes().value("nanoDensity").toString().toDouble();
        nanoThermalExpansion = reader.attributes().value("nanoThermalExpansion").toString().toDouble();
        nanoThermalConductivity = reader.attributes().value("nanoThermalConductivity").toString().toDouble();
        nanoHeatCapacitance = reader.attributes().value("nanoHeatCapacitance").toString().toDouble();
    }
    forces->clear();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    ForceList::activate(forces, reader, grid);
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    updateNano();
}

void GridConfig::setOpen(int open) {
    this->open = open;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                OpenCell *temp = dynamic_cast<OpenCell*>(grid->getGrid(i, j, k));
                if (temp != 0) {
                    switch (open) {
                    case OPEN_EQUILIBRIUM: {
                        EquilibriumBoundary *equilibrium = new EquilibriumBoundary();
                        equilibrium->configPressure(minP0);
                        temp->setImpl(equilibrium);
                    }
                        break;
                    case OPEN_EXTRAPOLATION:
                        temp->setImpl(new ExtrapolationBoundary());
                        break;
                    case OPEN_ZOUHE: {
                        ZouHeBoundary *zouhe = new ZouHeBoundary();
                        zouhe->configPressure(minP0);
                        temp->setImpl(zouhe);
                    }
                        break;
                    }
                }
            }
        }
    }
}

void GridConfig::setSource(int source) {
    this->source = source;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            for (int k = 0; k < LENGTH; k++) {
                SourceCell *temp = dynamic_cast<SourceCell*>(grid->getGrid(i, j, k));
                if (temp != 0) {
                    switch (source) {
                    case SOURCE_EQUILIBRIUM: {
                        EquilibriumBoundary *equilibrium = new EquilibriumBoundary();
                        equilibrium->configVelocity(temp->getImpl()->getVelocity());
                        temp->setImpl(equilibrium);
                    }
                        break;
                    case SOURCE_FIXED: {
                        FixedBoundary *fixed = new FixedBoundary();
                        fixed->configPressure(sourcePressure[temp->getIndex()]);
                        fixed->configVelocity(temp->getImpl()->getVelocity());
                        fixed->setMultiComponentIndex(temp->getIndex());
                        temp->setImpl(fixed);
                    }
                        break;
                    case SOURCE_ZOUHE: {
                        ZouHeBoundary *zouhe = new ZouHeBoundary();
                        zouhe->configVelocity(temp->getImpl()->getVelocity());
                        temp->setImpl(zouhe);
                    }
                        break;
                    }
                }
            }
        }
    }
}

int GridConfig::getOpen() {
    return open;
}

int GridConfig::getSource() {
    return source;
}

MyVector3D GridConfig::getWallVelocity() {
    return wallVelocity;
}

int GridConfig::getWallPeriod() {
    return wallPeriod;
}

double GridConfig::getSolidDensity() {
    return solidDensity;
}

void GridConfig::setSolidDensity(double solidDensity, double variation) {
    this->solidDensity = solidDensity;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int z = 0; z < LENGTH; z++) {
                BaseCell *cell = grid->getGrid(y, x, z);
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                if (psc != 0) {
                    cell = psc->getCell();
                }
                PorousCell *porous = dynamic_cast<PorousCell*>(cell);
                if (porous != 0) {
                    porous->setSolidDensity(solidDensity + (((double)(rand() % 1000)) / 1000 * 2 - 1) * variation);
                }
            }
        }
    }
}

double GridConfig::getRkA() {
    return rkA;
}

void GridConfig::setRkA(double rkA) {
    this->rkA = rkA;
}

double GridConfig::getRkBeta() {
    return rkBeta;
}

void GridConfig::setRkBeta(double rkBeta) {
    this->rkBeta = rkBeta;
}

double GridConfig::getShallowG() {
    return shallowG;
}

void GridConfig::setShallowG(double shallowG) {
    this->shallowG = shallowG;
}

double GridConfig::getShallowNB() {
    return shallowNB;
}

void GridConfig::setShallowNB(double shallowNB) {
    this->shallowNB = shallowNB;
}

double GridConfig::getDensity1() {
    return density1;
}

void GridConfig::setDensity1(double density1) {
    this->density1 = density1;
}

double GridConfig::getDensity2() {
    return density2;
}

void GridConfig::setDensity2(double density2) {
    this->density2 = density2;
}

bool GridConfig::getEntropic() {
    return entropic;
}

void GridConfig::setEntropic(bool entropic) {
    this->entropic = entropic;
}

double GridConfig::getPassiveScalarGravity() {
    return passiveScalarGravity;
}

void GridConfig::setPassiveScalarGravity(double passiveScalarGravity) {
    this->passiveScalarGravity = passiveScalarGravity;
}

double GridConfig::getThermal(int index) {
    return thermal[index];
}

void GridConfig::setThermal(int index, double value) {
    thermal[index] = value;
}

double GridConfig::getStopCriterion() {
    return stopCriterion;
}

void GridConfig::setStopCriterion(double stopCriterion) {
    this->stopCriterion = stopCriterion;
}

bool GridConfig::getMulticoreSupport() {
    return multicoreSupport;
}

void GridConfig::setMulticoreSupport(bool multicoreSupport) {
    this->multicoreSupport = multicoreSupport;
}

double GridConfig::getSmagorinsky() {
    return smagorinsky;
}

void GridConfig::setSmagorinsky(double smagorinsky) {
    this->smagorinsky = smagorinsky;
}

double GridConfig::getSlipWallCoefficient() {
    return slipWallCoefficient;
}

void GridConfig::setSlipWallCoefficient(double slipWallCoefficient) {
    this->slipWallCoefficient = slipWallCoefficient;
}

std::list<Vector3i> GridConfig::getPointsToReport() {
    return pointsToReport;
}

void GridConfig::addPointToReport(Vector3i pointToReport) {
    pointsToReport.push_back(pointToReport);
}

void GridConfig::clearPointsToReport() {
    pointsToReport.clear();
}

bool GridConfig::getDisableCharts() {
    return disableCharts;
}

void GridConfig::setDisableCharts(bool disableCharts) {
    this->disableCharts = disableCharts;
}

double GridConfig::getThermalExpansion() {
    return thermalExpansion;
}

void GridConfig::setThermalExpansion(double thermalExpansion) {
    this->thermalExpansion = thermalExpansion;
    updateNano();
}

double GridConfig::getDepositionRate() {
    return depositionRate;
}

void GridConfig::setDepositionRate(double depositionRate) {
    this->depositionRate = depositionRate;
}

int GridConfig::getVelocityOrder() {
    return velocityOrder;
}

void GridConfig::setVelocityOrder(int velocityOrder) {
    this->velocityOrder = velocityOrder;
}

double GridConfig::getReflectWithFactor() {
    return reflectWithFactor;
}

void GridConfig::setReflectWithFactor(double reflectWithFactor) {
    this->reflectWithFactor = reflectWithFactor;
}

void GridConfig::setNanoValues(double nanoFraction, double nanoDensity, double nanoThermalExpansion, double nanoThermalConductivity, double nanoHeatCapacitance) {
    this->nanoFraction = nanoFraction;
    this->nanoDensity = nanoDensity;
    this->nanoThermalExpansion = nanoThermalExpansion;
    this->nanoThermalConductivity = nanoThermalConductivity;
    this->nanoHeatCapacitance = nanoHeatCapacitance;
    updateNano();
}

void GridConfig::getNanoValues(double *nanoFraction, double *nanoDensity, double *nanoThermalExpansion, double *nanoThermalConductivity, double *nanoHeatCapacitance) {
    *nanoFraction = this->nanoFraction;
    *nanoDensity = this->nanoDensity;
    *nanoThermalExpansion = this->nanoThermalExpansion;
    *nanoThermalConductivity = this->nanoThermalConductivity;
    *nanoHeatCapacitance = this->nanoHeatCapacitance;
}

double GridConfig::getNanoEpsilon0() {
    return nanoEpsilon0;
}

double GridConfig::getNanoEpsilon1() {
    return nanoEpsilon1;
}

double GridConfig::getNanoBeta() {
    return nanoBeta;
}

double GridConfig::getNanoDensity() {
    return (1 - nanoFraction) + nanoFraction * nanoDensity;
}

void GridConfig::updateNano() {
    nanoEpsilon0 = 0.5 + (epsilons[0] - 0.5) / (pow(1 - nanoFraction, 2.5) * (1 - nanoFraction + nanoFraction * nanoDensity));
    nanoBeta = thermalExpansion * (1 - nanoFraction + nanoFraction * nanoDensity * nanoThermalExpansion) / (1 - nanoFraction + nanoFraction * nanoDensity);
    nanoEpsilon1 = 0.5 + (epsilons[1] - 0.5) * ((2 + nanoThermalConductivity - 2 * nanoFraction + 2 * nanoFraction * nanoThermalConductivity) / (2 + nanoThermalConductivity + nanoFraction - nanoFraction * nanoThermalConductivity)) / (1 - nanoFraction + nanoFraction * nanoDensity * nanoHeatCapacitance);
}

void GridConfig::setKorner(bool korner) {
    this->korner = korner;
}

bool GridConfig::getKorner() {
    return korner;
}
