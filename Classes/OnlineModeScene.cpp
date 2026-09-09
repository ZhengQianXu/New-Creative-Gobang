#include "OnlineModeScene.h"
#include "SimpleAudioEngine.h"
#include "BoardUI.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* OnlineMode::createScene() {
    return OnlineMode::create();
}

bool OnlineMode::init() {
    if (!Scene::init())
        return false;

    //创建游戏界面ui
    auto bu = BoardUI::create();
    this->addChild(bu, 0);

    //实现返回按钮处理
    bu->setOnReturnCallBack([]() {
        Director::getInstance()->popScene();
    });

    //实现音效开关切换
    bu->setOnToggleEffectCallBack([=]() {
        isEffectOn = !isEffectOn;
    });

    //bu->setOnTouchBeganCallBack(CC_CALLBACK_2(OnlineMode::onTouchBegan, this));

    //传入开始游戏处理回调
    bu->setOnStartGameCallBack(CC_CALLBACK_1(OnlineMode::onStartGame, this));

    //传入棋盘清理处理回调
    bu->setOnCleanBoardCallBack(CC_CALLBACK_1(OnlineMode::cleanBoard, this));

    //从ui类获取需要被操作的ui控件
    bu->getBoardUImember(chessSprites, selectedHighlight, startGameBtn, timer, blackRoundArrow, whiteRoundArrow, gameOverTip,
        victoryAnimation, drawAnimation, gameOverBtn, gameOverDrawBtn);

    auto origin = Director::getInstance()->getVisibleOrigin();
    bu->getGameOverUI()->setPosition(origin.x, origin.y + 84.0f);    //该ui组本来在正中间，往上提了点
    this->addChild(bu->getGameOverUI(), 2);                          //ui组单独拿出来的目的，为了显示在最高层级

    //创建游戏逻辑对象，并传入需要用到的ui控件
    gl = new GameLogic(this, isEffectOn, isGamePlaying, isBlackRound, roundSurplusTime, selectedChessName, selectedHighlight,
        selectedPlacePoint, chessSprites, placePoints, boardChesses, curChessSum);

    /*std::function<void(bool)> _gameOver = std::bind(&OnlineMode::gameOver, this, std::placeholders::_1);
    gl->setGameOverFunction(_gameOver);*/

    //调用场景实现的游戏结算处理函数
    gl->setGameOverFunction([this](bool isDraw) {
        this->gameOver(isDraw);
    });

    //传入触摸处理函数
    bu->setOnTouchBeganCallBack([=](Touch* touch, Event* event) {
        return gl->onTouchBegan(touch, event);
    });

    return true;
}

void OnlineMode::onStartGame(Ref* pSender) {
    startGameBtn->setVisible(false);                //按钮隐藏，代表已开始游戏

    isGamePlaying = true;                           //表示正在游戏中
    roundSurplusTime = 20.9f;                       //回合时间20秒左右
    isBlackRound = true;                            //默认第一回合是黑方
    blackRoundArrow->setVisible(true);
    whiteRoundArrow->setVisible(false);             //指向白方的箭头先隐藏
    timer->setVisible(true);                        //显示计时器

    this->scheduleUpdate();                         //启动帧循环，每帧自动调用 update(float dt)

    if (placePoints.empty())
        gl->onInitBoardPlacePoint();                //第一回合开始时初始化棋盘放置点

    selectedHighlight->setVisible(true);            //显示高亮
    gl->onSelectChess(chessSprites[0], "black_zhe");//自动选中第一个黑棋
}

void OnlineMode::update(float dt) {
    if (!isGamePlaying)
        return;                                         //不在游戏中不处理更新逻辑

    roundSurplusTime -= dt;                             //更新当前回合剩余时间
    if (roundSurplusTime <= 0) {                        //回合时间到
        if (lastChessSum == curChessSum) {              //如果玩家未落子
            if (selectedPlacePoint)                     //如果有选中放置点，自动落子
                gl->onPlaceChess(selectedPlacePoint->getPosition());
            else
                for (int row = 0; row < 19 && lastChessSum == curChessSum; row++)    //lastChessSum == curChessSum保证只落一个子
                    for (int col = 0; col < 19; col++)
                        if (boardChesses[row][col] == nullptr) {                    //没有选中放置点，找到第一个可放置点，帮忙落子
                            gl->onPlaceChess(placePoints[row][col]->getPosition()); //第一步是先选放置点
                            gl->onPlaceChess(placePoints[row][col]->getPosition()); //第二步才正式落子
                            break;                                                  //直接退出，保证只落一个子
                        }
        }
        if (!isGamePlaying)
            return;                                         //如果系统帮忙落子恰好有一方获胜，直接退出，无需后续逻辑        
        if (curChessSum == 19 * 19) {
            gameOver(true);                                 //如果棋盘放满都没胜负，则为平局
            return;
        }
        lastChessSum = curChessSum;                         //更新棋盘旧棋子总数
        roundSurplusTime = 20.9f;                           //回合结束，开始下一回合
        isBlackRound = !isBlackRound;                       //回合交换       
        if (isBlackRound) {                                 //如果黑方回合
            timer->setTextColor(Color4B::BLACK);            //黑方回合计时器是黑色的
            blackRoundArrow->setVisible(true);              //指向黑方的箭头显示
            whiteRoundArrow->setVisible(false);             //指向白方的箭头隐藏
            gl->onSelectChess(chessSprites[0], "black_zhe");//自动选中第一个黑方棋子
        }
        else {                                              //反之
            timer->setTextColor(Color4B::WHITE);
            whiteRoundArrow->setVisible(true);
            blackRoundArrow->setVisible(false);
            gl->onSelectChess(chessSprites[5], "white_zhe");//自动选中第一个白方棋子
        }
    }
    else if (roundSurplusTime < 6)
        timer->setTextColor(Color4B::RED);                  //倒计时剩5秒时呈红色

    int seconds = (int)std::floor(roundSurplusTime);        //倒计时向下取整，可以确保看得到0
    timer->setString(std::to_string(seconds));              //实时显示在计时器标签上
}

void OnlineMode::gameOver(bool isDraw) {
    if (isDraw) {
        gameOverTip->setString(u8"双方打平!");
        drawAnimation->setVisible(true);        //显示平局动画
        gameOverDrawBtn->setVisible(true);      //显示平局情况下游戏结束按钮

        if (isEffectOn)
            SimpleAudioEngine::getInstance()->playEffect("music/defeat.mp3");   //播放平局音效
    }
    else {
        //根据获胜方的不同，提示文本也不同
        if (isBlackRound)
            gameOverTip->setString(u8"黑方获胜!");
        else
            gameOverTip->setString(u8"白方获胜!");

        victoryAnimation->setVisible(true);     //显示获胜动画
        gameOverBtn->setVisible(true);          //显示结束游戏按钮

        if (isEffectOn)
            SimpleAudioEngine::getInstance()->playEffect("music/victory.mp3");  //播放获胜音效
    }

    isGamePlaying = false;                      //游戏结束
    timer->setVisible(false);                   //计时器隐藏
    blackRoundArrow->setVisible(false);         //黑方箭头隐藏
    whiteRoundArrow->setVisible(false);         //白方箭头隐藏
    selectedHighlight->setVisible(false);       //隐藏选中高亮
    selectedChessName = "";                     //选中棋子名字重置
    selectedPlacePoint = nullptr;               //选中放置点重置
    gameOverTip->setVisible(true);              //结算提示隐藏
}

void OnlineMode::cleanBoard(Ref* pSender) {
    lastChessSum = 0; curChessSum = 0;                          //棋盘上棋子总数清零
    for (int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++)
            if (boardChesses[row][col]) {
                boardChesses[row][col]->removeFromParent();     //删除所有保存的棋子
                boardChesses[row][col] = nullptr;
            }

    //隐藏游戏结算相关ui
    gameOverTip->setVisible(false);
    victoryAnimation->setVisible(false);                        //隐藏获胜动画
    drawAnimation->setVisible(false);
    gameOverBtn->setVisible(false);                             //隐藏结束游戏按钮
    gameOverDrawBtn->setVisible(false);

    startGameBtn->setVisible(true);                             //显示开始游戏按钮，为下一次游戏做准备
}