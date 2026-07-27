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

	//播放背景音乐
    void playBGM();

	//改变背景音乐播放状态
    void toggleBGM(Ref* pSender);

	//开始旋转按钮
    void startRotate();
    
	//停止旋转按钮
	void stopRotate();

private:
	cocos2d::MenuItemImage* bgmBtn = nullptr;   //背景音乐控制按钮
	cocos2d::Action* rotateAction = nullptr;    //旋转动作
	bool isBgmOn = true;                        //背景音乐播放状态，默认播放
};

#endif // __HELLOWORLD_SCENE_H__
