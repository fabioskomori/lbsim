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

#include "geometry.h"
#include "ui_geometry.h"
#include <QFileDialog>
#include <QtScript>
#include "../model/inputoutput/loaderfromimage.h"
#include <QDebug>

Geometry::Geometry(QWidget *parent) : QMainWindow(parent), ui(new Ui::Geometry) {
    ui->setupUi(this);
    on_actionNew_triggered();
}

Geometry::~Geometry() {
    delete ui;
}

void Geometry::on_actionNew_triggered() {
    ui->code->clear();
    ui->code->appendPlainText("var data = [];");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("function width() {");
    ui->code->appendPlainText("    return 100;");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("function height() {");
    ui->code->appendPlainText("    return 100;");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("function length() {");
    ui->code->appendPlainText("    return 1;");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("function init() {");
    ui->code->appendPlainText("    for (var x = 0; x < width(); x++) {");
    ui->code->appendPlainText("        data[x] = new Array();");
    ui->code->appendPlainText("        for (var y = 0; y < height(); y++) {");
    ui->code->appendPlainText("            data[x][y] = new Array();");
    ui->code->appendPlainText("            for (var z = 0; z < length(); z++) {");
    ui->code->appendPlainText("                data[x][y][z] = \"fluid\";");
    ui->code->appendPlainText("            }");
    ui->code->appendPlainText("        }");
    ui->code->appendPlainText("    }");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("/* possible values: \"fluid\", \"wall\", \"inlet\", \"outlet\" */");
    ui->code->appendPlainText("function create() {");
    ui->code->appendPlainText("    for (var i = 0; i < width(); i++) {");
    ui->code->appendPlainText("        data[i][0][0] = \"wall\";");
    ui->code->appendPlainText("        data[i][height() - 1][0] = \"wall\";");
    ui->code->appendPlainText("    }");
    ui->code->appendPlainText("    for (var i = 0; i < height(); i++) {");
    ui->code->appendPlainText("        data[0][i][0] = \"wall\";");
    ui->code->appendPlainText("        data[width() - 1][i][0] = \"wall\";");
    ui->code->appendPlainText("    }");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("function serialize() {");
    ui->code->appendPlainText("    var result = width() + \"|\" + height() + \"|\" + length();");
    ui->code->appendPlainText("    for (var x = 0; x < width(); x++) {");
    ui->code->appendPlainText("        for (var y = 0; y < height(); y++) {");
    ui->code->appendPlainText("            for (var z = 0; z < length(); z++) {");
    ui->code->appendPlainText("                result += \"|\" + data[x][y][z];");
    ui->code->appendPlainText("            }");
    ui->code->appendPlainText("        }");
    ui->code->appendPlainText("    }");
    ui->code->appendPlainText("    return result;");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("function main() {");
    ui->code->appendPlainText("    init();");
    ui->code->appendPlainText("    create();");
    ui->code->appendPlainText("    return serialize();");
    ui->code->appendPlainText("}");
    ui->code->appendPlainText("");
    ui->code->appendPlainText("main();");
    ui->code->appendPlainText("");
}

void Geometry::run() {
    QScriptEngine engine;
    engine.evaluate(ui->code->toPlainText());
    if (engine.hasUncaughtException()) {
        ui->output->appendPlainText(engine.uncaughtExceptionBacktrace().join("\n"));
    } else {
        QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "*.png");
        if (fileName != NULL && !fileName.isEmpty()) {
            QDateTime begin = QDateTime::currentDateTime();
            engine.setProcessEventsInterval(1000);
            QString temp = engine.evaluate(ui->code->toPlainText()).toString();
            QStringList serialized = temp.split('|');
            int width = serialized.at(0).toInt();
            int height = serialized.at(1).toInt();
            int length = serialized.at(2).toInt();
            for (int z = 0; z < length; z++) {
                QImage image = QImage(width, height, QImage::Format_RGB32);
                for (int x = 0; x < width; x++) {
                    for (int y = 0; y < height; y++) {
                        QString type = serialized.at(3 + x * length * height + y * length + z);
                        if (type == "wall") {
                            image.setPixel(x, y, LoaderFromImage::wall.rgb());
                        } else if (type == "inlet") {
                            image.setPixel(x, y, LoaderFromImage::source.rgb());
                        } else if (type == "outlet") {
                            image.setPixel(x, y, LoaderFromImage::open.rgb());
                        } else {
                            image.setPixel(x, y, QColor(255, 255, 255).rgb());
                        }
                    }
                }
                QString newFileName = z == 0 ? fileName : fileName.left(fileName.length() - 4) + "_" + QString::number(z + 1) + fileName.right(4);
                ui->output->appendPlainText("Creating " + newFileName + "...");
                QCoreApplication::processEvents();
                image.save(newFileName, "PNG");
            }
            ui->output->appendPlainText(QString("It took ").append(QString::number(begin.msecsTo(QDateTime::currentDateTime()))).append(" ms"));
        }
    }
}

void Geometry::on_actionGenerate_triggered() {
    QMetaObject::invokeMethod(this, "run", Qt::QueuedConnection);
}

void Geometry::on_actionSave_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "*.js");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << ui->code->toPlainText();
        file.close();
    }
}

void Geometry::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open", "", "*.js");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        ui->code->clear();
        ui->code->appendPlainText(in.readAll());
        file.close();
    }
}
