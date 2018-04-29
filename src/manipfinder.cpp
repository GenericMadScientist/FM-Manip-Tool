#include <stdexcept>

#include <QGridLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSpinBox>

#include "ui_manipfinderform.h"

#include "decks.hpp"
#include "manipfinder.hpp"
#include "seedfuns.hpp"
#include "seedindexvalidator.hpp"

ManipFinder::ManipFinder(std::shared_ptr<FMData> fmDataPtr, QWidget* parent)
    : QWidget(parent), ui{new Ui::ManipFinderForm}, fmData{fmDataPtr}
{
    if (fmDataPtr == nullptr) {
        delete ui;
        throw std::runtime_error("Null fmDataPtr");
    }

    ui->setupUi(this);

    ui->duelistBox->addItem("", -1);
    for (unsigned int i = 0; i < fmData->getDuelistInfo().size(); ++i) {
        ui->duelistBox->addItem(fmData->getDuelistInfo()[i].name.c_str(),
                                i + 1);
    }

    ui->rankBox->addItems({"", "SA Pow", "BCD", "SA Tec"});
    std::vector<PoolType> ranks {PoolType::saPow, PoolType::bcd,
                                 PoolType::saTec};
    for (int i = 0; i < 3; ++i) {
        ui->rankBox->setItemData(i + 1, QVariant::fromValue(ranks[i]));
    }

    ui->dropBox->addItem("", -1);

    connect(ui->duelistBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(listDrops()));
    connect(ui->rankBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(listDrops()));

    ui->seedBox->setValidator(new SeedIndValidator(ui->seedBox));

    choicesModel = new QStandardItemModel(this);
    ui->choicesBox->setModel(choicesModel);
    choicesSelModel = new QItemSelectionModel(choicesModel);
    ui->choicesBox->setSelectionModel(choicesSelModel);

    ui->convertLeftBox->setValidator(new SeedIndValidator(ui->convertLeftBox));

    auto validSeed = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{,8}"),
                                          ui->convertRightBox);
    ui->convertRightBox->setValidator(validSeed);

    ui->optsChanger->setModel(choicesModel);
    ui->optsChanger->setSelectionModel(choicesSelModel);
}

ManipFinder::~ManipFinder()
{
    delete ui;
}

void ManipFinder::addChoices(AnimGroup groupType)
{
    static const std::map<AnimGroup, QString> groupNames {
        {AnimGroup::dump, "Dumps"}, {AnimGroup::fusion, "Fusions"},
        {AnimGroup::equip, "Equips"}, {AnimGroup::field, "Field spell"},
        {AnimGroup::battle, "Battle"}, {AnimGroup::direct, "Direct attack"},
        {AnimGroup::trap, "Trap"}
    };

    QStandardItem* newItem = new QStandardItem(groupNames.at(groupType));
    newItem->setFlags({Qt::ItemIsSelectable, Qt::ItemIsEnabled});
    newItem->setData(QVariant::fromValue(AnimOptions(groupType)));
    choicesModel->appendRow(newItem);
}

void ManipFinder::on_dumpsBtn_clicked()
{
    addChoices(AnimGroup::dump);
}

void ManipFinder::on_fusionBtn_clicked()
{
    addChoices(AnimGroup::fusion);
}

void ManipFinder::on_equipsBtn_clicked()
{
    addChoices(AnimGroup::equip);
}

void ManipFinder::on_fieldsBtn_clicked()
{
    addChoices(AnimGroup::field);
}

void ManipFinder::on_battleBtn_clicked()
{
    addChoices(AnimGroup::battle);
}

void ManipFinder::on_directBtn_clicked()
{
    addChoices(AnimGroup::direct);
}

void ManipFinder::on_trapBtn_clicked()
{
    addChoices(AnimGroup::trap);
}

void ManipFinder::deleteSelectedItem()
{
    QModelIndexList indexes {choicesSelModel->selectedRows()};
    while (!indexes.isEmpty()) {
        int rowNumb = indexes.last().row();
        indexes.removeLast();

        delete choicesModel->takeItem(rowNumb);
        choicesModel->removeRows(rowNumb, 1);
    }
}

void ManipFinder::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        deleteSelectedItem();
    }
}

void ManipFinder::listDrops()
{
    ui->dropBox->clear();
    ui->dropBox->addItem("", -1);

    int duelistId = ui->duelistBox->currentData().toInt();
    QVariant rank = ui->rankBox->currentData();

    if ((duelistId == -1) || (rank == QVariant())) {
        return;
    }

    const Duelist& duelist {fmData->getDuelistInfo()[duelistId - 1]};
    const Pool& rankPool {duelist.pools.at(rank.value<PoolType>())};
    std::vector<int> drops {rankPool.possibleDrops()};

    for (int i : drops) {
        QString cardName(fmData->getCardInfo().at(i - 1).name.c_str());
        ui->dropBox->addItem(cardName, i);
    }

    ui->dropBox->model()->sort(0);
}

std::vector<std::vector<Choice>> ManipFinder::getChoicesTree()
{
    std::vector<std::vector<Choice>> tree;

    for (int i = 0; i < choicesModel->rowCount(); ++i) {
        QStandardItem* optItem = choicesModel->item(i);
        AnimOptions opts {optItem->data().value<AnimOptions>()};
        tree.push_back(opts.selectedChoices());
    }

    return tree;
}

uint32_t ManipFinder::getStartSeed()
{
    long long seedIndex = ui->seedBox->text().toLongLong();
    uint32_t seed = indexToSeed(seedIndex);

    if (ui->startCheck->isChecked()) {
        int duelistId = ui->duelistBox->currentData().toInt();
        if (duelistId != dmkDuelistId) {
            const Duelist& duelist {fmData->getDuelistInfo()[duelistId - 1]};
            const Pool& pool {duelist.pools.at(PoolType::deck)};
            seed = aiDeckAdvance(pool, seed);
        }
        else {
            seed = advance(seed, 640);
        }
    }

    return seed;
}

std::vector<uint32_t> ManipFinder::getGoodEnds()
{
    int duelistId = ui->duelistBox->currentData().toInt();
    PoolType rank = ui->rankBox->currentData().value<PoolType>();
    const Pool& rankPool {fmData->getDuelistInfo()[duelistId - 1].pools.at(rank)};
    int chosenDrop = ui->dropBox->currentData().toInt();

    return rankPool.matchingDropSlots(chosenDrop);
}

void ManipFinder::drawSearchResult(const SearchResult& searchResult)
{
    ui->manipBox->clear();
    QString finalText;

    if (searchResult.foundPath) {
        for (Choice c : searchResult.optimalPath) {
            finalText += choiceToText(c).c_str();
            finalText += "\n";
        }
        finalText += "\nDuel ends on seed ";
        finalText += QString::number(searchResult.endingSeedIndex);
    }
    else {
        finalText = "No manip found";
    }

    if (!searchResult.noMissedPaths) {
        finalText += "\n\nNot all paths checked";
    }

    ui->manipBox->setText(finalText);
}

void ManipFinder::on_manipBtn_clicked()
{
    int duelistId = ui->duelistBox->currentData().toInt();
    QVariant rank = ui->rankBox->currentData();
    int cardId = ui->dropBox->currentData().toInt();
    QString seedStr {ui->seedBox->text()};

    if ((duelistId == -1) || (rank == QVariant())
             || (cardId == -1) || (seedStr == "")) {
        return;
    }

    std::vector<std::vector<Choice>> tree {getChoicesTree()};
    uint32_t seed = getStartSeed();
    std::vector<uint32_t> goodEnds {getGoodEnds()};

    PathFinder pathFinder(fmData);
    SearchResult result {pathFinder.findOptimalPath(seed, tree, goodEnds)};
    drawSearchResult(result);
}

void ManipFinder::on_convertLeftBox_textEdited(const QString& seedIndStr)
{
    QString newSeedStr;

    if (!seedIndStr.isEmpty()) {
        ulong seedInd = seedIndStr.toULong();
        uint32_t seed = indexToSeed(seedInd);
        newSeedStr = QString::number(seed, 16).toUpper();
    }

    ui->convertRightBox->setText(newSeedStr);
}

void ManipFinder::on_convertRightBox_textEdited(const QString& seedStr)
{
    ui->convertRightBox->setText(ui->convertRightBox->text().toUpper());

    QString newSeedIndStr;

    if (!seedStr.isEmpty()) {
        bool ok = false;
        ulong seed = seedStr.toULong(&ok, 16);
        if (ok) {
            ulong seedInd = seedToIndex(static_cast<uint32_t>(seed));
            newSeedIndStr = QString::number(seedInd);
        }
    }

    ui->convertLeftBox->setText(newSeedIndStr);
}
