//--------------------------------------------------------------------
// 文件名:		FrameReceiver.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "frame_receiver.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include <process.h>

#define MIN_ALLOC_FRAME_COUNT 5

#pragma comment(lib, "ws2_32.lib")

// 等待线程结束
static bool wait_thread_exit(HANDLE handle)
{
	for (;;)
	{
		DWORD res = WaitForSingleObject(handle, 1000);

		if (res == WAIT_OBJECT_0)
		{
			return true;
		}
		else if (res == WAIT_TIMEOUT)
		{
			DWORD exit_code;

			if (GetExitCodeThread(handle, &exit_code) == FALSE)
			{
				return false;
			}

			if (exit_code != STILL_ACTIVE)
			{
				break;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

FrameReceiver::FrameReceiver(unsigned int nFrameSize)
{ 
	m_bInit = false;
	m_nFrameSize = nFrameSize;
	m_hThread = NULL;
	m_bQuit = true;
	m_nPort = 0;
	m_sock = INVALID_SOCKET;
}

FrameReceiver::~FrameReceiver() 
{
	if (m_bInit)
	{
		WSACleanup();
	}

	// 释放所有内存
	size_t nBufferSize = m_Buffers.size();
	for (size_t i = 0; i < nBufferSize; ++i)
	{
		CORE_FREE(m_Buffers[i].buffer, m_Buffers[i].size);
	}
	m_Buffers.clear();
}


bool FrameReceiver::Init()
{
	WSADATA wsa;	
	if (WSAStartup(0x101, &wsa) != 0)
	{
		CORE_TRACE("(FrameReceiver::Init) WSAStartup failed.");
		return false;
	}

	m_bInit = true;
	return true;
}


bool FrameReceiver::StartListen(const char* szMyIP, unsigned short nPort)
{
	if (m_hThread && (m_strIP == szMyIP) && m_nPort == nPort)
	{
		return false;
	}

	StopListen();

	m_sock = socket(AF_INET,SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_sock)
	{
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = inet_addr(szMyIP);

	int nResult = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (nResult == SOCKET_ERROR)
	{
		return false;
	}

	m_strIP = szMyIP;
	m_nPort = nPort;
	m_bQuit = false;

	if (NULL == m_hThread)
	{
		m_hThread = (HANDLE)_beginthread(WorkerProc, 0, this);
	}
	return true;
}


bool FrameReceiver::StopListen()
{
	m_bQuit = true;

	if (m_hThread)
	{
		wait_thread_exit(m_hThread);
		m_hThread = NULL;
	}

	::closesocket(m_sock);

	return true;
}


char* FrameReceiver::PopFromWaitList()
{
	if (m_WaitRenderFrameData.empty())
	{
		return NULL;
	}

	CAutoLock autolock(m_Lock);
	char* pFrameBuffer = m_WaitRenderFrameData.front();
	m_WaitRenderFrameData.pop_front();
	return pFrameBuffer;
}


void FrameReceiver::PushToWaitList(char* pFrameBuffer)
{
	CAutoLock autolock(m_Lock);
	m_WaitRenderFrameData.push_back(pFrameBuffer);
}


char* FrameReceiver::PopFromFreeList()
{
	CAutoLock autolock(m_Lock);
	// 空闲队列为空
	if (m_FreeFrameData.empty())
	{
		// 创建的帧
		//const int nAddFrameCount = m_WaitRenderFrameData.size() * 2 + MIN_ALLOC_FRAME_COUNT;
		const int nAddFrameCount = MIN_ALLOC_FRAME_COUNT;
		AllocFreeFrame(nAddFrameCount);

		if (m_FreeFrameData.empty())
		{
			Assert(0);
			return NULL;
		}

		return PopFromFreeList();
	}

	// 直接从空闲队列里取出一帧
	//char* pFrameBuffer = m_FreeFrameData.front();
	//m_FreeFrameData.pop_front();
	char* pFrameBuffer = NULL;
	pFrameBuffer = m_FreeFrameData.back();
	m_FreeFrameData.pop_back();

	return pFrameBuffer;
}


void FrameReceiver::PushToFreeList(char* pFrameBuffer)
{
	CAutoLock autolock(m_Lock);
	if (pFrameBuffer)
	{
		m_FreeFrameData.push_back(pFrameBuffer);
	}
}


void FrameReceiver::AllocFreeFrame(unsigned int nAddFrameCount)
{
	CAutoLock autolock(m_Lock);

	const int nBufferSize = m_nFrameSize * nAddFrameCount;
	char* pBuffer = (char*)CORE_ALLOC(nBufferSize);
	if (NULL == pBuffer)
	{
		Assert(0);
		return;
	}

	// 放入内存池
	sBuffer buffer;
	buffer.buffer = pBuffer;
	buffer.size = nBufferSize;
	m_Buffers.push_back(buffer);

	// 放入空闲队列
	for (unsigned int i = 0; i < nAddFrameCount; ++i)
	{
		PushToFreeList(pBuffer + m_nFrameSize);
	}
}


void __cdecl FrameReceiver::WorkerProc(void* lpParameter)
{
	FrameReceiver* pthis = (FrameReceiver*)lpParameter;

	fd_set fdread;
	timeval val = {60,0};
	int len = sizeof(sockaddr_in);
	sockaddr_in server;
	int sock = pthis->m_sock;

	while (!pthis->m_bQuit)
	{
		FD_ZERO(&fdread);
		FD_SET(pthis->m_sock, &fdread);

		// 查看socket是否可读
		int Result = select(0, &fdread, 0, 0, &val);
		if(Result == SOCKET_ERROR)
		{
			// 连接错误
			break;
		}

		if (sock != pthis->m_sock)
		{
			// 已经切换了连接
			break;
		}

		if(Result == 0)
		{
			// 没有可读的套接字,重新判断
			continue;
		}

		char* pFrameBuffer = pthis->PopFromFreeList();
		// 内存不足
		if (NULL == pFrameBuffer)
		{
			continue;
		}

		int rec = recvfrom(sock, (char*)pFrameBuffer, pthis->m_nFrameSize, 0, (sockaddr*)&server, &len);
		if(rec > 0)
		{
			pthis->PushToWaitList(pFrameBuffer);
		}
	}
}


void FrameReceiver::ReleaseWaitList()
{
	// 释放等待渲染的帧队列
	FRAME_LIST::iterator ite = m_WaitRenderFrameData.begin();
	for (; ite != m_WaitRenderFrameData.end(); ++ite)
	{
		// 把帧数据放入空闲队列回收利用
		PushToFreeList(*ite);
	}
	m_WaitRenderFrameData.clear();
}
