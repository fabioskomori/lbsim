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

#include "vtkreporter.h"
#include <QXmlStreamWriter>
#include <QFile>
#include "../../model/latticeboltzmann/lbgrid.h"
#include "../../model/latticeboltzmann/gridconfig.h"
#include "../../model/latticeboltzmann/basecell.h"
#include <QDebug>

VTKReporter::VTKReporter() {
}

void VTKReporter::exportVTK(Grid *grid, QString fileName) {
    int cells = 0;
    for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
            for (int z = 0; z < grid->getConfig()->getLength(); z++) {
                if (grid->getGrid(y, x, z)->isFluid()) {
                    cells++;
                }
            }
        }
    }
    std::map<QString, QString> points;
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QXmlStreamWriter writer(&file);
    writer.writeStartDocument();
    writer.writeStartElement("VTKFile");
    writer.writeAttribute("type", "UnstructuredGrid");
    //writer.writeAttribute("version", "0.1");
    //writer.writeAttribute("byte_order", "LittleEndian");
        writer.writeStartElement("UnstructuredGrid");
        //writer.writeAttribute("WholeExtent", QString("1 ").append(QString::number(grid->getConfig()->getWidth())).append(" 1 ").append(QString::number(grid->getConfig()->getHeight())).append(" 1 ").append(QString::number(grid->getConfig()->getLength())));
            writer.writeStartElement("Piece");
            writer.writeAttribute("NumberOfPoints", QString::number((grid->getConfig()->getWidth() + 1) * (grid->getConfig()->getHeight() + 1) * (grid->getConfig()->getLength() + 1)));
            writer.writeAttribute("NumberOfCells", QString::number(cells));
                writer.writeStartElement("Points");
                    writer.writeStartElement("DataArray");
                    writer.writeAttribute("type", "Int32");
                    writer.writeAttribute("NumberOfComponents", "3");
                    writer.writeAttribute("format", "ascii");
                    int counter = 0;
                    for (int x = 0; x < grid->getConfig()->getWidth() + 1; x++) {
                        for (int y = 0; y < grid->getConfig()->getHeight() + 1; y++) {
                            for (int z = 0; z < grid->getConfig()->getLength() + 1; z++) {
                                points[QString::number(x).append(" ").append(QString::number(y)).append(" ").append(QString::number(z))] = QString::number(counter++);
                                writer.writeCharacters(QString::number(x).append(" ").append(QString::number(y)).append(" ").append(QString::number(z)).append(" "));
                            }
                        }
                    }
                    writer.writeEndElement();
                writer.writeEndElement();
                writer.writeStartElement("Cells");
                    writer.writeStartElement("DataArray");
                    writer.writeAttribute("type", "Int32");
                    writer.writeAttribute("Name", "connectivity");
                    for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
                        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
                            for (int z = 0; z < grid->getConfig()->getLength(); z++) {
                                if (grid->getGrid(y, x, z)->isFluid()) {
                                    writer.writeCharacters(points[QString::number(x).append(" ").append(QString::number(y)).append(" ").append(QString::number(z))].append(" "));
                                    writer.writeCharacters(points[QString::number(x + 1).append(" ").append(QString::number(y)).append(" ").append(QString::number(z))].append(" "));
                                    writer.writeCharacters(points[QString::number(x).append(" ").append(QString::number(y + 1)).append(" ").append(QString::number(z))].append(" "));
                                    writer.writeCharacters(points[QString::number(x + 1).append(" ").append(QString::number(y + 1)).append(" ").append(QString::number(z))].append(" "));
                                    writer.writeCharacters(points[QString::number(x).append(" ").append(QString::number(y)).append(" ").append(QString::number(z + 1))].append(" "));
                                    writer.writeCharacters(points[QString::number(x + 1).append(" ").append(QString::number(y)).append(" ").append(QString::number(z + 1))].append(" "));
                                    writer.writeCharacters(points[QString::number(x).append(" ").append(QString::number(y + 1)).append(" ").append(QString::number(z + 1))].append(" "));
                                    writer.writeCharacters(points[QString::number(x + 1).append(" ").append(QString::number(y + 1)).append(" ").append(QString::number(z + 1))].append(" "));
                                }
                            }
                        }
                    }
                    writer.writeEndElement();
                    writer.writeStartElement("DataArray");
                    writer.writeAttribute("type", "Int32");
                    writer.writeAttribute("Name", "offsets");
                    for (int i = 0; i < cells; i++) {
                        writer.writeCharacters(QString::number((i + 1) * 8).append(" "));
                    }
                    writer.writeEndElement();
                    writer.writeStartElement("DataArray");
                    writer.writeAttribute("type", "UInt8");
                    writer.writeAttribute("Name", "types");
                    for (int i = 0; i < cells; i++) {
                        writer.writeCharacters("11 ");
                    }
                    writer.writeEndElement();
                writer.writeEndElement();
                writer.writeStartElement("CellData");
                writer.writeAttribute("Scalars", "density");
                writer.writeAttribute("Vectors", "velocity");
                    writer.writeStartElement("DataArray");
                    writer.writeAttribute("type", "Float64");
                    writer.writeAttribute("Name", "density");
                    writer.writeAttribute("format", "ascii");
                    for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
                        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
                            for (int z = 0; z < grid->getConfig()->getLength(); z++) {
                                BaseCell *cell = grid->getGrid(y, x, z);
                                if (cell->isFluid()) {
                                    writer.writeCharacters(QString::number(cell->getP(0)).append(" "));
                                }
                            }
                        }
                    }
                    writer.writeEndElement();
                    writer.writeStartElement("DataArray");
                    writer.writeAttribute("type", "Float64");
                    writer.writeAttribute("Name", "velocity");
                    writer.writeAttribute("NumberOfComponents", "3");
                    writer.writeAttribute("format", "ascii");
                    for (int x = 0; x < grid->getConfig()->getWidth(); x++) {
                        for (int y = 0; y < grid->getConfig()->getHeight(); y++) {
                            for (int z = 0; z < grid->getConfig()->getLength(); z++) {
                                BaseCell *cell = grid->getGrid(y, x, z);
                                if (cell->isFluid()) {
                                    writer.writeCharacters(QString::number(cell->getU(0).getX()).append(" ").append(QString::number(cell->getU(0).getY())).append(" ").append(QString::number(cell->getU(0).getZ())).append(" "));
                                }
                            }
                        }
                    }
                    writer.writeEndElement();
                writer.writeEndElement();
            writer.writeEndElement();
        writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}
