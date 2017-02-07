#pragma once

#include <win_sock_2.h>
#pragma comment(lib,"WS2_32.lib")

class FCon
{
public:
	FCon();
	virtual ~FCon();

	static DWORD __stdcall WorkThread(LPVOID Param);

	bool m_workThread;

public:
	bool Init();
	bool Connect(const char* addr,int port);
	int Send(const char* buf,int len);

protected:
	virtual void OnRead(char *buf, int len) {}
	bool Recv(char* buf,int len);

	virtual void OnConnect() {}

private:
	SOCKET s;

	HANDLE g_hwThread;		//	工作线程句柄
	DWORD  m_wthreadID;
};
