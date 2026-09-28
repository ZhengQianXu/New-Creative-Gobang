/*
	GameLogic类是游戏逻辑类，主要负责处理棋子选中、放置点选中、初始化棋盘可放置点以及判赢等逻辑。这些逻辑是每个场景逻辑处理都一致的，
	因此将其封装在GameLogic类中，供各个场景调用，避免每个场景都写相同的代码冗余，还不好维护。因为该类是逻辑处理，也就是对ui组件或数据
	的处理，所以需要传入外部场景的ui指针和数据引用，供该类处理。
*/

#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include "cocos2d.h"

//定义方向类型
struct Direction {
	int x;      //横坐标
	int y;      //纵坐标
};

constexpr Direction Up = { 1, 0 };          //上
constexpr Direction LeftUp = { 1,-1 };      //左上
constexpr Direction Left = { 0, -1 };       //左
constexpr Direction LeftDown = { -1, -1 };  //左下
constexpr Direction Down = { -1,0 };        //下
constexpr Direction RightDown = { -1,1 };   //右下
constexpr Direction Right = { 0,1 };        //右
constexpr Direction RightUp = { 1,1 };      //右上

class GameLogic {
public:
	//成员基本都是引用类型&，需要在构造函数处给予变量，并且只能如 GameLogic.cpp 里实现的那样进行初始化
	GameLogic(cocos2d::Scene* s, bool& iBR, std::string& sCN, cocos2d::Sprite*& sH, cocos2d::Sprite*& sPP, std::vector<int>& sRC, 
		std::vector<std::vector<cocos2d::Sprite*>>& pP, std::vector<std::vector<cocos2d::Sprite*>>& bC);

	//棋子选中处理
	void onSelectChess(cocos2d::Sprite* chessSprite, const std::string& chessName);

	//初始化棋盘可放置各点
	void onInitBoardPlacePoint();

	//放置点选中处理
	bool onSelectPlacePoint(cocos2d::Vec2 touchPos);

	//判赢处理
	bool isVictory(int row, int col);

private:
	//从当前落子处辐射搜索，判赢算法
	bool searchBoardChesses(int row, int col, Direction dir_1, Direction dir_2, std::unordered_map<std::string, int> needChesses);

private:
	cocos2d::Scene* scene;										//当前场景

	bool& isBlackRound;											//是否是黑方回合
	
	std::string& selectedChessName;								//选中棋子名字
	cocos2d::Sprite* selectedHighlight;							//选中高亮效果
	cocos2d::Sprite*& selectedPlacePoint;						//选中放置点
	std::vector<int>& selectedRowCol;							//当前选中放置点的行和列，用于直接落子，不必再判断是哪个位置被点击
	
	std::vector<std::vector<cocos2d::Sprite*>>& placePoints;    //棋盘所有可放置点数组
	std::vector<std::vector<cocos2d::Sprite*>>& boardChesses;   //存放棋盘上棋子数组
};

#endif // !__GAME_LOGIC_H__