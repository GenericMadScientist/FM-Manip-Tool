#ifndef CARDS_HPP
#define CARDS_HPP

#include <map>
#include <string>
#include <vector>

enum class Sort { num, abc, max, atk, def, typ, ai };

struct Card {
    std::string name;
    std::map<Sort, int> places;
    Card(std::string cardName, int numPlace, int abcPlace,
         int maxPlace, int atkPlace, int defPlace,
         int typPlace, int aiPlace)
        : name{cardName},
          places{{{Sort::num, numPlace}, {Sort::abc, abcPlace},
                  {Sort::max, maxPlace}, {Sort::atk, atkPlace},
                  {Sort::def, defPlace}, {Sort::typ, typPlace},
                  {Sort::ai, aiPlace}}} {}
};

class CardsList {
public:
    CardsList() {}
    explicit CardsList(const std::vector<int>&);
    std::vector<int> cardList() const;
private:
    std::vector<int> cards;
};

void sortCards(CardsList&, Sort, const std::vector<Card>&);

#endif
