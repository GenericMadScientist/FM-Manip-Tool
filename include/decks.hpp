#ifndef SHUFFLE_HPP
#define SHUFFLE_HPP

#include <cstdint>
#include <vector>

#include "cards.hpp"
#include "duelists.hpp"
#include "pools.hpp"

// Constructor throws a std::invalid_argument exception if the "deck" is not
// of length 40.
class Deck {
public:
    explicit Deck(const std::vector<int>&);
    const std::vector<int>& deck() const { return cards; }
    void shuffle(uint32_t);
private:
    std::vector<int> cards;
};

Deck aiDeck(const Pool&, uint32_t);
uint32_t aiDeckAdvance(const Pool&, uint32_t);
std::vector<int> partialAiSort(const Deck&, int handSize,
                               const std::vector<Card>&,
                               int numbOfPlays);
Deck aiSort(const Deck&, int, const std::vector<Card>&);
std::vector<long long> easyModes(const Duelist&, const std::vector<int>&, int,
                                 long long, int, const std::vector<Card>&);

#endif
