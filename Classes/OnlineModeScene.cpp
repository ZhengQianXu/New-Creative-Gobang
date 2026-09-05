#include "OnlineModeScene.h"
#include "BoardUI.h"

USING_NS_CC;

Scene* OnlineMode::createScene() {
	return OnlineMode::create();
}

bool OnlineMode::init() {
	if (!Scene::init())
		return false;

	auto bu = BoardUI::create();
	this->addChild(bu, 0);

	bu->setOnReturnCallBack([]() {
		Director::getInstance()->popScene();
	});

	return true;
}