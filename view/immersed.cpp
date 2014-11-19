#include "immersed.h"
#include "ui_immersed.h"

#include "model/latticeboltzmann/immersed/circleitem.h"
#include "model/latticeboltzmann/immersed/lineitem.h"
#include "model/latticeboltzmann/immersed/rectangleitem.h"
#include "model/latticeboltzmann/immersed/fileitem.h"
#include "lbwidget.h"
#include "painter/painter.h"
#include "model/latticeboltzmann/lbgrid.h"
#include "model/latticeboltzmann/immersed/immersedboundarycontainer.h"
#include <QDebug>

Immersed::Immersed(QWidget *parent) : QDialog(parent), ui(new Ui::Immersed) {
    ui->setupUi(this);
    ui->immersedType->addItem("Circle");
    ui->immersedType->addItem("Line");
    ui->immersedType->addItem("Rectangle");
    ui->immersedType->addItem("File");
}

Immersed::~Immersed() {
    delete ui;
}

void Immersed::on_immersedType_currentIndexChanged(const QString &immersed) {
    ui->immersedParameters->clear();
    ImmersedItem *item = create();
    ui->immersedParameters->setPlainText(item->parametersToString());
    delete item;
}

void Immersed::on_immersedAdd_clicked() {
    ImmersedItem *item = create();
    item->parametersFromString(ui->immersedParameters->toPlainText());
    item->create();
    item->calculateCenterOfMass();
    int id = widget->getPainter()->getGrid()->getImmersed()->addItem(item);
    ui->immersedList->addItem(QString::number(id) + " " + ui->immersedType->currentText() + " " + ui->immersedParameters->toPlainText().replace("\n", " "));
    widget->updateGL();
}

ImmersedItem* Immersed::create() {
    if (ui->immersedType->currentText() == "Circle") {
        return new CircleItem();
    } else if (ui->immersedType->currentText() == "Line") {
        return new LineItem();
    } else if (ui->immersedType->currentText() == "Rectangle") {
        return new RectangleItem();
    } else if (ui->immersedType->currentText() == "File") {
        return new FileItem();
    }
    return 0;
}

void Immersed::inject(LBWidget *widget) {
    this->widget = widget;
}

void Immersed::sync() {
    ui->immersedList->clear();
    std::list<ImmersedItem*> *items = widget->getPainter()->getGrid()->getImmersed()->getItems();
    for (std::list<ImmersedItem*>::iterator item = items->begin(); item != items->end(); item++) {
        ui->immersedList->addItem((*item)->getId() + " " + (*item)->getType() + " " + (*item)->parametersToString().replace("\n", " "));
    }
}

void Immersed::on_immersedRemove_clicked() {
    if (ui->immersedList->currentRow() >= 0) {
        widget->getPainter()->getGrid()->getImmersed()->removeItem(ui->immersedList->currentItem()->text().split(" ").at(0).toInt());
        ui->immersedList->model()->removeRow(ui->immersedList->currentRow());
        widget->updateGL();
    }
}
