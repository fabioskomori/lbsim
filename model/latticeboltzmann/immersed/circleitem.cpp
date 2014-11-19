#include "circleitem.h"

#include <cmath>
#include <QString>
#include <QDebug>

#define PI 3.14159265

CircleItem::CircleItem() {
    parameters["points"] = "10";
    parameters["x"] = "10";
    parameters["y"] = "10";
    parameters["radius"] = "10";
}

void CircleItem::create() {
    for (int i = 0; i < getIntParameter(QString("points")); i++) {
        points->push_back(MyVector3D(getParameter("x") + getParameter("radius") * std::cos(2 * PI * i / getParameter("points")), getParameter("y") + getParameter("radius") * std::sin(2 * PI * i / getParameter("points")), 0));
    }
}

QString CircleItem::getType() {
    return "Circle";
}
