//-------------------------------------------------------------------- 
// �ļ���:		linux_lock.h 
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_LOCK_H
#define _SYSTEM_LINUX_LOCK_H

#include <pthread.h>

// ������Դ���������

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

	// ����
	void Lock() 
	{ 
		pthread_mutex_lock(&m_Handle); 
	}

	// �ͷ�
	void Unlock() 
	{	
		pthread_mutex_unlock(&m_Handle); 
	}

private:
	pthread_mutex_t m_Handle;
};

#endif // _SYSTEM_LINUX_LOCK_H
