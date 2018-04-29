#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>

#include "constants.hpp"
#include "decks.hpp"
#include "seedfuns.hpp"
#include "shiftfuns.hpp"

Deck::Deck(const std::vector<int>& deck)
{
    if (deck.size() != deckSize) {
        throw std::invalid_argument("Deck not of length 40");
    }
    for (int i : deck) {
        if (i < 0 || i > maxCardId) {
            throw std::invalid_argument("Not all cards have a valid ID");
        }
    }
    cards = deck;
}

void Deck::shuffle(uint32_t seed)
{
    constexpr int shuffleSwapCount = 160;
    for (int i = 0; i < shuffleSwapCount; ++i) {
        seed = nextSeed(seed);
        int indexOne = prn(seed) % deckSize;
        seed = nextSeed(seed);
        int indexTwo = prn(seed) % deckSize;
        std::swap(cards[indexOne], cards[indexTwo]);
    }
}

std::tuple<uint32_t, Deck> genAiDeck(const Pool& deckPool, uint32_t seed)
{
    static std::vector<int> addedCards(maxCardId + 1, 0);
    static const AffineShift shuffleAdv = fmShift() ^ 320;
    std::vector<int> deckVec;

    seed = shuffleAdv(seed); // For player's deck shuffle.

    for (int i = 0; i < deckSize; ++i) {
        int card = -1;
        while ((card == -1) || (addedCards[card] == maxCopyCount)) {
            seed = nextSeed(seed);
            card = deckPool.drop(seed);
        }
        deckVec.push_back(card);
        ++addedCards[card];
    }
    std::fill(addedCards.begin(), addedCards.end(), 0);

    Deck deck(deckVec);
    deck.shuffle(seed);
    return std::make_tuple(shuffleAdv(seed), deck);
}

Deck aiDeck(const Pool& deckPool, uint32_t seed)
{
    return std::get<Deck>(genAiDeck(deckPool, seed));
}

uint32_t aiDeckAdvance(const Pool& deckPool, uint32_t seed)
{
    return std::get<uint32_t>(genAiDeck(deckPool, seed));
}

void makeAiPlay(std::vector<std::tuple<int, int>>& aiDeck, int rangeStart,
                int rangeEnd)
{
    int bestLocation = rangeStart;
    for (int i = rangeStart + 1; i <= rangeEnd; ++i) {
        if (std::get<1>(aiDeck[i]) < std::get<1>(aiDeck[bestLocation])) {
            bestLocation = i;
        }
    }

    if (bestLocation < rangeStart + defaultHandSize) {
        while (bestLocation > rangeStart) {
            std::swap(aiDeck[bestLocation - 1], aiDeck[bestLocation]);
            --bestLocation;
        }
    }
    else {
        std::swap(aiDeck[rangeStart], aiDeck[bestLocation]);
    }
}

std::vector<int> partialAiSort(const Deck& deck, int handSize,
                               const std::vector<Card>& cardInfo,
                               int numbOfPlays)
{
    std::vector<std::tuple<int, int>> pairsVec;
    for (int i : deck.deck()) {
        auto newTuple = std::make_tuple(i,
                                        cardInfo[i - 1].places.at(Sort::ai));
        pairsVec.push_back(newTuple);
    }

    for (int i = 0; i < numbOfPlays; ++i) {
        makeAiPlay(pairsVec, i, std::min(deckSize, handSize + i) - 1);
    }

    std::vector<int> cards;
    for (int i = 0; i < numbOfPlays; ++i) {
        cards.push_back(std::get<0>(pairsVec[i]));
    }

    return cards;
}

Deck aiSort(const Deck& deck, int handSize,
            const std::vector<Card>& cardInfo)
{
    return Deck(partialAiSort(deck, handSize, cardInfo, deckSize));
}

bool checkEasyMode(const Pool& aiDeckPool, const std::vector<int>& bannedCards,
                   int numbOfTurns, int handSize, uint32_t seed,
                   const std::vector<Card>& cardInfo)
{
    Deck deck {aiDeck(aiDeckPool, seed)};
    std::vector<int> aiPlays {partialAiSort(deck, handSize, cardInfo,
                                            numbOfTurns)};

    int currentPlay = 0;

    for (int i = 0; i < numbOfTurns; ++i) {
        currentPlay = aiPlays[i];
        auto iter = std::find(bannedCards.begin(), bannedCards.end(),
                              currentPlay);
        if (iter != bannedCards.end()) {
            return false;
        }
    }
    return true;
}

std::vector<long long> easyModes(const Duelist& duelist,
                                 const std::vector<int>& bannedCards,
                                 int numbOfTurns, long long startSeedInd,
                                 int numbOfDuels,
                                 const std::vector<Card>& cardInfo)
{
    const Pool& aiDeckPool {duelist.pools.at(PoolType::deck)};
    int handSize = duelist.handSize;

    uint32_t seed = indexToSeed(startSeedInd);

    std::vector<long long> easyModeInds;
    for (int i = 0; i < numbOfDuels; ++i) {
        if (checkEasyMode(aiDeckPool, bannedCards, numbOfTurns,
                          handSize, seed, cardInfo)) {
            easyModeInds.push_back((startSeedInd + i) % 4294967296);
        }
        seed = nextSeed(seed);
    }

    return easyModeInds;
}
