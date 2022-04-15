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

#include "chart.h"
#include "ui_chart.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "../model/latticeboltzmann/gridsimulation.h"
#include "../model/latticeboltzmann/moving/particlemanager.h"
#include "../model/latticeboltzmann/physical/parameters.h"
#include "../model/latticeboltzmann/basecell.h"
#include <vector>
#include <QResizeEvent>
#include "qwt_point_data.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_marker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_zoomer.h"
#include "qwt_scale_div.h"
#include "qwt_symbol.h"
#include "qwt_text.h"
#include "util/xypoint.h"
#include "util/singleton.h"
#include "painterconfig.h"
#include <QDebug>
#include <QButtonGroup>
#include <cmath>

Chart::Chart(QWidget *parent) : QDialog(parent), ui(new Ui::Chart) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    curve = new QwtPlotCurve("");
    //curve->setBrush(QBrush(QColor(0, 0, 255)));
    curve->setPen(QPen(QColor(0, 255, 0)));
    curve->setSymbol(new QwtSymbol(QwtSymbol::Diamond, QBrush(QColor(0, 0, 255)), QPen(QColor(0, 0, 255)), QSize(5, 5)));
    curve->attach(ui->chart);
    grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(192, 192, 192)));
    picker = new QwtPlotPicker(ui->chart->canvas());
    //picker->setTrackerMode(QwtPlotPicker::AlwaysOn);
    picker->setStateMachine(new QwtPickerClickPointMachine());
    marker = new QwtPlotMarker();
    marker->setSymbol(new QwtSymbol(QwtSymbol::Cross, QBrush(QColor(255, 0, 0)), QPen(QColor(255, 0, 0)), QSize(10, 10)));
    marker->setLabelAlignment(Qt::AlignRight);
    marker->setLinePen(QPen(QColor(255, 0, 0)));
    marker->attach(ui->chart);
    //zoomer = new QwtPlotZoomer(ui->chart->canvas());
    xy = new QButtonGroup();
    xy->addButton(ui->x);
    xy->addButton(ui->y);
    xy->addButton(ui->z);
    xy->addButton(ui->time);
    scale = new QButtonGroup();
    scale->addButton(ui->autoScale);
    scale->addButton(ui->fixedScale);
    ui->autoScale->setChecked(true);
    /*ui->minScale->setAllowDecimal(true);
    ui->minScale->setAllowNegative(true);
    ui->maxScale->setAllowDecimal(true);
    ui->maxScale->setAllowNegative(true);*/
    ui->xValue->setVisible(false);
    connect(picker, SIGNAL(selected(QPointF)), this, SLOT(plotPickerSelected(QPointF)));
    active = true;
    lastGrid = 0;
}

Chart::~Chart() {
    delete ui;
    delete xy;
    delete scale;
    delete grid;
    delete curve;
    delete picker;
    delete marker;
    //delete zoomer;
}

void Chart::closeEvent(QCloseEvent*) {
    active = false;
}

void Chart::reject() {
    active = false;
}

void Chart::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    if (event->oldSize().width() != -1 && event->oldSize().height() != -1) {
        ui->chart->resize(ui->chart->width() + event->size().width() - event->oldSize().width(), ui->chart->height() + event->size().height() - event->oldSize().height());
    }
}

void Chart::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Chart::reset() {
    temp.clear();
}

int Chart::getInterval() {
    return ui->interval->value();
}

void Chart::disableCharts(bool disable) {
    ui->chartType->setEnabled(!disable);
}

void Chart::processGrid(Grid *grid) {
    if (active) {
        /*MyVector3D *vorticities = 0;
        double *streams = 0;*/
        if (grid == 0) {
            grid = lastGrid;
            /*if (grid != 0) {
                vorticities = grid->computeVorticity();
                streams = grid->computeStream(Singleton::instance()->getPainterConfig()->getStreamIntegrationX(), Singleton::instance()->getPainterConfig()->getStreamIntegrationY());
            }*/
        } else {
            lastGrid = grid;
            /*vorticities = grid->computeVorticity();
            streams = grid->computeStream(Singleton::instance()->getPainterConfig()->getStreamIntegrationX(), Singleton::instance()->getPainterConfig()->getStreamIntegrationY());*/
            if (ui->chartType->currentText() == "-log(DeltaP)") {
                temp.push_back(XYPoint(grid->getSimulation()->getIterations(), -std::log10(grid->getSimulation()->getDeltaP())));
            }
            if (ui->chartType->currentText() == "Drag") {
                grid->computeDrag();
                if (grid->getDrags() > 0) {
                    temp.push_back(XYPoint(grid->getSimulation()->getIterations(), grid->getDrag(0).norm()));
                }
            }
            if (ui->chartType->currentText() != "" && ui->time->isChecked()) {
                int x = ui->xValue->text().toInt() - 1, y = ui->yValue->text().toInt() - 1, z = ui->zValue->text().toInt() - 1;
                BaseCell *cell = grid->getGrid(y, x, z);
                if (cell != 0 && cell->isFluid()) {
                    if (ui->chartType->currentText() == "Pressure") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), cell->getP(ui->other->text().toInt())));
                    } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), cell->getU(ui->other->text().toInt()).norm()));
                    } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "x") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), cell->getU(ui->other->text().toInt()).getX()));
                    } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "y") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), cell->getU(ui->other->text().toInt()).getY()));
                    } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "z") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), cell->getU(ui->other->text().toInt()).getZ()));
                    /*} else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), vorticities[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].norm()));
                    } else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "x") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), vorticities[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].getX()));
                    } else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "y") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), vorticities[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].getY()));
                    } else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "z") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), vorticities[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].getZ()));
                    } else if (ui->chartType->currentText() == "Stream") {
                        temp.push_back(XYPoint(grid->getSimulation()->getIterations(), streams[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()]));*/
                    }
                }
            }
        }
        std::list<XYPoint> points;
        if (grid != 0) {
        for (int var = 0; var < (ui->x->isChecked() ? grid->getConfig()->getWidth() : (ui->y->isChecked() ? grid->getConfig()->getHeight() : ui->z->isChecked() ? grid->getConfig()->getLength() : 0)); var++) {
            int y = (ui->y->isChecked() ? var : ui->yValue->text().toInt() - 1);
            int x = (ui->x->isChecked() ? var : ui->xValue->text().toInt() - 1);
            int z = (ui->z->isChecked() ? var : ui->zValue->text().toInt() - 1);
            BaseCell *lattice = (grid->getGrid(y, x, z));
            if (lattice != 0 && lattice->isFluid()) {
                /*MyVector3D vorticity = vorticities[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()];
                double stream = streams[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()];*/
                double value = 0;
                if (ui->chartType->currentText() == "Pressure") {
                    value = lattice->getP(ui->other->text().toInt());
                } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "") {
                    value = lattice->getU(ui->other->text().toInt()).norm();
                } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "x") {
                    value = lattice->getU(ui->other->text().toInt()).getX();
                } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "y") {
                    value = lattice->getU(ui->other->text().toInt()).getY();
                } else if (ui->chartType->currentText() == "Velocity" && ui->axis->currentText() == "z") {
                    value = lattice->getU(ui->other->text().toInt()).getZ();
                /*} else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "") {
                    value = vorticity.norm();
                } else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "x") {
                    value = vorticity.getX();
                } else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "y") {
                    value = vorticity.getY();
                } else if (ui->chartType->currentText() == "Vorticity" && ui->axis->currentText() == "z") {
                    value = vorticity.getZ();
                } else if (ui->chartType->currentText() == "Stream") {
                    value = stream;*/
                }
                points.push_back(XYPoint(var, value));
            }
        }
        if (ui->chartType->currentText() == "-log(DeltaP)" || ui->chartType->currentText() == "Drag" || ui->time->isChecked()) {
            points = temp;
        }
        updateChart(points, curve);
        ui->chart->replot();
        }
        //delete[] vorticities;
    }
}

void Chart::on_chartType_currentIndexChanged(QString text) {
    temp.clear();
    if (text != "None") {
        processGrid(0);
    }
}

void Chart::updateChart(std::list<XYPoint> &points, QwtPlotCurve *curve) {
    double* x(new double[points.size()]);
    double* y(new double[points.size()]);
    int i = 0;
    for (std::list<XYPoint>::iterator point = points.begin(); point != points.end(); point++) {
        x[i] = (*point).getX();
        y[i] = (*point).getY();
        i++;
    }
    curve->setData(new QwtPointArrayData<double>(x, y, points.size()));
    delete[] x;
    delete[] y;
}

void Chart::on_xValue_textChanged(QString) {
    temp.clear();
    processGrid(0);
}

void Chart::on_yValue_textChanged(QString) {
    temp.clear();
    processGrid(0);
}

void Chart::on_zValue_textChanged(QString) {
    temp.clear();
    processGrid(0);
}

void Chart::on_autoScale_clicked() {
    ui->chart->setAxisAutoScale(QwtPlot::yLeft);
    ui->chart->setAxisAutoScale(QwtPlot::xBottom);
    ui->minScale->setEnabled(false);
    ui->maxScale->setEnabled(false);
    ui->minScale2->setEnabled(false);
    ui->maxScale2->setEnabled(false);
    ui->fixedUpdate->setEnabled(false);
    processGrid(0);
}

void Chart::on_fixedScale_clicked() {
    ui->minScale->setEnabled(true);
    ui->minScale->setText(QString::number(ui->chart->axisScaleDiv(QwtPlot::yLeft).lowerBound()));
    ui->maxScale->setEnabled(true);
    ui->maxScale->setText(QString::number(ui->chart->axisScaleDiv(QwtPlot::yLeft).upperBound()));
    ui->minScale2->setEnabled(true);
    ui->minScale2->setText(QString::number(ui->chart->axisScaleDiv(QwtPlot::xBottom).lowerBound()));
    ui->maxScale2->setEnabled(true);
    ui->maxScale2->setText(QString::number(ui->chart->axisScaleDiv(QwtPlot::xBottom).upperBound()));
    ui->fixedUpdate->setEnabled(true);
}

void Chart::on_fixedUpdate_clicked() {
    ui->chart->setAxisScale(QwtPlot::yLeft, ui->minScale->text().toDouble(), ui->maxScale->text().toDouble());
    ui->chart->setAxisScale(QwtPlot::xBottom, ui->minScale2->text().toDouble(), ui->maxScale2->text().toDouble());
    processGrid(0);
}

void Chart::on_x_clicked() {
    ui->y->setChecked(false);
    ui->z->setChecked(false);
    ui->time->setChecked(false);
    ui->xValue->setVisible(false);
    ui->yValue->setVisible(true);
    ui->zValue->setVisible(true);
    temp.clear();
    processGrid(0);
}

void Chart::on_y_clicked() {
    ui->x->setChecked(false);
    ui->z->setChecked(false);
    ui->time->setChecked(false);
    ui->xValue->setVisible(true);
    ui->yValue->setVisible(false);
    ui->zValue->setVisible(true);
    temp.clear();
    processGrid(0);
}

void Chart::on_z_clicked() {
    ui->x->setChecked(false);
    ui->y->setChecked(false);
    ui->time->setChecked(false);
    ui->xValue->setVisible(true);
    ui->yValue->setVisible(true);
    ui->zValue->setVisible(false);
    temp.clear();
    processGrid(0);
}

void Chart::on_axis_currentIndexChanged(int) {
    temp.clear();
    processGrid(0);
}

void Chart::on_time_clicked() {
    temp.clear();
    ui->x->setChecked(false);
    ui->y->setChecked(false);
    ui->z->setChecked(false);
    ui->xValue->setVisible(true);
    ui->yValue->setVisible(true);
    ui->zValue->setVisible(true);
    processGrid(0);
}

void Chart::plotPickerSelected(const QPointF &pos) {
    double xScale = ui->chart->axisScaleDiv(QwtPlot::xBottom).range();
    double yScale = ui->chart->axisScaleDiv(QwtPlot::yLeft).range();
    double minSquareDistance = ((curve->maxYValue() - curve->minYValue()) / yScale) * ((curve->maxYValue() - curve->minYValue()) / yScale) + ((curve->maxXValue() - curve->minXValue()) / xScale) * ((curve->maxXValue() - curve->minXValue()) / xScale);
    int minIndex = -1;
    for (unsigned int i = 0; i < curve->dataSize(); i++) {
        double newSquareDistance = ((curve->data()->sample(i).y() - pos.y()) / yScale) * ((curve->data()->sample(i).y() - pos.y()) / yScale) + ((curve->data()->sample(i).x() - pos.x()) / xScale) * ((curve->data()->sample(i).x() - pos.x()) / xScale);
        if (newSquareDistance < minSquareDistance) {
            minSquareDistance = newSquareDistance;
            minIndex = i;
        }
    }
    QPointF newPos(curve->data()->sample(minIndex));
    marker->setValue(newPos);
    marker->setLabel(QwtText(QString::number(newPos.x()).append(", ").append(QString::number(newPos.y()))));
    ui->chart->replot();
}

void Chart::on_showGrid_clicked() {
    if (ui->showGrid->isChecked()) {
        grid->attach(ui->chart);
    } else {
        grid->detach();
    }
    ui->chart->replot();
}

void Chart::on_clear_clicked() {
    temp.clear();
    processGrid(0);
}

void Chart::on_other_returnPressed() {
    temp.clear();
    processGrid(0);
}
