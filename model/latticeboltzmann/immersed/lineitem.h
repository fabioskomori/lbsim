#ifndef LINEITEM_H
#define LINEITEM_H

#include "immerseditem.h"

class LineItem : public ImmersedItem {
public:
    LineItem();
    void create();
    QString getType();
};

#endif // LINEITEM_H
