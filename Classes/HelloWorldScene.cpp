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

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

//背景音乐默认播放
bool HelloWorld::isBgmOn = true;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 3. add your codes below...

    // add a label shows "Hello World"
    /*auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }*/

    // add "HelloWorld" splash screen"
    /*auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }*/

    //预加载背景音乐和落子、输赢音效
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/zhe.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/shui.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/beng.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/de.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/zhu.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/victory.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("music/defeat.mp3");

    //添加背景音乐控制按钮
    bgmBtn = MenuItemImage::create("bgm_btn.png", "bgm_btn.png", CC_CALLBACK_1(HelloWorld::toggleBGM, this));
    if (bgmBtn) {
        bgmBtn->setScale(44.0f / bgmBtn->getContentSize().width, 44.0f / bgmBtn->getContentSize().height);  //设置大小为44px * 44px
        bgmBtn->setPosition(Vec2(22.0f, 22.0f));        //位置放在左下角
    }
    else
        problemLoading("'music/bgm.mp3'");
	
    auto bgmMenu = Menu::create(bgmBtn, nullptr);       //创建菜单并添加按钮
    if (bgmMenu) {
        bgmMenu->setPosition(Vec2::ZERO);               //设置菜单位置为(0,0)
        this->addChild(bgmMenu, 1);
    }
    else
        problemLoading("'bgmBtn'");
    
    //添加背景音乐按钮提示
    auto bgmBtnTip = Label::create(u8"<-点击即可开关背景音乐", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (bgmBtnTip) {
        bgmBtnTip->setPosition(origin.x + 200.0f, origin.y + 22.0f);                //放在背景音乐按钮右边
        bgmBtnTip->setTextColor(Color4B::BLACK);
        this->addChild(bgmBtnTip, 1);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);   //播放背景音乐
	startRotate();                                                                  //让按钮旋转

    //添加落子音效控制按钮
    auto effectBtn = MenuItemImage::create("effect_btn.png", "effect_btn.png", CC_CALLBACK_1(HelloWorld::toggleEffect, this));
    if (effectBtn) {
        effectBtn->setScale(44.0f / effectBtn->getContentSize().width, 44.0f / effectBtn->getContentSize().height);
        effectBtn->setPosition(Vec2::ZERO);
    }
    else
        problemLoading("'effect_btn.png'");
    
    auto effectMenu = Menu::create(effectBtn, nullptr);
    if (effectMenu) {
        effectMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + 22.0f);//位置在底部中间
        this->addChild(effectMenu, 1);
    }
    else
        problemLoading("'effectBtn'");
    
    //添加落子音效按钮提示
    auto effectBtnTip = Label::create(u8"<-点击即可开关落子音效按钮", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (effectBtnTip) {
        effectBtnTip->setPosition(origin.x + visibleSize.width / 2 + 200.0f, origin.y + 22.0f);     //放在音效按钮右边
        effectBtnTip->setTextColor(Color4B::BLACK);
        this->addChild(effectBtnTip, 1);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

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
		problemLoading("'board.png'");

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
        problemLoading("black_zhe.png");
	
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
        problemLoading("chess/black_shui.png");
	
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
        problemLoading("chess/black_beng.png");
	
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
        problemLoading("chess/black_de.png");
    
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
        problemLoading("chess/black_zhu.png");
 
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
        problemLoading("chess/white_zhe.png");
	
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
        problemLoading("chess/white_shui.png");
	
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
        problemLoading("chess/white_beng.png");
	
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
        problemLoading("chess/white_de.png");
	
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
        problemLoading("chess/white_zhu.png");

    //添加黑方棋子选择提示
	auto blackTipLabel = Label::createWithTTF(u8"黑方在上面选择棋子", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (blackTipLabel) {
        blackTipLabel->setPosition(origin.x + 125.0f, origin.y + visibleSize.width - 25.0f);    //位置在5个黑棋正下方
		blackTipLabel->setTextColor(Color4B::BLACK);                                            //字体为黑色
        this->addChild(blackTipLabel, 1);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    
    //添加白方棋子选择提示，位置在5个白棋正下方
    auto whiteTipLabel = Label::createWithTTF(u8"白方在上面选择棋子", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (whiteTipLabel) {
        whiteTipLabel->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.width - 25.0f);
        whiteTipLabel->setTextColor(Color4B::BLACK);
        this->addChild(whiteTipLabel, 1);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    
    //添加点击事件处理
    auto listener = EventListenerTouchOneByOne::create();                       //创建点击事件监听器
    listener->setSwallowTouches(false);                                         //不吞掉点击事件，让其他监听器也能处理该事件
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);     //点击开始回调函数
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);   //将监听器注册到事件分发器

    //添加“当前回合”标签，位置在顶部中间
    auto curRound = Label::create(u8"当前回合", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 50);
    if (curRound) {
        curRound->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 35.0f);
        curRound->setTextColor(Color4B::BLACK);
        this->addChild(curRound, 0);
    }
    else
        problemLoading("fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf");
    
    //添加“黑方”标签，位置在顶部左边
    auto blackRound = Label::create(u8"黑方", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 35);
    if (blackRound) {
        blackRound->setPosition(origin.x + 125.0f, origin.y + visibleSize.height - 35.0f);
        blackRound->setTextColor(Color4B::BLACK);
        this->addChild(blackRound, 0);
    }
    else
        problemLoading("fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf");
    
    //添加“白方”标签，位置在顶部右边
    auto whiteRound = Label::create(u8"白方", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 35);
    if (whiteRound) {
        whiteRound->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.height - 35.0f);
        whiteRound->setTextColor(Color4B::WHITE);
        this->addChild(whiteRound, 0);
    }
    else
        problemLoading("fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf");

    //创建开始游戏按钮   
    startGameBtn = MenuItemImage::create("startGame.png", "startGame_pressed.png", CC_CALLBACK_1(HelloWorld::onStartGame, this));
    if (startGameBtn) {
        startGameBtn->setScale(250.0f / startGameBtn->getContentSize().width, 70.0f / startGameBtn->getContentSize().height);
        startGameBtn->setPosition(Vec2::ZERO);
    }
    else
        problemLoading("'startGame.png or startGame_pressed.png'");
    //创建存放开始游戏按钮的菜单，位置在棋盘正上方
    auto startGameMenu = Menu::create(startGameBtn, nullptr);                                               
    if (startGameMenu) {
        startGameMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.width + 45.0f); 
        this->addChild(startGameMenu, 0);
    }
    else
        problemLoading("'startGameBtn'");

    //创建规则按钮
    auto ruleBtn = MenuItemImage::create("rule.png", "rule.png", CC_CALLBACK_1(HelloWorld::onRuleShow, this));
    if (ruleBtn)
        ruleBtn->setPosition(Vec2::ZERO);
    else
        problemLoading("'rule.png'");
    //创建存放规则按钮的菜单，位置在屏幕左上方
    auto ruleMenu = Menu::create(ruleBtn, nullptr);
    if (ruleMenu) {
        ruleMenu->setPosition(origin.x + 25.0f, origin.y + visibleSize.height - 140.0f);
        this->addChild(ruleMenu, 0);
    }
    else
        problemLoading("'ruleBtn'");

    //创建反馈按钮
    auto suggestBtn = MenuItemImage::create("suggest.png", "suggest.png", CC_CALLBACK_1(HelloWorld::onSuggestShow, this));
    if (suggestBtn) {
        suggestBtn->setScale(96.5f / suggestBtn->getContentSize().width, 33.0f / suggestBtn->getContentSize().height);
        suggestBtn->setPosition(Vec2::ZERO);
    }
    else
        problemLoading("'suggest.png'");
    //创建存放反馈按钮的菜单，位置在屏幕右上方
    auto suggestMenu = Menu::create(suggestBtn, nullptr);
    if (suggestMenu) {
        suggestMenu->setPosition(origin.x + visibleSize.width - 48.25f, origin.y + visibleSize.height - 140.0f);  // 棋盘上方右侧
        this->addChild(suggestMenu, 0);
    }
    else
        problemLoading("'suggestBtn'");

    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);

    SimpleAudioEngine::getInstance()->end();
}

void HelloWorld::toggleBGM(Ref* pSender) {
	//如果背景音乐正在播放，则停止旋转按钮并暂停背景音乐；否则，开始旋转按钮并恢复背景音乐
    if (isBgmOn) {
        stopRotate();
        SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
        isBgmOn = false;
    }
    else {
        startRotate();
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
        isBgmOn = true;
    }
}

void HelloWorld::startRotate() {
    //创建旋转动作（3秒转一圈）并重复执行
    auto rotate = RotateBy::create(3.0f, 360.0f);
    rotateAction = RepeatForever::create(rotate);
    bgmBtn->runAction(rotateAction);
}

void HelloWorld::stopRotate() {
	//停止旋转按钮的旋转动作
    if (rotateAction) {
        bgmBtn->stopAction(rotateAction);
        rotateAction = nullptr;
    }
}

void HelloWorld::toggleEffect(Ref* pSender) {
    isEffectOn = !isEffectOn;       //切换音效开关状态
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event) {
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

void HelloWorld::onSelectChess(Sprite* chessSprite, const std::string& chessName) {
    //黑方回合时不可选择白方棋子，白方回合时不可选择黑方棋子
    if ((isBlackRound && chessName.substr(0, 5) == "white") || (!isBlackRound && chessName.substr(0, 5) == "black"))
        return;
    selectedChessName = chessName;                                          //保存当前被选中棋子的名字
    if (selectedHighlight) {
        selectedHighlight->removeFromParent();                              //移除之前的高亮效果
        selectedHighlight = nullptr;                                        //置空，防止野指针
    }
    selectedHighlight = Sprite::create("highlight.png");                    //创建高亮效果
    if (selectedHighlight) {
        selectedHighlight->setPosition(chessSprite->getPosition());         //和棋子相同位置
        float ScaleX = 70.0f / selectedHighlight->getContentSize().width;
        float ScaleY = 70.0f / selectedHighlight->getContentSize().height;
        selectedHighlight->setScale(ScaleX, ScaleY);                        //设置大小为70px * 70px
        this->addChild(selectedHighlight, 0);
    }
    else
        problemLoading("highlight.png");
}

void HelloWorld::onInitBoardPlacePoint() {
    placePoints.assign(19, std::vector<Sprite*>(19, nullptr));
    canPlace.assign(19, std::vector<bool>(19, true));       //一开始棋盘上无棋子，所有点均可放置
    boardChesses.assign(19, std::vector<Sprite*>(19, nullptr));
    for (size_t row = 0; row < 19; row++)
        for (size_t col = 0; col < 19; col++) {
            auto pp = Sprite::create("placePoint.png");
            if (pp) {
                //从左下角到右上角计算可放置点位置
                pp->setPosition(origin.x + 75.0f + col * 50.0f, origin.y + 75.0f + row * 50.0f);
                pp->setScale(35.0f / pp->getContentSize().width, 35.0f / pp->getContentSize().height);
                pp->setOpacity(200);
                pp->setVisible(false);          //全部放置点隐藏
                this->addChild(pp, 1);
                placePoints[row][col] = pp;     //保存在数组里
            }
            else
                problemLoading("placePoint.png");
        }
}

bool HelloWorld::onPlaceChess(Vec2 touchPos) {
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
                                gameOver();                                         //如果获胜，调用游戏结束函数
                            roundSurplusTime = 0;                                   //回合时间清零，即切换回合
                        }
                        else
                            problemLoading("chess.png");
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

void HelloWorld::onStartGame(Ref* pSender){
    startGameBtn->setVisible(false);                 //按钮隐藏，代表已开始游戏

    if (!timer) {
        //创建计时器标签，放在“当前回合”标签下面
        timer = Label::create("20", "fonts/arial.ttf", 50);                                                 
        if (timer) {
            timer->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 120.0f);   
            timer->setTextColor(Color4B::BLACK);
            this->addChild(timer, 0);
        }
        else
            problemLoading("'fonts/arial.ttf'");
    }
    if (!blackRoundArrow) {
        //创建指向黑方的箭头，放在“当前回合”标签左边
        blackRoundArrow = Sprite::create("blackRound.png");     
        if (blackRoundArrow) {
            blackRoundArrow->setScale(80.0f / blackRoundArrow->getContentSize().width, 25.0f / blackRoundArrow->getContentSize().height);            
            blackRoundArrow->setPosition(origin.x + visibleSize.width / 2 - 200.0f, origin.y + visibleSize.height - 35.0f);
            this->addChild(blackRoundArrow, 0);            
        }
        else
            problemLoading("blackRound.png");
    }
    if (!whiteRoundArrow) {
        //创建指向白方的箭头，放在“当前回合”标签右边
        whiteRoundArrow = Sprite::create("whiteRound.png");     
        if (whiteRoundArrow) {
            whiteRoundArrow->setScale(80.0f / whiteRoundArrow->getContentSize().width, 25.0f / whiteRoundArrow->getContentSize().height);
            whiteRoundArrow->setPosition(origin.x + visibleSize.width / 2 + 200.0f, origin.y + visibleSize.height - 35.0f);
            this->addChild(whiteRoundArrow, 0);
        }
        else
            problemLoading("whiteRound.png");
    }
    
    isGamePlaying = true;                           //表示正在游戏中
    roundSurplusTime = 20.9f;                       //回合时间20秒左右
    isBlackRound = true;                            //默认第一回合是黑方
    blackRoundArrow->setVisible(true);
    whiteRoundArrow->setVisible(false);             //指向白方的箭头先隐藏
    timer->setVisible(true);                        //显示计时器

    this->scheduleUpdate();                         //启动帧循环，每帧自动调用 update(float dt)

    if (placePoints.empty())                        
        onInitBoardPlacePoint();                    //第一回合开始时初始化棋盘放置点
    onSelectChess(chessSprites[0], "black_zhe");    //自动选中第一个黑棋
}

void HelloWorld::update(float dt) {
    if (!isGamePlaying)
        return;                                     //不在游戏中不处理更新逻辑
    
    roundSurplusTime -= dt;                         //更新当前回合剩余时间
    if (roundSurplusTime <= 0) {                    //回合时间到
        if (lastChessSum == curChessSum) {          //如果玩家未落子
            if (selectedPlacePoint)                 //如果有选中放置点，自动落子
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

bool HelloWorld::isVictory(int row, int col) {
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

bool HelloWorld::searchBoardChesses(int row, int col, Direction dir_1, Direction dir_2, std::unordered_map<std::string, int> needChesses){        
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

void HelloWorld::gameOver(){
    if (!victoryTip) {
        //创建获胜方提示，位置在获胜动画上面
        victoryTip = Label::create("", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 50);
        if (victoryTip) {
            victoryTip->setTextColor(Color4B::YELLOW);
            victoryTip->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 250.0f);
            this->addChild(victoryTip, 2);
        }
        else
            problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    }
    //根据获胜方的不同，提示文本也不同
    if (isBlackRound)
        victoryTip->setString(u8"黑方获胜!");
    else
        victoryTip->setString(u8"白方获胜!");
    
    if (!victoryAnimation) {
        //创建获胜动画，位置在屏幕正中间
        victoryAnimation = Sprite::create("victory.jpg");
        if (victoryAnimation) {
            victoryAnimation->setScale(316.0f / victoryAnimation->getContentSize().width, 360.5f / victoryAnimation->getContentSize().height);
            victoryAnimation->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
            this->addChild(victoryAnimation, 2);
        }
        else
            problemLoading("'victory.jpg'");
    }
    if(isEffectOn)
        SimpleAudioEngine::getInstance()->playEffect("music/victory.mp3");  //播放获胜音效

    if (!gameOverBtn) {
        //创建游戏结束按钮
        gameOverBtn = MenuItemImage::create("gameOver.png", "gameOver_pressed.png", CC_CALLBACK_1(HelloWorld::cleanBoard, this));
        if (gameOverBtn) {
            gameOverBtn->setScale(250.0f / gameOverBtn->getContentSize().width, 80.0f / gameOverBtn->getContentSize().height);
            gameOverBtn->setPosition(Vec2::ZERO);
        }
        else
            problemLoading("'gameOver.png or gameOver_pressed.png'");
        //创建存放游戏结束按钮的菜单，位置在获胜动画下方
        auto gameOverMenu = Menu::create(gameOverBtn, nullptr);
        if (gameOverMenu) {
            gameOverMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 280.0f);
            this->addChild(gameOverMenu, 2);
        }
        else
            problemLoading("'gameOverBtn'");
    }
        
    isGamePlaying = false;                      //游戏结束
    timer->setVisible(false);                   //计时器隐藏
    blackRoundArrow->setVisible(false);         //黑方箭头隐藏
    whiteRoundArrow->setVisible(false);         //白方箭头隐藏
    if (selectedHighlight) {
        selectedHighlight->removeFromParent();  //高亮移除
        selectedHighlight = nullptr;
    }
    selectedChessName = "";                     //选中棋子名字重置
    selectedPlacePoint = nullptr;               //选中放置点重置
    victoryTip->setVisible(true);
    victoryAnimation->setVisible(true);         //显示获胜动画
    gameOverBtn->setVisible(true);              //显示结束游戏按钮
}

void HelloWorld::cleanBoard(Ref* pSender) {
    lastChessSum = 0; curChessSum = 0;                          //棋盘上棋子总数清零
    for(int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++)
            if (boardChesses[row][col]) {
                boardChesses[row][col]->removeFromParent();     //删除所有保存的棋子
                boardChesses[row][col] = nullptr;
                canPlace[row][col] = true;                      //恢复可放置的状态
            }
    victoryTip->setVisible(false);
    victoryAnimation->setVisible(false);                        //隐藏获胜动画
    gameOverBtn->setVisible(false);                             //隐藏结束游戏按钮
    startGameBtn->setVisible(true);                             //显示开始游戏按钮，为下一次游戏做准备
}

void HelloWorld::onRuleShow(Ref* pSender) {
    std::string title = u8"游戏规则";
    std::string content =
        u8"黑方和白方各有5个棋子，分别写着：\n"
        "这、谁、绷、得、住\n"
        "双方轮流落子，每次只能落一个棋子。\n"
        "连成 \"这谁绷得住\" 5个不同字的一方获胜！\n"
        "可以不按顺序，回合限时20秒，超时自动落子。";

    createPopup(title, content);
}

void HelloWorld::onSuggestShow(Ref* pSender){
    std::string title = u8"建议反馈";
    std::string content =
        u8"作者：ZhengQianXu\n"
        "邮箱：2059984809@qq.com\n"
        "Github：https://github.com/ZhengQianXu\n"
        "如有任何建议或问题，欢迎联系作者！\n"
        "感谢您的支持！";

    createPopup(title, content);
}

void HelloWorld::closePopup(Ref* pSender){
    if (popup) {
        popup->removeFromParent();
        popup = nullptr;
    }
    if (popupMask) {
        popupMask->removeFromParent();
        popupMask = nullptr;
    }
}

void HelloWorld::createPopup(const std::string& title, const std::string& content) {
    //创建半透明遮罩层，遮罩层会覆盖整个屏幕，让主场景变暗
    auto mask = LayerColor::create(Color4B(0, 0, 0, 150));
    mask->setContentSize(visibleSize);                                      //铺满屏幕
    mask->setPosition(origin);                                              //从屏幕左下角开始
    this->addChild(mask, 10);
    popupMask = mask;                                                       //保存指针，方便关闭时移除

    //遮罩层拦截所有触摸事件，防止玩家在弹窗打开时点击到主场景的按钮或棋子
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);                                      //吞掉事件，不传递到主场景
    listener->onTouchBegan = [](Touch* touch, Event* event) -> bool {
        return true;                                                        //拦截所有触摸，消费掉事件
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mask);

    //创建弹窗主体（白色背景）
    popup = ui::Layout::create();
    popup->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);  //纯色背景
    popup->setBackGroundColor(Color3B::WHITE);                              //白色背景
    popup->setBackGroundColorOpacity(255);                                  //不透明
    popup->setContentSize(Size(600, 300));                                  //弹窗宽600，高300
    //位置：屏幕中心偏移半个弹窗大小（因为锚点在左下角）
    popup->setPosition(Vec2(origin.x + visibleSize.width / 2 - 300.0f, origin.y + visibleSize.height / 2 - 200.0f));
    popup->setAnchorPoint(Vec2::ZERO);                                      //锚点在左下角
    popup->setTouchEnabled(true);                                           //允许弹窗接收触摸，防止点击穿透
    popup->setCascadeOpacityEnabled(true);                                  //子节点继承父节点透明度
    mask->addChild(popup, 1);                                               //添加到遮罩层之上

    //创建标题文字
    auto titleLabel = Label::create(title, "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 32);
    if (titleLabel) {
        titleLabel->setTextColor(Color4B::BLACK);
        titleLabel->setPosition(popup->getContentSize().width / 2, popup->getContentSize().height - 40.0f);//标题在弹窗顶部居中
        popup->addChild(titleLabel);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //创建内容文字
    auto contentLabel = Label::create(content, "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 22);
    if (contentLabel) {
        contentLabel->setTextColor(Color4B::BLACK);
        contentLabel->setPosition(popup->getContentSize().width / 2, popup->getContentSize().height / 2 - 20.0f);
        contentLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);        //文字在区域内水平垂直居中
        popup->addChild(contentLabel);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //创建关闭按钮
    auto closeBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(HelloWorld::closePopup, this));
    if (closeBtn)
        closeBtn->setPosition(Vec2::ZERO);
    else
        problemLoading("'CloseNormal.png or CloseSelected.png'");
    //创建存放关闭按钮的菜单，位置在弹窗右上角
    auto closeMenu = Menu::create(closeBtn, nullptr);
    if (closeMenu) {        
        closeMenu->setPosition(popup->getContentSize().width - 30, popup->getContentSize().height - 30);
        popup->addChild(closeMenu, 1);
    }
    else
        problemLoading("closeBtn");
}