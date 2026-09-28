/*
	StartScene类是一个继承自cocos2d::Scene的类，表示游戏的主界面场景。该类是游戏的入口，负责显示游戏的规则、反馈途径以及提供本地对战
	和双人联机模式的选项。
*/

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

	cocos2d::Label* popupContent = nullptr;					//弹窗内容
	cocos2d::Label* curRoomId = nullptr;					//当前房间号
	cocos2d::ui::TextField* roomIdInput = nullptr;			//房间号输入框
};

#endif // !__StartScene_H__