

#ifndef _CMEM_LOG_H
#define _CMEM_LOG_H

#include "../public/macros.h"
#include "../utils/share_mem.h"
#include "../utils/lock_util.h"

#include <new>


//日志数据
struct Log_Info
{
   const char*	FileName;//日志名字
   LOG_TYPE     LogType;//日志类型
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
	//日志数据存放最大
	enum LogBufferMax
	{
		ItemTexMax = 1024,//每行最大长度1024字节
		ItemTexLineMax = 4096,//最大4096行
	};
    //大概大小1MB内容

	struct ItemLine
	{
		char data[ItemTexMax];
	};
    
	struct LogList
	{
		volatile size_t m_write;//写入行
		volatile size_t m_read;//写出行

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
		//当读的数据索引大于写入的索引的时候,就不在读取
		if( read == write )
			return NULL;
		static char tbuffer[ItemTexMax];//缓冲数据
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