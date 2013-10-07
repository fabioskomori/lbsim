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

#include "configuration.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

Configuration::Configuration() {
}

Configuration::Configuration(int id, int minx, int maxx, int miny, int maxy, int minz, int maxz, Vector3i color) : id(id), minx(minx), maxx(maxx), miny(miny), maxy(maxy), minz(minz), maxz(maxz), color(color) {
}

void Configuration::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("id", QString::number(id));
    writer.writeAttribute("minx", QString::number(minx));
    writer.writeAttribute("maxx", QString::number(maxx));
    writer.writeAttribute("miny", QString::number(miny));
    writer.writeAttribute("maxy", QString::number(maxy));
    writer.writeAttribute("minz", QString::number(minz));
    writer.writeAttribute("maxz", QString::number(maxz));
    writer.writeAttribute("color1X", QString::number(color.getX()));
    writer.writeAttribute("color1Y", QString::number(color.getY()));
    writer.writeAttribute("color1Z", QString::number(color.getZ()));
}

void Configuration::activate(QXmlStreamReader &reader, Grid*) {
    id = reader.attributes().value("id").toString().toInt();
    minx = reader.attributes().value("minx").toString().toInt();
    maxx = reader.attributes().value("maxx").toString().toInt();
    miny = reader.attributes().value("miny").toString().toInt();
    maxy = reader.attributes().value("maxy").toString().toInt();
    minz = reader.attributes().value("minz").toString().toInt();
    maxz = reader.attributes().value("maxz").toString().toInt();
    color = Vector3i(reader.attributes().value("color1X").toString().toInt(), reader.attributes().value("color1Y").toString().toInt(), reader.attributes().value("color1Z").toString().toInt());
}

int Configuration::getId() {
    return id;
}

int Configuration::getMinX() {
    return minx;
}

int Configuration::getMaxX() {
    return maxx;
}

int Configuration::getMinY() {
    return miny;
}

int Configuration::getMaxY() {
    return maxy;
}

int Configuration::getMinZ() {
    return minz;
}

int Configuration::getMaxZ() {
    return maxz;
}

Vector3i Configuration::getColor() {
    return color;
}
