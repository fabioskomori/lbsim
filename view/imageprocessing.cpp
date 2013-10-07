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

#include "imageprocessing.h"
#include "ui_imageprocessing.h"
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QColorDialog>
#include <QDebug>

ImageProcessing::ImageProcessing(QWidget *parent) : QMainWindow(parent), ui(new Ui::ImageProcessing) {
    ui->setupUi(this);
    ui->defaultColor->setStyleSheet("background-color: #ffffff");
    defaultColor = QColor(255, 255, 255);
    ui->fromColor->setStyleSheet("background-color: #ffffff");
    fromColor = QColor(255, 255, 255);
    ui->toColor->setStyleSheet("background-color: #ffffff");
    toColor = QColor(255, 255, 255);
    counter = 1;
    stringListModel = new QStringListModel();
    ui->colors->setModel(stringListModel);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->splitter->setStretchFactor(2, 1);
    ui->tolerance->setAllowDecimal(true);
}

ImageProcessing::~ImageProcessing() {
    delete ui;
    delete stringListModel;
}

void ImageProcessing::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open", "", tr("Images (*.png *.tiff)"));
    if (fileName != NULL && !fileName.isEmpty()) {
        QGraphicsScene *scene = new QGraphicsScene();
        fromImage = QImage(fileName);
        scene->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(fromImage)));
        ui->gv1->setScene(scene);
        process();
    }
}

void ImageProcessing::on_defaultColor_clicked() {
    QColor newColor = QColorDialog::getColor(defaultColor, this);
    if (newColor.isValid()) {
        defaultColor = newColor;
        ui->defaultColor->setStyleSheet(QString("background-color: ").append(defaultColor.name()));
    }
    process();
}

void ImageProcessing::on_fromColor_clicked() {
    QColor newColor = QColorDialog::getColor(fromColor, this);
    if (newColor.isValid()) {
        fromColor = newColor;
        ui->fromColor->setStyleSheet(QString("background-color: ").append(fromColor.name()));
    }
}

void ImageProcessing::on_toColor_clicked() {
    QColor newColor = QColorDialog::getColor(toColor, this);
    if (newColor.isValid()) {
        toColor = newColor;
        ui->toColor->setStyleSheet(QString("background-color: ").append(toColor.name()));
    }
}

void ImageProcessing::on_add_clicked() {
    fromColors[counter] = fromColor;
    toColors[counter] = toColor;
    stringList.append(QString::number(counter));
    stringListModel->setStringList(stringList);
    counter++;
    process();
}

void ImageProcessing::on_colors_clicked(const QModelIndex &index) {
    fromColor = fromColors[stringList.at(index.row()).toInt()];
    ui->fromColor->setStyleSheet(QString("background-color: ").append(fromColor.name()));
    toColor = toColors[stringList.at(index.row()).toInt()];
    ui->toColor->setStyleSheet(QString("background-color: ").append(toColor.name()));
}

void ImageProcessing::on_remove_clicked() {
    int index = ui->colors->selectionModel()->currentIndex().row();
    fromColors.erase(index);
    toColors.erase(index);
    stringList.removeAt(index);
    stringListModel->setStringList(stringList);
    ui->fromColor->setStyleSheet("background-color: #ffffff");
    fromColor = QColor(255, 255, 255);
    ui->toColor->setStyleSheet("background-color: #ffffff");
    toColor = QColor(255, 255, 255);
    process();
}

void ImageProcessing::process() {
    const int width = fromImage.width();
    const int height = fromImage.height();
    QColor *pixels = new QColor[width * height];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            pixels[y * width + x] = QColor(fromImage.pixel(x, y));
        }
    }
    int avg = ui->average->text().toInt();
    for (int x = avg; x < width - avg; x++) {
        for (int y = avg; y < height - avg; y++) {
            int r = 0, g = 0, b = 0;
            for (int dx = -avg; dx <= avg; dx++) {
                for (int dy = -avg; dy <= avg; dy++) {
                    r += pixels[(y + dy) * width + x + dx].red();
                    g += pixels[(y + dy) * width + x + dx].green();
                    b += pixels[(y + dy) * width + x + dx].blue();
                }
            }
            pixels[y * width + x] = QColor(r / ((2 * avg + 1) * (2 * avg + 1)), g / ((2 * avg + 1) * (2 * avg + 1)), b / ((2 * avg + 1) * (2 * avg + 1)));
        }
    }
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            QColor color = pixels[y * width + x];
            double distance = 0;
            int selected = -1;
            for (std::map<int, QColor>::iterator fromColor = fromColors.begin(); fromColor != fromColors.end(); fromColor++) {
                QColor temp = (*fromColor).second;
                double newDistance = (temp.red() - color.red()) * (temp.red() - color.red()) + (temp.green() - color.green()) * (temp.green() - color.green()) + (temp.blue() - color.blue()) * (temp.blue() - color.blue());
                if (selected == -1 || newDistance < distance) {
                    distance = newDistance;
                    selected = (*fromColor).first;
                }
            }
            if (selected != -1 && distance < ui->tolerance->text().toDouble()) {
                pixels[y * width + x] = toColors[selected];
            } else {
                pixels[y * width + x] = defaultColor;
            }
        }
    }
    toImage = QImage(width, height, QImage::Format_RGB32);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            toImage.setPixel(x, y, pixels[y * width + x].rgb());
        }
    }
    delete[] pixels;
    QGraphicsScene *scene = new QGraphicsScene();
    scene->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(toImage)));
    ui->gv2->setScene(scene);
}

void ImageProcessing::on_actionOnly_Left_triggered() {
    if (initialState.isEmpty()) {
        initialState = ui->splitter->saveState();
    }
    int min, max;
    ui->splitter->getRange(1, &min, &max);
    QList<int> sizes;
    sizes.append(max);
    sizes.append(0);
    sizes.append(0);
    ui->splitter->setSizes(sizes);
}

void ImageProcessing::on_actionOnly_Right_triggered() {
    if (initialState.isEmpty()) {
        initialState = ui->splitter->saveState();
    }
    int min, max;
    ui->splitter->getRange(1, &min, &max);
    QList<int> sizes;
    sizes.append(0);
    sizes.append(0);
    sizes.append(max);
    ui->splitter->setSizes(sizes);
}

void ImageProcessing::on_actionAll_triggered() {
    if (!initialState.isEmpty()) {
        ui->splitter->restoreState(initialState);
    }
}

void ImageProcessing::on_tolerance_returnPressed() {
    process();
}

void ImageProcessing::on_actionSave_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "*.png");
    if (fileName != NULL && !fileName.isEmpty()) {
        toImage.save(fileName, "PNG");
    }
}

void ImageProcessing::on_average_returnPressed() {
    process();
}
