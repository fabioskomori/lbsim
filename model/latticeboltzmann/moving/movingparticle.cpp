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

#include "movingparticle.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/basecell.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include <QDebug>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "../../../model/util/shared.h"

MovingParticle::MovingParticle(ParticleManager *particleManager) {
    this->particleManager = particleManager;
    dx = dy = 0;
    dz = 0;
    movementQuantity = MyVector3D();
    relativeDensity = 10;
    chargeProcessed = movementProcessed = false;
    electrostaticAcceleration = MyVector3D();
    points = new std::list<Vector3i*>();
    temperatureCoefficient = 0;
    velocity = MyVector3D();
    frozen = false;
}

MovingParticle::~MovingParticle() {
    delete points;
}

void MovingParticle::preUpdate(double[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    addBorderEffect(neighbors, dynamic_cast<MovingCell*>(grid->getGrid(position.getY(), position.getX(), position.getZ())));
    processCharge();
}

void MovingParticle::update() {
    processMovement();
}

MyVector3D MovingParticle::getD() {
    return MyVector3D(dx, dy, dz);
}

void MovingParticle::updateD(int x, int y, int z, Grid *grid, bool updatePoints) {
    dx -= x;
    dy -= y;
    dz -= z;
    if (updatePoints) {
        for (std::list<Vector3i*>::iterator point = points->begin(); point != points->end(); point++) {
            (*point)->addD(y, x, z, true, grid->getConfig()->getHeight(), grid->getConfig()->getWidth(), grid->getConfig()->getLength());
        }
    }
}

void MovingParticle::needsGridChange(int *dx, int *dy, int *dz) {
    *dx = 0;
    *dy = 0;
    *dz = 0;
    if (this->dx < -0.5) {
        *dx = -1;
    } else if (this->dx > 0.5) {
        *dx = 1;
    }
    if (this->dy < -0.5) {
        *dy = -1;
    } else if (this->dy > 0.5) {
        *dy = 1;
    }
    if (this->dz < -0.5) {
        *dz = -1;
    } else if (this->dz > 0.5) {
        *dz = 1;
    }
}

bool MovingParticle::addPoint(Vector3i *newPoint, int charge) {
    if (points->size() == 0) {
        this->charge = charge;
        points->push_back(newPoint);
        return true;
    }
    if (this->charge == charge) {
        for (std::list<Vector3i*>::iterator point = points->begin(); point != points->end(); point++) {
            if ((*point)->isNeighbor(newPoint)) {
                points->push_back(newPoint);
                return true;
            }
        }
    }
    return false;
}

bool MovingParticle::tryMerge(MovingParticle *moving) {
    bool merge = false;
    for (std::list<Vector3i*>::iterator point = points->begin(); point != points->end(); point++) {
        for (std::list<Vector3i*>::iterator point2 = moving->getPoints()->begin(); point2 != moving->getPoints()->end(); point2++) {
            if ((*point)->isNeighbor(*point2)) {
                merge = true;
            }
        }
    }
    if (merge) {
        for (std::list<Vector3i*>::iterator point2 = moving->getPoints()->begin(); point2 != moving->getPoints()->end(); point2++) {
            points->push_back(*point2);
        }
    }
    return merge;
}

std::list<Vector3i*>* MovingParticle::getPoints() {
    return points;
}

void MovingParticle::addBorderEffect(BaseCell* neighbors[], MovingCell*) {
    if (neighbors != 0) {
        int model = Shared::instance()->getGridConfig()->getModel();
        for (int i = 1; i < model; i++) {
            BaseCell *cell = neighbors[i];
            if (cell != 0) {
                if (cell->isFluid()) {
                    movementQuantity = movementQuantity + (LBUtil::C[model][LBUtil::OPPOSITE[model][i]] ^ (cell->getF(LBUtil::OPPOSITE[model][i], -1) + cell->getF(i, -1)));
                }
            }
        }
    }
}

int MovingParticle::getCharge() {
    return charge;
}

void MovingParticle::processCharge() {
    if (!chargeProcessed) {
        electrostaticAcceleration = MyVector3D();
        electricPotential = 0;
        std::list<MovingParticle*> *particles = particleManager->getParticles();
        for (std::list<MovingParticle*>::iterator particle = particles->begin(); particle != particles->end(); particle++) {
            if ((*particle) != this) {
                MyVector3D thisMassCenter = this->getMassCenter();
                MyVector3D thatMassCenter = (*particle)->getMassCenter();
                MyVector3D direction = MyVector3D(thisMassCenter.getX() - thatMassCenter.getX(), thisMassCenter.getY() - thatMassCenter.getY(), thisMassCenter.getZ() - thatMassCenter.getZ());
                double norm2 = direction.norm2();
                electricPotential += this->getTotalCharge() * (*particle)->getTotalCharge() / std::sqrt(norm2) + 1.0 / (12 * norm2 * norm2 * norm2 * norm2 * norm2 * norm2);
                electrostaticAcceleration = electrostaticAcceleration + (direction ^ (particleManager->getElectrostaticCoefficient() * this->getTotalCharge() * (*particle)->getTotalCharge() * (1.0 / norm2) - 1.0 / (norm2 * norm2 * norm2 * norm2 * norm2 * norm2)));
            }
        }
        chargeProcessed = true;
    }
    movementProcessed = false;
}

void MovingParticle::processSmallParticle(Grid*) {
    //Vector3i *point = *points->begin();
    /*if (!grid->getGrid(point->getY() + dy, point->getX() + dx, point->getZ() + dz)->isFluid()) {
        velocity = MyVector3D(dx != 0 ? -velocity.getX() : velocity.getX(), dy != 0 ? -velocity.getY() : velocity.getY(), velocity.getZ());
    }*/
    //MyVector3D point2 = MyVector3D(point->getX() + dx, point->getY() + dy, point->getZ() + dz);
    //MyVector3D newPoint = point2.periodic(grid->getConfig()->getWidth() - 1, grid->getConfig()->getHeight() - 1, grid->getConfig()->getLength() - 1);
    //MyVector3D newPoint = point2;
    /*dx = newPoint.getX() - point->getX();
    dy = newPoint.getY() - point->getY();
    dz = newPoint.getZ() - point->getZ();*/
    //updateD(dx, dy, dz, true);
}

void MovingParticle::processMovement() {
    if (!movementProcessed) {
        if (!frozen) {
            double mass = relativeDensity * points->size();
            MyVector3D averageMovementQuantity = (movementQuantity + lastMovementQuantity) ^ 0.5;
            MyVector3D newVelocity = velocity + (MyVector3D(averageMovementQuantity.getX() / relativeDensity + electrostaticAcceleration.getY() + ((double)rand() / RAND_MAX * 2 - 1) * temperatureCoefficient,
                                           averageMovementQuantity.getY() / relativeDensity + electrostaticAcceleration.getX() + ((double)rand() / RAND_MAX * 2 - 1) * temperatureCoefficient - particleManager->getGravityCoefficient() * mass,
                                           averageMovementQuantity.getZ() / relativeDensity + electrostaticAcceleration.getZ()) ^ (1.0 / mass));
            /*MyVector3D newVelocity = velocity + (MyVector3D(averageMovementQuantity.getX() / relativeDensity + electrostaticAcceleration.getY(),
                                           averageMovementQuantity.getY() / relativeDensity + electrostaticAcceleration.getX() - particleManager->getGravityCoefficient() * mass,
                                           averageMovementQuantity.getZ() / relativeDensity + electrostaticAcceleration.getZ()) ^ (1.0 / mass));*/
            MyVector3D averageVelocity = (velocity + newVelocity) ^ 0.5;
            //averageVelocity = averageVelocity.limit(0.1);
            dx += averageVelocity.getX();
            dy += averageVelocity.getY();
            dz += averageVelocity.getZ();
            velocity = newVelocity;
        }
        lastMovementQuantity = movementQuantity;
        movementQuantity = MyVector3D();
        movementProcessed = true;
    }
    chargeProcessed = false;
}

void MovingParticle::setRelativeDensity(double relativeDensity) {
    this->relativeDensity = relativeDensity;
}

double MovingParticle::getRelativeDensity() {
    return relativeDensity;
}

int MovingParticle::getTotalCharge() {
    return (int)points->size() * charge;
}

MyVector3D MovingParticle::getMassCenter() {
    MyVector3D result = MyVector3D();
    for (std::list<Vector3i*>::iterator point = points->begin(); point != points->end(); point++) {
        result = result + MyVector3D((*point)->getX(), (*point)->getY(), (*point)->getZ());
    }
    result = result ^ (1.0 / points->size());
    result = result + MyVector3D(dy, dx, dz);
    return result;
}

MyVector3D MovingParticle::getLastMovementQuantity() {
    return lastMovementQuantity;
}

MyVector3D MovingParticle::getElectrostaticAcceleration() {
    return electrostaticAcceleration;
}

void MovingParticle::setTemperatureCoefficient(double temperatureCoefficient) {
    this->temperatureCoefficient = temperatureCoefficient;
}

double MovingParticle::getTemperatureCoefficient() {
    return temperatureCoefficient;
}

MyVector3D MovingParticle::getVelocity() {
    return velocity;
}

void MovingParticle::setVelocity(MyVector3D velocity) {
    this->velocity = velocity;
}

void MovingParticle::setFrozen(bool frozen) {
    this->frozen = frozen;
}

bool MovingParticle::getFrozen() {
    return frozen;
}

double MovingParticle::getElectricPotential() {
    return electricPotential;
}

void MovingParticle::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("frozen", frozen ? "1" : "0");
    writer.writeAttribute("velocityX", QString::number(velocity.getX()));
    writer.writeAttribute("velocityY", QString::number(velocity.getY()));
    writer.writeAttribute("velocityZ", QString::number(velocity.getZ()));
    writer.writeAttribute("temperature", QString::number(temperatureCoefficient));
    writer.writeAttribute("charge", QString::number(charge));
    writer.writeAttribute("relativeDensity", QString::number(relativeDensity));
    writer.writeAttribute("movementQuantityX", QString::number(movementQuantity.getX()));
    writer.writeAttribute("movementQuantityY", QString::number(movementQuantity.getY()));
    writer.writeAttribute("movementQuantityZ", QString::number(movementQuantity.getZ()));
    writer.writeAttribute("lastMovementQuantityX", QString::number(lastMovementQuantity.getX()));
    writer.writeAttribute("lastMovementQuantityY", QString::number(lastMovementQuantity.getY()));
    writer.writeAttribute("lastMovementQuantityZ", QString::number(lastMovementQuantity.getZ()));
    writer.writeAttribute("electrostaticAccelerationX", QString::number(electrostaticAcceleration.getX()));
    writer.writeAttribute("electrostaticAccelerationY", QString::number(electrostaticAcceleration.getY()));
    writer.writeAttribute("electrostaticAccelerationZ", QString::number(electrostaticAcceleration.getZ()));
    writer.writeAttribute("chargeProcessed", chargeProcessed ? "1" : "0");
    writer.writeAttribute("movementProcessed", movementProcessed ? "1" : "0");
    writer.writeAttribute("gridx", QString::number(gridx));
    writer.writeAttribute("gridy", QString::number(gridy));
    writer.writeAttribute("gridz", QString::number(gridz));
    writer.writeAttribute("dx", QString::number(dx));
    writer.writeAttribute("dy", QString::number(dy));
    writer.writeAttribute("dz", QString::number(dz));
    writer.writeAttribute("pointsSize", QString::number(points->size()));
    for (std::list<Vector3i*>::iterator point = points->begin(); point != points->end(); point++) {
        writer.writeStartElement("point");
        writer.writeAttribute("x", QString::number((*point)->getX()));
        writer.writeAttribute("y", QString::number((*point)->getY()));
        writer.writeAttribute("z", QString::number((*point)->getZ()));
        writer.writeEndElement();
    }
}

void MovingParticle::activate(QXmlStreamReader &reader) {
    frozen = reader.attributes().value("frozen").toString() == "1";
    velocity = MyVector3D(reader.attributes().value("velocityX").toString().toDouble(), reader.attributes().value("velocityY").toString().toDouble(), reader.attributes().value("velocityZ").toString().toDouble());
    temperatureCoefficient = reader.attributes().value("temperature").toString().toDouble();
    charge = reader.attributes().value("charge").toString().toInt();
    relativeDensity = reader.attributes().value("relativeDensity").toString().toDouble();
    movementQuantity = MyVector3D(reader.attributes().value("movementQuantityX").toString().toDouble(), reader.attributes().value("movementQuantityY").toString().toDouble(), reader.attributes().value("movementQuantityZ").toString().toDouble());
    lastMovementQuantity = MyVector3D(reader.attributes().value("lastMovementQuantityX").toString().toDouble(), reader.attributes().value("lastMovementQuantityY").toString().toDouble(), reader.attributes().value("lastMovementQuantityZ").toString().toDouble());
    electrostaticAcceleration = MyVector3D(reader.attributes().value("electrostaticAccelerationX").toString().toDouble(), reader.attributes().value("electrostaticAccelerationY").toString().toDouble(), reader.attributes().value("electrostaticAccelerationZ").toString().toDouble());
    chargeProcessed = reader.attributes().value("chargeProcessed").toString() == "1";
    movementProcessed = reader.attributes().value("movementProcessed").toString() == "1";
    gridx = reader.attributes().value("gridx").toString().toInt();
    gridy = reader.attributes().value("gridy").toString().toInt();
    gridz = reader.attributes().value("gridz").toString().toInt();
    dx = reader.attributes().value("dx").toString().toDouble();
    dy = reader.attributes().value("dy").toString().toDouble();
    dz = reader.attributes().value("dz").toString().toDouble();
    int pointsSize = reader.attributes().value("pointsSize").toString().toInt();
    for (int i = 0; i < pointsSize; i++) {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        points->push_back(new Vector3i(reader.attributes().value("x").toString().toInt(), reader.attributes().value("y").toString().toInt(), reader.attributes().value("z").toString().toInt()));
    }
}

SomethingMoving* MovingParticle::clone() {
    return 0;
}
