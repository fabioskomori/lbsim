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

#include "commandpool.h"
#include "densitycommand.h"
#include "nextcommand.h"
#include "loadcommand.h"
#include "startcommand.h"
#include "resetcommand.h"
#include "saveall2command.h"
#include "setbackgroundcommand.h"
#include "setgravitycommand.h"
#include "setinitialconditionscommand.h"
#include "setmaxiterationscommand.h"
#include "setmovingwallscommand.h"
#include "setmultiphasecommand.h"
//#include "setopenclcommand.h"
#include "setp0command.h"
#include "setperformancecommand.h"
#include "setperformanceupdate.h"
#include "setpermeabilitycommand.h"
//#include "setpressurecommand.h"
#include "setresultscommand.h"
#include "setsmagorinskycommand.h"
#include "setstrategycommand.h"
#include "settaucommand.h"
#include "setthermalcommand.h"
#include "setvelocitycommand.h"
#include "setzoomcommand.h"
#include "startblockingcommand.h"
#include "stopcommand.h"
#include "viewxmlcommand.h"
#include "getcommand.h"
#include <QXmlStreamReader>

Grid* CommandPool::grid;
LBMainWindow* CommandPool::mainWindow;

CommandPool::CommandPool() {
}

void CommandPool::init(Grid *grid, LBMainWindow *mainWindow) {
    CommandPool::grid = grid;
    CommandPool::mainWindow = mainWindow;
}

std::list<Command*>* CommandPool::list() {
    static std::list<Command*> *commands = 0;
    if (commands == 0) {
        commands = new std::list<Command*>();
        commands->push_back(new DensityCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new GetCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new LoadCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new NextCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new ResetCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SaveAll2Command(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetBackgroundCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetGravityCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetInitialConditionsCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetMaxIterationsCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetMovingWallsCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetMultiphaseCommand(CommandPool::grid, CommandPool::mainWindow));
        //commands->push_back(new SetOpenCLCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetP0Command(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetPerformanceCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetPerformanceUpdate(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetPermeabilityCommand(CommandPool::grid, CommandPool::mainWindow));
        //commands->push_back(new SetPressureCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetResultsCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetSmagorinskyCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetStrategyCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetTauCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetThermalCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetVelocityCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new SetZoomCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new StartCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new StartBlockingCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new StopCommand(CommandPool::grid, CommandPool::mainWindow));
        commands->push_back(new ViewXMLCommand(CommandPool::grid, CommandPool::mainWindow));
    }
    return commands;
}

Command* CommandPool::activate(QXmlStreamReader &reader) {
    do { reader.readNext(); } while (!reader.isStartElement() && !reader.isEndDocument());
    Command *command = commandByName(reader.attributes().value("name").toString());
    int parameters = reader.attributes().value("parametersSize").toString().toInt();
    for (int i = 0; i < parameters; i++) {
        command->addParameter(reader.attributes().value(QString("parameter").append(QString::number(i))).toString());
    }
    return command;
}

Command* CommandPool::commandByName(QString name) {
    std::list<Command*>* commands = list();
    for (std::list<Command*>::iterator command = commands->begin(); command != commands->end(); command++) {
        if ((*command)->name() == name) {
            return (*command)->clone();
        }
    }
    return NULL;
}
