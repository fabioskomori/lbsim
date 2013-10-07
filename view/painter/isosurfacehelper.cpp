/*
    LBSim: a fluid dynamics simulator using the lattice Boltzmann method
    Copyright (C) 2013 Fabio Sussumu Komori - NDS/GNMD/LME/PSI/EP/USP

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "isosurfacehelper.h"
#include <cstdlib>
#include <cmath>
#include <QDebug>

IsosurfaceHelper::IsosurfaceHelper(int point1, int point2, double value) {
    points.push_back(point1);
    points.push_back(point2);
    values.push_back(value);
}

bool IsosurfaceHelper::isNeighbor(IsosurfaceHelper ih) {
    for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
        for (std::list<int>::iterator other = ih.points.begin(); other != ih.points.end(); other++) {
            if (std::abs(*point - *other) == 0 || std::abs(*point - *other) == 1 || std::abs(*point - *other) == 2 || std::abs(*point - *other) == 4) {
                return true;
            }
            other++;
        }
        point++;
    }
    return false;
}

int IsosurfaceHelper::count() {
    return values.size();
}

void IsosurfaceHelper::order() {
    if (values.size() == 4) {
        int origins[] = {0, 0, 0, 0, 0, 0, 0, 0};
        bool two = false;
        for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
            origins[*point]++;
            if (origins[*point] == 2) {
                two = true;
            }
            point++;
        }
        std::list<int> newPoints;
        std::list<double> newValues;
        std::list<int>::iterator point = points.begin();
        std::list<double>::iterator value = values.begin();
        if (two) {
            for (int i = 0; i < 8; i++) {
                if (origins[i] != 0) {
                    value = values.begin();
                    for (point = points.begin(); point != points.end(); point++) {
                        if (*point == i) {
                            newPoints.push_back(*point);
                            point++;
                            newPoints.push_back(*point);
                            newValues.push_back(*value);
                        } else {
                            point++;
                        }
                        value++;
                    }
                }
            }
        } else {
            int firstPoint = *point;
            newPoints.push_back(*point);
            point++;
            newPoints.push_back(*point);
            newValues.push_back(*value);
            value = values.begin();
            for (point = points.begin(); point != points.end(); point++) {
                if (std::abs(*point - firstPoint) == 1 || std::abs(*point - firstPoint) == 2 || std::abs(*point - firstPoint) == 4) {
                    newPoints.push_back(*point);
                    point++;
                    newPoints.push_back(*point);
                    newValues.push_back(*value);
                } else {
                    point++;
                }
                value++;
            }
            value = values.begin();
            for (point = points.begin(); point != points.end(); point++) {
                if (std::abs(*point - firstPoint) != 0 && std::abs(*point - firstPoint) != 1 && std::abs(*point - firstPoint) != 2 && std::abs(*point - firstPoint) != 4) {
                    newPoints.push_back(*point);
                    point++;
                    newPoints.push_back(*point);
                    newValues.push_back(*value);
                } else {
                    point++;
                }
                value++;
            }
        }
        points = newPoints;
        values = newValues;
    } else if (values.size() == 5) {
        int connections[] = {0, 0, 0, 0, 0, 0, 0, 0};
        for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
            connections[*point]++;
        }
        std::list<int> newPoints;
        std::list<double> newValues;
        std::list<double>::iterator value = values.begin();
        for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
            int point1 = *point;
            point++;
            int point2 = *point;
            if (connections[point1] == 1 || connections[point2] == 1) {
                newPoints.push_back(point1);
                newPoints.push_back(point2);
                newValues.push_back(*value);
            }
            value++;
        }
        value = values.begin();
        for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
            int point1 = *point;
            point++;
            int point2 = *point;
            if (connections[point1] == 2 && connections[point2] == 2) {
                newPoints.push_back(point1);
                newPoints.push_back(point2);
                newValues.push_back(*value);
            }
            value++;
        }
        points = newPoints;
        values = newValues;
    } else if (values.size() == 6) {
        points.clear();
        values.clear();
    }
}

bool IsosurfaceHelper::equals(IsosurfaceHelper other) {
    if (points.size() != other.points.size()) {
        return false;
    }
    std::list<int>::iterator o = other.points.begin();
    for (std::list<int>::iterator point = points.begin(); point != points.end(); point++) {
        if (*point != *o) {
            return false;
        }
        o++;
    }
    return true;
}

void IsosurfaceHelper::merge(IsosurfaceHelper other) {
    for (std::list<int>::iterator point = other.points.begin(); point != other.points.end(); point++) {
        points.push_back(*point);
    }
    for (std::list<double>::iterator value = other.values.begin(); value != other.values.end(); value++) {
        values.push_back(*value);
    }
}

std::list<int> IsosurfaceHelper::getPoints() {
    return points;
}

std::list<double> IsosurfaceHelper::getValues() {
    return values;
}
