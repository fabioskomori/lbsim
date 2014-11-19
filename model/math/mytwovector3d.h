#ifndef MYTWOVECTOR3D_H
#define MYTWOVECTOR3D_H

#include "myvector3d.h"

class MyTwoVector3D {
public:
    MyTwoVector3D(MyVector3D v1, MyVector3D v2);
    MyVector3D getV1();
    MyVector3D getV2();
private:
    MyVector3D v1;
    MyVector3D v2;
};

#endif // MYTWOVECTOR3D_H
