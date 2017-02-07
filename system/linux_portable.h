//--------------------------------------------------------------------
// 文件名:		linux_portable.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_PORTABLE_H
#define _SYSTEM_LINUX_PORTABLE_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#ifdef __ANDROID__
	#include "uuid.h"
#else
	#include <uuid/uuid.h>
#endif

// 日期和时间
struct port_date_time_t
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
	int nMilliseconds;
	int nDayOfWeek;
};

// 内存使用情况
struct port_memory_info_t
{
	double dMemoryLoad;
	double dTotalMemory;
	double dAvailMemory;
};

// 唯一标识符
struct port_uuid_t 
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
};
	
// 获得本地当前时间
inline bool Port_GetLocalTime(port_date_time_t* dt)
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);

	tm* pt = localtime(&tv.tv_sec);
	
	dt->nYear = pt->tm_year + 1900;
	dt->nMonth = pt->tm_mon + 1;
	dt->nDay = pt->tm_mday;
	dt->nHour = pt->tm_hour;
	dt->nMinute = pt->tm_min;
	dt->nSecond = pt->tm_sec;
	dt->nMilliseconds = tv.tv_usec / 1000;
	dt->nDayOfWeek = pt->tm_wday;
	
	return true;
}

// 获得系统计时
inline unsigned int Port_GetTickCount()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// 获得当前高精度时刻
inline double Port_GetPerformanceTime()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	
	return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
}

// 系统休眠
inline void Port_Sleep(unsigned int ms)
{
	struct timespec ts;
	
	ts.tv_sec = 0;
	ts.tv_nsec = ms * 1000000;
	
	nanosleep(&ts, NULL);
}

// 是否非法的浮点数
inline int Port_IsNaN(float value)
{
	return isnan(value);
}

// 创建唯一标识符
inline const char* Port_CreateGuid(char* buffer, size_t size)
{
#ifdef __ANDROID__
	uuid_t id;
	uuid_generate(id);
	snprintf(buffer, size -1, "%s", id);
	buffer[size - 1] = 0;
    return buffer;
#else
	port_uuid_t id;
	
	uuid_generate((unsigned char*)&id);
	snprintf(buffer, size - 1, 
		"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		id.Data1, id.Data2, id.Data3, 
		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
	buffer[size - 1] = 0;
		
	return buffer;
#endif
}

// 弹出信息窗口
inline void Port_PopupMsgBox(const char* info, const char* title)
{
	printf("(msg)%s:%s\n", title, info);
}

// 弹出错误信息窗口
inline void Port_PopupErrorMsgBox(const char* info, const char* title)
{
	printf("(err)%s:%s\n", title, info);
}

// 获得当前的进程ID
inline unsigned int Port_GetCurrentProcessId()
{
	return getpid();
}

// 获得当前执行文件的路径
inline bool Port_GetCurrentModuleFileName(char* buffer, size_t size)
{
	ssize_t res = readlink("/proc/self/exe", buffer, size);
	
	if ((res < 0) || (res >= size))
	{
		buffer[0] = 0;
		return false;
	}
	
	buffer[res] = 0;
	
	return true;
}

// 终止进程
inline bool Port_KillProcess(unsigned int proc_id)
{
	return kill(proc_id, SIGKILL);
}

inline char* skip_token(char* fp)
{
	char* ptr = fp;
	
	while (*ptr && (*ptr != ' '))
	{
		ptr++;
	}
	
	while (*ptr == ' ')
	{
		ptr++;
	}
	
	return ptr;
}

// 获得内存使用情况
inline bool Port_GetMemoryInfo(port_memory_info_t* pInfo)
{
	memset(pInfo, 0, sizeof(port_memory_info_t));
	
	// get system wide memory usage
	char buffer[1024];  
	char* p;  
	int fd = open("/proc/meminfo" , O_RDONLY);  
	int len = read(fd, buffer,  sizeof (buffer) - 1);  
	
	close(fd);  
	buffer[len] = '\0';  
	
	p = buffer;  
	p = skip_token(p);  
	
	pInfo->dTotalMemory = strtoul(p, &p, 10);  // total memory
	
	p = strchr(p, '\n');  
	p = skip_token(p);  
	
	pInfo->dAvailMemory = strtoul(p, &p, 10);  //  free memory  
	pInfo->dMemoryLoad = (1.0 - pInfo->dAvailMemory / pInfo->dTotalMemory);
	
	return true;
}

// 获得CPU使用率
inline double Port_GetCpuRate()
{
	char buffer[256];  
	char* p;
	int fd = open("/proc/loadavg", O_RDONLY);  
	int len = read(fd, buffer, sizeof(buffer) - 1);  
	
	close(fd);  
	buffer[len] = '\0';  
	
	double load_avg0 = strtod(buffer, &p);  
	double load_avg1 = strtod(p, &p);  
	double load_avg2 = strtod(p, &p);  
	
	return load_avg1;
}

// 初始化守护进程
inline bool Port_InitDaemon()
{
#ifndef NOFILE
#define NOFILE 0
#endif
	int pid = fork();
	
	if (pid > 0)
	{
		// 是父进程，结束父进程
		exit(0); 
	}
	else if (pid < 0)
	{
		// fork失败，退出
		exit(1); 
	}
	
	// 是第一子进程，后台继续执行
	//第一子进程成为新的会话组长和进程组长
	setsid(); 
	
	// 并与控制终端分离
	pid = fork();
	
	if (pid > 0)
	{
		// 是第一子进程，结束第一子进程
		exit(0); 
	}
	else if (pid < 0)
	{
		// fork失败，退出
		exit(1); 
	}
	
	// 是第二子进程，继续
	// 第二子进程不再是会话组长

	//关闭打开的文件描述符
	for (int i = 0; i < NOFILE; ++i)
	{
		close(i);
	}
	
	umask(0); //重设文件创建掩模
	
	return true;
}

// 启动控制台程序
inline bool Port_Execute(const char* cmd_line)
{
	char cmd[256];
	size_t size = strlen(cmd_line) + 1;
	
	if (size > sizeof(cmd))
	{
		return false;
	} 
	
	memcpy(cmd, cmd_line, size);
	
	char* argv[16] = { NULL };
	char* envp[1] = { NULL };
	int argc = 0;
	
	argv[argc++] = cmd;
	
	char* p = strchr(cmd, ' ');
	
	while (p)
	{
		*p = 0;
		
		while (*(++p) == ' ')
		{
			*p = 0;
		}
		
		argv[argc++] = p;
		
		if (argc == (sizeof(argv) / sizeof(argv[0]) - 1))
		{
			break;
		}
	}
	
	int pid = fork();
	
	if (0 == pid)
	{
		// 子进程
		if (execve(cmd, argv, envp) == -1)
		{
			// 启动程序失败，关闭子进程
			exit(0);
		}
	}
	
	return pid > 0;
}

// 设置当前进程与CPU核心的亲和性
inline bool Port_SetProcessAffinity(int core_id)
{
	return false;
}

#endif // _SYSTEM_LINUX_PORTABLE_H
