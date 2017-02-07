//--------------------------------------------------------------------
// 文件名:		win_socket.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WIN_SOCKET_H
#define _WIN_SOCKET_H

#include "../visual/i_win_socket.h"

// 网络接口管理

class WinSocket: public IWinSocket
{
private:
	enum { MAX_NETMSG_NUM = 1024 };

public:
	WinSocket();
	virtual ~WinSocket();

	virtual bool Init();
	virtual bool Shut();

	// 获得WINDOWS消息编号
	virtual int GetSockMsg();
	// 释放WINDOWS消息编号
	virtual bool ReleaseSockMsg(int msg);

private:
	bool m_bMsgInuse[MAX_NETMSG_NUM];
	unsigned int m_nLastUseTick[MAX_NETMSG_NUM];
};

#endif // _WIN_SOCKET_H
