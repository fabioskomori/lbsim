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

#include "interpolation.h"
#include "ui_interpolation.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/sccell.h"
#include "../model/latticeboltzmann/interpolation/interpolationcell.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "../model/math/vector3i.h"
#include "../model/util/shared.h"
#include "../model/latticeboltzmann/gridconfig.h"

Interpolation::Interpolation(Grid *grid, QWidget *parent) : QDialog(parent), ui(new Ui::Interpolation), grid(grid) {
    ui->setupUi(this);
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
}

Interpolation::~Interpolation() {
    delete ui;
}

void Interpolation::on_addLine_clicked() {
    ui->points->insertRow(0);
    ui->points->setItem(0, 0, new QTableWidgetItem(ui->x->text()));
    ui->points->setItem(0, 1, new QTableWidgetItem(ui->y->text()));
    ui->points->setItem(0, 2, new QTableWidgetItem(ui->z->text()));
    ui->points->setItem(0, 3, new QTableWidgetItem(ui->density->text()));
    ui->points->setItem(0, 4, new QTableWidgetItem(ui->variation->text()));
    ui->points->setItem(0, 5, new QTableWidgetItem(ui->period->text()));
    grid->setGrid(ui->y->text().toInt() - 1, ui->x->text().toInt() - 1, ui->z->text().toInt() - 1, new InterpolationCell(ui->density->text().toDouble(), ui->variation->text().toDouble(), ui->period->text().toInt()));
}

void Interpolation::save(QXmlStreamWriter &writer) {
    writer.writeStartElement("interpolation");
    writer.writeAttribute("pointsSize", QString::number(ui->points->rowCount()));
    for (int i = 0; i < ui->points->rowCount(); i++) {
        writer.writeStartElement("point");
        writer.writeAttribute("x", ui->points->item(i, 0) != 0 ? ui->points->item(i, 0)->text() : "");
        writer.writeAttribute("y", ui->points->item(i, 1) != 0 ? ui->points->item(i, 1)->text() : "");
        writer.writeAttribute("z", ui->points->item(i, 2) != 0 ? ui->points->item(i, 2)->text() : "");
        writer.writeAttribute("density", ui->points->item(i, 3) != 0 ? ui->points->item(i, 3)->text() : "");
        writer.writeAttribute("variation", ui->points->item(i, 4) != 0 ? ui->points->item(i, 4)->text() : "");
        writer.writeAttribute("period", ui->points->item(i, 5) != 0 ? ui->points->item(i, 5)->text() : "");
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

void Interpolation::load(QXmlStreamReader &reader) {
    if (reader.name().toString() == "interpolation") {
        int count = reader.attributes().value("pointsSize").toString().toInt();
        while (ui->points->rowCount() > 0) {
            ui->points->removeRow(0);
        }
        for (int i = 0; i < count; i++) {
            do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
            ui->points->insertRow(i);
            ui->points->setItem(i, 0, new QTableWidgetItem(reader.attributes().value("x").toString()));
            ui->points->setItem(i, 1, new QTableWidgetItem(reader.attributes().value("y").toString()));
            ui->points->setItem(i, 2, new QTableWidgetItem(reader.attributes().value("z").toString()));
            ui->points->setItem(i, 3, new QTableWidgetItem(reader.attributes().value("density").toString()));
            ui->points->setItem(i, 4, new QTableWidgetItem(reader.attributes().value("variation").toString()));
            ui->points->setItem(i, 5, new QTableWidgetItem(reader.attributes().value("period").toString()));
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void Interpolation::on_removeLine_clicked() {
    if (ui->points->selectedItems().size() > 0) {
        grid->processNeighbors();
        Vector3i point = Vector3i(ui->points->item(ui->points->selectedItems().at(0)->row(), 1)->text().toInt() - 1,
                                  ui->points->item(ui->points->selectedItems().at(0)->row(), 0)->text().toInt() - 1,
                                  ui->points->item(ui->points->selectedItems().at(0)->row(), 2)->text().toInt() - 1);
        BaseCell **neighbors = grid->getNeighbors(point.getX(), point.getY(), point.getZ());
        double p = 0;
        int model = Shared::instance()->getGridConfig()->getModel();
        for (int i = 0; i < model; i++) {
            if (neighbors[i] != 0) {
                p += neighbors[i]->getP(0);
            }
        }
        grid->setGrid(point.getX(), point.getY(), point.getZ(), new SCCell(p / model));
        ui->points->removeRow(ui->points->selectedItems().at(0)->row());
    }
}
