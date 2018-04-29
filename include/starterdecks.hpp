#ifndef STARTERDECKS_HPP
#define STARTERDECKS_HPP

#include <cstdint>
#include <vector>

#include "decks.hpp"
#include "fmdata.hpp"

Deck starterDeck(uint32_t, const FMData&);
std::vector<int> goodDecks(long long, int, int, int, const std::vector<int>&,
                           const FMData&);

#endif
