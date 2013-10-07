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

#include "console.h"
#include "ui_console.h"
#include "lbmainwindow.h"
#include "painter/camera.h"
#include "command/command.h"
#include "command/commandpool.h"

ConsoleDialog::ConsoleDialog(Grid *grid, LBMainWindow *mainWindow, QWidget *parent) : QDialog(parent), ui(new Ui::ConsoleDialog) {
    ui->setupUi(this);
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    this->grid = grid;
    this->mainWindow = mainWindow;
}

ConsoleDialog::~ConsoleDialog() {
    delete ui;
}

void ConsoleDialog::log(QString string) {
    ui->output->appendPlainText(string.append("\n"));
}

void ConsoleDialog::on_input_returnPressed() {
    QStringList tokens = ui->input->text().split(' ');
    if (ui->input->text() == "") {
        std::list<Command*> *commands = CommandPool::list();
        ui->output->appendPlainText(QString("clear"));
        for (std::list<Command*>::iterator command = commands->begin(); command != commands->end(); command++) {
            ui->output->appendPlainText((*command)->help());
        }
        ui->output->appendPlainText("");
        /*ui->output->appendPlainText(QString("clear").append("\n")
                                    .append("density <x> <y> {<z>}").append("\n")
                                    .append("drag <i>").append("\n")
                                    .append("get <x> <y> <z>").append("\n")
                                    .append("painter <type>").append("\n")
                                    .append("rotate <roll> <pitch> <yaw>").append("\n")
                                    .append("set density <x> <y> <value> {<x2> <y2> <value2>}").append("\n")
                                    .append("start").append("\n")
                                    .append("stop").append("\n")
                                    .append("translate <dx> <dy>").append("\n")
                                    .append("velocity <x> <y> {<z>}").append("\n")
                                    .append("view").append("\n")
                                    .append("zoom <value>").append("\n"));*/
    } else if (ui->input->text() == "clear") {
        ui->output->setPlainText("");
    } else {
        Command *command = CommandPool::commandByName(tokens.at(0));
        if (command != NULL) {
            for (int i = 1; i < tokens.size(); i++) {
                command->addParameter(tokens.at(i));
            }
            ui->output->appendPlainText(ui->input->text());
            ui->output->appendPlainText(command->execute());
            delete command;
        }
    }
    /*} else if (ui->input->text().startsWith("density ")) {
        BaseCell *cell = grid->getGrid(tokens.at(2).toInt() - 1, tokens.at(1).toInt() - 1, tokens.size() >= 4 ? tokens.at(3).toInt() - 1 : 0);
        if (cell != 0) {
            ui->output->appendPlainText(QString("p = ").append(QString::number(cell->getP(-1))));
        }
    } else if (ui->input->text().startsWith("drag ")) {
        grid->computeDrag();
        ui->output->appendPlainText(QString("drag = ").append(grid->getDrag(tokens.at(1).toInt() - 1).qtString()).append("  |drag| = ").append(QString::number(grid->getDrag(tokens.at(1).toInt() - 1).norm())));
    } else if (ui->input->text().startsWith("get ") && tokens.size() >= 4) {
        QDomDocument document;
        document.appendChild(document.createElement("sussumu"));
        grid->passivate(document, tokens.at(1).toInt() - 1, tokens.at(2).toInt() - 1, tokens.at(3).toInt() - 1);
        ui->output->appendPlainText(document.toString(2));
    } else if (ui->input->text().startsWith("painter ")) {
        mainWindow->getView()->changePainter(tokens.at(1));
    } else if (ui->input->text().startsWith("rotate ")) {
        mainWindow->getView()->setEuler(tokens.at(1).toInt(), tokens.at(2).toInt(), tokens.at(3).toInt());
        mainWindow->getView()->on_roll_returnPressed();
        mainWindow->getLB()->updateGL();
    } else if (ui->input->text().startsWith("set density")) {
        grid->pressure(tokens.at(2).toInt() - 1, tokens.size() > 5 ? tokens.at(5).toInt() - 1 : tokens.at(2).toInt() - 1,
                       tokens.at(3).toInt() - 1, tokens.size() > 6 ? tokens.at(6).toInt() - 1 : tokens.at(3).toInt() - 1,
                       0, 0, tokens.at(4).toDouble(), tokens.size() > 7 ? tokens.at(7).toDouble() : 0);
    } else if (ui->input->text() == "start") {
        mainWindow->on_actionStart_triggered();
    } else if (ui->input->text() == "stop") {
        mainWindow->on_actionStop_triggered();
    } else if (ui->input->text().startsWith("translate ")) {
        mainWindow->getLB()->getPainter()->getCamera()->setTranslation(MyVector3D(tokens.at(1).toDouble() * 0.001, tokens.at(2).toDouble() * 0.001, 0));
        mainWindow->getLB()->updateGL();
    } else if (ui->input->text().startsWith("velocity ")) {
        BaseCell *cell = grid->getGrid(tokens.at(2).toInt() - 1, tokens.at(1).toInt() - 1, tokens.size() >= 4 ? tokens.at(3).toInt() - 1 : 0);
        if (cell != 0) {
            MyVector3D u = cell->getU(-1);
            ui->output->appendPlainText(QString("u = ").append(u.qtString()).append("  |u| = ").append(QString::number(u.norm())));
        }
    } else if (ui->input->text().startsWith("view ")) {
        mainWindow->getView()->changeView(tokens.at(1));
    } else if (ui->input->text().startsWith("zoom ")) {
        mainWindow->getView()->setZoom(tokens.at(1).toDouble());
        mainWindow->getView()->on_zoom_returnPressed();
        mainWindow->getLB()->updateGL();
    }*/
    ui->input->setText("");
}
