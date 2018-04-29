#include <algorithm>
#include <stdexcept>
#include <tuple>

#include "cards.hpp"
#include "constants.hpp"

CardsList::CardsList(const std::vector<int>& cardList)
{
    for (int i : cardList) {
        if ((i <= 0) || (i > maxCardId)) {
            throw std::range_error("Invalid card values");
        }
        cards.push_back(i);
    }
}

std::vector<int> CardsList::cardList() const
{
    return cards;
}

void sortCards(CardsList& cards, Sort sort,
               const std::vector<Card>& cardInfo)
{
    std::vector<int> oldList {cards.cardList()};
    std::vector<std::tuple<int, int>> pairs;

    for (int i : oldList) {
        pairs.push_back(std::make_tuple(i, cardInfo[i - 1].places.at(sort)));
    }

    std::sort(pairs.begin(), pairs.end(),
              [](auto a, auto b) { return std::get<1>(a) < std::get<1>(b); });

    oldList.clear();

    for (auto t : pairs) {
        oldList.push_back(std::get<0>(t));
    }

    cards = CardsList(oldList);
}
