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

#include "lbwidget.h"
#include "info.h"
#include "lbmainwindow.h"
#include "chart.h"
#include "view.h"
#include "util/executor.h"
#include "painter/painter.h"
#include "painter/camera.h"
#include "../model/inputoutput/iterationreporter.h"
#include "../model/inputoutput/simulationreporter.h"
#include "../model/latticeboltzmann/gridconfig.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/latticeboltzmann/gridsimulation.h"
#include "util/singleton.h"
#include "ui_results.h"
#include <sstream>
#include <QDir>
#include <QDebug>
#include <QShortcut>
#include <QMouseEvent>
#include "results.h"
#include <cmath>
#include <QtCore>

LBWidget::LBWidget() {
    painter = new Painter(this);
    //timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(run()));
    folder = "";
    updateRate = 0;
    executions = 0;
    executor = new Executor(this);
    //previousX = previousY = -1;
    mousePressed = false;
    forcePaintAll = false;
    beginTime = 0;
    lastRendering = 0;
}

LBWidget::~LBWidget() {
    delete executor;
    //delete timer;
    delete painter;
    if (beginTime != 0) {
        delete beginTime;
    }
}

void LBWidget::initializeGL() {
}

void LBWidget::resizeGL(int w, int h) {
    painter->getCamera()->resize(w, h);
    updateGL();
}

Painter* LBWidget::getPainter() {
    return painter;
}

void LBWidget::paintGL() {
    painter->paint(forcePaintAll);
}

void LBWidget::glDraw() {
    QGLWidget::glDraw();
}

void LBWidget::start() {
    if (beginTime != 0) {
        delete beginTime;
    }
    beginTime = new QDateTime(QDateTime::currentDateTime());
    executions = 0;
    lastRendering = 0;
    //if (updateRate > 0) {
        /*if (!timer->isActive()) {
            timer->start(updateRate);
        }*/
        QMetaObject::invokeMethod(this, "run", Qt::QueuedConnection);
    /*} else {
        executor->run();
    }*/
}

void LBWidget::stop() {
    folder = "";
    if (beginTime != 0) {
        delete beginTime;
        beginTime = 0;
    }
    //if (updateRate > 0) {
        /*if (timer->isActive()) {
            timer->stop();
        }*/
    /*} else {
        executor->stop();
    }*/
}

void LBWidget::project(int x, int y, double *xx, double *yy, double *zz) {
    double tempX = (x - (width() / 2.0)) / (width() / 2.0);
    double tempY = -(y - (height() / 2.0)) / (height() / 2.0);
    double zSquared = (1.0 - (tempX * tempX) - (tempY * tempY));
    if (zSquared > 0.0f) {
        *xx = tempX;
        *yy = tempY;
        *zz = std::sqrt(zSquared);
    } else {
        float magnitude = sqrt((tempX * tempX) + (tempY * tempY));
        *xx = tempX / magnitude;
        *yy = tempY / magnitude;
        *zz = 0.0f;
    }
}

void LBWidget::mousePressEvent(QMouseEvent *event) {
    button = event->button();
    double xx, yy, zz;
    project(event->x(), event->y(), &xx, &yy, &zz);
    switch (event->button()) {
    case Qt::LeftButton:
        previousX = event->x();
        previousY = event->y();
        break;
    case Qt::RightButton:
        previous = MyVector3D(xx, yy, zz);
        break;
    default:
        break;
    }
    mousePressed = true;
    mouseMoved = false;
}

void LBWidget::mouseMoveEvent(QMouseEvent *event) {
    /*if (previousX != -1) {
        int size = width();
        if (height() > size) {
            size = height();
        }
        painter->getCamera()->translateX(((double) 2 * (event->x() - previousX)) / size);
        painter->getCamera()->translateY(((double) 2 * (previousY - event->y())) / size);
        previousX = event->x();
        previousY = event->y();
        updateGL();
    }*/
    if (mousePressed) {
        double xx, yy, zz;
        project(event->x(), event->y(), &xx, &yy, &zz);
        MyVector3D current = MyVector3D(xx, yy, zz);
        switch (button) {
        case Qt::LeftButton:
            painter->getCamera()->translate(MyVector3D((event->x() - previousX) * 0.001, (previousY - event->y()) * 0.001, 0));
            view->setTranslation(painter->getCamera()->getTranslation().getX(), painter->getCamera()->getTranslation().getY(), painter->getCamera()->getTranslation().getZ());
            previousX = event->x();
            previousY = event->y();
            break;
        case Qt::RightButton:
            if ((previous - current).norm() > 0.001) {
                painter->getCamera()->rotate(previous, current);
                view->setEuler((int)painter->getCamera()->getRotation().getRoll(), (int)painter->getCamera()->getRotation().getPitch(), (int)painter->getCamera()->getRotation().getYaw());
                previous = current;
            }
            break;
        default:
            break;
        }
        updateGL();
    }
    mouseMoved = true;
}

void LBWidget::mouseReleaseEvent(QMouseEvent*) {
    mousePressed = false;
}

void LBWidget::wheelEvent(QWheelEvent *event) {
    painter->getCamera()->resize(width(), height(), -event->delta() * 0.001);
    view->setZoom(painter->getCamera()->getZoom());
    updateGL();
}

void LBWidget::run() {
    //timer->stop();
    if (results->getSaveImage() && (painter->getGrid()->getSimulation()->getIterations() - 1 + results->getSaveInterval()) % results->getSaveInterval() == 0) {
        this->grabFrameBuffer().save(QString(folder + "/img_") + QString::number(painter->getGrid()->getSimulation()->getIterations() - 1).rightJustified(6, QChar('0')) + ".png", "PNG");
    }
    //Singleton::instance()->getRenderMutex()->lock();
    int updated = painter->getGrid()->update(info->getExecutionBurst());
    //Singleton::instance()->getRenderMutex()->unlock();
    forcePaintAll = (results->getSaveImage() && painter->getGrid()->getSimulation()->getIterations() % results->getSaveInterval() == 0);
    if ((painter->needsToPaint() && painter->getGrid()->getSimulation()->getIterations() - lastRendering >= info->getRenderUpdate()) || forcePaintAll) {
        lastRendering = painter->getGrid()->getSimulation()->getIterations();
        updateGL();
    }
    if (updated > 0) {
        executions += updated;
        if (beginTime != 0) {
            int remaining = 0;
            double performance = ((double) (painter->getGrid()->getConfig()->getWidth() * painter->getGrid()->getConfig()->getHeight() * painter->getGrid()->getConfig()->getLength())) * executions / beginTime->msecsTo(QDateTime::currentDateTime()) / 1000;
            if (mainWindow->getLB()->getPainter()->getGrid()->getConfig()->getMaxIterations() != 0) {
                remaining = (int)((double)painter->getGrid()->getConfig()->getWidth() * painter->getGrid()->getConfig()->getHeight() * painter->getGrid()->getConfig()->getLength() * (mainWindow->getLB()->getPainter()->getGrid()->getConfig()->getMaxIterations() - mainWindow->getLB()->getPainter()->getGrid()->getSimulation()->getIterations()) / (performance * 1e6));
            }
            info->logPerformance(beginTime->secsTo(QDateTime::currentDateTime()), remaining, performance);
            painter->getGrid()->getSimulation()->setPerformance(performance);
        }
        if ((results->getSaveImage() || results->getReportVelocities() || results->getReportSimulation()) &&
                painter->getGrid()->getSimulation()->getIterations() % results->getSaveInterval() == 0) {
            if (folder == "" && (results->getSaveImage() || results->getReportVelocities() || results->getReportSimulation())) {
                folder = Singleton::instance()->getFolder() + "sim" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + "-" + mainWindow->getLastFileName().remove(QRegExp(".*[/]")).remove(QRegExp("[.].*"));
                QDir().mkdir(folder);
            }
            if (results->getReportVelocities()) {
                QString fileName(folder + "/vp_" + QString::number(painter->getGrid()->getSimulation()->getIterations()).rightJustified(6, QChar('0')) + ".csv");
                IterationReporter *reporter = new IterationReporter(-1, -1, -1);
                reporter->reportCsv(painter->getGrid(), fileName);
                delete reporter;
            }
            if (results->getReportSimulation()) {
                QString fileName(folder + "/simulation.csv");
                SimulationReporter::reportSimulation(painter->getGrid(), fileName);
            }
        }
        for (std::list<Chart*>::iterator chart = charts.begin(); chart != charts.end(); chart++) {
            if (!painter->getGrid()->getConfig()->getDisableCharts() &&
                    painter->getGrid()->getSimulation()->getIterations() % (*chart)->getInterval() == 0) {
                (*chart)->processGrid(painter->getGrid());
            }
        }
    }
    if (restartTimer) {
        QMetaObject::invokeMethod(this, "run", Qt::QueuedConnection);
    }
}

void LBWidget::setUpdateRate(int updateRate) {
    this->updateRate = updateRate;
}

void LBWidget::reset() {
    folder = "";
}

void LBWidget::setInfo(Info *info) {
    this->info = info;
}

void LBWidget::injectChart(Chart *chart) {
    charts.push_back(chart);
}

void LBWidget::injectScheduler(Scheduler *scheduler) {
    this->scheduler = scheduler;
}

void LBWidget::injectMainWindow(LBMainWindow *mainWindow) {
    this->mainWindow = mainWindow;
}

void LBWidget::injectResults(Results *results) {
    this->results = results;
}

void LBWidget::setRestartTimer(bool restartTimer) {
    this->restartTimer = restartTimer;
}

void LBWidget::injectView(View *view) {
    this->view = view;
}

void LBWidget::injectConfig(Config2 *config) {
    this->config = config;
}

LBMainWindow* LBWidget::getMainWindow() {
    return mainWindow;
}
