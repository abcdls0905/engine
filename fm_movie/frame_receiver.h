//--------------------------------------------------------------------
// 文件名:		FrameReceiver.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __FRAME_RECEIVER_H__
#define __FRAME_RECEIVER_H__

#include "../utils/lock_util.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include <list>

// 帧数据接收类

class FrameReceiver
{
public:
	FrameReceiver(unsigned int nFrameSize);
	virtual ~FrameReceiver();

	bool Init();

	// 开始监听
	bool StartListen(const char* szMyIP, unsigned short nPort);
	// 停止监听
	bool StopListen();

	// 获得帧尺寸
	size_t GetFrameSize() const { return m_nFrameSize; }

	// 从等待处理的帧队列里取出一帧
	char* PopFromWaitList();
	// 添加一个空帧到空闲帧队列里
	void PushToFreeList(char* pFrameBuffer);

private:
	// 接收线程
	static void __cdecl WorkerProc(void* lpParameter);

	// 添加一个新帧到等待处理的帧队列里
	void PushToWaitList(char* pFrameBuffer);
	// 从空闲帧队列里获得一个空帧
	char* PopFromFreeList();

	// 释放等待渲染的帧队列
	void ReleaseWaitList();

	// 分配空闲帧
	void AllocFreeFrame(unsigned int nAddFrameCount);

private:
	bool m_bInit;

	unsigned int m_nFrameSize;

	// 等待渲染的帧队列
	typedef std::list<char*> FRAME_LIST;
	FRAME_LIST m_WaitRenderFrameData;
	// 空闲帧队列
	TArrayPod<char*, 10, TCoreAlloc> m_FreeFrameData;

	struct sBuffer
	{
		unsigned int size;
		char* buffer;
	};
	// 内存管理队列
	TArrayPod<sBuffer, 10, TCoreAlloc> m_Buffers;

	CLockUtil		m_Lock;
	HANDLE			m_hThread;
	bool			m_bQuit;
	std::string		m_strIP;
	unsigned short	m_nPort;
	SOCKET			m_sock;
};

#endif // __FRAME_RECEIVER_H__
