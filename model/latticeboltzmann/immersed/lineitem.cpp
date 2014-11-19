#include "lineitem.h"

#include <QString>

LineItem::LineItem() {
    parameters["points"] = "10";
    parameters["x1"] = "10";
    parameters["y1"] = "10";
    parameters["x2"] = "20";
    parameters["y2"] = "20";
}

void LineItem::create() {
    for (int i = 0; i < getIntParameter("points"); i++) {
        points->push_back(MyVector3D(getParameter("x1") + i * (getParameter("x2") - getParameter("x1")) / getIntParameter("points"),
                                                  getParameter("y1") + i * (getParameter("y2") - getParameter("y1")) / getIntParameter("points"), 0));
    }
    closed = false;
}

QString LineItem::getType() {
    return "Line";
}
