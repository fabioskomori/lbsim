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

#include "colors.h"
#include "ui_colors.h"
#include <QColor>
#include "painter/painter.h"
#include "lbwidget.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

Colors::Colors(LBWidget *widget, QWidget *parent) : QDialog(parent), widget(widget), ui(new Ui::Colors) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    average = -1;
    interval = -1;
    ui->x->setAllowDecimal(true);
    ui->x->setAllowNegative(true);
    ui->y->setAllowDecimal(true);
    ui->y->setAllowNegative(true);
    ui->width->setAllowDecimal(true);
    ui->width->setAllowNegative(true);
    ui->height->setAllowDecimal(true);
    ui->height->setAllowNegative(true);
    ui->captionX->setAllowDecimal(true);
    ui->captionX->setAllowNegative(true);
    ui->captionY->setAllowDecimal(true);
    ui->captionY->setAllowNegative(true);
}

Colors::~Colors() {
    delete ui;
}

void Colors::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Colors::config(double average, double interval) {
    this->average = average;
    this->interval = interval;
    ui->firstMarker->setText(QString::number(average - 2 * interval, 'g', 3));
    ui->secondMarker->setText(QString::number(average - interval, 'g', 3));
    ui->thirdMarker->setText(QString::number(average, 'g', 3));
    ui->fourthMarker->setText(QString::number(average + interval, 'g', 3));
    ui->fifthMarker->setText(QString::number(average + 2 * interval, 'g', 3));
    QGraphicsScene *scene = new QGraphicsScene();
    for (int i = 0; i < ui->graphicsView->width(); i++) {
        QColor color = widget->getPainter()->getColor(average - 2 * interval + 4 * interval * i / ui->graphicsView->width());
        scene->addRect(i, 0, 1, ui->graphicsView->height(), QPen(color), QBrush(color));
    }
    delete ui->graphicsView->scene();
    ui->graphicsView->setScene(scene);
}

void Colors::save(QXmlStreamWriter &writer) {
    writer.writeStartElement("colors");
    writer.writeAttribute("x", ui->x->text());
    writer.writeAttribute("y", ui->y->text());
    writer.writeAttribute("width", ui->width->text());
    writer.writeAttribute("height", ui->height->text());
    writer.writeAttribute("captionX", ui->captionX->text());
    writer.writeAttribute("captionY", ui->captionY->text());
    writer.writeAttribute("captionPt", ui->captionPt->text());
    writer.writeAttribute("colorStyle", QString::number(ui->colorStyle->currentIndex()));
    writer.writeEndElement();
}

void Colors::load(QXmlStreamReader &reader) {
    if (reader.name().toString() == "colors") {
        ui->x->setText(reader.attributes().value("x").toString());
        ui->y->setText(reader.attributes().value("y").toString());
        ui->width->setText(reader.attributes().value("width").toString());
        ui->height->setText(reader.attributes().value("height").toString());
        ui->captionX->setText(reader.attributes().value("captionX").toString());
        ui->captionY->setText(reader.attributes().value("captionY").toString());
        ui->captionPt->setText(reader.attributes().value("captionPt").toString());
        on_x_returnPressed();
        if (reader.attributes().hasAttribute("colorStyle")) {
            on_colorStyle_currentIndexChanged(reader.attributes().value("colorStyle").toString().toInt());
        } else {
            on_colorStyle_currentIndexChanged(1);
            on_colorStyle_currentIndexChanged(0);
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void Colors::on_x_returnPressed() {
    widget->getPainter()->setColorBar(ui->x->text().toDouble(), ui->y->text().toDouble(),
                                      ui->width->text().toDouble(), ui->height->text().toDouble(),
                                      ui->captionX->text().toDouble(), ui->captionY->text().toDouble(), ui->captionPt->text().toInt());
    widget->updateGL();
}

void Colors::on_y_returnPressed() {
    on_x_returnPressed();
}

void Colors::on_width_returnPressed() {
    on_x_returnPressed();
}

void Colors::on_height_returnPressed() {
    on_x_returnPressed();
}

void Colors::on_captionX_returnPressed() {
    on_x_returnPressed();
}

void Colors::on_captionY_returnPressed() {
    on_x_returnPressed();
}

void Colors::on_captionPt_returnPressed() {
    on_x_returnPressed();
}

void Colors::on_colorStyle_currentIndexChanged(int index) {
    widget->getPainter()->setColorStyle(index);
    config(average, interval);
    widget->updateGL();
}
