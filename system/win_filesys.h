//--------------------------------------------------------------------
// 文件名:		win_filesys.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_FILESYS_H
#define _SYSTEM_WIN_FILESYS_H

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 文件路径分隔符号
#define PORT_PATH_SEPARATOR '\\'

// 文件长度
struct port_file_size_t
{
	unsigned int nSizeHigh;
	unsigned int nSizeLow;
};

// 文件日期
struct port_file_time_t
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
};

// 文件搜索
struct port_file_search_t
{
	HANDLE handle;
	WIN32_FIND_DATAA data;
};

// 动态链接库句柄
typedef HMODULE port_dll_handle_t;

// 获得标准的路径
inline const char* Port_GetStandardPath(const char* path, char* buffer, 
	size_t size)
{
    // 标注前一个符号是不是目标分隔符，是则设成true
    bool slash = false;
    const char* src = path;
    char* dst = buffer;
    char* dst_end = buffer + size - 1;
    
    while (*src)
   	{
    	if (dst >= dst_end)
    	{
    		break;
    	}
    	
        if ((*src == '/') || (*src == '\\'))
        {
        	if (slash)
        	{
	            ++src;
	            continue;
        	}
        	else
        	{
        		slash = true;
        	}
        	
        	*dst = '\\';
    	}
    	else
    	{
   			*dst = *src;
    		slash = false;
    	}
    	
   		++dst;
   		++src;
	}
	
	*dst = 0;
	
	return buffer;
}

// 获得文件名中的完整路径
inline int Port_GetFullPathName(const char* file_name, char* buffer, 
	size_t size, char** file_part)
{
	DWORD res = GetFullPathNameA(file_name, (DWORD)size, buffer, file_part);
	
	if (0 == res)
	{
		buffer[0] = 0;
		return 0;
	}	
	
	if (size_t(res) >= size)
	{
		// 缓冲区不够
		buffer[0] = 0;
		return 0;
	}
	
	return (int)res;
}

// 获得当前工作目录
inline bool Port_GetCurrentDir(char* buffer, size_t size)
{
	DWORD res = GetCurrentDirectoryA((DWORD)size, buffer);
	
	if (0 == res)
	{
		buffer[0] = 0;
		return false;
	}
	
	if (size_t(res) >= size)
	{
		// 缓冲区不够
		buffer[0] = 0;
		return false;
	}
	
	return true;
}

// 设置当前工作目录
inline bool Port_SetCurrentDir(const char* dir_name)
{
	return SetCurrentDirectoryA(dir_name) == TRUE;
}

// 目录是否存在
inline bool Port_DirExists(const char* dir_name)
{
	DWORD res = GetFileAttributesA(dir_name);

	if (res == DWORD(-1))
	{
		return false;
	}

	return (res & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// 创建目录
inline bool Port_DirCreate(const char* dir_name)
{
	return CreateDirectoryA(dir_name, NULL) == TRUE;
}

// 删除目录
inline bool Port_DirDelete(const char* dir_name)
{
	return RemoveDirectoryA(dir_name) == TRUE;
}

// 文件是否存在
inline bool Port_FileExists(const char* file_name)
{
	DWORD res = GetFileAttributesA(file_name);

	if (res == DWORD(-1))
	{
		return false;
	}

	return (res & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

//为了window上面做监控 所以上面这块代码只对window有效果
#include "../public/inlines.h"
#include "../public/i_core.h"

// 打开文件
inline FILE* Port_FileOpen(const char* file_name, const char* mode)
{
   
	char mbzTemp[1024];
	SafeSprintf( mbzTemp,sizeof(mbzTemp),"[TheadID]%d : %s",GetCurrentThreadId(),file_name);
	fm_tracelog( mbzTemp,LOG_FILEIO);

	return fopen(file_name, mode);
}

// 文件删除
inline bool Port_FileDelete(const char* file_name)
{
	return DeleteFileA(file_name) == TRUE;
}

// 获得文件长度
inline bool Port_GetFileSize(const char* file_name, 
	port_file_size_t* file_size)
{
	HANDLE fp = CreateFileA(file_name, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == fp)
	{
		return false;
	}

	DWORD size_high;
	DWORD size_low = GetFileSize(fp, &size_high);

	CloseHandle(fp);

	file_size->nSizeHigh = size_high;
	file_size->nSizeLow = size_low;

	return true;
}

// 获得文件最后修改时间
inline bool Port_GetFileTime(const char* file_name, 
	port_file_time_t* file_time)
{
	HANDLE fp = CreateFileA(file_name, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == fp)
	{
		return false;
	}

	FILETIME ft;

	if (!GetFileTime(fp, NULL, NULL, &ft))
	{
		CloseHandle(fp);
		return false;
	}

	CloseHandle(fp);

	SYSTEMTIME st;

	// 使用国际标准时间
	FileTimeToSystemTime(&ft, &st);

	file_time->nYear = st.wYear;
	file_time->nMonth = st.wMonth;
	file_time->nDay = st.wDay;
	file_time->nHour = st.wHour;
	file_time->nMinute = st.wMinute;
	file_time->nSecond = st.wSecond;
	
	return true;
}

// 设置文件时间
inline bool Port_SetFileTime(const char* file_name, 
	const port_file_time_t* file_time)
{
	HANDLE handle = CreateFileA(file_name, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	
	if (INVALID_HANDLE_VALUE == handle)
	{
		return false;
	}
	
	// 设置文件时间
	SYSTEMTIME st;
	FILETIME ft;
	
	memset(&st, 0, sizeof(st));
	st.wYear = file_time->nYear;
	st.wMonth = file_time->nMonth;
	st.wDay = file_time->nDay;
	st.wHour = file_time->nHour;
	st.wMinute = file_time->nMinute;
	st.wSecond = file_time->nSecond;
	
	SystemTimeToFileTime(&st, &ft);
	
	if (!SetFileTime(handle, &ft, &ft, &ft))
	{
		CloseHandle(handle);
		return false;
	}
	
	CloseHandle(handle);
	
	return true;
}

// 改变文件属性，保证文件可被修改
inline bool Port_EnableFileModify(const char* file_name)
{
	return SetFileAttributesA(file_name, FILE_ATTRIBUTE_ARCHIVE) == TRUE;
}

// 在必要时修改动态链接库的名称以保证能够正确加载
inline const char* Port_DllName(const char* name, char* buf, size_t size, 
	const char* linux_so_ext)
{
	// 在WINDOWS下不需要对名字做任何修改
	size_t name_size = strlen(name) + 1;

	if (name_size > size)
	{
		memcpy(buf, name, size - 1);
		buf[size - 1] = 0;
	}
	else
	{
		memcpy(buf, name, name_size);
	}

	return buf;
}

// 加载动态链接库
inline port_dll_handle_t Port_DllOpen(const char* file_name)
{
	return LoadLibraryA(file_name);
}

// 返回加载动态链接库时的错误
inline const char* Port_DllError(char* buffer, size_t size)
{
	if (NULL == buffer)
	{
		return NULL;
	}	
	
	int err = GetLastError();
	
	if (0 == err)
	{
		return NULL;
	}
	
	if (FormatMessageA(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		0, err, 0, buffer, (DWORD)size, NULL) == 0)
	{
		return NULL;
	}		
		
	return buffer;
}

// 获得动态链接库中的函数地址
inline void* Port_DllSymbol(port_dll_handle_t handle, const char* proc_name)
{
	return GetProcAddress(handle, proc_name);
}

// 释放动态链接库
inline bool Port_DllClose(port_dll_handle_t handle)
{
	return FreeLibrary(handle) == TRUE;
}

// 开始搜索文件
inline bool Port_FindFirst(port_file_search_t* file_search, const char* path,
	const char* name)
{
	char fname[512];
	
	if (name[0] == 0)
	{
		// 搜索所有文件
		_snprintf(fname, sizeof(fname) - 1, "%s*.*", path);
		fname[sizeof(fname) - 1] = 0;
	}
	else
	{
		if (strchr(name, '.') != NULL)
		{
			// 指定通配符
			_snprintf(fname, sizeof(fname) - 1, "%s%s", path, name);
			fname[sizeof(fname) - 1] = 0;
		}
		else
		{
			// 指定扩展名
			_snprintf(fname, sizeof(fname) - 1, "%s*.%s", path, name);
			fname[sizeof(fname) - 1] = 0;
		}
	}
	
	file_search->handle = FindFirstFileA(fname, &file_search->data);
	
	if (INVALID_HANDLE_VALUE == file_search->handle)
	{
		return false;
	}
	
	return true;
}

// 获得搜索到的文件名
inline const char* Port_GetFindName(port_file_search_t* file_search)
{
	return file_search->data.cFileName;
}

// 获得搜索到的是否文件名
inline bool Port_GetFindIsFile(port_file_search_t* file_search)
{
	return (file_search->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

// 获得搜索到的是否目录名
inline bool Port_GetFindIsDir(port_file_search_t* file_search)
{
	return (file_search->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// 搜索下一个文件
inline bool Port_FindNext(port_file_search_t* file_search)
{
	return FindNextFileA(file_search->handle, &file_search->data) == TRUE;
}

// 结束搜索文件
inline bool Port_FindClose(port_file_search_t* file_search)
{
	return FindClose(file_search->handle) == TRUE;
}

#endif // _SYSTEM_WIN_FILESYS_H
