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

#include "improvedlineedit.h"
#include <QDebug>
#include "singleton.h"
#include <QKeyEvent>

ImprovedLineEdit::ImprovedLineEdit(QWidget *parent) : QLineEdit(parent) {
    allowDecimal = false;
    allowNegative = false;
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(textEdited()));
    connect(this, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
}

void ImprovedLineEdit::keyPressEvent(QKeyEvent *event) {
    QLineEdit::keyPressEvent(event);
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        this->setText(QString::number(this->text().toDouble() + (event->key() == Qt::Key_Up ? 1 : -1)));
        textEdited();
    }
}

void ImprovedLineEdit::textEdited() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, Qt::red);
    this->setPalette(palette);
}

void ImprovedLineEdit::returnPressed() {
    this->setText(allowDecimal ? QString::number(this->text().toDouble()) : QString::number(this->text().toInt()));
    if (!allowNegative && this->text().at(0) == '-') {
        this->setText("0");
    }
    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, Qt::black);
    this->setPalette(palette);
}

void ImprovedLineEdit::setAllowDecimal(bool allowDecimal) {
    this->allowDecimal = allowDecimal;
}

void ImprovedLineEdit::setAllowNegative(bool allowNegative) {
    this->allowNegative = allowNegative;
}
