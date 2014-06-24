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

#include "parameters2.h"
#include "ui_parameters2.h"
#include "../view/painter/painter.h"
#include "../model/latticeboltzmann/force/passivescalarforce.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/multi/mccell.h"
#include "../model/latticeboltzmann/sccell.h"
#include "../model/latticeboltzmann/physical/parameters.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "../model/latticeboltzmann/porouscell.h"
#include "../model/latticeboltzmann/meltingsolidification/meltingsolidificationcell.h"
#include "../model/latticeboltzmann/meltingsolidification/kornerimplementation.h"
#include <cstdlib>
#include <QDebug>
#include <QFileDialog>

Parameters2::Parameters2(QWidget *parent) : QDialog(parent), ui(new Ui::Parameters2) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    ui->epsilon->setAllowDecimal(true);
    ui->dx->setAllowDecimal(true);
    ui->dm->setAllowDecimal(true);
    ui->sourceVelocity->setAllowDecimal(true);
    ui->sourceVelocity2->setAllowDecimal(true);
    ui->sourcePressure->setAllowDecimal(true);
    ui->kinematicViscosity->setAllowDecimal(true);
    ui->minP0->setAllowDecimal(true);
    ui->maxP0->setAllowDecimal(true);
    ui->movingVelocity->setAllowDecimal(true);
    ui->movingVelocity->setAllowNegative(true);
    ui->movingVelocityY->setAllowNegative(true);
    ui->movingVelocityY->setAllowDecimal(true);
    ui->thermal1->setAllowDecimal(true);
    ui->thermalGravity->setAllowDecimal(true);
    ui->slipCoefficient->setAllowDecimal(true);
    ui->depositionRate->setAllowDecimal(true);
    ui->mpPsi0->setAllowDecimal(true);
    ui->mpG->setAllowNegative(true);
    ui->mpGads->setAllowNegative(true);
    ui->mcG->setAllowNegative(true);
    ui->mcG->setAllowDecimal(true);
    ui->gravity->setAllowDecimal(true);
    ui->gravity->setAllowNegative(true);
    ui->mcG->setAllowNegative(true);
    ui->mcGads->setAllowNegative(true);
    ui->mcGads->setAllowDecimal(true);
    ui->mcGads2->setAllowDecimal(true);
    ui->mcGads2->setAllowNegative(true);
    ui->porousSolidDensity->setAllowDecimal(true);
    ui->shallowG->setAllowDecimal(true);
    ui->shallowNB->setAllowDecimal(true);
    ui->smagorinsky->setAllowDecimal(true);
    ui->anodeCathodeFactor->setAllowDecimal(true);
    ui->externalForceStrengthX->setAllowDecimal(true);
    ui->externalForceStrengthX->setAllowNegative(true);
    ui->externalForceStrengthY->setAllowDecimal(true);
    ui->externalForceStrengthY->setAllowNegative(true);
    ui->nanoFraction->setAllowDecimal(true);
    ui->nanoDensity->setAllowDecimal(true);
    ui->nanoThermalExpansion->setAllowDecimal(true);
    ui->nanoThermalConductivity->setAllowDecimal(true);
    ui->nanoHeatCapacitance->setAllowDecimal(true);
    ui->porousVariation->setAllowDecimal(true);
    avoid_on_mcType_currentIndexChanged = false;
    avoid_on_mcQuantity_currentIndexChanged = false;
    material[0] = material[1] = 0;
    //on_activateKorner_clicked();
    ui->ebmPower->setAllowDecimal(true);
    ui->ebmStdDev->setAllowDecimal(true);
    ui->ebmVelocity->setAllowDecimal(true);
    ui->ebmAbsorptionCoefficient->setAllowDecimal(true);
    ui->mcDensityRatio->setAllowDecimal(true);
}

void Parameters2::load() {
    ui->anodeCathodeFactor->setText(QString::number(painter->getGrid()->getConfig()->getReflectWithFactor()));
    ui->dx->setText(QString::number(painter->getGrid()->getParameters()->getDX()));
    ui->dt->setText(QString::number(painter->getGrid()->getParameters()->getDT()));
    ui->dm->setText(QString::number(painter->getGrid()->getParameters()->getDM()));
    ui->epsilon->setText(QString::number(painter->getGrid()->getConfig()->getEpsilons()[0], 'g', 12));
    ui->sourceVelocity->setText(QString::number(painter->getGrid()->getConfig()->getSourceVelocity(0), 'g', 12));
    ui->minP0->setText(QString::number(painter->getGrid()->getConfig()->getMinP0(), 'g', 12));
    ui->maxP0->setText(QString::number(painter->getGrid()->getConfig()->getMaxP0(), 'g', 12));
    ui->movingVelocity->setText(QString::number(painter->getGrid()->getConfig()->getWallVelocity().getX(), 'g', 12));
    ui->movingVelocityY->setText(QString::number(painter->getGrid()->getConfig()->getWallVelocity().getY(), 'g', 12));
    ui->thermal1->setText(QString::number(painter->getGrid()->getConfig()->getThermal(0)));
    ui->thermalGravity->setText(QString::number(painter->getGrid()->getConfig()->getPassiveScalarGravity()));
    ui->outletStrategy->setCurrentIndex(painter->getGrid()->getConfig()->getOpen());
    ui->slipCoefficient->setText(QString::number(painter->getGrid()->getConfig()->getSlipWallCoefficient()));
    ui->thermalExpansion->setText(QString::number(painter->getGrid()->getConfig()->getThermalExpansion()));
    ui->depositionRate->setText(QString::number(painter->getGrid()->getConfig()->getDepositionRate()));
    qDebug() << "gravity: " << painter->getGrid()->getConfig()->getGravity();
    if (painter->getGrid()->getConfig()->getGravity() < -1e-100 || painter->getGrid()->getConfig()->getGravity() > 1e-100) {
        ui->gravityEnable->setChecked(true);
        on_gravityEnable_clicked();
        ui->gravity->setText(QString::number(painter->getGrid()->getConfig()->getGravity(), 'g', 12));
    } else {
        ui->gravityEnable->setChecked(false);
        on_gravityEnable_clicked();
    }
    double mpG, mpPsi0, mpP0, mpGads;
    painter->getGrid()->getConfig()->getMultiphase(&mpG, &mpPsi0, &mpP0, &mpGads);
    ui->mpG->setText(QString::number(mpG, 'g', 12));
    ui->mpPsi0->setText(QString::number(mpPsi0, 'g', 12));
    ui->mpP0->setText(QString::number(mpP0, 'g', 12));
    ui->mpGads->setText(QString::number(mpGads, 'g', 12));
    ui->periodicBoundaries->setChecked(painter->getGrid()->getConfig()->getPeriodicBoundaries());
    int mcQuantity, mcType;
    double mcG, mcGads, mcGads2;
    painter->getGrid()->getConfig()->getMulticomponent(&mcQuantity, &mcG, &mcGads, &mcGads2, &mcType);
    ui->mcG->setText(QString::number(mcG, 'g', 12));
    avoid_on_mcQuantity_currentIndexChanged = true;
    ui->mcQuantity->setCurrentIndex(mcQuantity - 1);
    ui->mcGads->setText(QString::number(mcGads, 'g', 12));
    ui->mcGads2->setText(QString::number(mcGads2, 'g', 12));
    avoid_on_mcQuantity_currentIndexChanged = false;
    avoid_on_mcType_currentIndexChanged = true;
    ui->mcType->setCurrentIndex(mcType - 1);
    avoid_on_mcType_currentIndexChanged = false;
    switch (painter->getGrid()->getConfig()->getModel()) {
    case 9:
        ui->model->setCurrentIndex(0);
        break;
    case 15:
        ui->model->setCurrentIndex(1);
        break;
    case 19:
        ui->model->setCurrentIndex(2);
        break;
    case 27:
        ui->model->setCurrentIndex(3);
        break;
    }
    ui->porousSolidDensity->setText(QString::number(painter->getGrid()->getConfig()->getSolidDensity(), 'g', 12));
    ui->shallowG->setText(QString::number(painter->getGrid()->getConfig()->getShallowG()));
    ui->shallowNB->setText(QString::number(painter->getGrid()->getConfig()->getShallowNB()));
    ui->smagorinsky->setText(QString::number(painter->getGrid()->getConfig()->getSmagorinsky()));
    ui->entropic->setChecked(painter->getGrid()->getConfig()->getEntropic());
    ui->velocityOrder->setText(QString::number(painter->getGrid()->getConfig()->getVelocityOrder()));
    double nanoFraction, nanoDensity, nanoThermalExpansion, nanoThermalConductivity, nanoHeatCapacitance;
    painter->getGrid()->getConfig()->getNanoValues(&nanoFraction, &nanoDensity, &nanoThermalExpansion, &nanoThermalConductivity, &nanoHeatCapacitance);
    ui->nanoFraction->setText(QString::number(nanoFraction));
    ui->nanoDensity->setText(QString::number(nanoDensity));
    ui->nanoThermalExpansion->setText(QString::number(nanoThermalExpansion));
    ui->nanoThermalConductivity->setText(QString::number(nanoThermalConductivity));
    ui->nanoHeatCapacitance->setText(QString::number(nanoHeatCapacitance));
    std::list<Force*>* forces = painter->getGrid()->getConfig()->getForces();
    for (std::list<Force*>::iterator force = forces->begin(); force != forces->end(); force++) {
        PassiveScalarForce *f = dynamic_cast<PassiveScalarForce*>(*force);
        if (f != 0) {
            ui->externalForceStrengthX->setText(QString::number(f->getStrengthX()));
            ui->externalForceStrengthY->setText(QString::number(f->getStrengthY()));
            ui->externalForceMinX->setText(QString::number(f->getMinX()));
            ui->externalForceMaxX->setText(QString::number(f->getMaxX()));
            ui->externalForceMinY->setText(QString::number(f->getMinY()));
            ui->externalForceMaxY->setText(QString::number(f->getMaxY()));
            ui->externalForcePeriod->setText(QString::number(f->getPeriod()));
        }
    }
    ui->mcDensityRatio->setText(QString::number(painter->getGrid()->getConfig()->getDensity2()));
}

Parameters2::~Parameters2() {
    delete ui;
}

void Parameters2::on_updateInitial_clicked() {
    painter->getGrid()->pressure(ui->minX->text().toInt() - 1, ui->maxX->text().toInt() - 1,
                   ui->minY->text().toInt() - 1, ui->maxY->text().toInt() - 1,
                   ui->minZ->text().toInt() - 1, ui->maxZ->text().toInt() - 1,
                   ui->density1->text().toDouble(), ui->density2->text().toDouble());
}

void Parameters2::apply() {
    on_gravity_returnPressed();
    on_mpG_returnPressed();
    on_periodicBoundaries_clicked();
    on_mcQuantity_currentIndexChanged(0);
}

void Parameters2::updateModel() {
    switch (painter->getGrid()->getConfig()->getModel()) {
    case 9:
        ui->model->setCurrentIndex(0);
        break;
    case 15:
        ui->model->setCurrentIndex(1);
        break;
    case 19:
        ui->model->setCurrentIndex(2);
        break;
    case 27:
        ui->model->setCurrentIndex(3);
        break;
    }
}

void Parameters2::inject(Painter *painter) {
    this->painter = painter;
    on_sourceVelocity_returnPressed();
}

void Parameters2::on_kinematicViscosity_returnPressed() {
    if (ui->fluidOrder->currentIndex() == 0) {
        painter->getGrid()->getConfig()->setEpsilon(ui->fluidOrder->currentIndex(), ui->epsilon->text().toDouble());
        painter->getGrid()->getParameters()->config(ui->dx->text().toDouble(), ui->kinematicViscosity->text().toDouble(), ui->epsilon->text().toDouble());
        ui->dt->setText(QString::number(Parameters::dtValue(ui->dx->text().toDouble(), ui->kinematicViscosity->text().toDouble(), ui->epsilon->text().toDouble())));
        ui->sourceVelocity2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->sourceVelocity->text().toDouble())));
        ui->movingVelocity2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->movingVelocity->text().toDouble())));
        ui->movingVelocityY2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->movingVelocityY->text().toDouble())));
    } else {
        ui->epsilon->setText(QString::number(3 * painter->getGrid()->getParameters()->nondimentionalViscosity(ui->kinematicViscosity->text().toDouble()) + 0.5));
        painter->getGrid()->getConfig()->setEpsilon(ui->fluidOrder->currentIndex(), ui->epsilon->text().toDouble());
    }
}

void Parameters2::on_material_currentIndexChanged(QString text) {
    ui->kinematicViscosity->setReadOnly(ui->material->currentText() != "Other");
    if (text == "Other") {
        ui->kinematicViscosity->setText("1e-6");
    } else if (text == "Water 20C") {
        ui->kinematicViscosity->setText("1e-6");
    } else if (text == "Air 25C") {
        ui->kinematicViscosity->setText("15.7e-6");
    } else if (text == "Kerosene 20C") {
        ui->kinematicViscosity->setText("2.7e-6");
    }
    on_kinematicViscosity_returnPressed();
    material[ui->fluidOrder->currentIndex()] = ui->material->currentIndex();
}

void Parameters2::on_dx_returnPressed() {
    on_kinematicViscosity_returnPressed();
}

void Parameters2::on_dm_returnPressed() {
    painter->getGrid()->getParameters()->setDM(ui->dm->text().toDouble());
}

void Parameters2::on_epsilon_returnPressed() {
    on_kinematicViscosity_returnPressed();
}

void Parameters2::on_minP0_returnPressed() {
    painter->getGrid()->getConfig()->setMinP0(ui->minP0->text().toDouble());
    painter->getGrid()->randomP();
}

void Parameters2::on_maxP0_returnPressed() {
    painter->getGrid()->getConfig()->setMaxP0(ui->maxP0->text().toDouble());
    painter->getGrid()->randomP();
}

void Parameters2::on_depositionRate_returnPressed() {
    painter->getGrid()->getConfig()->setDepositionRate(ui->depositionRate->text().toDouble());
}

void Parameters2::on_thermalExpansion_returnPressed() {
    painter->getGrid()->getConfig()->setThermalExpansion(ui->thermalExpansion->text().toDouble());
}

void Parameters2::on_slipCoefficient_returnPressed() {
    painter->getGrid()->getConfig()->setSlipWallCoefficient(ui->slipCoefficient->text().toDouble());
}

void Parameters2::on_thermalGravity_returnPressed() {
    painter->getGrid()->getConfig()->setPassiveScalarGravity(ui->thermalGravity->text().toDouble());
}

void Parameters2::on_thermal1_returnPressed() {
    painter->getGrid()->getConfig()->setThermal(ui->thermalIndex->currentIndex(), ui->thermal1->text().toDouble());
}

void Parameters2::on_movingVelocity_returnPressed() {
    painter->getGrid()->getConfig()->setWallVelocity(MyVector3D(ui->movingVelocity->text().toDouble(), ui->movingVelocityY->text().toDouble(), 0));
    ui->movingVelocity2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->movingVelocity->text().toDouble())));
}

void Parameters2::on_movingVelocityY_returnPressed() {
    painter->getGrid()->getConfig()->setWallVelocity(MyVector3D(ui->movingVelocity->text().toDouble(), ui->movingVelocityY->text().toDouble(), 0));
    ui->movingVelocityY2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->movingVelocityY->text().toDouble())));
}

void Parameters2::on_outletStrategy_currentIndexChanged(int index) {
    int strategy = -1;
    switch (index) {
    case 1:
        strategy = SOURCE_EQUILIBRIUM;
        break;
    case 2:
        strategy = SOURCE_FIXED;
        break;
    case 3:
        strategy = SOURCE_ZOUHE;
        break;
    }
    painter->getGrid()->getConfig()->setSource(strategy);
    strategy = -1;
    switch (index) {
    case 1:
        strategy = OPEN_EQUILIBRIUM;
        break;
    case 2:
        strategy = OPEN_EXTRAPOLATION;
        break;
    case 3:
        strategy = OPEN_ZOUHE;
        break;
    }
    painter->getGrid()->getConfig()->setOpen(strategy);
}

void Parameters2::on_sourceVelocity_returnPressed() {
    painter->getGrid()->getConfig()->setSourceVelocity(ui->inletOrder->currentIndex(), ui->sourceVelocity->text().toDouble());
    ui->sourceVelocity2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->sourceVelocity->text().toDouble())));
}

void Parameters2::on_velocityOrder_returnPressed() {
    painter->getGrid()->getConfig()->setVelocityOrder(ui->velocityOrder->text().toInt());
}

void Parameters2::on_smagorinsky_returnPressed() {
    painter->getGrid()->getConfig()->setSmagorinsky(ui->smagorinsky->text().toDouble());
}

void Parameters2::on_shallowNB_returnPressed() {
    painter->getGrid()->getConfig()->setShallowNB(ui->shallowNB->text().toDouble());
}

void Parameters2::on_entropic_clicked() {
    painter->getGrid()->getConfig()->setEntropic(ui->entropic->isChecked());
}

void Parameters2::on_shallowG_returnPressed() {
    painter->getGrid()->getConfig()->setShallowG(ui->shallowG->text().toDouble());
}

void Parameters2::on_porousSolidDensity_returnPressed() {
    painter->getGrid()->getConfig()->setSolidDensity(ui->porousSolidDensity->text().toDouble(), ui->porousVariation->text().toDouble());
}

void Parameters2::on_periodicBoundaries_clicked() {
    painter->getGrid()->getConfig()->setPeriodicBoundaries(ui->periodicBoundaries->isChecked());
}

void Parameters2::on_mpG_returnPressed() {
    painter->getGrid()->getConfig()->setMultiphase(ui->mpG->text().toDouble(), ui->mpPsi0->text().toDouble(), ui->mpP0->text().toDouble(), ui->mpGads->text().toDouble());
}

void Parameters2::on_mpPsi0_returnPressed() {
    on_mpG_returnPressed();
}

void Parameters2::on_mpP0_returnPressed() {
    on_mpG_returnPressed();
}

void Parameters2::on_mpGads_returnPressed() {
    on_mpG_returnPressed();
}

void Parameters2::on_gravityEnable_clicked() {
    if (ui->gravityEnable->isChecked()) {
        ui->gravity->setEnabled(true);
        ui->gravity->setText("0.01");
    } else {
        ui->gravity->setEnabled(false);
        ui->gravity->setText("0");
    }
}

void Parameters2::on_model_currentIndexChanged(const QString selected) {
    int model = 9;
    if (selected == "D3Q15") {
        model = 15;
    } else if (selected == "D3Q19") {
        model = 19;
    } else if (selected == "D3Q27") {
        model = 27;
    }
    painter->getGrid()->getConfig()->setModel(model);
}

void Parameters2::on_gravity_returnPressed() {
    painter->getGrid()->getConfig()->setGravity(ui->gravity->text().toDouble());
}

void Parameters2::on_mcQuantity_currentIndexChanged(int) {
    if (!avoid_on_mcQuantity_currentIndexChanged) {
        painter->getGrid()->getConfig()->setMulticomponent(ui->mcQuantity->currentIndex() + 1, ui->mcG->text().toDouble(), ui->mcGads->text().toDouble(), ui->mcGads2->text().toDouble(), ui->mcType->currentIndex() + 1);
        painter->updateCellsList();
    }
    avoid_on_mcQuantity_currentIndexChanged = false;
}

void Parameters2::on_mcG_returnPressed() {
    on_mcQuantity_currentIndexChanged(0);
}

void Parameters2::on_mcGads_returnPressed() {
    on_mcQuantity_currentIndexChanged(0);
}

void Parameters2::on_mcGads2_returnPressed() {
    on_mcQuantity_currentIndexChanged(0);
}

void Parameters2::on_mcType_currentIndexChanged(int) {
    if (!avoid_on_mcType_currentIndexChanged) {
        painter->getGrid()->getConfig()->setMulticomponent(ui->mcQuantity->currentIndex() + 1, ui->mcG->text().toDouble(), ui->mcGads->text().toDouble(), ui->mcGads2->text().toDouble(), ui->mcType->currentIndex() + 1);
        painter->updateCellsList();
    }
    avoid_on_mcType_currentIndexChanged = false;
}

Ui::Parameters2* Parameters2::getUI() {
    return ui;
}

void Parameters2::on_sourceVelocity2_returnPressed() {
    ui->sourceVelocity->setText(QString::number(painter->getGrid()->getParameters()->nondimentionalVelocity(ui->sourceVelocity2->text().toDouble())));
    on_sourceVelocity_returnPressed();
}

void Parameters2::on_inletOrder_currentIndexChanged(int index) {
    ui->sourceVelocity->setText(QString::number(painter->getGrid()->getConfig()->getSourceVelocity(index)));
    ui->sourceVelocity2->setText(QString::number(painter->getGrid()->getParameters()->dimentionalVelocity(ui->sourceVelocity->text().toDouble())));
}

void Parameters2::on_thermalIndex_currentIndexChanged(int index) {
    ui->thermal1->setText(QString::number(painter->getGrid()->getConfig()->getThermal(index)));
}

void Parameters2::on_movingVelocity2_returnPressed() {
    ui->movingVelocity->setText(QString::number(painter->getGrid()->getParameters()->nondimentionalVelocity(ui->movingVelocity2->text().toDouble())));
    on_movingVelocity_returnPressed();
}

void Parameters2::on_movingVelocityY2_returnPressed() {
    ui->movingVelocityY->setText(QString::number(painter->getGrid()->getParameters()->nondimentionalVelocity(ui->movingVelocityY2->text().toDouble())));
    on_movingVelocityY_returnPressed();
}

void Parameters2::on_fluidOrder_currentIndexChanged(int index) {
    ui->epsilon->setText(QString::number(painter->getGrid()->getConfig()->getEpsilons()[index]));
    ui->epsilon->setEnabled(index == 0);
    ui->material->setCurrentIndex(material[index]);
    ui->kinematicViscosity->setText(QString::number(painter->getGrid()->getParameters()->dimentionalViscosity((painter->getGrid()->getConfig()->getEpsilons()[index] - 0.5) / 3)));
}

void Parameters2::on_anodeCathodeFactor_returnPressed() {
    painter->getGrid()->getConfig()->setReflectWithFactor(ui->anodeCathodeFactor->text().toDouble());
}

void Parameters2::on_externalForceStrengthX_returnPressed() {
    painter->getGrid()->getConfig()->addForce(new PassiveScalarForce(
                                                  ui->externalForceStrengthX->text().toDouble(),
                                                  ui->externalForceStrengthY->text().toDouble(),
                                                  ui->externalForceMinX->text().toInt(),
                                                  ui->externalForceMaxX->text().toInt(),
                                                  ui->externalForceMinY->text().toInt(),
                                                  ui->externalForceMaxY->text().toInt(),
                                                  ui->externalForcePeriod->text().toInt()));
}

void Parameters2::on_externalForceStrengthY_returnPressed() {
    on_externalForceStrengthX_returnPressed();
}

void Parameters2::on_externalForceMinX_returnPressed() {
    on_externalForceStrengthX_returnPressed();
}

void Parameters2::on_externalForceMaxX_returnPressed() {
    on_externalForceStrengthX_returnPressed();
}

void Parameters2::on_externalForceMinY_returnPressed() {
    on_externalForceStrengthX_returnPressed();
}

void Parameters2::on_externalForceMaxY_returnPressed() {
    on_externalForceStrengthX_returnPressed();
}

void Parameters2::on_externalForcePeriod_returnPressed() {
    on_externalForceStrengthX_returnPressed();
}

void Parameters2::on_sourcePressure_returnPressed() {
    painter->getGrid()->getConfig()->setSourcePressure(ui->inletOrder->currentIndex(), ui->sourcePressure->text().toDouble());
}

void Parameters2::on_nanoFraction_returnPressed() {
    painter->getGrid()->getConfig()->setNanoValues(ui->nanoFraction->text().toDouble(), ui->nanoDensity->text().toDouble(), ui->nanoThermalExpansion->text().toDouble(), ui->nanoThermalConductivity->text().toDouble(), ui->nanoHeatCapacitance->text().toDouble());
}

void Parameters2::on_nanoDensity_returnPressed() {
    on_nanoFraction_returnPressed();
}

void Parameters2::on_nanoThermalExpansion_returnPressed() {
    on_nanoFraction_returnPressed();
}

void Parameters2::on_nanoThermalConductivity_returnPressed() {
    on_nanoFraction_returnPressed();
}

void Parameters2::on_nanoHeatCapacitance_returnPressed() {
    on_nanoFraction_returnPressed();
}

void Parameters2::on_nanoparticles_currentIndexChanged(const QString &arg1) {
    if (arg1 == "None") {
        ui->nanoDensity->setText("1");
        ui->nanoHeatCapacitance->setText("1");
        ui->nanoThermalConductivity->setText("1");
        ui->nanoThermalExpansion->setText("1");
    } else if (arg1 == "Ag") {
        ui->nanoDensity->setText("10.53");
        ui->nanoHeatCapacitance->setText("0.0562");
        ui->nanoThermalConductivity->setText("699.83");
        ui->nanoThermalExpansion->setText("9");
    } else if (arg1 == "Al2O3") {
        ui->nanoDensity->setText("3.98");
        ui->nanoHeatCapacitance->setText("0.183");
        ui->nanoThermalConductivity->setText("40.783");
        ui->nanoThermalExpansion->setText("4.0476");
    } else if (arg1 == "Cu") {
        ui->nanoDensity->setText("8.96");
        ui->nanoHeatCapacitance->setText("0.09");
        ui->nanoThermalConductivity->setText("654.16");
        ui->nanoThermalExpansion->setText("5.557");
    } else if (arg1 == "TiO2") {
        ui->nanoDensity->setText("4.262");
        ui->nanoHeatCapacitance->setText("0.1642");
        ui->nanoThermalConductivity->setText("14.6");
        ui->nanoThermalExpansion->setText("4.2857");
    }
    on_nanoFraction_returnPressed();
}

void Parameters2::on_porousVariation_returnPressed() {
    on_porousSolidDensity_returnPressed();
}

void Parameters2::on_updateConstant_clicked() {
    for (int x = ui->minX->text().toInt(); x <= ui->maxX->text().toInt(); x++) {
        for (int y = ui->minY->text().toInt(); y <= ui->maxY->text().toInt(); y++) {
            for (int z = ui->minZ->text().toInt(); z <= ui->maxZ->text().toInt(); z++) {
                MCCell *mc = dynamic_cast<MCCell*>(painter->getGrid()->getGrid(y - 1, x - 1, z - 1));
                if (mc != 0) {
                    mc->setConstantConcentration(0, ui->density1->text().toDouble());
                    mc->setConstantConcentration(1, ui->density2->text().toDouble());
                }
                SCCell *sc = dynamic_cast<SCCell*>(painter->getGrid()->getGrid(y - 1, x - 1, z - 1));
                if (sc != 0) {
                    sc->setConstantConcentration(ui->density1->text().toDouble());
                }
            }
        }
    }
}

void Parameters2::on_regionAll_clicked() {
    ui->minX->setText("1");
    ui->maxX->setText(QString::number(painter->getGrid()->getConfig()->getWidth()));
    ui->minY->setText("1");
    ui->maxY->setText(QString::number(painter->getGrid()->getConfig()->getHeight()));
    ui->minZ->setText("1");
    ui->maxZ->setText(QString::number(painter->getGrid()->getConfig()->getLength()));
    ui->density1->setFocus();
}

void Parameters2::on_regionCenter_clicked() {
    ui->minX->setText(QString::number(painter->getGrid()->getConfig()->getWidth() / 2 + (painter->getGrid()->getConfig()->getWidth() % 2 == 1 ? 1 : 0)));
    ui->maxX->setText(QString::number(painter->getGrid()->getConfig()->getWidth() / 2 + 1));
    ui->minY->setText(QString::number(painter->getGrid()->getConfig()->getHeight() / 2 + (painter->getGrid()->getConfig()->getHeight() % 2 == 1 ? 1 : 0)));
    ui->maxY->setText(QString::number(painter->getGrid()->getConfig()->getHeight() / 2 + 1));
    ui->minZ->setText(QString::number(painter->getGrid()->getConfig()->getLength() / 2 + (painter->getGrid()->getConfig()->getLength() % 2 == 1 ? 1 : 0)));
    ui->maxZ->setText(QString::number(painter->getGrid()->getConfig()->getLength() / 2 + 1));
    ui->density1->setFocus();
}

void Parameters2::on_loadMaskPorousMedia_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Mask", "", "*.png");
    if (fileName != NULL && !fileName.isEmpty()) {
        QImage image;
        image.load(fileName);
        for (int x = 0; x < painter->getGrid()->getConfig()->getWidth(); x++) {
            for (int y = 0; y < painter->getGrid()->getConfig()->getHeight(); y++) {
                PorousCell *porous = dynamic_cast<PorousCell*>(painter->getGrid()->getGrid(y, x, 0));
                if (porous != 0) {
                    QColor color(image.pixel(x, painter->getGrid()->getConfig()->getHeight() - 1 - y));
                    if (ui->colorComponentPorous->currentText() == "RGB - R") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.redF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "RGB - G") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.greenF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "RGB - B") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.blueF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "HSV - H") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.hueF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "HSV - S") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.saturationF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "HSV - V") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.valueF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "CMYK - C") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.cyanF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "CMYK - M") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.magentaF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "CMYK - Y") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.yellowF() - 0.5) * ui->variationPorous->text().toDouble());
                    } else if (ui->colorComponentPorous->currentText() == "CMYK - K") {
                        porous->setSolidDensity(ui->averageValuePorous->text().toDouble() + (color.blackF() - 0.5) * ui->variationPorous->text().toDouble());
                    }
                }
            }
        }
    }
}

void Parameters2::on_activateKorner_clicked() {
    painter->getGrid()->getConfig()->setKorner(ui->activateKorner->isChecked());
}

void Parameters2::on_ebmPower_returnPressed() {
    painter->getGrid()->getKorner()->configure(ui->ebmPower->text().toDouble(), ui->ebmStdDev->text().toDouble(), ui->ebmStartX->text().toInt(), ui->ebmVelocity->text().toDouble(), ui->ebmTimesteps->text().toInt(), ui->ebmAbsorptionCoefficient->text().toDouble());
}

void Parameters2::on_ebmStdDev_returnPressed() {
    on_ebmPower_returnPressed();
}

void Parameters2::on_ebmStartX_returnPressed() {
    on_ebmPower_returnPressed();
}

void Parameters2::on_ebmVelocity_returnPressed() {
    on_ebmPower_returnPressed();
}

void Parameters2::on_ebmTimesteps_returnPressed() {
    on_ebmPower_returnPressed();
}

void Parameters2::on_kornerSolidEnergy_returnPressed() {
    painter->getGrid()->getKorner()->configureEnergy(ui->kornerSolidEnergy->text().toDouble(), ui->kornerLiquidEnergy->text().toDouble());
}

void Parameters2::on_kornerLiquidEnergy_returnPressed() {
    on_kornerSolidEnergy_returnPressed();
}

void Parameters2::on_mcDensityRatio_returnPressed() {
    //painter->getGrid()->getConfig()->setMCDensityRatio(ui->mcDensityRatio->text().toDouble());
    painter->getGrid()->getConfig()->setDensity1(1);
    painter->getGrid()->getConfig()->setDensity2(ui->mcDensityRatio->text().toDouble());
}
