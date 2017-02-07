//--------------------------------------------------------------------
// �ļ���:		IFrameReceiver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __IFrameReceiver_H__
#define __IFrameReceiver_H__

#include "../public/i_entity.h"
#include "../utils/lock_util.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include <list>

// ֡���ݽ�����
// ע�⣺�̳��ڱ���̳е������������DECLARE_ENTITY��ʱ�ñ���������

class IFrameReceiver: public IEntity
{
protected:
	typedef std::list<char*> FRAME_LIST;

public:
	IFrameReceiver();
	virtual ~IFrameReceiver() = 0;

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ���֡�ߴ�
	size_t GetFrameSize() const { return m_nFrameSize; }

	// ���һ����֡���ȴ������֡������
	void PushToWaitList(char* pFrameBuffer);
	// �ӿ���֡��������һ����֡
	char* PopFromFreeList();

	virtual void Update();

	virtual void Reset() {}

	// ��յȴ���֡����
	void ClearWaitList();

protected:
	void SetFrameSize(unsigned int nFrameSize);

private:
	virtual void ProcessFrameData() = 0;

private:
	// �ӵȴ������֡������ȡ������֡
	bool PopWholeWaitList();
	// ������ڴ����֡����
	void ClearProcList();
	// �ӵȴ������֡������ȡ��һ֡
	//char* PopFromWaitList();
	// ���һ����֡������֡������
	void PushToFreeList(char* pFrameBuffer);

	// �������֡
	void AllocFreeFrame(unsigned int nAddFrameCount);
	// �����ڴ�
	void ResetMemory();

protected:
	// ���ڴ����֡����
	// �ö���ֻ�����߳��ﴦ������Ҫ����
	FRAME_LIST m_ProcessingFrameData;

private:
	CLockUtil m_Lock;
	unsigned int m_nFrameSize;
	bool m_bRecvNextFrame;			// ���Խ�����һ֡
	int m_nRecvNextFrame;

	// �ȴ������֡����
	FRAME_LIST m_WaitFrameData;

	// ����֡����
	TArrayPod<char*, 10, TCoreAlloc> m_FreeFrameData;

	struct sBuffer
	{
		unsigned int size;
		char* buffer;
	};
	// �ڴ�������
	TArrayPod<sBuffer, 10, TCoreAlloc> m_Buffers;
};


#endif // __IFrameReceiver_H__
