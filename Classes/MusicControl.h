#ifndef __MusicControl_H__
#define __MusicControl_H__

#include "cocos2d.h"


class MusicControl : public cocos2d::Node {
public:
    //初始化函数
    virtual bool init() override;

    //工厂方法，创建实例
    static MusicControl* create();

    //改变背景音乐播放状态
    void toggleBGM(cocos2d::Ref* pSender);

    //获取背景音乐播放状态
    static bool isBgmPlay() { return isBgmOn; }

    //开始旋转按钮
    void startRotate();

    //停止旋转按钮
    void stopRotate();

private:
    cocos2d::MenuItemImage* bgmBtn = nullptr;       //背景音乐控制按钮
    cocos2d::Action* rotateAction = nullptr;        //旋转动作
    static bool isBgmOn;                            //背景音乐播放状态
};

#endif // __MusicControl_H__