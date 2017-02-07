//--------------------------------------------------------------------
// 文件名:		win_socket.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#include <winsock2.h>
#include <windows.h>
const int WM_NETMSG_START = WM_USER + 100;
#else
const int WM_NETMSG_START = 100;
#endif
#include "win_socket.h"
#pragma comment(lib, "ws2_32.lib")
#include "../public/portable.h"

DECLARE_FUNCTION_INTERFACE(WinSocket);



// WinSocket

WinSocket::WinSocket()
{
	for (size_t i = 0; i < MAX_NETMSG_NUM; i++)
	{
		m_bMsgInuse[i] = false;
		m_nLastUseTick[i] = 0;
	}
}

WinSocket::~WinSocket()
{
}

bool WinSocket::Init()
{
#ifdef _WIN32
	WSADATA wsa;

	if (WSAStartup(0x101, &wsa) != 0)
	{
		return false;
	}
#endif
	return true;
}

bool WinSocket::Shut()
{
#ifdef _WIN32
	WSACleanup();
#endif
	return true;
}

int WinSocket::GetSockMsg()
{
	unsigned int cur_tick = Port_GetTickCount();
	
	for (int i = 0; i < MAX_NETMSG_NUM; i++)
	{
		// 防止相同的消息编号在短时间内被重复使用
		if ((m_nLastUseTick[i] != 0)
			&& (cur_tick - m_nLastUseTick[i]) < 10000)
		{
			continue;
		}
		
		if (!m_bMsgInuse[i])
		{
			m_bMsgInuse[i] = true;
			return (WM_NETMSG_START + i);
		}
	}

	return 0;
}

bool WinSocket::ReleaseSockMsg(int msg)
{
	int index = msg - WM_NETMSG_START;

	if (index >= MAX_NETMSG_NUM)
	{
		return false;
	}

	m_bMsgInuse[index] = false;
	m_nLastUseTick[index] = Port_GetTickCount();

	if (0 == m_nLastUseTick[index])
	{
		// 防止为0
		--m_nLastUseTick[index];
	}

	return true;
}
