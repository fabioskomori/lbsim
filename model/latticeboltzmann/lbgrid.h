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

#ifndef LBGRID_H
#define LBGRID_H

#define CPU 1
#define OPENCL_CPU 2
#define OPENCL_GPU 3

#include <QList>
class QXmlStreamWriter;
class QXmlStreamReader;
class MyVector3D;
#include <map>
class LatticeBoltzmannOpenCL;
class BaseCell;
class GridConfig;
class GridSimulation;
class Listener;
class MovingParticle;
class Parameters;
class ParticleManager;
class VelocityField;
class LatticeBoltzmannOpenCL;
class LBOCL;
class OpenCLDevice;
class Allocation;
class KornerImplementation;

class Grid {
public:
    Grid();
    ~Grid();
    void init(int width, int height, int length, bool dontDelete = false);
    void load(QString *data, int width, int height, int length);
    void alter(QString type, int minx, int maxx, int miny, int maxy, int minz, int maxz, bool seeNeighbors = false);
    void resize(int factor);
    void pressure(int minx, int maxx, int miny, int maxy, int minz, int maxz, double p1, double p2);
    void porous(int percent);
    int update(int steps);
    void reset();
    void optimize();
    void addListener(int id, Listener *listener);
    void removeListener(int id);
    GridConfig* getConfig();
    GridSimulation* getSimulation();
    BaseCell* getGrid(int x, int y, int z);
    void setGrid(int x, int y, int z, BaseCell *lattice, bool remove = true, bool additionalProcessing = true);
    void deleteLattice(int i, int j, int k);
    void setLattice(int i, int j, int k, BaseCell *lattice);
    BaseCell* newCell(int i, int j, int k, MyVector3D velocity);
    ParticleManager* getParticleManager();
    BaseCell** getNeighbors(int x, int y, int z);
    Parameters* getParameters();
    VelocityField* getVelocityField();
    void passivate(QXmlStreamWriter &writer, int version, QString fileName, int x = -1, int y = -1, int z = -1);
    void activate(QXmlStreamReader &reader, int version, QString fileName);
    BaseCell* activateCell(QXmlStreamReader &reader);
    void computeDrag();
    MyVector3D getDrag(int index);
    MyVector3D* computeVorticity();
    double* computeStream(int dx, int dy);
    int getDrags();
    //void setOpenclType(int openclType, bool doublePrecision, std::list<OpenCLDevice*> *devices);
    void setOpenclUpdate(int openclUpdate);
    int getOpenclUpdate();
    bool isRunning();
    void processNeighbors();
    //LBOCL* getOpenCL();
    void randomP();
    MyVector3D getFlux(int x, int y, int z);
    MyVector3D* getFlux();
    KornerImplementation* getKorner();
protected:
private:
    //LBOCL *opencl;
    void process(int stage);
    void notifyListeners(int event);
    double partialDerivative(char vAxis, char axis, int x, int y, int z);
    BaseCell **grid, **neighborsCache;
    QList<int> is;
    GridConfig *config;
    GridSimulation *simulation;
    ParticleManager *particleManager;
    Parameters *parameters;
    VelocityField *velocityField;
    std::map<int, MyVector3D> drag;
    int openclType, openclUpdate, lastOpenclUpdate;
    std::map<int, Listener*> listeners;
    MyVector3D *fluxes;
    bool fluxCalculated;
    KornerImplementation *korner;
};

#endif // GRID_H
