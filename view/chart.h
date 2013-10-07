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

#ifndef CHART_H
#define CHART_H

class XYPoint;
#include <QDialog>
class QwtPlotCurve;
class QwtPlotGrid;
class QButtonGroup;
class Grid;
class QwtPlotPicker;
class QwtPlotMarker;
class QwtPlotZoomer;

namespace Ui {
    class Chart;
}

class Chart : public QDialog {
    Q_OBJECT
public:
    Chart(QWidget *parent = 0);
    ~Chart();
    void processGrid(Grid *grid);
    void reset();
    void resizeEvent(QResizeEvent *);
    int getInterval();
    static void updateChart(std::list<XYPoint> &points, QwtPlotCurve *curve);
    void disableCharts(bool disable);
    void closeEvent(QCloseEvent *event);
    void reject();
protected:
    void changeEvent(QEvent *e);
private:
    std::list<XYPoint> temp;
    Grid *lastGrid;
    Ui::Chart *ui;
    QwtPlotCurve *curve;
    QButtonGroup *xy, *scale;
    QwtPlotGrid *grid;
    QwtPlotPicker *picker;
    QwtPlotMarker *marker;
    QwtPlotZoomer *zoomer;
    bool active;
public slots:
    void plotPickerSelected(const QPointF &pos);
    void on_fixedUpdate_clicked();
    void on_fixedScale_clicked();
    void on_autoScale_clicked();
    void on_xValue_textChanged(QString );
    void on_yValue_textChanged(QString );
    void on_zValue_textChanged(QString );
    void on_chartType_currentIndexChanged(QString );
private slots:
    void on_x_clicked();
    void on_y_clicked();
    void on_z_clicked();
    void on_axis_currentIndexChanged(int index);
    void on_time_clicked();
    void on_showGrid_clicked();
    void on_clear_clicked();
    void on_other_returnPressed();
};

#endif // CHART_H
