#ifndef CIRCLEITEM_H
#define CIRCLEITEM_H

#include "immerseditem.h"

class CircleItem : public ImmersedItem {
public:
    CircleItem();
    void create();
    QString getType();
};

#endif // CIRCLEITEM_H
