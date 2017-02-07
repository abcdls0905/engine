//--------------------------------------------------------------------
// 文件名:		game_win_sock.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_WIN_SOCK_H
#define _GAME_WIN_SOCK_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../visual/i_game_client.h"
#include "../visual/i_game_sock.h"
#include "../utils/lock_util.h"
#include "../visual/i_solo_stub.h"

// 单线程网络连接

class CSockBuf;
class CGameEncoder;
class GameSender;
class GameReceiver;
class IWinSocket;

class GameSock: public IGameSock
{
private:
	enum WORK_STATE_ENUM
	{
		WORKSTATE_CONNECTING = 1,
		WORKSTATE_CONNECTED = 2,
		WORKSTATE_FAILED = 3,
	};

	enum { BUFFER_LEN = 0x20000 };

	void MainLoop();
	void ProcessRead();
	bool ProcessWrite();
	void ProcessConnect();
	void ProcessClose();
public:
	GameSock();
	virtual ~GameSock();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	/*
	virtual bool MsgProc(void* hwnd, unsigned int msg, size_t param1, 
		size_t param2, int& result);*/
	virtual void Execute(float seconds);

	void SetSockMsgHandler(ISockMsgHandler* handler)
	{
		m_SockMsgHandler = handler;
	}

	// 消息发送器
	PERSISTID GetSender() const;
	// 消息接收器
	PERSISTID GetReceiver() const;
	
	// 是否在连接状态
	bool GetConnected() const;
	// 获得SOCKET状态
	int GetState() const;
	
	// 加载加密模块
	bool LoadEncodeDll(const char* dll_name);

	// 连接服务器
	bool Connect(const char* addr, int port, bool blocking = true);
	// 断开服务器
	bool Disconnect();
	// 发送消息
	bool SendData(void* pdata, size_t len);

	// 每帧允许处理消息的时间上限
	void SetLimitTime(double value) { m_dLimitTime = value; }
	double GetLimitTime() const { return m_dLimitTime; }

	// 直接投递消息
	virtual bool PutData(void* pdata, size_t len);

	virtual bool PutData2(const void* pdata1, size_t len1, 
						const void* pdata2,  size_t len2);
private:
	// 接收数据
	bool RecvData(void* pdata, size_t len);
	// 发送数据
	bool InnerSend();
	// 生成发送数据
	size_t MakeSendData(void* pdata, size_t len);

private:
	CLockUtil m_Lock;
	IWinSocket* m_pWinSocket;
	GameSender* m_pSender;
	GameReceiver* m_pReceiver;
	CGameEncoder* m_pEncoder;
	size_t m_hSocket;
	int m_nMsgId;
	int m_nState;
	CSockBuf* m_pSendBuf;
	CSockBuf* m_pRecvBuf;
	unsigned char* m_pOutBuf;
	bool m_bSendBroken;
	size_t m_nSendOffset;
	size_t m_nSendRemain;
	unsigned char* m_pInBuf;
	unsigned int m_nInCount;
	unsigned char m_nInPrior;
	core_string m_strAddr;
	int m_nPort;
	double m_dLimitTime;
	ISockMsgHandler * m_SockMsgHandler;
};

#endif // _GAME_WIN_SOCK_H

