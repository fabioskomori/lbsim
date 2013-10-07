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

#ifndef REGRESSIONSTRATEGY_H
#define REGRESSIONSTRATEGY_H

#include <list>
class LBMainWindow;
class ParameterControl;
#include "../../model/listener/listener.h"
#include "datapoint.h"

class RegressionStrategy : public Listener {
public:
    RegressionStrategy();
    void addDataPoint(DataPoint dataPoint);
    void addParameter(ParameterControl *parameter);
    void injectMainWindow(LBMainWindow *mainWindow);
    double squaredResiduals();
    double standardDeviation();
    void callback(int event);
    void setListener(Listener *listener);
    void notify();
    virtual void process() = 0;
    double getNewError();
    double getNewValue();
protected:
    std::list<DataPoint> dataPoints;
    std::list<ParameterControl*> parameters;
    LBMainWindow *mainWindow;
    Listener *listener;
    double newError, newValue;
};

#endif // REGRESSIONSTRATEGY_H
