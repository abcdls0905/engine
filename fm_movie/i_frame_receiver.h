//--------------------------------------------------------------------
// 文件名:		IFrameReceiver.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __IFrameReceiver_H__
#define __IFrameReceiver_H__

#include "../public/i_entity.h"
#include "../utils/lock_util.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include <list>

// 帧数据接收器
// 注意：继承于本类继承的子类必须在用DECLARE_ENTITY宏时用本类做基类

class IFrameReceiver: public IEntity
{
protected:
	typedef std::list<char*> FRAME_LIST;

public:
	IFrameReceiver();
	virtual ~IFrameReceiver() = 0;

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 获得帧尺寸
	size_t GetFrameSize() const { return m_nFrameSize; }

	// 添加一个新帧到等待处理的帧队列里
	void PushToWaitList(char* pFrameBuffer);
	// 从空闲帧队列里获得一个空帧
	char* PopFromFreeList();

	virtual void Update();

	virtual void Reset() {}

	// 清空等待的帧队列
	void ClearWaitList();

protected:
	void SetFrameSize(unsigned int nFrameSize);

private:
	virtual void ProcessFrameData() = 0;

private:
	// 从等待处理的帧队列里取出所有帧
	bool PopWholeWaitList();
	// 清空正在处理的帧队列
	void ClearProcList();
	// 从等待处理的帧队列里取出一帧
	//char* PopFromWaitList();
	// 添加一个空帧到空闲帧队列里
	void PushToFreeList(char* pFrameBuffer);

	// 分配空闲帧
	void AllocFreeFrame(unsigned int nAddFrameCount);
	// 重置内存
	void ResetMemory();

protected:
	// 正在处理的帧队列
	// 该队列只在主线程里处理，不需要加锁
	FRAME_LIST m_ProcessingFrameData;

private:
	CLockUtil m_Lock;
	unsigned int m_nFrameSize;
	bool m_bRecvNextFrame;			// 可以接收下一帧
	int m_nRecvNextFrame;

	// 等待处理的帧队列
	FRAME_LIST m_WaitFrameData;

	// 空闲帧队列
	TArrayPod<char*, 10, TCoreAlloc> m_FreeFrameData;

	struct sBuffer
	{
		unsigned int size;
		char* buffer;
	};
	// 内存管理队列
	TArrayPod<sBuffer, 10, TCoreAlloc> m_Buffers;
};


#endif // __IFrameReceiver_H__
