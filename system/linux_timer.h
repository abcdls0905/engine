//--------------------------------------------------------------------
// 文件名:		linux_timer.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_TIMER_H
#define _SYSTEM_LINUX_TIMER_H

#include <sys/time.h>

// 高精度计时器

class CGameTimer
{
public:
	CGameTimer()
	{
		m_bInit = false;
		m_dLastTime = 0.0;
	}
	
	~CGameTimer()
	{
	}

	// 初始化
	void Initialize()
	{
		struct timeval tv;
		
		gettimeofday(&tv, NULL);
		
		m_dLastTime = ToSeconds(&tv);
		m_bInit = true;
	}
	
	// 高精度计时器是否可用
	bool CanUseQPF() const
	{
		return true;
	}
	
	// 是否使用高精度计时器
	void SetUseQPF(bool flag)
	{
	}
	
	// 当前时间数值
	double GetCurrentTime()
	{
		struct timeval tv;
		
		gettimeofday(&tv, NULL);
		
		return ToSeconds(&tv);
	}
	
	// 获得逝去的秒数
	double GetElapseTime(double expect = 0.0)
	{
		struct timeval tv;
		
		gettimeofday(&tv, NULL);
		
		double now = ToSeconds(&tv);
		double elapse = now - m_dLastTime;
		
		if (elapse >= expect)
		{
			m_dLastTime = now;
		}
		
		return elapse;
	}
	
	// 获得逝去的毫秒数
	int GetElapseMillisec(int expect = 0)
	{
		struct timeval tv;
		
		gettimeofday(&tv, NULL);
		
		double now = ToSeconds(&tv);
		double elapse = now - m_dLastTime;
		
		if (elapse >= expect)
		{
			m_dLastTime = now;
		}
		
		return (int)(elapse * 1000.0);
	}
	
private:
	double ToSeconds(struct timeval* tv)
	{
		return (double)tv->tv_sec + (double)tv->tv_usec * 0.000001;
	}
	
private:
	bool m_bInit;
	double m_dLastTime;
};

#endif // _SYSTEM_LINUX_TIMER_H
