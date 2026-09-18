#ifndef __StartScene_H__
#define __StartScene_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class StartScene : public cocos2d::Scene {
public:
	//创建主界面
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(StartScene);

private:
	//规则内容显示
	void onRuleShow(cocos2d::Ref* pSender);

	//反馈途径显示
	void onSuggestShow(cocos2d::Ref* pSender);

	//关闭弹窗
	void closePopup(cocos2d::Ref* pSender);

	//创建弹窗
	void createPopup(const std::string& title, const std::string& content, bool isOnline = false);

	//创建本地对战模式场景
	void createLocalMode(cocos2d::Ref* pSender);

	//创建双人联机模式场景
	void createOnlineMode(cocos2d::Ref* pSender);

	//退出游戏按钮
	void closeStartScene(cocos2d::Ref* pSender);

	//创建房间
	void createRoom(cocos2d::Ref* pSender);

	//加入房间
	void joinRoom(cocos2d::Ref* pSender);	

private:
	cocos2d::Size visibleSize;                              //窗口大小
	cocos2d::Vec2 origin;                                   //坐标原点

	cocos2d::LayerColor* popupMask = nullptr;               //遮罩层
	cocos2d::ui::Layout* popup = nullptr;                   //弹窗
	cocos2d::Label* popupContent = nullptr;					//弹窗内容
	cocos2d::Label* curRoomId = nullptr;					//当前房间号
	cocos2d::ui::TextField* roomIdInput = nullptr;			//房间号输入框
};

#endif // !__StartScene_H__