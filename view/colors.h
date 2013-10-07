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

#ifndef COLORS_H
#define COLORS_H

#include <QDialog>
class QXmlStreamWriter;
class QXmlStreamReader;

namespace Ui {
    class Colors;
}

class Painter;
class LBWidget;

class Colors : public QDialog {
    Q_OBJECT
public:
    Colors(LBWidget *widget, QWidget *parent = 0);
    ~Colors();
    void config(double average, double interval);
    void save(QXmlStreamWriter &writer);
    void load(QXmlStreamReader &reader);
protected:
    void changeEvent(QEvent *e);
private:
    LBWidget *widget;
    Ui::Colors *ui;
    double average, interval;
private slots:
    void on_x_returnPressed();
    void on_y_returnPressed();
    void on_width_returnPressed();
    void on_height_returnPressed();
    void on_captionX_returnPressed();
    void on_captionY_returnPressed();
    void on_captionPt_returnPressed();
    void on_colorStyle_currentIndexChanged(int index);
};

#endif // COLORS_H
