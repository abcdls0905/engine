//--------------------------------------------------------------------
// 文件名:		mutex.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年8月23日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 自动加解锁

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
