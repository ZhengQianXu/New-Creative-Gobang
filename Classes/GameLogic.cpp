#include "GameLogic.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

GameLogic::GameLogic(Scene* s, bool& iBR, std::string& sCN, Sprite*& sH, Sprite*& sPP, std::vector<int>& sRC, 
    std::vector<std::vector<Sprite*>>& pP, std::vector<std::vector<Sprite*>>& bC) 
    : scene(s), isBlackRound(iBR), selectedChessName(sCN), selectedHighlight(sH), selectedPlacePoint(sPP), selectedRowCol(sRC), 
    placePoints(pP), boardChesses(bC) {}

void GameLogic::onSelectChess(Sprite* chessSprite, const std::string& chessName) {
    //黑方回合时不可选择白方棋子，白方回合时不可选择黑方棋子
    if ((isBlackRound && chessName.substr(0, 5) == "white") || (!isBlackRound && chessName.substr(0, 5) == "black"))
        return;

    selectedChessName = chessName;                                  //保存当前被选中棋子的名字
    selectedHighlight->setPosition(chessSprite->getPosition());     //高光和棋子相同位置
	selectedHighlight->setVisible(true);                            //高光显示
}

void GameLogic::onInitBoardPlacePoint() {
    //先初始化各数组，分配数组大小
    placePoints.assign(19, std::vector<Sprite*>(19, nullptr));
    boardChesses.assign(19, std::vector<Sprite*>(19, nullptr));
    for (size_t row = 0; row < 19; row++)
        for (size_t col = 0; col < 19; col++) {
            auto pp = Sprite::create("placePoint.png");
            if (pp) {                
                auto origin = Director::getInstance()->getVisibleOrigin();
                pp->setPosition(origin.x + 75.0f + col * 50.0f, origin.y + 75.0f + row * 50.0f);    //从左下角到右上角计算可放置点位置
                pp->setScale(35.0f / pp->getContentSize().width, 35.0f / pp->getContentSize().height);
                pp->setOpacity(200);            //微透明
                pp->setVisible(false);          //全部放置点隐藏
                scene->addChild(pp, 1);
                placePoints[row][col] = pp;     //保存在数组里
            }
            else
                cocos2d::log("placePoint.png");
        }
}

bool GameLogic::onSelectPlacePoint(Vec2 touchPos)
{
    for (int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++)
            //如果当前点可放置，boardChesses[row][col] == nullptr 表示该处未放置棋子，即可放置，并且点击的是当前点
            if (boardChesses[row][col] == nullptr && placePoints[row][col]->getBoundingBox().containsPoint(touchPos)) {
                if (selectedPlacePoint)
                    selectedPlacePoint->setVisible(false);      //把之前选中的放置点隐藏
                selectedPlacePoint = placePoints[row][col];     //更新刚才选中的放置点
                selectedPlacePoint->setVisible(true);           //显示出来
                selectedRowCol = { row, col };                  //存储选中的行和列
                return true;
            }        
    return false;                                               //代表点击不在棋盘上，让点击事件给下一个监听器处理
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
            return false;                                           //只要发现有一个棋子不在，就没获胜
    return true;
}