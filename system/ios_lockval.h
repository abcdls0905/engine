//-------------------------------------------------------------------- 
// 文件名:		ios_lockval.h 
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_LOCKVAL_H
#define _SYSTEM_IOS_LOCKVAL_H

#include <libkern/osatomic.h>

// 带锁的整数

class CLockVal
{
public:
	CLockVal()
	{
		m_value = 0;
	}

	// 加一
	int Inc()
	{
		OSAtomicIncrement32(&m_value);
		return m_value;
	}

	// 减一
	int Dec()
	{
		OSAtomicDecrement32(&m_value);
		return m_value;
	}

	// 设置
	int Set(int val)
	{
		m_value = val;
		return m_value;
	}

	// 获取
	int Get() const
	{
		return m_value;
	}

	// 比较后交换，返回原值(若当前值等于oldv，则将当前值设为newv，并返回oldv)
	int CompareExchange(int oldv, int newv)
	{
		if (OSAtomicCompareAndSwap32(oldv, newv, &m_value))
		{
			return oldv;
		}
		else
		{
			return m_value;
		}
	}
	
public:
	volatile int32_t m_value;
};

#endif // _SYSTEM_IOS_LOCKVAL_H
