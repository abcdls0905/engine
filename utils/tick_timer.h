//--------------------------------------------------------------------
// �ļ���:		tick_timer.h
// ��  ��:		
// ˵  ��:		
// ��������:	2002��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_TICK_TIMER_H
#define _UTILS_TICK_TIMER_H

#include "../public/portable.h"

// �;��ȼ�ʱ��

class CTickTimer
{
public:
	CTickTimer()
	{
		m_nLastMilliseconds = Port_GetTickCount();
	}
	
	// ��λ
	void Restart()
	{
		m_nLastMilliseconds = Port_GetTickCount();
	}
	
	// ��������ʼʱ��Ĳ�ֵ
	int GetStartedMillisec() const
	{
		return Port_GetTickCount() - m_nLastMilliseconds;
	}
	
	// ������ȥ�ĺ�����
	int GetElapseMillisec(int expect)
	{
		unsigned int cur = Port_GetTickCount();
		unsigned int dw = cur - m_nLastMilliseconds;
			
		if (dw >= (unsigned int)expect)
		{
			m_nLastMilliseconds = cur;
		}
			
		return dw;
	}

private:
	unsigned int m_nLastMilliseconds;
};

#endif // _UTILS_TICK_TIMER_H
