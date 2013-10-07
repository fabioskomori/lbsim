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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QDialog>
#include <QList>
#include <QFuture>
class QXmlStreamWriter;
class QXmlStreamReader;

namespace Ui {
    class Scheduler;
}

class Command;
class LBMainWindow;

class Scheduler : public QDialog {
    Q_OBJECT
public:
    explicit Scheduler(LBMainWindow *mainWindow, QWidget *parent = 0);
    ~Scheduler();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    void wait();
    bool isRunning();
    int iterationsMissing();
public slots:
    void on_commands_currentIndexChanged(const QString &arg1);
    void on_scheduled_currentRowChanged(int currentRow);
    void on_up_clicked();
    void on_down_clicked();
    void on_remove_clicked();
    void on_run_clicked();
    void on_stop_clicked();
    void on_add_clicked();
    void on_loopAdd_clicked();
    void on_loopValues_currentRowChanged(int currentRow);
    void on_loopRemove_clicked();
private:
    void execute();
    Ui::Scheduler *ui;
    QList<Command*> commands;
    QStringList loopValues;
    QFuture<void> future;
    Command *currentCommand;
    LBMainWindow *mainWindow;
    bool running;
    int currentIteration;
};

#endif // SCHEDULER_H
