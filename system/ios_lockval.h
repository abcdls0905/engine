//-------------------------------------------------------------------- 
// �ļ���:		ios_lockval.h 
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_LOCKVAL_H
#define _SYSTEM_IOS_LOCKVAL_H

#include <libkern/osatomic.h>

// ����������

class CLockVal
{
public:
	CLockVal()
	{
		m_value = 0;
	}

	// ��һ
	int Inc()
	{
		OSAtomicIncrement32(&m_value);
		return m_value;
	}

	// ��һ
	int Dec()
	{
		OSAtomicDecrement32(&m_value);
		return m_value;
	}

	// ����
	int Set(int val)
	{
		m_value = val;
		return m_value;
	}

	// ��ȡ
	int Get() const
	{
		return m_value;
	}

	// �ȽϺ󽻻�������ԭֵ(����ǰֵ����oldv���򽫵�ǰֵ��Ϊnewv��������oldv)
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
