#ifndef __BORAD_UI_H__
#define __BOARD_UI_H__

#include "cocos2d.h"

class BoardUI :public cocos2d::Node {
public:
	virtual bool init();

	CREATE_FUNC(BoardUI);

	//设置返回按钮点击回调（外部注入）
	void setOnReturnCallBack(std::function<void()> callback);

	void setOnToggleEffectCallBack(std::function<void()> callback);

	void setOnToughBeganCallBack(std::function<bool(cocos2d::Touch*, cocos2d::Event*)> callback);

	void setOnStartGameCallBack(std::function<void()> callback);

private:
	//返回按钮回调
	void onReturnBtn(cocos2d::Ref* pSender);
	std::function<void()> _onReturn = nullptr;

	//改变音效开关状态
	void onToggleEffectBtn(cocos2d::Ref* pSender);
	std::function<void()> _onToggleEffect = nullptr;

	//点击开始回调
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	std::function<bool(cocos2d::Touch*, cocos2d::Event*)> _onTouchBegan = nullptr;

	void onStartGameBtn(cocos2d::Ref* pSender);
	std::function<void()> _onStartGame = nullptr;

	std::vector<cocos2d::Sprite*> chessSprites;             //棋子数组
};

#endif // !__BORAD_UI_H__