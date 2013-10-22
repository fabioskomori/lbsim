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

#include "painter.h"
#include "camera.h"
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../model/latticeboltzmann/gridsimulation.h"
#include "../../model/latticeboltzmann/lbutil.h"
#include "../../model/latticeboltzmann/nullcell.h"
#include "../../model/latticeboltzmann/porouscell.h"
#include "../../model/latticeboltzmann/boundary/sourcecell.h"
#include "../../model/latticeboltzmann/boundary/opencell.h"
#include "../../model/latticeboltzmann/boundary/wallcell.h"
#include "../../model/latticeboltzmann/boundary/dragwallcell.h"
#include "../../model/latticeboltzmann/boundary/partialslipboundary.h"
#include "../../model/latticeboltzmann/extra/particle.h"
#include "../../model/latticeboltzmann/extra/velocityfield.h"
#include "../../model/latticeboltzmann/extra/initialparticle.h"
#include "../../model/latticeboltzmann/moving/movingcell.h"
#include "../../model/latticeboltzmann/moving/movingparticle.h"
#include "../../model/latticeboltzmann/moving/somethingmoving.h"
#include "../../model/latticeboltzmann/moving/particlemanager.h"
#include "../../model/latticeboltzmann/multi/mccell.h"
#include "../../model/listener/listener.h"
#include "../../model/math/myvector3d.h"
#include "../../model/latticeboltzmann/thermal/thermalwall.h"
#include "../../view/lbwidget.h"
#include "../../view/view.h"
#include "../../view/colors.h"
#include "../../view/util/singleton.h"
#include "../../model/latticeboltzmann/passivescalar/passivescalarsingleton.h"
#include "../../model/latticeboltzmann/passivescalar/passivescalarcell.h"
#include <sstream>
#include <typeinfo>
#include <QDebug>
#include <cmath>
#include <QMutex>
#include "paintercell.h"
#include "../../model/latticeboltzmann/streamline/streamlinegenerator.h"
#include "../../model/latticeboltzmann/streamline/streamlinecurve.h"
#include "isolineseeder.h"
#include "equallyspacedseeder.h"
#include "../../model/math/util.h"
#include "isosurfacehelper.h"

using namespace std;

Painter::Painter(LBWidget *widget) : widget(widget) {
    grid = new Grid();
    Singleton::instance()->setParameters(grid->getParameters());
    PassiveScalarSingleton::getInstance()->inject(grid);
    camera = new Camera();
    setStrategy("velocity");
    showSourceOpen = true;
    showFluid = true;
    showParticles = true;
    showAxis = false;
    showColorBar = false;
    show2ndDistribution = false;
    showIsolines = false;
    showHeightmap = false;
    analyzeX = analyzeY = analyzeZ = true;
    offsetx = offsety = offsetz = 0;
    transparency = 1;
    cellsSize = 10;
    cellsSize2 = 1;
    setColorRange(0.15, 0.15, true);
    continuousAutoAdjust = true;
    colors = 0;
    view = 0;
    arrowSteps = 1;
    colorSteps = 0;
    backgroundColor = QColor(255, 255, 255);
    colorBarX = 0;
    colorBarY = -0.1;
    colorBarWidth = 1;
    colorBarHeight = 0.01;
    colorBarCaptionX = 0;
    colorBarCaptionY = 0;
    colorBarCaptionPt = 12;
    colorStyle = 0;
    renderingOrientation = 4;
    cellsList = new std::list<PainterCell*>();
    wholePlane1 = wholePlane2 = wholePlane3 = false;
    colorComponent = 0;
    streamLineGenerator = 0;
    isolineSeeder = new EquallySpacedSeeder(10, this);
    scalarField = 0;
    streamIntegrationX = streamIntegrationY = 0;
    highlightX[0] = highlightY[0] = highlightZ[0] = highlightX[1] = highlightY[1] = highlightZ[1] = 0;
    isolinesDL = isosurfacesDL = -1;
    showLight = false;
    showForces = false;
}

Painter::~Painter() {
    delete grid;
    delete camera;
    delete cellsList;
    if (scalarField != 0) {
        delete[] scalarField;
    }
}

Grid* Painter::getGrid() {
    return grid;
}

void Painter::setColorComponent(int colorComponent) {
    this->colorComponent = colorComponent;
}

void Painter::updateDisplayList(bool keepBounds) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    latticeSize = 1.0 / grid->getConfig()->getWidth();
    if (1.0 / grid->getConfig()->getHeight() < latticeSize) {
        latticeSize = 1.0 / grid->getConfig()->getHeight();
    }
    if (1.0 / grid->getConfig()->getLength() < latticeSize) {
        latticeSize = 1.0 / grid->getConfig()->getLength();
    }
    offsetx = -latticeSize * grid->getConfig()->getWidth() / 2;
    offsety = -latticeSize * grid->getConfig()->getHeight() / 2;
    offsetz = -latticeSize * grid->getConfig()->getLength() / 2;
    axis = glGenLists(1);
    glNewList(axis, GL_COMPILE);
    double arrowLength = 0.05, arrowWidth = 0.02;
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(offsetx, offsety, offsetz);
    glVertex3f(1, offsety, offsetz);
    glVertex3f(1 - arrowLength, offsety + arrowWidth, offsetz);
    glVertex3f(1, offsety, offsetz);
    glVertex3f(1 - arrowLength, offsety - arrowWidth, offsetz);
    glVertex3f(1, offsety, offsetz);
    glEnd();
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(offsetx, offsety, offsetz);
    glVertex3f(offsetx, 1, offsetz);
    glVertex3f(offsetx + arrowWidth, 1 - arrowLength, offsetz);
    glVertex3f(offsetx, 1, offsetz);
    glVertex3f(offsetx - arrowWidth, 1 - arrowLength, offsetz);
    glVertex3f(offsetx, 1, offsetz);
    glEnd();
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(offsetx, offsety, offsetz);
    glVertex3f(offsetx, offsety, 1);
    glVertex3f(offsetx + arrowWidth, offsety + arrowWidth, 1 - arrowLength);
    glVertex3f(offsetx, offsety, 1);
    glVertex3f(offsetx - arrowWidth, offsety - arrowWidth, 1 - arrowLength);
    glVertex3f(offsetx, offsety, 1);
    glEnd();
    glEndList();
    background = glGenLists(1);
    glNewList(background, GL_COMPILE);
    if (!keepBounds) {
        minx = 0;
        miny = 0;
        minz = 0;
        maxx = grid->getConfig()->getWidth();
        maxy = grid->getConfig()->getHeight();
        maxz = grid->getConfig()->getLength();
    }
    //camera->centralize(MyVector3D(0, 0, -1));
    for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                BaseCell *cell = grid->getGrid(i, j, k);
                if (!cell->isFluid()) {
                    PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>(cell);
                    if (psc != 0) {
                        cell = psc->getCell();
                    }
                    if (typeid (*cell) == typeid (SourceCell)) {
                        glColor3f(1.0f, 0, 0);
                    } else if (typeid (*cell) == typeid (WallCell) || typeid (*cell) == typeid (ThermalWall) || typeid (*cell) == typeid (DragWallCell) || typeid (*cell) == typeid (PartialSlipBoundary)) {
                        glColor3f(0.5, 0.5, 0.5);
                    } else if (typeid (*cell) == typeid (OpenCell)) {
                        glColor3f(0, 0, 0);
                    } else if (typeid (*cell) == typeid (MovingCell) || typeid (*cell) == typeid (NullCell)) {
                        continue;
                    }
                    glNormal3f(0, 0, 1);
                    glBegin(GL_QUADS);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize + latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize + latticeSize, offsety + i * latticeSize + latticeSize, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize + latticeSize, offsetz + k * latticeSize);
                    glEnd();
                    glNormal3f(0, 1, 0);
                    glBegin(GL_QUADS);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize + latticeSize * arrowSteps, offsety + i * latticeSize, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize + latticeSize * arrowSteps, offsety + i * latticeSize, offsetz + k * latticeSize + latticeSize * arrowSteps);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize + latticeSize * arrowSteps);
                    glEnd();
                    glNormal3f(1, 0, 0);
                    glBegin(GL_QUADS);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize + latticeSize * arrowSteps, offsetz + k * latticeSize);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize + latticeSize * arrowSteps, offsetz + k * latticeSize + latticeSize * arrowSteps);
                    glVertex3f(offsetx + j * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize + latticeSize * arrowSteps);
                    glEnd();
                }
            }
        }
    }
    glEndList();
    if (scalarField != 0) {
        delete[] scalarField;
    }
    scalarField = new double[grid->getConfig()->getWidth() * grid->getConfig()->getHeight() * grid->getConfig()->getLength()];
}

void Painter::paint(bool) {
    Singleton::instance()->getRenderMutex()->lock();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (showColorBar) {
        paintColorBar();
    }
    paintLight();
    camera->prePaint();
    if (showAxis) {
        glCallList(axis);
    }
    if (showSourceOpen) {
        glCallList(background);
    }
    (this->*painter)(false);
    if (showFluid) {
        if (continuousAutoAdjust) {
            autoAdjust();
        }
        (this->*painter)(true);
    }
    if (showParticles) {
        paintPathLines();
    }
    if (showIsolines) {
        if (grid->getConfig()->getLength() == 1) {
            paintIsolines();
        } else {
            paintIsosurfaces();
        }
    }
    paintForces();
    paintHighlight();
    camera->postPaint();
    Singleton::instance()->getRenderMutex()->unlock();
}

void Painter::paintLight() {
    if (showLight) {
        GLfloat mat_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat mat_diffuse[] = {0.75f, 0.75f, 0.75f, 1.0f};
        GLfloat mat_specular[] = {0.50f, 0.50f, 0.50f, 1.0f};
        GLfloat mat_shininess[] = {25.0};
        GLfloat light_position[] = {0.0, -2.0, 0.0, 0.0};
        glShadeModel(GL_SMOOTH);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_COLOR_MATERIAL);
    }
}

void Painter::paintHighlight() {
    for (int i = 0; i < 2; i++) {
        if (highlightX[i] != 0 && highlightY[i] != 0 && highlightZ[i] != 0) {
            highlightX[i]--;
            highlightY[i]--;
            highlightZ[i]--;
            glColor4f(0.5, 0.5, 0.5, 0.5);
            glBegin(GL_QUADS);

            glVertex3f(offsetx + highlightX[i] * latticeSize, offsety + -0.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize, offsety + -0.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize, offsety + 1.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + highlightX[i] * latticeSize, offsety + 1.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);

            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + -0.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + -0.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + 1.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + 1.5 * grid->getConfig()->getHeight() * latticeSize, offsetz + highlightZ[i] * latticeSize);

            glVertex3f(offsetx + -0.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + -0.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + 1.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + 1.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize, offsetz + highlightZ[i] * latticeSize + latticeSize / 2);

            glVertex3f(offsetx + -0.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + highlightZ[i] * latticeSize);
            glVertex3f(offsetx + -0.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + highlightZ[i] * latticeSize + latticeSize);
            glVertex3f(offsetx + 1.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + highlightZ[i] * latticeSize + latticeSize);
            glVertex3f(offsetx + 1.5 * grid->getConfig()->getWidth() * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + highlightZ[i] * latticeSize);

            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + highlightY[i] * latticeSize, offsetz + -0.5 * grid->getConfig()->getLength() * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + highlightY[i] * latticeSize + latticeSize, offsetz + -0.5 * grid->getConfig()->getLength() * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + highlightY[i] * latticeSize + latticeSize, offsetz + 1.5 * grid->getConfig()->getLength() * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize / 2, offsety + highlightY[i] * latticeSize, offsetz + 1.5 * grid->getConfig()->getLength() * latticeSize);

            glVertex3f(offsetx + highlightX[i] * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + -0.5 * grid->getConfig()->getLength() * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + -0.5 * grid->getConfig()->getLength() * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize + latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + 1.5 * grid->getConfig()->getLength() * latticeSize);
            glVertex3f(offsetx + highlightX[i] * latticeSize, offsety + highlightY[i] * latticeSize + latticeSize / 2, offsetz + 1.5 * grid->getConfig()->getLength() * latticeSize);

            glEnd();
            highlightX[i]++;
            highlightY[i]++;
            highlightZ[i]++;
            //highlightX = highlightY = highlightZ = 0;
        }
    }
}

void Painter::paintNormalArrow(bool doPaint) {
    if (!doPaint) clearScalarField();
    /*glLineWidth(cellsSize2);
    minimumValue = 1e100; maximumValue = -1e100;
    glBegin(GL_LINES);
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        if (color((*cell)->getCell()->getU((*cell)->getC() - 1).norm())) {
            MyVector3D normU = (*cell)->getCell()->getU((*cell)->getC() - 1).normalize();
            glVertex3f(offsetx + ((*cell)->getJ() - normU.getX() * arrowSteps / 2) * latticeSize + latticeSize / 2, offsety + ((*cell)->getI() - normU.getY() * arrowSteps / 2) * latticeSize + latticeSize / 2, offsetz + ((*cell)->getK() - normU.getZ() * arrowSteps / 2) * latticeSize + latticeSize / 2);
            glVertex3f(offsetx + ((*cell)->getJ() + normU.getX() * arrowSteps / 2) * latticeSize + latticeSize / 2, offsety + ((*cell)->getI() + normU.getY() * arrowSteps / 2) * latticeSize + latticeSize / 2, offsetz + ((*cell)->getK() + normU.getZ() * arrowSteps / 2) * latticeSize + latticeSize / 2);
        }
    }
    glEnd();*/
    minimumValue = 1e100; maximumValue = -1e100;
    glColor3f(0.5, 0.5, 0.5);
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        MyVector3D u = ((*cell)->getCell()->getU((*cell)->getC() - 1)).normalize();
        arrow(*cell, u, doPaint, false);
    }
}

void Painter::paintIsolines() {
    glLineWidth(cellsSize2);
    glColor3f(0.5, 0.5, 0.5);
    if (isolinesDL == -1 || isolinesAge != grid->getSimulation()->getIterations() || isolinesRepaint) {
        int count = 0;
        double *seeds = isolineSeeder->seed(&count);
        if (isolinesDL != -1) {
            glDeleteLists(isolinesDL, 1);
        }
        isolinesRepaint = false;
        isolinesAge = grid->getSimulation()->getIterations();
        isolinesDL = glGenLists(1);
        glNewList(isolinesDL, GL_COMPILE);
        double v = 0;
        for (int i = 0; i < grid->getConfig()->getWidth() - 1; i++) {
            for (int j = 0; j < grid->getConfig()->getHeight() - 1; j++) {
                for (int s = 0; s < count; s++) {
                    if (!showFluid) {
                        color(seeds[s]);
                    }
                    glBegin(GL_LINES);
                    double z = showHeightmap ? (cellsSize / 100 * (seeds[s] - (colorAverage - colorDelta)) / (2 * colorDelta)) : 0;
                    v = Util::interpolate(getScalarValue(i, j), getScalarValue(i + 1, j), seeds[s]);
                    if (v >= 0 && v <= 1) {
                        glVertex3f(offsetx + (i + v) * latticeSize, offsety + j * latticeSize, offsetz + 0 * latticeSize + latticeSize / 2 + z);
                    }
                    v = Util::interpolate(getScalarValue(i + 1, j), getScalarValue(i + 1, j + 1), seeds[s]);
                    if (v >= 0 && v <= 1) {
                        glVertex3f(offsetx + (i + 1) * latticeSize, offsety + (j + v) * latticeSize, offsetz + 0 * latticeSize + latticeSize / 2 + z);
                    }
                    v = Util::interpolate(getScalarValue(i + 1, j + 1), getScalarValue(i, j + 1), seeds[s]);
                    if (v >= 0 && v <= 1) {
                        glVertex3f(offsetx + (i + 1 - v) * latticeSize, offsety + (j + 1) * latticeSize, offsetz + 0 * latticeSize + latticeSize / 2 + z);
                    }
                    v = Util::interpolate(getScalarValue(i, j + 1), getScalarValue(i, j), seeds[s]);
                    if (v >= 0 && v <= 1) {
                        glVertex3f(offsetx + i * latticeSize, offsety + (j + 1 - v) * latticeSize, offsetz + 0 * latticeSize + latticeSize / 2 + z);
                    }
                    v = Util::interpolate(getScalarValue(i, j), getScalarValue(i + 1, j), seeds[s]);
                    if (v >= 0 && v <= 1) {
                        glVertex3f(offsetx + (i + v) * latticeSize, offsety + j * latticeSize, offsetz + 0 * latticeSize + latticeSize / 2 + z);
                    }
                    glEnd();
                }
            }
        }
        glEndList();
    }
    glCallList(isolinesDL);
}

void Painter::paintIsosurfaces() {
    glLineWidth(cellsSize2);
    glColor3f(0.5, 0.5, 0.5);
    if (isosurfacesDL == -1 || isolinesAge != grid->getSimulation()->getIterations() || isolinesRepaint) {
        int count = 0;
        double *seeds = isolineSeeder->seed(&count);
        if (isosurfacesDL != -1) {
            glDeleteLists(isosurfacesDL, 1);
        }
        isolinesRepaint = false;
        isolinesAge = grid->getSimulation()->getIterations();
        isosurfacesDL = glGenLists(1);
        glNewList(isosurfacesDL, GL_COMPILE);
        double v = 0;
        MyVector3D ps[3];
        int pos = 0;
        for (int s = 0; s < count; s++) {
            if (!showFluid) {
                color(seeds[s]);
            }
            for (int x = 0; x < grid->getConfig()->getWidth() - 1; x++) {
                for (int y = 0; y < grid->getConfig()->getHeight() - 1; y++) {
                    for (int z = 0; z < grid->getConfig()->getLength() - 1; z++) {
                        std::list<IsosurfaceHelper> candidates;
                        for (int px = 0; px <= 1; px++) {
                            for (int py = 0; py <= 1; py++) {
                                for (int pz = 0; pz <= 1; pz++) {
                                    if (px == 0) {
                                        v = Util::interpolate(getScalarValue(x + px, y + py, z + pz), getScalarValue(x + px + 1, y + py, z + pz), seeds[s]);
                                        if (v >= 0 && v <= 1) {
                                            if (getScalarValue(x + px, y + py, z + pz) < getScalarValue(x + px + 1, y + py, z + pz)) {
                                                candidates.push_back(IsosurfaceHelper(px * 4 + py * 2 + pz, (px + 1) * 4 + py * 2 + pz, v));
                                            } else {
                                                candidates.push_back(IsosurfaceHelper((px + 1) * 4 + py * 2 + pz, px * 4 + py * 2 + pz, v));
                                            }
                                        }
                                    }
                                    if (py == 0) {
                                        v = Util::interpolate(getScalarValue(x + px, y + py, z + pz), getScalarValue(x + px, y + py + 1, z + pz), seeds[s]);
                                        if (v >= 0 && v <= 1) {
                                            if (getScalarValue(x + px, y + py, z + pz) < getScalarValue(x + px, y + py + 1, z + pz)) {
                                                candidates.push_back(IsosurfaceHelper(px * 4 + py * 2 + pz, px * 4 + (py + 1) * 2 + pz, v));
                                            } else {
                                                candidates.push_back(IsosurfaceHelper(px * 4 + (py + 1) * 2 + pz, px * 4 + py * 2 + pz, v));
                                            }
                                        }
                                    }
                                    if (pz == 0) {
                                        v = Util::interpolate(getScalarValue(x + px, y + py, z + pz), getScalarValue(x + px, y + py, z + pz + 1), seeds[s]);
                                        if (v >= 0 && v <= 1) {
                                            if (getScalarValue(x + px, y + py, z + pz) < getScalarValue(x + px, y + py, z + pz + 1)) {
                                                candidates.push_back(IsosurfaceHelper(px * 4 + py * 2 + pz, px * 4 + py * 2 + (pz + 1), v));
                                            } else {
                                                candidates.push_back(IsosurfaceHelper(px * 4 + py * 2 + (pz + 1), px * 4 + py * 2 + pz, v));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        bool changed = true;
                        while (changed) {
                            changed = false;
                            for (std::list<IsosurfaceHelper>::iterator cand1 = candidates.begin(); cand1 != candidates.end(); cand1++) {
                                std::list<IsosurfaceHelper>::iterator cand2 = cand1;
                                cand2++;
                                while (cand2 != candidates.end()) {
                                    if ((*cand1).isNeighbor(*cand2)) {
                                        (*cand2).merge(*cand1);
                                        changed = true;
                                        break;
                                    }
                                    cand2++;
                                }
                                if (changed) {
                                    candidates.erase(cand1);
                                    break;
                                }
                            }
                        }
                        for (std::list<IsosurfaceHelper>::iterator candidate = candidates.begin(); candidate != candidates.end(); candidate++) {
                            if ((*candidate).count() >= 3) {
                                (*candidate).order();
                                glBegin(GL_TRIANGLE_STRIP);
                                std::list<int> points = (*candidate).getPoints();
                                std::list<double> values = (*candidate).getValues();
                                std::list<double>::iterator value = values.begin();
                                pos = 0;
                                for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
                                    if (pos == 2) {
                                        ps[0] = ps[1];
                                        ps[1] = ps[2];
                                    }
                                    int p1x = 0, p1y = 0, p1z = 0, p2x = 0, p2y = 0, p2z = 0;
                                    decode(*point, &p1x, &p1y, &p1z);
                                    point++;
                                    decode(*point, &p2x, &p2y, &p2z);
                                    double v = *value;
                                    value++;
                                    ps[pos] = MyVector3D(offsetx + (x + p1x + (p2x - p1x) * v) * latticeSize, offsety + (y + p1y + (p2y - p1y) * v) * latticeSize, offsetz + (z + p1z + (p2z - p1z) * v) * latticeSize);
                                    if (pos == 2) {
                                        MyVector3D normal = (ps[0] - ps[2]) ^ (ps[0] - ps[1]);
                                        glNormal3f(normal.getX(), normal.getY(), normal.getZ());
                                    }
                                    glVertex3f(ps[pos].getX(), ps[pos].getY(), ps[pos].getZ());
                                    if (pos < 2) {
                                        pos++;
                                    }
                                }
                                glEnd();
                            }
                        }
                    }
                }
            }
        }
        glEndList();
    }
    glCallList(isosurfacesDL);
}

void Painter::decode(int v, int *x, int *y, int *z) {
    *x = *y = *z = 0;
    if (v >= 4) {
        *x = 1;
        v -=4;
    }
    if (v >= 2) {
        *y = 1;
        v -=2;
    }
    if (v >= 1) {
        *z = 1;
    }
}

void Painter::paintStreamLines() {
    clearScalarField();
    glColor3f(0.5, 0.5, 0.5);
    glLineWidth(cellsSize2);
    if (streamLineGenerator != 0) {
        std::list<StreamLineCurve*>* streamLines = streamLineGenerator->generate();
        for (std::list<StreamLineCurve*>::iterator streamLine = streamLines->begin(); streamLine != streamLines->end(); streamLine++) {
            std::list<MyVector3D>* points = (*streamLine)->getPoints();
            glBegin(GL_LINE_STRIP);
            for (std::list<MyVector3D>::iterator point = points->begin(); point != points->end(); point++) {
                glVertex3f(offsetx + (*point).getX() * latticeSize + latticeSize / 2, offsety + (*point).getY() * latticeSize + latticeSize / 2, offsetz + (*point).getZ() * latticeSize + latticeSize / 2);
            }
            glEnd();
        }
        delete streamLines;
    }
}

void Painter::paintPathLines() {
    glLineWidth(cellsSize2);
    list<InitialParticle*> *initialParticles = grid->getVelocityField()->getInitialParticles();
    for (list<InitialParticle*>::iterator initialParticle = initialParticles->begin(); initialParticle != initialParticles->end(); initialParticle++) {
        list<Particle*> *pathLines = (*initialParticle)->getStreakLines();
        glBegin(GL_LINES);
        for (list<Particle*>::iterator iterator = pathLines->begin(); iterator != pathLines->end(); iterator++) {
            glColor3f((*iterator)->getColor().getX() / 255.0, (*iterator)->getColor().getY() / 255.0, (*iterator)->getColor().getZ() / 255.0);
            if ((*iterator)->getPosition().getX() >= minx && (*iterator)->getPosition().getX() <= maxx &&
                    (*iterator)->getPosition().getY() >= miny && (*iterator)->getPosition().getY() <= maxy &&
                    (*iterator)->getPosition().getZ() >= minz && (*iterator)->getPosition().getZ() <= maxz) {
                //glBegin(GL_POINTS);
                //glVertex3f(offsetx + (*iterator)->getPosition().getX() * latticeSize + latticeSize / 2, offsety + (*iterator)->getPosition().getY() * latticeSize + latticeSize / 2, offsetz + (*iterator)->getPosition().getZ() * latticeSize + latticeSize / 2 + 0.01);
                //glEnd();
                glVertex3f(offsetx + (*iterator)->getPreviousPosition().getX() * latticeSize + latticeSize / 2, offsety + (*iterator)->getPreviousPosition().getY() * latticeSize + latticeSize / 2, offsetz + (*iterator)->getPreviousPosition().getZ() * latticeSize + latticeSize / 2 + 0.01);
                glVertex3f(offsetx + (*iterator)->getPosition().getX() * latticeSize + latticeSize / 2, offsety + (*iterator)->getPosition().getY() * latticeSize + latticeSize / 2, offsetz + (*iterator)->getPosition().getZ() * latticeSize + latticeSize / 2 + 0.01);
            }
        }
        glEnd();
    }
}

void Painter::quad(int component, BaseCell*, int i, int j, int k, int orientation, double value) {
    /*double nw = 0, ne = 0, se = 0, sw = 0;
    if (showHeightmap) {
        nw = ne = se = sw = cellsSize / 100 * (value - (colorAverage - colorDelta)) / (2 * colorDelta);
    }*/
    // 1: yz plane 2: xz plane 4: xy plane
    double heightValue = 0;
    BaseCell *cell = 0;
    if (orientation >= 4) {
        glNormal3f(0, 0, -1);
        if (showHeightmap) {
            /*glBegin(GL_TRIANGLE_FAN);
            heightValue = (value - (colorAverage - colorDelta)) / (2 * colorDelta) * cellsSize / 100;
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 0.5, offsety + i * latticeSize + latticeSize * 0.5, offsetz + k * latticeSize + heightValue);
            cell = grid->getGrid(i, j + 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 1, offsety + i * latticeSize + latticeSize * 0.5, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j + 1) + (i) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i + 1, j + 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 1, offsety + i * latticeSize + latticeSize * 1, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j + 1) + (i + 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i + 1, j, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 0.5, offsety + i * latticeSize + latticeSize * 1, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j) + (i + 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i + 1, j - 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 0, offsety + i * latticeSize + latticeSize * 1, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j - 1) + (i + 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i, j - 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 0, offsety + i * latticeSize + latticeSize * 0.5, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j - 1) + (i) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i - 1, j - 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 0, offsety + i * latticeSize + latticeSize * 0, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j - 1) + (i - 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i - 1, j, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 0.5, offsety + i * latticeSize + latticeSize * 0, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j) + (i - 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i - 1, j + 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 1, offsety + i * latticeSize + latticeSize * 0, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j + 1) + (i - 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            cell = grid->getGrid(i, j + 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * 1, offsety + i * latticeSize + latticeSize * 0.5, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (heightValue + (scalarField[(j + 1) + (i) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta)) / 2 : heightValue));
            glEnd();*/
            glBegin(GL_QUADS);
            heightValue = (value - (colorAverage - colorDelta)) / (2 * colorDelta) * cellsSize / 100;
            glVertex3f(1.1 * component + offsetx + j * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize + heightValue);
            cell = grid->getGrid(i, j + 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * arrowSteps, offsety + i * latticeSize, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (scalarField[(j + 1) + (i) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta) : heightValue));
            cell = grid->getGrid(i + 1, j + 1, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * arrowSteps, offsety + i * latticeSize + latticeSize * arrowSteps, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (scalarField[(j + 1) + (i + 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta) : heightValue));
            cell = grid->getGrid(i + 1, j, k);
            glVertex3f(1.1 * component + offsetx + j * latticeSize, offsety + i * latticeSize + latticeSize * arrowSteps, offsetz + k * latticeSize + (cell != 0 && cell->isFluid() ? (scalarField[(j) + (i + 1) * grid->getConfig()->getWidth()] - (colorAverage - colorDelta)) * cellsSize / 100 / (2 * colorDelta) : heightValue));
            glEnd();
        } else {
            glBegin(GL_QUADS);
            glVertex3f(1.1 * component + offsetx + (j - (arrowSteps - 1) / 2.0) * latticeSize, offsety + (i - (arrowSteps - 1) / 2.0) * latticeSize, offsetz + (k + 0.5 - (arrowSteps - 1) / 2.0) * latticeSize);
            glVertex3f(1.1 * component + offsetx + (j - (arrowSteps - 1) / 2.0) * latticeSize + latticeSize * arrowSteps, offsety + (i - (arrowSteps - 1) / 2.0) * latticeSize, offsetz + (k + 0.5 - (arrowSteps - 1) / 2.0) * latticeSize);
            glVertex3f(1.1 * component + offsetx + (j - (arrowSteps - 1) / 2.0) * latticeSize + latticeSize * arrowSteps, offsety + (i - (arrowSteps - 1) / 2.0) * latticeSize + latticeSize * arrowSteps, offsetz + (k + 0.5 - (arrowSteps - 1) / 2.0) * latticeSize);
            glVertex3f(1.1 * component + offsetx + (j - (arrowSteps - 1) / 2.0) * latticeSize, offsety + (i - (arrowSteps - 1) / 2.0) * latticeSize + latticeSize * arrowSteps, offsetz + (k + 0.5 - (arrowSteps - 1) / 2.0) * latticeSize);
            glEnd();
        }
        orientation -= 4;
    }
    if (orientation >= 2) {
        glNormal3f(0, -1, 0);
        glBegin(GL_QUADS);
        glVertex3f(1.1 * component + offsetx + j * latticeSize, offsety + (i + 0.5) * latticeSize, offsetz + k * latticeSize);
        glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * arrowSteps, offsety + (i + 0.5) * latticeSize, offsetz + k * latticeSize);
        glVertex3f(1.1 * component + offsetx + j * latticeSize + latticeSize * arrowSteps, offsety + (i + 0.5) * latticeSize, offsetz + k * latticeSize + latticeSize * arrowSteps);
        glVertex3f(1.1 * component + offsetx + j * latticeSize, offsety + (i + 0.5) * latticeSize, offsetz + k * latticeSize + latticeSize * arrowSteps);
        glEnd();
        orientation -= 2;
    }
    if (orientation >= 1) {
        glNormal3f(-1, 0, 0);
        glBegin(GL_QUADS);
        glVertex3f(1.1 * component + offsetx + (j + 0.5) * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize);
        glVertex3f(1.1 * component + offsetx + (j + 0.5) * latticeSize, offsety + i * latticeSize + latticeSize * arrowSteps, offsetz + k * latticeSize);
        glVertex3f(1.1 * component + offsetx + (j + 0.5) * latticeSize, offsety + i * latticeSize + latticeSize * arrowSteps, offsetz + k * latticeSize + latticeSize * arrowSteps);
        glVertex3f(1.1 * component + offsetx + (j + 0.5) * latticeSize, offsety + i * latticeSize, offsetz + k * latticeSize + latticeSize * arrowSteps);
        glEnd();
    }
}

void Painter::paintVelocity(bool doPaint) {
    if (!doPaint) clearScalarField();
    minimumValue = 1e100; maximumValue = -1e100;
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        if (color((*cell)->getCell()->getU((*cell)->getC() - 1).norm())) {
            setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), (*cell)->getCell()->getU(0).norm());
            if (doPaint) {
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), (*cell)->getCell()->getU((*cell)->getC() - 1).norm());
            }
        }
    }
}

void Painter::paintMulticomponent(bool doPaint) {
    if (!doPaint) clearScalarField();
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        if (color((*cell)->getCell()->getP(-1))) {
            setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), (*cell)->getCell()->getP(-1));
            if (doPaint) {
                glColor3f((*cell)->getCell()->getP(0) / (colorAverage + colorDelta), 0, (*cell)->getCell()->getP(1) / (colorAverage + colorDelta));
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), (*cell)->getCell()->getP(-1));
            }
        }
    }
}

void Painter::paintVorticityMagnitude(bool doPaint) {
    if (!doPaint) clearScalarField();
    minimumValue = 1e100; maximumValue = -1e100;
    MyVector3D *vorticities = grid->computeVorticity();
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        if (color(vorticities[(*cell)->getJ() + (*cell)->getI() * grid->getConfig()->getWidth() + (*cell)->getK() * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].getZ())) {
            setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), vorticities[(*cell)->getJ() + (*cell)->getI() * grid->getConfig()->getWidth() + (*cell)->getK() * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].getZ());
            if (doPaint) {
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), vorticities[(*cell)->getJ() + (*cell)->getI() * grid->getConfig()->getWidth() + (*cell)->getK() * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()].getZ());
            }
        }
    }
    delete[] vorticities;
}

void Painter::paintFlux(bool doPaint) {
    minimumValue = 1e100; maximumValue = -1e100;
    if (!doPaint) clearScalarField();
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        double flux = 0;
        BaseCell **neighbors = grid->getNeighbors((*cell)->getI(), (*cell)->getJ(), (*cell)->getK());
        for (int i = 0; i < grid->getConfig()->getModel(); i++) {
            if (neighbors[i] != 0 && neighbors[i]->isFluid()) {
                flux += neighbors[i]->getF(LBUtil::OPPOSITE[grid->getConfig()->getModel()][i], 0) - (*cell)->getCell()->getF(i, 0);
            }
        }
        if (color(flux)) {
            setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), flux);
            if (doPaint) {
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), flux);
            }
        }
    }
}

void Painter::paintPressure(bool doPaint) {
    minimumValue = 1e100; maximumValue = -1e100;
    if (!doPaint) clearScalarField();
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        if (color((*cell)->getCell()->getP((*cell)->getC() - 1))) {
            setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), (*cell)->getCell()->getP((*cell)->getC() - 1));
            if (doPaint) {
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), (*cell)->getCell()->getP((*cell)->getC() - 1));
            }
        }
    }
}

void Painter::paintPermeability(bool doPaint) {
    minimumValue = 1e100; maximumValue = -1e100;
    if (!doPaint) clearScalarField();
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        PorousCell *porous = dynamic_cast<PorousCell*>((*cell)->getCell());
        if (porous != 0) {
            if (color(porous->getSolidDensity())) {
                setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), porous->getSolidDensity());
                if (doPaint) {
                    quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), porous->getSolidDensity());
                }
            }
        }
    }
}

void Painter::paintPassiveScalar(bool doPaint) {
    if (!doPaint) clearScalarField();
    minimumValue = 1e100; maximumValue = -1e100;
    if (colorComponent == -1) {
        double maxConcentration = 0;
        int colorsQuantity = PassiveScalarSingleton::getInstance()->getColorsQuantity();
        Vector3i *colors = PassiveScalarSingleton::getInstance()->getColors();
        for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
            PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>((*cell)->getCell());
            if (psc != 0) {
                for (int a = 0; a < colorsQuantity; a++) {
                    if (psc->getP(10 + a) > maxConcentration) {
                        maxConcentration = psc->getP(10 + a);
                    }
                }
            }
        }
        for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
            PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>((*cell)->getCell());
            if (psc != 0) {
                MyVector3D color = MyVector3D();
                for (int i = 0; i < colorsQuantity; i++) {
                    color = color + (colors[i] ^ (psc->getP(10 + i) / maxConcentration));
                }
                double distance = 0;
                int comp = -1;
                for (int i = 0; i < colorsQuantity; i++) {
                    double newDistance = color.distance2(colors[i].toMyVector3D());
                    if (comp == -1 || newDistance < distance) {
                        distance = newDistance;
                        comp = i;
                    }
                }
                setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), psc->getP(10 + comp));
                //color = color ^ (1.0 / colorsQuantity);
                if (doPaint) {
                    glColor3f(color.getX() / 255, color.getY() / 255, color.getZ() / 255);
                    quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), psc->getP(10 + comp));
                }
                /*color(psc->getP(10));
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation());*/
            }
        }
    } else {
        for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
            PassiveScalarCell *psc = dynamic_cast<PassiveScalarCell*>((*cell)->getCell());
            if (psc != 0) {
                if (color(psc->getP(10 + colorComponent))) {
                    setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), psc->getP(10 + colorComponent));
                    if (doPaint) {
                        quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), psc->getP(10 + colorComponent));
                    }
                }
            }
        }
    }
}

void Painter::paintStreamFunction(bool doPaint) {
    minimumValue = 1e100; maximumValue = -1e100;
    if (!doPaint) clearScalarField();
    double *stream = grid->computeStream(streamIntegrationX, streamIntegrationY);
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        if (color(stream[(*cell)->getJ() + (*cell)->getI() * grid->getConfig()->getWidth()])) {
            setScalarValue((*cell)->getJ(), (*cell)->getI(), (*cell)->getK(), stream[(*cell)->getJ() + (*cell)->getI() * grid->getConfig()->getWidth()]);
            if (doPaint) {
                quad((*cell)->getC(), (*cell)->getCell(), (*cell)->getI(), (*cell)->getJ(), (*cell)->getK(), (*cell)->getOrientation(), stream[(*cell)->getJ() + (*cell)->getI() * grid->getConfig()->getWidth()]);
            }
        }
    }
    delete[] stream;
}

void Painter::paintVorticity(bool doPaint) {
    if (!doPaint) clearScalarField();
    minimumValue = 1e100; maximumValue = -1e100;
    MyVector3D* vorticities = grid->computeVorticity();
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        int i = (*cell)->getI();
        int j = (*cell)->getJ();
        int k = (*cell)->getK();
        MyVector3D u = vorticities[j + i * grid->getConfig()->getWidth() + k * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()];
        if (u.getX() != 0 || u.getY() != 0 || u.getZ() != 0) {
            if (u.getZ() > 0) {
                glColor3f(0, 0, 1);
            } else {
                glColor3f(1, 0, 0);
            }
            arrow(*cell, u, doPaint, false);
        }
    }
    delete[] vorticities;
}

void Painter::paintForces() {
    if (showForces) {
        glColor3f(0.5, 0.5, 0.5);
        for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
            MyVector3D u = (*cell)->getCell()->getForcesVelocity(0);
            arrow(*cell, u, true, false);
        }
    }
}

void Painter::arrow(PainterCell *cell, MyVector3D u, bool doPaint, bool doColor) {
    glLineWidth(cellsSize2);
    glPointSize(1);
    double parallel = 0.25, perpendicular = 0.05;
    int i = (cell)->getI();
    int j = (cell)->getJ();
    int k = (cell)->getK();
    if (u.getX() != 0 || u.getY() != 0 || u.getZ() != 0) {
        if (doColor) {
            color(u.norm());
        }
        if (doPaint) {
            glBegin(GL_POINTS);
            glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2, offsety + i * latticeSize + latticeSize / 2, offsetz + k * latticeSize + latticeSize / 2);
            glEnd();
            glBegin(GL_LINES);
            glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2, offsety + i * latticeSize + latticeSize / 2, offsetz + k * latticeSize + latticeSize / 2);
            glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2 + u.getX() * latticeSize / 2 * cellsSize, offsety + i * latticeSize + latticeSize / 2 + u.getY() * latticeSize / 2 * cellsSize, offsetz + k * latticeSize + latticeSize / 2 + u.getZ() * latticeSize / 2 * cellsSize);
            if (u.getX() != 0) {
                double y2 = std::sqrt(perpendicular * perpendicular * (u.getX() * u.getX() + u.getY() * u.getY()) / (1 + u.getY() * u.getY() / (u.getX() * u.getX())));
                double x2 = -u.getY() * y2 / u.getX();
                glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2 + u.getX() * latticeSize / 2 * cellsSize, offsety + i * latticeSize + latticeSize / 2 + u.getY() * latticeSize / 2 * cellsSize, offsetz + k * latticeSize + latticeSize / 2 + u.getZ() * latticeSize / 2 * cellsSize);
                glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2 + ((1 - parallel) * u.getX() + x2) * latticeSize / 2 * cellsSize, offsety + i * latticeSize + latticeSize / 2 + ((1 - parallel) * u.getY() + y2) * latticeSize / 2 * cellsSize, offsetz + k * latticeSize + latticeSize / 2 + u.getZ() * latticeSize / 2 * cellsSize);
                glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2 + u.getX() * latticeSize / 2 * cellsSize, offsety + i * latticeSize + latticeSize / 2 + u.getY() * latticeSize / 2 * cellsSize, offsetz + k * latticeSize + latticeSize / 2 + u.getZ() * latticeSize / 2 * cellsSize);
                glVertex3f(1.1 * (cell)->getC() + offsetx + j * latticeSize + latticeSize / 2 + ((1 - parallel) * u.getX() - x2) * latticeSize / 2 * cellsSize, offsety + i * latticeSize + latticeSize / 2 + ((1 - parallel) * u.getY() - y2) * latticeSize / 2 * cellsSize, offsetz + k * latticeSize + latticeSize / 2 + u.getZ() * latticeSize / 2 * cellsSize);
            }
            glEnd();
        }
    }
}

void Painter::paintArrow(bool doPaint) {
    if (!doPaint) clearScalarField();
    minimumValue = 1e100; maximumValue = -1e100;
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        MyVector3D u = (*cell)->getCell()->getU((*cell)->getC() - 1);
        arrow(*cell, u, doPaint, true);
    }
}

void Painter::paintDetail(bool) {
    clearScalarField();
    glLineWidth(cellsSize2);
    minimumValue = 1e100; maximumValue = -1e100;
    for (std::list<PainterCell*>::iterator cell = cellsList->begin(); cell != cellsList->end(); cell++) {
        glBegin(GL_LINES);
        for (int a = 0; a < grid->getConfig()->getModel(); a++) {
            if (LBUtil::W[grid->getConfig()->getModel()][a] > 1e-100) {
                MyVector3D vector = LBUtil::C[grid->getConfig()->getModel()][a] ^ (*cell)->getCell()->getF(a, /*(*cell)->getC() - 1*/10);
                int i = (*cell)->getI();
                int j = (*cell)->getJ();
                int k = (*cell)->getK();
                color(vector.norm());
                glVertex3f(1.1 * (*cell)->getC() + offsetx + j * latticeSize + latticeSize / 2, offsety + i * latticeSize + latticeSize / 2, offsetz + k * latticeSize + latticeSize / 2);
                glVertex3f(1.1 * (*cell)->getC() + offsetx + j * latticeSize + latticeSize / 2 + vector.getX() * latticeSize / 2 * cellsSize, offsety + i * latticeSize + latticeSize / 2 + vector.getY() * latticeSize / 2 * cellsSize, offsetz + k * latticeSize + latticeSize / 2 + vector.getZ() * latticeSize / 2 * cellsSize);
            }
        }
        glEnd();
    }
}

void Painter::paintColorBar() {
    int maxSteps = 100;
    for (int component = 0; component < (show2ndDistribution ? 3 : 1); component++) {
        for (int step = 0; step < maxSteps; step++) {
            color(colorAverage - colorDelta + (2 * colorDelta) * (((double)step + 0.5) / maxSteps), false);
            double tileWidth = colorBarWidth / maxSteps;
            glBegin(GL_QUADS);
            glVertex3f(1.1 * component + offsetx + colorBarX + tileWidth * step, offsety + colorBarY, offsetz);
            glVertex3f(1.1 * component + offsetx + colorBarX + tileWidth * (step + 1), offsety + colorBarY, offsetz);
            glVertex3f(1.1 * component + offsetx + colorBarX + tileWidth * (step + 1), offsety + colorBarY + colorBarHeight, offsetz);
            glVertex3f(1.1 * component + offsetx + colorBarX + tileWidth * step, offsety + colorBarY + colorBarHeight, offsetz);
            glEnd();
        }
        glColor3f(0, 0, 0);
        QFont font;
        font.setPointSize(colorBarCaptionPt);
        widget->renderText(1.1 * component + offsetx + colorBarX, offsety + colorBarY - colorBarHeight + colorBarCaptionY, offsetz, QString::number(colorAverage - colorDelta), font);
        widget->renderText(1.1 * component + offsetx + colorBarX + colorBarWidth + colorBarCaptionX, offsety + colorBarY - colorBarHeight + colorBarCaptionY, offsetz, QString::number(colorAverage + colorDelta), font);
    }
}

void Painter::setColorStyle(int colorStyle) {
    this->colorStyle = colorStyle;
    isolinesRepaint = true;
}

bool Painter::color(double value, bool update) {
    if (update) {
        if (value < minimumValue) {
            minimumValue = value;
        }
        if (value > maximumValue) {
            maximumValue = value;
        }
    }
    QColor color = getColor(value);
    if (color.red() == 255 && color.green() == 255 && color.blue() == 255) {
        return false;
    } else {
        glColor3f(color.redF(), color.greenF(), color.blueF());
        return true;
    }
}

double Painter::colorStep(double value) {
    if (colorSteps != 0) {
        for (int i = 0; i < colorSteps; i++) {
            if (value > i * 1.0 / colorSteps && value < (i + 1) * 1.0 / colorSteps) {
                value = (i + 0.5) / colorSteps;
            }
        }
    }
    return value;
}

QColor Painter::getColor(double value) {
    QColor result;
    value = (value - (colorAverage - colorDelta)) / (2 * colorDelta);
    switch (colorStyle) {
    case 0:
        // wavelength
        if (value <= 0 || value >= 1) {
            result.setRgbF(1, 1, 1);
        } else {
            if (value < 0.18) {
                result.setRgbF(1 - colorStep(value / 0.18), 0, 1);
            } else if (value < 0.4) {
                result.setRgbF(0, colorStep((value - 0.18) / 0.22), 1);
            } else if (value < 0.48) {
                result.setRgbF(0, 1, 1 - colorStep((value - 0.4) / 0.08));
            } else if (value < 0.76) {
                result.setRgbF(colorStep((value - 0.48) / 0.28), 1, 0);
            } else {
                result.setRgbF(1, 1 - colorStep((value - 0.76) / 0.24), 0);
            }
        }
        break;
    case 1:
        // BCGYR
        if (value <= 0 || value >= 1) {
            result.setRgbF(1, 1, 1);
        } else if (value < 0.25) {
            result.setRgbF(0, colorStep(value * 4), 1);
        } else if (value < 0.5) {
            result.setRgbF(0, 1, 1 - colorStep((value - 0.25) * 4));
        } else if (value < 0.75) {
            result.setRgbF(colorStep((value - 0.5) * 4), 1, 0);
        } else {
            result.setRgbF(1, 1 - colorStep((value - 0.75) * 4), 0);
        }
        break;
    case 2:
        // BGYOR
        if (value <= 0 || value >= 1) {
            result.setRgbF(1, 1, 1);
        } else if (value < 0.25) {
            result.setRgbF(0, colorStep(value * 4), 1 - value * 4);
        } else if (value < 0.5) {
            result.setRgbF(colorStep((value - 0.25) * 4), 1, 0);
        } else if (value < 0.75) {
            result.setRgbF(1, 1 - 0.5 * colorStep((value - 0.5) * 4), 0);
        } else {
            result.setRgbF(1, 0.5 - 0.5 * colorStep((value - 0.75) * 4), 0);
        }
        break;
    }
    return result;
}

void Painter::setStrategy(QString strategy) {
    this->strategy = strategy;
    if (strategy == "velocity") {
        painter = &Painter::paintVelocity;
    } else if (strategy == "arrow") {
        painter = &Painter::paintArrow;
    } else if (strategy == "normal arrow") {
        painter = &Painter::paintNormalArrow;
    } else if (strategy == "pressure") {
        painter = &Painter::paintPressure;
    } else if (strategy == "detail") {
        painter = &Painter::paintDetail;
    } else if (strategy == "passive scalar") {
        painter = &Painter::paintPassiveScalar;
    /*} else if (strategy == "streamline") {
        painter = &Painter::paintStreamLines;*/
    } else if (strategy == "vorticity") {
        painter = &Painter::paintVorticity;
    } else if (strategy == "vorticity magnitude") {
        painter = &Painter::paintVorticityMagnitude;
    } else if (strategy == "stream function") {
        painter = &Painter::paintStreamFunction;
    } else if (strategy == "multicomponent") {
        painter = &Painter::paintMulticomponent;
    } else if (strategy == "permeability") {
        painter = &Painter::paintPermeability;
    } else if (strategy == "flux") {
        painter = &Painter::paintFlux;
    }
    isolinesRepaint = true;
}

QString Painter::getStrategy() {
    return strategy;
}

void Painter::setListener(Listener *listener) {
    this->listener = listener;
}

Camera* Painter::getCamera() {
    return camera;
}

void Painter::setVisibility(int minx, int maxx, int miny, int maxy, int minz, int maxz) {
    this->minx = minx;
    this->maxx = maxx;
    this->miny = miny;
    this->maxy = maxy;
    this->minz = minz;
    this->maxz = maxz;
    updateCellsList();
}

void Painter::getVisibility(int *minx, int *maxx, int *miny, int *maxy, int *minz, int *maxz) {
    *minx = this->minx;
    *maxx = this->maxx;
    *miny = this->miny;
    *maxy = this->maxy;
    *minz = this->minz;
    *maxz = this->maxz;
}

void Painter::setMinX(int minx) {
    this->minx = minx;
    isolinesRepaint = true;
    updateCellsList();
}

void Painter::setMaxX(int maxx) {
    this->maxx = maxx;
    isolinesRepaint = true;
    updateCellsList();
}

void Painter::setMinY(int miny) {
    this->miny = miny;
    isolinesRepaint = true;
    updateCellsList();
}

void Painter::setMaxY(int maxy) {
    this->maxy = maxy;
    isolinesRepaint = true;
    updateCellsList();
}

void Painter::setMinZ(int minz) {
    this->minz = minz;
    isolinesRepaint = true;
    updateCellsList();
}

void Painter::setMaxZ(int maxz) {
    this->maxz = maxz;
    isolinesRepaint = true;
    updateCellsList();
}

void Painter::toggleShowSourceOpen() {
    showSourceOpen = !showSourceOpen;
    if (showSourceOpen) {
        updateDisplayList(true);
    }
}

void Painter::toggleShowFluid() {
    showFluid = !showFluid;
    isolinesRepaint = true;
}

void Painter::toggleShowParticles() {
    showParticles = !showParticles;
}

void Painter::toggleShowAxis() {
    showAxis = !showAxis;
}

void Painter::toggleShowColorBar() {
    showColorBar = !showColorBar;
}

void Painter::toggleShowIsolines() {
    showIsolines = !showIsolines;
    if (showIsolines) {
        isolinesRepaint = true;
    }
}

void Painter::toggleShowHeightmap() {
    showHeightmap = !showHeightmap;
}

void Painter::toggleShow2ndDistribution() {
    show2ndDistribution = !show2ndDistribution;
    updateCellsList();
}

void Painter::toggleShowLight() {
    showLight = !showLight;
}

void Painter::setTransparency(double transparency) {
    this->transparency = transparency;
    updateDisplayList(false);
}

void Painter::setColorRange(double colorAverage, double colorDelta, bool firstTime) {
    this->colorAverage = colorAverage;
    this->colorDelta = colorDelta;
    isolinesRepaint = true;
    if (!firstTime) {
        colors->config(colorAverage, colorDelta / 2);
        view->notify();
    }
}

void Painter::autoAdjust() {
    setColorRange((maximumValue + minimumValue) / 2, (maximumValue - minimumValue) / 2 * 1.01, false);
}

double Painter::getColorAverage() {
    return colorAverage;
}

double Painter::getColorDelta() {
    return colorDelta;
}

void Painter::setAnalyze(bool x, bool y, bool z) {
    analyzeX = x;
    analyzeY = y;
    analyzeZ = z;
}

void Painter::setCellsSize(double newCellsSize) {
    cellsSize = newCellsSize;
}

void Painter::setCellsSize2(double newCellsSize) {
    cellsSize2 = newCellsSize;
}

bool Painter::needsToPaint() {
    return showFluid || showSourceOpen || showParticles || showColorBar || show2ndDistribution || showAxis || showIsolines;
}

double Painter::getMaximumValue() {
    return maximumValue;
}

double Painter::getMinimumValue() {
    return minimumValue;
}

void Painter::injectColors(Colors *colors) {
    this->colors = colors;
}

void Painter::injectView(View *view) {
    this->view = view;
}

void Painter::toggleContinuousAutoAdjust() {
    continuousAutoAdjust = !continuousAutoAdjust;
}

void Painter::setArrowSteps(int arrowSteps) {
    this->arrowSteps = arrowSteps;
    updateCellsList();
}

void Painter::setColorSteps(int colorSteps) {
    this->colorSteps = colorSteps;
}

QColor Painter::getBackgroundColor() {
    return backgroundColor;
}

void Painter::setBackgroundColor(QColor backgroundColor) {
    this->backgroundColor = backgroundColor;
}

void Painter::setColorBar(double x, double y, double width, double height, double captionX, double captionY, double captionPt) {
    this->colorBarX = x;
    this->colorBarY = y;
    this->colorBarWidth = width;
    this->colorBarHeight = height;
    this->colorBarCaptionX = captionX;
    this->colorBarCaptionY = captionY;
    this->colorBarCaptionPt = captionPt;
}

int Painter::getRenderingOrientation() {
    return renderingOrientation;
}

void Painter::setRenderingOrientation(int renderingOrientation) {
    this->renderingOrientation = renderingOrientation;
    updateCellsList();
}

void Painter::updateCellsList() {
    cellsList->clear();
    int quantity, type;
    double g, gads, gads2;
    grid->getConfig()->getMulticomponent(&quantity, &g, &gads, &gads2, &type);
    for (int c = 0; c < (show2ndDistribution ? (quantity + 1) : 1); c++) {
        for (int i = 0; i < grid->getConfig()->getHeight(); i += arrowSteps) {
            for (int j = 0; j < grid->getConfig()->getWidth(); j += arrowSteps) {
                for (int k = 0; k < grid->getConfig()->getLength(); k += arrowSteps) {
                    if (((i >= miny && i <= maxy) || (wholePlane1 && j >= minx && j <= maxx) || (wholePlane3 && k >= minz && k <= maxz)) &&
                            ((j >= minx && j <= maxx) || (wholePlane2 && i >= miny && i <= maxy) || (wholePlane3 && k >= minz && k <= maxz)) &&
                            ((k >= minz && k <= maxz) || (wholePlane1 && j >= minx && j <= maxx) || (wholePlane2 && i >= miny && i <= maxy))) {
                        BaseCell *cell = grid->getGrid(i, j, k);
                        if (cell != 0 && cell->isFluid()) {
                            bool alreadyAdded = false;
                            if (wholePlane1 && j >= minx && j <= maxx) {
                                cellsList->push_back(new PainterCell(cell, i, j, k, c, 1));
                                alreadyAdded = true;
                            }
                            if (wholePlane2 && i >= miny && i <= maxy) {
                                cellsList->push_back(new PainterCell(cell, i, j, k, c, 2));
                                alreadyAdded = true;
                            }
                            if (wholePlane3 && k >= minz && k <= maxz) {
                                cellsList->push_back(new PainterCell(cell, i, j, k, c, 4));
                                alreadyAdded = true;
                            }
                            if (!alreadyAdded) {
                                cellsList->push_back(new PainterCell(cell, i, j, k, c, renderingOrientation));
                            }
                        }
                    }
                }
            }
        }
    }
}

void Painter::setWholePlane(bool wholePlane1, bool wholePlane2, bool wholePlane3) {
    this->wholePlane1 = wholePlane1;
    this->wholePlane2 = wholePlane2;
    this->wholePlane3 = wholePlane3;
    updateCellsList();
}

void Painter::setStreamLineGenerator(StreamLineGenerator *streamLineGenerator) {
    this->streamLineGenerator = streamLineGenerator;
}

StreamLineGenerator* Painter::getStreamLineGenerator() {
    return streamLineGenerator;
}

void Painter::setIsolineSeeder(IsolineSeeder *isolineSeeder) {
    if (this->isolineSeeder != 0) {
        delete this->isolineSeeder;
    }
    this->isolineSeeder = isolineSeeder;
    isolinesRepaint = true;
}

std::list<double> Painter::suggestIsolines(int qtt) {
    std::list<double> result;
    /*for (int counter(0); counter < qtt; ) {
        int x(qrand() % grid->getConfig()->getWidth());
        int y(qrand() % grid->getConfig()->getHeight());
        if (grid->getGrid(y, x, 0)->isFluid()) {
            result.push_back(scalarField[x + y * grid->getConfig()->getWidth()]);
            counter++;
        }
    }*/
    std::list<double> scalars;
    for (int i = 0; i < grid->getConfig()->getHeight(); i++) {
        for (int j = 0; j < grid->getConfig()->getWidth(); j++) {
            for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                if (grid->getGrid(i, j, k)->isFluid()) {
                    scalars.push_back(scalarField[j + i * grid->getConfig()->getWidth() + k * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()]);
                }
            }
        }
    }
    scalars.sort();
    double tile = ((double)scalars.size()) / qtt;
    std::list<double>::iterator scalar = scalars.begin();
    int index = 0;
    for (int i = 0; i < qtt; i++) {
        int newIndex = (int)(i * tile + tile * 0.5);
        while (index < newIndex) {
            scalar++;
            index++;
        }
        result.push_back(*scalar);
    }
    return result;
}

void Painter::clearScalarField() {
    if (scalarField != 0) {
        for (int i = 0; i < grid->getConfig()->getWidth(); i++) {
            for (int j = 0; j < grid->getConfig()->getHeight(); j++) {
                for (int k = 0; k < grid->getConfig()->getLength(); k++) {
                    scalarField[i + j * grid->getConfig()->getWidth() + k * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()] = 0;
                }
            }
        }
    }
}

void Painter::setScalarValue(int x, int y, int z, double value) {
    if (scalarField != 0) {
        scalarField[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()] = value;
    }
}

double Painter::getScalarValue(int x, int y, int z) {
    return scalarField != 0 ? scalarField[x + y * grid->getConfig()->getWidth() + z * grid->getConfig()->getWidth() * grid->getConfig()->getHeight()] : 0;
}

void Painter::setStreamIntegrationX(int streamIntegrationX) {
    this->streamIntegrationX = streamIntegrationX;
    isolinesRepaint = true;
}

void Painter::setStreamIntegrationY(int streamIntegrationY) {
    this->streamIntegrationY = streamIntegrationY;
    isolinesRepaint = true;
}

void Painter::toggleShowForces() {
    showForces = !showForces;
}

void Painter::setHighlight(int x, int y, int z, int index) {
    highlightX[index] = x;
    highlightY[index] = y;
    highlightZ[index] = z;
}
