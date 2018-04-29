#ifndef OPTSCHANGER_HPP
#define OPTSCHANGER_HPP

#include <vector>

#include <QItemSelectionModel>
#include <QScrollArea>
#include <QSpinBox>
#include <QStandardItemModel>

#include "customlistitems.hpp"

class OptsChanger : public QScrollArea {
    Q_OBJECT
public:
    explicit OptsChanger(QWidget* parent = nullptr);
    void setModel(QStandardItemModel*);
    void setSelectionModel(QItemSelectionModel*);
private:
    QVariant animGroup;
    QStandardItemModel* model = nullptr;
    QItemSelectionModel* selModel = nullptr;
    std::vector<QWidget*> widgets;

    QSpinBox* lowerBox;
    QSpinBox* upperBox;

    AnimOptions getOpts();

    void clearWidgets();
    void connectWidgets();
    void initAndDisplayWidgets();

    void drawRange();
    void drawFields();
    void drawBattle();
    void drawDirect();
    void drawTrap();
private slots:
    void drawSelectedItem();
    void setSpinBoxBounds();
    void writeChanges();
};

#endif
