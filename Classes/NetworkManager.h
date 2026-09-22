#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "network/WebSocket.h"

class NetworkManager : public cocos2d::network::WebSocket::Delegate{
public:
	//单例模式
	static NetworkManager* getInstance();

	//为保证只有唯一实例，不允许拷贝和赋值
	NetworkManager(const NetworkManager&) = delete;
	NetworkManager& operator=(const NetworkManager&) = delete;

	//创建房间
	void createRoom(std::function<void(bool, const std::string&)> callback);

	//加入房间
	void joinRoom(const std::string& roomId, std::function<void(const std::string&)> callback);

	//创建websocket连接
	void connectWebSocket(const std::string& roomId);

	//向对手发送信息
	void sendMsg(const std::string& msg);

	//断开连接
	void disconnect();
	
	//设置onEnter回调函数
	void setOnEnterSceneCallBack(std::function<void()> cb) { _onEnterScene = cb; }

	//设置当前角色回调
	void setOnCurRoleCallBack(std::function<void(bool)> cb) { _onCurRole = cb; }

	//设置开始游戏处理回调
	void setOnStartGameCallBack(std::function<void(bool)> cb) { _onStartGame = cb; }

	//设置回合时间更新回调
	void setOnUpdateTimeCallBack(std::function<void(float)> cb) { _onUpdateTime = cb; }

	//设置_onOpponentMove回调函数
	void setOnOpponentMoveCallBack(std::function<void(int, int, const std::string&)> cb) { _onOpponentMove = cb; }

	//设置退出房间回调
	void setOnQuitRoomCallBack(std::function<void()> cb) { _onQuitRoom = cb; }

	//设置_onError回调函数
	void setOnErrorCallBack(std::function<void(const std::string&)> cb) { _onError = cb; }

private:
	//为保证只有唯一实例，不允许构造和析构
	NetworkManager() = default;
	~NetworkManager() = default;
	
	//在主线程运行func函数
	void runOnMainThread(std::function<void()> func);

	//报错msg
	void fireError(const std::string& msg);

	//创建房间响应处理
	void onCreateRoomResponse(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);

	//加入房间响应处理
	void onJoinRoomResponse(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);

	//socket连接成功处理
	virtual void onOpen(cocos2d::network::WebSocket* ws) override;

	//socket消息处理
	virtual void onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data) override;

	//socket关闭处理
	virtual void onClose(cocos2d::network::WebSocket* ws) override;

	//socket出错处理
	virtual void onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error) override;

private:	
	std::function<void(bool, const std::string&)> _createRoomCallBack = nullptr;	//创建房间响应回调
	std::function<void(const std::string&)> _joinRoomCallBack = nullptr;			//加入房间响应回调

	std::function<void()> _onEnterScene = nullptr;									//联机匹配成功回调
	std::function<void(bool)> _onCurRole = nullptr;									//当前角色，即黑方或白方
	std::function<void(bool)> _onStartGame = nullptr;								//开始游戏函数
	std::function<void(float)> _onUpdateTime = nullptr;								//回合时间更新函数
	std::function<void(int, int, const std::string&)> _onOpponentMove = nullptr;	//对手落子回调
	std::function<void()> _onQuitRoom = nullptr;									//退出房间函数
	std::function<void(const std::string&)> _onError = nullptr;						//fireError回调

	std::string _serverHost = "127.0.0.1";											//IP地址
	std::string _serverPort = "8080";												//端口号

	std::string _currentRoomId = "";												//当前房间号

	cocos2d::network::WebSocket* _ws = nullptr;										//socket连接
};

#endif // !__NETWORK_MANAGER_H__