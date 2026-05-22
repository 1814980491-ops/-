#ifndef CARD_PILE_MATCH_VIEWS_CARD_VIEW_H_
#define CARD_PILE_MATCH_VIEWS_CARD_VIEW_H_

#include "Models/Card.h"
#include "cocos2d.h"

namespace cardgame {

class CardView : public cocos2d::Node {
public:
    static CardView* create(const Card& card);

    bool initWithCard(const Card& card);
    void updateCard(const Card& card, bool faceUp);
    bool containsWorldPoint(const cocos2d::Vec2& worldPoint) const;
    int cardId() const { return _cardId; }

private:
    void addCorner(const Card& card, bool bottom);
    void drawBack();

    int _cardId = -1;
    cocos2d::Size _cardSize;
};

}  // namespace cardgame

#endif
