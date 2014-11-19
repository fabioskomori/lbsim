#include "immerseditem.h"
#include <QString>
#include <QStringList>
#include <QDebug>

ImmersedItem::ImmersedItem() {
    points = new std::list<MyVector3D>();
    parameters["k"] = "1";
    parameters["rigidBody"] = "1";
    closed = true;
    cellular = false;
    fixedPoint = MyVector3D(-1, -1, -1);
}

void ImmersedItem::calculateCenterOfMass() {
    centerOfMass = MyVector3D();
    int count = 0;
    for (std::list<MyVector3D>::iterator point = points->begin(); point != points->end(); point++) {
        centerOfMass = centerOfMass + (*point);
        count++;
    }
    centerOfMass = centerOfMass ^ (1.0 / count);
}

MyVector3D ImmersedItem::getCenterOfMass() {
    return centerOfMass;
}

void ImmersedItem::setClosed(bool closed) {
    this->closed = closed;
}

bool ImmersedItem::isClosed() {
    return closed;
}

void ImmersedItem::setCellular(bool cellular) {
    this->cellular = cellular;
}

bool ImmersedItem::isCellular() {
    return cellular;
}

ImmersedItem::~ImmersedItem() {
    delete points;
}

std::list<MyVector3D>* ImmersedItem::getPoints() {
    return points;
}

void ImmersedItem::setPoints(std::list<MyVector3D> *points) {
    delete this->points;
    this->points = points;
}

void ImmersedItem::setParameter(QString name, double value) {
    this->parameters[name] = value;
}

double ImmersedItem::getParameter(QString name) {
    return this->parameters[name].toDouble();
}

int ImmersedItem::getIntParameter(QString name) {
    return this->parameters[name].toInt();
}

QString ImmersedItem::getStringParameter(QString name) {
    return this->parameters[name];
}

QString ImmersedItem::parametersToString() {
    QString toString;
    for (std::map<QString, QString>::iterator ps = parameters.begin(); ps != parameters.end(); ps++) {
        toString += (*ps).first + "=" + (*ps).second + "\n";
    }
    return toString;
}

void ImmersedItem::parametersFromString(QString string) {
    QStringList stringList = string.split("\n");
    for (int i = 0; i < stringList.size(); i++) {
        QStringList ps = stringList.at(i).split("=");
        parameters[ps.at(0)] = ps.at(1);
    }
}

void ImmersedItem::passivate(QXmlStreamWriter &writer) {
    writer.writeAttribute("pointsSize", QString::number(points->size()));
    writer.writeAttribute("parametersSize", QString::number(parameters.size()));
    writer.writeAttribute("centerOfMassX", QString::number(centerOfMass.getX()));
    writer.writeAttribute("centerOfMassY", QString::number(centerOfMass.getY()));
    writer.writeAttribute("centerOfMassZ", QString::number(centerOfMass.getZ()));
    writer.writeAttribute("fixedPointX", QString::number(fixedPoint.getX()));
    writer.writeAttribute("fixedPointY", QString::number(fixedPoint.getY()));
    writer.writeAttribute("fixedPointZ", QString::number(fixedPoint.getZ()));
    writer.writeAttribute("id", QString::number(id));
    writer.writeAttribute("closed", closed ? "1" : "0");
    writer.writeAttribute("cellular", cellular ? "1" : "0");
    for (std::list<MyVector3D>::iterator point = points->begin(); point != points->end(); point++) {
        writer.writeStartElement("point");
        writer.writeAttribute("x", QString::number((*point).getX()));
        writer.writeAttribute("y", QString::number((*point).getY()));
        writer.writeAttribute("z", QString::number((*point).getZ()));
        writer.writeEndElement();
    }
    for (std::map<QString, QString>::iterator parameter = parameters.begin(); parameter != parameters.end(); parameter++) {
        writer.writeStartElement("parameter");
        writer.writeAttribute("key", (*parameter).first);
        writer.writeAttribute("value", (*parameter).second);
        writer.writeEndElement();
    }
}

void ImmersedItem::activate(QXmlStreamReader &reader) {
    if (reader.name().toString() == "item") {
        int pointsSize = reader.attributes().value("pointsSize").toString().toInt();
        int parametersSize = reader.attributes().value("parametersSize").toString().toInt();
        centerOfMass = MyVector3D(reader.attributes().value("centerOfMassX").toString().toDouble(), reader.attributes().value("centerOfMassY").toString().toDouble(), reader.attributes().value("centerOfMassZ").toString().toDouble());
        fixedPoint = MyVector3D(reader.attributes().value("fixedPointX").toString().toDouble(), reader.attributes().value("fixedPointY").toString().toDouble(), reader.attributes().value("fixedPointZ").toString().toDouble());
        id = reader.attributes().value("id").toString().toInt();
        closed = (reader.attributes().value("closed").toString() == "1" ? true : false);
        cellular = (reader.attributes().value("cellular").toString() == "1" ? true : false);
        points->clear();
        for (int i = 0; i < pointsSize; i++) {
            do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
            points->push_back(MyVector3D(reader.attributes().value("x").toString().toDouble(), reader.attributes().value("y").toString().toDouble(), reader.attributes().value("z").toString().toDouble()));
        }
        parameters.clear();
        for (int i = 0; i < parametersSize; i++) {
            do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
            parameters[reader.attributes().value("key").toString()] = reader.attributes().value("value").toString();
        }
        do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    }
}

void ImmersedItem::setId(int id) {
    this->id = id;
}

int ImmersedItem::getId() {
    return id;
}

MyVector3D ImmersedItem::getFixedPoint() {
    return fixedPoint;
}
