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

#include "codeeditor.h"
#include "ui_codeeditor.h"
#include <QFileDialog>

CodeEditor::CodeEditor(QWidget *parent) : QMainWindow(parent), ui(new Ui::CodeEditor) {
    ui->setupUi(this);
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
}

CodeEditor::~CodeEditor() {
    delete ui;
}

void CodeEditor::on_actionLoad_lb2_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load .lb2", "", "*.lb2");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        ui->text->setPlainText(QString(qUncompress(file.readAll())));
        file.close();
    }
}

void CodeEditor::on_actionLoad_lb_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load .lb", "", "*.lb");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        ui->text->setPlainText(QString(file.readAll()));
        file.close();
    }
}

void CodeEditor::on_actionSave_lb_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save .lb", "", "*.lb");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        file.write(ui->text->toPlainText().toStdString().c_str());
        file.close();
    }
}

void CodeEditor::on_actionSave_lb2_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save .lb2", "", "*.lb2");
    if (fileName != NULL && !fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        file.write(qCompress(QByteArray(ui->text->toPlainText().toStdString().c_str())));
        file.close();
    }
}
