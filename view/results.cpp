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

#include "results.h"
#include "ui_results.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "../model/inputoutput/matlabreporter.h"
#include "../model/inputoutput/iterationreporter.h"
#include "painter/painter.h"
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

Results::Results(Painter *painter, QWidget *parent) : QDialog(parent), ui(new Ui::Results) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    this->painter = painter;
    ui->stopCriterion->setAllowDecimal(true);
    ui->stopCriterion->setAllowDecimal(true);
}

Results::~Results() {
    delete ui;
}

bool Results::getSaveImage() {
    return ui->images->isChecked();
}

bool Results::getReportVelocities() {
    return ui->velocities->isChecked();
}

int Results::getSaveInterval() {
    return ui->interval->text().toInt();
}

bool Results::getReportSimulation() {
    return ui->reportSimulation->isChecked();
}

void Results::on_maxSteps_returnPressed() {
    painter->getGrid()->getConfig()->setMaxIterations(ui->maxSteps->text().toInt());
}

Ui::Results* Results::getUI() {
    return ui;
}

/*void Results::on_matlabStreamlines_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "MATLAB streamlines", "", "*.m");
    if (fileName != NULL && !fileName.isEmpty()) {
        MATLABReporter reporter;
        reporter.streamlines(painter->getGrid(), fileName, ui->matlabSpacing->text());
    }
}*/

void Results::on_stopCriterion_returnPressed() {
    painter->getGrid()->getConfig()->setStopCriterion(ui->stopCriterion->text().toDouble());
}

/*void Results::on_csvGenerate_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "CSV report", "", "*.csv");
    if (fileName != NULL && !fileName.isEmpty()) {
        IterationReporter *reporter = new IterationReporter(ui->csvX->text().toInt() - 1, ui->csvY->text().toInt() - 1, ui->csvZ->text().toInt() - 1);
        reporter->reportCsv(painter->getGrid(), fileName);
    }
}*/

/*void Results::on_simX_returnPressed() {
    painter->getGrid()->getConfig()->setPointToReport(Vector3i(ui->simX->text().toInt(), ui->simY->text().toInt(), ui->simZ->text().toInt()));
}

void Results::on_simY_returnPressed() {
    on_simX_returnPressed();
}

void Results::on_simZ_returnPressed() {
    on_simX_returnPressed();
}*/

void Results::save(QXmlStreamWriter &writer) {
    writer.writeStartElement("results");
    writer.writeAttribute("maxIterations", ui->maxSteps->text());
    writer.writeAttribute("stopWhen", ui->stopCriterion->text());
    writer.writeAttribute("interval", ui->interval->text());
    writer.writeAttribute("images", ui->images->isChecked() ? "1" : "0");
    writer.writeAttribute("vp", ui->velocities->isChecked() ? "1" : "0");
    writer.writeAttribute("simulation", ui->reportSimulation->isChecked() ? "1" : "0");
    writer.writeAttribute("fixedX", ui->simX->text());
    writer.writeAttribute("fixedY", ui->simY->text());
    writer.writeAttribute("fixedZ", ui->simZ->text());
    writer.writeEndElement();
}

void Results::load(QXmlStreamReader &reader) {
    if (reader.name().toString() == "results") {
        ui->maxSteps->setText(reader.attributes().value("maxIterations").toString());
        on_maxSteps_returnPressed();
        ui->stopCriterion->setText(reader.attributes().value("stopWhen").toString());
        on_stopCriterion_returnPressed();
        ui->interval->setText(reader.attributes().value("interval").toString());
        ui->images->setChecked(reader.attributes().value("images").toString() == "1");
        ui->velocities->setChecked(reader.attributes().value("vp").toString() == "1");
        ui->reportSimulation->setChecked(reader.attributes().value("simulation").toString() == "1");
        ui->simX->setText("0");
        ui->simY->setText("0");
        ui->simZ->setText("0");
        if (reader.attributes().hasAttribute("fixedX")) {
            ui->simX->setText(reader.attributes().value("fixedX").toString());
            ui->simY->setText(reader.attributes().value("fixedY").toString());
            ui->simZ->setText(reader.attributes().value("fixedZ").toString());
        }
        //on_simX_returnPressed();
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void Results::on_addPoint_clicked() {
    ui->points->insertRow(ui->points->rowCount());
    ui->points->setItem(ui->points->rowCount() - 1, 0, new QTableWidgetItem(ui->simX->text()));
    ui->points->setItem(ui->points->rowCount() - 1, 1, new QTableWidgetItem(ui->simY->text()));
    ui->points->setItem(ui->points->rowCount() - 1, 2, new QTableWidgetItem(ui->simZ->text()));
}

void Results::on_removePoint_clicked() {
    ui->points->removeRow(ui->points->currentRow());
}

void Results::on_updatePoints_clicked() {
    painter->getGrid()->getConfig()->clearPointsToReport();
    for (int i = 0; i < ui->points->rowCount(); i++) {
        painter->getGrid()->getConfig()->addPointToReport(Vector3i(ui->points->item(i, 0)->text().toInt(), ui->points->item(i, 1)->text().toInt(), ui->points->item(i, 2)->text().toInt()));
    }
}
