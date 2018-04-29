#ifndef DUELISTS_HPP
#define DUELISTS_HPP

#include <map>
#include <string>

#include "pools.hpp"

struct Duelist {
    std::string name;
    int handSize;
    bool isMage;
    std::map<PoolType, Pool> pools;
    Duelist(std::string duelistName, int duelistHand,
            bool mageFlag, Pool deck, Pool pows,
            Pool bcds, Pool tecs)
        : name{duelistName}, handSize{duelistHand}, isMage{mageFlag},
          pools{{PoolType::deck, deck}, {PoolType::saPow, pows},
                {PoolType::bcd, bcds}, {PoolType::saTec, tecs}} {}
};

#endif
