//-------------------------------------------------------------------- 
// 文件名:		win_lock.h 
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_LOCK_H
#define _SYSTEM_WIN_LOCK_H

#include <windows.h>

// 共享资源互斥访问锁

class CLockUtil
{
public:
	CLockUtil() 
	{ 
		InitializeCriticalSection(&m_csHandle); 
	}

	~CLockUtil() 
	{ 
		DeleteCriticalSection(&m_csHandle); 
	}

	// 锁定
	void Lock() 
	{ 
		EnterCriticalSection(&m_csHandle); 
	}

	// 释放
	void Unlock() 
	{	
		LeaveCriticalSection(&m_csHandle); 
	}

private:
	CRITICAL_SECTION m_csHandle;
};

#endif // _SYSTEM_WIN_LOCK_H
