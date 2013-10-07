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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <QDialog>
class View;
class View2;

namespace Ui {
    class Animation;
}

class Animation : public QDialog {
    Q_OBJECT
public:
    explicit Animation(QWidget *parent = 0);
    ~Animation();
    void inject(View *view);
    void inject(View2 *view2);
private slots:
    void on_start_clicked();
    void run();
    void on_stop_clicked();
private:
    int normalize(int angle);
    QTimer *timer;
    View *view;
    View2 *view2;
    Ui::Animation *ui;
};

#endif // ANIMATION_H
