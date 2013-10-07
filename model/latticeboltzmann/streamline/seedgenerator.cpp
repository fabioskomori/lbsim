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

#include "seedgenerator.h"
#include "../../../model/latticeboltzmann/lbgrid.h"
#include "../../../model/latticeboltzmann/gridconfig.h"
#include "streamlinecurve.h"
#include "streamlinealgorithm.h"
#include "../../../model/math/myvector3d.h"
#include "../../../model/latticeboltzmann/streamline/euler.h"
#include "nextcellevalutor.h"

SeedGenerator::SeedGenerator(Grid *grid) : StreamLineGenerator(grid) {
    properties["minx"] = "0";
    properties["maxx"] = "0";
    properties["miny"] = "0";
    properties["maxy"] = "0";
    properties["minz"] = "0";
    properties["maxz"] = "0";
    properties["forward"] = "100";
    properties["backward"] = "100";
    properties["eulerIntegrationStep"] = "1";
    properties["spacing"] = "10";
    algorithm = new NextCellEvalutor();
}

SeedGenerator::~SeedGenerator() {
    delete algorithm;
}

void SeedGenerator::propertySetted(QString name, QString value) {
    if (name == "eulerIntegrationStep") {
        Euler *euler = dynamic_cast<Euler*>(algorithm);
        if (euler != 0) {
            euler->setIntegrationStep(value.toDouble());
        }
    }
}

std::list<StreamLineCurve*>* SeedGenerator::generate() {
    std::list<StreamLineCurve*>* result = new std::list<StreamLineCurve*>();
    for (int x = properties["minx"].toInt(); x <= (properties["maxx"].toInt() == 0 ? grid->getConfig()->getWidth() : properties["maxx"].toInt()); x += properties["spacing"].toInt()) {
        for (int y = properties["miny"].toInt(); y <= (properties["maxy"].toInt() == 0 ? grid->getConfig()->getWidth() : properties["maxy"].toInt()); y += properties["spacing"].toInt()) {
            for (int z = properties["minz"].toInt(); z <= (properties["maxz"].toInt() == 0 ? grid->getConfig()->getWidth() : properties["maxz"].toInt()); z += properties["spacing"].toInt()) {
                StreamLineCurve *curve = new StreamLineCurve();
                MyVector3D point = MyVector3D(x, y, z);
                curve->getPoints()->push_back(point);
                for (int i = 0; i < properties["forward"].toInt(); i++) {
                    point = algorithm->nextPoint(point, grid);
                    curve->getPoints()->push_back(point);
                }
                result->push_back(curve);
            }
        }
    }
    return result;
}
