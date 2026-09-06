#ifndef __BOARD_UI_H__
#define __BOARD_UI_H__

#include "cocos2d.h"

class BoardUI :public cocos2d::Node {
public:
	virtual bool init();

	CREATE_FUNC(BoardUI);

	//设置返回按钮点击回调（外部注入）
	void setOnReturnCallBack(std::function<void()> callback);

	//设置改变音效按钮点击回调（外部注入）
	void setOnToggleEffectCallBack(std::function<void()> callback);

	//设置触摸事件点击回调（外部注入）
	void setOnTouchBeganCallBack(std::function<bool(cocos2d::Touch*, cocos2d::Event*)> callback);

	//设置开始游戏按钮点击回调（外部注入）
	void setOnStartGameCallBack(std::function<void(cocos2d::Ref*)> callback);

	//设置清理棋盘状态回调（外部注入）
	void setOnCleanBoardCallBack(std::function<void(cocos2d::Ref*)> callback);

	//获取部分ui控件指针，用于逻辑处理
	void getBoardUImember(std::vector<cocos2d::Sprite*>& cs, cocos2d::Sprite*& sH, cocos2d::MenuItemImage*& sGB, cocos2d::Label*& t, 
		cocos2d::Sprite*& bRA, cocos2d::Sprite*& wRA, cocos2d::Label*& gOT, cocos2d::Sprite*& vA, cocos2d::Sprite*& dA, 
		cocos2d::MenuItemImage*& gOB, cocos2d::MenuItemImage*& gODB) const;

	//获取结算界面ui控件的集合，外部场景直接将其addChild，方便设置在上层，避免其他ui遮挡
	cocos2d::Node* getGameOverUI() const;

private:
	//返回按钮回调
	void onReturnBtn(cocos2d::Ref* pSender);
	std::function<void()> _onReturn = nullptr;

	//改变音效开关状态
	void onToggleEffectBtn(cocos2d::Ref* pSender);
	std::function<void()> _onToggleEffect = nullptr;

	//触摸事件回调
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	std::function<bool(cocos2d::Touch*, cocos2d::Event*)> _onTouchBegan = nullptr;

	//开始游戏回调
	void onStartGameBtn(cocos2d::Ref* pSender);
	std::function<void(cocos2d::Ref*)> _onStartGame = nullptr;

	//清理棋盘状态回调
	void onCleanBoard(cocos2d::Ref* pSender);
	std::function<void(cocos2d::Ref*)> _onCleanBoard = nullptr;

private:
	std::vector<cocos2d::Sprite*> chessSprites;             //棋子数组
	cocos2d::Sprite* selectedHighlight = nullptr;           //选中高亮效果

	cocos2d::MenuItemImage* startGameBtn = nullptr;			//开始游戏按钮

	cocos2d::Label* timer = nullptr;                        //计时器显示标签
	cocos2d::Sprite* blackRoundArrow = nullptr;             //指向黑方的箭头
	cocos2d::Sprite* whiteRoundArrow = nullptr;             //指向白方的箭头

	cocos2d::Label* gameOverTip = nullptr;                  //游戏结算提示
	cocos2d::Sprite* victoryAnimation = nullptr;            //获胜动画展示
	cocos2d::Sprite* drawAnimation = nullptr;				//平局结算动画
	cocos2d::MenuItemImage* gameOverBtn = nullptr;          //游戏结束按钮
	cocos2d::MenuItemImage* gameOverDrawBtn = nullptr;		//平局结束按钮

	cocos2d::Node* victoryUI = nullptr;						//用来存储3个获胜界面ui控件的节点
};

#endif // !__BOARD_UI_H__