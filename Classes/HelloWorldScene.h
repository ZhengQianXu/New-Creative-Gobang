/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

//定义方向类型
struct Direction {
    int x;      //横坐标
    int y;      //纵坐标
};

constexpr Direction Up = { 1, 0 };          //上
constexpr Direction LeftUp = { 1,-1 };      //左上
constexpr Direction Left = { 0, -1 };       //左
constexpr Direction LeftDown = { -1, -1 };  //左下
constexpr Direction Down = { -1,0 };        //下
constexpr Direction RightDown = { -1,1 };   //右下
constexpr Direction Right = { 0,1 };        //右
constexpr Direction RightUp = { 1,1 };      //右上

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
    void toggleBGM(cocos2d::Ref* pSender);

	//获取背景音乐播放状态
    static bool isBgmPlay() { return isBgmOn; }

	//开始旋转按钮
    void startRotate();
    
	//停止旋转按钮
	void stopRotate();

    //改变音效开关状态
    void toggleEffect(cocos2d::Ref* pSender);

    //点击开始回调
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    //初始化棋盘可放置各点
    void onInitBoardPlacePoint();

    //棋子选中处理
    void onSelectChess(cocos2d::Sprite* chessSprite, const std::string& chessName);

    //棋子放置处理
    bool onPlaceChess(cocos2d::Vec2 touchPos);

    //开始游戏处理
    void onStartGame(cocos2d::Ref* pSender);

    //每帧更新函数（由 scheduleUpdate() 触发），dt 两帧之间的时间间隔，通常约为 1/60 秒
    void update(float dt);

    //判赢处理
    bool isVictory(int row, int col);

    //从当前落子处辐射搜索
    bool searchBoardChesses(int row, int col, Direction dir_1, Direction dir_2, std::unordered_map<std::string, int> needChesses);

    //游戏结束处理
    void gameOver();

    //清空棋盘
    void cleanBoard(cocos2d::Ref* pSender);

    //规则内容显示
    void onRuleShow(cocos2d::Ref* pSender);

    //反馈途径显示
    void onSuggestShow(cocos2d::Ref* pSender);

    //关闭弹窗
    void closePopup(cocos2d::Ref* pSender);
    
    //创建弹窗
    void createPopup(const std::string& title, const std::string& content);

private:
    cocos2d::Size visibleSize;                              //窗口大小
    cocos2d::Vec2 origin;                                   //坐标原点

	cocos2d::MenuItemImage* bgmBtn = nullptr;               //背景音乐控制按钮
	cocos2d::Action* rotateAction = nullptr;                //旋转动作
	static bool isBgmOn;                                    //背景音乐播放状态
    bool isEffectOn = true;                                 //音效开关状态

    std::vector<cocos2d::Sprite*> chessSprites;             //棋子数组
    cocos2d::Sprite* selectedHighlight = nullptr;           //选中高亮效果
    std::string selectedChessName = "";                     //选中棋子名字
    cocos2d::Sprite* selectedPlacePoint = nullptr;          //选中放置点
    std::vector<std::vector<cocos2d::Sprite*>> placePoints; //棋盘所有可放置点数组
    std::vector<std::vector<bool>> canPlace;                //当前棋盘可放置点

    bool isGamePlaying = false;                             //是否正在游戏中
    float roundSurplusTime = 20.9f;                         //回合剩余时间，这里划定一个回合时间为20秒左右
    bool isBlackRound = true;                               //是否是黑方回合
    cocos2d::Label* timer = nullptr;                        //计时器显示标签
    cocos2d::Sprite* blackRoundArrow = nullptr;             //指向黑方的箭头
    cocos2d::Sprite* whiteRoundArrow = nullptr;             //指向白方的箭头
    int lastChessSum = 0;                                   //上一回合棋盘上棋子总数
    int curChessSum = 0;                                    //当前回合棋盘上棋子总数

    std::vector<std::vector<cocos2d::Sprite*>> boardChesses;//存放棋盘上棋子数组

    cocos2d::MenuItemImage* startGameBtn = nullptr;         //开始游戏按钮
    cocos2d::Label* victoryTip = nullptr;                   //获胜方提示
    cocos2d::Sprite* victoryAnimation = nullptr;            //获胜动画展示
    cocos2d::MenuItemImage* gameOverBtn = nullptr;          //游戏结束按钮

    cocos2d::LayerColor* popupMask = nullptr;               //遮罩层
    cocos2d::ui::Layout* popup = nullptr;                   //弹窗
};

#endif // __HELLOWORLD_SCENE_H__
