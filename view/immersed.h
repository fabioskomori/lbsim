#ifndef IMMERSED_H
#define IMMERSED_H

#include <QDialog>

namespace Ui {
    class Immersed;
}

class LBWidget;
class ImmersedItem;

class Immersed : public QDialog {
    Q_OBJECT
public:
    explicit Immersed(QWidget *parent = 0);
    ~Immersed();
    void inject(LBWidget *widget);
    void sync();
private slots:
    void on_immersedAdd_clicked();
    void on_immersedType_currentIndexChanged(const QString &arg1);
    void on_immersedRemove_clicked();
private:
    ImmersedItem* create();
    Ui::Immersed *ui;
    LBWidget *widget;
};

#endif // IMMERSED_H
