#include "GameLogic.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

GameLogic::GameLogic(Scene* s, bool& iEO, bool& iGP, bool& iBR, float& rST, std::string& sCN, Sprite*& sH, Sprite*& sPP, 
    std::vector<Sprite*>& cS, std::vector<std::vector<Sprite*>>& pP, std::vector<std::vector<Sprite*>>& bC, int& cCS) 
    : scene(s), isEffectOn(iEO), isGamePlaying(iGP), isBlackRound(iBR), roundSurplusTime(rST), selectedChessName(sCN)
    , selectedHighlight(sH), selectedPlacePoint(sPP), chessSprites(cS), placePoints(pP), boardChesses(bC), curChessSum(cCS) {}
//{
//    scene = s;
//    isEffectOn = iEO;
//    isGamePlaying = iGP;
//    isBlackRound = iBR;
//    roundSurplusTime = rST;
//    selectedChessName = sCN;
//    selectedHighlight = sH;
//    selectedPlacePoint = sPP;
//    chessSprites = cS;
//    placePoints = pP;
//    boardChesses = bC;
//    curChessSum = cCS;
//}

bool GameLogic::onTouchBegan(Touch* touch, Event* event) {
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

void GameLogic::onSelectChess(Sprite* chessSprite, const std::string& chessName) {
    //黑方回合时不可选择白方棋子，白方回合时不可选择黑方棋子
    if ((isBlackRound && chessName.substr(0, 5) == "white") || (!isBlackRound && chessName.substr(0, 5) == "black"))
        return;

    selectedChessName = chessName;                                  //保存当前被选中棋子的名字
    selectedHighlight->setPosition(chessSprite->getPosition());     //高光和棋子相同位置
}

void GameLogic::onInitBoardPlacePoint() {
    //先初始化各数组
    placePoints.assign(19, std::vector<Sprite*>(19, nullptr));
    boardChesses.assign(19, std::vector<Sprite*>(19, nullptr));
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
                scene->addChild(pp, 1);
                placePoints[row][col] = pp;     //保存在数组里
            }
            else
                cocos2d::log("placePoint.png");
        }
}

void GameLogic::setGameOverFunction(std::function<void(bool)> func) {
    _gameOver = func;
}

void GameLogic::gameOver(bool isDraw) {
    if (_gameOver)
        _gameOver(isDraw);
}

bool GameLogic::onPlaceChess(Vec2 touchPos) {
    for (int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++)
            //如果当前点可放置，boardChesses[row][col] == nullptr表示该处未放置棋子，即可放置
            if (boardChesses[row][col] == nullptr && placePoints[row][col]->getBoundingBox().containsPoint(touchPos)) {
                if (selectedPlacePoint) {                                           //如果已存在选中放置点
                    if (selectedPlacePoint == placePoints[row][col]) {              //检查是否是当前放置点
                        auto chess = Sprite::create("chess/" + selectedChessName + ".png");    //由选中棋子名字生成对应棋子
                        if (chess) {
                            chess->setPosition(selectedPlacePoint->getPosition());  //棋子位置与放置点一致
                            chess->setScale(50.0f / chess->getContentSize().width, 50.0f / chess->getContentSize().height);
                            chess->setName(selectedChessName);
                            scene->addChild(chess, 1);
                            boardChesses[row][col] = chess;                         //存放在棋盘棋子数组里
                            selectedPlacePoint->setVisible(false);                  //放置点隐藏
                            selectedPlacePoint = nullptr;                           //置空，防止野指针                           
                            //当音效开启时，根据棋子类型输出对应落子音效
                            CCLOG("onPlaceChess, %d", isEffectOn);
                            if (isEffectOn)
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

bool GameLogic::isVictory(int row, int col) {
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

bool GameLogic::searchBoardChesses(int row, int col, Direction dir_1, Direction dir_2, std::unordered_map<std::string, int> needChesses) {
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