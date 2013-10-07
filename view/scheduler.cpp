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

#include "scheduler.h"
#include "ui_scheduler.h"
#include "command/commandpool.h"
#include "command/command.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtCore>
#else
#include <QtConcurrent/QtConcurrent>
#endif
#include "lbmainwindow.h"

Scheduler::Scheduler(LBMainWindow *mainWindow, QWidget *parent) : QDialog(parent), ui(new Ui::Scheduler), mainWindow(mainWindow) {
    ui->setupUi(this);
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    std::list<Command*>* commands = CommandPool::list();
    QString firstCommand = "";
    for (std::list<Command*>::iterator command = commands->begin(); command != commands->end(); command++) {
        ui->commands->addItem((*command)->name());
        if (firstCommand.isEmpty()) {
            firstCommand = (*command)->name();
        }
    }
    on_commands_currentIndexChanged(firstCommand);
    currentCommand = 0;
    loopValues.append("1");
    running = false;
    currentIteration = 0;
}

Scheduler::~Scheduler() {
    delete ui;
}

void Scheduler::on_commands_currentIndexChanged(const QString &name) {
    Command *command = CommandPool::commandByName(name);
    ui->help->setText(command->help());
    ui->parameters->setText("");
    ui->parameters->setFocus();
    delete command;
}

void Scheduler::on_scheduled_currentRowChanged(int currentRow) {
    ui->remove->setEnabled(true);
    ui->up->setEnabled(currentRow != 0);
    ui->down->setEnabled(currentRow < (ui->scheduled->count() - 1));
}

void Scheduler::on_up_clicked() {
    QListWidgetItem *item = ui->scheduled->currentItem();
    int selected = ui->scheduled->currentIndex().row();
    ui->scheduled->insertItem(selected - 1, ui->scheduled->takeItem(selected));
    ui->scheduled->setCurrentItem(item);
    commands.swap(selected, selected - 1);
}

void Scheduler::on_down_clicked() {
    QListWidgetItem *item = ui->scheduled->currentItem();
    int selected = ui->scheduled->currentIndex().row();
    ui->scheduled->insertItem(selected + 1, ui->scheduled->takeItem(selected));
    ui->scheduled->setCurrentItem(item);
    commands.swap(selected, selected + 1);
}

void Scheduler::on_remove_clicked() {
    int selected = ui->scheduled->currentIndex().row();
    delete ui->scheduled->takeItem(selected);
    delete commands.takeAt(selected);
}

void Scheduler::passivate(QXmlStreamWriter &writer) {
    writer.writeStartElement("scheduler");
    writer.writeAttribute("loopSize", QString::number(loopValues.size()));
    for (int i = 0; i < loopValues.size(); i++) {
        writer.writeAttribute(QString("loop").append(QString::number(i)), loopValues.at(i));
    }
    writer.writeAttribute("commandsSize", QString::number(commands.size()));
    for (int i = 0; i < commands.size(); i++) {
        commands.at(i)->passivate(writer);
    }
    writer.writeEndElement();
}

void Scheduler::activate(QXmlStreamReader &reader) {
    ui->scheduled->clear();
    commands.clear();
    if (reader.name().toString() == "scheduler") {
        loopValues.clear();
        ui->loopValues->clear();
        for (int i = 0; i < reader.attributes().value("loopSize").toString().toInt(); i++) {
            loopValues.append(reader.attributes().value(QString("loop").append(QString::number(i))).toString());
            ui->loopValues->addItem(reader.attributes().value(QString("loop").append(QString::number(i))).toString());
        }
        int parametersSize = reader.attributes().value("commandsSize").toString().toInt();
        for (int i = 0; i < parametersSize; i++) {
            Command *command = CommandPool::activate(reader);
            commands.append(command);
            ui->scheduled->addItem(command->toString());
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void Scheduler::execute() {
    for (int i = 0; i < commands.size(); i++) {
        commands.at(i)->live();
    }
    for (currentIteration = 0; currentIteration < loopValues.size(); currentIteration++) {
        bool died = false;
        for (int a = 0; a < commands.size(); a++) {
            /*QMutex mutex;
            mutex.lock();
            QWaitCondition waitCondition;
            waitCondition.wait(&mutex, 1000);
            mutex.unlock();*/
            currentCommand = commands.at(a)->clone();
            currentCommand->setVariable("i", loopValues.at(currentIteration));
            ui->log->setText(QString("Executing: ").append(currentCommand->toString()).append(" with i = ").append(loopValues.at(currentIteration)));
            currentCommand->execute();
            died = currentCommand->dead();
            delete currentCommand;
            if (died) {
                break;
            }
        }
        if (died) {
            break;
        }
    }
}

void Scheduler::wait() {
    future.waitForFinished();
    running = false;
    ui->run->setEnabled(true);
    ui->log->setText("");
    ui->stop->setEnabled(false);
}

void Scheduler::on_run_clicked() {
    ui->log->clear();
    running = true;
    future = QtConcurrent::run(this, &Scheduler::execute);
    ui->run->setEnabled(false);
    ui->stop->setEnabled(true);
    QtConcurrent::run(this, &Scheduler::wait);
}

void Scheduler::on_stop_clicked() {
    running = false;
    currentCommand->die();
    ui->stop->setEnabled(false);
    ui->log->setText("");
    ui->run->setEnabled(true);
    mainWindow->on_actionStop_triggered();
}

void Scheduler::on_add_clicked() {
    Command *command = CommandPool::commandByName(ui->commands->currentText());
    QStringList stringList = ui->parameters->text().split(' ');
    for (int i = 0; i < stringList.size(); i++) {
        if (!stringList.at(i).isEmpty()) {
            command->addParameter(stringList.at(i));
        }
    }
    commands.append(command);
    ui->scheduled->addItem(command->toString());
    ui->parameters->setText("");
}

void Scheduler::on_loopAdd_clicked() {
    loopValues.append(ui->loopValue->text());
    ui->loopValues->addItem(ui->loopValue->text());
    ui->loopValue->setText("");
}

void Scheduler::on_loopValues_currentRowChanged(int) {
    ui->loopRemove->setEnabled(true);
}

void Scheduler::on_loopRemove_clicked() {
    int selected = ui->loopValues->currentIndex().row();
    delete ui->loopValues->takeItem(selected);
    loopValues.removeAt(selected);
}

bool Scheduler::isRunning() {
    return running;
}

int Scheduler::iterationsMissing() {
    if (running) {
        return loopValues.size() - (currentIteration + 1);
    }
    return 0;
}
