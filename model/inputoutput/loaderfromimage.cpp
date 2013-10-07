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

#include "loaderfromimage.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include <QFile>
#include <QImage>
#include <QDebug>
#include "../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../model/latticeboltzmann/moving/movingcell.h"
#include "../../model/latticeboltzmann/boundary/sourcecell.h"
#include "../../model/latticeboltzmann/boundary/opencell.h"
#include "../../model/latticeboltzmann/porouscell.h"
#include "../../model/latticeboltzmann/nullcell.h"
#include "../../model/latticeboltzmann/thermal/thermalwall.h"
#include "../../model/latticeboltzmann/boundary/reflectwithfactorcell.h"
#include "../../model/latticeboltzmann/passivescalar/passivescalarcell.h"

QColor LoaderFromImage::null = QColor(0, 255, 0);
QColor LoaderFromImage::porous = QColor(64, 64, 64);
QColor LoaderFromImage::moving = QColor(255, 255, 0);
QColor LoaderFromImage::open = QColor(0, 0, 0);
QColor LoaderFromImage::source = QColor(255, 0, 0);
QColor LoaderFromImage::movingWall = QColor(0, 0, 255);
QColor LoaderFromImage::wall = QColor(128, 128, 128);
QColor LoaderFromImage::negative = QColor(128, 0, 255);
QColor LoaderFromImage::positive = QColor(255, 0, 255);
QColor LoaderFromImage::shallow = QColor(128, 128, 255);
QColor LoaderFromImage::point = QColor(0, 255, 255);
QColor LoaderFromImage::thermal = QColor(192, 192, 192);
QColor LoaderFromImage::dragWall = QColor(160, 160, 160);
QColor LoaderFromImage::slipWall = QColor(96, 96, 96);
QColor LoaderFromImage::depositionWall = QColor(32, 32, 32);
QColor LoaderFromImage::reflectWithFactor = QColor(224, 224, 224);

void LoaderFromImage::save(Grid *grid, QString &fileName, int scale) {
    for (int z = 0; z < grid->getConfig()->getLength(); z++) {
        QImage image = QImage(grid->getConfig()->getWidth() * scale, grid->getConfig()->getHeight() * scale, QImage::Format_RGB32);
        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
            for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
                BaseCell *cell = grid->getGrid(y, x, z);
                PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                if (psc != 0) {
                    cell = psc->getCell();
                }
                QColor color = QColor(255, 255, 255);
                if (dynamic_cast<ThermalWall*>(cell) != 0) {
                    color = thermal;
                } else if (dynamic_cast<ReflectWithFactorCell*>(cell) != 0) {
                    color = reflectWithFactor;
                } else if (dynamic_cast<WallCell*>(cell) != 0) {
                    color = wall;
                } else if (dynamic_cast<MovingCell*>(cell) != 0) {
                    color = movingWall;
                } else if (dynamic_cast<SourceCell*>(cell) != 0) {
                    color = source;
                } else if (dynamic_cast<OpenCell*>(cell) != 0) {
                    color = open;
                } else if (dynamic_cast<PorousCell*>(cell) != 0) {
                    color = porous;
                } else if (dynamic_cast<NullCell*>(cell) != 0) {
                    color = null;
                }
                for (int dx = 0; dx < scale; dx++) {
                    for (int dy = 0; dy < scale; dy++) {
                        image.setPixel(x * scale + dx, (grid->getConfig()->getHeight() - 1 - y) * scale + dy, color.rgb());
                    }
                }
            }
        }
        image.save(z == 0 ? fileName : fileName.left(fileName.length() - 4) + "_" + QString::number(z + 1) + fileName.right(4), "PNG");
    }
}

void LoaderFromImage::load(Grid *grid, QString &fileName) {
    int LENGTH = 1;
    while (true) {
        QString newFileName = fileName;
        QFile file(newFileName.insert(newFileName.length() - 4, QString("_").append(QString::number(LENGTH + 1))));
        if (!file.exists()) {
            break;
        }
        LENGTH++;
    }
    QImage image;
    image.load(fileName);
    int WIDTH = image.width();
    int HEIGHT = image.height();
    char* data = new char[HEIGHT * WIDTH * LENGTH];
    if (LENGTH > 1 && grid->getConfig()->getModel() == 9) {
        grid->getConfig()->setModel(19);
    }
    for (int k = 0; k < LENGTH; k++) {
        if (LENGTH != 1) {
            QString newFileName = fileName;
            image.load(k == 0 ? fileName : newFileName.insert(newFileName.length() - 4, QString("_").append(QString::number(k + 1))));
        }
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                QColor color(image.pixel(j, HEIGHT - 1 - i));
                if (color == wall) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'w';
                } else if (color == movingWall) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'v';
                } else if (color == source) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 's';
                } else if (color == open) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'o';
                } else if (color == moving) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'm';
                } else if (color == negative) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'n';
                } else if (color == positive) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'p';
                } else if (color == porous) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = '*';
                } else if (color == shallow) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'h';
                } else if (color == null) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = '-';
                } else if (color == point) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = '.';
                } else if (color == thermal) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 't';
                } else if (color == dragWall) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'd';
                } else if (color == slipWall) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'k';
                } else if (color == depositionWall) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'e';
                } else if (color == reflectWithFactor) {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'r';
                } else {
                    data[k + j * LENGTH + i * LENGTH * WIDTH] = 'x';
                }
            }
        }
    }
    grid->load(data, WIDTH, HEIGHT, LENGTH);
    delete[] data;
}
