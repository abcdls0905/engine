//--------------------------------------------------------------------
// 文件名:		win_thread.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年10月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_THREAD_H
#define _SYSTEM_WIN_THREAD_H

#include <windows.h>
#include <process.h>

// 线程

class CThread
{
private:
	typedef void (__cdecl* LOOP_FUNC)(void*);
	typedef bool (__cdecl* INIT_FUNC)(void*);
	typedef bool (__cdecl* SHUT_FUNC)(void*);
	
	// 线程函数
	static void __cdecl WorkerProc(void* lpParameter)
	{
		CThread* pthis = (CThread*)lpParameter;
		LOOP_FUNC loop_func = pthis->m_LoopFunc;
		void* context = pthis->m_pContext;
		int sleep_ms = pthis->m_nSleep;
		
		if (pthis->m_InitFunc)
		{
			pthis->m_InitFunc(context);
		}
		
		while (!pthis->m_bQuit)
		{
			loop_func(context);
			
			if (sleep_ms >= 0)
			{
				Sleep(sleep_ms);
			}
		}
		
		if (pthis->m_ShutFunc)
		{
			pthis->m_ShutFunc(context);
		}
	}
	
public:
	CThread(LOOP_FUNC loop_func, INIT_FUNC init_func, SHUT_FUNC shut_func, 
		void* context, int sleep_ms, int stack_size)
	{
		m_LoopFunc = loop_func;
		m_InitFunc = init_func;
		m_ShutFunc = shut_func;
		m_pContext = context;
		m_nSleep = sleep_ms;		
		m_nStackSize = stack_size;
		m_bQuit = false;
		m_hThread = NULL;
	}
	
	~CThread()
	{
	}
	
	// 是否已退出线程
	void SetQuit(bool value) { m_bQuit = value; }
	bool GetQuit() const { return m_bQuit; }
	
	// 启动线程
	bool Start()
	{
		m_bQuit = false;
		m_hThread = (HANDLE)_beginthread(WorkerProc, m_nStackSize, this);
			
		return true;
	}
	
	// 停止线程
	bool Stop()
	{
		m_bQuit = true;
	
		if (m_hThread != NULL)
		{
			WaitThreadExit(m_hThread);
			m_hThread = NULL;
		}
	
		return true;
	}
	
private:
	CThread();
	CThread(const CThread&);
	CThread& operator=(const CThread&);
	
	/*
	// 等待线程结束
	bool WaitThreadExit(HANDLE handle)
	{
		DWORD exit_code;

		if (GetExitCodeThread(handle, &exit_code) == FALSE)
		{
			return false;
		}

		while (exit_code == STILL_ACTIVE)
		{
			Sleep(0);

			if (GetExitCodeThread(handle, &exit_code) == FALSE)
			{
				return false;
			}
		}

		return true;
	}
	*/
	// 等待线程结束
	bool WaitThreadExit(HANDLE handle)
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

private:
	LOOP_FUNC m_LoopFunc;
	INIT_FUNC m_InitFunc;
	SHUT_FUNC m_ShutFunc;
	void* m_pContext;
	int m_nSleep;
	int m_nStackSize;
	bool m_bQuit;
	HANDLE m_hThread;
};

// 线程等待

class CThreadWaiter
{
public:
	CThreadWaiter()
	{
		m_hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	}

	~CThreadWaiter()
	{
		CloseHandle(m_hEvent);
	}
	
	// 等待线程唤醒
	bool Wait(int ms)
	{
		if (ms < 0)
		{
			// 无限时间等待
			WaitForSingleObject(m_hEvent, INFINITE);
		}
		else
		{
			DWORD res = WaitForSingleObject(m_hEvent, ms);
			
			if (WAIT_TIMEOUT == res)
			{
				return false;
			}
		}	 
		
		return true;
	}
	
	// 唤醒
	bool Signal()
	{
		return SetEvent(m_hEvent) == TRUE;
	}
	
private:
	HANDLE m_hEvent;
};

#endif // _SYSTEM_WIN_THREAD_H

