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

#include "velocityfield.h"
#include "../../../model/math/myvector3d.h"
#include <QDebug>
#include "../../../model/latticeboltzmann/basecell.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "particle.h"
#include "startingparticle.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "initialparticle.h"

using std::list;

VelocityField::VelocityField(Grid *grid) {
    this->grid = grid;
    acceleration = 50;
    startingPathLines = new list<StartingParticle*>();
    initialParticles = new list<InitialParticle*>();
    diffusionEffect = 0;
    gridSpacing = 1;
    injectionPeriod = injectionTime = 0;
}

VelocityField::~VelocityField() {
    delete startingPathLines;
    delete initialParticles;
}

void VelocityField::rebuildInitial() {
    initialParticles->clear();
    for (std::list<StartingParticle*>::iterator startingPathLine = startingPathLines->begin(); startingPathLine != startingPathLines->end(); startingPathLine++) {
        std::list<InitialParticle*> temps = (*startingPathLine)->createParticles(gridSpacing);
        for (std::list<InitialParticle*>::iterator temp = temps.begin(); temp != temps.end(); temp++) {
            initialParticles->push_back(*temp);
        }
    }
}

std::list<StartingParticle*>* VelocityField::getStartingPathLines() {
    return startingPathLines;
}

void VelocityField::addStartingPathLine(StartingParticle* particle) {
    startingPathLines->push_back(particle);
    rebuildInitial();
}

void VelocityField::reset(int id) {
    for (std::list<InitialParticle*>::iterator pathLine = initialParticles->begin(); pathLine != initialParticles->end(); pathLine++) {
        if ((*pathLine)->getId() == id) {
            (*pathLine)->reset();
        }
    }
}

void VelocityField::resetAll() {
    for (std::list<InitialParticle*>::iterator pathLine = initialParticles->begin(); pathLine != initialParticles->end(); pathLine++) {
        (*pathLine)->reset();
    }
}

void VelocityField::remove(int id) {
    for (list<StartingParticle*>::iterator startingPathLine = startingPathLines->begin(); startingPathLine != startingPathLines->end(); startingPathLine++) {
        if ((*startingPathLine)->getId() == id) {
            startingPathLine = startingPathLines->erase(startingPathLine);
        }
    }
    reset(id);
    rebuildInitial();
}

void VelocityField::removeAll() {
    startingPathLines->clear();
    rebuildInitial();
}

std::list<InitialParticle*>* VelocityField::getInitialParticles() {
    return initialParticles;
}

void VelocityField::processPathLines() {
    for (list<InitialParticle*>::iterator startingPathLine = initialParticles->begin(); startingPathLine != initialParticles->end(); startingPathLine++) {
        (*startingPathLine)->process(grid);
    }
}

int VelocityField::getAcceleration() {
    return acceleration;
}

void VelocityField::setAcceleration(int acceleration) {
    this->acceleration = acceleration;
}

StartingParticle* VelocityField::getStartingPathLine(int id) {
    for (list<StartingParticle*>::iterator startingPathLine = startingPathLines->begin(); startingPathLine != startingPathLines->end(); startingPathLine++) {
        if ((*startingPathLine)->getId() == id) {
            return *startingPathLine;
        }
    }
    return 0;
}

double VelocityField::getDiffusionEffect() {
    return diffusionEffect;
}

void VelocityField::setDiffusionEffect(double diffusionEffect) {
    this->diffusionEffect = diffusionEffect;
}

void VelocityField::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("acceleration", QString::number(acceleration));
    writer.writeAttribute("particlesQuantity", QString::number(startingPathLines->size()));
    writer.writeAttribute("injectionPeriod", QString::number(injectionPeriod));
    writer.writeAttribute("injectionTime", QString::number(injectionTime));
    writer.writeAttribute("gridSpacing", QString::number(gridSpacing));
    for (list<StartingParticle*>::iterator startingPathLine = startingPathLines->begin(); startingPathLine != startingPathLines->end(); startingPathLine++) {
        writer.writeStartElement("particle");
        (*startingPathLine)->passivate(writer);
        writer.writeEndElement();
    }
}

void VelocityField::activate(QXmlStreamReader &reader) {
    acceleration = reader.attributes().value("acceleration").toString().toInt();
    injectionPeriod = reader.attributes().value("injectionPeriod").toString().toInt();
    injectionTime = reader.attributes().value("injectionTime").toString().toInt();
    gridSpacing = reader.attributes().value("gridSpacing").toString().toInt();
    int particlesQuantity = reader.attributes().value("particlesQuantity").toString().toInt();
    removeAll();
    for (int i = 0; i < particlesQuantity; i++) {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        StartingParticle *newParticle = new StartingParticle(0, Vector3i(), Vector3i(), Vector3i(), Vector3i());
        newParticle->activate(reader);
        startingPathLines->push_back(newParticle);
    }
    rebuildInitial();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
}

int VelocityField::getGridSpacing() {
    return gridSpacing;
}

void VelocityField::setGridSpacing(int gridSpacing) {
    this->gridSpacing = gridSpacing;
    rebuildInitial();
}

int VelocityField::getInjectionPeriod() {
    return injectionPeriod;
}

void VelocityField::setInjectionPeriod(int injectionPeriod) {
    this->injectionPeriod = injectionPeriod;
}

int VelocityField::getInjectionTime() {
    return injectionTime;
}

void VelocityField::setInjectionTime(int injectionTime) {
    this->injectionTime = injectionTime;
}
