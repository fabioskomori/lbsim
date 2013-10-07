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

#include "getcommand.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/basecell.h"
#include <QXmlStreamWriter>

GetCommand::GetCommand(Grid *grid, LBMainWindow *mainWindow) : Command(grid, mainWindow) {
}

QString GetCommand::name() {
    return "get";
}

QString GetCommand::help() {
    return "get <x> <y> <z>";
}

QString GetCommand::execute() {
    QString xml;
    QXmlStreamWriter writer(&xml);
    writer.setAutoFormatting(true);
    BaseCell *cell = grid->getGrid(parameters.at(1).toInt() - 1, parameters.at(0).toInt() - 1, parameters.at(2).toInt() - 1);
    if (cell != 0) {
        writer.writeStartElement("lattice");
        writer.writeAttribute("x", parameters.at(1));
        writer.writeAttribute("y", parameters.at(0));
        writer.writeAttribute("z", parameters.at(2));
        cell->passivate(writer);
        writer.writeEndElement();
    }
    return xml.append("\n");
}

Command* GetCommand::clone() {
    return new GetCommand(grid, mainWindow);
}
