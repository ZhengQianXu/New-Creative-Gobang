#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <chrono>

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

    //点击开始函数
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    
    //点击结束函数
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    //初始化棋盘可放置各点
    void onInitBoardPlacePoint();

    //棋子选中处理函数
    void onSelectChess(cocos2d::Sprite* chessSprite, const std::string& chessName);

    //棋子放置处理函数
    bool onPlaceChess(cocos2d::Vec2 touchPos);

    //开始游戏处理函数
    void onStartGame(Ref* pSender);

    /*  每帧更新函数（由 scheduleUpdate() 触发）
        @param dt 两帧之间的时间间隔（单位：秒），通常约为 1/60 秒  */
    void update(float dt);

private:
    cocos2d::Size visibleSize;                              //窗口大小
    cocos2d::Vec2 origin;                                   //坐标原点

	cocos2d::MenuItemImage* bgmBtn = nullptr;               //背景音乐控制按钮
	cocos2d::Action* rotateAction = nullptr;                //旋转动作
	static bool isBgmOn;                                    //背景音乐播放状态

    std::vector<cocos2d::Sprite*> chessSprites;             //棋子数组
    cocos2d::Sprite* selectedHighlight = nullptr;           //选中高亮效果
    std::string selectedChessName = "";                     //选中棋子名字
    cocos2d::Sprite* selectedPlacePoint = nullptr;          //选中放置点
    std::vector<std::vector<cocos2d::Sprite*>> placePoints; //棋盘所有可放置点数组
    std::vector<std::vector<bool>> canPlace;                //当前棋盘可放置点

    bool isGamePlaying = false;                             //是否正在游戏中
    float roundSurplusTime = 5.9f;                          //回合剩余时间，这里划定一个回合时间为20秒左右
    bool isBlackRound = true;                               //是否是黑方回合
    cocos2d::Label* timer = nullptr;                        //计时器显示标签
    cocos2d::Sprite* blackRoundArrow = nullptr;             //指向黑方的箭头
    cocos2d::Sprite* whiteRoundArrow = nullptr;             //指向白方的箭头
    int lastChessSum = 0;                                   //上一回合棋盘上棋子总数
    int curChessSum = 0;                                    //当前回合棋盘上棋子总数
};

#endif // __HELLOWORLD_SCENE_H__
