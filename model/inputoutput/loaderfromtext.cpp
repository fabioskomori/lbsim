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

#include "loaderfromtext.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../model/latticeboltzmann/boundary/sourcecell.h"
#include "../../model/latticeboltzmann/boundary/opencell.h"
#include "../../model/latticeboltzmann/boundary/wallcell.h"
#include <fstream>
#include <typeinfo>
#include <QString>

using std::fstream;

void LoaderFromText::save(Grid *grid, QString &fileName) {
    fstream fs;
    fs.open(fileName.toStdString().c_str(), fstream::out);
    fs << grid->getConfig()->getWidth() << " " << grid->getConfig()->getHeight() << " " << grid->getConfig()->getLength() << "\n";
    int source = 0, open = 0, wall = 0;
    for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                BaseCell *lattice = grid->getGrid(i, j, k);
                if (typeid (*lattice) == typeid (SourceCell)) {
                    source++;
                } else if (typeid (*lattice) == typeid (OpenCell)) {
                    open++;
                } else if (typeid (*lattice) == typeid (WallCell)) {
                    wall++;
                }
            }
        }
    }
    fs << source << "\n";
    for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                BaseCell *lattice = grid->getGrid(i, j, k);
                if (typeid (*lattice) == typeid (SourceCell)) {
                    fs << j << " " << i << " " << k << "\n";
                }
            }
        }
    }
    fs << open << "\n";
    for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                BaseCell *lattice = grid->getGrid(i, j, k);
                if (typeid (*lattice) == typeid (OpenCell)) {
                    fs << j << " " << i << " " << k << "\n";
                }
            }
        }
    }
    fs << wall << "\n";
    for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                BaseCell *lattice = grid->getGrid(i, j, k);
                if (typeid (*lattice) == typeid (WallCell)) {
                    fs << j << " " << i << " " << k << "\n";
                }
            }
        }
    }
}

void LoaderFromText::load(Grid *grid, QString &fileName) {
    fstream fs;
    fs.open(fileName.toStdString().c_str());
    int height, width, length;
    fs >> width >> height >> length;
    int source, open, wall;
    int x, y, z;
    fs >> source;
    char *data = new char[height * width * length];
    for (int i = 0; i < height * width * length; i++) {
        data[i] = 'x';
    }
    for (int i = 0; i < source; i++) {
        fs >> x >> y >> z;
        data[z + x * length + y * length * width] = 's';
    }
    fs >> open;
    for (int i = 0; i < open; i++) {
        fs >> x >> y >> z;
        data[z + x * length + y * length * width] = 'o';
    }
    fs >> wall;
    for (int i = 0; i < wall; i++) {
        fs >> x >> y >> z;
        data[z + x * length + y * length * width] = 'w';
    }
    grid->load(data, width, height, length);
    delete[] data;
}
