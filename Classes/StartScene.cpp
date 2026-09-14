#include "StartScene.h"
#include "MusicControl.h"
#include "SimpleAudioEngine.h"
#include "LocalModeScene.h"
#include "OnlineModeScene.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* StartScene::createScene() {
	return StartScene::create();
}

bool StartScene::init() {
	if (!Scene::init())
		return false;

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//添加木纹色背景
	auto bgLayer = LayerColor::create(Color4B(181, 136, 99, 255));
	this->addChild(bgLayer, -1);    //放在最底层

    //添加背景音乐控制模块（包含按钮和提示）
	MusicControl* mc = MusicControl::create();
	this->addChild(mc, 0);

    //创建规则按钮
    auto ruleBtn = MenuItemImage::create("rule.png", "rule.png", CC_CALLBACK_1(StartScene::onRuleShow, this));
    if (ruleBtn)
        ruleBtn->setPosition(Vec2::ZERO);
    else
        cocos2d::log("'rule.png'");
    
    //创建存放规则按钮的菜单，位置在屏幕左上方
    auto ruleMenu = Menu::create(ruleBtn, nullptr);
    if (ruleMenu) {
        ruleMenu->setPosition(origin.x + 25.0f, origin.y + visibleSize.height - 140.0f);
        this->addChild(ruleMenu, 0);
    }
    else
        cocos2d::log("'ruleBtn'");

    //创建反馈按钮
    auto suggestBtn = MenuItemImage::create("suggest.png", "suggest.png", CC_CALLBACK_1(StartScene::onSuggestShow, this));
    if (suggestBtn) {
        suggestBtn->setScale(96.5f / suggestBtn->getContentSize().width, 33.0f / suggestBtn->getContentSize().height);
        suggestBtn->setPosition(Vec2::ZERO);
    }
    else
        cocos2d::log("'suggest.png'");
    
    //创建存放反馈按钮的菜单，位置在屏幕右上方
    auto suggestMenu = Menu::create(suggestBtn, nullptr);
    if (suggestMenu) {
        suggestMenu->setPosition(origin.x + visibleSize.width - 48.25f, origin.y + visibleSize.height - 140.0f);  // 棋盘上方右侧
        this->addChild(suggestMenu, 0);
    }
    else
        cocos2d::log("'suggestBtn'");

    //创建标题，作为游戏欢迎
    auto Title = Label::create(u8"欢迎来到“这谁绷得住”", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 60);
    if (Title) {
        Title->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 80.0f);
        Title->setTextColor(Color4B::BLACK);
        this->addChild(Title, 0);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //主界面动画
    auto Animation = Sprite::create("victory.jpg");
    if (Animation) {
        Animation->setScale(316.0f / Animation->getContentSize().width, 360.5f / Animation->getContentSize().height);
        Animation->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 250.0f);
        this->addChild(Animation, 0);
    }
    else
        cocos2d::log("'victory.jpg'");

    auto listener = EventListenerTouchOneByOne::create();                       //创建点击监听
    listener->setSwallowTouches(false);                                         //不吞掉点击事件
    listener->onTouchBegan = [=](Touch* touch, Event* event) -> bool {
        auto touchPos = touch->getLocation();
        if (Animation->getBoundingBox().containsPoint(touchPos)) {
            SimpleAudioEngine::getInstance()->playEffect("music/victory.mp3");  //如果点击到动画，则触发对应音效
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);   //将监听器添加到事件分发器

    //添加本地对战模式入口
    auto localModeBtn = MenuItemImage::create("localMode.png", "localMode_pressed.png", CC_CALLBACK_1(StartScene::createLocalMode, this));
    if (localModeBtn)
        localModeBtn->setPosition(Vec2::ZERO);        
    else
        cocos2d::log("“localMode.png or localMode_pressed.png”");
    
    //创建存放本地对战模式按钮的菜单，位置在动画下方
    auto localModeMenu = Menu::create(localModeBtn, nullptr);
    if (localModeMenu) {
        localModeMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 100.0f);
        this->addChild(localModeMenu, 0);
    }
    else
        cocos2d::log("“localModeBtn”");

    //添加双人联机模式入口
    auto onlineModeBtn = MenuItemImage::create("onlineMode.png", "onlineMode_pressed.png", CC_CALLBACK_1(StartScene::createOnlineMode, this));
    if (onlineModeBtn)
        onlineModeBtn->setPosition(Vec2::ZERO);        
    else
        cocos2d::log("“onlineMode.png or onlineMode_pressed.png”");
    
    //创建存放双人联机模式按钮的菜单，位置在本地对战模式按钮下方
    auto onlineModeMenu = Menu::create(onlineModeBtn, nullptr);
    if (onlineModeMenu) {
        onlineModeMenu->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 250.0f);
        this->addChild(onlineModeMenu, 0);
    }
    else
        cocos2d::log("“onlineModeBtn”");

    //添加关闭游戏按钮
    auto closeGameBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(StartScene::closeStartScene, this));
    if (closeGameBtn) {
        closeGameBtn->setPosition(Vec2::ZERO);
        closeGameBtn->setScale(44.0f / closeGameBtn->getContentSize().width, 44.0f / closeGameBtn->getContentSize().height);
    }
    else
        cocos2d::log("“CloseNormal.png or CloseSelected.png”");
    
    //创建存放关闭游戏按钮的菜单，位置在屏幕右下角
    auto closeGameMenu = Menu::create(closeGameBtn, nullptr);
    if (closeGameMenu) {
        closeGameMenu->setPosition(origin.x + visibleSize.width - 22.0f, origin.y + 22.0f);
        this->addChild(closeGameMenu, 1);
    }
    else
        cocos2d::log("“closeItem”");
    
    //创建关闭游戏提示，位置在按钮左边
    auto closeTip = Label::create(u8"点击即可退出游戏->", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (closeTip) {
        closeTip->setPosition(origin.x + visibleSize.width - 160.0f, origin.y + 22.0f);
        closeTip->setTextColor(Color4B::BLACK);
        this->addChild(closeTip, 0);
    }
    else
        cocos2d::log("“fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf”");

	return true;
}

void StartScene::onRuleShow(Ref* pSender) {
    std::string title = u8"游戏规则";
    std::string content =
        u8"黑方和白方各有5个棋子，分别写着：\n"
        "这、谁、绷、得、住\n"
        "双方轮流落子，每次只能落一个棋子。\n"
        "连成 \"这谁绷得住\" 5个不同字的一方获胜！\n"
        "可以不按顺序，回合限时20秒，超时自动落子。";

    createPopup(title, content);
}

void StartScene::onSuggestShow(Ref* pSender) {
    std::string title = u8"建议反馈";
    std::string content =
        u8"作者：ZhengQianXu\n"
        "邮箱：2059984809@qq.com\n"
        "Github：https://github.com/ZhengQianXu\n"
        "如有任何建议或问题，欢迎联系作者！\n"
        "感谢您的支持！";

    createPopup(title, content);
}

void StartScene::closePopup(Ref* pSender) {
    if (popup) {
        popup->removeFromParent();
        popup = nullptr;
    }
    if (popupMask) {
        popupMask->removeFromParent();
        popupMask = nullptr;
    }
}

void StartScene::createPopup(const std::string& title, const std::string& content, bool isOnline) {
    //创建半透明遮罩层，遮罩层会覆盖整个屏幕，让主场景变暗
    popupMask = LayerColor::create(Color4B(0, 0, 0, 150));
    popupMask->setContentSize(visibleSize);                                 //铺满屏幕
    popupMask->setPosition(origin);                                         //从屏幕左下角开始
    this->addChild(popupMask, 10);
    
    //遮罩层拦截所有触摸事件，防止玩家在弹窗打开时点击到主场景的按钮或棋子
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);                                      //拦截所有触摸，吞掉事件，不传递到主场景
    listener->onTouchBegan = [=](Touch* touch, Event* event) -> bool {
        return true;                                                        //消费掉事件
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, popupMask);

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
    popupMask->addChild(popup, 1);                                          //添加到遮罩层之上

    //创建标题文字，位置在弹窗顶部居中
    auto titleLabel = Label::create(title, "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 32);
    if (titleLabel) {
        titleLabel->setTextColor(Color4B::BLACK);
        titleLabel->setPosition(origin.x + popup->getContentSize().width / 2, origin.y + popup->getContentSize().height - 40.0f);
        popup->addChild(titleLabel);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //创建内容文字
    auto contentLabel = Label::create(content, "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 22);
    if (contentLabel) {
        contentLabel->setTextColor(Color4B::BLACK);
		//如果是联机模式弹窗，内容文字位置在弹窗顶部偏下；如果是规则或反馈弹窗，内容文字位置在弹窗正中间偏下
        if(isOnline)
            contentLabel->setPosition(origin.x + popup->getContentSize().width / 2, origin.y + popup->getContentSize().height - 100.0f);
        else
            contentLabel->setPosition(origin.x + popup->getContentSize().width / 2, origin.y + popup->getContentSize().height / 2 - 20.0f);        
        contentLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);        //文字在区域内水平垂直居中
        popup->addChild(contentLabel);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //创建关闭弹窗按钮
    auto closeBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(StartScene::closePopup, this));
    if (closeBtn)
        closeBtn->setPosition(Vec2::ZERO);
    else
        cocos2d::log("'CloseNormal.png or CloseSelected.png'");
    
    //创建存放关闭按钮的菜单，位置在弹窗右上角
    auto closeMenu = Menu::create(closeBtn, nullptr);
    if (closeMenu) {
        closeMenu->setPosition(origin.x + popup->getContentSize().width - 30, origin.y + popup->getContentSize().height - 30);
        popup->addChild(closeMenu, 1);
    }
    else
        cocos2d::log("closeBtn");

    if (!isOnline)
		return;         //如果不是联机模式弹窗，则不需要添加创建房间按钮、输入框和加入房间按钮

    //添加创建房间按钮
    auto createRoomBtn = MenuItemImage::create("createRoom.png", "createRoom_pressed.png", CC_CALLBACK_1(StartScene::createRoom, this));
    if (createRoomBtn)
        createRoomBtn->setPosition(Vec2::ZERO);       
    else
        cocos2d::log("'CloseNormal.png or CloseSelected.png'");

    //添加存放创建房间按钮的菜单，位置在弹窗正中间偏下
    auto createRoomMenu = Menu::create(createRoomBtn, nullptr);
    if (createRoomMenu) {
        createRoomMenu->setPosition(origin.x + popup->getContentSize().width / 2, origin.y + popup->getContentSize().height / 2 - 20.0f);
        popup->addChild(createRoomMenu, 1);
    }
    else
        cocos2d::log("createRoomBtn");

	//添加房间号输入框
	auto roomCodeInput = ui::TextField::create(u8"这里输入房间号", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if(roomCodeInput) {
		roomCodeInput->setContentSize(Size(200.0f, 50.0f));
		roomCodeInput->setPosition(Vec2(origin.x + 180.0f, origin.y + 55.0f));  //位置在弹窗左下角
        roomCodeInput->setMaxLengthEnabled(true);
        roomCodeInput->setMaxLength(6);                                         //房间号长度限制为6位
        roomCodeInput->setTextColor(Color4B::BLACK);
        popup->addChild(roomCodeInput, 2);
    }
    else
		cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

	//设置输入框的占位符颜色为灰色，点击时变为透明
    roomCodeInput->addEventListener([=](Ref*, ui::TextField::EventType type) {
        if (type == ui::TextField::EventType::ATTACH_WITH_IME)
            roomCodeInput->setPlaceHolderColor(Color4B(0, 0, 0, 0));
        else if (type == ui::TextField::EventType::DETACH_WITH_IME)
            roomCodeInput->setPlaceHolderColor(Color4B::GRAY);               
    });        

	//创建输入框的边框图片，位置在输入框下方，层级比输入框低一层，作为输入框的背景
	auto roomCodeInputFrame = ui::Scale9Sprite::create("roomCodeInputFrame.png");
    if (roomCodeInputFrame) {
        roomCodeInputFrame->setScale(200.0f / roomCodeInputFrame->getContentSize().width, 50.0f / roomCodeInputFrame->getContentSize().height);
        roomCodeInputFrame->setPosition(Vec2(origin.x + 180.0f, origin.y + 55.0f));
        popup->addChild(roomCodeInputFrame, 1);
    }
    else
		CCLOG("'roomCodeInputFrame.png'");

    //创建加入房间按钮
    auto joinRoomBtn = MenuItemImage::create("joinRoom.png", "joinRoom_pressed.png", CC_CALLBACK_1(StartScene::joinRoom, this));
    if (joinRoomBtn)
        joinRoomBtn->setPosition(Vec2::ZERO);       
    else
        cocos2d::log("'CloseNormal.png or CloseSelected.png'");

	//创建存放加入房间按钮的菜单，位置在弹窗右下方，输入框的正右侧
    auto joinRoomMenu = Menu::create(joinRoomBtn, nullptr);
    if (joinRoomMenu) {
        joinRoomMenu->setPosition(origin.x + 420.0f, origin.y + 55.0f);
        popup->addChild(joinRoomMenu, 1);
    }
    else
        cocos2d::log("joinRoomBtn");
}

void StartScene::createLocalMode(Ref* pSender) {
    Director::getInstance()->pushScene(LocalMode::createScene());  //将本地对战场景压栈
}

void StartScene::createOnlineMode(Ref* pSender) {
    std::string title = u8"联机准备";
    std::string content = u8"请选择创建或加入房间";

    createPopup(title, content, true);
}

void StartScene::closeStartScene(Ref* pSender)
{
    Director::getInstance()->end();
    SimpleAudioEngine::getInstance()->end();
}

void StartScene::createRoom(Ref* pSender) {

}

void StartScene::joinRoom(Ref* pSender) {

}