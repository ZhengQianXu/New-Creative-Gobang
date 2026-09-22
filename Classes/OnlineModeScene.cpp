#include "OnlineModeScene.h"
#include "SimpleAudioEngine.h"
#include "BoardUI.h"
#include "NetworkManager.h"

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
        NetworkManager::getInstance()->disconnect();
        Director::getInstance()->popScene();
    });

    //实现音效开关切换
    bu->setOnToggleEffectCallBack([=]() {
        isEffectOn = !isEffectOn;
    });

    //传入触摸处理回调
    bu->setOnTouchBeganCallBack(CC_CALLBACK_2(OnlineMode::onTouchBegan, this));

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
        selectedPlacePoint, selectedRowCol, chessSprites, placePoints, boardChesses, curChessSum);

    //调用场景实现的游戏结算处理函数
    gl->setGameOverFunction([this](bool isDraw) {
        this->gameOver(isDraw);
    });

    //获取网络管理模块单例
    auto nm = NetworkManager::getInstance();

    //传入开始游戏回调
    nm->setOnStartGameCallBack([=](bool ok) {
        canStartGame = ok;                              //可以开始游戏
        onStartGame(startGameBtn);                      //再次调用开始游戏按钮回调
    });

    //传入当前角色设置回调
    nm->setOnCurRoleCallBack([=](bool curRole) {
        isBlackRole = curRole;
    });

    //传入回合时间更新回调
    nm->setOnUpdateTimeCallBack([=](float surplusTime) {
        roundSurplusTime = surplusTime;
        int seconds = (int)std::floor(roundSurplusTime);//倒计时向下取整，可以确保看得到0
        timer->setString(std::to_string(seconds));      //实时显示在计时器标签上
        if (roundSurplusTime <= 0)
            autoPlaceChess();
        else if (roundSurplusTime < 6)
            timer->setTextColor(Color4B::RED);          //倒计时剩5秒时呈红色
    });

    //传入对手落子回调
    nm->setOnOpponentMoveCallBack([=](int row, int col, const std::string chessName) {       
        selectedChessName = chessName;                  //先设置棋子名字
        gl->onPlaceChess({ row, col });                 //再进行落子       
        switchRound();                                  //落子完成回合切换        
    });

    //传入退出房间处理回调，另一方退出房间时调用
    nm->setOnQuitRoomCallBack([]() {
        NetworkManager::getInstance()->disconnect();
        Director::getInstance()->popScene();
    });

    return true;
}

bool OnlineMode::onTouchBegan(Touch* touch, Event* event) {
    if (!isGamePlaying)
        return false;                                           //没在游戏中就不响应点击
    
    Vec2 touchPos = touch->getLocation();                       //获取点击的位置
    for (auto& chess : chessSprites)
        if (chess->getBoundingBox().containsPoint(touchPos)) {  //判断是否有棋子被点击了
            bool myTurn = ((isBlackRole && isBlackRound) || (!isBlackRole && !isBlackRound));
            if (!myTurn)
                return false;                                   //不是自己回合不能选择棋子
            gl->onSelectChess(chess, chess->getName());         //调用选中棋子函数
            selectedHighlight->setVisible(true);
            return true;                                        //消费掉这个点击事件
        }
    
    //如果有棋子被选中，而点击的位置不是棋子，可能在棋盘上，但是只有在自己回合才能对棋盘进行操作
    if (!selectedChessName.empty() && ((isBlackRole && isBlackRound) || (!isBlackRole && !isBlackRound))) {
        //如果已选中放置点，且再次点击放置点，则落子
        if (selectedPlacePoint && selectedPlacePoint->getBoundingBox().containsPoint(touchPos)) {
            std::string row = std::to_string(selectedRowCol[0]), col = std::to_string(selectedRowCol[1]);   //转换行列数据类型
            NetworkManager::getInstance()->sendMsg("move:" + row + ',' + col + ',' + selectedChessName);    //发送落子信息
            bool ok = gl->onPlaceChess(selectedRowCol);         //先保存落子反馈
            switchRound();                                      //落子完成回合切换
            return ok;
        }
        else
            return gl->onSelectPlacePoint(touchPos);            //否则进入选中放置点逻辑
    }
    return false;
}

void OnlineMode::onStartGame(Ref* pSender) {
    startGameBtn->setVisible(false);                //按钮隐藏，代表已开始游戏
    if (!canStartGame)
        timer->setString(u8"等待对方准备中...");
    else
        timer->setString(u8"游戏开始!");
    timer->setVisible(true);                        //显示计时器

    //点击开始游戏按钮时，还不能开始游戏，因为需要双方准备，向服务器发送准备好的信息，等待服务器回消息触发回调，才执行后续操作
    if (!canStartGame) {
        NetworkManager::getInstance()->sendMsg("ready:ok");
        return;
    }  
    
    isGamePlaying = true;                           //表示正在游戏中
    isBlackRound = true;                            //默认第一回合是黑方
    blackRoundArrow->setVisible(true);              //指向黑方的箭头显示

    if (placePoints.empty())
        gl->onInitBoardPlacePoint();                //第一回合开始时初始化棋盘放置点

    bool blackTurn = isBlackRole && isBlackRound;   //在开始游戏后，只有黑方能选中第一个棋子
    if (!blackTurn)
        return;
    gl->onSelectChess(chessSprites[0], "black_zhe");//自动选中第一个黑棋
    selectedHighlight->setVisible(true);
}

void OnlineMode::switchRound()
{
    isBlackRound = !isBlackRound;                       //回合交换       
    if (isBlackRound) {                                 //如果黑方回合
        timer->setTextColor(Color4B::BLACK);            //黑方回合计时器是黑色的
        blackRoundArrow->setVisible(true);              //指向黑方的箭头显示
        whiteRoundArrow->setVisible(false);             //指向白方的箭头隐藏           
    }
    else {                                              //反之
        timer->setTextColor(Color4B::WHITE);
        whiteRoundArrow->setVisible(true);
        blackRoundArrow->setVisible(false);
    }

    //当处于自己回合时才自动选中棋子
    if (isBlackRole && isBlackRound) {                  
        gl->onSelectChess(chessSprites[0], "black_zhe");//自动选中第一个黑方棋子
        selectedHighlight->setVisible(true);
    }
    else if (!isBlackRole && !isBlackRound) {
        gl->onSelectChess(chessSprites[5], "white_zhe");//自动选中第一个白方棋子
        selectedHighlight->setVisible(true);
    }
    else
        selectedHighlight->setVisible(false);           //不在自己回合时隐藏高亮

    lastChessSum = curChessSum;                 //更新棋盘旧棋子总数
}

void OnlineMode::autoPlaceChess() {      
    //当处于自己回合，却没有落子时，系统帮忙落子
    bool myTurn = ((isBlackRole && isBlackRound) || (!isBlackRole && !isBlackRound));
    if (myTurn && lastChessSum == curChessSum) {       
        if (selectedPlacePoint) {               //如果有选中放置点，自动落子
            std::string msg = "move:" + std::to_string(selectedRowCol[0]) + ',' + std::to_string(selectedRowCol[1]) + ',' + selectedChessName;
            NetworkManager::getInstance()->sendMsg(msg);
            gl->onPlaceChess(selectedRowCol);
        }
        else
            for (int row = 0; row < 19 && lastChessSum == curChessSum; row++)   //lastChessSum == curChessSum保证只落一个子
                for (int col = 0; col < 19; col++)
                    if (boardChesses[row][col] == nullptr) {                    //没有选中放置点，找到第一个可放置处，帮忙落子
                        NetworkManager::getInstance()->sendMsg("move:" + std::to_string(row) + ',' + std::to_string(col) + ',' + selectedChessName);
                        gl->onPlaceChess({ row, col });                         //直接落子
                        break;                                                  //直接退出，保证只落一个子              
        }
        switchRound();                          //系统帮忙落子完也要切换回合
        
        if (!isGamePlaying)
            return;                             //如果系统帮忙落子恰好有一方获胜，直接退出，无需后续逻辑
    }
        
    if (curChessSum == 19 * 19) {
        gameOver(true);                         //如果棋盘放满都没胜负，则为平局
        return;
    }
    
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

    isBlackRole = !isBlackRole;
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