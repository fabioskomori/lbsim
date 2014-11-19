#include "mytwovector3d.h"

MyTwoVector3D::MyTwoVector3D(MyVector3D v1, MyVector3D v2) : v1(v1), v2(v2) {
}

MyVector3D MyTwoVector3D::getV1() {
    return v1;
}

MyVector3D MyTwoVector3D::getV2() {
    return v2;
}
