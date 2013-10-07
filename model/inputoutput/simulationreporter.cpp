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

#include "simulationreporter.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridsimulation.h"
#include "../../model/latticeboltzmann/moving/particlemanager.h"
#include "../../model/latticeboltzmann/moving/movingparticle.h"
#include "../../model/latticeboltzmann/boundary/dragwallcell.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include <fstream>
#include <QString>
#include <QFile>
#include <QDebug>

using std::fstream;

void SimulationReporter::reportSimulation(Grid *grid, QString &fileName) {
    QFile file(fileName);
    bool exists = file.exists();
    fstream fs;
    fs.open(fileName.toStdString().c_str(), fstream::app | fstream::out | fstream::ate);
    std::list<MovingParticle*>* particles = grid->getParticleManager()->getParticles();
    grid->computeDrag();
    std::list<Vector3i> points = grid->getConfig()->getPointsToReport();
    if (!exists) {
        fs << "iterations,performance,deltaP";
        for (std::list<Vector3i>::iterator point = points.begin(); point != points.end(); point++) {
            fs << ",(" << ((*point).getX()) << ";" << ((*point).getY()) << ";" << ((*point).getZ()) << "),|V|,Vx,Vy,Vz,p,stream,|W|,Wx,Wy,Wz,p1,p10,p11";
        }
        int i = 1;
        for (std::list<MovingParticle*>::iterator particle = particles->begin(); particle != particles->end(); particle++) {
            fs << ",mVx" << i << ",mVy" << i << ",mVz" << i;
            fs << ",mx" << i << ",my" << i << ",mz" << i;
            i++;
        }
        if (particles->size() > 0) {
            fs << ",Vi2/N,ElecPot";
        }
        for (int i = 0; i < grid->getDrags(); i++) {
            fs << ",drag" << i << "x,drag" << i << "y,drag" << i << "z";
        }
        fs << "\n";
    }
    fs << grid->getSimulation()->getIterations() << "," << grid->getSimulation()->getPerformance() << "," << grid->getSimulation()->getDeltaP();
    double* streams = grid->computeStream(0, 0);
    MyVector3D* vorticities = grid->computeVorticity();
    for (std::list<Vector3i>::iterator point = points.begin(); point != points.end(); point++) {
        BaseCell *cell = grid->getGrid((*point).getY() - 1, (*point).getX() - 1, (*point).getZ() - 1);
        if (cell != 0) {
            fs << ",," << cell->getU(1).norm() << "," << cell->getU(1).getX() << "," << cell->getU(1).getY() << "," << cell->getU(1).getZ() << "," << cell->getP(0);
            fs << "," << streams[(*point).getX() - 1 + ((*point).getY() - 1) * grid->getConfig()->getWidth()];
            fs << "," << vorticities[(*point).getX() - 1 + ((*point).getY() - 1) * grid->getConfig()->getWidth()].norm() << "," << vorticities[(*point).getX() - 1 + ((*point).getY() - 1) * grid->getConfig()->getWidth()].getX() << "," << vorticities[(*point).getX() - 1 + ((*point).getY() - 1) * grid->getConfig()->getWidth()].getY() << "," << vorticities[(*point).getX() - 1 + ((*point).getY() - 1) * grid->getConfig()->getWidth()].getZ();
            fs << "," << cell->getP(1) << "," << cell->getP(10) << "," << cell->getP(11);
        } else {
            fs << ",,,,,,,,,,,,,,";
        }
    }
    delete[] streams;
    delete[] vorticities;
    double vi2 = 0;
    double electricPotential = 0;
    for (std::list<MovingParticle*>::iterator particle = particles->begin(); particle != particles->end(); particle++) {
        fs << "," << (*particle)->getVelocity().getX() << "," << (*particle)->getVelocity().getY() << "," << (*particle)->getVelocity().getZ();
        fs << "," << (*particle)->getMassCenter().getX() << "," << (*particle)->getMassCenter().getY() << "," << (*particle)->getMassCenter().getZ();
        vi2 += (*particle)->getVelocity().norm2();
        electricPotential += (*particle)->getElectricPotential();
    }
    if (particles->size() > 0) {
        fs << "," << (vi2 / particles->size()) << "," << (electricPotential * grid->getParticleManager()->getElectrostaticCoefficient() / particles->size());
    }
    for (int i = 0; i < grid->getDrags(); i++) {
        fs << "," << grid->getDrag(i).getX() << "," << grid->getDrag(i).getY() << "," << grid->getDrag(i).getZ();
    }
    fs << "\n";
    fs.close();
}
