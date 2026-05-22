#include "MainScene.h"

#include "audio/include/AudioEngine.h"

#include <limits>

namespace cardgame {
namespace {
const float kMoveDuration = 0.28f;
const char* kMoveTableSound = "move_table.mp3";
const char* kMoveStockSound = "move_stock.mp3";
const char* kUndoSound = "undo.mp3";
const char* kFailedSound = "failed.mp3";
const char* kSuccessSound = "success.mp3";
}

cocos2d::Scene* MainScene::createScene() {
    auto scene = cocos2d::Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    return scene;
}

bool MainScene::init() {
    if (!Layer::init()) {
        return false;
    }

    _state.setupDemo();
    _lastResult = _state.result();
    preloadSounds();
    buildBoard();
    buildCards();
    refreshVisibility();
    refreshResult();
    return true;
}

void MainScene::buildBoard() {
    const auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    auto background = cocos2d::LayerColor::create(cocos2d::Color4B(29, 121, 91, 255),
                                                  visibleSize.width,
                                                  visibleSize.height);
    addChild(background, -10);

    auto tableTitle = cocos2d::Label::createWithSystemFont("MAIN PILES", "Arial", 34);
    tableTitle->setPosition(540.0f, 1770.0f);
    tableTitle->setTextColor(cocos2d::Color4B(236, 244, 231, 255));
    addChild(tableTitle);

    auto stockTitle = cocos2d::Label::createWithSystemFont("STOCK", "Arial", 32);
    stockTitle->setPosition(stockPosition().x, 585.0f);
    stockTitle->setTextColor(cocos2d::Color4B(236, 244, 231, 255));
    addChild(stockTitle);

    auto topTitle = cocos2d::Label::createWithSystemFont("TOP", "Arial", 32);
    topTitle->setPosition(wastePosition().x, 585.0f);
    topTitle->setTextColor(cocos2d::Color4B(236, 244, 231, 255));
    addChild(topTitle);

    auto undoLabel = cocos2d::Label::createWithSystemFont("UNDO", "Arial", 34);
    undoLabel->setTextColor(cocos2d::Color4B(255, 246, 190, 255));
    auto undoItem = cocos2d::MenuItemLabel::create(undoLabel, CC_CALLBACK_1(MainScene::onUndo, this));
    auto menu = cocos2d::Menu::create(undoItem, nullptr);
    menu->setPosition(860.0f, 320.0f);
    addChild(menu, 50);

    _resultLabel = cocos2d::Label::createWithSystemFont("", "Arial", 54);
    _resultLabel->setPosition(540.0f, 1030.0f);
    _resultLabel->setTextColor(cocos2d::Color4B(255, 246, 190, 255));
    _resultLabel->setVisible(false);
    addChild(_resultLabel, 80);
}

void MainScene::buildCards() {
    for (const auto& card : _state.cards()) {
        auto view = CardView::create(card);
        view->setPosition(card.position);
        addChild(view, 10);
        _views[card.id] = view;
        bindTouch(view);
    }
}

void MainScene::bindTouch(CardView* view) {
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this, view](cocos2d::Touch* touch, cocos2d::Event*) {
        if (_busy || _state.result() != GameResult::Playing || !view->isVisible()) {
            return false;
        }
        return frontCardAtPoint(touch->getLocation()) == view->cardId();
    };
    listener->onTouchEnded = [this, view](cocos2d::Touch*, cocos2d::Event*) {
        onCardTapped(view->cardId());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, view);
}

void MainScene::onCardTapped(int cardId) {
    const Card* card = _state.cardById(cardId);
    if (!card || !_state.isVisibleTop(cardId) || card->zone == Zone::Waste || _state.result() != GameResult::Playing) {
        return;
    }

    MoveRecord record;
    if (card->zone == Zone::Table && _state.moveTableTopToWaste(cardId, &record)) {
        playSound(kMoveTableSound);
        animateToWaste(record);
    } else if (card->zone == Zone::Stock && _state.moveStockTopToWaste(cardId, &record)) {
        playSound(kMoveStockSound);
        animateToWaste(record);
    }
}

void MainScene::onUndo(cocos2d::Ref*) {
    if (_busy) {
        return;
    }

    MoveRecord record;
    if (_state.undoLast(&record)) {
        playSound(kUndoSound);
        animateUndo(record);
    }
}

void MainScene::animateToWaste(const MoveRecord& record) {
    auto found = _views.find(record.cardId);
    if (found == _views.end()) {
        refreshVisibility();
        return;
    }

    _busy = true;
    auto view = found->second;
    view->setVisible(true);
    view->setLocalZOrder(100);
    view->runAction(cocos2d::Sequence::create(
        cocos2d::MoveTo::create(kMoveDuration, wastePosition()),
        cocos2d::CallFunc::create([this]() {
            _busy = false;
            refreshVisibility();
            refreshResult();
        }),
        nullptr));
}

void MainScene::animateUndo(const MoveRecord& record) {
    refreshVisibility();
    auto found = _views.find(record.cardId);
    if (found == _views.end()) {
        return;
    }

    _busy = true;
    auto view = found->second;
    view->setVisible(true);
    view->setPosition(wastePosition());
    view->setLocalZOrder(120);
    view->runAction(cocos2d::Sequence::create(
        cocos2d::MoveTo::create(kMoveDuration, record.fromPosition),
        cocos2d::CallFunc::create([this]() {
            _busy = false;
            refreshVisibility();
            refreshResult();
        }),
        nullptr));
}

int MainScene::frontCardAtPoint(const cocos2d::Vec2& worldPoint) const {
    int cardId = -1;
    int bestZ = std::numeric_limits<int>::min();

    for (const auto& item : _views) {
        auto view = item.second;
        if (!view || !view->isVisible() || !view->containsWorldPoint(worldPoint)) {
            continue;
        }
        if (view->getLocalZOrder() > bestZ) {
            bestZ = view->getLocalZOrder();
            cardId = item.first;
        }
    }

    return cardId;
}

void MainScene::refreshVisibility() {
    int order = 10;
    for (const auto& card : _state.cards()) {
        auto found = _views.find(card.id);
        if (found == _views.end()) {
            continue;
        }

        auto view = found->second;
        bool visible = false;
        bool faceUp = false;

        if (card.zone == Zone::Table) {
            visible = true;
            faceUp = _state.isFaceUpTableCard(card.id);
        } else if (card.zone == Zone::Stock) {
            visible = _state.stockTopCard() == card.id;
            faceUp = visible;
        } else if (card.zone == Zone::Waste) {
            visible = !_state.wasteStack().empty() && _state.wasteStack().back() == card.id;
            faceUp = visible;
        }

        view->updateCard(card, faceUp);
        view->setPosition(card.position);
        view->setVisible(visible);
        view->setLocalZOrder(order++);
    }

    if (!_state.stockPile().empty()) {
        auto found = _views.find(_state.stockTopCard());
        if (found != _views.end()) {
            found->second->setLocalZOrder(40);
        }
    }

    if (!_state.wasteStack().empty()) {
        auto found = _views.find(_state.wasteStack().back());
        if (found != _views.end()) {
            found->second->setLocalZOrder(60);
        }
    }
}

void MainScene::refreshResult() {
    if (!_resultLabel) {
        return;
    }

    const GameResult result = _state.result();
    if (result != _lastResult) {
        if (result == GameResult::Success) {
            playSound(kSuccessSound);
        } else if (result == GameResult::Failed) {
            playSound(kFailedSound);
        }
        _lastResult = result;
    }

    if (result == GameResult::Success) {
        _resultLabel->setString("SUCCESS");
        _resultLabel->setVisible(true);
    } else if (result == GameResult::Failed) {
        _resultLabel->setString("FAILED");
        _resultLabel->setVisible(true);
    } else {
        _resultLabel->setVisible(false);
    }
}

void MainScene::preloadSounds() {
    cocos2d::experimental::AudioEngine::preload(kMoveTableSound);
    cocos2d::experimental::AudioEngine::preload(kMoveStockSound);
    cocos2d::experimental::AudioEngine::preload(kUndoSound);
    cocos2d::experimental::AudioEngine::preload(kFailedSound);
    cocos2d::experimental::AudioEngine::preload(kSuccessSound);
}

void MainScene::playSound(const char* filePath) {
    cocos2d::experimental::AudioEngine::play2d(filePath, false, 1.0f);
}

cocos2d::Vec2 MainScene::wastePosition() const {
    return cocos2d::Vec2(540.0f, 320.0f);
}

cocos2d::Vec2 MainScene::stockPosition() const {
    return cocos2d::Vec2(270.0f, 320.0f);
}

}  // namespace cardgame
