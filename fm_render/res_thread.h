
#ifndef _RES_THREAD_H
#define _RES_THREAD_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../utils/fix_msg_buf.h"
#include "../utils/lock_util.h"
#include "../utils/thread.h"

class TResThreadAlloc
{
public:
	TResThreadAlloc() {}
	~TResThreadAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TResThreadAlloc& src) {}
};

// 资源加载线程

class IResLoader;

class CResThread
{
private:
	enum
	{
		RESULT_FAILED = 0,
		RESULT_SUCCEED = 1,
	};
	
	struct in_msg_t
	{
		int nCategory;
		IResLoader* pLoader;
	};

	struct out_msg_t
	{
		int nCategory;
		IResLoader* pLoader;
		int nResult;		
	};
	
	static bool InitWorkerProc(void* pParameter);
	static void WorkerProc(void* pParameter);
	
public:
	CResThread();
	~CResThread();

	// 初始化
	bool Initialize();
	// 结束
	bool Shutdown();
	// 清除所有输入和输出
	void Clear();
	// 处理输入
	void WorkerFunc();
	// 读取结果数据
	bool Execute();
	// 请求加载
	bool Request(int category, IResLoader* loader);
	// 获得任务数量
	int GetTaskCount();
	
private:
	CResThread(const CResThread&);
	CResThread& operator=(const CResThread&);

	// 输出回应
	bool Response(int category, IResLoader* loader, int result);
	// 获得输入消息
	bool GetInMsg(in_msg_t& msg);
	// 获得输出消息
	bool GetOutMsg(out_msg_t& msg);
	
private:
	CThread* m_pThread;
	CThreadWaiter* m_pThreadWaiter;
	TFixMsgBuf<in_msg_t, TResThreadAlloc> m_InMsg;
	CLockUtil m_InMsgLock;
	TFixMsgBuf<out_msg_t, TResThreadAlloc> m_OutMsg;
	CLockUtil m_OutMsgLock;
};

#endif // _RES_THREAD_H
