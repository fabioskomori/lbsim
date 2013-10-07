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

#include "forcelist.h"
#include "gravity.h"
#include "passivescalarforce.h"
#include "multicomponent.h"
#include "multiphase.h"
#include <typeinfo>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "../../../model/latticeboltzmann/lbgrid.h"

ForceList::ForceList() {
}

void ForceList::passivate(std::list<Force *> *forces, QXmlStreamWriter &writer) {
    writer.writeStartElement("forces");
    writer.writeAttribute("forcesQuantity", QString::number(forces->size()));
    for (std::list<Force*>::iterator force = forces->begin(); force != forces->end(); force++) {
        writer.writeStartElement("force");
        if (typeid(**force) == typeid(Gravity)) {
            writer.writeAttribute("type", "gravity");
        } else if (typeid(**force) == typeid(MultiComponent)) {
            writer.writeAttribute("type", "multicomponent");
        } else if (typeid(**force) == typeid(MultiPhase)) {
            writer.writeAttribute("type", "multiphase");
        } else if (typeid(**force) == typeid(PassiveScalarForce)) {
            writer.writeAttribute("type", "passiveScalarForce");
        }
        (*force)->passivate(writer);
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

void ForceList::activate(std::list<Force *> *forces, QXmlStreamReader &reader, Grid *grid) {
    int forcesQuantity = reader.attributes().value("forcesQuantity").toString().toInt();
    for (int i = 0; i < forcesQuantity; i++) {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
        Force *force = 0;
        if (reader.attributes().value("type").toString() == "gravity") {
            force = new Gravity();
        } else if (reader.attributes().value("type").toString() == "multicomponent") {
            force = new MultiComponent();
        } else if (reader.attributes().value("type").toString() == "multiphase") {
            force = new MultiPhase();
        } else if (reader.attributes().value("type").toString() == "passiveScalarForce") {
            force = new PassiveScalarForce(0, 0, 0, 0, 0, 0, 0);
        }
        force->activate(reader, grid);
        forces->push_back(force);
    }
}
