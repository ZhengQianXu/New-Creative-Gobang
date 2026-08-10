#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	//改变背景音乐播放状态
    void toggleBGM(Ref* pSender);

	//获取背景音乐播放状态
    static bool isBgmPlay() { return isBgmOn; }

	//开始旋转按钮
    void startRotate();
    
	//停止旋转按钮
	void stopRotate();

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void onSelectChess(cocos2d::Sprite* chessSprite, const std::string& chessName);

    void onShowBoardPlacePoint();

private:
	cocos2d::MenuItemImage* bgmBtn = nullptr;   //背景音乐控制按钮
	cocos2d::Action* rotateAction = nullptr;    //旋转动作
	static bool isBgmOn;                        //背景音乐播放状态

    std::vector<cocos2d::Sprite*> chessSprites;
    cocos2d::Sprite* selectedHighlight = nullptr;
    std::string selectedChess = "";
};

#endif // __HELLOWORLD_SCENE_H__
