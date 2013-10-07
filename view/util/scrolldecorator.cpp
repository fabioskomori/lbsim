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

#include "scrolldecorator.h"
#include <QSizePolicy>
#include <QScrollArea>

ScrollDecorator::ScrollDecorator(QDockWidget *dockWidget, int minWidth, int minHeight, QWidget *parent, QString title) : QDockWidget(title, parent, Qt::SubWindow) {
    this->dockWidget = dockWidget;
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    scrollArea = new QScrollArea();
    scrollArea->setWidget(dockWidget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidget(scrollArea);
    this->minWidth = minWidth;
    this->minHeight = minHeight;
}

QSize ScrollDecorator::minimumSizeHint() const {
    return QSize(minWidth, minHeight);
}

ScrollDecorator::~ScrollDecorator() {
    delete scrollArea;
}
