//--------------------------------------------------------------------
// �ļ���:		mutex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2006��8��23��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_MUTEX_H
#define _UTILS_MUTEX_H

#include "../public/macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_mutex.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_mutex.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_mutex.h"
#endif // FX_SYSTEM_IOS

// �Զ��ӽ���

class CAutoMutex
{
public:
	explicit CAutoMutex(CMutex& mutex): m_Mutex(mutex)
	{
		Assert(mutex.IsValid());

		m_Mutex.Lock();
	}

	~CAutoMutex()
	{
		m_Mutex.Unlock();
	}

private:
	CAutoMutex();
	CAutoMutex(const CAutoMutex&);
	CAutoMutex& operator=(const CAutoMutex&);

private:
	CMutex& m_Mutex;
};

#endif // _UTILS_MUTEX_H
