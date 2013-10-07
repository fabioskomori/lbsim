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

#include "regression.h"
#include "ui_regression.h"
#include "parameter/parameterpool.h"
#include "parameter/parameter.h"
#include <list>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "lbmainwindow.h"
#include "lbwidget.h"
#include "painter/painter.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/gridsimulation.h"
#include "regression/simplecontroller.h"
#include "regression/parametercontrol.h"
#include "regression/datapoint.h"
#include "chart.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"

Regression::Regression(LBMainWindow *mainWindow, QWidget *parent) : QDialog(parent), ui(new Ui::Regression), mainWindow(mainWindow) {
    ui->setupUi(this);
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    std::list<Parameter*>* parameters = ParameterPool::list();
    for (std::list<Parameter*>::iterator parameter = parameters->begin(); parameter != parameters->end(); parameter++) {
        ui->parameter->addItem((*parameter)->name());
    }
    strategy = 0;
    errorsCurve = new QwtPlotCurve();
    valuesCurve = new QwtPlotCurve();
    errorsCurve->attach(ui->errorChart);
    valuesCurve->attach(ui->valueChart);
    new QwtPlotPanner(ui->errorChart->canvas());
    new QwtPlotPanner(ui->valueChart->canvas());
    new QwtPlotMagnifier(ui->errorChart->canvas());
    new QwtPlotMagnifier(ui->valueChart->canvas());
}

Regression::~Regression() {
    delete ui;
    if (strategy != 0) {
        delete strategy;
    }
}

void Regression::callback(int) {
    errors.push_back(XYPoint(mainWindow->getLB()->getPainter()->getGrid()->getSimulation()->getIterations(), strategy->getNewError()));
    values.push_back(XYPoint(mainWindow->getLB()->getPainter()->getGrid()->getSimulation()->getIterations(), strategy->getNewValue()));
    Chart::updateChart(errors, errorsCurve);
    Chart::updateChart(values, valuesCurve);
    ui->errorChart->replot();
    ui->valueChart->replot();
}

void Regression::save(QXmlStreamWriter &writer) {
    writer.writeStartElement("regression");
    writer.writeAttribute("parameter", ui->parameter->currentText());
    writer.writeAttribute("initialValue", ui->initialValue->text());
    writer.writeAttribute("delta", ui->delta->text());
    writer.writeAttribute("simplePeriod", ui->simplePeriod->text());
    writer.writeAttribute("simpleFirstVariation", ui->simpleFirstVariation->text());
    writer.writeAttribute("simpleP", ui->simpleP->text());
    writer.writeAttribute("pointsSize", QString::number(ui->points->rowCount()));
    for (int i = 0; i < ui->points->rowCount(); i++) {
        writer.writeStartElement("point");
        writer.writeAttribute("x", ui->points->item(i, 0) != 0 ? ui->points->item(i, 0)->text() : "");
        writer.writeAttribute("y", ui->points->item(i, 1) != 0 ? ui->points->item(i, 1)->text() : "");
        writer.writeAttribute("z", ui->points->item(i, 2) != 0 ? ui->points->item(i, 2)->text() : "");
        writer.writeAttribute("density", ui->points->item(i, 3) != 0 ? ui->points->item(i, 3)->text() : "");
        writer.writeAttribute("velocityX", ui->points->item(i, 4) != 0 ? ui->points->item(i, 4)->text() : "");
        writer.writeAttribute("velocityY", ui->points->item(i, 5) != 0 ? ui->points->item(i, 5)->text() : "");
        writer.writeAttribute("velocityZ", ui->points->item(i, 6) != 0 ? ui->points->item(i, 6)->text() : "");
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

void Regression::load(QXmlStreamReader &reader) {
    if (reader.name().toString() == "regression") {
        for (int i = 0; i < ui->parameter->count(); i++) {
            if (ui->parameter->itemText(i) == reader.attributes().value("parameter").toString()) {
                ui->parameter->setCurrentIndex(i);
                break;
            }
        }
        ui->simplePeriod->setText(reader.attributes().value("simplePeriod").toString());
        ui->simpleFirstVariation->setText(reader.attributes().value("simpleFirstVariation").toString());
        ui->simpleP->setText(reader.attributes().value("simpleP").toString());
        ui->initialValue->setText(reader.attributes().value("initialValue").toString());
        ui->delta->setText(reader.attributes().value("delta").toString());
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
            ui->points->setItem(i, 4, new QTableWidgetItem(reader.attributes().value("velocityX").toString()));
            ui->points->setItem(i, 5, new QTableWidgetItem(reader.attributes().value("velocityY").toString()));
            ui->points->setItem(i, 6, new QTableWidgetItem(reader.attributes().value("velocityZ").toString()));
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void Regression::on_addLine_clicked() {
    ui->points->insertRow(ui->points->rowCount());
}

void Regression::on_removeLine_clicked() {
    if (ui->points->selectedItems().size() > 0) {
        ui->points->removeRow(ui->points->selectedItems().at(0)->row());
    }
}

void Regression::on_start_clicked() {
    ui->start->setEnabled(false);
    ui->points->setEnabled(false);
    ui->addLine->setEnabled(false);
    ui->removeLine->setEnabled(false);
    ui->parameters->setEnabled(false);
    ui->strategy->setEnabled(false);
    ui->stop->setEnabled(true);
    if (strategy != 0) {
        delete strategy;
    }
    strategy = new SimpleController(ui->simplePeriod->text().toInt(), ui->simpleFirstVariation->text().toDouble(), ui->simpleP->text().toDouble());
    strategy->addParameter(new ParameterControl(ParameterPool::parameterByName(ui->parameter->currentText()), ui->initialValue->text().toDouble(), ui->initialValue->text().toDouble() + ui->delta->text().toDouble(), ui->initialValue->text().toDouble() - ui->delta->text().toDouble()));
    for (int i = 0; i < ui->points->rowCount(); i++) {
        strategy->addDataPoint(DataPoint(
                ui->points->item(i, 0) != 0 ? ui->points->item(i, 0)->text().toInt() - 1 : 0,
                ui->points->item(i, 1) != 0 ? ui->points->item(i, 1)->text().toInt() - 1 : 0,
                ui->points->item(i, 2) != 0 ? ui->points->item(i, 2)->text().toInt() - 1 : 0,
                ui->points->item(i, 3) != 0 ? ui->points->item(i, 3)->text().toDouble() : 0,
                ui->points->item(i, 3) != 0 && !ui->points->item(i, 3)->text().isEmpty() ? 1 : 0,
                ui->points->item(i, 4) != 0 ? ui->points->item(i, 4)->text().toDouble() : 0,
                ui->points->item(i, 5) != 0 ? ui->points->item(i, 5)->text().toDouble() : 0,
                ui->points->item(i, 6) != 0 ? ui->points->item(i, 6)->text().toDouble() : 0,
                ui->points->item(i, 4) != 0 && !ui->points->item(i, 4)->text().isEmpty() ? 1 : 0,
                ui->points->item(i, 5) != 0 && !ui->points->item(i, 5)->text().isEmpty() ? 1 : 0,
                ui->points->item(i, 6) != 0 && !ui->points->item(i, 6)->text().isEmpty() ? 1 : 0));
    }
    strategy->injectMainWindow(mainWindow);
    strategy->setListener(this);
    mainWindow->getLB()->getPainter()->getGrid()->addListener(2, strategy);
    QMetaObject::invokeMethod(mainWindow, "on_actionStart_triggered");
}

void Regression::on_stop_clicked() {
    ui->stop->setEnabled(false);
    ui->points->setEnabled(true);
    ui->addLine->setEnabled(true);
    ui->removeLine->setEnabled(true);
    ui->parameters->setEnabled(true);
    ui->strategy->setEnabled(true);
    ui->start->setEnabled(true);
    mainWindow->getLB()->getPainter()->getGrid()->removeListener(2);
    QMetaObject::invokeMethod(mainWindow, "on_actionStop_triggered");
}
