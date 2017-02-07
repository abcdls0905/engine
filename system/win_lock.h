//-------------------------------------------------------------------- 
// �ļ���:		win_lock.h 
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_LOCK_H
#define _SYSTEM_WIN_LOCK_H

#include <windows.h>

// ������Դ���������

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

	// ����
	void Lock() 
	{ 
		EnterCriticalSection(&m_csHandle); 
	}

	// �ͷ�
	void Unlock() 
	{	
		LeaveCriticalSection(&m_csHandle); 
	}

private:
	CRITICAL_SECTION m_csHandle;
};

#endif // _SYSTEM_WIN_LOCK_H
