//--------------------------------------------------------------------
// �ļ���:		ios_thread.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_THREAD_H
#define _SYSTEM_IOS_THREAD_H

#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>

// �߳�

class CThread
{
private:
	typedef void (__cdecl* LOOP_FUNC)(void*);
	typedef bool (__cdecl* INIT_FUNC)(void*);
	typedef bool (__cdecl* SHUT_FUNC)(void*);
	
	// �̺߳���
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
		m_hThread = 0;
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
		
		int res = pthread_create(&m_hThread, NULL, WorkerProc, this);
			
		return (0 == res);
	}
	
	// ֹͣ�߳�
	bool Stop()
	{
		m_bQuit = true;
	
		if (m_hThread != 0)
		{
			pthread_join(m_hThread, NULL);
			m_hThread = 0;
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

// �̵߳ȴ�

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
	
	// �ȴ��̻߳���
	bool Wait(int ms)
	{
		bool result = false;
		
		if (ms < 0)
		{
			// ����ʱ��ȴ�
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
			struct timeval now;
			
			gettimeofday(&now, NULL);
			
			ts.tv_sec = now.tv_sec;
			ts.tv_nsec = now.tv_usec * 1000;
			//clock_gettime(CLOCK_REALTIME, &ts);
			//clock_serv_t cclock;
			//mach_timespec_t mts;
			
			//host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
			//clock_get_time(cclock, &mts);
			//mach_port_deallocate(mach_task_self(), cclock);
			
			//ts.tv_sec = mts.tv_sec;
			//ts.tv_nsec = mts.tv_nsec;
			
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
	
	// ����
	bool Signal()
	{
		return pthread_cond_signal(&m_cond) == 0;
	}
	
private:
	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
};

#endif // _SYSTEM_IOS_THREAD_H
