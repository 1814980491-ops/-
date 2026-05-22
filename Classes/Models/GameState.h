#ifndef CARD_PILE_MATCH_MODELS_GAME_STATE_H_
#define CARD_PILE_MATCH_MODELS_GAME_STATE_H_

#include "Models/Card.h"

#include <vector>

namespace cardgame {

enum class MoveType {
    TableToWaste,
    StockToWaste
};

enum class GameResult {
    Playing,
    Success,
    Failed
};

struct MoveRecord {
    MoveType type = MoveType::TableToWaste;
    int cardId = -1;
    Zone fromZone = Zone::Table;
    int fromPileIndex = -1;
    cocos2d::Vec2 fromPosition;
};

class GameState {
public:
    void setupDemo();

    const std::vector<Card>& cards() const { return _cards; }
    const std::vector<int>& wasteStack() const { return _wasteStack; }
    const std::vector<std::vector<int>>& tablePiles() const { return _tablePiles; }
    const std::vector<int>& stockPile() const { return _stockPile; }

    const Card* cardById(int cardId) const;
    Card* mutableCardById(int cardId);
    const Card* wasteTop() const;
    int tableTopCard(int pileIndex) const;
    int stockTopCard() const;

    bool isVisibleTop(int cardId) const;
    bool isFaceUpTableCard(int cardId) const;
    bool canMatch(int cardId) const;
    bool moveTableTopToWaste(int cardId, MoveRecord* outRecord);
    bool moveStockTopToWaste(int cardId, MoveRecord* outRecord);
    bool undoLast(MoveRecord* outRecord);
    bool hasUndo() const { return !_undoStack.empty(); }
    bool isInitiallySolvable() const { return _initiallySolvable; }
    bool areTablePilesEmpty() const;
    bool hasAnyTableMatch() const;
    GameResult result() const;

private:
    struct SolverState {
        std::vector<std::vector<int>> tablePiles;
        std::vector<int> stockPile;
        int wasteRank = 0;
    };

    int addCard(int rank, Suit suit, Zone zone, int pileIndex, const cocos2d::Vec2& position);
    bool moveToWaste(int cardId, MoveType type, MoveRecord* outRecord);
    std::vector<int>* pileFor(Zone zone, int pileIndex);
    const std::vector<int>* pileFor(Zone zone, int pileIndex) const;
    bool hasWinningRoute() const;
    bool solve(SolverState state) const;
    bool solverTableEmpty(const SolverState& state) const;
    bool solverCanMatch(int cardId, int wasteRank) const;
    bool solverHasCard(const SolverState& state, int cardId) const;
    bool solverIsFaceUp(const SolverState& state, int cardId) const;

    std::vector<Card> _cards;
    std::vector<std::vector<int>> _tablePiles;
    std::vector<int> _stockPile;
    std::vector<int> _wasteStack;
    std::vector<MoveRecord> _undoStack;
    bool _initiallySolvable = false;
};

}  // namespace cardgame

#endif
