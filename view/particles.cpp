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

#include "particles.h"
#include "ui_particles.h"
#include "lbwidget.h"
#include "painter/painter.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/extra/velocityfield.h"
#include "../model/latticeboltzmann/extra/startingparticle.h"
#include <QColorDialog>
#include "../model/latticeboltzmann/passivescalar/passivescalarsingleton.h"
#include "../model/latticeboltzmann/passivescalar/configuration.h"
#include <QDebug>

Particles::Particles(LBWidget *widget, QWidget *parent) : QDialog(parent), ui(new Ui::Particles) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    this->widget = widget;
    stringListModel = new QStringListModel();
    ui->particles->setModel(stringListModel);
    stringListModel->setStringList(stringList);
    nextId = 1;
    color = QColor(255, 255, 255);
    color2 = QColor(255, 255, 255);
    ui->diffusionEffect->setAllowDecimal(true);
}

Particles::~Particles() {
    delete ui;
    delete stringListModel;
}

void Particles::updateNextId() {
    nextId = 1;
    for (int i = 0; i < stringList.length(); i++) {
        if (stringList.at(i).toInt() >= nextId) {
            nextId = stringList.at(i).toInt() + 1;
        }
    }
}

void Particles::load() {
    stringList.clear();
    if (ui->particlesType->isChecked()) {
        std::list<StartingParticle*> *particles = widget->getPainter()->getGrid()->getVelocityField()->getStartingPathLines();
        for (std::list<StartingParticle*>::iterator particle(particles->begin()); particle != particles->end(); particle++) {
            stringList << QString::number((*particle)->getId());
        }
        ui->velocity->setText(QString::number(widget->getPainter()->getGrid()->getVelocityField()->getAcceleration()));
        ui->diffusionEffect->setText(QString::number(widget->getPainter()->getGrid()->getVelocityField()->getDiffusionEffect()));
        ui->gridSpacing->setText(QString::number(widget->getPainter()->getGrid()->getVelocityField()->getGridSpacing()));
        ui->injectionPeriod->setText(QString::number(widget->getPainter()->getGrid()->getVelocityField()->getInjectionPeriod()));
    } else {
        std::list<Configuration*> pss = PassiveScalarSingleton::getInstance()->listPSS();
        for (std::list<Configuration*>::iterator ps = pss.begin(); ps != pss.end(); ps++) {
            stringList << QString::number((*ps)->getId());
        }
        ui->diffusionEffect->setText(QString::number(PassiveScalarSingleton::getInstance()->getEpsilon(0)));
        ui->injectionPeriod->setText(QString::number(PassiveScalarSingleton::getInstance()->getInjectionPeriod()));
        ui->injectionTime->setText(QString::number(PassiveScalarSingleton::getInstance()->getInjectionTime()));
        ui->passiveScalarMomentPropagation->setCurrentIndex(PassiveScalarSingleton::getInstance()->getLastType() - 1);
    }
    stringListModel->setStringList(stringList);
    updateNextId();
}

void Particles::on_particlesNew_clicked() {
    stringList << QString::number(nextId++);
    stringListModel->setStringList(stringList);
    controls(false);
}

void Particles::on_color_clicked() {
    QColor newColor = QColorDialog::getColor(color, this);
    if (newColor.isValid()) {
        color = newColor;
        delete ui->colorView->scene();
        QGraphicsScene *scene = new QGraphicsScene();
        scene->addRect(0, 0, 21, 21, QPen(color), QBrush(color));
        ui->colorView->setScene(scene);
    }
}

void Particles::on_particles_clicked(QModelIndex index) {
    controls(true);
    ui->minX->setText("");
    ui->maxX->setText("");
    ui->minY->setText("");
    ui->maxY->setText("");
    ui->minZ->setText("");
    ui->maxZ->setText("");
    if (ui->particlesType->isChecked()) {
        StartingParticle *particle = widget->getPainter()->getGrid()->getVelocityField()->getStartingPathLine(stringList.at(index.row()).toInt());
        if (particle != 0) {
            ui->minX->setText(QString::number((particle)->getFrom().getX() + 1));
            ui->maxX->setText(QString::number((particle)->getTo().getX() + 1));
            ui->minY->setText(QString::number((particle)->getFrom().getY() + 1));
            ui->maxY->setText(QString::number((particle)->getTo().getY() + 1));
            ui->minZ->setText(QString::number((particle)->getFrom().getZ() + 1));
            ui->maxZ->setText(QString::number((particle)->getTo().getZ() + 1));
            color = QColor((particle)->getColor().getX(), (particle)->getColor().getY(), (particle)->getColor().getZ());
            delete ui->colorView->scene();
            QGraphicsScene *scene = new QGraphicsScene();
            scene->addRect(0, 0, 21, 21, QPen(color), QBrush(color));
            ui->colorView->setScene(scene);
        }
    } else {
        Configuration *conf = PassiveScalarSingleton::getInstance()->get(stringList.at(index.row()).toInt());
        if (conf != 0) {
            ui->minX->setText(QString::number(conf->getMinX() + 1));
            ui->maxX->setText(QString::number(conf->getMaxX() + 1));
            ui->minY->setText(QString::number(conf->getMinY() + 1));
            ui->maxY->setText(QString::number(conf->getMaxY() + 1));
            ui->minZ->setText(QString::number(conf->getMinZ() + 1));
            ui->maxZ->setText(QString::number(conf->getMaxZ() + 1));
            color = QColor((conf)->getColor().getX(), (conf)->getColor().getY(), (conf)->getColor().getZ());
            delete ui->colorView->scene();
            QGraphicsScene *scene = new QGraphicsScene();
            scene->addRect(0, 0, 21, 21, QPen(color), QBrush(color));
            ui->colorView->setScene(scene);
        }
    }
}

void Particles::on_update_clicked() {
    int id = stringList.at(ui->particles->selectionModel()->currentIndex().row()).toInt();
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->remove(id);
        widget->getPainter()->getGrid()->getVelocityField()->addStartingPathLine(new StartingParticle(id, Vector3i(ui->minX->text().toInt() - 1, ui->minY->text().toInt() - 1, ui->minZ->text().toInt() - 1), Vector3i(ui->maxX->text().toInt() - 1, ui->maxY->text().toInt() - 1, ui->maxZ->text().toInt() - 1), Vector3i(color.red(), color.green(), color.blue()), Vector3i(color2.red(), color2.green(), color2.blue())));
    } else {
        PassiveScalarSingleton::getInstance()->remove(id);
        PassiveScalarSingleton::getInstance()->add(id, ui->minX->text().toInt() - 1, ui->maxX->text().toInt() - 1, ui->minY->text().toInt() - 1, ui->maxY->text().toInt() - 1, ui->minZ->text().toInt() - 1, ui->maxZ->text().toInt() - 1, Vector3i(color.red(), color.green(), color.blue()), ui->passiveScalarMomentPropagation->currentIndex() + 1);
        widget->getPainter()->updateCellsList();
    }
    stringListModel->setStringList(stringList);
    controls(false);
}

void Particles::on_reset_clicked() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->reset(stringList.at(ui->particles->selectionModel()->currentIndex().row()).toInt());
    } else {
        PassiveScalarSingleton::getInstance()->reset(stringList.at(ui->particles->selectionModel()->currentIndex().row()).toInt());
    }
}

void Particles::on_particlesDelete_clicked() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->remove(stringList.at(ui->particles->selectionModel()->currentIndex().row()).toInt());
    } else {
        PassiveScalarSingleton::getInstance()->remove(stringList.at(ui->particles->selectionModel()->currentIndex().row()).toInt());
        widget->getPainter()->updateCellsList();
    }
    stringList.removeAt(ui->particles->selectionModel()->currentIndex().row());
    stringListModel->setStringList(stringList);
    updateNextId();
    controls(false);
}

void Particles::on_resetAll_clicked() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->resetAll();
    } else {
        PassiveScalarSingleton::getInstance()->resetAll();
    }
}

void Particles::on_deleteAll_clicked() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->removeAll();
    } else {
        PassiveScalarSingleton::getInstance()->removeAll();
        widget->getPainter()->updateCellsList();
    }
    stringList.clear();
    stringListModel->setStringList(stringList);
    updateNextId();
    controls(false);
}

void Particles::controls(bool enabled) {
    ui->minX->setEnabled(enabled);
    ui->maxX->setEnabled(enabled);
    ui->minY->setEnabled(enabled);
    ui->maxY->setEnabled(enabled);
    ui->minZ->setEnabled(enabled);
    ui->maxZ->setEnabled(enabled);
    ui->color->setEnabled(enabled);
    ui->color2->setEnabled(enabled);
    ui->update->setEnabled(enabled);
    ui->particlesDelete->setEnabled(enabled);
    ui->reset->setEnabled(enabled);
}

void Particles::on_velocity_returnPressed() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->setAcceleration(ui->velocity->text().toInt());
    }
}

void Particles::on_diffusionEffect_returnPressed() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->setDiffusionEffect(ui->diffusionEffect->text().toDouble());
    } else {
        PassiveScalarSingleton::getInstance()->setEpsilon(0, ui->diffusionEffect->text().toDouble());
    }
}

void Particles::on_gridSpacing_returnPressed() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->setGridSpacing(ui->gridSpacing->text().toInt());
    }
}

void Particles::on_injectionPeriod_returnPressed() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->setInjectionPeriod(ui->injectionPeriod->text().toInt());
    } else {
        PassiveScalarSingleton::getInstance()->setInjectionPeriod(ui->injectionPeriod->text().toInt());
    }
}

void Particles::on_color2_clicked() {
    QColor newColor = QColorDialog::getColor(color, this);
    if (newColor.isValid()) {
        color2 = newColor;
        delete ui->colorView2->scene();
        QGraphicsScene *scene = new QGraphicsScene();
        scene->addRect(0, 0, 21, 21, QPen(color2), QBrush(color2));
        ui->colorView2->setScene(scene);
    }
}

void Particles::particlesOrPassiveScalar() {
    /*ui->injectionPeriod->setVisible(ui->particlesType->isChecked());
    ui->injectionPeriodLabel->setVisible(ui->particlesType->isChecked());*/
    ui->velocity->setVisible(ui->particlesType->isChecked());
    ui->velocityLabel->setVisible(ui->particlesType->isChecked());
    ui->color2->setVisible(ui->particlesType->isChecked());
    ui->colorView2->setVisible(ui->particlesType->isChecked());
    ui->gridSpacingLabel->setVisible(ui->particlesType->isChecked());
    ui->gridSpacing->setVisible(ui->particlesType->isChecked());
    load();
}

void Particles::on_particlesType_clicked() {
    particlesOrPassiveScalar();
}

void Particles::on_passiveScalarType_clicked() {
    particlesOrPassiveScalar();
}

void Particles::on_injectionTime_returnPressed() {
    if (ui->particlesType->isChecked()) {
        widget->getPainter()->getGrid()->getVelocityField()->setInjectionTime(ui->injectionTime->text().toInt());
    } else {
        PassiveScalarSingleton::getInstance()->setInjectionTime(ui->injectionTime->text().toInt());
    }
}

void Particles::on_initialConcentration_returnPressed() {
    if (!ui->particlesType->isChecked()) {
        PassiveScalarSingleton::getInstance()->setInitialConcentration(ui->initialConcentration->text().toDouble());
    }
}

void Particles::on_fixedConcentration_returnPressed() {
    if (!ui->particlesType->isChecked()) {
        PassiveScalarSingleton::getInstance()->setFixedConcentration(ui->fixedConcentration->text().toDouble());
    }
}
