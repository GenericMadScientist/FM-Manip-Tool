#ifndef FMDATA_HPP
#define FMDATA_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "sqlite3.h"

#include "cards.hpp"
#include "duelists.hpp"
#include "manipanims.hpp"
#include "pools.hpp"

class FMData {
public:
    FMData(sqlite3*, sqlite3*);
    const std::vector<Card>& getCardInfo() const;
    const std::vector<Duelist>& getDuelistInfo() const;
    const std::vector<Pool>& getStarterPools() const;
    const std::vector<int>& getSampleSizes() const;

    const std::vector<int>& getMagics() const;
    const std::vector<int>& getEquips() const;
    const std::vector<int>& getFields() const;
    const std::vector<int>& getDragons() const;
    const std::vector<int>& getThunders() const;
    const std::vector<int>& getStartMonsters() const;

    const std::map<Choice, std::vector<Anim>>& getDecomps() const;
    const std::map<Anim, uint16_t>& getFixedAdvs() const;
    const std::map<Anim, std::vector<uint16_t>>& getVariableAdvs() const;
    const std::map<Choice, int>& getTimeCosts() const;
private:
    std::vector<Card> cardInfo;
    std::vector<Duelist> duelistInfo;

    std::vector<Pool> starterDeckPools;
    std::vector<int> poolSampleSizes;

    std::vector<int> magicsList;
    std::vector<int> equipsList;
    std::vector<int> fieldsList;
    std::vector<int> dragsList;
    std::vector<int> thundsList;
    std::vector<int> starterMonsters;

    std::map<Choice, std::vector<Anim>> decomps;
    std::map<Anim, uint16_t> fixedAdvCounts;
    std::map<Anim, std::vector<uint16_t>> variableAdvData;
    std::map<Choice, int> timeCosts;
};

std::shared_ptr<FMData> readFromDbs(std::string, std::string);

enum class FMDatabaseType { fmDb, animDb };

class BadFMDB : public std::runtime_error {
public:
    explicit BadFMDB(FMDatabaseType badDbType)
        : std::runtime_error(""), dbType{badDbType} {}
    FMDatabaseType getDbType() { return dbType; }
    const char* what() const noexcept override
    {
        return "Invalid database";
    }
private:
    FMDatabaseType dbType;
};

class MissingFMDB : public std::runtime_error {
public:
    explicit MissingFMDB(FMDatabaseType badDbType)
        : std::runtime_error(""), dbType{badDbType} {}
    FMDatabaseType getDbType() { return dbType; }
    const char* what() const noexcept override
    {
        return "Missing database";
    }
private:
    FMDatabaseType dbType;
};

#endif
