/*
	PopupUI类包括遮罩层和弹窗组件，提供了初始化、创建和关闭弹窗的功能。该类只定义了满屏的遮罩层和正中间的空白弹窗，用户可以通过标题和
	内容来初始化弹窗，并且可以获取弹窗组件和内容组件进行进一步操作。
*/

#ifndef __POPUP_UI_H__
#define __POPUP_UI_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class PopupUI : public cocos2d::Node {
public:
	//创建弹窗实例
	static PopupUI* create(const std::string& title, const std::string& content);

	//根据标题和内容初始化弹窗
	bool init(const std::string& title, const std::string& content);

	//关闭弹窗
	void closePopup(cocos2d::Ref* pSender);

	//获取弹窗组件
	cocos2d::ui::Layout* getPopup() { return popup; }

	//获取弹窗内容组件
	cocos2d::Label* getPopupContent() { return popupContent; }

	//弹窗关闭回调
	void setOnCloseCallBack(std::function<void()> cb) { _onClose = cb; }

private:
	cocos2d::LayerColor* popupMask = nullptr;               //遮罩层
	cocos2d::ui::Layout* popup = nullptr;                   //弹窗
	cocos2d::Label* popupContent = nullptr;					//弹窗内容
	
	std::function<void()> _onClose = nullptr;				//弹窗关闭处理
};

#endif // !__POPUP_UI_H__