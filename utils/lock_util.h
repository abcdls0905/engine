//-------------------------------------------------------------------- 
// �ļ���:		lock_util.h 
// ��  ��:		
// ˵  ��:		
// ��������:	2002��7��9��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// �ýṹ���������Զ��ӽ���

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
