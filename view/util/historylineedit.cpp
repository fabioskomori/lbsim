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

#include "historylineedit.h"
#include <QDebug>
#include <QKeyEvent>

HistoryLineEdit::HistoryLineEdit(QWidget *parent) : QLineEdit(parent) {
    connect(this, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    pointer = -1;
}

void HistoryLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) {
        if (pointer > 0) {
            pointer--;
            this->setText(history.at(pointer));
        }
    } else if (event->key() == Qt::Key_Down) {
        if (pointer < history.size() - 1) {
            pointer++;
            this->setText(history.at(pointer));
        }
    }
    QLineEdit::keyPressEvent(event);
}

void HistoryLineEdit::returnPressed() {
    history << this->text();
    pointer = history.size();
}
