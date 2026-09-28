/*
	LocalMode类是一个继承自cocos2d::Scene的类，主要用于实现本地对战模式下的游戏场景。它包含了游戏逻辑、用户交互、棋盘管理以及游戏状态
	控制等功能。在主界面点击“本地对战模式”按钮后，会创建并切换到该场景，玩家在该场景中进行本地对战游戏。
*/

#ifndef __LOCALMODE_SCENE_H__
#define __LOCALMODE_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "GameLogic.h"

class LocalMode : public cocos2d::Scene{
public:
    //创建场景
    static cocos2d::Scene* createScene();

    //初始化函数
    virtual bool init();
    
    //创建静态方法 create()
    CREATE_FUNC(LocalMode);

    ~LocalMode() { if (gl) delete gl; }

private:
    //触摸事件回调
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    //开始游戏处理
    void onStartGame(cocos2d::Ref* pSender);

    //棋子放置处理，rowCol包含两个元素,[0]=row,[1]=col
    bool onPlaceChess(std::vector<int> rowCol);

    //切换回合处理
    void switchRound();

    //系统自动落子处理
    void autoPlaceChess();

    //每帧更新函数（由 scheduleUpdate() 触发），dt 两帧之间的时间间隔，通常约为 1/60 秒
    void update(float dt);   

    //游戏结束处理，分平局和非平局，isDraw = true表示平局
    void gameOver(bool isDraw);

    //清空棋盘
    void cleanBoard(cocos2d::Ref* pSender);    

private:
    GameLogic* gl = nullptr;                                    //游戏逻辑指针，用于调用逻辑处理函数

    bool isEffectOn = true;                                     //音效开关状态

    std::vector<cocos2d::Sprite*> chessSprites;                 //棋子数组
    std::string selectedChessName = "";                         //选中棋子名字
    cocos2d::Sprite* selectedHighlight = nullptr;               //选中高亮效果    
    cocos2d::Sprite* selectedPlacePoint = nullptr;              //选中放置点
    std::vector<int> selectedRowCol = { 0,0 };                  //选中行列

    std::vector<std::vector<cocos2d::Sprite*>> placePoints;     //棋盘所有可放置点数组
    std::vector<std::vector<cocos2d::Sprite*>> boardChesses;    //存放棋盘上棋子数组

    cocos2d::MenuItemImage* startGameBtn = nullptr;             //开始游戏按钮
    bool isGamePlaying = false;                                 //是否正在游戏中
    float roundSurplusTime = 20.9f;                             //回合剩余时间，这里划定一个回合时间为20秒左右
    bool isBlackRound = true;                                   //是否是黑方回合
    cocos2d::Label* timer = nullptr;                            //计时器显示标签
    cocos2d::Sprite* blackRoundArrow = nullptr;                 //指向黑方的箭头
    cocos2d::Sprite* whiteRoundArrow = nullptr;                 //指向白方的箭头
    int lastChessSum = 0;                                       //上一回合棋盘上棋子总数
    int curChessSum = 0;                                        //当前回合棋盘上棋子总数
   
    cocos2d::Label* gameOverTip = nullptr;                      //游戏结算提示
    cocos2d::Sprite* victoryAnimation = nullptr;                //获胜动画展示
    cocos2d::Sprite* defeatAnimation = nullptr;				    //失败动画展示
    cocos2d::Sprite* drawAnimation = nullptr;				    //平局结算动画
    cocos2d::MenuItemImage* gameOverVictoryBtn = nullptr;       //获胜结束按钮
    cocos2d::MenuItemImage* gameOverDefeatBtn = nullptr;	    //失败结束按钮
    cocos2d::MenuItemImage* gameOverDrawBtn = nullptr;		    //平局结束按钮
};

#endif // __LOCALMODE_SCENE_H__