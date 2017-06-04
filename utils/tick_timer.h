
#ifndef _UTILS_TICK_TIMER_H
#define _UTILS_TICK_TIMER_H

#include "../public/portable.h"

// 低精度计时器

class CTickTimer
{
public:
	CTickTimer()
	{
		m_nLastMilliseconds = Port_GetTickCount();
	}
	
	// 复位
	void Restart()
	{
		m_nLastMilliseconds = Port_GetTickCount();
	}
	
	// 返回与起始时间的差值
	int GetStartedMillisec() const
	{
		return Port_GetTickCount() - m_nLastMilliseconds;
	}
	
	// 返回逝去的毫秒数
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
