#include <algorithm>
#include <cstdlib>
#include <cstring>

#include "constants.hpp"
#include "fmdata.hpp"

bool compareStrings(int argc, char** colNames,
                    const std::vector<const char*>& correctNames)
{
    if (static_cast<unsigned int>(argc) != correctNames.size()) {
        return false;
    }
    for (unsigned int i = 0; i < correctNames.size(); ++i) {
        if (std::strcmp(correctNames[i], colNames[i]) != 0) {
            return false;
        }
    }

    return true;
}

int addToCardsVec(void* voidCardsPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "Name", "NumSort", "AbcSort", "MaxSort",
        "AtkSort", "DefSort", "TypSort", "AISort"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto cardsPtr = static_cast<std::vector<Card>*>(voidCardsPtr);
    cardsPtr->push_back(Card(std::string(argv[0]), std::atoi(argv[1]),
                             std::atoi(argv[2]), std::atoi(argv[3]),
                             std::atoi(argv[4]), std::atoi(argv[5]),
                             std::atoi(argv[6]), std::atoi(argv[7])));
    return 0;
}

std::vector<Card> dbCards(sqlite3* fmDb)
{
    std::vector<Card> cards;

    const char* stmtStr = "select Name, NumSort, AbcSort, MaxSort, AtkSort, "
                          "DefSort, TypSort, AISort from Cards order by NumSort";
    int rc = sqlite3_exec(fmDb, stmtStr, addToCardsVec,
                          static_cast<void*>(&cards), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    return cards;
}

struct TempDuelist {
    std::string name;
    int handSize;
    bool isMage;

    TempDuelist(std::string duelistName, int hand, bool mageFlag)
        : name{duelistName}, handSize{hand}, isMage{mageFlag} {}
};

int addToDuelistPools(void* voidVecsPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "DuelistID", "PoolTypeID", "CardID", "Prob"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto poolVecsPtr =
            static_cast<std::vector<std::vector<std::vector<int>>>*>(voidVecsPtr);

    int duelistId = std::atoi(argv[0]);
    int poolTypeId = std::atoi(argv[1]);
    int cardId = std::atoi(argv[2]);
    int cardProb = std::atoi(argv[3]);
    poolVecsPtr->at(duelistId - 1).at(poolTypeId - 1).at(cardId - 1) = cardProb;

    return 0;
}

int addToTempDuelists(void* voidTempsPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "DuelistID", "Name", "HandSize", "IsMage"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto tempsPtr = static_cast<std::map<int, TempDuelist>*>(voidTempsPtr);

    int duelistId = std::atoi(argv[0]);
    std::string name {argv[1]};
    int handSize = std::atoi(argv[2]);
    bool isMage = argv[3][0] == '1';
    tempsPtr->insert({duelistId, TempDuelist(name, handSize, isMage)});

    return 0;
}

std::vector<Duelist> dbDuelists(sqlite3* fmDb)
{
    std::vector<std::vector<std::vector<int>>> duelPoolsVec;

    for (int i = 0; i < numOfDuelists; ++i) {
        std::vector<std::vector<int>> newPools;
        for (int j = 0; j < numbOfPoolTypes; ++j) {
            std::vector<int> newPool(maxCardId);
            newPools.push_back(newPool);
        }
        duelPoolsVec.push_back(newPools);
    }

    const char* stmtStr = "select * from DuelistPools";
    int rc = sqlite3_exec(fmDb, stmtStr, addToDuelistPools,
                          static_cast<void*>(&duelPoolsVec), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    std::map<int, TempDuelist> tempDuelists;
    const char* stmtStrTemps = "select * from Duelists";
    rc = sqlite3_exec(fmDb, stmtStrTemps, addToTempDuelists,
                      static_cast<void*>(&tempDuelists), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    std::vector<Duelist> duelistsVec;
    try {
        for (int i = 0; i < numOfDuelists; ++i) {
            std::string name {tempDuelists.at(i + 1).name};
            int handSize = tempDuelists.at(i + 1).handSize;
            bool isMage = tempDuelists.at(i + 1).isMage;
            Duelist newDuelist(name, handSize, isMage,
                               Pool(duelPoolsVec.at(i).at(0)),
                               Pool(duelPoolsVec.at(i).at(1)),
                               Pool(duelPoolsVec.at(i).at(2)),
                               Pool(duelPoolsVec.at(i).at(3)));
            duelistsVec.push_back(newDuelist);
        }
    }
    catch (const std::out_of_range&) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    return duelistsVec;
}

int addToPoolVecs(void* voidVecsPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "PoolID", "CardID", "Prob"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto poolVecsPtr = static_cast<std::vector<std::vector<int>>*>(voidVecsPtr);

    int poolId = std::atoi(argv[0]);
    int cardId = std::atoi(argv[1]);
    int prob = std::atoi(argv[2]);
    poolVecsPtr->at(poolId - 1).at(cardId - 1) = prob;

    return 0;
}

std::vector<Pool> dbStarterPools(sqlite3* fmDb)
{
    std::vector<std::vector<int>> poolVecs;
    for (int i = 0; i < numbOfStarterGroups; ++i) {
        poolVecs.push_back(std::vector<int>(maxCardId));
    }

    const char* stmtStr = "select * from StarterPools";
    int rc = sqlite3_exec(fmDb, stmtStr, addToPoolVecs,
                          static_cast<void*>(&poolVecs), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    std::vector<Pool> pools;
    for (unsigned int i = 0; i < poolVecs.size(); ++i) {
        pools.push_back(Pool(poolVecs.at(i)));
    }

    return pools;
}

int addToPoolSizes(void* voidMapPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "PoolID", "SampleSize"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto mapPtr = static_cast<std::map<int, int>*>(voidMapPtr);
    mapPtr->insert({std::atoi(argv[0]), std::atoi(argv[1])});
    return 0;
}

std::vector<int> dbPoolSizes(sqlite3* fmDb)
{
    std::map<int, int> poolSizes;

    const char* stmtStr = "select * from StarterGroups";
    int rc = sqlite3_exec(fmDb, stmtStr, addToPoolSizes,
                          static_cast<void*>(&poolSizes), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    std::vector<int> poolSizesVec;
    try {
        for (unsigned int i = 0; i < poolSizes.size(); ++i) {
            poolSizesVec.push_back(poolSizes.at(i + 1));
        }
    }
    catch (const std::out_of_range&) {
        throw BadFMDB(FMDatabaseType::fmDb);
    }

    return poolSizesVec;
}

int addToIntVector(void* voidVectorPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "NumSort"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto vectorPtr = static_cast<std::vector<int>*>(voidVectorPtr);
    vectorPtr->push_back(std::atoi(argv[0]));
    return 0;
}

// grabs all the int vectors in an FMData, save for poolSampleSizes
std::vector<std::vector<int>> dbSixVectors(sqlite3* fmDb)
{
    std::vector<std::vector<int>> sixVecs;
    for (int i = 0; i < 6; ++i) {
        sixVecs.push_back({});
    }

    std::vector<const char*> stmtStrs {
        "select NumSort from"
        "  Cards"
        "  join StarterPools on"
        "    (Cards.NumSort == StarterPools.CardID and StarterPools.Prob > 0"
        "     and Cards.TypeID == 21)"
        "  order by Cards.AbcSort",
        "select NumSort from"
        "  Cards"
        "  join StarterPools on"
        "    (Cards.NumSort == StarterPools.CardID and StarterPools.Prob > 0"
        "     and Cards.TypeID == 25)"
        "  order by Cards.AbcSort",
        "select NumSort from"
        "  Cards"
        "  join StarterPools on"
        "    (Cards.NumSort == StarterPools.CardID and StarterPools.Prob > 0"
        "     and Cards.TypeID == 22)"
        "  order by Cards.AbcSort",
        "select NumSort from"
        "  Cards"
        "  join StarterPools on"
        "    (Cards.NumSort == StarterPools.CardID and StarterPools.Prob > 0"
        "     and Cards.IsDragon == 1)"
        "  order by Cards.AbcSort",
        "select NumSort from"
        "  Cards"
        "  join StarterPools on"
        "    (Cards.NumSort == StarterPools.CardID and StarterPools.Prob > 0"
        "     and Cards.IsThunder == 1)"
        "  order by Cards.AbcSort",
        "select NumSort from"
        "  Cards"
        "  join CardTypes on"
        "    (Cards.TypeID == CardTypes.TypeID and CardTypes.IsMonster == 1)"
        "  join StarterPools on"
        "    (Cards.NumSort == StarterPools.CardID and StarterPools.Prob > 0)"
        "  order by Cards.AbcSort"
    };

    for (int i = 0; i < 6; ++i) {
        int rc = sqlite3_exec(fmDb, stmtStrs.at(i), addToIntVector,
                              static_cast<void*>(&(sixVecs.at(i))), nullptr);
        if (rc != SQLITE_OK) {
            throw BadFMDB(FMDatabaseType::fmDb);
        }
    }

    return sixVecs;
}

// TODO: improve significantly by doing one query and
// ordering by ChoiceID, Position
std::map<Choice, std::vector<Anim>> dbDecomps(sqlite3* animDb)
{
    std::map<Choice, std::vector<Anim>> decomps;
    std::vector<Choice> choices {choiceVals()};
    for (Choice choice : choices) {
        decomps.insert({choice, {}});
    }

    sqlite3_stmt* stmt;
    const char* stmtStr = "select ChoiceID, AnimationID "
                          "from Decomps where Position == ?";
    int rc = sqlite3_prepare_v2(animDb, stmtStr, 62, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::animDb);
    }

    int position = 1;
    while (true) {
        sqlite3_bind_int(stmt, 1, position);
        bool exploredAllRows = false;
        bool anyRowsExist = false;

        while (!exploredAllRows) {
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE) {
                exploredAllRows = true;
            }
            else if (rc == SQLITE_ROW) {
                anyRowsExist = true;
                int choiceID = sqlite3_column_int(stmt, 0);
                int animID = sqlite3_column_int(stmt, 1);
                decomps.at(static_cast<Choice>(choiceID)).push_back(static_cast<Anim>(animID));
            }
            else {
                sqlite3_finalize(stmt);
                throw BadFMDB(FMDatabaseType::animDb);
            }
        }

        if (!anyRowsExist) {
            sqlite3_finalize(stmt);
            return decomps;
        }

        ++position;
        sqlite3_reset(stmt);
    }
}

int fixedAdvsCallback(void* voidMapPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "AnimationID", "Advances"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto mapPtr = static_cast<std::map<Anim, uint16_t>*>(voidMapPtr);
    mapPtr->insert({static_cast<Anim>(std::atoi(argv[0])),
                    static_cast<uint16_t>(std::atoi(argv[1]))});
    return 0;
}

std::map<Anim, uint16_t> dbFixedAdvs(sqlite3* animDb)
{
    std::map<Anim, uint16_t> advSizes;

    const char* stmtStr = "select * from FixedAdvs";
    int rc = sqlite3_exec(animDb, stmtStr, fixedAdvsCallback,
                          static_cast<void*>(&advSizes), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::animDb);
    }

    return advSizes;
}

int variableAdvsCallback(void* voidMapPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "AnimationID", "InitialSeedIndex", "AdvanceSize"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto mapPtr = static_cast<std::map<Anim, std::vector<uint16_t>>*>(voidMapPtr);

    int animId = std::atoi(argv[0]);
    int initSeedIndex = std::atoi(argv[1]);
    uint16_t seedAdv = static_cast<uint16_t>(std::atoi(argv[2]));

    if (mapPtr->at(static_cast<Anim>(animId)).size()
            != static_cast<uint32_t>(initSeedIndex)) {
        return 1;
    }
    else {
        mapPtr->at(static_cast<Anim>(animId)).push_back(seedAdv);
    }

    return 0;
}

std::map<Anim, std::vector<uint16_t>> dbVariableAdvs(sqlite3* animDb)
{
    std::vector<Anim> anims {animVals()};
    std::map<Anim, std::vector<uint16_t>> variableAdvs;

    for (Anim anim : anims) {
        variableAdvs.insert({anim, {}});
    }

    const char* stmtStr = "select * from VariableAdvs order by InitialSeedIndex";
    int rc = sqlite3_exec(animDb, stmtStr, variableAdvsCallback,
                          static_cast<void*>(&variableAdvs), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::animDb);
    }

    return variableAdvs;
}

int timeCostsCallback(void* voidMapPtr, int argc, char** argv, char** colNames)
{
    const std::vector<const char*> correctNames {
        "ChoiceID", "TimeCost"
    };
    if (!compareStrings(argc, colNames, correctNames)) {
        return 1;
    }

    auto mapPtr = static_cast<std::map<Choice, int>*>(voidMapPtr);
    mapPtr->insert({static_cast<Choice>(std::atoi(argv[0])),
                    std::atoi(argv[1])});
    return 0;
}

std::map<Choice, int> dbTimeCosts(sqlite3* animDb)
{
    std::map<Choice, int> timeCosts;

    const char* stmtStr = "select ChoiceID, TimeCost from ChoiceTypes";
    int rc = sqlite3_exec(animDb, stmtStr, timeCostsCallback,
                          static_cast<void*>(&timeCosts), nullptr);
    if (rc != SQLITE_OK) {
        throw BadFMDB(FMDatabaseType::animDb);
    }

    return timeCosts;
}

FMData::FMData(sqlite3* fmDb, sqlite3* animDb)
{
    cardInfo = dbCards(fmDb);
    duelistInfo = dbDuelists(fmDb);

    starterDeckPools = dbStarterPools(fmDb);
    poolSampleSizes = dbPoolSizes(fmDb);

    auto sixVecs = dbSixVectors(fmDb);
    magicsList = sixVecs.at(0);
    equipsList = sixVecs.at(1);
    fieldsList = sixVecs.at(2);
    dragsList = sixVecs.at(3);
    thundsList = sixVecs.at(4);
    starterMonsters = sixVecs.at(5);

    decomps = dbDecomps(animDb);
    fixedAdvCounts = dbFixedAdvs(animDb);
    variableAdvData = dbVariableAdvs(animDb);
    timeCosts = dbTimeCosts(animDb);
}

const std::vector<Card>& FMData::getCardInfo() const
{
    return cardInfo;
}

const std::vector<Duelist>& FMData::getDuelistInfo() const
{
    return duelistInfo;
}

const std::vector<Pool>& FMData::getStarterPools() const
{
    return starterDeckPools;
}

const std::vector<int>& FMData::getSampleSizes() const
{
    return poolSampleSizes;
}

const std::vector<int>& FMData::getMagics() const
{
    return magicsList;
}

const std::vector<int>& FMData::getEquips() const
{
    return equipsList;
}

const std::vector<int>& FMData::getFields() const
{
    return fieldsList;
}

const std::vector<int>& FMData::getDragons() const
{
    return dragsList;
}

const std::vector<int>& FMData::getThunders() const
{
    return thundsList;
}

const std::vector<int>& FMData::getStartMonsters() const
{
    return starterMonsters;
}

const std::map<Choice, std::vector<Anim>>& FMData::getDecomps() const
{
    return decomps;
}

const std::map<Anim, uint16_t>& FMData::getFixedAdvs() const
{
    return fixedAdvCounts;
}

const std::map<Anim, std::vector<uint16_t>>& FMData::getVariableAdvs() const
{
    return variableAdvData;
}

const std::map<Choice, int>& FMData::getTimeCosts() const
{
    return timeCosts;
}

std::shared_ptr<FMData> readFromDbs(std::string fmDbPath,
                                    std::string animDbPath)
{
    sqlite3* fmDb;
    int rc = sqlite3_open_v2(fmDbPath.c_str(), &fmDb,
                             SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        throw MissingFMDB(FMDatabaseType::fmDb);
    }

    sqlite3* animDb;
    rc = sqlite3_open_v2(animDbPath.c_str(), &animDb,
                         SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close_v2(fmDb);
        throw MissingFMDB(FMDatabaseType::animDb);
    }

    try {
        std::shared_ptr<FMData> dbDump(new FMData(fmDb, animDb));
        sqlite3_close_v2(fmDb);
        sqlite3_close_v2(animDb);
        return dbDump;
    }
    catch (const BadFMDB& e) {
        sqlite3_close_v2(fmDb);
        sqlite3_close_v2(animDb);
        throw e;
    }
}
