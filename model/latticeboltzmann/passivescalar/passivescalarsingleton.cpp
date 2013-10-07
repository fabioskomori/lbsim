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

#include "passivescalarsingleton.h"
#include "../../../model/math/vector3i.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "passivescalarcell.h"
#include "momentpropagationcell.h"
#include "configuration.h"
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

PassiveScalarSingleton::PassiveScalarSingleton() {
    colors = 0;
    epsilon = 0.5;
    injectionPeriod = injectionTime = 0;
    lastType = 1;
}

void PassiveScalarSingleton::inject(Grid *grid) {
    this->grid = grid;
}

void PassiveScalarSingleton::add(int id, int minx, int maxx, int miny, int maxy, int minz, int maxz, Vector3i color, int type) {
    pss.push_back(new Configuration(id, minx, maxx, miny, maxy, minz, maxz, color));
    lastType = type;
    rebuild(type);
}

Configuration* PassiveScalarSingleton::get(int id) {
    for (std::list<Configuration*>::iterator ps = pss.begin(); ps != pss.end(); ps++) {
        if ((*ps)->getId() == id) {
            return *ps;
        }
    }
    return 0;
}

std::list<Configuration*> PassiveScalarSingleton::listPSS() {
    return pss;
}

void PassiveScalarSingleton::reset(int id) {
    for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
            for (int z = 0; z < grid->getConfig()->getLength(); z++) {
                BaseCell *cell = grid->getGrid(y, x, z);
                if (cell != 0) {
                    PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                    if (psc != 0) {
                        psc->reset(id);
                    }
                }
            }
        }
    }
}

void PassiveScalarSingleton::resetAll() {
    reset(-1);
}

void PassiveScalarSingleton::remove(int id) {
    for (std::list<Configuration*>::iterator ps = pss.begin(); ps != pss.end(); ps++) {
        if ((*ps)->getId() == id) {
            ps = pss.erase(ps);
        }
    }
    rebuild();
}

void PassiveScalarSingleton::removeAll() {
    pss.clear();
    rebuild();
}

void PassiveScalarSingleton::rebuild(int type) {
    if (colors != 0) {
        delete[] colors;
        colors = 0;
    }
    for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
            for (int z = 0; z < grid->getConfig()->getLength(); z++) {
                BaseCell *cell = grid->getGrid(y, x, z);
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                if (psc == 0 && !pss.empty()) {
                    grid->setGrid(y, x, z, type == 1 ? new PassiveScalarCell(cell, -1, -1) : new MomentPropagationCell(cell, -1, -1), false);
                } else if (psc != 0 && pss.empty()) {
                    grid->setGrid(y, x, z, psc->getCell());
                }
                if (!pss.empty()) {
                    psc = dynamic_cast<PassiveScalarCell*>(grid->getGrid(y, x, z));
                    psc->setFixedConcentration(-1);
                    psc->setIndex(-1);
                    psc->initializeDistribution();
                }
            }
        }
    }
    if (!pss.empty()) {
        colors = new Vector3i[pss.size()];
        int index = 0;
        for (std::list<Configuration*>::iterator ps = pss.begin(); ps != pss.end(); ps++) {
            colors[index] = (*ps)->getColor();
            for (int x = (*ps)->getMinX(); x <= (*ps)->getMaxX(); x++) {
                for (int y = (*ps)->getMinY(); y <= (*ps)->getMaxY(); y++) {
                    for (int z = (*ps)->getMinZ(); z <= (*ps)->getMaxZ(); z++) {
                        BaseCell *cell = grid->getGrid(y, x, z);
                        if (cell != 0) {
                            PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                            psc->setFixedConcentration(1);
                            psc->setIndex(index);
                            psc->reset(-1);
                        }
                    }
                }
            }
            index++;
        }
    }
    grid->getConfig()->setProcessNeighbors(true);
}

int PassiveScalarSingleton::getColorsQuantity() {
    return (int)pss.size();
}

Vector3i* PassiveScalarSingleton::getColors() {
    return colors;
}

double PassiveScalarSingleton::getEpsilon(int) {
    return epsilon;
}

void PassiveScalarSingleton::setEpsilon(int, double value) {
    epsilon = value;
}

void PassiveScalarSingleton::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("epsilon", QString::number(epsilon));
    writer.writeAttribute("colorsQuantity", QString::number(pss.size()));
    writer.writeAttribute("injectionPeriod", QString::number(injectionPeriod));
    writer.writeAttribute("injectionTime", QString::number(injectionTime));
    writer.writeAttribute("type", QString::number(lastType));
    int index = 0;
    for (std::list<Configuration*>::iterator ps = pss.begin(); ps != pss.end(); ps++) {
        writer.writeStartElement("color");
        (*ps)->passivate(writer);
        writer.writeEndElement();
        index++;
    }
}

void PassiveScalarSingleton::activate(QXmlStreamReader &reader, Grid *grid) {
    if (reader.name().toString() == "passivescalarsingleton") {
        epsilon = reader.attributes().value("epsilon").toString().toDouble();
        injectionPeriod = reader.attributes().value("injectionPeriod").toString().toInt();
        injectionTime = reader.attributes().value("injectionTime").toString().toInt();
        pss.clear();
        int colorsQuantity = reader.attributes().value("colorsQuantity").toString().toInt();
        for (int i = 0; i < colorsQuantity; i++) {
            do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
            Configuration *conf = new Configuration();
            conf->activate(reader, grid);
            pss.push_back(conf);
        }
        lastType = 1;
        if (reader.attributes().hasAttribute("type")) {
            lastType = reader.attributes().value("type").toString().toInt();
        }
        rebuild(lastType);
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

PassiveScalarSingleton* PassiveScalarSingleton::getInstance() {
    static PassiveScalarSingleton *instance = 0;
    if (instance == 0) {
        instance = new PassiveScalarSingleton();
    }
    return instance;
}

void PassiveScalarSingleton::setInjectionPeriod(int injectionPeriod) {
    this->injectionPeriod = injectionPeriod;
}

int PassiveScalarSingleton::getInjectionPeriod() {
    return injectionPeriod;
}

void PassiveScalarSingleton::setInjectionTime(int injectionTime) {
    this->injectionTime = injectionTime;
}

int PassiveScalarSingleton::getInjectionTime() {
    return injectionTime;
}

int PassiveScalarSingleton::getLastType() {
    return lastType;
}
