#ifndef CARD_PILE_MATCH_MODELS_CARD_H_
#define CARD_PILE_MATCH_MODELS_CARD_H_

#include "cocos2d.h"

#include <string>
#include <vector>

namespace cardgame {

enum class Suit {
    Spade,
    Heart,
    Club,
    Diamond
};

enum class Zone {
    Table,
    Stock,
    Waste
};

struct Card {
    int id = -1;
    int rank = 1;
    Suit suit = Suit::Spade;
    Zone zone = Zone::Table;
    int pileIndex = -1;
    cocos2d::Vec2 position;
    std::vector<int> blockers;
};

std::string rankName(int rank);
std::string colorName(Suit suit);
std::string suitAsset(Suit suit);
std::string bigRankAsset(const Card& card);
std::string smallRankAsset(const Card& card);
bool isRed(Suit suit);

}  // namespace cardgame

#endif
