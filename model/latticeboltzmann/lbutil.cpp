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

#include "lbutil.h"
#include "../../model/math/vector3i.h"
#include "../../model/math/myvector3d.h"
#include "basecell.h"

Vector3i LBUtil::C[28][27] = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {
    Vector3i(0, 0, 0),
    Vector3i(1, 0, 0),
    Vector3i(1, 1, 0),
    Vector3i(0, 1, 0),
    Vector3i(-1, 1, 0),
    Vector3i(-1, 0, 0),
    Vector3i(-1, -1, 0),
    Vector3i(0, -1, 0),
    Vector3i(1, -1, 0)
}, {}, {}, {}, {}, {}, {
    Vector3i(0, 0, 0),
    Vector3i(1, 0, 0),
    Vector3i(0, 1, 0),
    Vector3i(-1, 0, 0),
    Vector3i(0, -1, 0),
    Vector3i(0, 0, -1),
    Vector3i(1, 1, -1),
    Vector3i(-1, 1, -1),
    Vector3i(-1, -1, -1),
    Vector3i(1, -1, -1),
    Vector3i(0, 0, 1),
    Vector3i(1, 1, 1),
    Vector3i(-1, 1, 1),
    Vector3i(-1, -1, 1),
    Vector3i(1, -1, 1)
}, {}, {}, {}, {
    Vector3i(0, 0, 0),
    Vector3i(1, 0, 0),
    Vector3i(1, 1, 0),
    Vector3i(0, 1, 0),
    Vector3i(-1, 1, 0),
    Vector3i(-1, 0, 0),
    Vector3i(-1, -1, 0),
    Vector3i(0, -1, 0),
    Vector3i(1, -1, 0),
    Vector3i(0, 0, -1),
    Vector3i(1, 0, -1),
    Vector3i(0, 1, -1),
    Vector3i(-1, 0, -1),
    Vector3i(0, -1, -1),
    Vector3i(0, 0, 1),
    Vector3i(1, 0, 1),
    Vector3i(0, 1, 1),
    Vector3i(-1, 0, 1),
    Vector3i(0, -1, 1)
}, {}, {}, {}, {}, {}, {}, {}, {
    Vector3i(0, 0, 0),
    Vector3i(1, 0, 0),
    Vector3i(1, 1, 0),
    Vector3i(0, 1, 0),
    Vector3i(-1, 1, 0),
    Vector3i(-1, 0, 0),
    Vector3i(-1, -1, 0),
    Vector3i(0, -1, 0),
    Vector3i(1, -1, 0),
    Vector3i(0, 0, -1),
    Vector3i(1, 0, -1),
    Vector3i(1, 1, -1),
    Vector3i(0, 1, -1),
    Vector3i(-1, 1, -1),
    Vector3i(-1, 0, -1),
    Vector3i(-1, -1, -1),
    Vector3i(0, -1, -1),
    Vector3i(1, -1, -1),
    Vector3i(0, 0, 1),
    Vector3i(1, 0, 1),
    Vector3i(1, 1, 1),
    Vector3i(0, 1, 1),
    Vector3i(-1, 1, 1),
    Vector3i(-1, 0, 1),
    Vector3i(-1, -1, 1),
    Vector3i(0, -1, 1),
    Vector3i(1, -1, 1)
}};

double LBUtil::W[28][27] = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {
        // d2q9
        4.0 / 9, 1.0 / 9, 1.0 / 36, 1.0 / 9, 1.0 / 36, 1.0 / 9, 1.0 / 36, 1.0 / 9, 1.0 / 36,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }, {}, {}, {}, {}, {}, {
        // d3q15
        2.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9,
        1.0 / 9, 1.0 / 72, 1.0 / 72, 1.0 / 72, 1.0 / 72,
        1.0 / 9, 1.0 / 72, 1.0 / 72, 1.0 / 72, 1.0 / 72,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }, {}, {}, {}, {
        // d3q19
        12.0 / 36, 2.0 / 36, 1.0 / 36, 2.0 / 36, 1.0 / 36, 2.0 / 36, 1.0 / 36, 2.0 / 36, 1.0 / 36,
        2.0 / 36, 1.0 / 36, 1.0 / 36, 1.0 / 36, 1.0 / 36,
        2.0 / 36, 1.0 / 36, 1.0 / 36, 1.0 / 36, 1.0 / 36,
        0, 0, 0, 0, 0, 0, 0, 0
    }, {}, {}, {}, {}, {}, {}, {}, {
        // d3q27
        8.0 / 27, 2.0 / 27, 1.0 / 54, 2.0 / 27, 1.0 / 54, 2.0 / 27, 1.0 / 54, 2.0 / 27, 1.0 / 54,
        2.0 / 27, 1.0 / 54, 1.0 / 216, 1.0 / 54, 1.0 / 216, 1.0 / 54, 1.0 / 216, 1.0 / 54, 1.0 / 216,
        2.0 / 27, 1.0 / 54, 1.0 / 216, 1.0 / 54, 1.0 / 216, 1.0 / 54, 1.0 / 216, 1.0 / 54, 1.0 / 216
    }
};

int LBUtil::OPPOSITE[28][27] = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {
    0, 5, 6, 7, 8, 1, 2, 3, 4,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}, {}, {}, {}, {}, {}, {
    0, 3, 4, 1, 2, 10, 13, 14, 11, 12, 5, 8, 9, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}, {}, {}, {}, {
    0, 5, 6, 7, 8, 1, 2, 3, 4, 14, 17, 18, 15, 16, 9, 12, 13, 10, 11,
    0, 0, 0, 0, 0, 0, 0, 0
}, {}, {}, {}, {}, {}, {}, {}, {
    0, 5, 6, 7, 8, 1, 2, 3, 4,
    18, 23, 24, 25, 26, 19, 20, 21, 22,
    9, 14, 15, 16, 17, 10, 11, 12, 13
}};

void LBUtil::f_eq(const MyVector3D &u, double p, int model, double f[]) {
    for (int i = 0; i < model; i++) {
        f[i] = f_eq(u, p, model, i);
    }
}

double LBUtil::f_eq(const MyVector3D &u, double p, int model, int i) {
    double result = 0;
    if (LBUtil::W[model][i] > 1e-100) {
        double scalar = LBUtil::C[model][i] * u;
        result = p * LBUtil::W[model][i] * (1 + 3 * scalar +
                4.5 * scalar * scalar - 1.5 * (u * u));
    }
    return result;
}

double LBUtil::f_eq2(const MyVector3D &u, double p, int model, int i) {
    double result = 0;
    if (LBUtil::W[model][i] > 1e-100) {
        double scalar = LBUtil::C[model][i] * u;
        result = p * LBUtil::W[model][i] * (1 + 3 * scalar);
    }
    return result;
}

void LBUtil::stream(int model, BaseCell* neighbors[], double nextF[]) {
    for (int i = 0; i < model; i++) {
        BaseCell* neighbor = neighbors[LBUtil::OPPOSITE[model][i]];
        if (neighbor != 0) {
            nextF[i] = neighbor->getF(i, -1);
        } else {
            nextF[i] = -2;
        }
    }
}

void LBUtil::calc_pu(double f[], int model, double *p, MyVector3D *u) {
    *p = 0;
    MyVector3D pu = MyVector3D(0, 0, 0);
    for (int i = 0; i < model; i++) {
        if (W[model][i] > 1e-100) {
            *p += f[i];
            pu = pu + (LBUtil::C[model][i] ^ f[i]);
        }
    }
    *u = (*p > 1e-100 ? pu ^ (1.0 / *p) : MyVector3D(0, 0, 0));
}
