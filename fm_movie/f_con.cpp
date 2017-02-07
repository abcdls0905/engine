//#include "stdafx.h"
#include "f_con.h"
#include <stdio.h>

FCon::FCon()
{

}

FCon::~FCon()
{
	m_workThread = false;
}

DWORD __stdcall FCon::WorkThread(LPVOID Param)
{
	FCon * pthis = (FCon *)Param;

	while(pthis->m_workThread)
	{
		char buf[4096];
		bool ret = pthis->Recv(buf,4096);

		if (!ret)
		{
			//MessageBox(NULL,L"Net Recv false!",L"ERROR",MB_OK);
			break;
		}

		pthis->OnRead(buf,2048);
	}
	return 0;
}

bool FCon::Init()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) 
	{
		return false;
	} 

	if ((s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{
		closesocket(s);
		return false;
	}
	return true;
}

bool FCon::Connect(const char *addr, int port)
{
	sockaddr_in addrin;
	addrin.sin_family = AF_INET;
	addrin.sin_addr.S_un.S_addr = inet_addr(addr);
	addrin.sin_port = htons(port);

/*	if (bind(s,&addr,sizeof(addr))==SOCKET_ERROR)
	{
		return false;
	}*/

	int ret = connect(s,(SOCKADDR*)&addrin,sizeof(sockaddr_in));
	if (ret==-1)
	{
//		wchar_t buff[50];
	//	int error = WSAGetLastError();
		//printf("connect error: %d\n", error);
		//swprintf_s(buff, 50, L"connect error: %d", error);
		//MessageBox(NULL,buff,L"ERROR",MB_OK);
		return false;
	}

	this->m_workThread = true;
	g_hwThread = CreateThread(NULL, 0, WorkThread, (LPVOID)this, 0, &m_wthreadID);	//	创建工作线程，用来处理完成端口消息的
	
	OnConnect();
	return true;
}

int FCon::Send(const char* buf,int len)
{
	return send(s,buf,len,0);
}

bool FCon::Recv(char* buf,int len)
{
	int ret = recv(s,buf,len,0);
	if (ret == SOCKET_ERROR)
	{
		wchar_t buff[50];
		int error = WSAGetLastError();
		//printf("recv error: %d\n", error);
		swprintf_s(buff, 50, L"recv error: %d", error);
		MessageBox(NULL,buff,L"ERROR",MB_OK);
		return false;
	}
	return true;
}
