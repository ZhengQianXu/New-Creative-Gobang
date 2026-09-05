#ifndef __StartScene_H__
#define __StartScene_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class StartScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(StartScene);

	//规则内容显示
	void onRuleShow(cocos2d::Ref* pSender);

	//反馈途径显示
	void onSuggestShow(cocos2d::Ref* pSender);

	//关闭弹窗
	void closePopup(cocos2d::Ref* pSender);

	//创建弹窗
	void createPopup(const std::string& title, const std::string& content);

	//创建本地对战模式场景
	void createLocalMode(cocos2d::Ref* pSender);

	//创建双人联机模式场景
	void createOnlineMode(cocos2d::Ref* pSender);

	//退出游戏按钮
	void closeStartScene(cocos2d::Ref* pSender);

private:
	cocos2d::Size visibleSize;                              //窗口大小
	cocos2d::Vec2 origin;                                   //坐标原点

	cocos2d::LayerColor* popupMask = nullptr;               //遮罩层
	cocos2d::ui::Layout* popup = nullptr;                   //弹窗
};

#endif // __StartScene_H__