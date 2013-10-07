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

#include "view.h"
#include "ui_view.h"
#include "lbwidget.h"
#include "painter/painter.h"
#include "painter/camera.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include <QColorDialog>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "painterconfig.h"
#include "util/singleton.h"

View::View(LBWidget *widget, QWidget *parent) : QDockWidget("View", parent), ui(new Ui::View) {
    ui->setupUi(this);
    this->widget = widget;
    ui->colorOffset->setAllowNegative(true);
    ui->colorOffset->setAllowDecimal(true);
    ui->colorInterval->setAllowNegative(true);
    ui->colorInterval->setAllowDecimal(true);
    ui->cellsSize->setAllowDecimal(true);
    ui->cellsSize2->setAllowDecimal(true);
    ui->zoom->setAllowDecimal(true);
    ui->roll->setAllowNegative(true);
    ui->pitch->setAllowNegative(true);
    ui->yaw->setAllowNegative(true);
    ui->tx->setAllowDecimal(true);
    ui->tx->setAllowNegative(true);
    ui->ty->setAllowDecimal(true);
    ui->ty->setAllowNegative(true);
    ui->tz->setAllowDecimal(true);
    ui->tz->setAllowNegative(true);
    ui->colorComponent->setAllowNegative(true);
    painterConfig = new PainterConfig(parent);
    painterConfig->injectPainter(widget->getPainter());
    painterConfig->injectWidget(widget);
    Singleton::instance()->setPainterConfig(painterConfig);
}

View::~View() {
    delete painterConfig;
    delete ui;
}

Ui::View* View::getUI() {
    return ui;
}

QSize View::sizeHint() const {
    return QSize(170, 100);
}

void View::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void View::changePainter(QString painter) {
    if (painter == "velocity") {
        ui->painter->setCurrentIndex(0);
    } else if (painter == "pressure") {
        ui->painter->setCurrentIndex(1);
    } else if (painter == "arrow") {
        ui->painter->setCurrentIndex(2);
    } else if (painter == "detail") {
        ui->painter->setCurrentIndex(3);
    }
}

void View::changeView(QString view) {
    if (view == "axis") {
        ui->showAxis->click();
    } else if (view == "static") {
        ui->showStatic->click();
    } else if (view == "dynamic") {
        ui->showDynamic->click();
    } else if (view == "particles") {
        ui->showParticles->click();
    }
}

void View::on_showStatic_stateChanged(int) {
    widget->getPainter()->toggleShowSourceOpen();
    widget->updateGL();
}

void View::on_showDynamic_stateChanged(int) {
    widget->getPainter()->toggleShowFluid();
    widget->updateGL();
}

void View::on_painter_currentIndexChanged(QString) {
    widget->getPainter()->setStrategy(ui->painter->currentText());
    widget->repaint();
}

void View::on_colorOffset_returnPressed() {
    widget->getPainter()->setColorRange((ui->colorOffset->text().toDouble() + ui->colorInterval->text().toDouble()) / 2, (ui->colorInterval->text().toDouble() - ui->colorOffset->text().toDouble()) / 2, false);
    widget->updateGL();
}

void View::on_colorInterval_returnPressed() {
    widget->getPainter()->setColorRange((ui->colorOffset->text().toDouble() + ui->colorInterval->text().toDouble()) / 2, (ui->colorInterval->text().toDouble() - ui->colorOffset->text().toDouble()) / 2, false);
    widget->updateGL();
}

void View::on_updatePeriod_returnPressed() {
    widget->setUpdateRate(ui->updatePeriod->text().toInt());
}

void View::on_autoAdjust_clicked() {
    widget->getPainter()->autoAdjust();
    widget->updateGL();
}

void View::notify() {
    ui->colorOffset->setText(QString::number(widget->getPainter()->getColorAverage() - widget->getPainter()->getColorDelta(), 'g', 6));
    ui->colorInterval->setText(QString::number(widget->getPainter()->getColorAverage() + widget->getPainter()->getColorDelta(), 'g', 6));
}

void View::injectColors(Colors *colors) {
    this->colors = colors;
}

void View::on_continuousAutoAdjust_clicked() {
    widget->getPainter()->toggleContinuousAutoAdjust();
}

void View::on_cellsSize_returnPressed() {
    widget->getPainter()->setCellsSize(ui->cellsSize->text().toDouble());
    widget->getPainter()->updateDisplayList(true);
    widget->updateGL();
}

void View::on_cellsSize2_returnPressed() {
    widget->getPainter()->setCellsSize2(ui->cellsSize2->text().toDouble());
    widget->updateGL();
}

void View::on_showParticles_toggled(bool) {
    widget->getPainter()->toggleShowParticles();
    widget->updateGL();
}

void View::on_colorSteps_returnPressed() {
    widget->getPainter()->setColorSteps(ui->colorSteps->text().toInt());
}

void View::on_arrowSteps_returnPressed() {
    widget->getPainter()->setArrowSteps(ui->arrowSteps->text().toInt());
    widget->updateGL();
}

void View::on_resetProjection_clicked() {
    ui->tx->setText("0");
    ui->ty->setText("0");
    ui->tz->setText("0");
    ui->roll->setText("0");
    ui->pitch->setText("0");
    ui->yaw->setText("0");
    widget->getPainter()->getCamera()->reset();
    widget->repaint();
}

void View::setZoom(double zoom) {
    ui->zoom->setText(QString::number(zoom, 'g', 3));
}

void View::setEuler(int roll, int pitch, int yaw) {
    ui->roll->setText(QString::number(roll));
    ui->pitch->setText(QString::number(pitch));
    ui->yaw->setText(QString::number(yaw));
}

void View::setTranslation(double x, double y, double z) {
    ui->tx->setText(QString::number(x, 'f', 2));
    ui->ty->setText(QString::number(y, 'f', 2));
    ui->tz->setText(QString::number(z, 'f', 2));
}

/*void View::on_background_clicked() {
    QColor color = QColorDialog::getColor(widget->getPainter()->getBackgroundColor(), this);
    if (color.isValid()) {
        widget->getPainter()->setBackgroundColor(color);
        widget->repaint();
    }
}*/

void View::on_zoom_returnPressed() {
    widget->getPainter()->getCamera()->setZoom(ui->zoom->text().toDouble());
    widget->updateGL();
}

void View::on_roll_returnPressed() {
    widget->getPainter()->getCamera()->setEuler(ui->roll->text().toInt(), ui->pitch->text().toInt(), ui->yaw->text().toInt());
    widget->updateGL();
}

void View::on_pitch_returnPressed() {
    on_roll_returnPressed();
}

void View::on_yaw_returnPressed() {
    on_roll_returnPressed();
}

void View::save(QXmlStreamWriter &writer) {
    writer.writeStartElement("view");
    writer.writeAttribute("roll", ui->roll->text());
    writer.writeAttribute("pitch", ui->pitch->text());
    writer.writeAttribute("yaw", ui->yaw->text());
    writer.writeAttribute("zoom", ui->zoom->text());
    writer.writeAttribute("painter", QString::number(ui->painter->currentIndex()));
    writer.writeAttribute("scale1", ui->cellsSize->text());
    writer.writeAttribute("scale2", ui->cellsSize2->text());
    writer.writeAttribute("updatePeriod", ui->updatePeriod->text());
    writer.writeAttribute("arrowSteps", ui->arrowSteps->text());
    writer.writeAttribute("showAxis", ui->showAxis->isChecked() ? "1" : "0");
    writer.writeAttribute("showStatic", ui->showStatic->isChecked() ? "1" : "0");
    writer.writeAttribute("showDynamic", ui->showDynamic->isChecked() ? "1" : "0");
    writer.writeAttribute("showParticles", ui->showParticles->isChecked() ? "1" : "0");
    writer.writeAttribute("show2ndDistribution", ui->show2ndDistribution->isChecked() ? "1" : "0");
    writer.writeAttribute("colorOffset", QString::number((ui->colorOffset->text().toDouble() + ui->colorInterval->text().toDouble()) / 2));
    writer.writeAttribute("colorInterval", QString::number((ui->colorOffset->text().toDouble() - ui->colorInterval->text().toDouble()) / 2));
    writer.writeAttribute("colorSteps", ui->colorSteps->text());
    writer.writeAttribute("autoAdjust", ui->continuousAutoAdjust->isChecked() ? "1" : "0");
    writer.writeAttribute("backgroundR", QString::number(widget->getPainter()->getBackgroundColor().red()));
    writer.writeAttribute("backgroundG", QString::number(widget->getPainter()->getBackgroundColor().green()));
    writer.writeAttribute("backgroundB", QString::number(widget->getPainter()->getBackgroundColor().blue()));
    writer.writeAttribute("showColorBar", ui->showColorBar->isChecked() ? "1" : "0");
    writer.writeAttribute("tx", ui->tx->text());
    writer.writeAttribute("ty", ui->ty->text());
    writer.writeAttribute("tz", ui->tz->text());
    writer.writeEndElement();
}

void View::load(QXmlStreamReader &reader) {
    if (reader.name().toString() == "view") {
        ui->roll->setText(reader.attributes().value("roll").toString());
        ui->pitch->setText(reader.attributes().value("pitch").toString());
        ui->yaw->setText(reader.attributes().value("yaw").toString());
        on_roll_returnPressed();
        ui->zoom->setText(reader.attributes().value("zoom").toString());
        on_zoom_returnPressed();
        ui->painter->setCurrentIndex(reader.attributes().value("painter").toString().toInt());
        on_painter_currentIndexChanged("");
        ui->cellsSize->setText(reader.attributes().value("scale1").toString());
        on_cellsSize_returnPressed();
        ui->cellsSize2->setText(reader.attributes().value("scale2").toString());
        on_cellsSize2_returnPressed();
        if (reader.attributes().hasAttribute("updatePeriod")) {
            widget->getPainter()->setBackgroundColor(QColor(reader.attributes().value("backgroundR").toString().toInt(), reader.attributes().value("backgroundG").toString().toInt(), reader.attributes().value("backgroundB").toString().toInt()));
            ui->updatePeriod->setText(reader.attributes().value("updatePeriod").toString());
            ui->updatePeriod->returnPressed();
            ui->arrowSteps->setText(reader.attributes().value("arrowSteps").toString());
            ui->arrowSteps->returnPressed();
            ui->showAxis->setChecked(reader.attributes().value("showAxis").toString() == "1");
            ui->showStatic->setChecked(reader.attributes().value("showStatic").toString() == "1");
            ui->showDynamic->setChecked(reader.attributes().value("showDynamic").toString() == "1");
            ui->showParticles->setChecked(reader.attributes().value("showParticles").toString() == "1");
            bool checked = ui->continuousAutoAdjust->isChecked();
            ui->continuousAutoAdjust->setChecked(reader.attributes().value("autoAdjust").toString() == "1");
            if (checked != ui->continuousAutoAdjust->isChecked()) {
                on_continuousAutoAdjust_clicked();
            }
            double colorOffset = reader.attributes().value("colorOffset").toString().toDouble();
            double colorInterval = reader.attributes().value("colorInterval").toString().toDouble();
            ui->colorOffset->setText(QString::number(colorOffset + colorInterval));
            ui->colorInterval->setText(QString::number(colorOffset - colorInterval));
            on_colorOffset_returnPressed();
            ui->colorSteps->setText(reader.attributes().value("colorSteps").toString());
            ui->colorSteps->returnPressed();
        }
        if (reader.attributes().hasAttribute("showColorBar")) {
            ui->showColorBar->setChecked(reader.attributes().value("showColorBar").toString() == "1");
            ui->tx->setText(reader.attributes().value("tx").toString());
            ui->ty->setText(reader.attributes().value("ty").toString());
            ui->tz->setText(reader.attributes().value("tz").toString());
            on_tx_returnPressed();
        }
        if (reader.attributes().hasAttribute("show2ndDistribution")) {
            ui->show2ndDistribution->setChecked(reader.attributes().value("show2ndDistribution").toString() == "1");
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void View::on_allView_clicked() {
    ui->showAxis->setChecked(ui->allView->isChecked());
    ui->showDynamic->setChecked(ui->allView->isChecked());
    ui->showParticles->setChecked(ui->allView->isChecked());
    ui->showStatic->setChecked(ui->allView->isChecked());
    ui->showColorBar->setChecked(ui->allView->isChecked());
    ui->show2ndDistribution->setChecked(ui->allView->isChecked());
}

void View::on_showColorBar_toggled(bool) {
    widget->getPainter()->toggleShowColorBar();
    widget->updateGL();
}

void View::on_showAxis_toggled(bool) {
    widget->getPainter()->toggleShowAxis();
    widget->updateGL();
}

void View::on_tx_returnPressed() {
    widget->getPainter()->getCamera()->setTranslation(ui->tx->text().toDouble(), ui->ty->text().toDouble(), ui->tz->text().toDouble());
    widget->updateGL();
}

void View::on_ty_returnPressed() {
    on_tx_returnPressed();
}

void View::on_tz_returnPressed() {
    on_tx_returnPressed();
}

void View::on_show2ndDistribution_toggled(bool) {
    widget->getPainter()->toggleShow2ndDistribution();
    widget->updateGL();
}

void View::on_colorComponent_returnPressed() {
    widget->getPainter()->setColorComponent(ui->colorComponent->text().toInt());
    widget->updateGL();
}

void View::on_configurePainter_clicked() {
    painterConfig->show();
}

void View::on_showIsolines_toggled(bool) {
    widget->getPainter()->toggleShowIsolines();
    widget->updateGL();
}

void View::on_heightmap_toggled(bool) {
    widget->getPainter()->toggleShowHeightmap();
    widget->updateGL();
}

void View::on_illumination_toggled(bool) {
    widget->getPainter()->toggleShowLight();
    widget->updateGL();
}

void View::on_showForces_toggled(bool) {
    widget->getPainter()->toggleShowForces();
    widget->repaint();
}
