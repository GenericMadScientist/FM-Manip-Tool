#include <map>

#include "manipanims.hpp"

std::vector<Anim> animVals()
{
    std::vector<Anim> vals;
    for (int i = 1; i <= 18; ++i) {
        vals.push_back(static_cast<Anim>(i));
    }
    return vals;
}

std::vector<Choice> choiceRange(int first, int last)
{
    std::vector<Choice> vals;
    for (int i = first; i <= last; ++i) {
        vals.push_back(static_cast<Choice>(i));
    }
    return vals;
}

std::vector<Choice> choiceVals()
{
    return choiceRange(1, 35);
}

std::vector<Choice> dumps()
{
    return choiceRange(1, 5);
}

std::vector<Choice> fuses()
{
    return choiceRange(6, 10);
}

std::vector<Choice> equips()
{
    return choiceRange(11, 15);
}

std::vector<Choice> fields()
{
    return choiceRange(16, 21);
}

std::vector<Choice> directs()
{
    return choiceRange(22, 23);
}

std::vector<Choice> atks()
{
    return choiceRange(24, 26);
}

std::vector<Choice> gsAtks()
{
    return choiceRange(27, 29);
}

std::vector<Choice> atks3d()
{
    return choiceRange(30, 31);
}

std::vector<Choice> gsAtks3d()
{
    return choiceRange(32, 33);
}

std::string choiceToText(Choice choice)
{
    const std::map<Choice, std::string> choiceStrs {
        {Choice::dump0, "Dump 0 cards"},
        {Choice::dump1, "Dump 1 card"},
        {Choice::dump2, "Dump 2 cards"},
        {Choice::dump3, "Dump 3 cards"},
        {Choice::dump4, "Dump 4 cards"},
        {Choice::fuse0, "Do 0 fusions"},
        {Choice::fuse1, "Do 1 fusion"},
        {Choice::fuse2, "Do 2 fusions"},
        {Choice::fuse3, "Do 3 fusions"},
        {Choice::fuse4, "Do 4 fusions"},
        {Choice::equip0, "Do 0 equips"},
        {Choice::equip1, "Do 1 equip"},
        {Choice::equip2, "Do 2 equips"},
        {Choice::equip3, "Do 3 equips"},
        {Choice::equip4, "Do 4 equips"},
        {Choice::fieldForest, "Activate Forest"},
        {Choice::fieldMount, "Activate Mountain"},
        {Choice::fieldSogen, "Activate Sogen"},
        {Choice::fieldUmi, "Activate Umi"},
        {Choice::fieldWaste, "Activate Wasteland"},
        {Choice::fieldYami, "Activate Yami"},
        {Choice::atkDirectHigh, "Do ≥ 1k direct damage"},
        {Choice::atkDirectLow, "Do < 1k direct damage"},
        {Choice::atkNorm, "Kill a def. position monster\n"
                          "  (no 3d battle, no GS anim.)"},
        {Choice::atkDamHigh, "Kill an enemy monster doing ≥ 1k damage\n"
                             "  (no 3d battle, no GS anim.)"},
        {Choice::atkDamLow, "Kill an enemy monster doing < 1k damage\n"
                            "  (no 3d battle, no GS anim.)"},
        {Choice::atkGsNorm, "Kill a def. position monster\n"
                            "  (no 3d battle, with GS anim.)"},
        {Choice::atkGsDamHigh, "Kill an enemy monster doing ≥ 1k damage\n"
                               "  (no 3d battle, with GS anim.)"},
        {Choice::atkGsDamLow, "Kill an enemy monster doing < 1k damage\n"
                              "  (no 3d battle, with GS anim.)"},
        {Choice::atkQuick3d, "Kill an enemy monster\n"
                             "  (quick 3d battle, no GS anim.)"},
        {Choice::atkSpawn3d, "Kill an enemy monster\n"
                             "  (spawn 3d battle, no GS anim.)"},
        {Choice::atkGsQuick3d, "Kill an enemy monster\n"
                               "  (quick 3d battle, with GS anim.)"},
        {Choice::atkGsSpawn3d, "Kill an enemy monster\n"
                               "  (spawn 3d battle, with GS anim.)"},
        {Choice::trap, "Activate trap"},
        {Choice::emptyChoice, "Do nothing"}
    };

    return choiceStrs.at(choice);
}
