#include "PopupUI.h"

USING_NS_CC;

PopupUI* PopupUI::create(const std::string& title, const std::string& content) {
    auto instance = new PopupUI();
    if (instance && instance->init(title, content)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool PopupUI::init(const std::string& title, const std::string& content) {
    if (!Node::init())
        return false;

    auto origin = Director::getInstance()->getVisibleOrigin();
    auto visibleSize = Director::getInstance()->getVisibleSize();

    //创建半透明遮罩层，遮罩层会覆盖整个屏幕，让主场景变暗
    popupMask = LayerColor::create(Color4B(0, 0, 0, 150));
    popupMask->setContentSize(visibleSize);                                 //铺满屏幕
    popupMask->setPosition(origin);                                         //从屏幕左下角开始
    this->addChild(popupMask, 1);

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

    //创建内容文字，内容文字位置在弹窗正中间偏下
    popupContent = Label::create(content, "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 22);
    if (popupContent) {
        popupContent->setTextColor(Color4B::BLACK);
        popupContent->setPosition(origin.x + popup->getContentSize().width / 2, origin.y + popup->getContentSize().height / 2 - 20.0f);
        popupContent->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);        //文字在区域内水平垂直居中
        popup->addChild(popupContent);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    //创建关闭弹窗按钮
    auto closeBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(PopupUI::closePopup, this));
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

    return true;
}

void PopupUI::closePopup(cocos2d::Ref* pSender) {
    if (_onClose)
        _onClose();
    this->removeFromParent();
}