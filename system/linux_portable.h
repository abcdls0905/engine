//--------------------------------------------------------------------
// �ļ���:		linux_portable.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ���ں�ʱ��
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

// �ڴ�ʹ�����
struct port_memory_info_t
{
	double dMemoryLoad;
	double dTotalMemory;
	double dAvailMemory;
};

// Ψһ��ʶ��
struct port_uuid_t 
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
};
	
// ��ñ��ص�ǰʱ��
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

// ���ϵͳ��ʱ
inline unsigned int Port_GetTickCount()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// ��õ�ǰ�߾���ʱ��
inline double Port_GetPerformanceTime()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	
	return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
}

// ϵͳ����
inline void Port_Sleep(unsigned int ms)
{
	struct timespec ts;
	
	ts.tv_sec = 0;
	ts.tv_nsec = ms * 1000000;
	
	nanosleep(&ts, NULL);
}

// �Ƿ�Ƿ��ĸ�����
inline int Port_IsNaN(float value)
{
	return isnan(value);
}

// ����Ψһ��ʶ��
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

// ������Ϣ����
inline void Port_PopupMsgBox(const char* info, const char* title)
{
	printf("(msg)%s:%s\n", title, info);
}

// ����������Ϣ����
inline void Port_PopupErrorMsgBox(const char* info, const char* title)
{
	printf("(err)%s:%s\n", title, info);
}

// ��õ�ǰ�Ľ���ID
inline unsigned int Port_GetCurrentProcessId()
{
	return getpid();
}

// ��õ�ǰִ���ļ���·��
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

// ��ֹ����
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

// ����ڴ�ʹ�����
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

// ���CPUʹ����
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

// ��ʼ���ػ�����
inline bool Port_InitDaemon()
{
#ifndef NOFILE
#define NOFILE 0
#endif
	int pid = fork();
	
	if (pid > 0)
	{
		// �Ǹ����̣�����������
		exit(0); 
	}
	else if (pid < 0)
	{
		// forkʧ�ܣ��˳�
		exit(1); 
	}
	
	// �ǵ�һ�ӽ��̣���̨����ִ��
	//��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤
	setsid(); 
	
	// ��������ն˷���
	pid = fork();
	
	if (pid > 0)
	{
		// �ǵ�һ�ӽ��̣�������һ�ӽ���
		exit(0); 
	}
	else if (pid < 0)
	{
		// forkʧ�ܣ��˳�
		exit(1); 
	}
	
	// �ǵڶ��ӽ��̣�����
	// �ڶ��ӽ��̲����ǻỰ�鳤

	//�رմ򿪵��ļ�������
	for (int i = 0; i < NOFILE; ++i)
	{
		close(i);
	}
	
	umask(0); //�����ļ�������ģ
	
	return true;
}

// ��������̨����
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
		// �ӽ���
		if (execve(cmd, argv, envp) == -1)
		{
			// ��������ʧ�ܣ��ر��ӽ���
			exit(0);
		}
	}
	
	return pid > 0;
}

// ���õ�ǰ������CPU���ĵ��׺���
inline bool Port_SetProcessAffinity(int core_id)
{
	return false;
}

#endif // _SYSTEM_LINUX_PORTABLE_H
