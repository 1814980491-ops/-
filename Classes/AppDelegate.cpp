#include "AppDelegate.h"

#include "MainScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() = default;
AppDelegate::~AppDelegate() = default;

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs attrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(attrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::createWithRect("CardPileMatch", Rect(0, 0, 1080, 2080), 0.5f);
        director->setOpenGLView(glview);
    }

    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 60.0f);
    director->runWithScene(cardgame::MainScene::createScene());
    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
