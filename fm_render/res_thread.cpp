//--------------------------------------------------------------------
// 文件名:		res_thread.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年7月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "res_thread.h"
#include "../utils/tick_timer.h"
#include "../visual/i_res_loader.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/portable.h"
#include "common_device.h"

// 记录资源加载时间消耗的开关
#ifdef _DEBUG
//#define LOG_LOAD_TIME
#endif // _DEBUG

// 获得当前时刻
#define performance_time Port_GetPerformanceTime

// CResThread

void CResThread::WorkerProc(void* pParameter)
{
	
	CResThread* pthis = (CResThread*)pParameter;
	
	pthis->WorkerFunc();

}

bool CResThread::InitWorkerProc(void* pParameter)
{
	CResThread* pthis = (CResThread*)pParameter;

#ifdef GPU_MULTITHREADING
	if(g_ResourceContext)
		g_ResourceContext->StartResourceContext();
#endif
    
	return true;
}

CResThread::CResThread()
{
	m_pThread = (CThread*)CORE_ALLOC(sizeof(CThread));
	new (m_pThread) CThread(WorkerProc, InitWorkerProc, NULL, this, -1, 0);
	m_pThreadWaiter = CORE_NEW(CThreadWaiter);
}

CResThread::~CResThread()
{
	CORE_DELETE(m_pThread);
	CORE_DELETE(m_pThreadWaiter);
}

bool CResThread::Initialize()
{
	m_pThread->Start();
	
	return true;
}

bool CResThread::Shutdown()
{
	m_pThread->Stop();
	
	return true;
}

void CResThread::Clear()
{
	// 释放所有的加载器，防止内存泄漏
	{
		CAutoLock autolock(m_InMsgLock);

		in_msg_t msg;

		while (m_InMsg.Get(msg))
		{
			msg.pLoader->Release();
		}
	}

	{
		CAutoLock autolock(m_OutMsgLock);

		out_msg_t msg;

		while (m_OutMsg.Get(msg))
		{
			msg.pLoader->Release();
		}
	}
}

bool CResThread::GetInMsg(in_msg_t& msg)
{
	if (m_InMsg.IsEmpty())
	{
		return false;
	}
	
	CAutoLock autolock(m_InMsgLock);
	
	return m_InMsg.Get(msg);
}

bool CResThread::GetOutMsg(out_msg_t& msg)
{
	if (m_OutMsg.IsEmpty())
	{
		return false;
	}
	
	CAutoLock autolock(m_OutMsgLock);
	
	return m_OutMsg.Get(msg);
}

void CResThread::WorkerFunc()
{
	if (m_InMsg.IsEmpty())
	{
		m_pThreadWaiter->Wait(100);
	}
	
	if (m_InMsg.IsEmpty())
	{
		return;
	}
	
	in_msg_t msg;
	
	while (GetInMsg(msg))
	{
		IResLoader* loader = msg.pLoader;

		if (loader->GetRefs() == 1)
		{
			// 资源主体已经被释放，无需再加载
			Response(msg.nCategory, loader, RESULT_FAILED);
			continue;
		}

#ifdef LOG_LOAD_TIME
		double load_time = performance_time();
#endif

		bool succeed = loader->Load(true);
		
#ifdef LOG_LOAD_TIME
		char info[256];

		if (succeed)
		{
			SafeSprintf(info, sizeof(info), "use %f seconds, load: %s succeed", 
				performance_time() - load_time, msg.pLoader->GetName());
		}
		else
		{
			SafeSprintf(info, sizeof(info), "use %f seconds, load: %s failed", 
				performance_time() - load_time, msg.pLoader->GetName());
		}

		CORE_TRACE(info);
#endif
		
		if (succeed)
		{
			Response(msg.nCategory, loader, RESULT_SUCCEED);
		}
		else
		{
			Response(msg.nCategory, loader, RESULT_FAILED);
		}
	}
}

bool CResThread::Execute()
{
	if (m_OutMsg.IsEmpty())
	{
		return true;
	}

	CTickTimer ticks;
	out_msg_t msg;
	
	while (GetOutMsg(msg))
	{
		IResLoader* loader = msg.pLoader;
		
		if (loader->GetRefs() > 1)
		{
			if (msg.nResult == RESULT_SUCCEED)
			{
				if (loader->Create(true))
				{
					// 结束创建，释放加载器
					loader->EndCreate(msg.nCategory, true);
				}
				else
				{
					// 结束创建，释放加载器
					loader->EndCreate(msg.nCategory, false);
				}
			}
			else
			{
				// 结束创建，释放加载器
				loader->EndCreate(msg.nCategory, false);
			}
		}
		
		// 减少引用
		loader->Release();
		
		// 控制运行的时间
		if (ticks.GetStartedMillisec() > 20)
		{
			break;
		}
	}
	
	return true;
}

bool CResThread::Request(int category, IResLoader* loader)
{
	Assert(loader != NULL);
	
	in_msg_t data;

	data.nCategory = category;
	data.pLoader = loader;
	
	// 增加引用
	loader->IncRefs();
	
	CAutoLock autolock(m_InMsgLock);

	if (!m_InMsg.Put(data))
	{
		CORE_TRACE("(CResThread::Request)put msg failed");
		loader->Release();
		return false;
	}

	// 唤醒线程
	m_pThreadWaiter->Signal();
	
	return true;
}

bool CResThread::Response(int category, IResLoader* loader, int result)
{
	Assert(loader != NULL);
	
	out_msg_t data;
	
	data.nCategory = category;
	data.pLoader = loader;
	data.nResult = result;
	
	CAutoLock autolock(m_OutMsgLock);
	
	if (!m_OutMsg.Put(data))
	{
		CORE_TRACE("(CResThread::Response)put msg failed");
		loader->Release();
		return false;
	}
	
	return true;
}

int CResThread::GetTaskCount()
{
	return int(m_InMsg.GetMsgCount() + m_OutMsg.GetMsgCount());
}
