#include "rectangleitem.h"

#include <QString>

RectangleItem::RectangleItem() {
    parameters["x"] = "5";
    parameters["y"] = "5";
    parameters["width"] = "5";
    parameters["height"] = "5";
}

void RectangleItem::create() {
    points->push_back(MyVector3D(getParameter("x"), getParameter("y"), 0));
    points->push_back(MyVector3D(getParameter("x") + getParameter("width"), getParameter("y"), 0));
    points->push_back(MyVector3D(getParameter("x") + getParameter("width"), getParameter("y") + getParameter("height"), 0));
    points->push_back(MyVector3D(getParameter("x"), getParameter("y") + getParameter("height"), 0));
    closed = true;
}

QString RectangleItem::getType() {
    return "Rectangle";
}
