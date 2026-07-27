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

    // Ìí¼Ó±³¾°ÒôÀÖ
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("bgm.mp3");    //Ô¤¼ÓÔØ±³¾°ÒôÀÖ
    playBGM();                                                              //²¥·Å±³¾°ÒôÀÖ

	// Ìí¼ÓÄ¾ÎÆÉ«±³¾°
    auto bgLayer = LayerColor::create(Color4B(181, 136, 99, 255));
    this->addChild(bgLayer, -1);    //·ÅÔÚ×îµ×²ã

    // Ìí¼ÓÆåÅÌÍ¼Æ¬
    auto board = Sprite::create("board.png");
    if (board) {
		//»ñÈ¡ÆåÅÌÍ¼Æ¬µÄ¿í¸ß
		float boardWidth = board->getContentSize().width;
		float boardHeight = board->getContentSize().height;
		//¸ù¾ÝÆÁÄ»¿í¶ÈºÍÆåÅÌÍ¼Æ¬¿í¶È¼ÆËã²¢ÉèÖÃËõ·Å±ÈÀý
        float scaleX = visibleSize.width / boardWidth;
        float scaleY = scaleX;
        board->setScale(scaleX, scaleY);
		//ÉèÖÃÆåÅÌÍ¼Æ¬Î»ÖÃÎªÆÁÄ»ÏÂ·½ÖÐÐÄ
		float posX = origin.x + visibleSize.width / 2;
        float posY = origin.y + boardHeight * scaleY / 2;
        board->setPosition(Vec2(posX, posY));
		//Ìí¼ÓÆåÅÌÍ¼Æ¬µ½³¡¾°ÖÐ
        this->addChild(board, 0);
    }
    else
		problemLoading("'board.png'");

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

void HelloWorld::playBGM() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", false);
    this->scheduleOnce([this](float dt) {
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        this->playBGM();
    }, 36.5f, "loop_bgm");
}