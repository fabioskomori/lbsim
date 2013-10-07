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

#include "view2.h"
#include "ui_view2.h"
#include "painter/painter.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "lbwidget.h"

View2::View2(LBWidget *widget, QWidget *parent) : QDialog(parent), ui(new Ui::View2) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    this->widget = widget;
    fixedX = fixedY = fixedZ = false;
}

View2::~View2() {
    delete ui;
}

Ui::View2* View2::getUI() {
    return ui;
}

void View2::on_fixedX_valueChanged(int value) {
    fixedX = true;
    if (value < ui->minX->value()) {
        ui->minX->setValue(value);
        ui->maxX->setValue(value);
    } else {
        ui->maxX->setValue(value);
        ui->minX->setValue(value);
    }
    fixedX = false;
}

void View2::on_fixedY_valueChanged(int value) {
    fixedY = true;
    if (value < ui->minY->value()) {
        ui->minY->setValue(value);
        ui->maxY->setValue(value);
    } else {
        ui->maxY->setValue(value);
        ui->minY->setValue(value);
    }
    fixedY = false;
}

void View2::on_minX_valueChanged(int value) {
    if (value > ui->maxX->value()) {
        value = ui->maxX->value();
        ui->minX->setValue(value);
    }
    if (!fixedX) {
        if (ui->minX->value() == ui->maxX->value()) {
            //ui->fixedX->setEnabled(true);
            ui->fixedX->setValue(ui->minX->value());
        } else {
            //ui->fixedX->setEnabled(false);
        }
    }
    int newValue = (widget->getPainter()->getGrid()->getConfig()->getWidth() - 1) / 1000.0 * value;
    ui->minXLabel->setText(QString::number(newValue + 1));
    widget->getPainter()->setMinX(newValue);
    widget->updateGL();
}

void View2::on_maxX_valueChanged(int value) {
    if (value < ui->minX->value()) {
        value = ui->minX->value();
        ui->maxX->setValue(value);
    }
    if (!fixedX) {
        if (ui->minX->value() == ui->maxX->value()) {
            //ui->fixedX->setEnabled(true);
            ui->fixedX->setValue(ui->minX->value());
        } else {
            //ui->fixedX->setEnabled(false);
        }
    }
    int newValue = (widget->getPainter()->getGrid()->getConfig()->getWidth() - 1) / 1000.0 * value;
    ui->maxXLabel->setText(QString::number(newValue + 1));
    widget->getPainter()->setMaxX(newValue);
    widget->updateGL();
}

void View2::on_minY_valueChanged(int value) {
    if (value > ui->maxY->value()) {
        value = ui->maxY->value();
        ui->minY->setValue(value);
    }
    if (!fixedY) {
        if (ui->minY->value() == ui->maxY->value()) {
            //ui->fixedY->setEnabled(true);
            ui->fixedY->setValue(ui->minY->value());
        } else {
            //ui->fixedY->setEnabled(false);
        }
    }
    int newValue = (widget->getPainter()->getGrid()->getConfig()->getHeight() - 1) / 1000.0 * value;
    ui->minYLabel->setText(QString::number(newValue + 1));
    widget->getPainter()->setMinY(newValue);
    widget->updateGL();
}

void View2::on_maxY_valueChanged(int value) {
    if (value < ui->minY->value()) {
        value = ui->minY->value();
        ui->maxY->setValue(value);
    }
    if (!fixedY) {
        if (ui->minY->value() == ui->maxY->value()) {
            //ui->fixedY->setEnabled(true);
            ui->fixedY->setValue(ui->minY->value());
        } else {
            //ui->fixedY->setEnabled(false);
        }
    }
    int newValue = (widget->getPainter()->getGrid()->getConfig()->getHeight() - 1) / 1000.0 * value;
    ui->maxYLabel->setText(QString::number(newValue + 1));
    widget->getPainter()->setMaxY(newValue);
    widget->updateGL();
}

void View2::on_minZ_valueChanged(int value) {
    if (value > ui->maxZ->value()) {
        value = ui->maxZ->value();
        ui->minZ->setValue(value);
    }
    if (!fixedZ) {
        if (ui->minZ->value() == ui->maxZ->value()) {
            //ui->fixedZ->setEnabled(true);
            ui->fixedZ->setValue(ui->minZ->value());
        } else {
            //ui->fixedZ->setEnabled(false);
        }
    }
    int newValue = (widget->getPainter()->getGrid()->getConfig()->getLength() - 1) / 1000.0 * value;
    ui->minZLabel->setText(QString::number(newValue + 1));
    widget->getPainter()->setMinZ(newValue);
    widget->updateGL();
}

void View2::on_maxZ_valueChanged(int value) {
    if (value < ui->minZ->value()) {
        value = ui->minZ->value();
        ui->maxZ->setValue(value);
    }
    if (!fixedZ) {
        if (ui->minZ->value() == ui->maxZ->value()) {
            //ui->fixedZ->setEnabled(true);
            ui->fixedZ->setValue(ui->minZ->value());
        } else {
            //ui->fixedZ->setEnabled(false);
        }
    }
    int newValue = (widget->getPainter()->getGrid()->getConfig()->getLength() - 1) / 1000.0 * value;
    ui->maxZLabel->setText(QString::number(newValue + 1));
    widget->getPainter()->setMaxZ(newValue);
    widget->updateGL();
}

void View2::on_fixedZ_valueChanged(int value) {
    fixedZ = true;
    if (value < ui->minZ->value()) {
        ui->minZ->setValue(value);
        ui->maxZ->setValue(value);
    } else {
        ui->maxZ->setValue(value);
        ui->minZ->setValue(value);
    }
    fixedZ = false;
}

void View2::on_allX_clicked() {
    ui->minX->setValue(0);
    ui->maxX->setValue(1000);
    //ui->fixedX->setEnabled(false);
    ui->minXLabel->setText("1");
    ui->maxXLabel->setText(QString::number(widget->getPainter()->getGrid()->getConfig()->getWidth()));
}

void View2::on_allY_clicked() {
    ui->minY->setValue(0);
    ui->maxY->setValue(1000);
    //ui->fixedY->setEnabled(false);
    ui->minYLabel->setText("1");
    ui->maxYLabel->setText(QString::number(widget->getPainter()->getGrid()->getConfig()->getHeight()));
}

void View2::on_allZ_clicked() {
    ui->minZ->setValue(0);
    ui->maxZ->setValue(1000);
    //ui->fixedZ->setEnabled(false);
    ui->minZLabel->setText("1");
    ui->maxZLabel->setText(QString::number(widget->getPainter()->getGrid()->getConfig()->getLength()));
}

void View2::on_xyPlane_clicked() {
    widget->getPainter()->setRenderingOrientation((ui->xyPlane->isChecked() ? 4 : 0) + (ui->xzPlane->isChecked() ? 2 : 0) + (ui->yzPlane->isChecked() ? 1 : 0));
    widget->updateGL();
}

void View2::on_xzPlane_clicked() {
    on_xyPlane_clicked();
}

void View2::on_yzPlane_clicked() {
    on_xyPlane_clicked();
}

void View2::on_wholePlane1_clicked() {
    widget->getPainter()->setWholePlane(ui->wholePlane1->isChecked(), ui->wholePlane2->isChecked(), ui->wholePlane3->isChecked());
    widget->updateGL();
}

void View2::on_wholePlane2_clicked() {
    on_wholePlane1_clicked();
}

void View2::on_wholePlane3_clicked() {
    on_wholePlane1_clicked();
}
