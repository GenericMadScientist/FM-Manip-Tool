#ifndef STARTSEARCHER_HPP
#define STARTSEARCHER_HPP

#include <memory>
#include <vector>

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QWidget>

#include "fmdata.hpp"

namespace Ui {
    class StartSearchForm;
}

class StartSearcher : public QWidget {
    Q_OBJECT
public:
    StartSearcher(std::shared_ptr<FMData>, QWidget* parent = nullptr);
    ~StartSearcher();
private:
    Ui::StartSearchForm* ui;
    const std::shared_ptr<FMData> fmData;

    QStandardItemModel* specificsModel;
    QStandardItemModel* goodDecksModel;
    QItemSelectionModel* goodDecksSelModel;

    std::vector<int> requiredCards();
private slots:
    void deckSelected();
    void on_genDecksBtn_clicked();
};

#endif
