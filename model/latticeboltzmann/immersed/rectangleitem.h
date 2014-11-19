#ifndef RECTANGLEITEM_H
#define RECTANGLEITEM_H

#include "immerseditem.h"

class RectangleItem : public ImmersedItem {
public:
    RectangleItem();
    void create();
    QString getType();
};

#endif // RECTANGLEITEM_H
