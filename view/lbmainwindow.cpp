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

#include "lbmainwindow.h"
#include "ui_lbmainwindow.h"
#include "info.h"
#include "view.h"
#include "colors.h"
#include "chart.h"
#include "view2.h"
#include "about.h"
#include "particles.h"
#include "changes.h"
#include "codeeditor.h"
#include "interpolation.h"
//#include "opencl.h"
#include "parameters2.h"
#include "util/scrolldecorator.h"
#include "util/singleton.h"
#include "lbwidget.h"
#include "mesher.h"
#include <fstream>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include "../model/inputoutput/loaderfromtext.h"
#include "../model/inputoutput/loaderfromimage.h"
#include "../model/inputoutput/vtkreporter.h"
#include <QShortcut>
#include "painter/painter.h"
#include "results.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtCore>
#else
#include <QtConcurrent/QtConcurrent>
#endif
#include "animation.h"
#include "moreinfo.h"
#include "imageprocessing.h"
#include "geometry.h"
#include "ui_parameters2.h"
#include "immersed.h"

using namespace std;

QString LBMainWindow::getVersion() {
    return "";
}

LBMainWindow::LBMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::LBMainWindow) {
    ui->setupUi(this);
    widget = new LBWidget();
    this->setCentralWidget(widget);
    colors = new Colors(widget, this);
    widget->getPainter()->injectColors(colors);
    //addDockWidget(Qt::BottomDockWidgetArea, colors);
    info = 0;
    view = 0;
    createInfo();
    createView();
    //chart = new Chart(this);
    //info->inject(chart);
    info->inject(widget);
    results = new Results(widget->getPainter(), this);
    //widget->injectChart(chart);
    widget->injectView(view);
    widget->injectResults(results);
    widget->injectMainWindow(this);
    view2 = new View2(widget, this);
    about = new About(this, this);
    particles = new Particles(widget, this);
    changes = new Changes(this);
    codeEditor = new CodeEditor(this);
    interpolation = new Interpolation(widget->getPainter()->getGrid(), this);
    //opencl = new OpenCL(widget->getPainter()->getGrid(), this);
    animation = new Animation(this);
    animation->inject(view);
    animation->inject(view2);
    parameters = new Parameters2(this);
    parameters->inject(widget->getPainter());
    setStatusBar(0);
    setWindowTitle(QString("LBSim ").append(getVersion()));
    versionLoaded = 0;
    mesher = new Mesher(this);
    mesher->injectWidget(widget);
    mesher->injectParameters(parameters);
    moreInfo = new MoreInfo(this);
    moreInfo->inject(widget->getPainter()->getGrid());
    immersed = new Immersed(this);
    immersed->inject(widget);
    imageProcessing = new ImageProcessing(this);
    geometry = new Geometry(this);
    serverRunning = false;
    /*ui->actionStart->setShortcut(QKeySequence(tr("Ctrl+b", "Begin")));
    ui->actionStop->setShortcut(QKeySequence(tr("Ctrl+e", "End")));
    ui->actionNext->setShortcut(QKeySequence(tr("Ctrl+n", "Next")));
    ui->actionReset->setShortcut(QKeySequence(tr("Ctrl+r", "Reset")));
    ui->actionLoad->setShortcut(QKeySequence(tr("Ctrl+l", "Load")));
    ui->actionLoad_All->setShortcut(QKeySequence(tr("Ctrl+Shift+L", "Load All")));
    ui->actionSave->setShortcut(QKeySequence(tr("Ctrl+s", "Save")));
    ui->actionSave_All->setShortcut(QKeySequence(tr("Ctrl+Shift+s", "Save All")));
    ui->actionDraw_2->setShortcut(QKeySequence(tr("Ctrl+1", "Draw")));
    ui->actionModel->setShortcut(QKeySequence(tr("Ctrl+2", "Model")));
    ui->actionParameters->setShortcut(QKeySequence(tr("Ctrl+3", "Config")));
    ui->actionConditions->setShortcut(QKeySequence(tr("Ctrl+4", "Conditions")));
    ui->actionPhysics->setShortcut(QKeySequence(tr("Ctrl+5", "Physics")));
    ui->actionColors->setShortcut(QKeySequence(tr("Ctrl+6", "Colors")));
    ui->actionView2->setShortcut(QKeySequence(tr("Ctrl+7", "View2")));
    ui->actionParticles->setShortcut(QKeySequence(tr("Ctrl+8", "Particles")));
    ui->actionChart->setShortcut(QKeySequence(tr("Ctrl+9", "Chart")));
    ui->actionView->setShortcut(QKeySequence(tr("Ctrl+w", "View")));
    ui->actionInfo->setShortcut(QKeySequence(tr("Ctrl+i", "Info")));
    ui->actionHelp->setShortcut(QKeySequence(tr("Ctrl+h", "Help")));
    ui->actionVelocity->setShortcut(QKeySequence(tr("Alt+1", "Velocity")));
    ui->actionPressure->setShortcut(QKeySequence(tr("Alt+2", "Pressure")));
    ui->actionArrow->setShortcut(QKeySequence(tr("Alt+3", "Arrow")));
    ui->actionDetail->setShortcut(QKeySequence(tr("Alt+4", "Detail")));
    ui->actionAxis->setShortcut(QKeySequence(tr("Alt+a", "Axis")));
    ui->actionStatic->setShortcut(QKeySequence(tr("Alt+s", "Static")));
    ui->actionDynamic->setShortcut(QKeySequence(tr("Alt+d", "Dynamic")));
    ui->actionParticles_2->setShortcut(QKeySequence(tr("Alt+p", "Particles")));*/
}

LBMainWindow::~LBMainWindow() {
    delete ui;
    delete widget;
    delete view;
    delete info;
    delete colors;
    delete view2;
    delete particles;
    delete results;
    delete about;
    delete changes;
    delete codeEditor;
    delete interpolation;
    //delete opencl;
    delete animation;
    delete parameters;
    delete moreInfo;
    delete imageProcessing;
    delete geometry;
    delete immersed;
}

Parameters2* LBMainWindow::getParameters() {
    return parameters;
}

View* LBMainWindow::getView() {
    return view;
}

LBWidget* LBMainWindow::getLB() {
    return widget;
}

Results* LBMainWindow::getResults() {
    return results;
}

Info* LBMainWindow::getInfo() {
    return info;
}

/*OpenCL* LBMainWindow::getOpenCL() {
    return opencl;
}*/

void LBMainWindow::on_actionVelocity_triggered() {
    view->changePainter("velocity");
}

void LBMainWindow::on_actionPressure_triggered() {
    view->changePainter("pressure");
}

void LBMainWindow::on_actionArrow_triggered() {
    view->changePainter("arrow");
}

void LBMainWindow::on_actionDetail_triggered() {
    view->changePainter("detail");
}

void LBMainWindow::on_actionForces_triggered() {
    view->changePainter("forces");
}

void LBMainWindow::on_actionAxis_triggered() {
    view->changeView("axis");
}

void LBMainWindow::on_actionStatic_triggered() {
    view->changeView("static");
}

void LBMainWindow::on_actionDynamic_triggered() {
    view->changeView("dynamic");
}

void LBMainWindow::on_actionParticles_2_triggered() {
    view->changeView("particles");
}

void LBMainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void LBMainWindow::on_actionStart_triggered() {
    widget->setRestartTimer(true);
    widget->start();
}

void LBMainWindow::setLastFileName(QString filename) {
    lastFileName = filename;
}

void LBMainWindow::load() {
    QString fileName = lastFileName;
    bool loaded = false;
    if (fileName.endsWith(".png")) {
        LoaderFromImage::load(widget->getPainter()->getGrid(), fileName, parameters->getUI()->activateKorner->isChecked() ? "korner" : "");
        loaded = true;
    } else if (fileName.endsWith(".txt") || fileName.endsWith(".lb")) {
        LoaderFromText::load(widget->getPainter()->getGrid(), fileName);
        loaded = true;
    }
    if (loaded) {
        info->setFile(fileName.remove(QRegExp(".*/")));
        parameters->updateModel();
        parameters->apply();
        /*if (fileName.endsWith(".png")) {
            LoaderFromImage::config(widget->getPainter()->getGrid(), fileName);
        }*/
        parameters->load();
        widget->getPainter()->updateDisplayList(false);
        widget->getPainter()->updateCellsList();
        widget->updateGL();
    }
}

void LBMainWindow::on_actionLoad_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load", "", "*.png");
    if (fileName != NULL && !fileName.isEmpty()) {
        lastFileName = fileName;
        load();
    }
}

void LBMainWindow::on_actionStop_triggered() {
    widget->setRestartTimer(false);
    widget->stop();
}

void LBMainWindow::on_actionReset_triggered() {
    widget->reset();
    widget->getPainter()->getGrid()->reset();
    info->logPerformance(0, 0, 0);
    widget->updateGL();
    for (std::list<Chart*>::iterator chart = charts.begin(); chart != charts.end(); chart++) {
        (*chart)->reset();
    }
}

void LBMainWindow::on_actionNext_triggered() {
    widget->setRestartTimer(false);
    widget->run();
}

void LBMainWindow::on_actionView_triggered() {
    if (!view->isVisible()) {
        this->createView();
    } else {
        view->close();
    }
}

void LBMainWindow::createView() {
    if (view == 0) {
        view = new View(widget, this);
        view->injectColors(colors);
        widget->getPainter()->injectView(view);
        this->addDockWidget(Qt::LeftDockWidgetArea, view);
    } else {
        view->setVisible(true);
    }
    //view->setFloating(true);
}

void LBMainWindow::createInfo() {
    if (info == 0) {
        info = new Info(widget->getPainter()->getGrid(), this);
        widget->getPainter()->getGrid()->addListener(1, info);
        widget->getPainter()->setListener(info);
        this->addDockWidget(Qt::RightDockWidgetArea, info);
        widget->setInfo(info);
    } else {
        info->setVisible(true);
    }
    //info->setFloating(true);
}

void LBMainWindow::on_actionInfo_triggered() {
    if (!info->isVisible()) {
        createInfo();
    } else {
        info->close();
    }
}

void LBMainWindow::on_actionColors_triggered() {
    colors->show();
}

void LBMainWindow::on_actionChart_triggered() {
    Chart *chart = new Chart(this);
    charts.push_back(chart);
    widget->injectChart(chart);
    info->inject(chart);
    chart->show();
}

void LBMainWindow::on_actionResults_triggered() {
    results->show();
}

void LBMainWindow::on_actionSave_All_2_testing_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save All", "", "*.lb2");
    if (fileName != NULL && !fileName.isEmpty()) {
        saveAll2(fileName);
    }
}

void LBMainWindow::saveAll2(QString fileName, int version) {
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartElement("sussumu");
    widget->getPainter()->getGrid()->passivate(writer, version, fileName);
    view->save(writer);
    //scheduler->passivate(writer);
    // for backward compatibility
    writer.writeStartElement("scheduler");
    writer.writeEndElement();
    info->save(writer);
    results->save(writer);
    colors->save(writer);
    //regression->save(writer);
    // for backward compatibility
    writer.writeStartElement("regression");
    writer.writeEndElement();
    interpolation->save(writer);
    //opencl->save(writer);
    writer.writeEndElement();
    file.close();
    if (ui->actionUse_zipped_lb3->isChecked() && version == 2) {
        file.open(QIODevice::ReadOnly);
        QByteArray byteArray = file.readAll();
        byteArray = qCompress(byteArray);
        file.close();
        file.open(QIODevice::WriteOnly);
        file.write(byteArray);
        file.close();
    }
}

void LBMainWindow::on_actionSave_Image_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "*.png");
    if (fileName != NULL && !fileName.isEmpty()) {
        widget->grabFrameBuffer().save(fileName, "PNG");
    }
}

void LBMainWindow::on_actionReload_All_triggered() {
    switch (versionLoaded) {
    case 2:
        loadAll2();
        break;
    }
}

void LBMainWindow::on_actionLoad_All_2_testing_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load All", "", "*.lb2");
    if (fileName != NULL && !fileName.isEmpty()) {
        lastFileName = fileName;
        loadAll2();
        versionLoaded = 2;
    }
}

void LBMainWindow::loadAll2(int version) {
    if (ui->actionUse_zipped_lb3->isChecked() && version == 2) {
        QFile file(lastFileName);
        file.open(QIODevice::ReadOnly);
        QByteArray byteArray = file.readAll();
        file.close();
        QFile file2(lastFileName + ".tmp");
        file2.open(QIODevice::WriteOnly);
        file2.write(qUncompress(byteArray));
        file2.close();
    }
    QString file2name = ui->actionUse_zipped_lb3->isChecked() && version == 2 ? lastFileName + ".tmp" : lastFileName;
    QFile file2(file2name);
    file2.open(QIODevice::ReadOnly);
    QXmlStreamReader reader(&file2);
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    widget->getPainter()->getGrid()->activate(reader, version, file2name);
    widget->getPainter()->updateDisplayList(false);
    widget->getPainter()->updateCellsList();
    parameters->load();
    particles->load();
    parameters->load();
    for (std::list<Chart*>::iterator chart = charts.begin(); chart != charts.end(); chart++) {
        (*chart)->processGrid(widget->getPainter()->getGrid());
    }
    view->load(reader);
    //scheduler->activate(reader);
    // for backward compatibility
    if (reader.name().toString() == "scheduler") {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
    info->load(reader);
    results->load(reader);
    colors->load(reader);
    //regression->load(reader);
    // for backward compatibility
    if (reader.name().toString() == "regression") {
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
    interpolation->load(reader);
    immersed->sync();
    //opencl->load(reader);
    widget->updateGL();
    file2.close();
    if (ui->actionUse_zipped_lb3->isChecked() && version == 2) {
        file2.remove();
    }
}

void LBMainWindow::on_actionView2_triggered() {
    view2->show();
}

void LBMainWindow::on_actionAbout_triggered() {
    about->show();
}

void LBMainWindow::on_actionParticles_triggered() {
    particles->show();
}

void LBMainWindow::on_actionConvert_lb_to_lb2_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Convert to lb2", "", "*.lb");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QFile file2(fileName.replace(".lb", ".lb2"));
        file2.open(QIODevice::WriteOnly);
        file2.write(qCompress(file.readAll()));
        file.close();
        file2.close();
    }
}

void LBMainWindow::on_actionConvert_lb2_to_lb_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Convert to lb", "", "*.lb2");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QFile file2(fileName.replace(".lb2", ".lb"));
        file2.open(QIODevice::WriteOnly);
        file2.write(qUncompress(file.readAll()));
        file.close();
        file2.close();
    }
}

QString LBMainWindow::getLastFileName() {
    return lastFileName;
}

void LBMainWindow::execute() {
    QMutex mutex;
    QWaitCondition waitCondition;
    while (serverRunning) {
        QStringList filters;
        filters << "*.lb2";
        QStringList files = serverFolder.entryList(filters, QDir::NoFilter, QDir::Time);
        if (!files.isEmpty()) {
            lastFileName = serverFolder.absolutePath() + "/" + files.at(0);
            QMetaObject::invokeMethod(this, "loadAll2");
            while (serverRunning) {
                mutex.lock();
                waitCondition.wait(&mutex, 1000);
                mutex.unlock();
                /*if (!QFile(lastFileName).exists()) {
                    QMetaObject::invokeMethod(scheduler, "on_stop_clicked");
                    break;
                }*/
            }
            if (serverRunning) {
                QFile(lastFileName).remove();
            }
        }
        mutex.lock();
        waitCondition.wait(&mutex, 1000);
        mutex.unlock();
    }
}

void LBMainWindow::on_actionStart_Server_triggered() {
    QString fileName = QFileDialog::getExistingDirectory(this, "Start Server");
    if (fileName != NULL && !fileName.isEmpty()) {
        startServer(fileName);
    }
}

bool LBMainWindow::serverIsRunning() {
    return serverRunning;
}

void LBMainWindow::startServer(QString folder) {
    Singleton::instance()->setFolder(folder + "/");
    serverFolder = QDir(folder);
    serverRunning = true;
    QtConcurrent::run(this, &LBMainWindow::execute);
}

void LBMainWindow::on_actionStop_Server_triggered() {
    serverRunning = false;
}

void LBMainWindow::on_actionChanges_triggered() {
    changes->show();
}

void LBMainWindow::on_action_lb_lb2_Editor_triggered() {
    codeEditor->showMaximized();
}

void LBMainWindow::on_actionInterpolation_triggered() {
    interpolation->show();
}

void LBMainWindow::on_actionOpenCL_triggered() {
    //opencl->show();
}

void LBMainWindow::on_actionAnimation_triggered() {
    animation->show();
}

void LBMainWindow::on_actionModel_and_Parameters_triggered() {
    parameters->show();
}

void LBMainWindow::on_actionSave_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Geometry", "", "*.png");
    if (fileName != NULL && !fileName.isEmpty()) {
        LoaderFromImage::save(widget->getPainter()->getGrid(), fileName);
    }
}

void LBMainWindow::on_actionSave_x2_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Geometry", "", "*.png");
    if (fileName != NULL && !fileName.isEmpty()) {
        LoaderFromImage::save(widget->getPainter()->getGrid(), fileName, 2);
    }
}

void LBMainWindow::on_actionSave_VTK_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export VTK", "", "*.vtu");
    if (fileName != NULL && !fileName.isEmpty()) {
        VTKReporter::exportVTK(widget->getPainter()->getGrid(), fileName);
    }
}

void LBMainWindow::on_actionView_2_triggered() {
    if (view->isVisible()) {
        view->hide();
    } else {
        view->show();
    }
}

void LBMainWindow::on_actionInfo_2_triggered() {
    if (info->isVisible()) {
        info->hide();
    } else {
        info->show();
    }
}

void LBMainWindow::on_actionMesher_triggered() {
    mesher->show();
}

void LBMainWindow::on_actionSave_All_v3_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save All", "", "*.lb3");
    if (fileName != NULL && !fileName.isEmpty()) {
        saveAll2(fileName, 3);
    }
}

void LBMainWindow::on_actionLoad_All_v3_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load All", "", "*.lb3");
    if (fileName != NULL && !fileName.isEmpty()) {
        lastFileName = fileName;
        loadAll2(3);
        versionLoaded = 2;
    }
}

void LBMainWindow::showMoreInfo() {
    moreInfo->show();
}

void LBMainWindow::on_actionImage_Processing_triggered() {
    imageProcessing->showMaximized();
}

void LBMainWindow::on_actionGeometry_triggered() {
    geometry->showMaximized();
}

void LBMainWindow::on_actionExec_2_it_triggered() {
    for (int i = 0; i < 2; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_4_it_triggered() {
    for (int i = 0; i < 4; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_8_it_triggered() {
    for (int i = 0; i < 8; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_16_it_triggered() {
    for (int i = 0; i < 16; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_32_it_triggered() {
    for (int i = 0; i < 32; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_64_it_triggered() {
    for (int i = 0; i < 64; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_128_it_triggered() {
    for (int i = 0; i < 128; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_256_it_triggered() {
    for (int i = 0; i < 256; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_512_it_triggered() {
    for (int i = 0; i < 512; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionExec_1024_it_triggered() {
    for (int i = 0; i < 1024; i++) {
        on_actionNext_triggered();
    }
}

void LBMainWindow::on_actionImmersed_triggered() {
    immersed->show();
}
