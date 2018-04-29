#include <algorithm>
#include <stdexcept>

#include "pools.hpp"
#include "seedfuns.hpp"

Pool::Pool(const std::vector<int>& pool)
{
    if (pool.size() != maxCardId) {
        throw std::runtime_error("Bad database.");
    }
    // We do not check sum == 2048 to allow for the 000 glitch.
    poolVec = std::vector<int>(totalProb, 0);

    int totalLen = 0;
    for (int i = 0; i < maxCardId; ++i) {
        for (int j = totalLen; j < totalLen + pool[i]; ++j) {
            if (j == totalProb) {
                return;
            }
            poolVec[j] = i + 1;
        }
        totalLen += pool[i];
    }
}

std::vector<int> Pool::possibleDrops() const
{
    std::vector<int> drops {poolVec};
    std::sort(drops.begin(), drops.end());
    drops.erase(std::unique(drops.begin(), drops.end()), drops.end());
    return drops;
}

std::vector<uint32_t> Pool::matchingDropSlots(int card) const
{
    std::vector<uint32_t> dropSlots;

    for (uint32_t i = 0; i < poolVec.size(); ++i) {
        if (poolVec[i] == card) {
            dropSlots.push_back(i);
        }
    }

    return dropSlots;
}
