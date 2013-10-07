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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QMainWindow>

namespace Ui {
    class Geometry;
}

class Geometry : public QMainWindow {
    Q_OBJECT
public:
    explicit Geometry(QWidget *parent = 0);
    ~Geometry();
public slots:
    void run();
private slots:
    void on_actionNew_triggered();
    void on_actionGenerate_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
private:
    Ui::Geometry *ui;
};

#endif // GEOMETRY_H
