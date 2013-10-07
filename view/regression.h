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

#ifndef REGRESSION_H
#define REGRESSION_H

#include <QDialog>
#include <list>
#include "../view/util/xypoint.h"
#include "../model/listener/listener.h"
class QXmlStreamWriter;
class QXmlStreamReader;
class LBMainWindow;
class RegressionStrategy;
class QwtPlotCurve;

namespace Ui {
    class Regression;
}

class Regression : public QDialog, public Listener {
    Q_OBJECT
public:
    explicit Regression(LBMainWindow *mainWindow, QWidget *parent = 0);
    ~Regression();
    void save(QXmlStreamWriter &writer);
    void load(QXmlStreamReader &reader);
    void callback(int event);
private slots:
    void on_addLine_clicked();
    void on_removeLine_clicked();
    void on_start_clicked();
    void on_stop_clicked();
private:
    Ui::Regression *ui;
    LBMainWindow *mainWindow;
    RegressionStrategy *strategy;
    std::list<XYPoint> errors, values;
    QwtPlotCurve *errorsCurve, *valuesCurve;
};

#endif // REGRESSION_H
