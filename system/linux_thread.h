//--------------------------------------------------------------------
// 文件名:		linux_thread.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_THREAD_H
#define _SYSTEM_LINUX_THREAD_H

#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

// 线程

class CThread
{
private:
	typedef void (__cdecl* LOOP_FUNC)(void*);
	typedef bool (__cdecl* INIT_FUNC)(void*);
	typedef bool (__cdecl* SHUT_FUNC)(void*);
	
	// 线程函数
	static void* WorkerProc(void* lpParameter)
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
			
			if (sleep_ms > 0)
			{
				struct timespec ts;
				
				ts.tv_sec = 0;
				ts.tv_nsec = sleep_ms * 1000000;
				
				nanosleep(&ts, NULL);
			}
			else if (sleep_ms == 0)
			{
				sched_yield();
			}
		}
		
		if (pthis->m_ShutFunc)
		{
			pthis->m_ShutFunc(context);
		}
		
		return (void*)0;
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
		m_hThread = -1;
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
		
		int res = pthread_create(&m_hThread, NULL, WorkerProc, this);
			
		return (0 == res);
	}
	
	// 停止线程
	bool Stop()
	{
		m_bQuit = true;
	
		if (m_hThread != -1)
		{
			pthread_join(m_hThread, NULL);
			m_hThread = -1;
		}
	
		return true;
	}
	
private:
	CThread();
	CThread(const CThread&);
	CThread& operator=(const CThread&);
	
private:
	LOOP_FUNC m_LoopFunc;
	INIT_FUNC m_InitFunc;
	SHUT_FUNC m_ShutFunc;
	void* m_pContext;
	int m_nSleep;
	int m_nStackSize;
	bool m_bQuit;
	pthread_t m_hThread;
};

// 线程等待

class CThreadWaiter
{
public:
	CThreadWaiter()
	{
		pthread_cond_init(&m_cond, NULL);
		pthread_mutex_init(&m_mutex, NULL); 
	}

	~CThreadWaiter()
	{
		pthread_cond_destroy(&m_cond);
		pthread_mutex_destroy(&m_mutex);
	}
	
	// 等待线程唤醒
	bool Wait(int ms)
	{
		bool result = false;
		
		if (ms < 0)
		{
			// 无限时间等待
			pthread_mutex_lock(&m_mutex);
			
			int res = pthread_cond_wait(&m_cond, &m_mutex);
			
			if (0 == res)
			{
				result = true;
			}
			
			pthread_mutex_unlock(&m_mutex);
		}
		else
		{
			struct timespec ts;
			
			clock_gettime(CLOCK_REALTIME, &ts);
			
			ts.tv_nsec += ms * 1000000;
			
			if (ts.tv_nsec > 999999999)
			{
				ts.tv_sec += 1;
				ts.tv_nsec -= 1000000000;
			}
			
			pthread_mutex_lock(&m_mutex);
			
			int res = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
			
			if (0 == res)
			{
				result = true;
			}
			
			pthread_mutex_unlock(&m_mutex);
		}
		
		return result;
	}
	
	// 唤醒
	bool Signal()
	{
		return pthread_cond_signal(&m_cond) == 0;
	}
	
private:
	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
};

#endif // _SYSTEM_LINUX_THREAD_H

