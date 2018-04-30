#ifndef MANIPANIMS_HPP
#define MANIPANIMS_HPP

#include <string>
#include <vector>

enum class Anim {
    burn = 1,
    burn3d,
    directHigh,
    directLow,
    dump,
    emptyAnim,
    endOfDuel,
    equip,
    fieldMount,
    fieldUsual,
    fieldYami,
    fuse,
    gsAnim,
    spawn,
    swipeAtkHigh,
    swipeAtkLow,
    swipeDef,
    trapTriggered
};

enum class Choice {
    dump0 = 1, dump1, dump2, dump3, dump4,
    fuse0, fuse1, fuse2, fuse3, fuse4,
    equip0, equip1, equip2, equip3, equip4,
    fieldForest, fieldMount, fieldSogen,
    fieldUmi, fieldWaste, fieldYami,
    atkDirectHigh, atkDirectLow,
    atkNorm, atkDamHigh, atkDamLow,
    atkGsNorm, atkGsDamHigh, atkGsDamLow,
    atkQuick3d, atkSpawn3d, atkGsQuick3d, atkGsSpawn3d,
    trap, emptyChoice
};

std::vector<Anim> animVals();
std::vector<Choice> choiceVals();

std::vector<Choice> dumps();
std::vector<Choice> fuses();
std::vector<Choice> equips();
std::vector<Choice> fields();
std::vector<Choice> directs();
std::vector<Choice> atks();
std::vector<Choice> gsAtks();
std::vector<Choice> atks3d();
std::vector<Choice> gsAtks3d();

std::string choiceToText(Choice);

#endif
