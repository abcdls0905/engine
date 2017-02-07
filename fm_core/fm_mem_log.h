

#ifndef _CMEM_LOG_H
#define _CMEM_LOG_H

#include "../public/macros.h"
#include "../utils/share_mem.h"
#include "../utils/lock_util.h"

#include <new>


//��־����
struct Log_Info
{
   const char*	FileName;//��־����
   LOG_TYPE     LogType;//��־����
};

Log_Info g_log_info[LOG_TYPE_MAX] = 
{
	//FileName      LogType 
	{ "render.log",  LOG_RENDER    },
	{ "gui.log",     LOG_GUI       },
	{ "net.log",     LOG_NET       },
	{ "core.log",    LOG_CORE      },
	{ "physics.log", LOG_PHYSICS   },
	{ "logic.log",   LOG_LOGIC     },
	{ "fileio.log",  LOG_FILEIO    },
	{ "frame.log",   LOG_FRAME     },
	{ "perf.log",    LOG_PERF      },
};

 
class CMemLog
{
public:
	//��־���ݴ�����
	enum LogBufferMax
	{
		ItemTexMax = 1024,//ÿ����󳤶�1024�ֽ�
		ItemTexLineMax = 4096,//���4096��
	};
    //��Ŵ�С1MB����

	struct ItemLine
	{
		char data[ItemTexMax];
	};
    
	struct LogList
	{
		volatile size_t m_write;//д����
		volatile size_t m_read;//д����

		ItemLine Log[ItemTexLineMax];
	};
public:
	static CMemLog* instrance(LOG_TYPE log_type)
	{
		if( NULL == s_pInstance[log_type] )
		{
			CLockUtil lock;
			lock.Lock();
			if( NULL == s_pInstance[log_type])
			{
				static CMemLog s_memlog[LOG_TYPE_MAX];
				s_pInstance[log_type] = &s_memlog[log_type];
	    		new (s_pInstance[log_type]) CMemLog(log_type);
			}
			lock.Unlock();
		}
		return s_pInstance[log_type];
	}



	inline bool Write(const char * pstr)
	{
		if(!m_pLog) return false;

		volatile size_t write =  m_pLog->m_write;
		volatile size_t read = m_pLog->m_read;

		size_t n_abs = 0;
		if( write > read )
		{
			n_abs = write - read + 1;
		}
		else
		{
			n_abs = read - write + 1;
		}

		if( n_abs >= ItemTexLineMax)
			return false;

//		if((write+1) == read)
//			return false;

		write%=ItemTexLineMax;
		memcpy(m_pLog->Log[write].data,pstr,ItemTexMax);
		write++;
		m_pLog->m_write =write;
		return true;
	}

	inline char* Read()
	{
		if(!m_pLog) return NULL;
		volatile size_t read = m_pLog->m_read;
		volatile size_t write =  m_pLog->m_write;
		//������������������д���������ʱ��,�Ͳ��ڶ�ȡ
		if( read == write )
			return NULL;
		static char tbuffer[ItemTexMax];//��������
		read%=ItemTexLineMax;
		memcpy(tbuffer,m_pLog->Log[read].data,ItemTexMax);
		read++;
		m_pLog->m_read = read;
        return tbuffer;
	}
private:

	CMemLog(LOG_TYPE LogType)
	{
#ifdef FX_SYSTEM_WINDOWS
		bool bExst;
		m_pmem.Create(g_log_info[LogType].FileName,sizeof(LogList),&bExst);
		m_pLog = (LogList*)m_pmem.GetMem();

		if(!bExst)
		{
			m_pLog->m_read = 0;
			m_pLog->m_write = 0;
		}

#else
		m_pmem = new char[sizeof(LogList)];
		m_pLog = (LogList*)m_pmem;
		m_pLog->m_read = 0;
		m_pLog->m_write = 0;
#endif
	}

 	CMemLog():m_pLog(NULL)
 	{
 	}

	~CMemLog()
	{
	  if(!m_pLog)
		  return;
#ifdef FX_SYSTEM_WINDOWS
	  m_pmem.Destroy();
#else
	  if(m_pmem)
		  delete [] m_pmem;
#endif
	  m_pLog = NULL;
	}

private:
#ifdef FX_SYSTEM_WINDOWS
	CShareMem  m_pmem;
#else
	char*      m_pmem;
#endif

	LogList*   m_pLog;
    static CMemLog* s_pInstance[LOG_TYPE_MAX];
};

CMemLog* CMemLog::s_pInstance[LOG_TYPE_MAX] = {NULL};



#endif