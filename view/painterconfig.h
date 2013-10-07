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

#ifndef PAINTERCONFIG_H
#define PAINTERCONFIG_H

#include <QDialog>
class StreamLineGenerator;
class Painter;
class LBWidget;

namespace Ui {
    class PainterConfig;
}

class PainterConfig : public QDialog {
    Q_OBJECT
public:
    explicit PainterConfig(QWidget *parent = 0);
    void injectPainter(Painter *painter);
    void injectWidget(LBWidget *widget);
    int getStreamIntegrationX();
    int getStreamIntegrationY();
    ~PainterConfig();
private slots:
    void on_strategy_currentIndexChanged(int index);
    void on_update_clicked();
    void on_equallySpaced_clicked();
    void on_custom_clicked();
    void on_addRow_clicked();
    void on_deleteRow_clicked();
    void on_updateIsolines_clicked();
    void on_streamIntegrationX_currentIndexChanged(int index);
    void on_streamIntegrationY_currentIndexChanged(int index);
    void on_sortIsolines_clicked();

    void on_randomIsolines_clicked();

private:
    Ui::PainterConfig *ui;
    Painter *painter;
    LBWidget *widget;
};

#endif // PAINTERCONFIG_H
