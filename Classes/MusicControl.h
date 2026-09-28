/*
	MusicControl类是一个ui节点和操作类，用于控制游戏中的背景音乐播放状态。它继承自cocos2d::Node类，提供了初始化、创建实例、切换背景音
    乐播放状态、获取播放状态、开始和停止按钮旋转等功能。此外，它还重写了onEnter方法，这里主要是为了保持每个场景的背景音乐播放状态一致。
	该类包含一个用于控制背景音乐的按钮和一个旋转动作，并使用静态变量来跟踪背景音乐的播放状态。外部通过添加该类的实例到场景中即可实现
	背景音乐的控制功能，每个场景的背景音乐播放按钮是独立的，但背景音乐播放状态是全局共享的。
*/

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

    //当该组件显示时调用
    virtual void onEnter() override;

private:
    cocos2d::MenuItemImage* bgmBtn = nullptr;       //背景音乐控制按钮
    cocos2d::Action* rotateAction = nullptr;        //旋转动作
    static bool isBgmOn;                            //背景音乐播放状态
};

#endif // __MusicControl_H__