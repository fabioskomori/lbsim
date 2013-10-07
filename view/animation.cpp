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

#include "animation.h"
#include "ui_animation.h"
#include "view.h"
#include "ui_view.h"
#include "view2.h"
#include "ui_view2.h"
#include <QTimer>

Animation::Animation(QWidget *parent) : QDialog(parent), ui(new Ui::Animation) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(run()));
}

Animation::~Animation() {
    delete timer;
    delete ui;
}

void Animation::on_start_clicked() {
    ui->start->setEnabled(false);
    ui->stop->setEnabled(true);
    timer->setInterval(1000 / ui->fps->text().toInt());
    timer->start();
}

int Animation::normalize(int angle) {
    if (angle >= 180) {
        return angle - 360;
    } else if (angle <= -180) {
        return angle + 360;
    }
    return angle;
}

void Animation::run() {
    view->getUI()->roll->setText(QString::number(normalize(view->getUI()->roll->text().toInt() + ui->roll->text().toInt())));
    view->on_roll_returnPressed();
    view->getUI()->pitch->setText(QString::number(normalize(view->getUI()->pitch->text().toInt() + ui->pitch->text().toInt())));
    view->on_pitch_returnPressed();
    view->getUI()->yaw->setText(QString::number(normalize(view->getUI()->yaw->text().toInt() + ui->yaw->text().toInt())));
    view->on_yaw_returnPressed();
    view2->getUI()->fixedX->setValue((view2->getUI()->fixedX->value() + ui->yzPlane->text().toInt()) % 1001);
    view2->getUI()->fixedY->setValue((view2->getUI()->fixedY->value() + ui->xzPlane->text().toInt()) % 1001);
    view2->getUI()->fixedZ->setValue((view2->getUI()->fixedZ->value() + ui->xyPlane->text().toInt()) % 1001);
}

void Animation::inject(View *view) {
    this->view = view;
}

void Animation::inject(View2 *view2) {
    this->view2 = view2;
}

void Animation::on_stop_clicked() {
    timer->stop();
    ui->stop->setEnabled(false);
    ui->start->setEnabled(true);
}
