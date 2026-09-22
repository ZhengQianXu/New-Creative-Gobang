#include "BoardUI.h"
#include "MusicControl.h"

USING_NS_CC;

bool BoardUI::init() {
	if (!Node::init())
		return false;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    //添加返回的按钮
    auto returnBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(BoardUI::onReturnBtn, this));
    if (returnBtn) {
        returnBtn->setPosition(Vec2::ZERO);
        returnBtn->setScale(44.0f / returnBtn->getContentSize().width, 44.0f / returnBtn->getContentSize().height);
    }
    else
        cocos2d::log("'CloseNormal.png' and 'CloseSelected.png'");

    //创建存放返回按钮的菜单，位置在右下角
    auto returnMenu = Menu::create(returnBtn, nullptr);
    if (returnMenu) {
        returnMenu->setPosition(origin.x + visibleSize.width - 22.0f, origin.y + 22.0f);
        this->addChild(returnMenu, 1);
    }
    else
        cocos2d::log("“returnBtn”");

    //给关闭按钮加个提示，文字位置在按钮左边
    auto returnTip = Label::create(u8"点击返回主界面->", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (returnTip) {
        returnTip->setPosition(origin.x + visibleSize.width - 130.0f, origin.y + 22.0f);
        returnTip->setTextColor(Color4B::BLACK);
        this->addChild(returnTip, 1);
    }
    else
        cocos2d::log("“fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf”");

    //添加背景音乐和控制按钮
    MusicControl* mc = MusicControl::create();
    this->addChild(mc, 1);

    //添加落子音效控制按钮
    auto effectBtn = MenuItemImage::create("effect_btn.png", "effect_btn.png", CC_CALLBACK_1(BoardUI::onToggleEffectBtn, this));
    if (effectBtn) {
        effectBtn->setScale(44.0f / effectBtn->getContentSize().width, 44.0f / effectBtn->getContentSize().height);
        effectBtn->setPosition(Vec2::ZERO);
    }
    else
        cocos2d::log("'effect_btn.png'");

    //添加存放音效按钮的菜单
    auto effectMenu = Menu::create(effectBtn, nullptr);
    if (effectMenu) {
        effectMenu->setPosition(origin.x + visibleSize.width / 2 - 70.0f, origin.y + 22.0f);    //位置在底部中间靠左
        this->addChild(effectMenu, 1);
    }
    else
        cocos2d::log("'effectBtn'");

    //添加落子音效按钮提示
    auto effectBtnTip = Label::create(u8"<-点击开关落子音效", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (effectBtnTip) {
        effectBtnTip->setPosition(origin.x + visibleSize.width / 2 + 70.0f, origin.y + 22.0f); //放在音效按钮右边
        effectBtnTip->setTextColor(Color4B::BLACK);
        this->addChild(effectBtnTip, 1);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //添加木纹色背景
    auto bgLayer = LayerColor::create(Color4B(181, 136, 99, 255));
    this->addChild(bgLayer, -1);    //放在最底层

    //添加棋盘图片
    auto board = Sprite::create("board.png");
    if (board) {
        //设置棋盘占满屏宽，长宽一致
        board->setScale(visibleSize.width / board->getContentSize().width, visibleSize.width / board->getContentSize().height);
        //设置棋盘图片位置为屏幕下方中心
        board->setPosition(origin.x + visibleSize.width / 2, origin.x + visibleSize.width / 2);
        //添加棋盘图片到场景中
        this->addChild(board, 0);
    }
    else
        cocos2d::log("'board.png'");

    //添加黑方棋子,按“这谁绷得住”次序添加，黑底白字，显示在棋盘左上方
    //创建黑方棋子 "这"
    auto black_zhe = Sprite::create("chess/black_zhe.png");
    if (black_zhe) {
        //设置大小50px * 50px
        black_zhe->setScale(50.0f / black_zhe->getContentSize().width, 50.0f / black_zhe->getContentSize().height);
        //位置在棋盘左上方第一个
        black_zhe->setPosition(origin.x + 25.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_zhe, 0);
        black_zhe->setName("black_zhe");        //设置精灵名字
        chessSprites.push_back(black_zhe);      //加入棋子数组里
    }
    else
        cocos2d::log("black_zhe.png");

    //创建黑方棋子 "谁"
    auto black_shui = Sprite::create("chess/black_shui.png");
    if (black_shui) {
        black_shui->setScale(50.0f / black_shui->getContentSize().width, 50.0f / black_shui->getContentSize().height);
        black_shui->setPosition(origin.x + 75.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_shui, 0);
        black_shui->setName("black_shui");
        chessSprites.push_back(black_shui);
    }
    else
        cocos2d::log("chess/black_shui.png");

    //创建黑方棋子 "绷"
    auto black_beng = Sprite::create("chess/black_beng.png");
    if (black_beng) {
        black_beng->setScale(50.0f / black_beng->getContentSize().width, 50.0f / black_beng->getContentSize().height);
        black_beng->setPosition(origin.x + 125.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_beng, 0);
        black_beng->setName("black_beng");
        chessSprites.push_back(black_beng);
    }
    else
        cocos2d::log("chess/black_beng.png");

    //创建黑方棋子 "得"
    auto black_de = Sprite::create("chess/black_de.png");
    if (black_de) {
        black_de->setScale(50.0f / black_de->getContentSize().width, 50.0f / black_de->getContentSize().height);
        black_de->setPosition(origin.x + 175.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_de, 0);
        black_de->setName("black_de");
        chessSprites.push_back(black_de);
    }
    else
        cocos2d::log("chess/black_de.png");

    //创建黑方棋子 "住"
    auto black_zhu = Sprite::create("chess/black_zhu.png");
    if (black_zhu) {
        black_zhu->setScale(50.0f / black_zhu->getContentSize().width, 50.0f / black_zhu->getContentSize().height);
        black_zhu->setPosition(origin.x + 225.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_zhu, 0);
        black_zhu->setName("black_zhu");
        chessSprites.push_back(black_zhu);
    }
    else
        cocos2d::log("chess/black_zhu.png");

    //添加白方棋子,按“这谁绷得住”次序添加，白底黑字，显示在棋盘右上方
    //创建白方棋子 "这"
    auto white_zhe = Sprite::create("chess/white_zhe.png");
    if (white_zhe) {
        white_zhe->setScale(50.0f / white_zhe->getContentSize().width, 50.0f / white_zhe->getContentSize().height);
        //位置为棋盘右上方靠左第一个
        white_zhe->setPosition(origin.x + visibleSize.width - 225.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_zhe, 0);
        white_zhe->setName("white_zhe");
        chessSprites.push_back(white_zhe);
    }
    else
        cocos2d::log("chess/white_zhe.png");

    //创建白方棋子 "谁"
    auto white_shui = Sprite::create("chess/white_shui.png");
    if (white_shui) {
        white_shui->setScale(50.0f / white_shui->getContentSize().width, 50.0f / white_shui->getContentSize().height);
        white_shui->setPosition(origin.x + visibleSize.width - 175.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_shui, 0);
        white_shui->setName("white_shui");
        chessSprites.push_back(white_shui);
    }
    else
        cocos2d::log("chess/white_shui.png");

    //创建白方棋子 "绷"
    auto white_beng = Sprite::create("chess/white_beng.png");
    if (white_beng) {
        white_beng->setScale(50.0f / white_beng->getContentSize().width, 50.0f / white_beng->getContentSize().height);
        white_beng->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_beng, 0);
        white_beng->setName("white_beng");
        chessSprites.push_back(white_beng);
    }
    else
        cocos2d::log("chess/white_beng.png");

    //创建白方棋子 "得"
    auto white_de = Sprite::create("chess/white_de.png");
    if (white_de) {
        white_de->setScale(50.0f / white_de->getContentSize().width, 50.0f / white_de->getContentSize().height);
        white_de->setPosition(origin.x + visibleSize.width - 75.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_de, 0);
        white_de->setName("white_de");
        chessSprites.push_back(white_de);
    }
    else
        cocos2d::log("chess/white_de.png");

    //创建白方棋子 "住"
    auto white_zhu = Sprite::create("chess/white_zhu.png");
    if (white_zhu) {
        white_zhu->setScale(50.0f / white_zhu->getContentSize().width, 50.0f / white_zhu->getContentSize().height);
        white_zhu->setPosition(origin.x + visibleSize.width - 25.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_zhu, 0);
        white_zhu->setName("white_zhu");
        chessSprites.push_back(white_zhu);
    }
    else
        cocos2d::log("chess/white_zhu.png");

    //添加黑方棋子选择提示
    auto blackTipLabel = Label::createWithTTF(u8"黑方在上面选择棋子", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (blackTipLabel) {
        blackTipLabel->setPosition(origin.x + 125.0f, origin.y + visibleSize.width - 25.0f);    //位置在5个黑棋正下方
        blackTipLabel->setTextColor(Color4B::BLACK);                                            //字体为黑色
        this->addChild(blackTipLabel, 1);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //添加白方棋子选择提示，位置在5个白棋正下方
    auto whiteTipLabel = Label::createWithTTF(u8"白方在上面选择棋子", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (whiteTipLabel) {
        whiteTipLabel->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.width - 25.0f);
        whiteTipLabel->setTextColor(Color4B::BLACK);
        this->addChild(whiteTipLabel, 1);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //添加点击事件处理
    auto listener = EventListenerTouchOneByOne::create();                       //创建点击事件监听器
    listener->setSwallowTouches(false);                                         //不吞掉点击事件，让其他监听器也能处理该事件
    listener->onTouchBegan = CC_CALLBACK_2(BoardUI::onTouchBegan, this);        //点击开始回调函数
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);   //将监听器注册到事件分发器

    //添加高亮效果
    selectedHighlight = Sprite::create("highlight.png");
    if (selectedHighlight) {        
        float ScaleX = 70.0f / selectedHighlight->getContentSize().width;
        float ScaleY = 70.0f / selectedHighlight->getContentSize().height;
        selectedHighlight->setScale(ScaleX, ScaleY);                            //设置大小为70px * 70px
        selectedHighlight->setVisible(false);
        this->addChild(selectedHighlight, 0);
    }
    else
        cocos2d::log("highlight.png");

    //添加“当前回合”标签，位置在顶部中间
    auto curRound = Label::create(u8"当前回合", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 50);
    if (curRound) {
        curRound->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 35.0f);
        curRound->setTextColor(Color4B::BLACK);
        this->addChild(curRound, 0);
    }
    else
        cocos2d::log("fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf");

    //添加“黑方”标签，位置在顶部左边
    auto blackRound = Label::create(u8"黑方", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 35);
    if (blackRound) {
        blackRound->setPosition(origin.x + 125.0f, origin.y + visibleSize.height - 35.0f);
        blackRound->setTextColor(Color4B::BLACK);
        this->addChild(blackRound, 0);
    }
    else
        cocos2d::log("fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf");

    //添加“白方”标签，位置在顶部右边
    auto whiteRound = Label::create(u8"白方", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 35);
    if (whiteRound) {
        whiteRound->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.height - 35.0f);
        whiteRound->setTextColor(Color4B::WHITE);
        this->addChild(whiteRound, 0);
    }
    else
        cocos2d::log("fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf");

    //创建开始游戏按钮   
    startGameBtn = MenuItemImage::create("startGame.png", "startGame_pressed.png", CC_CALLBACK_1(BoardUI::onStartGameBtn, this));
    if (startGameBtn) {
        startGameBtn->setScale(250.0f / startGameBtn->getContentSize().width, 70.0f / startGameBtn->getContentSize().height);
        startGameBtn->setPosition(Vec2::ZERO);
    }
    else
        cocos2d::log("'startGame.png or startGame_pressed.png'");

    //创建存放开始游戏按钮的菜单，位置在棋盘正上方
    auto startGameMenu = Menu::create(startGameBtn, nullptr);
    if (startGameMenu) {
        startGameMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.width + 45.0f);
        this->addChild(startGameMenu, 0);
    }
    else
        cocos2d::log("'startGameBtn'");
    
    //创建计时器标签，放在“当前回合”标签下面
    timer = Label::create("20", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 50);
    if (timer) {
        timer->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 120.0f);
        timer->setTextColor(Color4B::BLACK);
        this->addChild(timer, 0);
        timer->setVisible(false);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    
    //创建指向黑方的箭头，放在“当前回合”标签左边
    blackRoundArrow = Sprite::create("blackRound.png");
    if (blackRoundArrow) {
        blackRoundArrow->setScale(80.0f / blackRoundArrow->getContentSize().width, 25.0f / blackRoundArrow->getContentSize().height);
        blackRoundArrow->setPosition(origin.x + visibleSize.width / 2 - 200.0f, origin.y + visibleSize.height - 35.0f);
        this->addChild(blackRoundArrow, 0);
        blackRoundArrow->setVisible(false);
    }
    else
        cocos2d::log("blackRound.png");
    
    //创建指向白方的箭头，放在“当前回合”标签右边
    whiteRoundArrow = Sprite::create("whiteRound.png");
    if (whiteRoundArrow) {
        whiteRoundArrow->setScale(80.0f / whiteRoundArrow->getContentSize().width, 25.0f / whiteRoundArrow->getContentSize().height);
        whiteRoundArrow->setPosition(origin.x + visibleSize.width / 2 + 200.0f, origin.y + visibleSize.height - 35.0f);
        this->addChild(whiteRoundArrow, 0);
        whiteRoundArrow->setVisible(false);
    }
    else
        cocos2d::log("whiteRound.png");
    
    //创建获胜方提示，位置在获胜动画上面
    gameOverTip = Label::create("", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 50);
    if (gameOverTip) {
        gameOverTip->setTextColor(Color4B::YELLOW);
        gameOverTip->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 250.0f);
        gameOverTip->setVisible(false);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    
    //创建获胜动画，位置在屏幕正中间
    victoryAnimation = Sprite::create("victory.jpg");
    if (victoryAnimation) {
        victoryAnimation->setScale(316.0f / victoryAnimation->getContentSize().width, 360.5f / victoryAnimation->getContentSize().height);
        victoryAnimation->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);       
        victoryAnimation->setVisible(false);
    }
    else
        cocos2d::log("'victory.jpg'");

    //创建平局动画，位置在屏幕正中间
    drawAnimation = Sprite::create("defeat.jpg");
    if (drawAnimation) {
        drawAnimation->setScale(320.0f / drawAnimation->getContentSize().width, 338.75f / drawAnimation->getContentSize().height);
        drawAnimation->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
        drawAnimation->setVisible(false);
    }
    else
        cocos2d::log("'defeat.jpg'");
    
    //创建游戏结束按钮
    gameOverBtn = MenuItemImage::create("gameOver.png", "gameOver_pressed.png", CC_CALLBACK_1(BoardUI::onCleanBoard, this));
    if (gameOverBtn) {
        gameOverBtn->setPosition(Vec2::ZERO);
        gameOverBtn->setVisible(false);
    }
    else
        cocos2d::log("'gameOver.png or gameOver_pressed.png'");

    //创建平局结束按钮
    gameOverDrawBtn = MenuItemImage::create("gameOverDraw.png", "gameOverDraw_pressed.png", CC_CALLBACK_1(BoardUI::onCleanBoard, this));
    if (gameOverDrawBtn) {        
        gameOverDrawBtn->setPosition(Vec2::ZERO);
        gameOverDrawBtn->setVisible(false);
    }
    else
        cocos2d::log("'gameOverDraw.png or gameOverDraw_pressed.png'");
    
    //创建存放游戏结束按钮的菜单，位置在结算动画下方
    auto gameOverMenu = Menu::create(gameOverBtn, gameOverDrawBtn, nullptr);
    if (gameOverMenu)
        gameOverMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 280.0f);        
    else
        cocos2d::log("'gameOverBtn' or 'gameOverDrawBtn'");

    //创建节点存放游戏结算界面的ui控件
    gameOverUI = Node::create();
    gameOverUI->addChild(gameOverTip, 0);
    gameOverUI->addChild(victoryAnimation, 0);
    gameOverUI->addChild(drawAnimation, 0);
    gameOverUI->addChild(gameOverMenu, 0);

    return true;
}

void BoardUI::setOnReturnCallBack(std::function<void()> callback){
    _onReturn = callback;
}

void BoardUI::onReturnBtn(Ref* pSender) {
    if (_onReturn)
        _onReturn();
}

void BoardUI::setOnToggleEffectCallBack(std::function<void()> callback){
    _onToggleEffect = callback;
}

void BoardUI::onToggleEffectBtn(Ref* pSender) {
    if (_onToggleEffect)
        _onToggleEffect();
}

void BoardUI::setOnTouchBeganCallBack(std::function<bool(Touch*, Event*)> callback){
    _onTouchBegan = callback;
}

bool BoardUI::onTouchBegan(Touch* touch, Event* event) {
    if (_onTouchBegan)
        return _onTouchBegan(touch, event);
    return false;
}

void BoardUI::setOnStartGameCallBack(std::function<void(Ref*)> callback){
    _onStartGame = callback;
}

void BoardUI::onStartGameBtn(Ref* pSender){
    if (_onStartGame)
        _onStartGame(pSender);
}

void BoardUI::setOnCleanBoardCallBack(std::function<void(Ref*)> callback) {
    _onCleanBoard = callback;
}

void BoardUI::onCleanBoard(Ref* pSender){
    if (_onCleanBoard)
        _onCleanBoard(pSender);
}

void BoardUI::getBoardUImember(std::vector<Sprite*>& cs, Sprite*& sH, MenuItemImage*& sGB, Label*& t, Sprite*& bRA, Sprite*& wRA, 
    Label*& gOT, Sprite*& vA, Sprite*& dA, MenuItemImage*& gOB, MenuItemImage*& gODB) const{
    cs = chessSprites;
    sH = selectedHighlight;
    sGB = startGameBtn;
    t = timer;
    bRA = blackRoundArrow;
    wRA = whiteRoundArrow;
    gOT = gameOverTip;
    vA = victoryAnimation;
    dA = drawAnimation;
    gOB = gameOverBtn;
    gODB = gameOverDrawBtn;
}

Node* BoardUI::getGameOverUI() const{        
    return gameOverUI;
}