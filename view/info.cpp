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

#include "info.h"
#include "ui_info.h"
#include <QTime>
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/basecell.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "../model/latticeboltzmann/gridsimulation.h"
//#include "../model/latticeboltzmann/opencl/openclplatform.h"
//#include "../model/latticeboltzmann/opencl/openclsingleton.h"
//#include "../model/latticeboltzmann/opencl/opencldevice.h"
//#include "../model/latticeboltzmann/opencl/allocation.h"
//#include "../model/latticeboltzmann/opencl/lbocl.h"
#include "../model/listener/listenerdata.h"
#include "painter/painter.h"
#include "lbwidget.h"
#include "lbmainwindow.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>
#include "chart.h"

using namespace std;

Info::Info(Grid *grid, QWidget *parent) : QDockWidget("Info", parent, Qt::SubWindow), ui(new Ui::Info) {
    this->grid = grid;
    ui->setupUi(this);
    /*ui->openclCPU->setEnabled(OpenCLFacade::supportCPU());
    ui->openclGPU->setEnabled(OpenCLFacade::supportGPU());
    ui->openclIterations->setEnabled(ui->openclCPU->isEnabled() || ui->openclGPU->isEnabled());*/
}

void Info::callback(int) {
    /*if (data.getIterations() != -1) {
        ui->iterations->setText(QString::number(data.getIterations()));
        ui->deltaP->setText(QString::number(data.getDeltaP()));
        ui->averageP->setText(QString::number(data.getAverageP()));
        if (data.getIterations() == 0) {
            ui->performance->setText("0");
        }
    }
    if (data.getWidth() != -1) {
        ui->gridSize->setText(QString(QString::number(data.getWidth()) + QString(" x ") +
                QString::number(data.getHeight()) + QString(" x ") +
                QString::number(data.getLength())));
    }*/
    ui->gridSize->setText(QString::number(grid->getConfig()->getWidth()).append(QString(" x ")).
            append(QString::number(grid->getConfig()->getHeight())).append(QString(" x ")).
            append(QString::number(grid->getConfig()->getLength())));
    ui->iterations->setText(QString::number(grid->getSimulation()->getIterations()));
    ui->deltaP->setText(QString::number(grid->getSimulation()->getDeltaP()));
    ui->averageP->setText(QString::number(grid->getSimulation()->getDeltasP() / grid->getSimulation()->getTotalP()));
    if (grid->getSimulation()->getIterations() == 0) {
        ui->performance->setText("0");
    }
}

void Info::setFile(QString file) {
    ui->file->setText(file);
}

void Info::clearPointInfo() {
    ui->infoX->setText("0");
    ui->infoY->setText("0");
    ui->infoZ->setText("0");
    on_infoX_returnPressed();
}

QSize Info::sizeHint() const {
    return QSize(200, 100);
}

Info::~Info() {
    delete ui;
}

Ui::Info* Info::getUI() {
    return ui;
}

void Info::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void Info::inject(Chart *chart) {
    charts.push_back(chart);
    chart->disableCharts(ui->disableCharts->isChecked());
}

void Info::inject(LBWidget *widget) {
    this->widget = widget;
}

void Info::logPerformance(int expendedSecs, int remainingSecs, double meanPerformance) {
    ui->expendedTime->setText(formatTime(expendedSecs));
    ui->performance->setText(QString::number(meanPerformance));
    ui->remainingTime->setText(formatTime(remainingSecs));
}

void Info::on_disableCharts_clicked() {
    grid->getConfig()->setDisableCharts(ui->disableCharts->isChecked());
    for (std::list<Chart*>::iterator chart = charts.begin(); chart != charts.end(); chart++) {
        (*chart)->disableCharts(ui->disableCharts->isChecked());
    }
}

void Info::on_multicoreSupport_clicked() {
    grid->getConfig()->setMulticoreSupport(ui->multicoreSupport->isChecked());
}

QColor Info::color(double value) {
    double colorAverage = 0;
    double colorDelta = 0.023;
    double lowerLimit = colorAverage - colorDelta;
    double firstLimit = colorAverage - colorDelta / 2;
    double secondLimit = colorAverage;
    double thirdLimit = colorAverage + colorDelta / 2;
    double upperLimit = colorAverage + colorDelta;
    double firstInterval = colorDelta / 2;
    double secondInterval = colorDelta / 2;
    double thirdInterval = colorDelta / 2;
    double fourthInterval = colorDelta / 2;
    QColor result;
    if (value <= lowerLimit || value >= upperLimit) {
        result.setRgbF(1, 1, 1);
    } else if (value < firstLimit) {
        result.setRgbF(0, (value - lowerLimit) / firstInterval, 1 - (value - lowerLimit) / firstInterval);
    } else if (value < secondLimit) {
        result.setRgbF((value - firstLimit) / secondInterval, 1, 0);
    } else if (value < thirdLimit) {
        result.setRgbF(1, 1 - 0.5 * (value - secondLimit) / thirdInterval, 0);
    } else {
        result.setRgbF(1, 0.5 - 0.5 * (value - thirdLimit) / fourthInterval, 0);
    }
    return result;
}

QString Info::formatTime(int secs) {
    int hours = secs / 3600;
    secs -= hours * 3600;
    int minutes = secs / 60;
    secs -= minutes * 60;
    QString formattedHours = QString::number(hours);
    if (formattedHours.size() == 1) {
        formattedHours = "0" + formattedHours;
    }
    QString formattedMinutes = QString::number(minutes);
    if (formattedMinutes.size() == 1) {
        formattedMinutes = "0" + formattedMinutes;
    }
    QString formattedSeconds = QString::number(secs);
    if (formattedSeconds.size() == 1) {
        formattedSeconds = "0" + formattedSeconds;
    }
    return formattedHours.append(":").append(formattedMinutes).append(":").append(formattedSeconds);
}

/*void Info::on_cpu_clicked() {
    if (ui->cpu->isChecked()) {
        ui->openclPrecision->setEnabled(false);
        grid->setOpenclType(CPU, ui->doublePrecision->isChecked());
    } else {
        ui->openclPrecision->setEnabled(true);
        ui->singlePrecision->setEnabled(true);
        if (OpenCLFacade::supportDouble(ui->openclGPU->isChecked())) {
            ui->doublePrecision->setEnabled(true);
        } else {
            ui->doublePrecision->setEnabled(false);
            ui->singlePrecision->setChecked(true);
        }
        if (ui->openclCPU->isChecked()) {
            grid->setOpenclType(OPENCL_CPU, ui->doublePrecision->isChecked());
        } else if (ui->openclGPU->isChecked()) {
            grid->setOpenclType(OPENCL_GPU, ui->doublePrecision->isChecked());
        }
    }
}

void Info::on_openclCPU_clicked() {
    on_cpu_clicked();
}

void Info::on_openclGPU_clicked() {
    on_cpu_clicked();
}*/

int Info::getRenderUpdate() {
    return ui->renderUpdate->text().toInt();
}

void Info::save(QXmlStreamWriter &writer) {
    writer.writeStartElement("info");
    writer.writeAttribute("multicore", ui->multicoreSupport->isChecked() ? "1" : "0");
    writer.writeAttribute("charts", ui->disableCharts->isChecked() ? "1" : "0");
    writer.writeAttribute("renderUpdate", ui->renderUpdate->text());
    writer.writeEndElement();
}

void Info::load(QXmlStreamReader &reader) {
    if (reader.name().toString() == "info") {
        ui->multicoreSupport->setChecked(reader.attributes().value("multicore").toString() == "1");
        on_multicoreSupport_clicked();
        ui->disableCharts->setChecked(reader.attributes().value("charts").toString() == "1");
        on_disableCharts_clicked();
        ui->renderUpdate->setText(reader.attributes().value("renderUpdate").toString());
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

int Info::getExecutionBurst() {
    return ui->executionBurst->text().toInt();
}

void Info::updatePointInfo() {
    ui->velocityX->setText("");
    ui->velocityY->setText("");
    ui->velocityZ->setText("");
    ui->density->setText("");
    ui->vorticityX->setText("");
    ui->vorticityY->setText("");
    ui->vorticityZ->setText("");
    ui->stream->setText("");
    BaseCell *cell = grid->getGrid(ui->infoY->text().toInt() - 1, ui->infoX->text().toInt() - 1, ui->infoZ->text().toInt() - 1);
    widget->getPainter()->setHighlight(ui->infoX->text().toInt(), ui->infoY->text().toInt(), ui->infoZ->text().toInt());
    if (cell != 0) {
        ui->velocityX->setText(QString::number(cell->getU(0).getX()));
        ui->velocityY->setText(QString::number(cell->getU(0).getY()));
        ui->velocityZ->setText(QString::number(cell->getU(0).getZ()));
        ui->density->setText(QString::number(cell->getP(0)));
        MyVector3D *vorticities = grid->computeVorticity();
        ui->vorticityX->setText(QString::number(vorticities[ui->infoX->text().toInt() + ui->infoY->text().toInt() * grid->getConfig()->getWidth()].getX()));
        ui->vorticityY->setText(QString::number(vorticities[ui->infoX->text().toInt() + ui->infoY->text().toInt() * grid->getConfig()->getWidth()].getY()));
        ui->vorticityZ->setText(QString::number(vorticities[ui->infoX->text().toInt() + ui->infoY->text().toInt() * grid->getConfig()->getWidth()].getZ()));
        delete[] vorticities;
        double *streams = grid->computeStream(0, 0);
        ui->stream->setText(QString::number(streams[ui->infoX->text().toInt() + ui->infoY->text().toInt() * grid->getConfig()->getWidth()]));
        delete[] streams;
    }
    widget->repaint();
}

void Info::updatePointInfo2() {
    widget->getPainter()->setHighlight(ui->infoX2->text().toInt(), ui->infoY2->text().toInt(), ui->infoZ2->text().toInt(), 1);
    widget->repaint();
}

void Info::on_infoX_returnPressed() {
    if (ui->infoX->text().toInt() == 0) {
        ui->infoY->setText("0");
        ui->infoZ->setText("0");
    }
    updatePointInfo();
}

void Info::on_infoY_returnPressed() {
    if (ui->infoY->text().toInt() == 0) {
        ui->infoX->setText("0");
        ui->infoZ->setText("0");
    }
    updatePointInfo();
}

void Info::on_infoZ_returnPressed() {
    if (ui->infoZ->text().toInt() == 0) {
        ui->infoX->setText("0");
        ui->infoY->setText("0");
    }
    updatePointInfo();
}

void Info::on_infoX2_returnPressed() {
    if (ui->infoX2->text().toInt() == 0) {
        ui->infoY2->setText("0");
        ui->infoZ2->setText("0");
    }
    updatePointInfo2();
}

void Info::on_infoY2_returnPressed() {
    if (ui->infoY2->text().toInt() == 0) {
        ui->infoX2->setText("0");
        ui->infoZ2->setText("0");
    }
    updatePointInfo2();
}

void Info::on_infoZ2_returnPressed() {
    if (ui->infoZ2->text().toInt() == 0) {
        ui->infoX2->setText("0");
        ui->infoY2->setText("0");
    }
    updatePointInfo2();
}

void Info::on_test_clicked() {
    QMetaObject::invokeMethod(this, "test", Qt::QueuedConnection);
}

void Info::test() {
    /*for (int i = 100; i <= 200; i += 10) {
        int width = i, height = i, executions = 1000, update = 1000;
        char *data = new char[width * height];
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                data[x + y * width] = 'x';
            }
        }
        for (int x = 0; x < width; x++) {
            data[x + (height - 1) * width] = 'v';
            data[x] = 'w';
        }
        for (int y = 0; y < height; y++) {
            data[y * width] = 'w';
            data[(width - 1) + y * width] = 'w';
        }
        grid->load(data, width, height, 1);
        delete[] data;
        grid->setOpenclUpdate(update);
        std::list<OpenCLDevice*> *devices = new std::list<OpenCLDevice*>();
        OpenCLSingleton::getAllDevices(CL_DEVICE_TYPE_ALL);
        OpenCLSingleton::getDevice(0)->setAllocation(new Allocation(0, width - 1, false, false));
        devices->push_back(OpenCLSingleton::getDevice(0));
        grid->setOpenclType(OPENCL_GPU, false, devices);
        grid->getOpenCL()->load();
        QDateTime dt = QDateTime::currentDateTime();
        grid->update(executions);
        long ms = dt.msecsTo(QDateTime::currentDateTime());
        qDebug() << "size:" << i << "elapsed:" << ms << "ms /" << ((double)(width * height * executions)) / (1000 * ms) << "MLUPS";
    }*/
}
