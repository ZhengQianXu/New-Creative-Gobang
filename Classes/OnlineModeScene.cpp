#include "OnlineModeScene.h"
#include "SimpleAudioEngine.h"
#include "BoardUI.h"
#include "NetworkManager.h"
#include "PopupUI.h"

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
    bu->setOnReturnCallBack([=]() {
        if (isGamePlaying) {
			showExitConfirmPopup();     //游戏中点击返回按钮，弹出确认退出提示框
            return;
        }
		//否则直接返回主菜单
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
        victoryAnimation, defeatAnimation, drawAnimation, gameOverVictoryBtn, gameOverDefeatBtn, gameOverDrawBtn);

    auto origin = Director::getInstance()->getVisibleOrigin();
    bu->getGameOverUI()->setPosition(origin.x, origin.y + 84.0f);    //该ui组本来在正中间，往上提了点
    this->addChild(bu->getGameOverUI(), 2);                          //ui组单独拿出来的目的，为了显示在最高层级

    //创建游戏逻辑对象，并传入需要用到的ui控件
    gl = new GameLogic(this, isBlackRound, selectedChessName, selectedHighlight, selectedPlacePoint, selectedRowCol, placePoints, boardChesses);

    auto nm = NetworkManager::getInstance();            //获取网络管理模块单例

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
        int seconds = (int)std::floor(surplusTime);     //倒计时向下取整，可以确保看得到0
        timer->setString(std::to_string(seconds));      //实时显示在计时器标签上 
        if (surplusTime <= 0)
			autoPlaceChess();                           //倒计时结束检查是否需要系统自动落子
        else if (surplusTime < 6)
            timer->setTextColor(Color4B::RED);          //倒计时剩5秒时呈红色
    });

    //传入对手落子回调
    nm->setOnOpponentMoveCallBack([=](int row, int col, const std::string chessName) {       
        selectedChessName = chessName;                  //先设置棋子名字
        onPlaceChess({ row, col }, false);              //再进行落子，对手发来的落子不要再发回去            
    });    

    //传入退出房间处理回调，另一方退出房间时调用
    nm->setOnQuitRoomCallBack([=]() {
        opponentQuitRoom();
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
            return true;                                        //消费掉这个点击事件
        }
    
    //如果有棋子被选中，而点击的位置不是棋子，可能在棋盘上，但是只有在自己回合才能对棋盘进行操作
    if (!selectedChessName.empty() && ((isBlackRole && isBlackRound) || (!isBlackRole && !isBlackRound))) {
        if (selectedPlacePoint && selectedPlacePoint->getBoundingBox().containsPoint(touchPos))          
            return onPlaceChess(selectedRowCol, true);          //如果已选中放置点，且再次点击放置点，则落子
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
	timer->setTextColor(Color4B::BLACK);            //第一回合黑方回合，计时器是黑色的
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

bool OnlineMode::onPlaceChess(std::vector<int> rowCol, bool isSendMsg)
{
    int row = rowCol[0], col = rowCol[1];
    if (row < 0 || row >= 19 || col < 0 || col >= 19)
        return false;                                                   //防止下标越界访问
    if (!isGamePlaying || boardChesses[row][col])
        return false;                                                   //不在游戏中或该位置已有棋子，不可落子
    auto chess = Sprite::create("chess/" + selectedChessName + ".png"); //由选中棋子名字生成对应棋子
    auto nm = NetworkManager::getInstance();
    if (chess) {
        if (isSendMsg) {                                                //如果是己方落子，需要向服务器报告，然后由服务器转发给对手
            std::string msg = "move:" + std::to_string(row) + ',' + std::to_string(col) + ',' + selectedChessName;
            nm->sendMsg(msg);
        }
        chess->setPosition(placePoints[row][col]->getPosition());       //棋子位置与放置点一致
        chess->setScale(50.0f / chess->getContentSize().width, 50.0f / chess->getContentSize().height);
        chess->setName(selectedChessName);
        this->addChild(chess, 1);
        boardChesses[row][col] = chess;                                 //存放在棋盘棋子数组里
        if (selectedPlacePoint) {
            selectedPlacePoint->setVisible(false);                      //放置点隐藏
            selectedPlacePoint = nullptr;                               //置空，防止野指针     
        }
        //当音效开启时，根据棋子类型输出对应落子音效        
        if (isEffectOn)
            SimpleAudioEngine::getInstance()->playEffect(("music/" + selectedChessName.substr(6) + ".mp3").c_str());
        curChessSum++;                                                  //当前棋盘上棋子总数加1
        if (gl->isVictory(row, col)) {                                  //如果有一方获胜            
            gameOver(false);                                            //调用游戏结束函数
            return true;                                                //立即返回，不需要切换回合引起不必要的麻烦
        }
        if(curChessSum == 19 * 19) {                                    //如果棋盘放满都没胜负，则为平局
            gameOver(true);                                             //平局结束处理
            return true;                                                //立即返回，不需要切换回合引起不必要的麻烦
		}
        switchRound();                                                  //落子完成回合切换
        return true;
    }
    else
        cocos2d::log("chess.png");
    return false;
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
    if (isBlackRole && isBlackRound)                  
        gl->onSelectChess(chessSprites[0], "black_zhe");//自动选中第一个黑方棋子        
    else if (!isBlackRole && !isBlackRound)
        gl->onSelectChess(chessSprites[5], "white_zhe");//自动选中第一个白方棋子        
    else
        selectedHighlight->setVisible(false);           //不在自己回合时隐藏高亮

    lastChessSum = curChessSum;                         //更新棋盘旧棋子总数
}

void OnlineMode::autoPlaceChess() {
    if (!isGamePlaying)
        return;
    //当处于自己回合，却没有落子时，lastChessSum == curChessSum 代表没有落子，系统帮忙落子
    bool myTurn = ((isBlackRole && isBlackRound) || (!isBlackRole && !isBlackRound));
    if (myTurn && lastChessSum == curChessSum) {       
        if (selectedPlacePoint)
            onPlaceChess(selectedRowCol, true);                                 //如果有选中放置点，自动落子
        else
            for (int row = 0; row < 19 && lastChessSum == curChessSum; row++)   //lastChessSum == curChessSum 保证只落一个子
                for (int col = 0; col < 19; col++)
                    if (boardChesses[row][col] == nullptr) {                    //没有选中放置点，找到第一个可放置处，帮忙落子                        
                        onPlaceChess({ row, col }, true);                       //直接落子
                        lastChessSum--;                                         //落子函数更新了lastChessSum，先减1，触发外层循环退出
                        break;                                                  //直接退出，保证只落一个子              
                    }
        lastChessSum++;                                                         //加回来，其实现在 lastChessSum == curChessSum
    }
}

void OnlineMode::gameOver(bool isDraw) {
    NetworkManager::getInstance()->sendMsg("gameOver");                     //告诉服务器游戏结算，停止计时

    auto victoryHandler = [=]() -> void {
        victoryAnimation->setVisible(true);                                 //显示获胜动画
        gameOverVictoryBtn->setVisible(true);                               //显示获胜结束游戏按钮
        SimpleAudioEngine::getInstance()->playEffect("music/victory.mp3");  //播放获胜音效
    };

    auto defeatHandler = [=]() -> void {
        defeatAnimation->setVisible(true);                                  //显示失败动画
        gameOverDefeatBtn->setVisible(true);                                //显示失败结束游戏按钮
        SimpleAudioEngine::getInstance()->playEffect("music/defeat.mp3");   //播放失败音效
	};

    if (isDraw) {
        gameOverTip->setString(u8"双方打平!");
        drawAnimation->setVisible(true);                                    //显示平局动画
        gameOverDrawBtn->setVisible(true);                                  //显示平局游戏结束按钮
        SimpleAudioEngine::getInstance()->playEffect("music/defeat.mp3");   //播放平局音效
    }
    else {
		//这时isBlackRound代表获胜方，而isBlackRole代表玩家自己，二者相同则玩家获胜，否则玩家失败
        if (isBlackRole) {
            if (isBlackRound) {
                gameOverTip->setString(u8"黑方获胜！你赢了!");
                victoryHandler();
            }
            else {
                gameOverTip->setString(u8"白方获胜，你输了!");
                defeatHandler();
            }
        }
        else {
            if (isBlackRound){
                gameOverTip->setString(u8"黑方获胜，你输了!");
                defeatHandler();
			}
            else {
                gameOverTip->setString(u8"白方获胜！你赢了!");
                victoryHandler();
            }
        }   
    }

    isGamePlaying = false;                      //游戏结束
    timer->setVisible(false);                   //计时器隐藏
    blackRoundArrow->setVisible(false);         //黑方箭头隐藏
    whiteRoundArrow->setVisible(false);         //白方箭头隐藏
    selectedHighlight->setVisible(false);       //隐藏选中高亮
    selectedChessName = "";                     //选中棋子名字重置
    selectedPlacePoint = nullptr;               //选中放置点重置
    gameOverTip->setVisible(true);              //显示结算提示

    isBlackRole = !isBlackRole;                 //交换角色
}

void OnlineMode::cleanBoard(Ref* pSender) {
	//如果对手退出房间，等玩家点击结算按钮后直接返回主菜单，不需要后续处理
    if (isOpponentQuit) {
        NetworkManager::getInstance()->disconnect();
        Director::getInstance()->popScene();
        return;
    }

    lastChessSum = 0; curChessSum = 0;                          //棋盘上棋子总数清零
    for (size_t row = 0; row < 19; row++)
        for (size_t col = 0; col < 19; col++)
            if (boardChesses[row][col]) {
                boardChesses[row][col]->removeFromParent();     //删除所有保存的棋子
                boardChesses[row][col] = nullptr;
            }

    //隐藏游戏结算相关ui
    gameOverTip->setVisible(false);
    victoryAnimation->setVisible(false);                        //隐藏结束动画
	defeatAnimation->setVisible(false);
    drawAnimation->setVisible(false);
    gameOverVictoryBtn->setVisible(false);                      //隐藏结束游戏按钮
    gameOverDefeatBtn->setVisible(false);
    gameOverDrawBtn->setVisible(false);

    startGameBtn->setVisible(true);                             //显示开始游戏按钮，为下一次游戏做准备
    canStartGame = false;                                       //恢复到准备状态
}

void OnlineMode::showExitConfirmPopup()
{
    auto title = u8"确认退出房间吗?";
    auto content = u8"如果你现在退出房间，视为认输";
	auto popupUI = PopupUI::create(title, content);                     //添加遮罩层和弹窗
    this->addChild(popupUI, 10);
    auto origin = Director::getInstance()->getVisibleOrigin();
	auto popupContent = popupUI->getPopupContent();                     //获取弹窗内容节点
	popupContent->setPosition(origin.x + 300.0f, origin.y + 160.0f);    //重新设置弹窗位置，在屏幕正中间偏上
	auto popup = popupUI->getPopup();                                   //获取弹窗节点，用于添加按钮

	//创建取消按钮，点击按钮关闭弹窗，回到游戏界面
    auto cancelBtn = MenuItemImage::create("cancel.png", "cancel_pressed.png", [=](Ref* pSender) {
		popupUI->removeFromParent();
    });
    if (cancelBtn)
        cancelBtn->setPosition(Vec2::ZERO);
    else
        CCLOG("'cancel.png or cancel_pressed.png'");

	//创建存放取消按钮的菜单，位置在弹窗左下角
    auto cancelBtnMenu = Menu::create(cancelBtn, nullptr);
    if (cancelBtnMenu) {
        cancelBtnMenu->setPosition(origin.x + 180.0f, origin.y + 60.0f);
        popup->addChild(cancelBtnMenu, 1);
    }
    else
        CCLOG("'cancelBtn'");

	//创建确认按钮，点击按钮断开网络连接，返回主菜单
    auto confirmBtn = MenuItemImage::create("confirm.png", "confirm_pressed.png", [](Ref* pSender) {
        NetworkManager::getInstance()->disconnect();
        Director::getInstance()->popScene();
    });
    if (confirmBtn)
        confirmBtn->setPosition(Vec2::ZERO);
    else
        CCLOG("'confirm.png or confirm_pressed.png'");

    //创建存放确认按钮的菜单，位置在弹窗右下角
    auto confirmBtnMenu = Menu::create(confirmBtn, nullptr);
    if (confirmBtnMenu) {
        confirmBtnMenu->setPosition(origin.x + popup->getContentSize().width - 180.0f, origin.y + 60.0f);
        popup->addChild(confirmBtnMenu, 1);
    }
    else
        CCLOG("'confirmBtn'");
}

void OnlineMode::opponentQuitRoom()
{
	//游戏准备状态，不在游戏中，如果这时对手退出房间，玩家直接回到主菜单
    if (!canStartGame) {
        NetworkManager::getInstance()->disconnect();
        Director::getInstance()->popScene();
    }
	//游戏中对手退出房间，直接获胜结算，显示对手已退出提示，而玩家在点击结算按钮后也返回主菜单
    else if (isGamePlaying) {        
        gameOverTip->setString(u8"对方已退出，你赢了!");
        gameOverTip->setVisible(true);
        victoryAnimation->setVisible(true);
        gameOverVictoryBtn->setVisible(true);
        isOpponentQuit = true;
    }
	//游戏结算后对手退出房间，但玩家未按下结算按钮，等待玩家点击结算按钮后直接返回主菜单
    else if (canStartGame)
        isOpponentQuit = true;
}