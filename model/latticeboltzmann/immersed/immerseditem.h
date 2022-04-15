#ifndef IMMERSEDITEM_H
#define IMMERSEDITEM_H

#include "../../model/math/myvector3d.h"
#include <list>
#include <QString>
#include <map>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class ImmersedItem {
public:
    ImmersedItem();
    ~ImmersedItem();
    void calculateCenterOfMass();
    MyVector3D getCenterOfMass();
    std::list<MyVector3D>* getPoints();
    void setPoints(std::list<MyVector3D> *points);
    double getParameter(QString name);
    int getIntParameter(QString name);
    QString getStringParameter(QString name);
    void setParameter(QString name, double value);
    QString parametersToString();
    void parametersFromString(QString string);
    virtual void create() = 0;
    void setId(int id);
    int getId();
    void setClosed(bool closed);
    bool isClosed();
    void setCellular(bool cellular);
    bool isCellular();
    MyVector3D getFixedPoint();
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader);
    virtual QString getType() = 0;
protected:
    std::list<MyVector3D> *points;
    std::map<QString, QString> parameters;
    MyVector3D centerOfMass;
    MyVector3D fixedPoint;
    int id;
    bool closed;
    bool cellular;
};

#endif // IMMERSEDITEM_H
