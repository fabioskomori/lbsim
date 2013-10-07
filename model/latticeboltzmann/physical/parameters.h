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

#ifndef PARAMETERS_H
#define PARAMETERS_H

class QXmlStreamReader;
class QXmlStreamWriter;

class Parameters {
public:
    Parameters();
    static double dtValue(double dx, double kinematicViscosity, double epsilon);
    void config(double dx, double kinematicViscosity, double epsilon);
    void setDM(double dm);
    double dimentionalVelocity(double velocity);
    double nondimentionalVelocity(double velocity);
    double dimentionalLength(double length);
    double nondimentionalLength(double length);
    double dimentionalTime(double time);
    double nondimentionalTime(double time);
    double dimentionalViscosity(double viscosity);
    double nondimentionalViscosity(double viscosity);
    double dimentionalPressure(double pressure);
    double nondimentionalPressure(double pressure);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    double getDX();
    double getDT();
    double getDM();
private:
    double dx, dt, dm;
};

#endif // PARAMETERS_H
