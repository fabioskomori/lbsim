#ifndef IMMERSEDBOUNDARYCONTAINER_H
#define IMMERSEDBOUNDARYCONTAINER_H

#include <list>
class ImmersedItem;
class Grid;
#include "../../model/math/mytwovector3d.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class ImmersedBoundaryContainer {
public:
    ImmersedBoundaryContainer(Grid *grid);
    ~ImmersedBoundaryContainer();
    std::list<ImmersedItem*>* getItems();
    void preUpdate1();
    void preUpdate2(int x, int y, int z);
    int addItem(ImmersedItem *item);
    void removeItem(int id);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
private:
    Grid *grid;
    std::list<ImmersedItem*>* items;
    std::list<MyTwoVector3D> forces;
};

#endif // IMMERSEDBOUNDARYCONTAINER_H
