#ifndef __ONLINEMODE_SCENE_H__
#define __ONLINEMODE_SCENE_H__

#include "cocos2d.h"

class OnlineMode :public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(OnlineMode);
};

#endif // __ONLINEMODE_SCENE_H__