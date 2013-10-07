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

#ifndef LISTENER_H
#define LISTENER_H

#define LISTENER_EVENT_GRIDUPDATE 1
#define LISTENER_EVENT_RESET 2
#define LISTENER_EVENT_INIT 3
#define LISTENER_EVENT_ACTIVATE 4

class Listener {
public:
    virtual void callback(int event) = 0;
};

#endif // LISTENER_H
