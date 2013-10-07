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

#ifndef PARAMETERS2_H
#define PARAMETERS2_H

#include <QDialog>
class Painter;

namespace Ui {
    class Parameters2;
}

class Parameters2 : public QDialog {
    Q_OBJECT
public:
    explicit Parameters2(QWidget *parent = 0);
    void inject(Painter *painter);
    void load();
    void apply();
    void updateModel();
    Ui::Parameters2* getUI();
    ~Parameters2();
public slots:
    void on_updateInitial_clicked();
    void on_kinematicViscosity_returnPressed();
    void on_dx_returnPressed();
    void on_dm_returnPressed();
    void on_epsilon_returnPressed();
    void on_material_currentIndexChanged(QString text);
    void on_minP0_returnPressed();
    void on_maxP0_returnPressed();
    void on_depositionRate_returnPressed();
    void on_thermalExpansion_returnPressed();
    void on_slipCoefficient_returnPressed();
    void on_thermalGravity_returnPressed();
    void on_thermal1_returnPressed();
    void on_movingVelocity_returnPressed();
    void on_movingVelocityY_returnPressed();
    void on_outletStrategy_currentIndexChanged(int index);
    void on_sourceVelocity_returnPressed();
    void on_velocityOrder_returnPressed();
    void on_smagorinsky_returnPressed();
    void on_shallowNB_returnPressed();
    void on_entropic_clicked();
    void on_shallowG_returnPressed();
    void on_porousSolidDensity_returnPressed();
    void on_periodicBoundaries_clicked();
    void on_mpG_returnPressed();
    void on_mpPsi0_returnPressed();
    void on_mpP0_returnPressed();
    void on_mpGads_returnPressed();
    void on_gravityEnable_clicked();
    void on_model_currentIndexChanged(const QString arg1);
    void on_gravity_returnPressed();
    void on_mcQuantity_currentIndexChanged(int index);
    void on_mcG_returnPressed();
    void on_mcGads_returnPressed();
    void on_mcGads2_returnPressed();
    void on_mcType_currentIndexChanged(int index);
    void on_sourceVelocity2_returnPressed();
    void on_inletOrder_currentIndexChanged(int index);
    void on_thermalIndex_currentIndexChanged(int index);
    void on_movingVelocity2_returnPressed();
    void on_movingVelocityY2_returnPressed();
    void on_fluidOrder_currentIndexChanged(int index);
    void on_anodeCathodeFactor_returnPressed();
    void on_externalForceMinX_returnPressed();
    void on_externalForceStrengthX_returnPressed();
    void on_externalForceStrengthY_returnPressed();
    void on_externalForceMaxX_returnPressed();
    void on_externalForceMinY_returnPressed();
    void on_externalForceMaxY_returnPressed();
    void on_externalForcePeriod_returnPressed();
    void on_sourcePressure_returnPressed();
    void on_regionAll_clicked();
    void on_regionCenter_clicked();
    void on_loadMaskPorousMedia_clicked();
private slots:
    void on_nanoFraction_returnPressed();
    void on_nanoDensity_returnPressed();
    void on_nanoThermalExpansion_returnPressed();
    void on_nanoThermalConductivity_returnPressed();
    void on_nanoHeatCapacitance_returnPressed();
    void on_nanoparticles_currentIndexChanged(const QString &arg1);
    void on_porousVariation_returnPressed();
    void on_updateConstant_clicked();
private:
    Ui::Parameters2 *ui;
    Painter *painter;
    bool avoid_on_mcType_currentIndexChanged;
    bool avoid_on_mcQuantity_currentIndexChanged;
    int material[2];
};

#endif // PARAMETERS2_H
