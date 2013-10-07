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

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <QDialog>
class Grid;
class QXmlStreamWriter;
class QXmlStreamReader;

namespace Ui {
    class Interpolation;
}

class Interpolation : public QDialog {
    Q_OBJECT
public:
    explicit Interpolation(Grid *grid, QWidget *parent = 0);
    ~Interpolation();
    void save(QXmlStreamWriter &writer);
    void load(QXmlStreamReader &reader);
private slots:
    void on_addLine_clicked();
    void on_removeLine_clicked();

private:
    Ui::Interpolation *ui;
    Grid *grid;
};

#endif // INTERPOLATION_H
