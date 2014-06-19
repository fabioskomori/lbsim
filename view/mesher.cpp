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

#include "mesher.h"
#include "ui_mesher.h"
#include "lbwidget.h"
#include "painter/painter.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "parameters2.h"
#include "ui_parameters2.h"

Mesher::Mesher(QWidget *parent) : QDialog(parent), ui(new Ui::Mesher) {
    ui->setupUi(this);
}

Mesher::~Mesher() {
    delete ui;
}

void Mesher::injectWidget(LBWidget *widget) {
    this->widget = widget;
}

void Mesher::injectParameters(Parameters2 *parameters) {
    this->parameters = parameters;
}

void Mesher::on_changeCells_clicked() {
    QString type = " ";
    if (ui->cellType->currentText() == "wall") {
        type = "w";
    }
    widget->getPainter()->getGrid()->alter(type, ui->minx->text().toInt() - 1, ui->maxx->text().toInt() - 1,
                                           ui->miny->text().toInt() - 1, ui->maxy->text().toInt() - 1,
                                           ui->minz->text().toInt() - 1, ui->maxz->text().toInt() - 1, true);
    widget->getPainter()->getGrid()->processNeighbors();
    widget->getPainter()->updateDisplayList(false);
    widget->getPainter()->updateCellsList();
    widget->updateGL();
}

void Mesher::on_resizeGeometry_clicked() {
    parameters->getUI()->fluidOrder->setCurrentIndex(0);
    parameters->on_fluidOrder_currentIndexChanged(0);
    parameters->getUI()->epsilon->setText(QString::number(0.5 + (ui->resizeFactor->currentIndex() + 2) * (widget->getPainter()->getGrid()->getConfig()->getEpsilons()[0] - 0.5)));
    parameters->on_epsilon_returnPressed();
    widget->getPainter()->getGrid()->resize(ui->resizeFactor->currentIndex() + 2);
    widget->getPainter()->updateDisplayList(false);
    widget->getPainter()->updateCellsList();
    widget->updateGL();
}

void Mesher::on_optimize_clicked() {
    widget->getPainter()->getGrid()->optimize();
}

void Mesher::on_porous_clicked() {
    widget->getPainter()->getGrid()->porous(ui->porousPercent->text().toInt());
    widget->getPainter()->getGrid()->processNeighbors();
    widget->getPainter()->updateDisplayList(false);
    widget->getPainter()->updateCellsList();
    widget->updateGL();
}

void Mesher::on_newGrid_clicked() {
    widget->getPainter()->getGrid()->init(ui->maxx->text().toInt(), ui->maxy->text().toInt(), ui->maxz->text().toInt());
    widget->getPainter()->getGrid()->processNeighbors();
    widget->getPainter()->updateDisplayList(false);
    widget->getPainter()->updateCellsList();
    widget->updateGL();
}
