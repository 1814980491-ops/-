#include "Views/CardView.h"

#include <new>

namespace cardgame {

CardView* CardView::create(const Card& card) {
    auto view = new (std::nothrow) CardView();
    if (view && view->initWithCard(card)) {
        view->autorelease();
        return view;
    }
    delete view;
    return nullptr;
}

bool CardView::initWithCard(const Card& card) {
    if (!Node::init()) {
        return false;
    }
    updateCard(card, true);
    return true;
}

void CardView::updateCard(const Card& card, bool faceUp) {
    _cardId = card.id;
    removeAllChildren();

    auto base = cocos2d::Sprite::create("res/card_general.png");
    if (!base) {
        return;
    }

    _cardSize = base->getContentSize();
    setContentSize(_cardSize);
    setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    base->setPosition(_cardSize.width * 0.5f, _cardSize.height * 0.5f);
    addChild(base);

    if (!faceUp) {
        drawBack();
        return;
    }

    addCorner(card, false);
    addCorner(card, true);

    auto bigRank = cocos2d::Sprite::create(bigRankAsset(card));
    if (bigRank) {
        bigRank->setPosition(_cardSize.width * 0.5f, _cardSize.height * 0.57f);
        addChild(bigRank, 2);
    }

    auto bigSuit = cocos2d::Sprite::create(suitAsset(card.suit));
    if (bigSuit) {
        bigSuit->setScale(1.35f);
        bigSuit->setPosition(_cardSize.width * 0.5f, _cardSize.height * 0.30f);
        addChild(bigSuit, 2);
    }
}

bool CardView::containsWorldPoint(const cocos2d::Vec2& worldPoint) const {
    const auto local = convertToNodeSpace(worldPoint);
    return cocos2d::Rect(0.0f, 0.0f, _cardSize.width, _cardSize.height).containsPoint(local);
}

void CardView::addCorner(const Card& card, bool bottom) {
    auto number = cocos2d::Sprite::create(smallRankAsset(card));
    auto suit = cocos2d::Sprite::create(suitAsset(card.suit));
    if (!number || !suit) {
        return;
    }

    number->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    suit->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));

    if (bottom) {
        number->setRotation(180.0f);
        suit->setRotation(180.0f);
        number->setPosition(_cardSize.width - 31.0f, 38.0f);
        suit->setPosition(_cardSize.width - 31.0f, 82.0f);
    } else {
        number->setPosition(31.0f, _cardSize.height - 38.0f);
        suit->setPosition(31.0f, _cardSize.height - 82.0f);
    }

    addChild(number, 2);
    addChild(suit, 2);
}

void CardView::drawBack() {
    auto panel = cocos2d::DrawNode::create();
    panel->drawSolidRect(cocos2d::Vec2(16.0f, 16.0f),
                         cocos2d::Vec2(_cardSize.width - 16.0f, _cardSize.height - 16.0f),
                         cocos2d::Color4F(0.05f, 0.45f, 0.95f, 1.0f));
    panel->drawRect(cocos2d::Vec2(20.0f, 20.0f),
                    cocos2d::Vec2(_cardSize.width - 20.0f, _cardSize.height - 20.0f),
                    cocos2d::Color4F(0.65f, 0.85f, 1.0f, 1.0f));
    panel->drawRect(cocos2d::Vec2(32.0f, 32.0f),
                    cocos2d::Vec2(_cardSize.width - 32.0f, _cardSize.height - 32.0f),
                    cocos2d::Color4F(0.20f, 0.70f, 1.0f, 1.0f));
    addChild(panel, 3);
}

}  // namespace cardgame
