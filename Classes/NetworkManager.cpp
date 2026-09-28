#include "NetworkManager.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <fstream>

USING_NS_CC;
using namespace cocos2d::network;

NetworkManager* NetworkManager::getInstance() {
	static NetworkManager instance;		//静态局部变量，保证只创建一次
	static bool isLoadIP = false;		//标志是否加载IP地址
	if (!isLoadIP) {
		instance.loadIPConfig();		//初始化时加载
		isLoadIP = true;
	}
	return &instance;					//返回唯一实例
}

void NetworkManager::runOnMainThread(std::function<void()> func) {
	//如果调度器存在，则在Cocos线程中执行func，否则直接执行func，这时只有主线程
	if (Director::getInstance()->getScheduler())
		Director::getInstance()->getScheduler()->performFunctionInCocosThread(func);
	else
		func();
}

void NetworkManager::fireError(const std::string& msg) {
	auto cb = _onError;		//网络线程中先拷贝回调，避免跨线程竞争
	runOnMainThread([=]() {
		if (cb)
			cb(msg);
	});
}

void NetworkManager::createRoom(std::function<void(bool, const std::string&)> callback)
{
	_createRoomCallBack = callback;

	//创建HttpRequest对象，std::nothrow表示如果内存不足则返回nullptr而不是抛出异常
	HttpRequest* request = new (std::nothrow) HttpRequest();
	if (!request) {
		fireError(u8"网络连接错误，请稍后再试...");
		return;
	}

	request->setUrl("http://" + _serverHost + ":" + _serverPort + "/create_room");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(NetworkManager::onCreateRoomResponse, this));

	HttpClient::getInstance()->send(request);
	request->release();
}

void NetworkManager::onCreateRoomResponse(HttpClient* client, HttpResponse* response)
{
	std::string roomId = "";
	bool success = false;

	if (response && response->isSucceed()) {
		//获取响应数据并转换为字符串
		std::vector<char>* data = response->getResponseData();
		std::string body(data->begin(), data->end());	
		//将响应数据解析为JSON格式
		rapidjson::Document doc;
		doc.Parse(body.c_str());
		CCLOG("[NetworkManager] CreateRoom response: %s", body.c_str());

		if (!doc.HasParseError() && doc.HasMember("roomId") && doc.HasMember("status"))	//检查是否解析成功并且包含roomId和status字段
			if (std::string(doc["status"].GetString()) == "waiting") {					//如果状态为waiting，表示房间创建成功
				roomId = doc["roomId"].GetString();
				success = true;
				connectWebSocket(roomId);												//连接WebSocket服务器
			}
	}
	else
		CCLOG("[NetworkManager] CreateRoom response is not successed");

	auto cb = _createRoomCallBack;		//网络线程中先拷贝回调，避免跨线程竞争
	_createRoomCallBack = nullptr;		//置空，保证回调只触发一次
	runOnMainThread([cb, success, roomId]() {
		if (cb)
			cb(success, roomId);
	});
}

void NetworkManager::joinRoom(const std::string& roomId, std::function<void(const std::string&)> callback) {	
	_joinRoomCallBack = callback;

	HttpRequest* request = new (std::nothrow) HttpRequest();
	if (!request) {
		fireError(u8"网络连接错误，请稍后再试...");
		return;
	}	

	request->setUrl("http://" + _serverHost + ":" + _serverPort + "/join_room");
	request->setRequestType(HttpRequest::Type::POST);
	request->setHeaders({ "Content-Type: application/json" });			//设置请求头为JSON格式

	rapidjson::Document doc;											//创建一个JSON文档对象
	doc.SetObject();													//设置文档类型为对象
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();	//获取分配器，用于分配内存
	doc.AddMember("roomId", rapidjson::Value(roomId.c_str(), allocator).Move(), allocator);	//添加roomId字段到JSON对象中

	rapidjson::StringBuffer buffer;										//创建一个字符串缓冲区，用于存储JSON字符串
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);			//创建一个JSON写入器，将JSON对象写入缓冲区
	doc.Accept(writer);													//将JSON对象写入缓冲区
	std::string body = buffer.GetString();								//获取JSON字符串

	request->setRequestData(body.c_str(), body.size());
	request->setResponseCallback(CC_CALLBACK_2(NetworkManager::onJoinRoomResponse, this));
	
	HttpClient::getInstance()->send(request);
	request->release();
}

void NetworkManager::onJoinRoomResponse(HttpClient* client, HttpResponse* response)
{	
	bool success = false;
	std::string msg = u8"未知错误";

	if (response && response->isSucceed()) {
		auto data = response->getResponseData();
		std::string body(data->begin(), data->end());
		rapidjson::Document doc;
		doc.Parse(body.c_str());
		CCLOG("[NetworkManager] joinRoom response: %s", body.c_str());

		if (!doc.HasParseError() && doc.HasMember("status")) {
			if (std::string(doc["status"].GetString()) == "error" && doc.HasMember("msg"))
				msg = doc["msg"].GetString();					//如果加入房间失败，获取错误消息
			else if (std::string(doc["status"].GetString()) == "success" && doc.HasMember("roomId")) {
				success = true;
				connectWebSocket(doc["roomId"].GetString());	//如果加入房间成功，连接WebSocket服务器
			}
		}
	}
	else
		CCLOG("[NetworkManager] joinRoom response is not successed");

	auto cb = _joinRoomCallBack;
	_joinRoomCallBack = nullptr;
	if(success)
		msg = u8"房间加入成功";
	runOnMainThread([cb, msg]() {
		if (cb)
			cb(msg);
	});
}

void NetworkManager::connectWebSocket(const std::string& roomId)
{
	if (_ws) {
		CCLOG(u8"[NetworkManager] WebSocket 已存在，先断开");
		disconnect();			//断开旧的socket连接，连接新的
	}

	_currentRoomId = roomId;	//设置当前房间号

	_ws = new (std::nothrow) WebSocket();
	if (!_ws) {
		fireError(u8"网络连接失败，请稍后再试...");
		return;
	}

	//设置WebSocket的回调函数为当前对象，并连接到服务器的WebSocket地址，当WebSocket有事件发生时，会调用当前对象的回调函数
	if (!_ws->init(*this, "ws://" + _serverHost + ":" + _serverPort + "/ws")) {
		fireError(u8"访问服务器失败！请稍后再试或联系作者");
		return;
	}
}

void NetworkManager::disconnect() {	
	if (_ws) {
		_ws->close();		//关闭WebSocket连接
		_ws = nullptr;
	}
	
	//重置房间号
	_currentRoomId = "";
}

void NetworkManager::onOpen(WebSocket* ws)
{
	CCLOG(u8"WebSocket 已连接");

	ws->send("join:" + _currentRoomId);		//发送加入房间的消息，格式为"join:房间号"
}

void NetworkManager::onMessage(WebSocket* ws, const WebSocket::Data& data)
{
	std::string msg(data.bytes, data.len);
	CCLOG(u8"[NetworkManager] 收到消息: %s", msg.c_str());

	//收到服务器preparing，即匹配成功、等待游戏开始的消息，调用_onEnter回调函数
	if (msg == "preparing") {
		auto cb = _onEnterScene;
		runOnMainThread([cb]() {
			if (cb)
				cb();
		});
	}
	//服务器指派角色，黑方或白方，然后每回合轮换
	else if (msg.substr(0, 5) == "role:") {
		bool curRole = (msg.substr(5) == "black") ? true : false;	//true代表黑方，false代表白方		
		auto cb = _onCurRole;
		runOnMainThread([cb, curRole]() {
			if (cb)
				cb(curRole);
		});
	}
	//双方都已准备好，开始游戏
	else if (msg == "playing") {		
		auto cb = _onStartGame;
		runOnMainThread([cb]() {
			if (cb)
				cb(true);
		});
	}
	//回合时间在服务器更新，再分发给两个客户端
	else if (msg.substr(0, 7) == "update:") {
		float surplusTime = 0.0f;
		try { surplusTime = std::stof(msg.substr(7)); }
		catch (...) { return; }
		auto cb = _onUpdateTime;
		runOnMainThread([cb, surplusTime]() {
			if (cb)
				cb(surplusTime);
		});
	}
	//move是另一个客户端的落子消息，格式为"move:行号,列号,棋子名字"，调用_onOpponentMove回调函数
	else if (msg.substr(0, 5) == "move:") {
		auto body = msg.substr(5);
		auto p1 = body.find(',');			//第一个‘，’的位置
		auto p2 = body.find(',', p1 + 1);	//第二个‘，’的位置
		if (p1 == std::string::npos || p2 == std::string::npos) {
			CCLOG(u8"对方落子信息格式不正确");
			return;
		}
		//由两个逗号分隔3部分，分别进行处理，获取对应信息
		int row = 0, col = 0;
		try { row = std::stoi(body.substr(0, p1)); col = std::stoi(body.substr(p1 + 1, p2 - p1 - 1)); }
		catch (...) { return; }
		auto chessName = body.substr(p2 + 1);
		auto cb = _onOpponentMove;
		runOnMainThread([cb, row, col, chessName]() {
			if (cb)
				cb(row, col, chessName);
		});
	}
	//有一方离开房间时通知另一方，另一方根据不同情况做不同处理
	else if (msg == "notice:opponentQuit") {
		auto cb = _onQuitRoom;
		auto tip = _onError;
		runOnMainThread([cb, tip]() {
			if (cb)
				cb();
			if (tip)
				tip(u8"对方已退出房间！");
		});
	}
}

void NetworkManager::onClose(WebSocket* ws)
{
	if(_ws == ws)
		_ws = nullptr;
	if (ws)
		delete ws;
	
	//重置房间号
	_currentRoomId = "";
	CCLOG(u8"[NetworkManager] ws 已断开");
}

void NetworkManager::onError(WebSocket* ws, const WebSocket::ErrorCode& error)
{
	CCLOG(u8"[NetworkManager] ws 错误: %d", int(error));
	fireError(u8"网络连接错误，请稍后再试...");
}

void NetworkManager::sendMsg(const std::string& msg)
{
	if(_ws)
		_ws->send(msg);
}

void NetworkManager::loadIPConfig()
{	
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);		//调用WindowsAPI，获取当前exe绝对路径
	std::string dir = exePath;
	dir = dir.substr(0, dir.find_last_of("\\/") + 1);	//截取exe同级目录
	std::string iniPath = dir + "server.ini";			//得到配置文件绝对路径，这样需要将配置文件放在exe同级目录才能读取
	CCLOG(iniPath.c_str());
	
	std::ifstream file(iniPath);
	if (!file.is_open()) {
		CCLOG(u8"[NetworkManager] 未找到server.ini，使用默认IP");
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#' || line[0] == '[')	//过滤空行或注释
			continue;
		
		size_t pos = line.find('=');							//获取=位置
		if (pos == std::string::npos)							//如果后面没有值就不用继续
			continue;

		std::string key = line.substr(0, pos);					//=前面字符串
		std::string value = line.substr(pos + 1);				//=后面字符串

		if (key == "host")
			_serverHost = value;								//得到服务器IP地址
		else if (key == "port")
			_serverPort = value;								//得到端口号
	}
	file.close();
	CCLOG("host = %s, port = %s", _serverHost.c_str(), _serverPort.c_str());
}