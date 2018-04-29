#ifndef POOLS_HPP
#define POOLS_HPP

#include <cstdint>
#include <vector>

#include <QMetaType>

#include "constants.hpp"

// Constructor throws a std::runtime_error exception if the pool is not of
// size 722, since this should only come from an incorrect database.
class Pool {
public:
    explicit Pool(const std::vector<int>&);
    int drop(uint32_t seed) const { return poolVec[(seed >> 16) % totalProb]; }
    std::vector<int> possibleDrops() const;
    std::vector<uint32_t> matchingDropSlots(int) const;
private:
    std::vector<int> poolVec;
};

enum class PoolType { deck, saPow, bcd, saTec };
Q_DECLARE_METATYPE(PoolType)

#endif
