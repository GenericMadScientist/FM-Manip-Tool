#include <stdexcept>

#include <QIntValidator>
#include <QStandardItem>

#include "ui_startsearchform.h"

#include "cards.hpp"
#include "constants.hpp"
#include "seedfuns.hpp"
#include "seedindexvalidator.hpp"
#include "starterdecks.hpp"
#include "startsearcher.hpp"

StartSearcher::StartSearcher(std::shared_ptr<FMData> fmDataPtr,
                             QWidget* parent)
    : QWidget(parent), ui{new Ui::StartSearchForm}, fmData{fmDataPtr}
{
    if (fmDataPtr == nullptr) {
        delete ui;
        throw std::runtime_error("Null fmDataPtr");
    }

    ui->setupUi(this);

    ui->startBox->setValidator(new SeedIndValidator(ui->startBox));
    ui->numbSeedsBox->setValidator(new QIntValidator(0, 50000, ui->numbSeedsBox));
    ui->dragBox->setValidator(new QIntValidator(0, deckSize, ui->dragBox));
    ui->thundBox->setValidator(new QIntValidator(0, deckSize, ui->thundBox));

    ui->destMagicBox->addItem("", -1);
    for (int i : fmData->getMagics()) {
        ui->destMagicBox->addItem(fmData->getCardInfo().at(i - 1).name.c_str(), i);
    }

    ui->equipBox->addItem("", -1);
    for (int i : fmData->getEquips()) {
        ui->equipBox->addItem(fmData->getCardInfo().at(i - 1).name.c_str(), i);
    }

    ui->fieldBox->addItem("", -1);
    for (int i : fmData->getFields()) {
        ui->fieldBox->addItem(fmData->getCardInfo().at(i - 1).name.c_str(), i);
    }

    specificsModel = new QStandardItemModel(this);
    ui->specificsBox->setModel(specificsModel);

    for (int i : fmData->getStartMonsters()) {
        QString cardName(fmData->getCardInfo().at(i - 1).name.c_str());
        QStandardItem* newItem = new QStandardItem(cardName);
        newItem->setData(i);
        newItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        newItem->setCheckState(Qt::Unchecked);
        specificsModel->appendRow(newItem);
    }

    goodDecksModel = new QStandardItemModel(this);
    ui->goodDecksBox->setModel(goodDecksModel);
    goodDecksSelModel = new QItemSelectionModel(goodDecksModel);
    ui->goodDecksBox->setSelectionModel(goodDecksSelModel);
    connect(goodDecksSelModel, SIGNAL(currentChanged(const QModelIndex&,
                                                     const QModelIndex&)),
            this, SLOT(deckSelected()));
}

StartSearcher::~StartSearcher()
{
    delete ui;
}

std::vector<int> StartSearcher::requiredCards()
{
    std::vector<int> neededCards;

    for (QComboBox* q : {ui->destMagicBox, ui->equipBox, ui->fieldBox}) {
        if (q->currentText() != "") {
            neededCards.push_back(q->currentData().toInt());
        }
    }

    for (int i = 0; i < specificsModel->rowCount(); ++i) {
        if (specificsModel->item(i)->checkState() == Qt::Checked) {
            neededCards.push_back(specificsModel->item(i)->data().toInt());
        }
    }

    return neededCards;
}

void StartSearcher::deckSelected()
{
    QModelIndex index = goodDecksSelModel->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int deckNumber = goodDecksModel->item(index.row())->data().toInt();
    CardsList deckCards(starterDeck(indexToSeed(deckNumber), *fmData).deck());
    sortCards(deckCards, Sort::abc, fmData->getCardInfo());

    QString deckString;
    for (int i : deckCards.cardList()) {
        deckString += fmData->getCardInfo().at(i - 1).name.c_str();
        deckString += "\n";
    }
    deckString.chop(1);

    ui->deckBox->clear();
    ui->deckBox->setText(deckString);
}

void StartSearcher::on_genDecksBtn_clicked()
{
    std::vector<int> searchArgs;

    if (ui->startBox->text().isEmpty()) {
        return;
    }
    else {
        searchArgs.push_back(ui->startBox->text().toLongLong());
    }

    for (QLineEdit* q : {ui->numbSeedsBox, ui->dragBox, ui->thundBox}) {
        if (q->text().isEmpty()) {
            return;
        }
        searchArgs.push_back(q->text().toInt());
    }

    ui->deckBox->clear();

    std::vector<int> validDecks {goodDecks(searchArgs.at(0), searchArgs.at(1),
                                           searchArgs.at(2), searchArgs.at(3),
                                           requiredCards(), *fmData)};
    goodDecksModel->clear();
    for (int i : validDecks) {
        QStandardItem* newItem = new QStandardItem(QString::number(i));
        newItem->setData(i);
        newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
        goodDecksModel->appendRow(newItem);
    }
    if (validDecks.size() == 0) {
        ui->deckBox->setText("No decks found");
    }
}
