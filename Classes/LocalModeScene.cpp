#include "LocalModeScene.h"
#include "SimpleAudioEngine.h"
#include "BoardUI.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* LocalMode::createScene() {
    return LocalMode::create();
}

bool LocalMode::init() {
    if (!Scene::init())
        return false;

    auto bu = BoardUI::create();
    this->addChild(bu, 0);

    bu->setOnReturnCallBack([]() {
        Director::getInstance()->popScene();
    });

    bu->setOnToggleEffectCallBack([=]() {
        isEffectOn = !isEffectOn;
    });

    bu->setOnTouchBeganCallBack(CC_CALLBACK_2(LocalMode::onTouchBegan, this));

    bu->setOnStartGameCallBack(CC_CALLBACK_1(LocalMode::onStartGame, this));

    bu->setOnCleanBoardCallBack(CC_CALLBACK_1(LocalMode::cleanBoard, this));

    bu->getBoardUImember(chessSprites, selectedHighlight, startGameBtn, timer, blackRoundArrow, whiteRoundArrow, gameOverTip,
        victoryAnimation, drawAnimation, gameOverBtn, gameOverDrawBtn);

    auto origin = Director::getInstance()->getVisibleOrigin();
    bu->getGameOverUI()->setPosition(origin.x, origin.y + 84.0f);    //该ui组本来在正中间，往上提了点
    this->addChild(bu->getGameOverUI(), 2);                          //ui组单独拿出来的目的，为了显示在最高层级

    return true;
}

bool LocalMode::onTouchBegan(Touch* touch, Event* event) {
    if (!isGamePlaying)
        return false;                                           //没在游戏中就不响应点击
    Vec2 touchPos = touch->getLocation();                       //获取点击的位置
    for (auto& chess : chessSprites)
        if (chess->getBoundingBox().containsPoint(touchPos)) {  //判断是否有棋子被点击了
            onSelectChess(chess, chess->getName());             //调用选中棋子函数
            return true;                                        //消费掉这个点击事件
        }
    if (!selectedChessName.empty())                             //如果有棋子被选中，而点击的位置不是棋子，可能在棋盘上
        return onPlaceChess(touchPos);                          //进行落子处理
    return false;
}

void LocalMode::onSelectChess(Sprite* chessSprite, const std::string& chessName) {
    //黑方回合时不可选择白方棋子，白方回合时不可选择黑方棋子
    if ((isBlackRound && chessName.substr(0, 5) == "white") || (!isBlackRound && chessName.substr(0, 5) == "black"))
        return;
    
    selectedChessName = chessName;                                  //保存当前被选中棋子的名字
    selectedHighlight->setPosition(chessSprite->getPosition());     //高光和棋子相同位置
}

void LocalMode::onInitBoardPlacePoint() {
    //先初始化各数组
    placePoints.assign(19, std::vector<Sprite*>(19, nullptr));
    boardChesses.assign(19, std::vector<Sprite*>(19, nullptr));
    canPlace.assign(19, std::vector<bool>(19, true));       //一开始棋盘上无棋子，所有点均可放置   
    for (size_t row = 0; row < 19; row++)
        for (size_t col = 0; col < 19; col++) {
            auto pp = Sprite::create("placePoint.png");
            if (pp) {
                //从左下角到右上角计算可放置点位置
                auto origin = Director::getInstance()->getVisibleOrigin();
                pp->setPosition(origin.x + 75.0f + col * 50.0f, origin.y + 75.0f + row * 50.0f);
                pp->setScale(35.0f / pp->getContentSize().width, 35.0f / pp->getContentSize().height);
                pp->setOpacity(200);
                pp->setVisible(false);          //全部放置点隐藏
                this->addChild(pp, 1);
                placePoints[row][col] = pp;     //保存在数组里
            }
            else
                cocos2d::log("placePoint.png");
        }
}

bool LocalMode::onPlaceChess(Vec2 touchPos) {
    for (int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++)
            //如果当前点可放置
            if (canPlace[row][col] && placePoints[row][col]->getBoundingBox().containsPoint(touchPos)) {
                if (selectedPlacePoint) {                                           //如果已存在选中放置点
                    if (selectedPlacePoint == placePoints[row][col]) {              //检查是否是当前放置点
                        auto chess = Sprite::create("chess/" + selectedChessName + ".png");    //由选中棋子名字生成对应棋子
                        if (chess) {
                            chess->setPosition(selectedPlacePoint->getPosition());  //棋子位置与放置点一致
                            chess->setScale(50.0f / chess->getContentSize().width, 50.0f / chess->getContentSize().height);
                            chess->setName(selectedChessName);
                            this->addChild(chess, 1);
                            boardChesses[row][col] = chess;                         //存放在棋盘棋子数组里
                            selectedPlacePoint->setVisible(false);                  //放置点隐藏
                            selectedPlacePoint = nullptr;                           //置空，防止野指针
                            canPlace[row][col] = false;                             //当前点已有棋子，表示不可放置
                            //当音效开启时，根据棋子类型输出对应落子音效
                            if(isEffectOn)
                                SimpleAudioEngine::getInstance()->playEffect(("music/" + selectedChessName.substr(6) + ".mp3").c_str());
                            curChessSum++;                                          //当前棋盘上棋子总数加1
                            if (isVictory(row, col))
                                gameOver(false);                                    //如果获胜，调用游戏结束函数
                            roundSurplusTime = 0;                                   //回合时间清零，即切换回合
                        }
                        else
                            cocos2d::log("chess.png");
                        return true;
                    }
                    else
                        selectedPlacePoint->setVisible(false);                      //如果选中点不是当前点，将之前点隐藏
                }
                selectedPlacePoint = placePoints[row][col];                         //更新选中点
                selectedPlacePoint->setVisible(true);                               //显示选中点
                return true;
            }
    return false;
}

void LocalMode::onStartGame(Ref* pSender){
    startGameBtn->setVisible(false);                //按钮隐藏，代表已开始游戏
    
    isGamePlaying = true;                           //表示正在游戏中
    roundSurplusTime = 20.9f;                       //回合时间20秒左右
    isBlackRound = true;                            //默认第一回合是黑方
    blackRoundArrow->setVisible(true);
    whiteRoundArrow->setVisible(false);             //指向白方的箭头先隐藏
    timer->setVisible(true);                        //显示计时器

    this->scheduleUpdate();                         //启动帧循环，每帧自动调用 update(float dt)

    if (placePoints.empty())                        
        onInitBoardPlacePoint();                    //第一回合开始时初始化棋盘放置点
    
    selectedHighlight->setVisible(true);            //显示高亮
    onSelectChess(chessSprites[0], "black_zhe");    //自动选中第一个黑棋
}

void LocalMode::update(float dt) {
    if (!isGamePlaying)
        return;                                         //不在游戏中不处理更新逻辑

    roundSurplusTime -= dt;                             //更新当前回合剩余时间
    if (roundSurplusTime <= 0) {                        //回合时间到
        if (lastChessSum == curChessSum) {              //如果玩家未落子
            if (selectedPlacePoint)                     //如果有选中放置点，自动落子
                onPlaceChess(selectedPlacePoint->getPosition());           
            else
                for(int row = 0; row < 19 && lastChessSum == curChessSum; row++)//lastChessSum == curChessSum保证只落一个子
                    for (int col = 0; col < 19; col++)
                        if (canPlace[row][col]) {                               //没有选中放置点，找到第一个可放置点，帮忙落子
                            onPlaceChess(placePoints[row][col]->getPosition()); //第一步是先选放置点
                            onPlaceChess(placePoints[row][col]->getPosition()); //第二步才正式落子
                            break;                                              //直接退出，保证只落一个子
                        }           
        }        
        if (!isGamePlaying)
            return;                                     //如果系统帮忙落子恰好有一方获胜，直接退出，无需后续逻辑        
        if (curChessSum == 19 * 19) {
            gameOver(true);                             //如果棋盘放满都没胜负，则为平局
            return;
        }
        lastChessSum = curChessSum;                     //更新棋盘旧棋子总数
        roundSurplusTime = 20.9f;                       //回合结束，开始下一回合
        isBlackRound = !isBlackRound;                   //回合交换       
        if (isBlackRound) {                             //如果黑方回合
            timer->setTextColor(Color4B::BLACK);        //黑方回合计时器是黑色的
            blackRoundArrow->setVisible(true);          //指向黑方的箭头显示
            whiteRoundArrow->setVisible(false);         //指向白方的箭头隐藏
            onSelectChess(chessSprites[0], "black_zhe");//自动选中第一个黑方棋子
        }
        else {                                          //反之
            timer->setTextColor(Color4B::WHITE);
            whiteRoundArrow->setVisible(true);
            blackRoundArrow->setVisible(false);
            onSelectChess(chessSprites[5], "white_zhe");//自动选中第一个白方棋子
        }
    }
    else if (roundSurplusTime < 6)
        timer->setTextColor(Color4B::RED);              //倒计时剩5秒时呈红色
    
    int seconds = (int)std::floor(roundSurplusTime);    //倒计时向下取整，可以确保看得到0
    timer->setString(std::to_string(seconds));          //实时显示在计时器标签上
}

bool LocalMode::isVictory(int row, int col) {
    std::unordered_map<std::string, int> needChesses;   //用来判断连成线的5个棋子是否同颜色不同字
    if (isBlackRound) {
        //表示在黑方回合时，需要“这谁绷得住”5种黑棋各一个
        needChesses["black_zhe"] = 1;
        needChesses["black_shui"] = 1;
        needChesses["black_beng"] = 1;
        needChesses["black_de"] = 1;
        needChesses["black_zhu"] = 1;
    }
    else {
        //表示在白方回合时，需要“这谁绷得住”5种白棋各一个
        needChesses["white_zhe"] = 1;
        needChesses["white_shui"] = 1;
        needChesses["white_beng"] = 1;
        needChesses["white_de"] = 1;
        needChesses["white_zhu"] = 1;
    }

    //一共有4条线需要判断，分别是竖、斜、横、反斜，而一条线又分两个方向，只要有一条线能连成5子，即为获胜
    if (searchBoardChesses(row, col, Up, Down, needChesses))
        return true;
    else if (searchBoardChesses(row, col, LeftUp, RightDown, needChesses))
        return true;
    else if (searchBoardChesses(row, col, Left, Right, needChesses))
        return true;
    else if (searchBoardChesses(row, col, LeftDown, RightUp, needChesses))
        return true;
    return false;
}

bool LocalMode::searchBoardChesses(int row, int col, Direction dir_1, Direction dir_2, std::unordered_map<std::string, int> needChesses){
    int r = row, c = col;                                           //先保存落子点坐标，判断第一个方向
    while (r >= 0 && r < 19 && c >= 0 && c < 19 && boardChesses[r][c]) {
        std::string curChessName = boardChesses[r][c]->getName();
        if (needChesses.find(curChessName) == needChesses.end())    //如果遇到另一方的棋子，直接退出判断
            break;
        else if (needChesses[curChessName] == 0)                    //如果该棋子类型已有一个，退出判断
            break;
        needChesses[curChessName]--;                                //减1为0，代表该棋子类型已找到一个
        r += dir_1.x; c += dir_1.y;                                 //更新坐标
    }
    row += dir_2.x; col += dir_2.y;                                 //从落子点另一个方向走一步，开始另一个方向的判断
    while (row >= 0 && row < 19 && col >= 0 && col < 19 && boardChesses[row][col]) {
        std::string curChessName = boardChesses[row][col]->getName();
        if (needChesses.find(curChessName) == needChesses.end())
            break;
        else if (needChesses[curChessName] == 0)
            break;
        needChesses[curChessName]--;
        row += dir_2.x; col += dir_2.y;
    }
    for (auto& n : needChesses)
        if (n.second == 1)
            return false;           //只要发现有一个棋子不在，就没获胜
    return true;
}

void LocalMode::gameOver(bool isDraw){
    if (isDraw) {
        gameOverTip->setString(u8"双方打平!");
        drawAnimation->setVisible(true); //显示平局动画
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
    gameOverTip->setVisible(true);               //结算提示隐藏
}

void LocalMode::cleanBoard(Ref* pSender) {
    lastChessSum = 0; curChessSum = 0;                          //棋盘上棋子总数清零
    for(int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++)
            if (boardChesses[row][col]) {
                boardChesses[row][col]->removeFromParent();     //删除所有保存的棋子
                boardChesses[row][col] = nullptr;
                canPlace[row][col] = true;                      //恢复可放置的状态
            }
    
    //隐藏游戏结算相关ui
    gameOverTip->setVisible(false);
    victoryAnimation->setVisible(false);                        //隐藏获胜动画
    drawAnimation->setVisible(false);
    gameOverBtn->setVisible(false);                             //隐藏结束游戏按钮
    gameOverDrawBtn->setVisible(false);
    
    startGameBtn->setVisible(true);                             //显示开始游戏按钮，为下一次游戏做准备
}