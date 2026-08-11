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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

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

    // 添加背景音乐
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("bgm.mp3");    //预加载背景音乐

    // 添加背景音乐控制按钮
    bgmBtn = MenuItemImage::create("bgm_btn.png", "bgm_btn.png", CC_CALLBACK_1(HelloWorld::toggleBGM, this));
    bgmBtn->setScale(44.0f / bgmBtn->getContentSize().width, 44.0f / bgmBtn->getContentSize().height);  //设置大小为44px * 44px
    bgmBtn->setPosition(Vec2(22, 22));          //位置放在左下角
	auto me = Menu::create(bgmBtn, nullptr);    //创建菜单并添加按钮
	me->setPosition(Vec2::ZERO);                //设置菜单位置为(0,0)
    this->addChild(me, 1);
    SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);     //播放背景音乐
	startRotate();                              //让按钮旋转

	// 添加木纹色背景
    auto bgLayer = LayerColor::create(Color4B(181, 136, 99, 255));
    this->addChild(bgLayer, -1);    //放在最底层

    // 添加棋盘图片
    auto board = Sprite::create("board.png");
    if (board) {
		//获取棋盘图片的宽高
		float boardWidth = board->getContentSize().width;
		float boardHeight = board->getContentSize().height;
		//根据屏幕宽度和棋盘图片宽度计算并设置缩放比例
        float scaleX = visibleSize.width / boardWidth;      //宽度占满
        float scaleY = scaleX;                              //长宽一致
        board->setScale(scaleX, scaleY);
		//设置棋盘图片位置为屏幕下方中心
		float posX = origin.x + visibleSize.width / 2;
        float posY = origin.y + boardHeight * scaleY / 2;
        board->setPosition(Vec2(posX, posY));
		//添加棋盘图片到场景中
        this->addChild(board, 0);
    }
    else
		problemLoading("'board.png'");

    //添加黑方棋子,按“这谁绷得住”次序添加，黑底白字，显示在棋盘左上方
    //创建黑方棋子 "这"
    auto black_zhe = Sprite::create("black_zhe.png");
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
    auto black_shui = Sprite::create("black_shui.png");
    if (black_shui) {
        black_shui->setScale(50.0f / black_shui->getContentSize().width, 50.0f / black_shui->getContentSize().height);
        black_shui->setPosition(origin.x + 75.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_shui, 0);
        black_shui->setName("black_shui");
        chessSprites.push_back(black_shui);
    }
    else
        problemLoading("black_shui.png");
	//创建黑方棋子 "绷"
    auto black_beng = Sprite::create("black_beng.png");
    if (black_beng) {
        black_beng->setScale(50.0f / black_beng->getContentSize().width, 50.0f / black_beng->getContentSize().height);
        black_beng->setPosition(origin.x + 125.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_beng, 0);
        black_beng->setName("black_beng");
        chessSprites.push_back(black_beng);
    }
    else
        problemLoading("black_beng.png");
	//创建黑方棋子 "得"
    auto black_de = Sprite::create("black_de.png");
    if (black_de) {
        black_de->setScale(50.0f / black_de->getContentSize().width, 50.0f / black_de->getContentSize().height);
        black_de->setPosition(origin.x + 175.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_de, 0);
        black_de->setName("black_de");
        chessSprites.push_back(black_de);
    }
    else
        problemLoading("black_de.png");
    //创建黑方棋子 "住"
    auto black_zhu = Sprite::create("black_zhu.png");
    if (black_zhu) {
        black_zhu->setScale(50.0f / black_zhu->getContentSize().width, 50.0f / black_zhu->getContentSize().height);
        black_zhu->setPosition(origin.x + 225.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(black_zhu, 0);
        black_zhu->setName("black_zhu");
        chessSprites.push_back(black_zhu);
    }
    else
        problemLoading("black_zhu.png");

    //添加白方棋子,按“这谁绷得住”次序添加，白底黑字，显示在棋盘右上方
	//创建白方棋子 "这"
    auto white_zhe = Sprite::create("white_zhe.png");
    if (white_zhe) {
        white_zhe->setScale(50.0f / white_zhe->getContentSize().width, 50.0f / white_zhe->getContentSize().height);
        //位置为棋盘右上方靠左第一个
        white_zhe->setPosition(origin.x + visibleSize.width - 225.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_zhe, 0);
        white_zhe->setName("white_zhe");
        chessSprites.push_back(white_zhe);
    }
    else
        problemLoading("white_zhe.png");
	//创建白方棋子 "谁"
    auto white_shui = Sprite::create("white_shui.png");
    if (white_shui) {
        white_shui->setScale(50.0f / white_shui->getContentSize().width, 50.0f / white_shui->getContentSize().height);
        white_shui->setPosition(origin.x + visibleSize.width - 175.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_shui, 0);
        white_shui->setName("white_shui");
        chessSprites.push_back(white_shui);
    }
    else
        problemLoading("white_shui.png");
	//创建白方棋子 "绷"
    auto white_beng = Sprite::create("white_beng.png");
    if (white_beng) {
        white_beng->setScale(50.0f / white_beng->getContentSize().width, 50.0f / white_beng->getContentSize().height);
        white_beng->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_beng, 0);
        white_beng->setName("white_beng");
        chessSprites.push_back(white_beng);
    }
    else
        problemLoading("white_beng.png");
	//创建白方棋子 "得"
    auto white_de = Sprite::create("white_de.png");
    if (white_de) {
        white_de->setScale(50.0f / white_de->getContentSize().width, 50.0f / white_de->getContentSize().height);
        white_de->setPosition(origin.x + visibleSize.width - 75.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_de, 0);
        white_de->setName("white_de");
        chessSprites.push_back(white_de);
    }
    else
        problemLoading("white_de.png");
	//创建白方棋子 "住"
    auto white_zhu = Sprite::create("white_zhu.png");
    if (white_zhu) {
        white_zhu->setScale(50.0f / white_zhu->getContentSize().width, 50.0f / white_zhu->getContentSize().height);
        white_zhu->setPosition(origin.x + visibleSize.width - 25.0f, origin.y + visibleSize.width + 25.0f);
        this->addChild(white_zhu, 0);
        white_zhu->setName("white_zhu");
        chessSprites.push_back(white_zhu);
    }
    else
        problemLoading("white_zhu.png");

    //添加棋子选择提示
	auto blackTipLabel = Label::createWithTTF(u8"黑方在上面选择棋子", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (blackTipLabel) {
        blackTipLabel->setPosition(origin.x + 125.0f, origin.y + visibleSize.width - 25.0f);    //位置在5个黑棋正下方
		blackTipLabel->setTextColor(Color4B::BLACK);                                            //字体为黑色
        this->addChild(blackTipLabel, 1);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    auto whiteTipLabel = Label::createWithTTF(u8"白方在上面选择棋子", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (whiteTipLabel) {
        whiteTipLabel->setPosition(origin.x + visibleSize.width - 125.0f, origin.y + visibleSize.width - 25.0f);
        whiteTipLabel->setTextColor(Color4B::BLACK);
        this->addChild(whiteTipLabel, 1);
    }
    else
        problemLoading("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");
    
    auto listener = EventListenerTouchOneByOne::create();                       //创建点击事件监听器
    listener->setSwallowTouches(false);                                         //不吞掉点击事件，让其他监听器也能处理该事件
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);     //点击开始回调函数
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);     //点击结束回调函数
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);   //将监听器注册到事件分发器
    
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

bool HelloWorld::onTouchBegan(Touch* touch, Event* event) {
    Vec2 touchPos = touch->getLocation();                       //获取点击的位置
    for (auto& chess : chessSprites)
        if (chess->getBoundingBox().containsPoint(touchPos)) {  //判断是否有棋子被点击了
            if(selectedChessName.empty())                       //如果第一次选中棋子
                onInitBoardPlacePoint();                        //初始化棋盘放置点
            onSelectChess(chess, chess->getName());             //调用选中棋子函数
            return true;                                        //消费掉这个点击事件
        }
    if (!selectedChessName.empty())                             //如果有棋子被选中
        for (int row = 0; row < 19; row++)
            for (int col = 0; col < 19; col++)
                //如果当前点可放置
                if (canPlace[row][col] && placePoints[row][col]->getBoundingBox().containsPoint(touchPos)) {
                    if (selectedPlacePoint) {                                           //如果已存在选中放置点
                        if (selectedPlacePoint == placePoints[row][col]) {              //检查是否是当前放置点
                            auto chess = Sprite::create(selectedChessName + ".png");    //由选中棋子名字生成对应棋子
                            if (chess) {
                                chess->setPosition(selectedPlacePoint->getPosition());  //棋子位置与放置点一致
                                chess->setScale(50.0f / chess->getContentSize().width, 50.0f / chess->getContentSize().height);
                                this->addChild(chess, 1);
                                selectedPlacePoint->setVisible(false);                  //放置点隐藏
                                selectedPlacePoint = nullptr;                           //置空，防止野指针
                                canPlace[row][col] = false;                             //当前点已有棋子，表示不可放置
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

void HelloWorld::onTouchEnded(Touch* touch, Event* event) {
    
}

void HelloWorld::onSelectChess(Sprite* chessSprite, const std::string& chessName) {
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
    for (int row = 0; row < 19; row++)
        for (int col = 0; col < 19; col++) {
            auto pp = Sprite::create("placePoint.png");
            if (pp) {
                //从左下角到右上角计算可放置点位置
                pp->setPosition(75.0f + col * 50.0f, 75.0f + row * 50.0f);
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