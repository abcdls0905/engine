//--------------------------------------------------------------------
// �ļ���:		linux_timer.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_TIMER_H
#define _SYSTEM_LINUX_TIMER_H

#include <sys/time.h>

// �߾��ȼ�ʱ��

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

	// ��ʼ��
	void Initialize()
	{
		struct timeval tv;
		
		gettimeofday(&tv, NULL);
		
		m_dLastTime = ToSeconds(&tv);
		m_bInit = true;
	}
	
	// �߾��ȼ�ʱ���Ƿ����
	bool CanUseQPF() const
	{
		return true;
	}
	
	// �Ƿ�ʹ�ø߾��ȼ�ʱ��
	void SetUseQPF(bool flag)
	{
	}
	
	// ��ǰʱ����ֵ
	double GetCurrentTime()
	{
		struct timeval tv;
		
		gettimeofday(&tv, NULL);
		
		return ToSeconds(&tv);
	}
	
	// �����ȥ������
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
	
	// �����ȥ�ĺ�����
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
