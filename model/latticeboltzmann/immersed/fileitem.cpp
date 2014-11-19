#include "fileitem.h"

#include <QImage>
#include <QDebug>
#include <QColor>

FileItem::FileItem() {
    parameters["file"] = "";
    parameters["x"] = "0";
    parameters["y"] = "0";
}

void FileItem::create() {
    QImage image;
    image.load(parameters["file"]);
    fixedPoint = MyVector3D(-1, -1, -1);
    for (int x = 0; x < image.width(); x++) {
        for (int y = 0; y < image.height(); y++) {
            if (QColor(image.pixel(x, image.height() - 1 - y)) == QColor(0, 0, 255)) {
                points->push_back(MyVector3D(getParameter("x") + x, getParameter("y") + y, 0));
            } else if (QColor(image.pixel(x, image.height() - 1 - y)) == QColor(255, 0, 0)) {
                fixedPoint = MyVector3D(getParameter("x") + x, getParameter("y") + y, 0);
            }
        }
    }
    cellular = true;
}

QString FileItem::getType() {
    return "File";
}
