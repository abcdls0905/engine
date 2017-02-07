//--------------------------------------------------------------------
// �ļ���:		win_socket.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WIN_SOCKET_H
#define _WIN_SOCKET_H

#include "../visual/i_win_socket.h"

// ����ӿڹ���

class WinSocket: public IWinSocket
{
private:
	enum { MAX_NETMSG_NUM = 1024 };

public:
	WinSocket();
	virtual ~WinSocket();

	virtual bool Init();
	virtual bool Shut();

	// ���WINDOWS��Ϣ���
	virtual int GetSockMsg();
	// �ͷ�WINDOWS��Ϣ���
	virtual bool ReleaseSockMsg(int msg);

private:
	bool m_bMsgInuse[MAX_NETMSG_NUM];
	unsigned int m_nLastUseTick[MAX_NETMSG_NUM];
};

#endif // _WIN_SOCKET_H
