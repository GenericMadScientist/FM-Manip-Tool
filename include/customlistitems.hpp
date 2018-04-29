#ifndef CUSTOMLISTITEMS_HPP
#define CUSTOMLISTITEMS_HPP

#include <map>
#include <vector>

#include <QMetaType>

#include "manipanims.hpp"

enum class AnimGroup { dump, fusion, equip, field, battle, direct, trap };
Q_DECLARE_METATYPE(AnimGroup)

// damageType 0/1/2 = no damage/<1k/>=1k
enum class ChoiceFlags {
    rangeLower, rangeUpper, needField,
    forestFlag, mountFlag, sogenFlag,
    umiFlag, wasteFlag, yamiFlag,
    gsFlag, damageType, aiFlag,
    directNeeded, directLowFlag, directHighFlag,
    trapNeeded
};
Q_DECLARE_METATYPE(ChoiceFlags)

struct AnimOptions {
    AnimGroup choiceType;
    std::map<ChoiceFlags, int> choices;
    // Needed for Q_DECLARE_METATYPE
    AnimOptions() {}
    explicit AnimOptions(AnimGroup);
    std::vector<Choice> selectedChoices() const;
};
Q_DECLARE_METATYPE(AnimOptions)

std::vector<ChoiceFlags> relevantFlags(AnimGroup);

#endif
