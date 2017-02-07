//--------------------------------------------------------------------
// 文件名:		file_sys.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "file_sys.h"
#include "../public/i_core.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/portable.h"
#include <string.h>
#include <stdio.h>

extern ICore* g_pCore;
extern bool g_bLogFileIO;

// CFileSys

CFileSys* CFileSys::NewInstance()
{
	return NEW CFileSys;
}

CFileSys::CFileSys()
{
}

CFileSys::~CFileSys()
{
}

bool CFileSys::Init()
{
	return true;
}

bool CFileSys::Shut()
{
	return true;
}

void CFileSys::Release()
{
	delete this;
}
	
bool CFileSys::LoadConfig(const char* config)
{
	return true;
}

bool CFileSys::FileExists(const char* name)
{
	return Port_FileExists(name);
}

bool CFileSys::FileInPack(const char* name)
{
	return Port_FileExists(name);
}

bool CFileSys::FileDelete(const char* name)
{
	return Port_FileDelete(name);
}

size_t CFileSys::FileSize(const char* name)
{
	port_file_size_t pfs;
	
	if (!Port_GetFileSize(name, &pfs))
	{
		return 0;
	}

	return pfs.nSizeLow;
}

bool CFileSys::FileTime(const char* name, file_time_t* file_time)
{
	port_file_time_t pft;

	if (!Port_GetFileTime(name, &pft))
	{
		return false;
	}
	
	file_time->nYear = pft.nYear;
	file_time->nMonth = pft.nMonth;
	file_time->nDay = pft.nDay;
	file_time->nHour = pft.nHour;
	file_time->nMinute = pft.nMinute;
	file_time->nSecond = pft.nSecond;

	return true;
}

void* CFileSys::FileOpen(const char* name, const char* mode)
{
#ifdef FX_SYSTEM_WINDOWS
	if (g_bLogFileIO)
	{
		// 记录文件访问
		char info[256];

		SafeSprintf(info, sizeof(info), "thread %d open %s", 
			GetCurrentThreadId(), name);
		g_pCore->TraceLog(info);
	}
#endif // FX_SYSTEM_WINDOWS

	return Port_FileOpen(name, mode);
}

bool CFileSys::FileClose(void* handle)
{
	return fclose((FILE*)handle) == 0;
}

size_t CFileSys::FileRead(void* handle, void* pdata, size_t len)
{
	return fread(pdata, sizeof(char), len, (FILE*)handle);
}

size_t CFileSys::FileWrite(void* handle, const void* pdata, size_t len)
{
	return fwrite(pdata, sizeof(char), len, (FILE*)handle);
}

size_t CFileSys::FilePrint(void* handle, const char* info, ...)
{
	va_list vlist;

	va_start(vlist, info);

	size_t res = vfprintf((FILE*)handle, info, vlist);

	va_end(vlist);

	return res;
}

size_t CFileSys::FilePrintList(void* handle, const char* info, va_list vlist)
{
	return vfprintf((FILE*)handle, info, vlist);
}

bool CFileSys::FileFlush(void* handle)
{
	return fflush((FILE*)handle) == 0;
}

bool CFileSys::FileSeek(void* handle, long offset, int origin)
{
	return fseek((FILE*)handle, offset, origin) == 0;
}

size_t CFileSys::FileTell(void* handle)
{
	return ftell((FILE*)handle);
}

const void* CFileSys::FileContent(void* handle)
{
	return NULL;
}

bool CFileSys::DirExists(const char* name)
{
	return Port_DirExists(name);
}

bool CFileSys::DirCreate(const char* name)
{
	if (Port_DirExists(name))
	{
		return false;
	}
	
	if (Port_DirCreate(name))
	{
		return true;
	}

	bool succeed = true;
	size_t size = strlen(name) + 1;
	TAutoMem<char, 256> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();

	CopyString(buffer, size, name);

	for (size_t i = 0; i < size; ++i)
	{
		if ((buffer[i] == '\\') || (buffer[i] == '/'))
		{
			buffer[i] = 0;

			if (!Port_DirExists(buffer))
			{
				if (!Port_DirCreate(buffer))
				{
					succeed = false;
				}
			}

			buffer[i] = PORT_PATH_SEPARATOR;
		}
	}

	return succeed;
}

bool CFileSys::DirDelete(const char* name)
{
	return Port_DirDelete(name);
}

