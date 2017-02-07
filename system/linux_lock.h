//-------------------------------------------------------------------- 
// 文件名:		linux_lock.h 
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_LOCK_H
#define _SYSTEM_LINUX_LOCK_H

#include <pthread.h>

// 共享资源互斥访问锁

class CLockUtil
{
public:
	CLockUtil() 
	{ 
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_Handle, &attr); 
	}

	~CLockUtil() 
	{ 
		pthread_mutex_destroy(&m_Handle); 
	}

	// 锁定
	void Lock() 
	{ 
		pthread_mutex_lock(&m_Handle); 
	}

	// 释放
	void Unlock() 
	{	
		pthread_mutex_unlock(&m_Handle); 
	}

private:
	pthread_mutex_t m_Handle;
};

#endif // _SYSTEM_LINUX_LOCK_H
