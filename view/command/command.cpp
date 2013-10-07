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

#include "command.h"
#include <QXmlStreamWriter>

Command::Command(Grid *grid, LBMainWindow *mainWindow) : grid(grid), mainWindow(mainWindow), canceled(false) {
    canceled = false;
}

void Command::live() {
    canceled = false;
}

void Command::die() {
    canceled = true;
}

bool Command::dead() {
    return canceled;
}

Command* Command::copy(Command *from) {
    parameters.clear();
    for (int i = 0; i < from->parameters.size(); i++) {
        parameters.append(from->parameters.at(i));
    }
    return this;
}

void Command::addParameter(QString parameter) {
    parameters.append(parameter);
}

void Command::setVariable(QString variable, QString value) {
    QStringList newParameters;
    for (int i = 0; i < parameters.count(); i++) {
        QString variableCode(QString("$").append(variable));
        QString toReplace = parameters.at(i);
        QString replaced(toReplace.replace(variableCode, value));
        newParameters.append(replaced);
    }
    parameters.clear();
    for (int i = 0; i < newParameters.size(); i++) {
        parameters.append(newParameters.at(i));
    }
}

QString Command::toString() {
    QString string(name());
    for (int i = 0; i < parameters.size(); i++) {
        string = string.append(" ").append(parameters.at(i));
    }
    return string;
}

void Command::passivate(QXmlStreamWriter &writer) {
    writer.writeStartElement("command");
    writer.writeAttribute("name", name());
    writer.writeAttribute("parametersSize", QString::number(parameters.size()));
    for (int i = 0; i < parameters.size(); i++) {
        writer.writeAttribute(QString("parameter").append(QString::number(i)), parameters.at(i));
    }
    writer.writeEndElement();
}
