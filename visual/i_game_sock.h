#ifndef _I_GAME_WIN_SOCK_H
#define _I_GAME_WIN_SOCK_H

#include "../public/module.h"
#include "../visual/i_game_client.h"

class IGameSock: public IEntity
{
public:
	// 消息发送器
	virtual PERSISTID GetSender() const = 0;
	// 消息接收器
	virtual PERSISTID GetReceiver() const = 0;
	
	// 是否在连接状态
	virtual bool GetConnected() const = 0;
	// 获得SOCKET状态
	virtual int GetState() const = 0;
	
	// 加载加密模块
	virtual bool LoadEncodeDll(const char* dll_name) = 0;

	// 连接服务器
	virtual bool Connect(const char* addr, int port, bool blocking = true) = 0;

	// 断开服务器
	virtual bool Disconnect() = 0;

	// 发送消息
	virtual bool SendData(void* pdata, size_t len) = 0;

	// 每帧允许处理消息的时间上限
	virtual void SetLimitTime(double value) = 0;

	virtual double GetLimitTime() const = 0;

	// 直接投递消息
	virtual bool PutData(void* pdata, size_t len) = 0;

	virtual bool PutData2(const void* pdata1, size_t len1, 
		const void* pdata2,  size_t len2) = 0;
};

#endif