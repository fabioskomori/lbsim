#ifndef FILEITEM_H
#define FILEITEM_H

#include "immerseditem.h"

class FileItem : public ImmersedItem {
public:
    FileItem();
    void create();
    QString getType();
};

#endif // FILEITEM_H
