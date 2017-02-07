//--------------------------------------------------------------------
// �ļ���:		res_thread.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��7��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TResThreadAlloc& src) {}
};

// ��Դ�����߳�

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

	// ��ʼ��
	bool Initialize();
	// ����
	bool Shutdown();
	// ���������������
	void Clear();
	// ��������
	void WorkerFunc();
	// ��ȡ�������
	bool Execute();
	// �������
	bool Request(int category, IResLoader* loader);
	// �����������
	int GetTaskCount();
	
private:
	CResThread(const CResThread&);
	CResThread& operator=(const CResThread&);

	// �����Ӧ
	bool Response(int category, IResLoader* loader, int result);
	// ���������Ϣ
	bool GetInMsg(in_msg_t& msg);
	// ��������Ϣ
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
