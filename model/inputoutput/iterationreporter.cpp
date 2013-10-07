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

#include "iterationreporter.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../model/latticeboltzmann/basecell.h"
#include <fstream>
#include <QString>

using std::fstream;

IterationReporter::IterationReporter(int x, int y, int z) : x(x), y(y), z(z) {
}

void IterationReporter::reportCsv(Grid *grid, QString &fileName) {
    fstream fs;
    fs.open(fileName.toStdString().c_str(), fstream::out);
    double* stream = grid->computeStream(0, 0);
    MyVector3D* vorticities = grid->computeVorticity();
    fs << "x,y,z,|V|,Vx,Vy,Vz,p,stream,|W|,Wx,Wy,Wz,p1,p10,p11\n";
    for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
        for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                if ((x == -1 || j == x) && (y == -1 || i == y) && (z == -1 || k == z)) {
                    BaseCell *empty = (grid->getGrid(i, j, k));
                    if (empty->isFluid()) {
                        fs << (j + 1) << "," << (i + 1) << "," << (k + 1) << "," <<
                                empty->getU(-1).norm() << "," <<
                                empty->getU(-1).getX() << "," <<
                                empty->getU(-1).getY() << "," <<
                                empty->getU(-1).getZ() << "," <<
                                empty->getP(-1) << "," <<
                                stream[j + i * grid->getConfig()->getWidth()] << "," <<
                                vorticities[j + i * grid->getConfig()->getWidth()].norm() << "," <<
                                vorticities[j + i * grid->getConfig()->getWidth()].getX() << "," <<
                                vorticities[j + i * grid->getConfig()->getWidth()].getY() << "," <<
                                vorticities[j + i * grid->getConfig()->getWidth()].getZ() << "," <<
                                empty->getP(1) << "," <<
                                empty->getP(10) << "," <<
                                empty->getP(11) << "\n";
                    }
                }
            }
        }
    }
    delete[] stream;
    delete[] vorticities;
    fs.close();
}
