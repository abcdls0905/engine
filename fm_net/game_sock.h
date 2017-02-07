//--------------------------------------------------------------------
// �ļ���:		game_win_sock.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ���߳���������

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

	// ��Ϣ������
	PERSISTID GetSender() const;
	// ��Ϣ������
	PERSISTID GetReceiver() const;
	
	// �Ƿ�������״̬
	bool GetConnected() const;
	// ���SOCKET״̬
	int GetState() const;
	
	// ���ؼ���ģ��
	bool LoadEncodeDll(const char* dll_name);

	// ���ӷ�����
	bool Connect(const char* addr, int port, bool blocking = true);
	// �Ͽ�������
	bool Disconnect();
	// ������Ϣ
	bool SendData(void* pdata, size_t len);

	// ÿ֡��������Ϣ��ʱ������
	void SetLimitTime(double value) { m_dLimitTime = value; }
	double GetLimitTime() const { return m_dLimitTime; }

	// ֱ��Ͷ����Ϣ
	virtual bool PutData(void* pdata, size_t len);

	virtual bool PutData2(const void* pdata1, size_t len1, 
						const void* pdata2,  size_t len2);
private:
	// ��������
	bool RecvData(void* pdata, size_t len);
	// ��������
	bool InnerSend();
	// ���ɷ�������
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

