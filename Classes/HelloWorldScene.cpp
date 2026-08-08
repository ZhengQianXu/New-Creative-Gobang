#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

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
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
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

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
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
    }

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
    bgmBtn->setScale(0.3f, 0.3f);
    bgmBtn->setPosition(Vec2(22, 22));
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
        float scaleX = visibleSize.width / boardWidth;
        float scaleY = scaleX;
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
    auto black_zhe = Sprite::create("black_zhe.png");
    if (black_zhe) {
        black_zhe->setScale(50.0 / black_zhe->getContentSize().width, 50.0 / black_zhe->getContentSize().height);
        black_zhe->setPosition(origin.x + 25.0, origin.y + visibleSize.width + 25.0);
        this->addChild(black_zhe, 0);
    }
    else
        problemLoading("black_zhe.png");
    auto black_shui = Sprite::create("black_shui.png");
    if (black_shui) {
        black_shui->setScale(50.0 / black_shui->getContentSize().width, 50.0 / black_shui->getContentSize().height);
        black_shui->setPosition(origin.x + 75.0, origin.y + visibleSize.width + 25.0);
        this->addChild(black_shui, 0);
    }
    else
        problemLoading("black_shui.png");
    auto black_beng = Sprite::create("black_beng.png");
    if (black_beng) {
        black_beng->setScale(50.0 / black_beng->getContentSize().width, 50.0 / black_beng->getContentSize().height);
        black_beng->setPosition(origin.x + 125.0, origin.y + visibleSize.width + 25.0);
        this->addChild(black_beng, 0);
    }
    else
        problemLoading("black_beng.png");
    auto black_de = Sprite::create("black_de.png");
    if (black_de) {
        black_de->setScale(50.0 / black_de->getContentSize().width, 50.0 / black_de->getContentSize().height);
        black_de->setPosition(origin.x + 175.0, origin.y + visibleSize.width + 25.0);
        this->addChild(black_de, 0);
    }
    else
        problemLoading("black_de.png");
    auto black_zhu = Sprite::create("black_zhu.png");
    if (black_zhu) {
        black_zhu->setScale(50.0 / black_zhu->getContentSize().width, 50.0 / black_zhu->getContentSize().height);
        black_zhu->setPosition(origin.x + 225.0, origin.y + visibleSize.width + 25.0);
        this->addChild(black_zhu, 0);
    }
    else
        problemLoading("black_zhu.png");

    //添加白方棋子,按“这谁绷得住”次序添加，白底黑字，显示在棋盘右上方
    auto white_zhe = Sprite::create("white_zhe.png");
    if (white_zhe) {
        white_zhe->setScale(50.0 / white_zhe->getContentSize().width, 50.0 / white_zhe->getContentSize().height);
        white_zhe->setPosition(origin.x + visibleSize.width - 225.0, origin.y + visibleSize.width + 25.0);
        this->addChild(white_zhe, 0);
    }
    else
        problemLoading("white_zhe.png");
    auto white_shui = Sprite::create("white_shui.png");
    if (white_shui) {
        white_shui->setScale(50.0 / white_shui->getContentSize().width, 50.0 / white_shui->getContentSize().height);
        white_shui->setPosition(origin.x + visibleSize.width - 175.0, origin.y + visibleSize.width + 25.0);
        this->addChild(white_shui, 0);
    }
    else
        problemLoading("white_shui.png");
    auto white_beng = Sprite::create("white_beng.png");
    if (white_beng) {
        white_beng->setScale(50.0 / white_beng->getContentSize().width, 50.0 / white_beng->getContentSize().height);
        white_beng->setPosition(origin.x + visibleSize.width - 125.0, origin.y + visibleSize.width + 25.0);
        this->addChild(white_beng, 0);
    }
    else
        problemLoading("white_beng.png");
    auto white_de = Sprite::create("white_de.png");
    if (white_de) {
        white_de->setScale(50.0 / white_de->getContentSize().width, 50.0 / white_de->getContentSize().height);
        white_de->setPosition(origin.x + visibleSize.width - 75.0, origin.y + visibleSize.width + 25.0);
        this->addChild(white_de, 0);
    }
    else
        problemLoading("white_de.png");
    auto white_zhu = Sprite::create("white_zhu.png");
    if (white_zhu) {
        white_zhu->setScale(50.0 / white_zhu->getContentSize().width, 50.0 / white_zhu->getContentSize().height);
        white_zhu->setPosition(origin.x + visibleSize.width - 25.0, origin.y + visibleSize.width + 25.0);
        this->addChild(white_zhu, 0);
    }
    else
        problemLoading("white_zhu.png");

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
    //创建旋转动作（2秒转一圈）并重复执行
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