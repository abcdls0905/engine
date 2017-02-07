//--------------------------------------------------------------------
// 文件名:		win_timer.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_TIMER_H
#define _SYSTEM_WIN_TIMER_H

#include <windows.h>
#pragma comment(lib, "winmm.lib")

// 高精度计时器

class CGameTimer
{
public:
	CGameTimer()
	{
		m_bInit = false;
		m_bUseQPF = true;
		m_dBaseTime = 0.0;
	}
	
	~CGameTimer()
	{
		if (m_bInit && (!m_bUseQPF))
		{
			timeEndPeriod(m_nPeriodMin);
		}
	}

	// 初始化
	void Initialize()
	{
		if (m_bUseQPF)
		{
			m_llFrequency.QuadPart = 0;
			m_llLastCount.QuadPart = 0;
			
			if (QueryPerformanceFrequency(&m_llFrequency))
			{
				QueryPerformanceCounter(&m_llLastCount);
				m_llStartCount = m_llLastCount;
			}
			else
			{
				m_bUseQPF = false;
			}
		}
		
		if (!m_bUseQPF)
		{
			TIMECAPS tc;
	
			timeGetDevCaps(&tc, sizeof(tc));
			m_nPeriodMin = tc.wPeriodMin;
			timeBeginPeriod(m_nPeriodMin);
			
			m_nLastMilliseconds = timeGetTime();
			m_nStartMilliseconds = m_nLastMilliseconds;
		}
		
		m_dBaseTime = 0.0;
		m_bInit = true;
	}
	
	// 高精度计时器是否可用
	bool CanUseQPF() const
	{
		LARGE_INTEGER ll;
	
		return (QueryPerformanceFrequency(&ll) != 0);
	}
	
	// 是否使用高精度计时器
	void SetUseQPF(bool flag)
	{
		m_bUseQPF = flag;
	}
	
	// 当前累计时间数值
	double GetCurrentTime()
	{
		if (m_bUseQPF)
		{
			LARGE_INTEGER ll;
	
			QueryPerformanceCounter(&ll);
	
			return (double)(ll.QuadPart - m_llStartCount.QuadPart) 
				/ (double)m_llFrequency.QuadPart;
		}
		else
		{
			DWORD cur = timeGetTime();
			
			if (cur < m_nStartMilliseconds)
			{
				// 时间回绕时的处理
				m_dBaseTime = (0xFFFFFFFF - m_nStartMilliseconds) * 0.001; 
				m_dBaseTime += 0.001;
				m_nStartMilliseconds = 0;
			}
			
			return double(cur - m_nStartMilliseconds) * 0.001 + m_dBaseTime;
		}
	}
	
	// 获得逝去的秒数
	double GetElapseTime(double expect = 0.0)
	{
		if (m_bUseQPF)
		{
			LARGE_INTEGER ll;
	
			QueryPerformanceCounter(&ll);
	
			double time = (double)(ll.QuadPart - m_llLastCount.QuadPart) 
				/ (double)m_llFrequency.QuadPart;
			
			if (time >= expect)
			{		
				m_llLastCount.QuadPart = ll.QuadPart;
			}
			
			return time;
		}
		else
		{
			DWORD cur = timeGetTime();
			DWORD dw = cur - m_nLastMilliseconds;
			
			double time = double(dw) * 0.001;
			
			if (time >= expect)
			{
				m_nLastMilliseconds = cur;
			}
			
			return time;
		}
	}
	
	// 获得逝去的毫秒数
	int GetElapseMillisec(int expect = 0)
	{
		if (m_bUseQPF)
		{
			LARGE_INTEGER ll;
	
			QueryPerformanceCounter(&ll);
			
			DWORD dw = DWORD((ll.QuadPart - m_llLastCount.QuadPart) * 1000
				/ m_llFrequency.QuadPart);
			
			if (dw >= DWORD(expect))
			{
				m_llLastCount.QuadPart = ll.QuadPart;
			}
			
			return int(dw);
		}
		else
		{
			DWORD cur = timeGetTime();
			DWORD dw = cur - m_nLastMilliseconds;
			
			if (dw >= DWORD(expect))
			{
				m_nLastMilliseconds = cur;
			}
			
			return int(dw);
		}
	}
	
private:
	bool m_bInit;
	bool m_bUseQPF;
	unsigned int m_nPeriodMin;
	LARGE_INTEGER m_llFrequency;
	LARGE_INTEGER m_llStartCount;
	LARGE_INTEGER m_llLastCount;
	DWORD m_nLastMilliseconds;
	DWORD m_nStartMilliseconds;
	double m_dBaseTime;
};

#endif // _SYSTEM_WIN_TIMER_H
