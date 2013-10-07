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

#include "painterconfig.h"
#include "ui_painterconfig.h"
#include "../model/latticeboltzmann/streamline/streamlinegenerator.h"
#include "../model/latticeboltzmann/streamline/seedgenerator.h"
#include "../model/latticeboltzmann/streamline/euler.h"
#include "../model/latticeboltzmann/streamline/velocitylinearinterpolator.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "painter/painter.h"
#include "painter/isolineseeder.h"
#include "painter/equallyspacedseeder.h"
#include "painter/customseeder.h"
#include "lbwidget.h"
#include <QTableWidgetItem>

PainterConfig::PainterConfig(QWidget *parent) : QDialog(parent), ui(new Ui::PainterConfig) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
}

void PainterConfig::injectPainter(Painter *painter) {
    this->painter = painter;
    on_strategy_currentIndexChanged(0);
}

void PainterConfig::injectWidget(LBWidget *widget) {
    this->widget = widget;
}

PainterConfig::~PainterConfig() {
    delete ui;
}

void PainterConfig::on_strategy_currentIndexChanged(int index) {
    StreamLineGenerator *generator = 0;
    switch (index) {
    case 0:
        generator = new SeedGenerator(painter->getGrid());
        break;
    }
    painter->setStreamLineGenerator(generator);
    ui->properties->clearContents();
    std::map<QString, QString> properties = generator->getProperties();
    for (std::map<QString, QString>::iterator property = properties.begin(); property != properties.end(); property++) {
        ui->properties->insertRow(0);
        ui->properties->setItem(0, 0, new QTableWidgetItem((*property).first));
        ui->properties->setItem(0, 1, new QTableWidgetItem((*property).second));
    }
}

void PainterConfig::on_update_clicked() {
    for (int row = 0; row < ui->properties->rowCount(); row++) {
        painter->getStreamLineGenerator()->setProperty(ui->properties->item(row, 0)->text(), ui->properties->item(row, 1)->text());
    }
    widget->updateGL();
}

void PainterConfig::on_equallySpaced_clicked() {
    //ui->values->clear();
    ui->values->setEnabled(false);
    ui->randomIsolines->setEnabled(false);
    ui->sortIsolines->setEnabled(false);
    ui->addRow->setEnabled(false);
    ui->deleteRow->setEnabled(false);
    ui->count->setText("10");
    ui->count->setEnabled(true);
}

void PainterConfig::on_custom_clicked() {
    ui->count->setText("");
    ui->count->setEnabled(false);
    ui->values->setEnabled(true);
    ui->randomIsolines->setEnabled(true);
    ui->sortIsolines->setEnabled(true);
    ui->addRow->setEnabled(true);
    ui->deleteRow->setEnabled(true);
}

void PainterConfig::on_addRow_clicked() {
    ui->values->insertRow(ui->values->rowCount());
}

void PainterConfig::on_deleteRow_clicked() {
    ui->values->removeRow(ui->values->currentRow());
}

void PainterConfig::on_updateIsolines_clicked() {
    IsolineSeeder *newIsolineSeeder = 0;
    if (ui->equallySpaced->isChecked()) {
        newIsolineSeeder = new EquallySpacedSeeder(ui->count->text().toInt(), painter);
    } else if (ui->custom->isChecked()) {
        double *seeds = new double[ui->values->rowCount()];
        for (int i = 0; i < ui->values->rowCount(); i++) {
            seeds[i] = ui->values->item(i, 0)->text().toDouble();
        }
        newIsolineSeeder = new CustomSeeder(seeds, ui->values->rowCount());
    }
    painter->setIsolineSeeder(newIsolineSeeder);
    widget->updateGL();
}

void PainterConfig::on_streamIntegrationX_currentIndexChanged(int index) {
    painter->setStreamIntegrationX(index);
    widget->updateGL();
}

void PainterConfig::on_streamIntegrationY_currentIndexChanged(int index) {
    painter->setStreamIntegrationY(index);
    widget->updateGL();
}

int PainterConfig::getStreamIntegrationX() {
    return ui->streamIntegrationX->currentIndex();
}

int PainterConfig::getStreamIntegrationY() {
    return ui->streamIntegrationY->currentIndex();
}

void PainterConfig::on_sortIsolines_clicked() {
    std::list<double> values;
    for (int i = 0; i < ui->values->rowCount(); i++) {
        values.push_back(ui->values->item(i, 0)->text().toDouble());
    }
    values.sort();
    int row = 0;
    for (std::list<double>::iterator value = values.begin(); value != values.end(); value++) {
        ui->values->setItem(row, 0, new QTableWidgetItem(QString::number(*value)));
        row++;
    }
}

void PainterConfig::on_randomIsolines_clicked() {
    std::list<double> isos = painter->suggestIsolines(ui->randomQuantity->text().toInt());
    isos.sort();
    ui->values->setRowCount(isos.size());
    int row = 0;
    for (std::list<double>::iterator iso = isos.begin(); iso != isos.end(); iso++) {
        ui->values->setItem(row++, 0, new QTableWidgetItem(QString::number(*iso)));
    }
}
