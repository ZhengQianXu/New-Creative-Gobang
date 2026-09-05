#include "MusicControl.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

//背景音乐默认播放
bool MusicControl::isBgmOn = true;

MusicControl* MusicControl::create() {
    auto instance = new MusicControl();
    if (instance && instance->init()) {
        instance->autorelease();        //自动释放instance
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool MusicControl::init() {
    if (!Node::init())
        return false;

    //预加载背景音乐、动画点击音效、落子和输赢音效
    auto audio = SimpleAudioEngine::getInstance();
    audio->preloadBackgroundMusic("music/bgm.mp3");
    audio->preloadEffect("music/victory.mp3");
    audio->preloadEffect("music/zhe.mp3");
    audio->preloadEffect("music/shui.mp3");
    audio->preloadEffect("music/beng.mp3");
    audio->preloadEffect("music/de.mp3");
    audio->preloadEffect("music/zhu.mp3");
    audio->preloadEffect("music/defeat.mp3");

    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //添加背景音乐控制按钮
    bgmBtn = MenuItemImage::create("bgm_btn.png", "bgm_btn.png", CC_CALLBACK_1(MusicControl::toggleBGM, this));
    if (bgmBtn) {
        bgmBtn->setScale(44.0f / bgmBtn->getContentSize().width, 44.0f / bgmBtn->getContentSize().height);  //设置大小为44px * 44px
        bgmBtn->setPosition(Vec2::ZERO);        
    }
    else
        cocos2d::log("'music/bgm.mp3'");
    
    //创建菜单并添加按钮
    auto bgmMenu = Menu::create(bgmBtn, nullptr);                   
    if (bgmMenu) {
        bgmMenu->setPosition(origin.x + 22.0f, origin.y + 22.0f);   //位置放在左下角        
        this->addChild(bgmMenu, 0);
    }
    else
        cocos2d::log("'bgmBtn'");

    //添加背景音乐按钮提示
    auto bgmBtnTip = Label::create(u8"<-点击开关背景音乐", "fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf", 24);
    if (bgmBtnTip) {
        bgmBtnTip->setPosition(origin.x + 160.0f, origin.y + 22.0f);                //放在背景音乐按钮右边
        bgmBtnTip->setTextColor(Color4B::BLACK);
        this->addChild(bgmBtnTip, 0);
    }
    else
        cocos2d::log("'fonts/SourceHanSerifCN/SourceHanSerifCN-Regular.ttf'");

    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);   //播放背景音乐
    
    return true;
}

void MusicControl::toggleBGM(Ref* pSender) {
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

void MusicControl::startRotate() {
    //创建旋转动作（3秒转一圈）并重复执行
    auto rotate = RotateBy::create(3.0f, 360.0f);
    rotateAction = RepeatForever::create(rotate);
    bgmBtn->runAction(rotateAction);
}

void MusicControl::stopRotate() {
    //停止旋转按钮的旋转动作
    if (rotateAction) {
        bgmBtn->stopAction(rotateAction);
        rotateAction = nullptr;
    }
}

void MusicControl::onEnter(){
    Node::onEnter();

    isBgmOn = !isBgmOn;     //先转变状态，因为接下来还要转变一次，相当于不变
    toggleBGM(bgmBtn);      //调用此函数用来更新背景音乐状态
}