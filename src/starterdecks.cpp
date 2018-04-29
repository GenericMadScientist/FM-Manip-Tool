#include <algorithm>
#include <tuple>

#include "constants.hpp"
#include "seedfuns.hpp"
#include "shiftfuns.hpp"
#include "starterdecks.hpp"

std::vector<AffineShift> deckShifts()
{
    std::vector<AffineShift> shiftsVec;
    for (int i = 0; i <= maxCardId; ++i) {
        shiftsVec.push_back(fmShift() ^ i);
    }
    return shiftsVec;
}

std::tuple<uint32_t, std::vector<int>> deckFragment(uint32_t seed,
                                                    int fragNumber,
                                                    const FMData& fmData)
{
    static const std::vector<AffineShift> shiftsVec {deckShifts()};
    int fragSize = fmData.getSampleSizes().at(fragNumber);
    const std::vector<Pool>& fragPools {fmData.getStarterPools()};

    std::vector<int> fragment;
    int nextCard = -1;
    while (static_cast<int>(fragment.size()) < fragSize) {
        seed = nextSeed(seed);
        nextCard = fragPools.at(fragNumber).drop(seed);
        seed = shiftsVec[nextCard](seed);
        if (std::count(fragment.begin(),
                       fragment.end(), nextCard) != maxCopyCount) {
            fragment.push_back(nextCard);
        }
    }

    return std::make_tuple(seed, fragment);
}

Deck starterDeck(uint32_t seed, const FMData& fmData)
{
    std::vector<int> deckVec;

    for (int i = 0; i < 7; ++i) {
        auto pair = deckFragment(seed, i, fmData);
        seed = std::get<0>(pair);
        std::vector<int> newFrag {std::get<1>(pair)};
        deckVec.insert(deckVec.end(), newFrag.begin(), newFrag.end());
    }

    return Deck(deckVec);
}

std::vector<int> goodDecks(long long startSeedInd, int numbOfSeeds,
                           int dragCount, int thundCount,
                           const std::vector<int>& requiredCards,
                           const FMData& fmData)
{
    uint32_t seed = indexToSeed(startSeedInd);
    std::vector<int> goodDecks;
    std::vector<int> currentCards;
    int currentDrags = 0;
    int currentThunds = 0;
    bool foundAll = true;

    const std::vector<int>& drags {fmData.getDragons()};
    const std::vector<int>& thunds {fmData.getThunders()};

    for (int i = 0; i < numbOfSeeds; ++i) {
        currentCards = starterDeck(seed, fmData).deck();
        currentDrags = 0;
        currentThunds = 0;
        for (int j : currentCards) {
            if (std::find(drags.begin(), drags.end(), j) != drags.end()) {
                currentDrags += 1;
            }
            if (std::find(thunds.begin(), thunds.end(), j) != thunds.end()) {
                currentThunds += 1;
            }
        }
        if ((currentDrags >= dragCount) && (currentThunds >= thundCount)) {
            foundAll = true;
            for (int j : requiredCards) {
                if (std::find(currentCards.begin(),
                              currentCards.end(), j) == currentCards.end()) {
                    foundAll = false;
                }
            }
            if (foundAll) {
                goodDecks.push_back(startSeedInd + i);
            }
        }
        seed = nextSeed(seed);
    }

    return goodDecks;
}
