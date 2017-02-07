//--------------------------------------------------------------------
// 文件名:		IFrameReceiver.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "i_frame_receiver.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/module.h"

/// entity: IFrameReceiver
/// \brief 帧数据接收器基类
DECLARE_ABSTRACT_ENTITY("", IFrameReceiver, IEntity);

#define MIN_ALLOC_FRAME_COUNT 5

IFrameReceiver::IFrameReceiver()
{
	m_nFrameSize = 1024;
	m_nRecvNextFrame = 3;
}

IFrameReceiver::~IFrameReceiver()
{
	// 释放所有内存
	size_t nBufferSize = m_Buffers.size();
	for (size_t i = 0; i < nBufferSize; ++i)
	{
		CORE_FREE(m_Buffers[i].buffer, m_Buffers[i].size);
	}
	m_Buffers.clear();
}

bool IFrameReceiver::Init(const IVarList& args)
{
	return true;
}

bool IFrameReceiver::Shut()
{
	return true;
}

bool IFrameReceiver::PopWholeWaitList()
{
	if (m_WaitFrameData.empty())
	{
		return false;
	}

	CAutoLock autolock(m_Lock);

	m_ProcessingFrameData.assign(m_WaitFrameData.begin(), 
		m_WaitFrameData.end());
	m_WaitFrameData.clear();

	return true;
}

//char* IFrameReceiver::PopFromWaitList()
//{
//	if (m_WaitFrameData.empty())
//	{
//		return NULL;
//	}
//
//	CAutoLock autolock(m_Lock);
//	char* pFrameBuffer = m_WaitFrameData.front();
//	m_WaitFrameData.pop_front();
//	return pFrameBuffer;
//}


void IFrameReceiver::PushToWaitList(char* pFrameBuffer)
{
	CAutoLock autolock(m_Lock);
	m_WaitFrameData.push_back(pFrameBuffer);
}


char* IFrameReceiver::PopFromFreeList()
{
	CAutoLock autolock(m_Lock);
	// 空闲队列为空
	if (m_FreeFrameData.empty())
	{
		// 创建的帧
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


void IFrameReceiver::PushToFreeList(char* pFrameBuffer)
{
	CAutoLock autolock(m_Lock);
	if (pFrameBuffer)
	{
		m_FreeFrameData.push_back(pFrameBuffer);
	}
}

void IFrameReceiver::AllocFreeFrame(unsigned int nAddFrameCount)
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
		PushToFreeList(pBuffer + i * m_nFrameSize);
	}
}

void IFrameReceiver::ClearWaitList()
{
	CAutoLock autolock(m_Lock);

	FRAME_LIST::iterator ite = m_WaitFrameData.begin();
	for (; ite != m_WaitFrameData.end(); ++ite)
	{
		// 把帧数据放入空闲队列回收利用
		PushToFreeList(*ite);
	}
	m_WaitFrameData.clear();
}

void IFrameReceiver::ClearProcList()
{
	FRAME_LIST::iterator ite = m_ProcessingFrameData.begin();
	for (; ite != m_ProcessingFrameData.end(); ++ite)
	{
		// 把帧数据放入空闲队列回收利用
		PushToFreeList(*ite);
	}
	m_ProcessingFrameData.clear();
}

void IFrameReceiver::Update()
{
	// 从等待队列里取出所有帧进行处理
	if (PopWholeWaitList())
	{
		ProcessFrameData();

		// 清空处理后的帧队列
		ClearProcList();
	}

}

void IFrameReceiver::ResetMemory()
{
	CAutoLock autolock(m_Lock);

	m_WaitFrameData.clear();
	m_FreeFrameData.clear();

	// 释放所有内存
	size_t nBufferSize = m_Buffers.size();
	for (size_t i = 0; i < nBufferSize; ++i)
	{
		CORE_FREE(m_Buffers[i].buffer, m_Buffers[i].size);
	}
	m_Buffers.clear();
}

void IFrameReceiver::SetFrameSize(unsigned int nFrameSize)
{
	// 不应该发生在使用过程中修改帧尺寸的情况
	Assert(m_Buffers.empty());

	// 释放正在等待的队列并重置内存
	ResetMemory();
	m_nFrameSize = nFrameSize;
}
