//--------------------------------------------------------------------
// �ļ���:		win_thread.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��10��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_THREAD_H
#define _SYSTEM_WIN_THREAD_H

#include <windows.h>
#include <process.h>

// �߳�

class CThread
{
private:
	typedef void (__cdecl* LOOP_FUNC)(void*);
	typedef bool (__cdecl* INIT_FUNC)(void*);
	typedef bool (__cdecl* SHUT_FUNC)(void*);
	
	// �̺߳���
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
	
	// �Ƿ����˳��߳�
	void SetQuit(bool value) { m_bQuit = value; }
	bool GetQuit() const { return m_bQuit; }
	
	// �����߳�
	bool Start()
	{
		m_bQuit = false;
		m_hThread = (HANDLE)_beginthread(WorkerProc, m_nStackSize, this);
			
		return true;
	}
	
	// ֹͣ�߳�
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
	// �ȴ��߳̽���
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
	// �ȴ��߳̽���
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

// �̵߳ȴ�

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
	
	// �ȴ��̻߳���
	bool Wait(int ms)
	{
		if (ms < 0)
		{
			// ����ʱ��ȴ�
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
	
	// ����
	bool Signal()
	{
		return SetEvent(m_hEvent) == TRUE;
	}
	
private:
	HANDLE m_hEvent;
};

#endif // _SYSTEM_WIN_THREAD_H

