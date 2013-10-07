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

#include "gridsimulation.h"
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

GridSimulation::GridSimulation() {
    iterations = 0;
    deltasP = 0;
    deltaP = 0;
    performance = 0;
}

void GridSimulation::resetIterations() {
    iterations = 0;
}

long GridSimulation::addIteration(int steps) {
    iterations += steps;
    return iterations;
}

long GridSimulation::getIterations() {
    return iterations;
}

void GridSimulation::addDeltasP() {
    deltasP++;
}

void GridSimulation::removeDeltasP() {
    deltasP--;
}

int GridSimulation::getDeltasP() {
    return deltasP;
}

void GridSimulation::resetDeltaP() {
    deltaP = 0;
}

void GridSimulation::addDeltaP(double deltaP) {
    this->deltaP += deltaP;
}

void GridSimulation::calcDeltaP() {
    deltaP /= deltasP;
}

double GridSimulation::getDeltaP() {
    return deltaP;
}

void GridSimulation::resetTotalP() {
    totalP = 0;
}

void GridSimulation::resetDeltasP() {
    deltasP = 0;
}

void GridSimulation::addTotalP(double totalP) {
    this->totalP += totalP;
}

double GridSimulation::getTotalP() {
    return totalP;
}

void GridSimulation::setPerformance(double performance) {
    this->performance = performance;
}

double GridSimulation::getPerformance() {
    return performance;
}

void GridSimulation::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("iterations", QString::number(iterations));
    writer.writeAttribute("deltaP", QString::number(deltaP));
    writer.writeAttribute("deltasP", QString::number(deltasP));
}

void GridSimulation::activate(QXmlStreamReader &reader) {
    iterations = reader.attributes().value("iterations").toString().toInt();
    deltaP = reader.attributes().value("deltaP").toString().toDouble();
    deltasP = reader.attributes().value("deltasP").toString().toInt();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
}
