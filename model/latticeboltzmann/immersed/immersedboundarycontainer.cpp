#include "immersedboundarycontainer.h"

#include "circleitem.h"
#include "../model/latticeboltzmann/lbgrid.h"
#include "../model/math/util.h"
#include <cmath>
#include "../model/latticeboltzmann/basecell.h"
#include "../model/latticeboltzmann/lbutil.h"
#include "../model/math/vector3i.h"
#include "../model/latticeboltzmann/sccell.h"
#include "fileitem.h"
#include <QDebug>

ImmersedBoundaryContainer::ImmersedBoundaryContainer(Grid *grid) {
    this->grid = grid;
    items = new std::list<ImmersedItem*>();
}

void ImmersedBoundaryContainer::preUpdate1() {
    forces.clear();
    for (std::list<ImmersedItem*>::iterator item = items->begin(); item != items->end(); item++) {
        std::list<MyVector3D> *newPoints = new std::list<MyVector3D>();
        std::list<MyVector3D> *points = (*item)->getPoints();
        MyVector3D forceSum = MyVector3D();
        int forceCount = 0;
        double rotation = 0;
        for (std::list<MyVector3D>::iterator point = points->begin(); point != points->end(); point++) {
            MyVector3D u = grid->interpolateVelocity((*point).getX(), (*point).getY());
            if ((*item)->getIntParameter("rigidBody") == 0) {
                newPoints->push_back((*point) + u);
            }
            MyVector3D newForce = (u ^ -(*item)->getParameter("k"));
            forces.push_back(MyTwoVector3D(MyVector3D((*point).getX(), (*point).getY(), 0.0), newForce));
            forceSum = forceSum + u;
            forceCount++;
            rotation += (u ^ ((*point) - (*item)->getCenterOfMass())).getZ();
        }
        rotation /= forceCount;
        if ((*item)->getIntParameter("rigidBody") == 1) {
            if ((*item)->getFixedPoint().getX() >= 0) {
                for (std::list<MyVector3D>::iterator point = points->begin(); point != points->end(); point++) {
                    newPoints->push_back((*point).rotate2D((*item)->getFixedPoint(), -rotation));
                }
            } else {
                for (std::list<MyVector3D>::iterator point = points->begin(); point != points->end(); point++) {
                    newPoints->push_back(((*point) + (forceSum ^ (1.0 / forceCount))).rotate2D((*item)->getCenterOfMass(), -rotation));
                }
            }
        }
        (*item)->setPoints(newPoints);
        (*item)->calculateCenterOfMass();
    }
}

void ImmersedBoundaryContainer::preUpdate2(int x, int y, int z) {
    BaseCell *cell = grid->getGrid(y, x, z);
    if (items->size() > 0 && cell->isFluid()) {
        MyVector3D f = MyVector3D();
        for (std::list<MyTwoVector3D>::iterator force = forces.begin(); force != forces.end(); force++) {
            f = f + ((*force).getV2() ^ Util::dirac(std::sqrt(MyVector3D(x, y, z).distance2((*force).getV1()))));
        }
        SCCell *sc = dynamic_cast<SCCell*>(cell);
        if (sc != 0) {
            sc->setForcesVelocity(f);
        }
        for (int i = 0; i < 9; i++) {
            cell->setNextF(i, cell->getNextF(i, 0) + 3.0 / 2 * LBUtil::W[9][i] * (f * LBUtil::C[9][i].toMyVector3D()), 0);
        }
    }
}

int ImmersedBoundaryContainer::addItem(ImmersedItem *item) {
    int max = 0;
    for (std::list<ImmersedItem*>::iterator existing = items->begin(); existing != items->end(); existing++) {
        if ((*existing)->getId() > max) {
            max = (*existing)->getId();
        }
    }
    item->setId(max + 1);
    items->push_back(item);
    return max + 1;
}

void ImmersedBoundaryContainer::removeItem(int id) {
    for (std::list<ImmersedItem*>::iterator existing = items->begin(); existing != items->end(); existing++) {
        if ((*existing)->getId() == id) {
            existing = items->erase(existing);
        }
    }
}

ImmersedBoundaryContainer::~ImmersedBoundaryContainer() {
    delete items;
}

std::list<ImmersedItem*>* ImmersedBoundaryContainer::getItems() {
    return items;
}

void ImmersedBoundaryContainer::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("itemsSize", QString::number(items->size()));
    for (std::list<ImmersedItem*>::iterator item = items->begin(); item != items->end(); item++) {
        writer.writeStartElement("item");
        (*item)->passivate(writer);
        writer.writeEndElement();
    }
}

void ImmersedBoundaryContainer::activate(QXmlStreamReader &reader) {
    if (reader.name().toString() == "immersedBoundary") {
        items->clear();
        int itemsQuantity = reader.attributes().value("itemsSize").toString().toInt();
        for (int i = 0; i < itemsQuantity; i++) {
            do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
            ImmersedItem *item = new FileItem();
            item->activate(reader);
            items->push_back(item);
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}
