#ifndef CARD_PILE_MATCH_MAIN_SCENE_H_
#define CARD_PILE_MATCH_MAIN_SCENE_H_

#include "Models/GameState.h"
#include "Views/CardView.h"
#include "cocos2d.h"

#include <unordered_map>

namespace cardgame {

class MainScene : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(MainScene);

private:
    void buildBoard();
    void buildCards();
    void bindTouch(CardView* view);
    void onCardTapped(int cardId);
    void onUndo(cocos2d::Ref* sender);
    void animateToWaste(const MoveRecord& record);
    void animateUndo(const MoveRecord& record);
    int frontCardAtPoint(const cocos2d::Vec2& worldPoint) const;
    void refreshVisibility();
    void refreshResult();
    void preloadSounds();
    void playSound(const char* filePath);
    cocos2d::Vec2 wastePosition() const;
    cocos2d::Vec2 stockPosition() const;

    GameState _state;
    std::unordered_map<int, CardView*> _views;
    cocos2d::Label* _resultLabel = nullptr;
    GameResult _lastResult = GameResult::Playing;
    bool _busy = false;
};

}  // namespace cardgame

#endif
