#include "Models/Card.h"

namespace cardgame {

std::string rankName(int rank) {
    switch (rank) {
        case 1:
            return "A";
        case 11:
            return "J";
        case 12:
            return "Q";
        case 13:
            return "K";
        default:
            return std::to_string(rank);
    }
}

bool isRed(Suit suit) {
    return suit == Suit::Heart || suit == Suit::Diamond;
}

std::string colorName(Suit suit) {
    return isRed(suit) ? "red" : "black";
}

std::string suitAsset(Suit suit) {
    switch (suit) {
        case Suit::Spade:
            return "res/suits/spade.png";
        case Suit::Heart:
            return "res/suits/heart.png";
        case Suit::Club:
            return "res/suits/club.png";
        case Suit::Diamond:
            return "res/suits/diamond.png";
    }
    return "res/suits/spade.png";
}

std::string bigRankAsset(const Card& card) {
    return "res/number/big_" + colorName(card.suit) + "_" + rankName(card.rank) + ".png";
}

std::string smallRankAsset(const Card& card) {
    return "res/number/small_" + colorName(card.suit) + "_" + rankName(card.rank) + ".png";
}

}  // namespace cardgame
