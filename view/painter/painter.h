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

#ifndef PAINTER_H
#define PAINTER_H

#include <QGLWidget>
class Camera;
class Colors;
class Grid;
class Listener;
class MovingCell;
class View;
class BaseCell;
class SomethingMoving;
class QString;
class QColor;
class LBWidget;
#include <list>
class PainterCell;
class StreamLineGenerator;
class IsolineSeeder;
class MyVector3D;

class Painter {
public:
    Painter(LBWidget *widget);
    ~Painter();
    Grid* getGrid();
    void updateDisplayList(bool keepBounds);
    void paint(bool forcePaintAll);
    void paintLight();
    void paintIsolines();
    void paintIsosurfaces();
    void paintStreamLines();
    void paintMulticomponent(bool doPaint = true);
    void paintStreamFunction(bool doPaint = true);
    void paintVorticity(bool doPaint = true);
    void paintVorticityMagnitude(bool doPaint = true);
    void paintVelocity(bool doPaint = true);
    void paintNormalArrow(bool doPaint = true);
    void paintArrow(bool doPaint = true);
    void paintPressure(bool doPaint = true);
    void paintPermeability(bool doPaint = true);
    void paintDetail(bool doPaint = true);
    void paintFlux(bool doPaint = true);
    void paintMacroFlux(bool doPaint = true);
    void paintPathLines();
    void paintForces();
    void paintColorBar();
    void paintPassiveScalar(bool doPaint = true);
    void paintHighlight();
    QString getStrategy();
    Camera* getCamera();
    void setStrategy(QString strategy);
    void setListener(Listener *listener);
    void setVisibility(int minx, int maxx, int miny, int maxy, int minz, int maxz);
    void getVisibility(int *minx, int *maxx, int *miny, int *maxy, int *minz, int *maxz);
    void setMinX(int minx);
    void setMaxX(int maxx);
    void setMinY(int miny);
    void setMaxY(int maxy);
    void setMinZ(int minz);
    void setMaxZ(int maxz);
    void toggleShowSourceOpen();
    void toggleShowFluid();
    void toggleShowParticles();
    void toggleShowAxis();
    void toggleShowColorBar();
    void toggleShow2ndDistribution();
    void toggleShowIsolines();
    void toggleShowHeightmap();
    void toggleShowLight();
    void toggleShowForces();
    void setTransparency(double transparency);
    void setColorRange(double offset, double interval, bool firstTime);
    void setAnalyze(bool x, bool y, bool z);
    //double getCellsSize();
    void setCellsSize(double newCellsSize);
    void setCellsSize2(double newCellsSize);
    bool needsToPaint();
    double getMinimumValue();
    double getMaximumValue();
    double getColorAverage();
    double getColorDelta();
    void autoAdjust();
    void injectColors(Colors *colors);
    void injectView(View *view);
    void toggleContinuousAutoAdjust();
    QColor getColor(double value);
    void setArrowSteps(int arrowSteps);
    void setColorSteps(int colorSteps);
    QColor getBackgroundColor();
    void setBackgroundColor(QColor backgroundColor);
    void setColorBar(double x, double y, double width, double height, double captionX, double captionY, double captionPt);
    void setColorStyle(int colorStyle);
    int getRenderingOrientation();
    void setRenderingOrientation(int renderingOrientation);
    void updateCellsList();
    void setWholePlane(bool wholePlane1, bool wholePlane2, bool wholePlane3);
    void setColorComponent(int colorComponent);
    void setStreamLineGenerator(StreamLineGenerator *streamLineGenerator);
    StreamLineGenerator* getStreamLineGenerator();
    void setIsolineSeeder(IsolineSeeder *isolineSeeder);
    void setStreamIntegrationX(int streamIntegrationX);
    void setStreamIntegrationY(int streamIntegrationY);
    std::list<double> suggestIsolines(int qtt);
    void setHighlight(int x, int y, int z, int index = 0);
private:
    void decode(int v, int *x, int *y, int *z);
    bool color(double value, bool update = true);
    double colorStep(double value);
    void quad(int component, BaseCell *empty, int i, int j, int k, int orientation, double value);
    void arrow(PainterCell *cell, MyVector3D u, bool doPaint, bool doColor);
    void clearScalarField();
    void setScalarValue(int x, int y, int z, double value);
    double getScalarValue(int x, int y, int z = 0);
    Grid *grid;
    QString strategy;
    void (Painter::*painter) (bool);
    GLuint background, axis, isolinesDL, isosurfacesDL;
    double latticeSize;
    Listener *listener;
    Camera *camera;
    int minx, maxx, miny, maxy, minz, maxz;
    bool showSourceOpen, showFluid, showParticles, showAxis, showColorBar, show2ndDistribution, showIsolines, showHeightmap, showLight, showForces;
    bool analyzeX, analyzeY, analyzeZ;
    double offsetx, offsety, offsetz;
    double transparency;
    double cellsSize, cellsSize2;
    double minimumValue, maximumValue;
    double colorAverage, colorDelta;
    Colors *colors;
    bool continuousAutoAdjust;
    View *view;
    int arrowSteps, colorSteps;
    QColor backgroundColor;
    double colorBarX, colorBarY, colorBarWidth, colorBarHeight, colorBarCaptionX, colorBarCaptionY;
    int colorBarCaptionPt;
    LBWidget *widget;
    int colorStyle;
    int renderingOrientation;
    std::list<PainterCell*> *cellsList;
    bool wholePlane1, wholePlane2, wholePlane3;
    int colorComponent;
    StreamLineGenerator *streamLineGenerator;
    IsolineSeeder *isolineSeeder;
    double *scalarField;
    int streamIntegrationX, streamIntegrationY;
    int highlightX[2], highlightY[2], highlightZ[2];
    int isolinesAge;
    bool isolinesRepaint;
};

#endif // PAINTER_H
