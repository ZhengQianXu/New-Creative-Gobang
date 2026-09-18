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

	//向对手发送落子信息
	void sendMove(int row, int col);

	//断开连接
	void disconnect();
	
	//设置onStart回调函数
	void setOnStartCallBack(std::function<void()> cb) { _onStart = cb; }

	//设置_onOpponentMove回调函数
	void setOnOpponentMoveCallBack(std::function<void(int, int)> cb) { _onOpponentMove = cb; }

	//设置_onError回调函数
	void setOnErrorCallBack(std::function<void(const std::string&)> cb) { _onError = cb; }

	//返回socket连接状态
	bool isConnected() const { return _isConnected; }

	//获取当前房间号
	std::string getCurrentRoomId() const { return _currentRoomId; }

	//设置服务器IP地址和端口号
	void setServerAddress(const std::string& host, int post) { _serverHost = host; _serverPort = post; }

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

	std::function<void()> _onStart = nullptr;										//联机匹配成功回调
	std::function<void(int, int)> _onOpponentMove = nullptr;						//对手落子回调
	std::function<void(const std::string&)> _onError = nullptr;						//fireError回调

	std::string _serverHost = "127.0.0.1";											//IP地址
	std::string _serverPort = "8080";												//端口号

	std::string _currentRoomId = "";												//当前房间号
	bool _isConnected = false;														//socket连接状态

	cocos2d::network::WebSocket* _ws = nullptr;										//socket连接
};

#endif // !__NETWORK_MANAGER_H__