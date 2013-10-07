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

#include "parameters.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

Parameters::Parameters() {
    dx = 1e-6;
    dt = 1.0e-6 / 6;
    dm = 8e-18;
}

void Parameters::setDM(double dm) {
    this->dm = dm;
}

double Parameters::dtValue(double dx, double kinematicViscosity, double epsilon) {
    return 1.0 / 3 * (epsilon - 0.5) * dx * dx / kinematicViscosity;
}

void Parameters::config(double dx, double kinematicViscosity, double epsilon) {
    this->dx = dx;
    this->dt = dtValue(dx, kinematicViscosity, epsilon);
}

double Parameters::dimentionalVelocity(double velocity) {
    return velocity * dx / dt;
}

double Parameters::nondimentionalVelocity(double velocity) {
    return velocity * dt / dx;
}

double Parameters::dimentionalLength(double length) {
    return length * dx;
}

double Parameters::nondimentionalLength(double length) {
    return length / dx;
}

double Parameters::dimentionalTime(double time) {
    return time * dt;
}

double Parameters::nondimentionalTime(double time) {
    return time / dt;
}

double Parameters::dimentionalViscosity(double viscosity) {
    return viscosity * dx * dx / dt;
}

double Parameters::nondimentionalViscosity(double viscosity) {
    return viscosity / dx / dx * dt;
}

double Parameters::dimentionalPressure(double pressure) {
    return pressure * dm / dx / dt / dt;
}

double Parameters::nondimentionalPressure(double pressure) {
    return pressure / dm * dx * dt * dt;
}

void Parameters::activate(QXmlStreamReader &reader) {
    dx = reader.attributes().value("dx").toString().toDouble();
    dt = reader.attributes().value("dt").toString().toDouble();
    dm = reader.attributes().value("dm").toString().toDouble();
}

void Parameters::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("dx", QString::number(dx));
    writer.writeAttribute("dt", QString::number(dt));
    writer.writeAttribute("dm", QString::number(dm));
}

double Parameters::getDX() {
    return dx;
}

double Parameters::getDT() {
    return dt;
}

double Parameters::getDM() {
    return dm;
}
