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

#include "momentpropagationcell.h"
#include "passivescalarsingleton.h"
#include "../../../model/math/vector3i.h"
#include "../../../model/latticeboltzmann/nullcell.h"
#include "../../../model/latticeboltzmann/boundary/opencell.h"
#include "../../../model/util/shared.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "../../../model/latticeboltzmann/lbutil.h"
#include "../../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../../model/latticeboltzmann/boundary/reflectwithfactorcell.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "../../../model/latticeboltzmann/lbgrid.h"

MomentPropagationCell::MomentPropagationCell(BaseCell *cell, double fixedConcentration, int index) : PassiveScalarCell(cell, fixedConcentration, index) {
}

void MomentPropagationCell::initializeDistribution() {
    clearMemory();
    int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
    f = new double[colors];
    for (int i = 0; i < colors; i++) {
        f[i] = 0;
    }
    nextF = new double[colors];
    for (int i = 0; i < colors; i++) {
        nextF[i] = 0;
    }
    p = new double[colors];
    for (int i = 0; i < colors; i++) {
        p[i] = 0;
    }
}

void MomentPropagationCell::calc() {
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        p[a] = f[a];
    }
}

void MomentPropagationCell::reset(int) {
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        f[a] = 0;
    }
    calc();
}

void MomentPropagationCell::reset(double p0, double p1) {
    cell->reset(p0, p1);
    if (p1 != 0) {
        f[0] = p1;
        calc();
    }
}

void MomentPropagationCell::preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    cell->preUpdate(epsilon, neighbors, grid, position);
}

void MomentPropagationCell::preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position) {
    cell->preUpdate2(epsilon, neighbors, grid, position);
    if (dynamic_cast<NullCell*>(cell) == 0) {
        int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
        int model = grid->getConfig()->getModel();
        for (int a = 0; a < colors; a++) {
            if (fixedConcentration > 0 && a == index) {
                nextF[a] = fixedConcentration;
            } else if (cell->isFluid()) {
                double newEpsilon = PassiveScalarSingleton::getInstance()->getEpsilon(a);
                double newNextF = 0;
                for (int i = 0; i < model; i++) {
                    if (neighbors[i] != 0) {
                        if (neighbors[i]->isFluid()) {
                            //newNextF += neighbors[i]->getF(0, 10 + a) * (cell->getNextF(LBUtil::OPPOSITE[model][i], 0) - newEpsilon / model) / neighbors[i]->getP(0);
                            newNextF += (neighbors[i]->getF(LBUtil::OPPOSITE[model][i], 0) / neighbors[i]->getP(0) - LBUtil::W[model][i] * newEpsilon) * neighbors[i]->getF(0, 10 + a);
                        } else {
                            OpenCell *open = dynamic_cast<OpenCell*>(neighbors[i]);
                            if (open != 0) {
                                //newNextF += f[a] * (cell->getNextF(LBUtil::OPPOSITE[model][i], 0) - newEpsilon / model) / cell->getP(0);
                                //newNextF += (cell->getF(LBUtil::OPPOSITE[model][i], 0) / cell->getP(0) - LBUtil::W[model][i] * newEpsilon) * f[a];
                            } else {
                                //newNextF += f[a] * (cell->getNextF(i, 0) - newEpsilon / model) / cell->getP(0);
                                newNextF += (cell->getF(i, 0) / cell->getP(0) - LBUtil::W[model][i] * newEpsilon) * f[a];
                            }
                        }
                    }
                }
                //newNextF += newEpsilon * f[a] / cell->getP(0);
                newNextF += newEpsilon * f[a];
                nextF[a] = newNextF;
            }
        }
    }
}

void MomentPropagationCell::update() {
    cell->update();
    if (dynamic_cast<NullCell*>(cell) == 0) {
        int colors = PassiveScalarSingleton::getInstance()->getColorsQuantity();
        for (int a = 0; a < colors; a++) {
            f[a] = nextF[a];
        }
        calc();
    }
}

double MomentPropagationCell::getF(int index, int component) {
    return component < 10 ? cell->getF(index, component) : f[(component - 10)];
}

void MomentPropagationCell::setNextF(int index, double value, int component) {
    if (component < 10) {
        cell->setNextF(index, value, component);
    }
}

void MomentPropagationCell::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("type", "momentPropagationCell");
    writer.writeAttribute("fixedConcentration", QString::number(fixedConcentration));
    writer.writeAttribute("index", QString::number(index));
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        writer.writeAttribute(QString("f").append(QString::number(a)), QString::number(f[a]));
    }
    writer.writeStartElement("cell");
    cell->passivate(writer);
    writer.writeEndElement();
}

void MomentPropagationCell::activate(QXmlStreamReader &reader, Grid *grid) {
    fixedConcentration = reader.attributes().value("fixedConcentration").toString().toDouble();
    index = reader.attributes().value("index").toString().toInt();
    initializeDistribution();
    for (int a = 0; a < PassiveScalarSingleton::getInstance()->getColorsQuantity(); a++) {
        f[a] = reader.attributes().value(QString("f").append(QString::number(a))).toString().toDouble();
    }
    calc();
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    cell = grid->activateCell(reader);
    cell->activate(reader, grid);
}
