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

#ifndef LBWIDGET_H
#define LBWIDGET_H

#include "../model/math/myvector3d.h"
#include <QGLWidget>
#include <QTime>
class QMouseEvent;
class Results;
class Config2;
class View;
class Chart;
class Info;
class Executor;
class Painter;
class Scheduler;
class LBMainWindow;

class LBWidget : public QGLWidget {
    Q_OBJECT
public:
    LBWidget();
    ~LBWidget();
    void start();
    void stop();
    void reset();
    Painter* getPainter();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setUpdateRate(int updateRate);
    void setInfo(Info *info);
    void injectChart(Chart *chart);
    void injectView(View *view);
    void injectConfig(Config2 *config);
    void injectResults(Results *results);
    void injectMainWindow(LBMainWindow *mainWindow);
    void injectScheduler(Scheduler *scheduler);
    void setRestartTimer(bool restartTimer);
    LBMainWindow* getMainWindow();
public slots:
    void run();
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void glDraw();
private:
    void project(int x, int y, double *xx, double *yy, double *zz);
    Painter *painter;
    /*QTimer *timer;*/
    QPoint point;
    Qt::MouseButton button;
    Config2 *config;
    Results *results;
    LBMainWindow *mainWindow;
    QString folder;
    int updateRate;
    Info *info;
    int executions;
    int lastRendering;
    QDateTime *beginTime;
    Executor *executor;
    Scheduler *scheduler;
    int previousX, previousY;
    MyVector3D previous;
    bool mousePressed;
    bool forcePaintAll;
    std::list<Chart*> charts;
    bool restartTimer;
    View *view;
    bool mouseMoved;
};

#endif // LBWIDGET_H
