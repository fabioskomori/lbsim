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

#ifndef GRIDCONFIG_H
#define GRIDCONFIG_H

#define OPEN_EQUILIBRIUM 1
#define OPEN_EXTRAPOLATION 2
#define OPEN_ZOUHE 3

#define SOURCE_EQUILIBRIUM 1
#define SOURCE_FIXED 2
#define SOURCE_ZOUHE 3

#define MC_SC 1
#define MC_RK 2
#define MC_FE 3
#define MC_PS 4
#define MC_DEPOSITION 5

class Grid;
class QXmlStreamWriter;
class QXmlStreamReader;
class Force;
#include "../../model/math/myvector3d.h"
#include "../../model/math/vector3i.h"
#include <list>

class GridConfig {
public:
    GridConfig(Grid *grid);
    ~GridConfig();
    int getWidth();
    int getHeight();
    int getLength();
    bool contains(double x, double y, double z);
    int getTotalSites();
    void setDimension(int width, int height, int length);
    int getModel();
    void setModel(int model);
    double* getEpsilons();
    void setEpsilon(int index, double epsilon);
    double getMinP0();
    void setMinP0(double minP0);
    double getMaxP0();
    void setMaxP0(double maxP0);
    double randomP();
    double getSourceVelocity(int index);
    void setSourceVelocity(int index, double velocity);
    double getSourcePressure(int index);
    void setSourcePressure(int index, double pressure);
    int getSourcePeriod(int index);
    void setSourcePeriod(int index, int period);
    double getSourceAmplitude(int index);
    void setSourceAmplitude(int index, double sourceAmplitude);
    int getMaxIterations();
    void setMaxIterations(int maxIterations);
    bool getProcessNeighbors();
    void setProcessNeighbors(bool processNeighbors);
    bool getPeriodicBoundaries();
    void setPeriodicBoundaries(bool periodicBoundaries);
    std::list<Force*>* getForces();
    double getGravity();
    void setGravity(double gravity);
    void getMultiphase(double *g, double *psi0, double *p0, double *gads);
    void setMultiphase(double g, double psi0, double p0, double gads);
    void getMulticomponent(int *quantity, double *g, double *gads1, double *gads2, int *type);
    void setMulticomponent(int quantity, double g, double gads1, double gads2, int type);
    void setParticleRelativeDensity(double relativeDensity);
    void setParticleTemperatureCoefficient(double temperatureCoefficient);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    void setOpen(int open);
    void setSource(int source);
    int getOpen();
    int getSource();
    MyVector3D getWallVelocity();
    void setWallVelocity(MyVector3D wallVelocity);
    int getWallPeriod();
    void setWallPeriod(int wallPeriod);
    double getSolidDensity();
    void setSolidDensity(double solidDensity, double variation);
    double getRkA();
    void setRkA(double rkA);
    double getRkBeta();
    void setRkBeta(double rkBeta);
    double getShallowG();
    void setShallowG(double shallowG);
    double getShallowNB();
    void setShallowNB(double shallowNB);
    double getDensity1();
    void setDensity1(double density1);
    double getDensity2();
    void setDensity2(double density2);
    bool getEntropic();
    void setEntropic(bool entropic);
    double getPassiveScalarGravity();
    void setPassiveScalarGravity(double passiveScalarGravity);
    double getThermal(int index);
    void setThermal(int index, double value);
    double getStopCriterion();
    void setStopCriterion(double stopCriterion);
    bool getMulticoreSupport();
    void setMulticoreSupport(bool multicoreSupport);
    double getSmagorinsky();
    void setSmagorinsky(double smagorinsky);
    double getSlipWallCoefficient();
    void setSlipWallCoefficient(double slipWallCoefficient);
    std::list<Vector3i> getPointsToReport();
    void addPointToReport(Vector3i pointToReport);
    void clearPointsToReport();
    bool getDisableCharts();
    void setDisableCharts(bool disableCharts);
    double getThermalExpansion();
    void setThermalExpansion(double thermalExpansion);
    double getDepositionRate();
    void setDepositionRate(double depositionRate);
    int getVelocityOrder();
    void setVelocityOrder(int velocityOrder);
    double getReflectWithFactor();
    void setReflectWithFactor(double reflectWithFactor);
    void addForce(Force *force);
    void setNanoValues(double nanoFraction, double nanoDensity, double nanoThermalExpansion, double nanoThermalConductivity, double nanoHeatCapacitance);
    void getNanoValues(double *nanoFraction, double *nanoDensity, double *nanoThermalExpansion, double *nanoThermalConductivity, double *nanoHeatCapacitance);
    double getNanoEpsilon0();
    double getNanoEpsilon1();
    double getNanoBeta();
    double getNanoDensity();
    void setKorner(bool korner);
    bool getKorner();
private:
    void removeForce(std::type_info toRemove);
    void updateNano();
    int WIDTH, HEIGHT, LENGTH;
    int model;
    double epsilons[3];
    double minP0, maxP0;
    double sourceVelocity[3];
    double sourcePressure[3];
    int sourcePeriod[3];
    double sourceAmplitude[3];
    MyVector3D wallVelocity;
    int maxIterations;
    bool processNeighbors, periodicBoundaries;
    Grid *grid;
    double gravity;
    double mpG, mpPsi0, mpP0, mpGads;
    int mcQuantity, mcType;
    double mcG, mcGads1, mcGads2;
    int open, source;
    int wallPeriod;
    double solidDensity;
    double rkA, rkBeta;
    double shallowG, shallowNB;
    double density1, density2;
    bool entropic;
    double passiveScalarGravity;
    double thermal[3];
    double thermalExpansion;
    double stopCriterion;
    bool multicoreSupport;
    double smagorinsky;
    double slipWallCoefficient;
    std::list<Vector3i> pointsToReport;
    bool disableCharts;
    double depositionRate;
    int velocityOrder;
    std::list<Force*> *forces;
    double reflectWithFactor;
    double nanoFraction, nanoDensity, nanoThermalExpansion, nanoThermalConductivity, nanoHeatCapacitance;
    double nanoEpsilon0, nanoEpsilon1, nanoBeta;
    bool korner;
};

#endif // GRIDCONFIG_H
