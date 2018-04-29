#include <stdexcept>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

#include "optschanger.hpp"

OptsChanger::OptsChanger(QWidget* parent)
    : QScrollArea(parent)
{
    setLayout(new QGridLayout);
}

void OptsChanger::setModel(QStandardItemModel* newModel)
{
    model = newModel;
    clearWidgets();
    connect(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(drawSelectedItem()));
}

void OptsChanger::setSelectionModel(QItemSelectionModel* newSelModel)
{
    selModel = newSelModel;
    clearWidgets();
    connect(selModel, SIGNAL(currentChanged(const QModelIndex&,
                                            const QModelIndex&)),
            this, SLOT(drawSelectedItem()));
}

AnimOptions OptsChanger::getOpts()
{
    if (animGroup.canConvert<AnimGroup>()) {
        AnimGroup group = animGroup.value<AnimGroup>();
        std::vector<ChoiceFlags> flags {relevantFlags(group)};
        AnimOptions opts(group);

        for (unsigned int i = 0; i < flags.size(); ++i) {
            QWidget* nextWidget = widgets[2 * i + 1];
            if (auto widget = dynamic_cast<QSpinBox*>(nextWidget)) {
                opts.choices.at(flags[i]) = widget->value();
            }
            else if (auto widget = dynamic_cast<QCheckBox*>(nextWidget)) {
                opts.choices.at(flags[i]) = widget->checkState() == Qt::Checked;
            }
            else if (auto widget = dynamic_cast<QComboBox*>(nextWidget)) {
                opts.choices.at(flags[i]) = widget->currentIndex();
            }
            else {
                throw std::runtime_error("Can't cast in getOpts");
            }
        }

        return opts;
    }
    else {
        throw std::runtime_error("Tried getOpts with no options");
    }
}

void OptsChanger::connectWidgets()
{
    for (QWidget* nextWidget : widgets) {
        if (auto widget = dynamic_cast<QSpinBox*>(nextWidget)) {
            connect(widget, SIGNAL(valueChanged(int)),
                    this, SLOT(writeChanges()));
        }
        else if (auto widget = dynamic_cast<QCheckBox*>(nextWidget)) {
            connect(widget, SIGNAL(stateChanged(int)),
                    this, SLOT(writeChanges()));
        }
        else if (auto widget = dynamic_cast<QComboBox*>(nextWidget)) {
            connect(widget, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(writeChanges()));
        }
    }
}

void OptsChanger::clearWidgets()
{
    animGroup = QVariant();
    for (auto q : widgets) {
        delete q;
    }
    widgets.clear();
}

void OptsChanger::initAndDisplayWidgets()
{
    QGridLayout* gridLayout = dynamic_cast<QGridLayout*>(layout());
    // layout() will always be a QGridLayout, so gridLayout != nullptr
    for (unsigned int i = 0; i < widgets.size(); i += 2) {
        gridLayout->addWidget(widgets[i], i, 0);
        gridLayout->addWidget(widgets[i + 1], i, 1);
    }

    QStandardItem* item = model->item(selModel->currentIndex().row());
    const AnimOptions& opts = item->data().value<AnimOptions>();
    std::vector<ChoiceFlags> flags {relevantFlags(opts.choiceType)};

    for (unsigned int i = 0; i < flags.size(); ++i) {
        QWidget* nextWidget = widgets[2 * i + 1];
        if (auto widget = dynamic_cast<QSpinBox*>(nextWidget)) {
            widget->setValue(opts.choices.at(flags[i]));
        }
        else if (auto widget = dynamic_cast<QCheckBox*>(nextWidget)) {
            if (opts.choices.at(flags[i])) {
                widget->setCheckState(Qt::Checked);
            }
            else {
                widget->setCheckState(Qt::Unchecked);
            }
        }
        else if (auto widget = dynamic_cast<QComboBox*>(nextWidget)) {
            widget->setCurrentIndex(opts.choices.at(flags[i]));
        }
    }
}

void OptsChanger::drawSelectedItem()
{
    clearWidgets();

    if (model == nullptr || selModel == nullptr) {
        return;
    }
    QModelIndex index = selModel->currentIndex();
    if (!index.isValid()) {
        return;
    }

    AnimOptions opts = model->item(index.row())->data().value<AnimOptions>();
    animGroup = QVariant::fromValue(opts.choiceType);

    switch (opts.choiceType) {
    case AnimGroup::dump:
    case AnimGroup::equip:
    case AnimGroup::fusion:
        drawRange();
        break;
    case AnimGroup::field:
        drawFields();
        break;
    case AnimGroup::battle:
        drawBattle();
        break;
    case AnimGroup::direct:
        drawDirect();
        break;
    case AnimGroup::trap:
        drawTrap();
        break;
    }
    connectWidgets();
}

void OptsChanger::drawRange()
{
    QLabel* minLbl = new QLabel("Min number", this);
    QLabel* maxLbl = new QLabel("Max number", this);
    QSpinBox* minBox = new QSpinBox(this);
    QSpinBox* maxBox = new QSpinBox(this);

    widgets = {minLbl, minBox, maxLbl, maxBox};
    initAndDisplayWidgets();

    lowerBox = minBox;
    upperBox = maxBox;
    for (auto box : {minBox, maxBox}) {
        box->setMinimum(0);
        box->setMaximum(4);
        connect(box, SIGNAL(valueChanged(int)),
                this, SLOT(setSpinBoxBounds()));
    }
}

void OptsChanger::drawFields()
{
    const std::vector<QString> lblNames {
        "Require spell?", "Allow Forest?", "Allow Mountain?", "Allow Sogen?",
        "Allow Umi?", "Allow Wasteland?", "Allow Yami?"
    };

    for (int i = 0; i < 7; ++i) {
        QLabel* newLbl = new QLabel(lblNames[i], this);
        QCheckBox* newCheck = new QCheckBox(this);
        widgets.push_back(newLbl);
        widgets.push_back(newCheck);
    }

    initAndDisplayWidgets();
}

void OptsChanger::drawBattle()
{
    QLabel* gsLbl = new QLabel("GS animation?", this);
    QLabel* dmgLbl = new QLabel("Damage dealt", this);
    QLabel* aiLbl = new QLabel("AI battle?", this);
    QComboBox* gsCombo = new QComboBox(this);
    QComboBox* dmgCombo = new QComboBox(this);
    QCheckBox* aiBox = new QCheckBox(this);
    gsCombo->addItems({"No", "Yes", "Either"});
    dmgCombo->addItems({"No damage", "< 1k damage", "≥ 1k damage"});

    widgets = {gsLbl, gsCombo, dmgLbl, dmgCombo, aiLbl, aiBox};
    initAndDisplayWidgets();
}

void OptsChanger::drawDirect()
{
    const std::vector<QString> lblNames {
        "Need to attack?", "Can do < 1k damage?", "Can do ≥ 1k damage?"
    };

    for (int i = 0; i < 3; ++i) {
        QLabel* newLbl = new QLabel(lblNames[i], this);
        QCheckBox* newCheck = new QCheckBox(this);
        widgets.push_back(newLbl);
        widgets.push_back(newCheck);
    }

    initAndDisplayWidgets();
}

void OptsChanger::drawTrap()
{
    QLabel* trapLbl = new QLabel("Trap needed?", this);
    QCheckBox* trapCheck = new QCheckBox(this);

    widgets = {trapLbl, trapCheck};
    initAndDisplayWidgets();
}

void OptsChanger::writeChanges()
{
    QStandardItem* item = model->item(selModel->currentIndex().row());
    item->setData(QVariant::fromValue(getOpts()));
}

void OptsChanger::setSpinBoxBounds()
{
    lowerBox->setMaximum(upperBox->value());
    upperBox->setMinimum(lowerBox->value());
}
