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

#ifndef VIEW2_H
#define VIEW2_H

#include <QDialog>
class LBWidget;

namespace Ui {
    class View2;
}

class View2 : public QDialog {
    Q_OBJECT
public:
    explicit View2(LBWidget *widget, QWidget *parent = 0);
    ~View2();
    Ui::View2* getUI();
private:
    Ui::View2 *ui;
    LBWidget *widget;
    bool fixedX, fixedY, fixedZ;
public slots:
    void on_allZ_clicked();
    void on_allY_clicked();
    void on_allX_clicked();
    void on_fixedZ_valueChanged(int value);
    void on_maxZ_valueChanged(int value);
    void on_minZ_valueChanged(int value);
    void on_fixedY_valueChanged(int value);
    void on_maxY_valueChanged(int value);
    void on_minY_valueChanged(int value);
    void on_fixedX_valueChanged(int value);
    void on_maxX_valueChanged(int value);
    void on_minX_valueChanged(int value);
private slots:
    void on_xyPlane_clicked();
    void on_xzPlane_clicked();
    void on_yzPlane_clicked();
    void on_wholePlane1_clicked();
    void on_wholePlane2_clicked();
    void on_wholePlane3_clicked();
};

#endif // VIEW2_H
