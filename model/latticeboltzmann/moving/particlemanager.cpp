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

#include "particlemanager.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/sccell.h"
#include "gridchange.h"
#include "movingcell.h"
#include "somethingmoving.h"
#include "movingparticle.h"
#include <QDebug>
#include <cmath>

ParticleManager::ParticleManager(Grid *grid) {
    this->grid = grid;
    particles = new std::list<MovingParticle*>();
    smallParticles = new std::list<MovingParticle*>();
    electrostaticCoefficient = 0.001;
    gravityCoefficient = 0;
    removeOrphanParticles = true;
    normalizeVelocities = false;
}

ParticleManager::~ParticleManager() {
    delete particles;
}

void ParticleManager::transformSmallParticles() {
    for (std::list<MovingParticle*>::iterator mp = particles->begin(); mp != particles->end(); mp++) {
        if ((*mp)->getPoints()->size() == 1) {
            for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
                for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                    for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                        if (grid->getGrid(i, j, k) != 0) {
                            MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid(i, j, k));
                            if (moving != 0) {
                                if (moving->getParticle() == *mp) {
                                    smallParticles->push_back(*mp);
                                    removeOrphanParticles = false;
                                    grid->alter(' ', j, j, i, i, k, k);
                                    removeOrphanParticles = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ParticleManager::preUpdate() {
    for (std::list<MovingParticle*>::iterator sp = smallParticles->begin(); sp != smallParticles->end(); sp++) {
        Vector3i *point = (*(*sp)->getPoints()->begin());
        (*sp)->preUpdate(grid->getConfig()->getEpsilons(), grid->getNeighbors(point->getY(), point->getX(), point->getZ()), grid, Vector3i(point->getX(), point->getY(), point->getZ()));
    }
}

void ParticleManager::update() {
    for (std::list<MovingParticle*>::iterator sp = smallParticles->begin(); sp != smallParticles->end(); sp++) {
        (*sp)->update();
        (*sp)->processSmallParticle(grid);
    }
    if (particles->size() > 0) {
        bool gridChanged = false;
        int dx, dy, dz;
        for (std::list<MovingParticle*>::iterator mp = particles->begin(); mp != particles->end(); mp++) {
            (*mp)->needsGridChange(&dx, &dy, &dz);
            if (dx != 0 || dy != 0 || dz != 0) {
                bool commitChanges = true;
                std::list<GridChange*> fills;
                std::list<Vector3i> removes;
                std::list<GridChange*> changes;
                for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
                    for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
                        for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                            MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid(i, j, k));
                            if (moving != 0 && moving->getParticle() == (*mp)) {
                                BaseCell *to = grid->getGrid(i + dy, j + dx, k + dz);
                                MovingCell *toMoving = dynamic_cast<MovingCell*>(to);
                                if ((to != 0 && to->isFluid()) || (toMoving != 0 && toMoving->getParticle() == (*mp))) {
                                    changes.push_back(new GridChange(Vector3i(i + dy, j + dx, k + dz), moving));
                                    BaseCell *invert = grid->getGrid(i - dy, j - dx, k - dz);
                                    if (invert != 0 && invert->isFluid()) {
                                        fills.push_back(new GridChange(Vector3i(i, j, k), grid->newCell(i, j, k, (*mp)->getVelocity())));
                                    }
                                    if (toMoving != 0 && toMoving->getParticle() == (*mp)) {
                                        removes.push_back(Vector3i(i + dy, j + dx, k + dz));
                                    }
                                } else {
                                    commitChanges = false;
                                }
                            }
                        }
                    }
                }
                if (commitChanges) {
                    gridChanged = true;
                    for (std::list<GridChange*>::iterator change = changes.begin(); change != changes.end(); change++) {
                        grid->setLattice((*change)->getPosition().getX(), (*change)->getPosition().getY(), (*change)->getPosition().getZ(), (*change)->getLattice());
                    }
                    for (std::list<GridChange*>::iterator fill = fills.begin(); fill != fills.end(); fill++) {
                        grid->setLattice((*fill)->getPosition().getX(), (*fill)->getPosition().getY(), (*fill)->getPosition().getZ(), (*fill)->getLattice());
                    }
                    for (std::list<Vector3i>::iterator remove = removes.begin(); remove != removes.end(); remove++) {
                        MovingCell *moving = dynamic_cast<MovingCell*>(grid->getGrid((*remove).getX(), (*remove).getY(), (*remove).getZ()));
                        if (moving == 0) {
                            //delete cell;
                        }
                    }
                }
                (*mp)->updateD(dx, dy, dz, grid, commitChanges);
            }
        }
        if (gridChanged) {
            grid->getConfig()->setProcessNeighbors(true);
        }
        if (normalizeVelocities) {
            double newT2 = 0;
            int cells = 0;
            for (std::list<MovingParticle*>::iterator mp = particles->begin(); mp != particles->end(); mp++) {
                if ((*mp)->getPoints()->size() == 1) {
                    for (unsigned int i = 0; i < (*mp)->getPoints()->size(); i++) {
                        double norm = (*mp)->getVelocity().norm();
                        newT2 += norm * norm;
                        cells++;
                    }
                }
            }
            temp = newT2;
            newT2 /= cells;
            for (std::list<MovingParticle*>::iterator mp = particles->begin(); mp != particles->end(); mp++) {
                if ((*mp)->getPoints()->size() == 1) {
                    (*mp)->setVelocity((*mp)->getVelocity() ^ ((*mp)->getTemperatureCoefficient() / std::sqrt(newT2)));
                }
            }
        }
    }
}

double ParticleManager::getTemp() {
    return temp;
}

void ParticleManager::onLatticeAdded(int i, int j, int k, MovingCell *moving) {
    if (moving->getParticle() == 0) {
        bool found = false;
        Vector3i *v = new Vector3i(i, j, k);
        for (std::list<MovingParticle*>::iterator it = particles->begin(); it != particles->end(); it++) {
            if ((*it)->addPoint(v, moving->getCharge())) {
                moving->setParticle(*it);
                found = true;
                break;
            }
        }
        if (!found) {
            MovingParticle *particle = new MovingParticle(this);
            particle->addPoint(v, moving->getCharge());
            moving->setParticle(particle);
            particles->push_back(particle);
        }
        for (std::list<MovingParticle*>::iterator it = particles->begin(); it != particles->end(); it++) {
            for (std::list<MovingParticle*>::iterator it2 = particles->begin(); it2 != particles->end(); it2++) {
                if (*it != *it2) {
                    if ((*it)->tryMerge(*it2)) {
                        for (int i2 = 0; i2 < grid->getConfig()->getHeight(); i2++) {
                            for (int j2 = 0; j2 < grid->getConfig()->getWidth(); j2++) {
                                for (int k2 = 0; k2 < grid->getConfig()->getLength(); k2++) {
                                    BaseCell *cell = grid->getGrid(i2, j2, k2);
                                    if (cell != 0) {
                                        MovingCell *moving2 = dynamic_cast<MovingCell*>(cell);
                                        if (moving2 != 0 && (moving2->getParticle() == (*it2))) {
                                            moving->setParticle(*it);
                                        }
                                    }
                                }
                            }
                        }
                        it2 = particles->erase(it2);
                    }
                }
            }
        }
    }
}

void ParticleManager::onLatticeRemoved(int i, int j, int k, MovingCell *lattice) {
    MovingParticle *particle = dynamic_cast<MovingParticle*>(lattice->getParticle());
    if (removeOrphanParticles && particle != 0) {
        bool canRemove = true;
        for (int i2 = 0; i2 < grid->getConfig()->getHeight(); i2++) {
            for (int j2 = 0; j2 < grid->getConfig()->getWidth(); j2++) {
                for (int k2 = 0; k2 < grid->getConfig()->getLength(); k2++) {
                    if (i != i2 || j != j2 || k != k2) {
                        MovingCell *moving2 = dynamic_cast<MovingCell*>(grid->getGrid(i2, j2, k2));
                        if (moving2 != 0 && moving2->getParticle() == particle) {
                            canRemove = false;
                            break;
                        }
                    }
                }
            }
        }
        if (canRemove) {
            particles->remove(particle);
        }
    }
}

std::list<MovingParticle*>* ParticleManager::getParticles() {
    return particles;
}

double ParticleManager::getElectrostaticCoefficient() {
    return electrostaticCoefficient;
}

void ParticleManager::setElectrostaticCoefficient(double electrostaticCoefficient) {
    this->electrostaticCoefficient = electrostaticCoefficient;
}

double ParticleManager::getGravityCoefficient() {
    return gravityCoefficient;
}

void ParticleManager::setGravityCoefficient(double gravityCoefficient) {
    this->gravityCoefficient = gravityCoefficient;
}

void ParticleManager::reset() {
    particles->clear();
}

void ParticleManager::setFrozen(bool frozen) {
    for (std::list<MovingParticle*>::iterator particle = particles->begin(); particle != particles->end(); particle++) {
        (*particle)->setFrozen(frozen);
    }
}

bool ParticleManager::getNormalizeVelocities() {
    return normalizeVelocities;
}

void ParticleManager::setNormalizeVelocities(bool normalizeVelocities) {
    this->normalizeVelocities = normalizeVelocities;
}

void ParticleManager::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("electrostatic", QString::number(electrostaticCoefficient));
    writer.writeAttribute("gravity", QString::number(gravityCoefficient));
    writer.writeAttribute("removeOrphan", removeOrphanParticles ? "1" : "0");
    writer.writeAttribute("normalizeVelocities", normalizeVelocities ? "1" : "0");
    writer.writeAttribute("particlesQuantity", QString::number(particles->size()));
    writer.writeAttribute("smallParticlesQuantity", QString::number(smallParticles->size()));
    for (std::list<MovingParticle*>::iterator particle = particles->begin(); particle != particles->end(); particle++) {
        writer.writeStartElement("particle");
        (*particle)->passivate(writer);
        writer.writeEndElement();
    }
    for (std::list<MovingParticle*>::iterator particle = smallParticles->begin(); particle != smallParticles->end(); particle++) {
        writer.writeStartElement("smallParticle");
        writer.writeAttribute("index", QString::number(getParticlesMap(*particle)));
        writer.writeEndElement();
    }
}

void ParticleManager::activate(QXmlStreamReader &reader) {
    electrostaticCoefficient = reader.attributes().value("electrostatic").toString().toDouble();
    gravityCoefficient = reader.attributes().value("gravity").toString().toDouble();
    removeOrphanParticles = reader.attributes().value("removeOrphan").toString() == "1";
    normalizeVelocities = reader.attributes().value("normalizeVelocities").toString() == "1";
    particles = new std::list<MovingParticle*>();
    smallParticles = new std::list<MovingParticle*>();
    int particlesQuantity = reader.attributes().value("particlesQuantity").toString().toInt();
    int smallParticlesQuantity = reader.attributes().value("smallParticlesQuantity").toString().toInt();
    for (int i = 0; i < particlesQuantity; i++) {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        MovingParticle *newMoving = new MovingParticle(this);
        newMoving->activate(reader);
        particles->push_back(newMoving);
    }
    for (int i = 0; i < smallParticlesQuantity; i++) {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        smallParticles->push_back(getParticlesMap(reader.attributes().value("index").toString().toInt()));
    }
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
}

int ParticleManager::getParticlesMap(MovingParticle *searchFor) {
    int result = 0;
    for (std::list<MovingParticle*>::iterator particle = particles->begin(); particle != particles->end(); particle++) {
        if (*particle == searchFor) {
            return result;
        }
        result++;
    }
    return -1;
}

MovingParticle* ParticleManager::getParticlesMap(int position) {
    std::list<MovingParticle*>::iterator particle = particles->begin();
    for (int i = 0; i < position; i++) {
        particle++;
    }
    return *particle;
}
