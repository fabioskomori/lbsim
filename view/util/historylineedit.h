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

#ifndef HISTORYLINEEDIT_H
#define HISTORYLINEEDIT_H

#include <QLineEdit>
class QKeyEvent;

class HistoryLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit HistoryLineEdit(QWidget *parent = 0);
public slots:
    void returnPressed();
protected:
    void keyPressEvent(QKeyEvent *);
private:
    QStringList history;
    int pointer;
};

#endif // HISTORYLINEEDIT_H
