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

#include "parameterpool.h"
#include "smagorinskyparameter.h"
#include "tauparameter.h"
#include <QString>

std::list<Parameter*>* ParameterPool::parameters;

void ParameterPool::init(LBMainWindow *mainWindow) {
    parameters = new std::list<Parameter*>();
    parameters->push_back(new SmagorinskyParameter(mainWindow));
    parameters->push_back(new TauParameter(mainWindow));
}

std::list<Parameter*>* ParameterPool::list() {
    return parameters;
}

Parameter* ParameterPool::parameterByName(QString name) {
    for (std::list<Parameter*>::iterator parameter = parameters->begin(); parameter != parameters->end(); parameter++) {
        if ((*parameter)->name() == name) {
            return *parameter;
        }
    }
    return 0;
}
