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

#ifndef MESHER_H
#define MESHER_H

#include <QDialog>
class LBWidget;
class Parameters2;

namespace Ui {
    class Mesher;
}

class Mesher : public QDialog {
    Q_OBJECT
public:
    explicit Mesher(QWidget *parent = 0);
    void injectWidget(LBWidget *widget);
    void injectParameters(Parameters2 *parameters);
    ~Mesher();
private slots:
    void on_changeCells_clicked();
    void on_resizeGeometry_clicked();
    void on_optimize_clicked();
    void on_porous_clicked();
    void on_newGrid_clicked();
private:
    Ui::Mesher *ui;
    LBWidget *widget;
    Parameters2 *parameters;
};

#endif // MESHER_H
