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

#ifndef INFO_H
#define INFO_H

#include "../model/listener/listener.h"
#include <QDockWidget>
class QColor;
class QString;
class Grid;
class QXmlStreamWriter;
class QXmlStreamReader;
class Chart;
class LBWidget;

namespace Ui {
    class Info;
}

class Info : public QDockWidget, public Listener {
    Q_OBJECT
public:
    Info(Grid *grid, QWidget *parent = 0);
    ~Info();
    void inject(LBWidget *widget);
    void callback(int event);
    void setFile(QString file);
    void logPerformance(int expendedSecs, int remainingSecs, double meanPerformance);
    int getRenderUpdate();
    int getExecutionBurst();
    void save(QXmlStreamWriter &writer);
    void load(QXmlStreamReader &reader);
    Ui::Info* getUI();
    QSize sizeHint() const;
    void inject(Chart *chart);
    void clearPointInfo();
protected:
    void changeEvent(QEvent *e);
public slots:
    void test();
    void on_disableCharts_clicked();
    void on_multicoreSupport_clicked();
private slots:
    void on_infoX_returnPressed();
    void on_infoY_returnPressed();
    void on_infoZ_returnPressed();
    void on_infoX2_returnPressed();
    void on_infoY2_returnPressed();
    void on_infoZ2_returnPressed();
    void on_test_clicked();
private:
    void updatePointInfo();
    void updatePointInfo2();
    QColor color(double value);
    QString formatTime(int secs);
    void updateOpenCL();
    Ui::Info *ui;
    Grid *grid;
    std::list<Chart*> charts;
    LBWidget *widget;
};

#endif // INFO_H
