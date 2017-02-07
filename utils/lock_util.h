//-------------------------------------------------------------------- 
// 文件名:		lock_util.h 
// 内  容:		
// 说  明:		
// 创建日期:	2002年7月9日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_LOCK_UTIL_H
#define _UTILS_LOCK_UTIL_H

#include "../public/macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_lock.h"
	#include "../system/win_lockval.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_lock.h"
	#include "../system/linux_lockval.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_lock.h"
	#include "../system/ios_lockval.h"
#endif // FX_SYSTEM_IOS

// 用结构析构函数自动加解锁

class CAutoLock
{
public:
	explicit CAutoLock(CLockUtil& lock): m_Lock(lock)
	{
		m_Lock.Lock();
	}
	
	~CAutoLock() 
	{ 
		m_Lock.Unlock(); 
	}

private:
	CAutoLock();
	CAutoLock(const CAutoLock&);
	CAutoLock& operator=(const CAutoLock&);

private:
	CLockUtil& m_Lock;
};

#endif // _UTILS_LOCK_UTIL_H
