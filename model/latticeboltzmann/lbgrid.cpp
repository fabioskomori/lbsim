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

#include "lbgrid.h"
#include "gridconfig.h"
#include "gridsimulation.h"
#include "sccell.h"
#include "basecell.h"
#include "porouscell.h"
#include "nullcell.h"
#include "lbutil.h"
#include "boundary/sourcecell.h"
#include "boundary/wallcell.h"
#include "boundary/opencell.h"
#include "boundary/pointboundary.h"
#include "boundary/dragwallcell.h"
#include "boundary/partialslipboundary.h"
#include "moving/movingcell.h"
#include "moving/particlemanager.h"
#include "moving/movingwall.h"
#include "multi/mccell.h"
#include "multi/rkcell.h"
#include "shallow/shallowcell.h"
#include "thermal/passivescalar.h"
#include "deposition/depositioncell.h"
#include "deposition/depositionwall.h"
#include "physical/parameters.h"
#include "extra/velocityfield.h"
#include "../../model/math/myvector3d.h"
#include "../../model/math/vector3i.h"
#include "../../model/listener/listener.h"
#include "thermal/thermalwall.h"
#include "meltingsolidification/meltingsolidificationcell.h"
#include "interpolation/interpolationcell.h"
#include <typeinfo>
#include <cstdlib>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    #include <QtConcurrentMap>
#else
    #include <QtConcurrent/QtConcurrentMap>
#endif
#include <QtCore>
#include "../../model/listener/listenerdata.h"
//#include "opencl/multilatticeboltzmannopencl.h"
//#include "opencl/allocation.h"
#include "../../model/listener/listener.h"
#include "../../model/util/shared.h"
#include "passivescalar/passivescalarcell.h"
#include "passivescalar/passivescalarsingleton.h"
#include "passivescalar/momentpropagationcell.h"
#include "boundary/reflectwithfactorcell.h"
#include "meltingsolidification/kornerimplementation.h"
#include <cmath>

#define FILES 10

Grid::Grid() {
    config = new GridConfig(this);
    Shared::instance()->setGridConfig(config);
    simulation = new GridSimulation();
    particleManager = new ParticleManager(this);
    grid = new BaseCell*[config->getTotalSites()];
    neighborsCache = new BaseCell*[config->getTotalSites() * config->getModel()];
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()] = 0;
                for (int a = 0; a < config->getModel(); a++) {
                    neighborsCache[a + k * config->getModel() + j * config->getModel() * config->getLength() + i * config->getModel() * config->getLength() * config->getWidth()] = 0;
                }
            }
        }
    }
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                setGrid(i, j, k, new SCCell(config->randomP()));
            }
        }
    }
    neighborsCache = 0;
    parameters = new Parameters();
    velocityField = new VelocityField(this);
    //opencl = 0;
    openclType = CPU;
    openclUpdate = 1;
    lastOpenclUpdate = 0;
    fluxes = 0;
    fluxCalculated = false;
    korner = new KornerImplementation(this);
}

Grid::~Grid() {
    delete config;
    delete simulation;
    delete particleManager;
    delete[] grid;
    delete[] neighborsCache;
    delete parameters;
    delete velocityField;
    //delete opencl;
    delete korner;
}

void Grid::init(int width, int height, int length, bool dontDelete) {
    config->setDimension(width, height, length);
    //ListenerData listenerData(ListenerData::createSize(config->getWidth(), config->getHeight(), config->getLength()));
    //listener->modelCallback(listenerData);
    notifyListeners(LISTENER_EVENT_INIT);
    /*for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                delete getGrid(i, j, k);
            }
        }
    }*/
    if (!dontDelete) {
        delete[] grid;
        delete[] neighborsCache;
    }
    simulation->resetIterations();
    simulation->resetDeltasP();
    particleManager->reset();
    grid = new BaseCell*[config->getTotalSites()];
    neighborsCache = new BaseCell*[config->getTotalSites() * config->getModel()];
    is.clear();
    for (int i = 0; i < config->getHeight(); i++) {
        is.append(i);
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()] = 0;
                setGrid(i, j, k, new SCCell(config->randomP()));
                for (int a = 0; a < config->getModel(); a++) {
                    neighborsCache[a + k * config->getModel() + j * config->getLength() * config->getModel() + i * config->getLength() * config->getWidth() * config->getModel()] = 0;
                }
            }
        }
    }
}

void Grid::randomP() {
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                BaseCell *cell = getGrid(i, j, k);
                if (cell != 0) {
                    cell->reset(config->randomP(), -1);
                }
            }
        }
    }
}

void Grid::load(QString *data, int width, int height, int length) {
    init(width, height, length);
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                QString lattice = data[k + j * config->getLength() + i * config->getLength() * config->getWidth()];
                alter(lattice, j, j, i, i, k, k);
            }
        }
    }
    reset();
    velocityField->removeAll();
}

void Grid::resize(int factor) {
    BaseCell** oldGrid = grid;
    BaseCell** oldNeighborsCache = neighborsCache;
    int oldWidth = config->getWidth();
    int oldHeight = config->getHeight();
    int oldLength = config->getLength();
    init(oldWidth * factor, oldHeight * factor, oldLength, true);
    for (int x = 0; x < config->getWidth(); x++) {
        for (int y = 0; y < config->getHeight(); y++) {
            for (int z = 0; z < config->getLength(); z++) {
                setGrid(y, x, z, oldGrid[z + (x / 2) * oldLength + (y / 2) * oldLength * oldWidth]->clone());
            }
        }
    }
    delete[] oldGrid;
    delete[] oldNeighborsCache;
    processNeighbors();
}

void Grid::alter(QString type, int minx, int maxx, int miny, int maxy, int minz, int maxz, bool seeNeighbors) {
    for (int i = miny; i <= maxy; i++) {
        for (int j = minx; j <= maxx; j++) {
            for (int k = minz; k <= maxz; k++) {
                if (type == "korner_solid") {
                    MeltingSolidificationCell *cell = new MeltingSolidificationCell(korner);
                    cell->setType('S');
                    setGrid(i, j, k, cell);
                } else if (type == "korner_liquid") {
                    MeltingSolidificationCell *cell = new MeltingSolidificationCell(korner);
                    cell->setType('L');
                    setGrid(i, j, k, cell);
                } else if (type == "korner_gas") {
                    MeltingSolidificationCell *cell = new MeltingSolidificationCell(korner);
                    cell->setType('G');
                    setGrid(i, j, k, cell);
                } else if (type == "korner_interface") {
                    MeltingSolidificationCell *cell = new MeltingSolidificationCell(korner);
                    cell->setType('I');
                    setGrid(i, j, k, cell);
                } else if (type == "korner_wall") {
                    MeltingSolidificationCell *cell = new MeltingSolidificationCell(korner);
                    cell->setType('W');
                    setGrid(i, j, k, cell);
                } else if (type == "w") {
                    setGrid(i, j, k, new WallCell());
                } else if (type == "v") {
                    MovingCell *moving = new MovingCell(config->randomP(), 0, this);
                    moving->setParticle(new MovingWall(config->getWallVelocity()));
                    setGrid(i, j, k, moving);
                } else if (type == "s") {
                    //setGrid(i, j, k, SourceCell::createEquilibrium(config->getSourceVelocity(0), config->getMinP0(), config->getModel()));
                    setGrid(i, j, k, SourceCell::createFixed(config->getSourceVelocity(0), config->getMinP0()));
                } else if (type == "o") {
                    //setGrid(i, j, k, OpenCell::createEquilibrium(config->getMinP0(), config->getModel()));
                    setGrid(i, j, k, OpenCell::createExtrapolation(config->getModel()));
                } else if (type == "m") {
                    setGrid(i, j, k, new MovingCell(config->randomP(), 0, this));
                } else if (type == "p") {
                    setGrid(i, j, k, new MovingCell(config->randomP(), 1, this));
                } else if (type == "n") {
                    setGrid(i, j, k, new MovingCell(config->randomP(), -1, this));
                } else if (type == "*") {
                    setGrid(i, j, k, new PorousCell(config->randomP()));
                } else if (type == "h") {
                    setGrid(i, j, k, new ShallowCell());
                } else if (type == "-") {
                    setGrid(i, j, k, new NullCell());
                } else if (type == ".") {
                    setGrid(i, j, k, SourceCell::createPoint(config->getSourcePressure(0)));
                } else if (type == "t") {
                    setGrid(i, j, k, new ThermalWall());
                } else if (type == "d") {
                    setGrid(i, j, k, new DragWallCell());
                } else if (type == "k") {
                    setGrid(i, j, k, new PartialSlipBoundary());
                } else if (type == "e") {
                    setGrid(i, j, k, new DepositionWall());
                } else if (type == "r") {
                    setGrid(i, j, k, new ReflectWithFactorCell());
                } else if (seeNeighbors) {
                    double p1 = 0, p2 = 0;
                    MyVector3D v = MyVector3D();
                    int count = 0;
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            for (int dz = -1; dz <= 1; dz++) {
                                if (dx != 0 || dy != 0 || dz != 0) {
                                    BaseCell *cell = getGrid(i + dy, j + dx, k + dz);
                                    if (cell != 0 && cell->isFluid()) {
                                        p1 += cell->getP(0);
                                        p2 += cell->getP(1);
                                        v = v + cell->getU(0);
                                        count++;
                                    }
                                }
                            }
                        }
                    }
                    if (count != 0) {
                        p1 /= count;
                        v = v ^ (1.0 / count);
                        p2 /= count;
                    } else {
                        p1 = 1;
                        v = MyVector3D();
                        p2 = 0;
                    }
                    int mcQuantity, mcType;
                    double mcG, mcGads1, mcGads2;
                    config->getMulticomponent(&mcQuantity, &mcG, &mcGads1, &mcGads2, &mcType);
                    if (mcType == MC_PS) {
                        PassiveScalar *psc = new PassiveScalar();
                        psc->config(p1, v, p2);
                        setGrid(i, j, k, psc);
                    } else {
                        SCCell *newCell = new SCCell(config->randomP());
                        newCell->config(v, p1);
                        setGrid(i, j, k, newCell);
                    }
                } else {
                    setGrid(i, j, k, new SCCell(config->randomP()));
                }
            }
        }
    }
}

void Grid::pressure(int minx, int maxx, int miny, int maxy, int minz, int maxz, double p1, double p2) {
    for (int i = miny; i <= maxy; i++) {
        for (int j = minx; j <= maxx; j++) {
            for (int k = minz; k <= maxz; k++) {
                BaseCell *cell = getGrid(i, j, k);
                if (cell != 0) {
                    cell->reset(p1, p2);
                }
            }
        }
    }
}

void Grid::porous(int percent) {
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                BaseCell *cell = getGrid(i, j, k);
                if (cell->isFluid()) {
                    if (rand() % 100 < percent) {
                        alter("w", j, j, i, i, k, k);
                    }
                }
            }
        }
    }
}

BaseCell* Grid::newCell(int i, int j, int k, MyVector3D velocity) {
    BaseCell** neighbors = getNeighbors(i, j, k);
    double p0 = 0;
    int count = 0;
    for (int a = 0; a < config->getModel(); a++) {
        BaseCell *cell = neighbors[a];
        if (cell != 0 && cell->isFluid()) {
            p0 += cell->getP(-1);
            count++;
        }
    }
    p0 = p0 / count;
    SCCell *newCell = new SCCell(p0);
    newCell->config(velocity, p0);
    return newCell;
}

struct Scaled {

    Scaled(Grid *grid, BaseCell** neighborsCache, int stage) {
        this->grid = grid;
        this->neighborsCache = neighborsCache;
        this->stage = stage;
    }
    typedef int result_type;

    int operator() (int i) {
        switch (stage) {
        case 0:
            for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    //if (dynamic_cast<MovingCell*>(grid->getGrid(i, j, k)) == 0) {
                        grid->getGrid(i, j, k)->preUpdate(grid->getConfig()->getEpsilons(), &neighborsCache[k * grid->getConfig()->getModel() + j * grid->getConfig()->getLength() * grid->getConfig()->getModel() + i * grid->getConfig()->getLength() * grid->getConfig()->getWidth() * grid->getConfig()->getModel()], grid, Vector3i(j, i, k));
                    //}
                }
            }
            return i;
        case 1:
            for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    //if (dynamic_cast<MovingCell*>(grid->getGrid(i, j, k)) == 0) {
                        grid->getGrid(i, j, k)->preUpdate2(grid->getConfig()->getEpsilons(), &neighborsCache[k * grid->getConfig()->getModel() + j * grid->getConfig()->getLength() * grid->getConfig()->getModel() + i * grid->getConfig()->getLength() * grid->getConfig()->getWidth() * grid->getConfig()->getModel()], grid, Vector3i(j, i, k));
                    //}
                }
            }
            return i;
        case 2:
            for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    BaseCell *cell = grid->getGrid(i, j, k);
                    grid->getSimulation()->addDeltaP(cell->deltaP());
                    cell->update();
                    grid->getSimulation()->addTotalP(cell->getP(-1));
                }
            }
            return i;
        case 3:
            for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    grid->getGrid(i, j, k)->postUpdate(grid, Vector3i(j, i, k));
                }
            }
            return i;
        }
        return -1;
    }
    Grid *grid;
    BaseCell** neighborsCache;
    int stage;
};

void Grid::process(int stage) {
    if (config->getMulticoreSupport()) {
        QtConcurrent::blockingMap(is, Scaled(this, neighborsCache, stage));
    } else {
        foreach (int i, is) {
            Scaled(this, neighborsCache, stage)(i);
        }
    }
    fluxCalculated = false;
}

/*LBOCL* Grid::getOpenCL() {
    return opencl;
}*/

/*void Grid::setOpenclType(int openclType, bool doublePrecision, std::list<OpenCLDevice*> *devices) {
    this->openclType = openclType;
    if (openclType != CPU) {
        if (opencl != 0) {
            delete opencl;
        }
        opencl = new MultiLatticeBoltzmannOpenCL(this, devices, doublePrecision);
        //opencl->load(this);
    }
}*/

void Grid::setOpenclUpdate(int openclUpdate) {
    this->openclUpdate = openclUpdate;
}

int Grid::getOpenclUpdate() {
    return openclUpdate;
}

void Grid::processNeighbors() {
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                for (int a = 0; a < config->getModel(); a++) {
                    neighborsCache[k * config->getModel() + j * config->getLength() * config->getModel() + i * config->getLength() * config->getWidth() * config->getModel() + a] = 0;
                    if (LBUtil::W[config->getModel()][a] > 1e-100) {
                        Vector3i position = Vector3i(j, i, k) + LBUtil::C[config->getModel()][a];
                        if (config->getPeriodicBoundaries()) {
                            position = position.reflect(config->getWidth(), config->getHeight(), config->getLength());
                        }
                        if (position.bounds(config->getWidth(), config->getHeight(), config->getLength())) {
                            neighborsCache[k * config->getModel() + j * config->getLength() * config->getModel() + i * config->getLength() * config->getWidth() * config->getModel() + a] = getGrid(position.getY(), position.getX(), position.getZ());
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                BaseCell *cell = getGrid(i, j, k);
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                if (psc != 0) {
                    cell = psc->getCell();
                }
                WallCell *wall = dynamic_cast<WallCell*>(cell);
                ThermalWall *thermal = dynamic_cast<ThermalWall*>(cell);
                if (wall != 0) {
                    wall->injectNeighbors(&neighborsCache[k * config->getModel() + j * getConfig()->getLength() * config->getModel() + i * getConfig()->getLength() * getConfig()->getWidth() * config->getModel()]);
                } else if (thermal != 0) {
                    thermal->injectNeighbors(&neighborsCache[k * config->getModel() + j * getConfig()->getLength() * config->getModel() + i * getConfig()->getLength() * getConfig()->getWidth() * config->getModel()]);
                }
            }
        }
    }
}

int Grid::update(int steps) {
    if (isRunning()) {
        if (config->getProcessNeighbors()) {
            processNeighbors();
            config->setProcessNeighbors(false);
        }
        switch (openclType) {
            case CPU:
                for (int i = 0; i < steps; i++) {
                    process(0);
                    particleManager->preUpdate();
                    korner->preUpdate();
                    process(1);
                    simulation->resetDeltaP();
                    simulation->resetTotalP();
                    process(2);
                    simulation->calcDeltaP();
                    particleManager->update();
                    process(3);
                    velocityField->processPathLines();
                    simulation->addIteration();
                }
                break;
            case OPENCL_CPU: case OPENCL_GPU:
                //opencl->execute(steps);
                simulation->addIteration(steps);
                if (simulation->getIterations() != 0 && (simulation->getIterations() - lastOpenclUpdate >= openclUpdate || simulation->getIterations() < lastOpenclUpdate)) {
                    lastOpenclUpdate = simulation->getIterations();
                    //opencl->unload();
                }
                break;
        }
        //ListenerData listenerData(ListenerData::createIterations(simulation->addIteration(), 0, simulation->getDeltaP(), 0, simulation->getTotalP() / simulation->getDeltasP()));
        //listener->modelCallback(listenerData);
        notifyListeners(LISTENER_EVENT_GRIDUPDATE);
        return steps;
    }
    return 0;
}

void Grid::reset() {
    simulation->resetIterations();
    for (int i = 0; i < config->getHeight(); i++) {
        for (int j = 0; j < config->getWidth(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                getGrid(i, j, k)->reset(-1, -1);
            }
        }
    }
    //ListenerData listenerData(ListenerData::createIterations(0, 0, 0, 0, 0));
    //listener->callback(listenerData);
    notifyListeners(LISTENER_EVENT_RESET);
}

void Grid::notifyListeners(int event) {
    for (std::map<int, Listener*>::iterator listener = listeners.begin(); listener != listeners.end(); listener++) {
        ((*listener).second)->callback(event);
    }
}

void Grid::addListener(int id, Listener *listener) {
    listeners[id] = listener;
}

void Grid::removeListener(int id) {
    listeners.erase(id);
}

GridConfig* Grid::getConfig() {
    return config;
}

GridSimulation* Grid::getSimulation() {
    return simulation;
}

BaseCell* Grid::getGrid(int i, int j, int k) {
    if (i >= 0 && i < config->getHeight() && j >= 0 && j < config->getWidth() && k >= 0 && k < config->getLength()) {
        return grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()];
    } else {
        return 0;
    }
}

BaseCell** Grid::getNeighbors(int i, int j, int k) {
    if (i >= 0 && i < config->getHeight() && j >= 0 && j < config->getWidth() && k >= 0 && k < config->getLength()) {
        return &neighborsCache[k * config->getModel() + j * config->getModel() * config->getLength() + i * config->getModel() * config->getLength() * config->getWidth()];
    } else {
        return 0;
    }
}

void Grid::setGrid(int i, int j, int k, BaseCell *lattice, bool remove, bool additionalProcessing) {
    if (grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()] != 0) {
        BaseCell *cell = grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()];
        if (cell != 0 && cell->isFluid()) {
            simulation->removeDeltasP();
        }
        MovingCell *moving = dynamic_cast<MovingCell*>(grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()]);
        if (moving != 0) {
            particleManager->onLatticeRemoved(i, j, k, moving);
        }
        if (remove) {
            delete grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()];
        }
    }
    if (lattice->isFluid()) {
        simulation->addDeltasP();
    }
    setLattice(i, j, k, lattice);
    MovingCell *moving = dynamic_cast<MovingCell*>(lattice);
    if (moving != 0) {
        particleManager->onLatticeAdded(i, j, k, moving);
    }
    if (additionalProcessing) {
        SourceCell *source = dynamic_cast<SourceCell*>(lattice);
        if (source != 0) {
            bool hasNeighbor = false;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    for (int dk = -1; dk <= 1; dk++) {
                        if ((di != 0 || dj != 0 || dk != 0) && getGrid(i + di, j + dj, k + dk) != 0) {
                            SourceCell *neighbor = dynamic_cast<SourceCell*>(getGrid(i + di, j + dj, k + dk));
                            if (neighbor != 0) {
                                source->setIndex(neighbor->getIndex());
                                hasNeighbor = true;
                                break;
                            }
                        }
                    }
                    if (hasNeighbor) break;
                }
                if (hasNeighbor) break;
            }
            if (!hasNeighbor) {
                bool zero = false, one = false, two = false;
                for (int ii = 0; ii < config->getHeight(); ii++) {
                    for (int jj = 0; jj < config->getWidth(); jj++) {
                        for (int kk = 0; kk < config->getLength(); kk++) {
                            SourceCell *source = dynamic_cast<SourceCell*>(getGrid(ii, jj, kk));
                            if (source != 0 && (i != ii || j != jj || k != kk)) {
                                switch (source->getIndex()) {
                                case 0:
                                    zero = true;
                                    break;
                                case 1:
                                    one = true;
                                    break;
                                case 2:
                                    two = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (!zero) {
                    source->setIndex(0);
                } else if (!one) {
                    source->setIndex(1);
                } else if (!two) {
                    source->setIndex(2);
                }
            }
            source->getImpl()->configVelocity(config->getSourceVelocity(source->getIndex()));
        }
        ThermalWall *thermal = dynamic_cast<ThermalWall*>(lattice);
        if (thermal != 0) {
            bool hasNeighbor = false;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    for (int dk = -1; dk <= 1; dk++) {
                        if ((di != 0 || dj != 0 || dk != 0) && getGrid(i + di, j + dj, k + dk) != 0) {
                            ThermalWall *neighbor = dynamic_cast<ThermalWall*>(getGrid(i + di, j + dj, k + dk));
                            if (neighbor != 0) {
                                thermal->setIndex(neighbor->getIndex());
                                hasNeighbor = true;
                                break;
                            }
                        }
                    }
                    if (hasNeighbor) break;
                }
                if (hasNeighbor) break;
            }
            if (!hasNeighbor) {
                int last = -1;
                for (int ii = 0; ii < config->getHeight(); ii++) {
                    for (int jj = 0; jj < config->getWidth(); jj++) {
                        for (int kk = 0; kk < config->getLength(); kk++) {
                            ThermalWall *thermal2 = dynamic_cast<ThermalWall*>(getGrid(ii, jj, kk));
                            if (thermal2 != 0) {
                                if ((ii != i || jj != j || kk != k) && thermal2->getIndex() > last) {
                                    last = thermal2->getIndex();
                                }
                            }
                        }
                    }
                }
                thermal->setIndex(last + 1);
            }
        }
        DragWallCell *drag = dynamic_cast<DragWallCell*>(lattice);
        if (drag != 0) {
            int newIndex = -1, newIndex2 = -1;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    for (int dk = -1; dk <= 1; dk++) {
                        if ((di != 0 || dj != 0 || dk != 0) && getGrid(i + di, j + dj, k + dk) != 0) {
                            DragWallCell *neighbor = dynamic_cast<DragWallCell*>(getGrid(i + di, j + dj, k + dk));
                            if (neighbor != 0) {
                                if (newIndex == -1) {
                                    newIndex = neighbor->getIndex();
                                } else if (newIndex2 == -1 && neighbor->getIndex() != newIndex) {
                                    newIndex2 = neighbor->getIndex();
                                }
                            }
                        }
                    }
                }
            }
            if (newIndex == -1) {
                int last = -1;
                for (int ii = 0; ii < config->getHeight(); ii++) {
                    for (int jj = 0; jj < config->getWidth(); jj++) {
                        for (int kk = 0; kk < config->getLength(); kk++) {
                            DragWallCell *drag2 = dynamic_cast<DragWallCell*>(getGrid(ii, jj, kk));
                            if (drag2 != 0) {
                                if ((ii != i || jj != j || kk != k) && drag2->getIndex() > last) {
                                    last = drag2->getIndex();
                                }
                            }
                        }
                    }
                }
                drag->setIndex(last + 1);
            } else {
                if (newIndex2 != -1) {
                    int from = newIndex > newIndex2 ? newIndex : newIndex2;
                    int to = newIndex < newIndex2 ? newIndex : newIndex2;
                    for (int ii = 0; ii < config->getHeight(); ii++) {
                        for (int jj = 0; jj < config->getWidth(); jj++) {
                            for (int kk = 0; kk < config->getLength(); kk++) {
                                DragWallCell *drag2 = dynamic_cast<DragWallCell*>(getGrid(ii, jj, kk));
                                if (drag2 != 0) {
                                    if ((ii != i || jj != j || kk != k) && drag2->getIndex() == from) {
                                        drag2->setIndex(to);
                                    }
                                }
                            }
                        }
                    }
                    newIndex = to;
                }
                drag->setIndex(newIndex);
            }
        }
    }
    config->setProcessNeighbors(true);
}

void Grid::deleteLattice(int i, int j, int k) {
    delete getGrid(i, j, k);
}

void Grid::setLattice(int i, int j, int k, BaseCell *lattice) {
    grid[k + j * config->getLength() + i * config->getLength() * config->getWidth()] = lattice;
}

ParticleManager* Grid::getParticleManager() {
    return particleManager;
}

Parameters* Grid::getParameters() {
    return parameters;
}

VelocityField* Grid::getVelocityField() {
    return velocityField;
}

struct Save {

    Save(Grid *grid, QString fileName) {
        this->grid = grid;
        this->fileName = fileName;
    }
    typedef int result_type;

    int operator() (int a) {
        QString newFileName = fileName;
        int size = (a == FILES - 1 ? grid->getConfig()->getHeight() - (grid->getConfig()->getHeight() / FILES) * (FILES - 1) : grid->getConfig()->getHeight() / FILES);
        QFile file(newFileName.append(".n").append(QString::number(a)));
        file.open(QIODevice::WriteOnly);
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartElement("sussumu");
        for (int i = a * (grid->getConfig()->getHeight() / FILES); i < a * (grid->getConfig()->getHeight() / FILES) + size; i++) {
            for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    BaseCell *cell = grid->getGrid(i, j, k);
                    if (cell != 0) {
                        writer.writeStartElement("lattice");
                        writer.writeAttribute("x", QString::number(j));
                        writer.writeAttribute("y", QString::number(i));
                        writer.writeAttribute("z", QString::number(k));
                        cell->passivate(writer);
                        writer.writeEndElement();
                    }
                }
            }
        }
        writer.writeEndElement();
        file.close();
        return a;
    }
    Grid *grid;
    QString fileName;
};

void Grid::passivate(QXmlStreamWriter &writer, int version, QString fileName, int x, int y, int z) {
    writer.writeStartElement("particleManager");
    particleManager->passivate(writer);
    writer.writeEndElement();
    writer.writeStartElement("passivescalarsingleton");
    PassiveScalarSingleton::getInstance()->passivate(writer);
    writer.writeEndElement();
    writer.writeStartElement("grid");
    writer.writeAttribute("width", QString::number(config->getWidth()));
    writer.writeAttribute("height", QString::number(config->getHeight()));
    writer.writeAttribute("length", QString::number(config->getLength()));
    writer.writeAttribute("model", QString::number(config->getModel()));
    if (version == 2) {
        for (int i = (y != -1 ? y : 0); i < (y != -1 ? y + 1 : config->getHeight()); i++) {
            for (int j = (x != -1 ? x : 0); j < (x != -1 ? x + 1 : config->getWidth()); j++) {
                for (int k = (z != -1 ? z : 0); k < (z != -1 ? z + 1 : config->getLength()); k++) {
                    BaseCell *cell = getGrid(i, j, k);
                    if (cell != 0) {
                        writer.writeStartElement("lattice");
                        writer.writeAttribute("x", QString::number(j));
                        writer.writeAttribute("y", QString::number(i));
                        writer.writeAttribute("z", QString::number(k));
                        cell->passivate(writer);
                        writer.writeEndElement();
                    }
                }
            }
        }
    } else if (version == 3) {
        QList<int> files;
        for (int i = 0; i < FILES; i++) {
            files.append(i);
        }
        QtConcurrent::blockingMap(files, Save(this, fileName));
    }
    writer.writeEndElement();
    writer.writeStartElement("simulation");
    simulation->passivate(writer);
    writer.writeEndElement();
    writer.writeStartElement("config");
    config->passivate(writer);
    writer.writeEndElement();
    writer.writeStartElement("particles");
    velocityField->passivate(writer);
    writer.writeEndElement();
    writer.writeStartElement("parameters");
    parameters->passivate(writer);
    writer.writeEndElement();
}

BaseCell* Grid::activateCell(QXmlStreamReader &reader) {
    BaseCell *lattice = 0;
    if (reader.attributes().value("type").toString() == "singleComponent") {
        lattice = new SCCell(reader.attributes().value("p0").toString().toDouble());
    } else if (reader.attributes().value("type").toString() == "porous") {
         lattice = new PorousCell(reader.attributes().value("p0").toString().toDouble());
    } else if (reader.attributes().value("type").toString() == "wall") {
        lattice = new WallCell();
    } else if (reader.attributes().value("type").toString() == "source") {
        lattice = SourceCell::createNull();
    } else if (reader.attributes().value("type").toString() == "open") {
        lattice = OpenCell::createNull();
    } else if (reader.attributes().value("type").toString() == "multiComponent") {
        lattice = new MCCell();
    } else if (reader.attributes().value("type").toString() == "moving") {
        lattice = new MovingCell(0, 0, this);
    } else if (reader.attributes().value("type").toString() == "shallow") {
        lattice = new ShallowCell();
    } else if (reader.attributes().value("type").toString() == "multi_rk") {
        lattice = new RKCell();
    } else if (reader.attributes().value("type").toString() == "passivescalar") {
        lattice = new PassiveScalar();
    } else if (reader.attributes().value("type").toString() == "thermalWall") {
        lattice = new ThermalWall();
    } else if (reader.attributes().value("type").toString() == "partialSlip") {
        lattice = new PartialSlipBoundary();
    } else if (reader.attributes().value("type").toString() == "null") {
        lattice = new NullCell();
    } else if (reader.attributes().value("type").toString() == "dragWall") {
        lattice = new DragWallCell();
    } else if (reader.attributes().value("type").toString() == "deposition") {
        lattice = new DepositionCell();
    } else if (reader.attributes().value("type").toString() == "depositionWall") {
        lattice = new DepositionWall();
    } else if (reader.attributes().value("type").toString() == "interpolation") {
        lattice = new InterpolationCell(0, 0, 0);
    } else if (reader.attributes().value("type").toString() == "passiveScalarCell") {
        lattice = new PassiveScalarCell(0, 0, 0);
    } else if (reader.attributes().value("type").toString() == "momentPropagationCell") {
        lattice = new MomentPropagationCell(0, 0, 0);
    } else if (reader.attributes().value("type").toString() == "reflectWithFactor") {
        lattice = new ReflectWithFactorCell();
    }
    return lattice;
}

struct Load {

    Load(Grid *grid, QString fileName) {
        this->grid = grid;
        this->fileName = fileName;
    }
    typedef int result_type;

    int operator() (int a) {
        QString newFileName = fileName;
        int size = (a == FILES - 1 ? grid->getConfig()->getHeight() - (grid->getConfig()->getHeight() / FILES) * (FILES - 1) : grid->getConfig()->getHeight() / FILES);
        QFile file(newFileName.append(".n").append(QString::number(a)));
        file.open(QIODevice::ReadOnly);
        QXmlStreamReader reader(&file);
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        for (int i = a * (grid->getConfig()->getHeight() / FILES); i < a * (grid->getConfig()->getHeight() / FILES) + size; i++) {
            for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
                    BaseCell *lattice = grid->activateCell(reader);
                    if (lattice != 0) {
                        lattice->activate(reader, grid);
                        grid->setGrid(i, j, k, lattice, true, false);
                    } else {
                        qWarning() << "load error";
                    }
                }
            }
        }
        file.close();
        return a;
    }
    Grid *grid;
    QString fileName;
};

void Grid::activate(QXmlStreamReader &reader, int version, QString fileName) {
    particleManager->activate(reader);
    PassiveScalarSingleton::getInstance()->activate(reader, this);
    if (reader.attributes().hasAttribute("model")) {
        config->setModel(reader.attributes().value("model").toString().toInt());
    }
    init(reader.attributes().value("width").toString().toInt(), reader.attributes().value("height").toString().toInt(), reader.attributes().value("length").toString().toInt());
    if (version == 2) {
        for (int i = 0; i < config->getHeight(); i++) {
            for (int j = 0; j < config->getWidth(); j++) {
                for (int k = 0; k < config->getLength(); k++) {
                    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
                    BaseCell *lattice = activateCell(reader);
                    if (lattice != 0) {
                        lattice->activate(reader, this);
                        setGrid(i, j, k, lattice, true, false);
                    } else {
                        qWarning() << "load error";
                    }
                }
            }
        }
    } else if (version == 3) {
        QList<int> files;
        for (int i = 0; i < FILES; i++) {
            files.append(i);
        }
        QtConcurrent::blockingMap(files, Load(this, fileName));
        /*for (int i = 0; i < FILES; i++) {
            Load(this, fileName)(i);
        }*/
    }
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    simulation->activate(reader);
    //listener->modelCallback(ListenerData::createIterations(simulation->getIterations(), 0, 0, 0, 0));
    notifyListeners(LISTENER_EVENT_ACTIVATE);
    config->activate(reader);
    velocityField->activate(reader);
    parameters->activate(reader);
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
}

void Grid::computeDrag() {
    drag.clear();
    for (int i = 0; i < getConfig()->getHeight(); i++) {
        for (int j = 0; j < getConfig()->getWidth(); j++) {
            for (int k = 0; k < getConfig()->getLength(); k++) {
                DragWallCell *cell = dynamic_cast<DragWallCell*>(getGrid(i, j, k));
                if (cell != 0) {
                    if (drag.count(cell->getIndex()) == 0) {
                        drag[cell->getIndex()] = MyVector3D();
                    }
                    drag[cell->getIndex()] = drag[cell->getIndex()] + cell->getDragForce(this, Vector3i(i, j, k));
                }
            }
        }
    }
}

int Grid::getDrags() {
    return (int)drag.size();
}

MyVector3D Grid::getDrag(int index) {
    return drag[index];
}

double Grid::partialDerivative(char vAxis, char axis, int x, int y, int z) {
    int nx1 = (axis == 'x' ? x - 1 : x);
    int nx2 = (axis == 'x' ? x + 1 : x);
    int ny1 = (axis == 'y' ? y - 1 : y);
    int ny2 = (axis == 'y' ? y + 1 : y);
    int nz1 = (axis == 'z' ? z - 1 : z);
    int nz2 = (axis == 'z' ? z + 1 : z);
    int cells = 2;
    BaseCell *cell1 = getGrid(ny1, nx1, nz1);
    if (cell1 == 0 || !cell1->isFluid()) {
        cell1 = getGrid(y, x, z);
        cells--;
    }
    BaseCell *cell2 = getGrid(ny2, nx2, nz2);
    if (cell2 == 0 || !cell2->isFluid()) {
        cell2 = getGrid(y, x, z);
        cells--;
    }
    if (cells != 2) {
        return 0;
    }
    double d = 0;
    if (vAxis == 'x') {
        d = cell2->getU(0).getX() - cell1->getU(0).getX();
    } else if (vAxis == 'y') {
        d = cell2->getU(0).getY() - cell1->getU(0).getY();
    } else if (vAxis == 'z') {
        d = cell2->getU(0).getZ() - cell1->getU(0).getZ();
    }
    return d / cells;
}

double* Grid::computeStream(int dx, int dy) {
    double *result = new double[config->getWidth() * config->getHeight()],
            *intu = new double[config->getWidth() * config->getHeight()],
            *intv = new double[config->getWidth() * config->getHeight()];
    for (int i = 0; i < config->getWidth(); i++) {
        for (int j = 0; j < config->getHeight(); j++) {
            intu[i + j * config->getWidth()] = 0;
            intv[i + j * config->getWidth()] = 0;
        }
    }
    for (int y = (dy == 0 ? 1 : config->getHeight() - 2); (dy == 0 && y < config->getHeight()) || (dy == 1 && y >= 0); y = y + (dy == 0 ? 1 : -1)) {
        for (int x = 0; x < config->getWidth(); x++) {
            intu[x + y * config->getWidth()] += intu[x + (y + (dy == 0 ? -1 : 1)) * config->getWidth()];
            if (getGrid(y, x, 0)->isFluid()) {
                intu[x + y * config->getWidth()] += getGrid(y, x, 0)->getU(0).getX();
            }
        }
    }
    for (int x = (dx == 0 ? 1 : config->getWidth() - 2); (dx == 0 && x < config->getWidth()) || (dx == 1 && x >= 0); x = x + (dx == 0 ? 1 : -1)) {
        for (int y = 0; y < config->getHeight(); y++) {
            intv[x + y * config->getWidth()] += intv[(x + (dx == 0 ? -1 : 1)) + y * config->getWidth()];
            if (getGrid(y, x, 0)->isFluid()) {
                intv[x + y * config->getWidth()] += getGrid(y, x, 0)->getU(0).getY();
            }
        }
    }
    for (int i = 0; i < config->getWidth(); i++) {
        for (int j = 0; j < config->getHeight(); j++) {
            result[i + j * config->getWidth()] = intu[i + j * config->getWidth()] - intv[i + j * config->getWidth()];
        }
    }
    delete[] intu;
    delete[] intv;
    return result;
}

MyVector3D* Grid::computeVorticity() {
    MyVector3D* result = new MyVector3D[config->getWidth() * config->getHeight() * config->getLength()];
    for (int i = 0; i < config->getWidth(); i++) {
        for (int j = 0; j < config->getHeight(); j++) {
            for (int k = 0; k < config->getLength(); k++) {
                BaseCell *cell = getGrid(j, i, k);
                if (cell != 0 && cell->isFluid()) {
                    double x = 0, y = 0, z = 0;
                    if (config->getLength() > 1) {
                        x = partialDerivative('z', 'y', i, j, k) - partialDerivative('y', 'z', i, j, k);
                        y = partialDerivative('x', 'z', i, j, k) - partialDerivative('z', 'x', i, j, k);
                    }
                    z = partialDerivative('y', 'x', i, j, k) - partialDerivative('x', 'y', i, j, k);
                    result[i + j * config->getWidth() + k * config->getWidth() * config->getHeight()] = MyVector3D(x, y, z);
                } else {
                    result[i + j * config->getWidth() + k * config->getWidth() * config->getHeight()] = MyVector3D();
                }
            }
        }
    }
    return result;
}

void Grid::optimize() {
    std::list<Vector3i> toNulls;
    if (config->getLength() == 1) {
        for (int i = 1; i < config->getWidth() - 1; i++) {
            for (int j = 1; j < config->getHeight() - 1; j++) {
                WallCell *wall = dynamic_cast<WallCell*>(getGrid(j, i, 0));
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(getGrid(j, i, 0));
                if (psc != 0) {
                    wall = dynamic_cast<WallCell*>(psc->getCell());
                }
                if (wall != 0) {
                    bool wallNeighbors = true;
                    for (int a = 1; a < 9; a++) {
                        WallCell *neighbor = dynamic_cast<WallCell*>(getGrid(j + LBUtil::C[9][a].getY(), i + LBUtil::C[9][a].getX(), 0));
                        PassiveScalarCell *psc2 = dynamic_cast<PassiveScalarCell*>(getGrid(j + LBUtil::C[9][a].getY(), i + LBUtil::C[9][a].getX(), 0));
                        if (psc2 != 0) {
                            neighbor = dynamic_cast<WallCell*>(psc2->getCell());
                        }
                        if (neighbor == 0) {
                            wallNeighbors = false;
                            break;
                        }
                    }
                    if (wallNeighbors) {
                        toNulls.push_back(Vector3i(i, j, 0));
                    }
                }
            }
        }
    }
    for (std::list<Vector3i>::iterator toNull = toNulls.begin(); toNull != toNulls.end(); toNull++) {
        Vector3i v = (*toNull);
        setGrid(v.getY(), v.getX(), v.getZ(), new NullCell());
    }
}

MyVector3D Grid::getFlux(int x, int y, int z) {
    int RESOLUTION = 100;
    BaseCell *cell = getGrid(y, x, 0);
    if (cell->isFluid()) {
        MyVector3D v = cell->getU(0);
        MyVector3D p = MyVector3D(-v.getY(), v.getX(), 0);
        p = p ^ (1.0 / (RESOLUTION * p.norm()));
        MyVector3D flux = v ^ cell->getP(0);
        MyVector3D next = MyVector3D(x + 0.5, y + 0.5, 0);
        while (1) {
            next = next + p;
            BaseCell *cell = getGrid(next.getY(), next.getX(), 0);
            if (cell == 0 || !cell->isFluid()) break;
            flux = flux + ((v ^ ((cell->getU(0) * v) / v.norm2())) ^ cell->getP(0));
        }
        next = MyVector3D(x + 0.5, y + 0.5, 0);
        while (1) {
            next = next - p;
            BaseCell *cell = getGrid(next.getY(), next.getX(), 0);
            if (cell == 0 || !cell->isFluid()) break;
            flux = flux + ((v ^ ((cell->getU(0) * v) / v.norm2())) ^ cell->getP(0));
        }
        return flux ^ (1.0 / RESOLUTION);
    }
    return MyVector3D();
}

struct ParallelFlux {

    ParallelFlux(Grid *grid, MyVector3D *fluxes) {
        this->grid = grid;
        this->fluxes = fluxes;
    }
    //typedef int result_type;

    int operator() (int i) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                fluxes[j + i * grid->getConfig()->getWidth() + k * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()] = grid->getFlux(j, i, k);
            }
        }
        return i;
    }
    Grid *grid;
    MyVector3D *fluxes;
};

MyVector3D* Grid::getFlux() {
    if (!fluxCalculated) {
        if (fluxes != 0) {
            delete[] fluxes;
        }
        fluxes = new MyVector3D[config->getWidth() * config->getHeight() * config->getLength()];
        QtConcurrent::blockingMap(is, ParallelFlux(this, fluxes));
        fluxCalculated = true;
    }
    return fluxes;
}

bool Grid::isRunning() {
    bool temp = (config->getMaxIterations() == 0 || simulation->getIterations() < config->getMaxIterations()) &&
            (simulation->getIterations() == 0 || config->getStopCriterion() == 0 || simulation->getDeltaP() * simulation->getDeltasP() / simulation->getTotalP() >= config->getStopCriterion());
    return temp;
}

KornerImplementation* Grid::getKorner() {
    return korner;
}
