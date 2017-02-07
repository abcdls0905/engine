//--------------------------------------------------------------------
// �ļ���:		IFrameReceiver.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#include "i_frame_receiver.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/module.h"

/// entity: IFrameReceiver
/// \brief ֡���ݽ���������
DECLARE_ABSTRACT_ENTITY("", IFrameReceiver, IEntity);

#define MIN_ALLOC_FRAME_COUNT 5

IFrameReceiver::IFrameReceiver()
{
	m_nFrameSize = 1024;
	m_nRecvNextFrame = 3;
}

IFrameReceiver::~IFrameReceiver()
{
	// �ͷ������ڴ�
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
	// ���ж���Ϊ��
	if (m_FreeFrameData.empty())
	{
		// ������֡
		const int nAddFrameCount = MIN_ALLOC_FRAME_COUNT;
		AllocFreeFrame(nAddFrameCount);

		if (m_FreeFrameData.empty())
		{
			Assert(0);
			return NULL;
		}

		return PopFromFreeList();
	}

	// ֱ�Ӵӿ��ж�����ȡ��һ֡
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

	// �����ڴ��
	sBuffer buffer;
	buffer.buffer = pBuffer;
	buffer.size = nBufferSize;
	m_Buffers.push_back(buffer);

	// ������ж���
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
		// ��֡���ݷ�����ж��л�������
		PushToFreeList(*ite);
	}
	m_WaitFrameData.clear();
}

void IFrameReceiver::ClearProcList()
{
	FRAME_LIST::iterator ite = m_ProcessingFrameData.begin();
	for (; ite != m_ProcessingFrameData.end(); ++ite)
	{
		// ��֡���ݷ�����ж��л�������
		PushToFreeList(*ite);
	}
	m_ProcessingFrameData.clear();
}

void IFrameReceiver::Update()
{
	// �ӵȴ�������ȡ������֡���д���
	if (PopWholeWaitList())
	{
		ProcessFrameData();

		// ��մ�����֡����
		ClearProcList();
	}

}

void IFrameReceiver::ResetMemory()
{
	CAutoLock autolock(m_Lock);

	m_WaitFrameData.clear();
	m_FreeFrameData.clear();

	// �ͷ������ڴ�
	size_t nBufferSize = m_Buffers.size();
	for (size_t i = 0; i < nBufferSize; ++i)
	{
		CORE_FREE(m_Buffers[i].buffer, m_Buffers[i].size);
	}
	m_Buffers.clear();
}

void IFrameReceiver::SetFrameSize(unsigned int nFrameSize)
{
	// ��Ӧ�÷�����ʹ�ù������޸�֡�ߴ�����
	Assert(m_Buffers.empty());

	// �ͷ����ڵȴ��Ķ��в������ڴ�
	ResetMemory();
	m_nFrameSize = nFrameSize;
}
