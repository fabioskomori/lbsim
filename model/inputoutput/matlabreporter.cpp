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

#include "matlabreporter.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/basecell.h"
#include "../../model/math/myvector3d.h"
#include <fstream>
#include <QString>

using std::fstream;

MATLABReporter::MATLABReporter() {
}

void MATLABReporter::streamlines(Grid *grid, QString &fileName, QString start) {
    fstream fs;
    fs.open(fileName.toStdString().c_str(), fstream::out);
    fs << "[sx,sy] = " << start.toStdString() << ";\n";
    fs << "u = [\n";
    for (int i = 0; i < grid->getConfig()->getWidth(); i++) {
        fs << grid->getGrid(0, i, 0)->getU(0).getX() << " ";
    }
    for (int j = 1; j < grid->getConfig()->getHeight(); j++) {
        fs << ";\n";
        for (int i = 0; i < grid->getConfig()->getWidth(); i++) {
            fs << grid->getGrid(j, i, 0)->getU(0).getX() << " ";
        }
    }
    fs << "];\n";
    fs << "v = [\n";
    for (int i = 0; i < grid->getConfig()->getWidth(); i++) {
        fs << grid->getGrid(0, i, 0)->getU(0).getY() << " ";
    }
    for (int j = 1; j < grid->getConfig()->getHeight(); j++) {
        fs << ";\n";
        for (int i = 0; i < grid->getConfig()->getWidth(); i++) {
            fs << grid->getGrid(j, i, 0)->getU(0).getY() << " ";
        }
    }
    fs << "];\n";
    fs << "h = streamline(u,v,sx,sy);\n";
    fs << "view(2);\n";
    fs.close();
}
