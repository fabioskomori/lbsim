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

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QMainWindow>

namespace Ui {
    class CodeEditor;
}

class CodeEditor : public QMainWindow {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = 0);
    ~CodeEditor();
private slots:
    void on_actionLoad_lb2_triggered();
    void on_actionLoad_lb_triggered();
    void on_actionSave_lb_triggered();
    void on_actionSave_lb2_triggered();
private:
    Ui::CodeEditor *ui;
};

#endif // CODEEDITOR_H
