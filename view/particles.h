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

#ifndef PARTICLES_H
#define PARTICLES_H

#include <QDialog>
#include <QStringListModel>
class LBWidget;

namespace Ui {
    class Particles;
}

class Particles : public QDialog {
    Q_OBJECT
public:
    explicit Particles(LBWidget *widget, QWidget *parent = 0);
    ~Particles();
    void load();
private:
    void controls(bool enabled);
    void particlesOrPassiveScalar();
    void updateNextId();
    Ui::Particles *ui;
    LBWidget *widget;
    QStringListModel *stringListModel;
    QStringList stringList;
    int nextId;
    QColor color;
    QColor color2;
public slots:
    void on_diffusionEffect_returnPressed();
    void on_velocity_returnPressed();
    void on_deleteAll_clicked();
    void on_resetAll_clicked();
    void on_particlesDelete_clicked();
    void on_reset_clicked();
    void on_update_clicked();
    void on_particles_clicked(QModelIndex index);
    void on_color_clicked();
    void on_particlesNew_clicked();
private slots:
    void on_gridSpacing_returnPressed();
    void on_injectionPeriod_returnPressed();
    void on_color2_clicked();
    void on_particlesType_clicked();
    void on_passiveScalarType_clicked();
    void on_injectionTime_returnPressed();
};

#endif // PARTICLES_H
