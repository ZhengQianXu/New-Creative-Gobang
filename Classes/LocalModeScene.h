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

private:    
    //开始游戏处理
    void onStartGame(cocos2d::Ref* pSender);

    //每帧更新函数（由 scheduleUpdate() 触发），dt 两帧之间的时间间隔，通常约为 1/60 秒
    void update(float dt);   

    //游戏结束处理，分平局和非平局，isDraw = true表示平局
    void gameOver(bool isDraw);

    //清空棋盘
    void cleanBoard(cocos2d::Ref* pSender);    

    ~LocalMode() { if(gl) delete gl; };
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
    cocos2d::Sprite* drawAnimation = nullptr;                   //平局结算动画
    cocos2d::MenuItemImage* gameOverBtn = nullptr;              //游戏结束按钮
    cocos2d::MenuItemImage* gameOverDrawBtn = nullptr;          //平局结束按钮
};

#endif // __LOCALMODE_SCENE_H__