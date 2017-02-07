//-------------------------------------------------------------------- 
// �ļ���:		win_lockval.h 
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_LOCKVAL_H
#define _SYSTEM_WIN_LOCKVAL_H

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#include <windows.h>

// ����������

class CLockVal
{
public:
	CLockVal()
	{
		m_nVal = 0;
	}

	// ��һ
	int Inc()
	{
		return InterlockedIncrement(&m_nVal);
	}

	// ��һ
	int Dec()
	{
		return InterlockedDecrement(&m_nVal);
	}

	// ����
	int Set(int val)
	{
		return InterlockedExchange(&m_nVal, val);
	}

	// ��ȡ
	int Get() const
	{
		return m_nVal;
	}

	// �ȽϺ󽻻�������ԭֵ(����ǰֵ����oldv���򽫵�ǰֵ��Ϊnewv��������oldv)
	int CompareExchange(int oldv, int newv)
	{
		return InterlockedCompareExchange(&m_nVal, newv, oldv);
	}
	
public:
	volatile LONG m_nVal;
};

#endif // _SYSTEM_WIN_LOCKVAL_H
