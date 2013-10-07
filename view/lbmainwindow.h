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

#ifndef LBMAINWINDOW_H
#define LBMAINWINDOW_H

class Info;
class View;
class Colors;
class Chart;
class View2;
class About;
class Particles;
class ConsoleDialog;
class Scheduler;
class ScrollDecorator;
class LBWidget;
class Results;
class Regression;
class Changes;
class CodeEditor;
class Interpolation;
//class OpenCL;
class Animation;
class Parameters2;
class Mesher;
class MoreInfo;
class ImageProcessing;
class Geometry;
#include <QMainWindow>
class About;
#include <QDir>

namespace Ui {
    class LBMainWindow;
}

class LBMainWindow : public QMainWindow {
    Q_OBJECT
public:
    LBMainWindow(QWidget *parent = 0);
    ~LBMainWindow();
    View* getView();
    LBWidget* getLB();
    Results* getResults();
    Info* getInfo();
    //OpenCL* getOpenCL();
    Parameters2* getParameters();
    QString getVersion();
    QString getLastFileName();
    void setLastFileName(QString filename);
    void saveAll2(QString filename, int version = 2);
    void startServer(QString folder);
    bool serverIsRunning();
    void showMoreInfo();
protected:
    void changeEvent(QEvent *e);
private:
    void execute();
    void createView();
    void createInfo();
    void createChart();
    Ui::LBMainWindow *ui;
    LBWidget *widget;
    Info *info;
    View *view;
    Colors *colors;
    std::list<Chart*> charts;
    View2 *view2;
    About *about;
    Results *results;
    Particles *particles;
    ConsoleDialog *console;
    Scheduler *scheduler;
    Regression *regression;
    Changes *changes;
    CodeEditor *codeEditor;
    Interpolation *interpolation;
    Mesher *mesher;
    //OpenCL *opencl;
    Animation *animation;
    Parameters2 *parameters;
    MoreInfo *moreInfo;
    ImageProcessing *imageProcessing;
    Geometry *geometry;
    QString lastFileName;
    int versionLoaded;
    bool serverRunning;
    QDir serverFolder;
public slots:
    void loadAll2(int version = 2);
    void load();
    void on_actionConsole_triggered();
    void on_actionForces_triggered();
    void on_actionParticles_2_triggered();
    void on_actionDynamic_triggered();
    void on_actionStatic_triggered();
    void on_actionAxis_triggered();
    void on_actionDetail_triggered();
    void on_actionArrow_triggered();
    void on_actionPressure_triggered();
    void on_actionVelocity_triggered();
    void on_actionParticles_triggered();
    void on_actionAbout_triggered();
    void on_actionView2_triggered();
    void on_actionChart_triggered();
    void on_actionColors_triggered();
    void on_actionInfo_triggered();
    void on_actionView_triggered();
    void on_actionNext_triggered();
    void on_actionReset_triggered();
    void on_actionStop_triggered();
    void on_actionLoad_triggered();
    void on_actionStart_triggered();
    void on_actionSave_Image_triggered();
    void on_actionReload_All_triggered();
    void on_actionResults_triggered();
    void on_actionConvert_lb2_to_lb_triggered();
    void on_actionConvert_lb_to_lb2_triggered();
    void on_actionLoad_All_2_testing_triggered();
    void on_actionSave_All_2_testing_triggered();
    void on_actionScheduler_triggered();
    void on_actionStart_Server_triggered();
    void on_actionStop_Server_triggered();
private slots:
    void on_actionRegression_triggered();
    void on_actionChanges_triggered();
    void on_action_lb_lb2_Editor_triggered();
    void on_actionInterpolation_triggered();
    void on_actionOpenCL_triggered();
    void on_actionAnimation_triggered();
    void on_actionModel_and_Parameters_triggered();
    void on_actionSave_triggered();
    void on_actionSave_x2_triggered();
    void on_actionSave_VTK_triggered();
    void on_actionView_2_triggered();
    void on_actionInfo_2_triggered();
    void on_actionMesher_triggered();
    void on_actionSave_All_v3_triggered();
    void on_actionLoad_All_v3_triggered();
    void on_actionImage_Processing_triggered();
    void on_actionGeometry_triggered();
};

#endif // MAINWINDOW_H
