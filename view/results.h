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

#ifndef RESULTS_H
#define RESULTS_H

class Painter;
#include <QDialog>
class QXmlStreamWriter;
class QXmlStreamReader;

namespace Ui {
    class Results;
}

class Results : public QDialog {
    Q_OBJECT
public:
    explicit Results(Painter *painter, QWidget *parent = 0);
    ~Results();
    bool getSaveImage();
    bool getReportVelocities();
    int getSaveInterval();
    bool getReportSimulation();
    Ui::Results* getUI();
    void save(QXmlStreamWriter &writer);
    void load(QXmlStreamReader &reader);
public slots:
    void on_maxSteps_returnPressed();
    //void on_matlabStreamlines_clicked();
    void on_stopCriterion_returnPressed();
    //void on_csvGenerate_clicked();
    //void on_simX_returnPressed();
    //void on_simY_returnPressed();
    //void on_simZ_returnPressed();
private slots:
    void on_addPoint_clicked();

    void on_removePoint_clicked();

    void on_updatePoints_clicked();

private:
    Painter *painter;
    Ui::Results *ui;
};

#endif // RESULTS_H
