#include <stdexcept>

#include <QIntValidator>
#include <QString>

#include "ui_easymodefinder.h"

#include "cards.hpp"
#include "constants.hpp"
#include "decks.hpp"
#include "duelists.hpp"
#include "easymodefinder.hpp"
#include "seedfuns.hpp"
#include "seedindexvalidator.hpp"

EasyModeFinder::EasyModeFinder(std::shared_ptr<FMData> fmDataPtr,
                               QWidget* parent)
    : QWidget(parent), ui{new Ui::EasyModeForm}, fmData{fmDataPtr}
{
    if (fmDataPtr == nullptr) {
        delete ui;
        throw std::runtime_error("Null fmDataPtr");
    }

    ui->setupUi(this);

    ui->opponents->addItem("", -1);

    for (unsigned int i = 0; i < fmData->getDuelistInfo().size() - 1; ++i) {
        ui->opponents->addItem(fmData->getDuelistInfo()[i].name.c_str(),
                               i + 1);
    }

    ui->startSeedBox->setValidator(new SeedIndValidator(ui->startSeedBox));
    ui->numbSeedBox->setValidator(new QIntValidator(0, 10000, ui->numbSeedBox));
    ui->easyTurnsBox->setValidator(new QIntValidator(0, 40, ui->easyTurnsBox));

    allowedCardsModel = new QStandardItemModel(this);
    ui->allowedCards->setModel(allowedCardsModel);

    easyModesModel = new QStandardItemModel(this);
    ui->easyModesBox->setModel(easyModesModel);

    easyModesSelModel = new QItemSelectionModel(easyModesModel);
    ui->easyModesBox->setSelectionModel(easyModesSelModel);

    connect(ui->opponents, SIGNAL(currentIndexChanged(int)),
            this, SLOT(opponentChanged(int)));
    connect(easyModesSelModel,
            SIGNAL(selectionChanged(const QItemSelection&,
                                    const QItemSelection&)),
            this, SLOT(drawPlays()));
}

EasyModeFinder::~EasyModeFinder()
{
    delete ui;
}

void EasyModeFinder::opponentChanged(int selectedRow)
{
    int duelistId = ui->opponents->itemData(selectedRow).toInt();
    drawBoxes(duelistId);
    easyModesModel->clear();
    ui->playsBox->clear();
}

std::vector<int> EasyModeFinder::forbiddenCards()
{
    std::vector<int> bannedCards;

    for (int i = 0; i < allowedCardsModel->rowCount(); ++i) {
        if (allowedCardsModel->item(i)->checkState() == Qt::Unchecked) {
            bannedCards.push_back(allowedCardsModel->item(i)->data().toInt());
        }
    }

    return bannedCards;
}

void EasyModeFinder::drawBoxes(int duelistId)
{
    if (duelistId == -1) {
        allowedCardsModel->clear();
        return;
    }

    Duelist selectedDuelist {fmData->getDuelistInfo()[duelistId - 1]};

    const Pool& selectedPool {selectedDuelist.pools.at(PoolType::deck)};
    CardsList drops(selectedPool.possibleDrops());
    sortCards(drops, Sort::max, fmData->getCardInfo());

    allowedCardsModel->clear();

    for (int i : drops.cardList()) {
        QString cardName(fmData->getCardInfo()[i - 1].name.c_str());
        QStandardItem* nextCard = new QStandardItem(cardName);
        nextCard->setData(i);
        nextCard->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        nextCard->setCheckState(Qt::Checked);
        allowedCardsModel->appendRow(nextCard);
    }
}

void EasyModeFinder::drawPlays()
{
    ui->playsBox->clear();

    // Happens if drawPlays() is triggered by the seed box being cleared.
    if (!(easyModesSelModel->hasSelection())) {
        return;
    }
    if (ui->opponents->currentData() == -1) {
        return;
    }

    int row = easyModesSelModel->currentIndex().row();
    int seedIndex = easyModesModel->item(row)->text().toInt();
    int duelistId = ui->opponents->currentData().toInt();

    const Duelist& selectedDuelist {fmData->getDuelistInfo()[duelistId - 1]};
    const Pool& deckPool {selectedDuelist.pools.at(PoolType::deck)};
    int handSize = selectedDuelist.handSize;

    Deck enemyDeck {aiDeck(deckPool, indexToSeed(seedIndex))};
    std::vector<int> enemyPlays {aiSort(enemyDeck, handSize,
                                        fmData->getCardInfo()).deck()};

    QString deckString;
    for (int i : enemyPlays) {
        deckString += fmData->getCardInfo().at(i - 1).name.c_str();
        deckString += "\n";
    }
    deckString.chop(1);

    ui->playsBox->setText(deckString);
}

void EasyModeFinder::on_findEasyBtn_clicked()
{
    if (ui->opponents->currentData() == -1) {
        return;
    }

    int duelistId = ui->opponents->currentData().toInt();
    std::vector<int> bannedCards {forbiddenCards()};

    if (ui->easyTurnsBox->text().isEmpty()) {
        return;
    }
    int numbOfTurns = ui->easyTurnsBox->text().toInt();

    if (ui->startSeedBox->text().isEmpty()) {
        return;
    }
    long long startSeedInd = ui->startSeedBox->text().toLongLong();

    if (ui->numbSeedBox->text().isEmpty()) {
        return;
    }
    int numbOfDuels = ui->numbSeedBox->text().toInt();

    const Duelist& selectedDuelist {fmData->getDuelistInfo()[duelistId - 1]};
    std::vector<long long> easySeeds {easyModes(selectedDuelist, bannedCards,
                                                numbOfTurns, startSeedInd,
                                                numbOfDuels,
                                                fmData->getCardInfo())};

    ui->playsBox->clear();
    easyModesModel->clear();
    if (easySeeds.size() == 0) {
        ui->playsBox->setText("No decks found");
    }
    else {
        for (long long s : easySeeds) {
            QStandardItem* newItem = new QStandardItem(QString::number(s));
            newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
            easyModesModel->appendRow(newItem);
        }
    }
}
