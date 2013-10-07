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

#ifndef VIEW_H
#define VIEW_H

#include <QDockWidget>
class QXmlStreamWriter;
class QXmlStreamReader;
class LBWidget;
class Colors;
class PainterConfig;

namespace Ui {
    class View;
}

class View : public QDockWidget {
    Q_OBJECT
public:
    View(LBWidget *widget, QWidget *parent = 0);
    ~View();
    void injectColors(Colors *colors);
    void notify();
    QSize sizeHint() const;
    void setZoom(double zoom);
    void setEuler(int roll, int pitch, int yaw);
    void setTranslation(double x, double y, double z);
    void changePainter(QString painter);
    void changeView(QString view);
    void save(QXmlStreamWriter &writer);
    void load(QXmlStreamReader &reader);
    Ui::View* getUI();
protected:
    void changeEvent(QEvent *e);
private:
    Ui::View *ui;
    LBWidget *widget;
    Colors *colors;
    PainterConfig *painterConfig;
public slots:
    void on_zoom_returnPressed();
    void on_roll_returnPressed();
    void on_yaw_returnPressed();
    void on_pitch_returnPressed();
    void on_cellsSize2_returnPressed();
    //void on_background_clicked();
    void on_resetProjection_clicked();
    void on_arrowSteps_returnPressed();
    void on_colorSteps_returnPressed();
    void on_showParticles_toggled(bool checked);
    void on_continuousAutoAdjust_clicked();
    void on_autoAdjust_clicked();
    void on_updatePeriod_returnPressed();
    void on_colorInterval_returnPressed();
    void on_colorOffset_returnPressed();
    void on_painter_currentIndexChanged(QString);
    void on_showDynamic_stateChanged(int);
    void on_showStatic_stateChanged(int);
    void on_cellsSize_returnPressed();
    void on_allView_clicked();
    void on_showColorBar_toggled(bool checked);
    void on_colorComponent_returnPressed();
    void on_illumination_toggled(bool checked);
private slots:
    void on_showAxis_toggled(bool checked);
    void on_tx_returnPressed();
    void on_ty_returnPressed();
    void on_tz_returnPressed();
    void on_show2ndDistribution_toggled(bool checked);
    void on_configurePainter_clicked();
    void on_showIsolines_toggled(bool checked);
    void on_heightmap_toggled(bool checked);
    void on_showForces_toggled(bool checked);
};

#endif // VIEW_H
