#ifndef SEEDFUNS_HPP
#define SEEDFUNS_HPP

#include <cstdint>

#include "constants.hpp"

inline uint32_t nextSeed(uint32_t seed)
{
    return rngMult * seed + rngInc;
}

uint32_t advance(uint32_t, long long);
uint32_t indexToSeed(long long);
long long seedToIndex(uint32_t);
uint32_t prn(uint32_t);

#endif
