#ifndef EASYMODEFINDER_HPP
#define EASYMODEFINDER_HPP

#include <memory>
#include <vector>

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QWidget>

#include "fmdata.hpp"

namespace Ui {
    class EasyModeForm;
}

class EasyModeFinder : public QWidget {
    Q_OBJECT
public:
    EasyModeFinder(std::shared_ptr<FMData>, QWidget* parent = nullptr);
    ~EasyModeFinder();
private:
    Ui::EasyModeForm* ui;

    const std::shared_ptr<FMData> fmData;
    QStandardItemModel* allowedCardsModel;
    QStandardItemModel* easyModesModel;
    QItemSelectionModel* easyModesSelModel;

    std::vector<int> forbiddenCards();
    void drawBoxes(int);
private slots:
    void drawPlays();
    void on_findEasyBtn_clicked();
    void opponentChanged(int);
};

#endif
