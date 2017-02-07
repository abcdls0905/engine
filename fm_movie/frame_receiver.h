//--------------------------------------------------------------------
// �ļ���:		FrameReceiver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��10��16��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __FRAME_RECEIVER_H__
#define __FRAME_RECEIVER_H__

#include "../utils/lock_util.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include <list>

// ֡���ݽ�����

class FrameReceiver
{
public:
	FrameReceiver(unsigned int nFrameSize);
	virtual ~FrameReceiver();

	bool Init();

	// ��ʼ����
	bool StartListen(const char* szMyIP, unsigned short nPort);
	// ֹͣ����
	bool StopListen();

	// ���֡�ߴ�
	size_t GetFrameSize() const { return m_nFrameSize; }

	// �ӵȴ������֡������ȡ��һ֡
	char* PopFromWaitList();
	// ���һ����֡������֡������
	void PushToFreeList(char* pFrameBuffer);

private:
	// �����߳�
	static void __cdecl WorkerProc(void* lpParameter);

	// ���һ����֡���ȴ������֡������
	void PushToWaitList(char* pFrameBuffer);
	// �ӿ���֡��������һ����֡
	char* PopFromFreeList();

	// �ͷŵȴ���Ⱦ��֡����
	void ReleaseWaitList();

	// �������֡
	void AllocFreeFrame(unsigned int nAddFrameCount);

private:
	bool m_bInit;

	unsigned int m_nFrameSize;

	// �ȴ���Ⱦ��֡����
	typedef std::list<char*> FRAME_LIST;
	FRAME_LIST m_WaitRenderFrameData;
	// ����֡����
	TArrayPod<char*, 10, TCoreAlloc> m_FreeFrameData;

	struct sBuffer
	{
		unsigned int size;
		char* buffer;
	};
	// �ڴ�������
	TArrayPod<sBuffer, 10, TCoreAlloc> m_Buffers;

	CLockUtil		m_Lock;
	HANDLE			m_hThread;
	bool			m_bQuit;
	std::string		m_strIP;
	unsigned short	m_nPort;
	SOCKET			m_sock;
};

#endif // __FRAME_RECEIVER_H__
