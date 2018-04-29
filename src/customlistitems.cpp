#include <QString>

#include "customlistitems.hpp"

// dump, fusion, equip keep the endpoints of their ranges
// field keeps empty, then fields in alphabetical order
// battle keeps 0/1/2 for first, for no GS/GS/either
//    and 0/1/2 for second, for no damage/>=1k damage/<1k damage
// direct keeps three flags, for required?/high?/low?

AnimOptions::AnimOptions(AnimGroup groupType)
    : choiceType{groupType}
{
    const std::map<AnimGroup, std::map<ChoiceFlags, int>> defaults {
        {AnimGroup::dump, {{ChoiceFlags::rangeLower, 0},
                           {ChoiceFlags::rangeUpper, 4}}},
        {AnimGroup::fusion, {{ChoiceFlags::rangeLower, 0},
                             {ChoiceFlags::rangeUpper, 4}}},
        {AnimGroup::equip, {{ChoiceFlags::rangeLower, 0},
                            {ChoiceFlags::rangeUpper, 4}}},
        {AnimGroup::field, {{ChoiceFlags::needField, 1},
                            {ChoiceFlags::forestFlag, 1},
                            {ChoiceFlags::mountFlag, 1},
                            {ChoiceFlags::sogenFlag, 1},
                            {ChoiceFlags::umiFlag, 1},
                            {ChoiceFlags::wasteFlag, 1},
                            {ChoiceFlags::yamiFlag, 1}}},
        {AnimGroup::battle, {{ChoiceFlags::gsFlag, 0},
                             {ChoiceFlags::damageType, 0},
                             {ChoiceFlags::aiFlag, 0}}},
        {AnimGroup::direct, {{ChoiceFlags::directNeeded, 1},
                             {ChoiceFlags::directLowFlag, 0},
                             {ChoiceFlags::directHighFlag, 1}}},
        {AnimGroup::trap, {{ChoiceFlags::trapNeeded, 1}}}
    };

    choices = defaults.at(groupType);
}

std::vector<Choice> AnimOptions::selectedChoices() const
{
    std::vector<Choice> results;
    std::vector<Choice> tempChoices;

    switch (choiceType) {
    case AnimGroup::dump:
        tempChoices = dumps();
        for (int i = choices.at(ChoiceFlags::rangeLower);
             i <= choices.at(ChoiceFlags::rangeUpper); ++i) {
            results.push_back(tempChoices.at(i));
        }
        break;
    case AnimGroup::fusion:
        tempChoices = fuses();
        for (int i = choices.at(ChoiceFlags::rangeLower);
             i <= choices.at(ChoiceFlags::rangeUpper); ++i) {
            results.push_back(tempChoices.at(i));
        }
        break;
    case AnimGroup::equip:
        tempChoices = equips();
        for (int i = choices.at(ChoiceFlags::rangeLower);
             i <= choices.at(ChoiceFlags::rangeUpper); ++i) {
            results.push_back(tempChoices.at(i));
        }
        break;
    case AnimGroup::field: {
        tempChoices = fields();
        constexpr std::array<ChoiceFlags, 6> flags {
            ChoiceFlags::forestFlag, ChoiceFlags::mountFlag,
            ChoiceFlags::sogenFlag, ChoiceFlags::umiFlag,
            ChoiceFlags::wasteFlag, ChoiceFlags::yamiFlag
        };

        if (choices.at(ChoiceFlags::needField) == 0) {
            results.push_back(Choice::emptyChoice);
        }
        for (unsigned int i = 0; i < flags.size(); ++i) {
            if (choices.at(flags.at(i)) == 1) {
                results.push_back(tempChoices.at(i));
            }
        }
        break;
    }
    case AnimGroup::battle:
        if (choices.at(ChoiceFlags::gsFlag) != 1) {
            results.push_back(atks().at(choices.at(ChoiceFlags::damageType)));
            if (choices.at(ChoiceFlags::aiFlag) == 0) {
                results.push_back(Choice::atkQuick3d);
                results.push_back(Choice::atkSpawn3d);
            }
        }
        if (choices.at(ChoiceFlags::gsFlag) != 0) {
            results.push_back(gsAtks().at(choices.at(ChoiceFlags::damageType)));
            if (choices.at(ChoiceFlags::aiFlag) == 0) {
                results.push_back(Choice::atkGsQuick3d);
                results.push_back(Choice::atkGsSpawn3d);
            }
        }
        break;
    case AnimGroup::direct:
        if (choices.at(ChoiceFlags::directNeeded) == 0) {
            results.push_back(Choice::emptyChoice);
        }
        if (choices.at(ChoiceFlags::directLowFlag) == 1) {
            results.push_back(Choice::atkDirectLow);
        }
        if (choices.at(ChoiceFlags::directHighFlag) == 1) {
            results.push_back(Choice::atkDirectHigh);
        }
        break;
    case AnimGroup::trap:
        if (choices.at(ChoiceFlags::trapNeeded) == 1) {
            results.push_back(Choice::trap);
        }
        break;
    }

    return results;
}

std::vector<ChoiceFlags> relevantFlags(AnimGroup group)
{
    std::vector<ChoiceFlags> flags;

    switch (group) {
    case AnimGroup::dump:
    case AnimGroup::equip:
    case AnimGroup::fusion:
        flags = {ChoiceFlags::rangeLower, ChoiceFlags::rangeUpper};
        break;
    case AnimGroup::field:
        flags = {ChoiceFlags::needField, ChoiceFlags::forestFlag,
                 ChoiceFlags::mountFlag, ChoiceFlags::sogenFlag,
                 ChoiceFlags::umiFlag, ChoiceFlags::wasteFlag,
                 ChoiceFlags::yamiFlag};
        break;
    case AnimGroup::battle:
        flags = {ChoiceFlags::gsFlag, ChoiceFlags::damageType,
                 ChoiceFlags::aiFlag};
        break;
    case AnimGroup::direct:
        flags = {ChoiceFlags::directNeeded, ChoiceFlags::directLowFlag,
                 ChoiceFlags::directHighFlag};
        break;
    case AnimGroup::trap:
        flags = {ChoiceFlags::trapNeeded};
        break;
    }

    return flags;
}
