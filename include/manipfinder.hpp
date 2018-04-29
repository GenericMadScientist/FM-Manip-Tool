#ifndef MANIPFINDER_HPP
#define MANIPFINDER_HPP

#include <memory>
#include <vector>

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QWidget>

#include "customlistitems.hpp"
#include "fmdata.hpp"
#include "manipanims.hpp"
#include "pathfinder.hpp"

namespace Ui {
    class ManipFinderForm;
}

class ManipFinder : public QWidget {
    Q_OBJECT
public:
    ManipFinder(std::shared_ptr<FMData>, QWidget* parent = nullptr);
    ~ManipFinder();
protected:
    void keyPressEvent(QKeyEvent*);
private:
    Ui::ManipFinderForm* ui;
    const std::shared_ptr<FMData> fmData;

    QStandardItemModel* choicesModel;
    QItemSelectionModel* choicesSelModel;
    QStandardItemModel* choicesProxyModel;

    void addChoices(AnimGroup);

    void deleteSelectedItem();

    std::vector<std::vector<Choice>> getChoicesTree();
    uint32_t getStartSeed();
    std::vector<uint32_t> getGoodEnds();
    void drawSearchResult(const SearchResult&);
private slots:
    void on_dumpsBtn_clicked();
    void on_fusionBtn_clicked();
    void on_equipsBtn_clicked();
    void on_fieldsBtn_clicked();
    void on_battleBtn_clicked();
    void on_directBtn_clicked();
    void on_trapBtn_clicked();
    void listDrops();

    void on_manipBtn_clicked();

    void on_convertLeftBox_textEdited(const QString&);
    void on_convertRightBox_textEdited(const QString&);
};

#endif
