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

#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QMainWindow>
#include <map>
#include <QStringListModel>

namespace Ui {
    class ImageProcessing;
}

class ImageProcessing : public QMainWindow {
    Q_OBJECT
public:
    explicit ImageProcessing(QWidget *parent = 0);
    ~ImageProcessing();
private slots:
    void on_actionOpen_triggered();
    void on_defaultColor_clicked();
    void on_fromColor_clicked();
    void on_toColor_clicked();
    void on_add_clicked();
    void on_colors_clicked(const QModelIndex &index);
    void on_remove_clicked();
    void on_actionOnly_Left_triggered();
    void on_actionOnly_Right_triggered();
    void on_actionAll_triggered();
    void on_tolerance_returnPressed();
    void on_actionSave_triggered();

    void on_average_returnPressed();

private:
    void process();
    Ui::ImageProcessing *ui;
    QImage fromImage, toImage;
    QColor defaultColor, fromColor, toColor;
    std::map<int, QColor> fromColors;
    std::map<int, QColor> toColors;
    int counter;
    QStringListModel *stringListModel;
    QStringList stringList;
    QByteArray initialState;
};

#endif // IMAGEPROCESSING_H
