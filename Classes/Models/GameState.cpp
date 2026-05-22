#include "Models/GameState.h"

#include <algorithm>
#include <cstdlib>

namespace cardgame {
namespace {
const cocos2d::Vec2 kWastePosition(540.0f, 320.0f);
}

void GameState::setupDemo() {
    _cards.clear();
    _tablePiles.clear();
    _stockPile.clear();
    _wasteStack.clear();
    _undoStack.clear();
    _initiallySolvable = false;

    _tablePiles.resize(3);

    const cocos2d::Vec2 stockPosition(270.0f, 320.0f);
    const float peakCenterX[3] = {170.0f, 540.0f, 910.0f};
    const float topY = 1590.0f;
    const float midY = 1390.0f;
    const float bottomY = 1190.0f;
    const float tailOneY = 1050.0f;
    const float tailTwoY = 910.0f;
    const float offsetX = 60.0f;

    auto addPeak = [this, peakCenterX, topY, midY, bottomY, tailOneY, tailTwoY, offsetX](
        int pileIndex,
        const Card& bottomLeft,
        const Card& bottomRight,
        const Card& tailOne,
        const Card& tailTwo,
        const Card& middleLeft,
        const Card& middleRight,
        const Card& top) {
        const float cx = peakCenterX[pileIndex];

        const int bottomLeftId = addCard(bottomLeft.rank, bottomLeft.suit, Zone::Table, pileIndex,
                                         cocos2d::Vec2(cx - offsetX, bottomY));
        const int bottomRightId = addCard(bottomRight.rank, bottomRight.suit, Zone::Table, pileIndex,
                                          cocos2d::Vec2(cx + offsetX, bottomY));
        const int middleLeftId = addCard(middleLeft.rank, middleLeft.suit, Zone::Table, pileIndex,
                                         cocos2d::Vec2(cx - offsetX, midY));
        const int middleRightId = addCard(middleRight.rank, middleRight.suit, Zone::Table, pileIndex,
                                          cocos2d::Vec2(cx + offsetX, midY));
        const int topId = addCard(top.rank, top.suit, Zone::Table, pileIndex, cocos2d::Vec2(cx, topY));
        const int tailOneId = addCard(tailOne.rank, tailOne.suit, Zone::Table, pileIndex,
                                      cocos2d::Vec2(cx, tailOneY));
        const int tailTwoId = addCard(tailTwo.rank, tailTwo.suit, Zone::Table, pileIndex,
                                      cocos2d::Vec2(cx, tailTwoY));

        mutableCardById(bottomLeftId)->blockers.push_back(middleLeftId);
        mutableCardById(bottomRightId)->blockers.push_back(middleRightId);
        mutableCardById(middleLeftId)->blockers.push_back(topId);
        mutableCardById(middleRightId)->blockers.push_back(topId);
        mutableCardById(tailOneId)->blockers.push_back(bottomLeftId);
        mutableCardById(tailOneId)->blockers.push_back(bottomRightId);
        mutableCardById(tailTwoId)->blockers.push_back(tailOneId);

        _tablePiles[pileIndex].push_back(tailTwoId);
        _tablePiles[pileIndex].push_back(tailOneId);
        _tablePiles[pileIndex].push_back(bottomLeftId);
        _tablePiles[pileIndex].push_back(bottomRightId);
        _tablePiles[pileIndex].push_back(middleLeftId);
        _tablePiles[pileIndex].push_back(middleRightId);
        _tablePiles[pileIndex].push_back(topId);
    };

    Card sevenTop; sevenTop.rank = 7; sevenTop.suit = Suit::Spade;
    Card sixMid; sixMid.rank = 6; sixMid.suit = Suit::Heart;
    Card fiveBottom; fiveBottom.rank = 5; fiveBottom.suit = Suit::Diamond;
    Card fourMid; fourMid.rank = 4; fourMid.suit = Suit::Club;
    Card threeBottom; threeBottom.rank = 3; threeBottom.suit = Suit::Diamond;
    Card aceTail; aceTail.rank = 1; aceTail.suit = Suit::Spade;
    Card twoTail; twoTail.rank = 2; twoTail.suit = Suit::Heart;
    addPeak(0, fiveBottom, threeBottom, twoTail, aceTail, sixMid, fourMid, sevenTop);

    Card nineTop; nineTop.rank = 9; nineTop.suit = Suit::Diamond;
    Card tenMid; tenMid.rank = 10; tenMid.suit = Suit::Diamond;
    Card jackBottom; jackBottom.rank = 11; jackBottom.suit = Suit::Heart;
    Card nineMid; nineMid.rank = 9; nineMid.suit = Suit::Heart;
    Card eightBottom; eightBottom.rank = 8; eightBottom.suit = Suit::Diamond;
    Card nineTail; nineTail.rank = 9; nineTail.suit = Suit::Heart;
    Card tenTail; tenTail.rank = 10; tenTail.suit = Suit::Club;
    addPeak(1, jackBottom, eightBottom, nineTail, tenTail, tenMid, nineMid, nineTop);

    Card queenTop; queenTop.rank = 12; queenTop.suit = Suit::Heart;
    Card kingMid; kingMid.rank = 13; kingMid.suit = Suit::Spade;
    Card queenBottom; queenBottom.rank = 12; queenBottom.suit = Suit::Club;
    Card jackMid; jackMid.rank = 11; jackMid.suit = Suit::Diamond;
    Card tenBottom; tenBottom.rank = 10; tenBottom.suit = Suit::Heart;
    Card nineTrapTail; nineTrapTail.rank = 9; nineTrapTail.suit = Suit::Spade;
    Card eightTail; eightTail.rank = 8; eightTail.suit = Suit::Spade;
    addPeak(2, queenBottom, tenBottom, nineTrapTail, eightTail, kingMid, jackMid, queenTop);

    Card stockBottom; stockBottom.rank = 3; stockBottom.suit = Suit::Heart;
    Card stockMiddle; stockMiddle.rank = 13; stockMiddle.suit = Suit::Club;
    Card stockTop; stockTop.rank = 5; stockTop.suit = Suit::Spade;
    _stockPile.push_back(addCard(stockBottom.rank, stockBottom.suit, Zone::Stock, 0, stockPosition));
    _stockPile.push_back(addCard(stockMiddle.rank, stockMiddle.suit, Zone::Stock, 0, stockPosition));
    _stockPile.push_back(addCard(stockTop.rank, stockTop.suit, Zone::Stock, 0, stockPosition));

    _wasteStack.push_back(addCard(8, Suit::Club, Zone::Waste, 0, kWastePosition));
    _initiallySolvable = hasWinningRoute();
}

const Card* GameState::cardById(int cardId) const {
    for (const auto& card : _cards) {
        if (card.id == cardId) {
            return &card;
        }
    }
    return nullptr;
}

Card* GameState::mutableCardById(int cardId) {
    for (auto& card : _cards) {
        if (card.id == cardId) {
            return &card;
        }
    }
    return nullptr;
}

const Card* GameState::wasteTop() const {
    return _wasteStack.empty() ? nullptr : cardById(_wasteStack.back());
}

int GameState::tableTopCard(int pileIndex) const {
    if (pileIndex < 0 || pileIndex >= static_cast<int>(_tablePiles.size())) {
        return -1;
    }

    for (auto it = _tablePiles[pileIndex].rbegin(); it != _tablePiles[pileIndex].rend(); ++it) {
        if (isFaceUpTableCard(*it)) {
            return *it;
        }
    }
    return -1;
}

int GameState::stockTopCard() const {
    return _stockPile.empty() ? -1 : _stockPile.back();
}

bool GameState::isVisibleTop(int cardId) const {
    const Card* card = cardById(cardId);
    if (!card) {
        return false;
    }
    if (card->zone == Zone::Waste) {
        return !_wasteStack.empty() && _wasteStack.back() == cardId;
    }
    if (card->zone == Zone::Stock) {
        return stockTopCard() == cardId;
    }
    return isFaceUpTableCard(cardId);
}

bool GameState::isFaceUpTableCard(int cardId) const {
    const Card* card = cardById(cardId);
    if (!card || card->zone != Zone::Table) {
        return false;
    }

    for (int blockerId : card->blockers) {
        const Card* blocker = cardById(blockerId);
        if (blocker && blocker->zone == Zone::Table) {
            return false;
        }
    }
    return true;
}

bool GameState::canMatch(int cardId) const {
    const Card* card = cardById(cardId);
    const Card* top = wasteTop();
    if (!card || !top || card->zone != Zone::Table || !isFaceUpTableCard(cardId)) {
        return false;
    }
    return std::abs(card->rank - top->rank) == 1;
}

bool GameState::areTablePilesEmpty() const {
    for (const auto& pile : _tablePiles) {
        if (!pile.empty()) {
            return false;
        }
    }
    return true;
}

bool GameState::hasAnyTableMatch() const {
    for (const auto& pile : _tablePiles) {
        for (int cardId : pile) {
            if (canMatch(cardId)) {
                return true;
            }
        }
    }
    return false;
}

GameResult GameState::result() const {
    if (areTablePilesEmpty()) {
        return GameResult::Success;
    }
    if (_initiallySolvable && _stockPile.empty() && !hasAnyTableMatch()) {
        return GameResult::Failed;
    }
    return GameResult::Playing;
}

bool GameState::moveTableTopToWaste(int cardId, MoveRecord* outRecord) {
    if (!canMatch(cardId)) {
        return false;
    }
    return moveToWaste(cardId, MoveType::TableToWaste, outRecord);
}

bool GameState::moveStockTopToWaste(int cardId, MoveRecord* outRecord) {
    if (stockTopCard() != cardId) {
        return false;
    }
    return moveToWaste(cardId, MoveType::StockToWaste, outRecord);
}

bool GameState::undoLast(MoveRecord* outRecord) {
    if (_undoStack.empty()) {
        return false;
    }

    MoveRecord record = _undoStack.back();
    _undoStack.pop_back();

    if (!_wasteStack.empty() && _wasteStack.back() == record.cardId) {
        _wasteStack.pop_back();
    }

    Card* card = mutableCardById(record.cardId);
    if (!card) {
        return false;
    }

    auto* pile = pileFor(record.fromZone, record.fromPileIndex);
    if (!pile) {
        return false;
    }

    card->zone = record.fromZone;
    card->pileIndex = record.fromPileIndex;
    card->position = record.fromPosition;
    pile->push_back(record.cardId);

    if (outRecord) {
        *outRecord = record;
    }
    return true;
}

int GameState::addCard(int rank, Suit suit, Zone zone, int pileIndex, const cocos2d::Vec2& position) {
    Card card;
    card.id = static_cast<int>(_cards.size());
    card.rank = rank;
    card.suit = suit;
    card.zone = zone;
    card.pileIndex = pileIndex;
    card.position = position;
    _cards.push_back(card);
    return card.id;
}

bool GameState::moveToWaste(int cardId, MoveType type, MoveRecord* outRecord) {
    Card* card = mutableCardById(cardId);
    if (!card) {
        return false;
    }

    auto* sourcePile = pileFor(card->zone, card->pileIndex);
    if (!sourcePile) {
        return false;
    }

    if (card->zone == Zone::Stock) {
        if (sourcePile->empty() || sourcePile->back() != cardId) {
            return false;
        }
        sourcePile->pop_back();
    } else {
        auto it = std::find(sourcePile->begin(), sourcePile->end(), cardId);
        if (it == sourcePile->end()) {
            return false;
        }
        sourcePile->erase(it);
    }

    MoveRecord record;
    record.type = type;
    record.cardId = cardId;
    record.fromZone = card->zone;
    record.fromPileIndex = card->pileIndex;
    record.fromPosition = card->position;

    card->zone = Zone::Waste;
    card->pileIndex = 0;
    card->position = kWastePosition;
    _wasteStack.push_back(cardId);
    _undoStack.push_back(record);

    if (outRecord) {
        *outRecord = record;
    }
    return true;
}

std::vector<int>* GameState::pileFor(Zone zone, int pileIndex) {
    if (zone == Zone::Stock) {
        return &_stockPile;
    }
    if (zone == Zone::Table && pileIndex >= 0 && pileIndex < static_cast<int>(_tablePiles.size())) {
        return &_tablePiles[pileIndex];
    }
    return nullptr;
}

const std::vector<int>* GameState::pileFor(Zone zone, int pileIndex) const {
    if (zone == Zone::Stock) {
        return &_stockPile;
    }
    if (zone == Zone::Table && pileIndex >= 0 && pileIndex < static_cast<int>(_tablePiles.size())) {
        return &_tablePiles[pileIndex];
    }
    return nullptr;
}

bool GameState::hasWinningRoute() const {
    const Card* top = wasteTop();
    if (!top) {
        return false;
    }

    SolverState state;
    state.tablePiles = _tablePiles;
    state.stockPile = _stockPile;
    state.wasteRank = top->rank;
    return solve(state);
}

bool GameState::solve(SolverState state) const {
    if (solverTableEmpty(state)) {
        return true;
    }

    for (int i = 0; i < static_cast<int>(state.tablePiles.size()); ++i) {
        for (int cardId : state.tablePiles[i]) {
            if (!solverIsFaceUp(state, cardId) || !solverCanMatch(cardId, state.wasteRank)) {
                continue;
            }

            SolverState next = state;
            auto& pile = next.tablePiles[i];
            pile.erase(std::remove(pile.begin(), pile.end(), cardId), pile.end());
            next.wasteRank = cardById(cardId)->rank;
            if (solve(next)) {
                return true;
            }
        }
    }

    if (!state.stockPile.empty()) {
        const int cardId = state.stockPile.back();
        state.stockPile.pop_back();
        state.wasteRank = cardById(cardId)->rank;
        if (solve(state)) {
            return true;
        }
    }

    return false;
}

bool GameState::solverTableEmpty(const SolverState& state) const {
    for (const auto& pile : state.tablePiles) {
        if (!pile.empty()) {
            return false;
        }
    }
    return true;
}

bool GameState::solverCanMatch(int cardId, int wasteRank) const {
    const Card* card = cardById(cardId);
    return card && std::abs(card->rank - wasteRank) == 1;
}

bool GameState::solverHasCard(const SolverState& state, int cardId) const {
    for (const auto& pile : state.tablePiles) {
        if (std::find(pile.begin(), pile.end(), cardId) != pile.end()) {
            return true;
        }
    }
    return false;
}

bool GameState::solverIsFaceUp(const SolverState& state, int cardId) const {
    const Card* card = cardById(cardId);
    if (!card) {
        return false;
    }

    for (int blockerId : card->blockers) {
        if (solverHasCard(state, blockerId)) {
            return false;
        }
    }
    return solverHasCard(state, cardId);
}

}  // namespace cardgame
