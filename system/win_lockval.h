//-------------------------------------------------------------------- 
// 文件名:		win_lockval.h 
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_LOCKVAL_H
#define _SYSTEM_WIN_LOCKVAL_H

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#include <windows.h>

// 带锁的整数

class CLockVal
{
public:
	CLockVal()
	{
		m_nVal = 0;
	}

	// 加一
	int Inc()
	{
		return InterlockedIncrement(&m_nVal);
	}

	// 减一
	int Dec()
	{
		return InterlockedDecrement(&m_nVal);
	}

	// 设置
	int Set(int val)
	{
		return InterlockedExchange(&m_nVal, val);
	}

	// 获取
	int Get() const
	{
		return m_nVal;
	}

	// 比较后交换，返回原值(若当前值等于oldv，则将当前值设为newv，并返回oldv)
	int CompareExchange(int oldv, int newv)
	{
		return InterlockedCompareExchange(&m_nVal, newv, oldv);
	}
	
public:
	volatile LONG m_nVal;
};

#endif // _SYSTEM_WIN_LOCKVAL_H
