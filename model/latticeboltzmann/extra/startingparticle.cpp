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

#include "startingparticle.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "../../../model/math/myvector3d.h"
#include "particle.h"
#include "initialparticle.h"

StartingParticle::StartingParticle(int id, Vector3i from, Vector3i to, Vector3i color, Vector3i color2) : id(id), from(from), to(to), color(color), color2(color2) {
}

std::list<InitialParticle*> StartingParticle::createParticles(int gridSpacing) {
    std::list<InitialParticle*> result;
    for (int x = from.getX(); x <= to.getX(); x += gridSpacing) {
        for (int y = from.getY(); y <= to.getY(); y += gridSpacing) {
            for (int z = from.getZ(); z <= to.getZ(); z += gridSpacing) {
                result.push_back(new InitialParticle(Vector3i(x, y, z), color/* + ((color2 - color) ^ ((Vector3i(x, y, z) - from).norm() / (to - from).norm())).toVector3i()*/, id));
            }
        }
    }
    return result;
}

void StartingParticle::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("fromX", QString::number(from.getX()));
    writer.writeAttribute("fromY", QString::number(from.getY()));
    writer.writeAttribute("fromZ", QString::number(from.getZ()));
    writer.writeAttribute("toX", QString::number(to.getX()));
    writer.writeAttribute("toY", QString::number(to.getY()));
    writer.writeAttribute("toZ", QString::number(to.getZ()));
    writer.writeAttribute("colorR", QString::number(color.getX()));
    writer.writeAttribute("colorG", QString::number(color.getY()));
    writer.writeAttribute("colorB", QString::number(color.getZ()));
    writer.writeAttribute("color2R", QString::number(color2.getX()));
    writer.writeAttribute("color2G", QString::number(color2.getY()));
    writer.writeAttribute("color2B", QString::number(color2.getZ()));
    writer.writeAttribute("id", QString::number(id));
}

void StartingParticle::activate(QXmlStreamReader &reader) {
    from = Vector3i(reader.attributes().value("fromX").toString().toInt(), reader.attributes().value("fromY").toString().toInt(),reader.attributes().value("fromZ").toString().toInt());
    to = Vector3i(reader.attributes().value("toX").toString().toInt(), reader.attributes().value("toY").toString().toInt(),reader.attributes().value("toZ").toString().toInt());
    color = Vector3i(reader.attributes().value("colorR").toString().toInt(), reader.attributes().value("colorG").toString().toInt(), reader.attributes().value("colorB").toString().toInt());
    color2 = Vector3i(reader.attributes().value("color2R").toString().toInt(), reader.attributes().value("color2G").toString().toInt(), reader.attributes().value("color2B").toString().toInt());
    id = reader.attributes().value("id").toString().toInt();
}

int StartingParticle::getId() {
    return id;
}

Vector3i StartingParticle::getFrom() {
    return from;
}

Vector3i StartingParticle::getTo() {
    return to;
}

Vector3i StartingParticle::getColor() {
    return color;
}
