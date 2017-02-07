//--------------------------------------------------------------------
// 文件名:		ios_filesys.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_FILESYS_H
#define _SYSTEM_IOS_FILESYS_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <dlfcn.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

// 文件路径分隔符号
#define PORT_PATH_SEPARATOR '/'

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
	DIR* pDir;
	dirent* pEnt;
	char strPath[256];
	char strExt[16];
};

// 动态链接库句柄
typedef void* port_dll_handle_t;

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
	            // 消除重复的斜杠
	            ++src;
	            continue;
        	}
        	else
        	{
        		slash = true;
        	}
        	
        	*dst = '/';
    	}
    	else
    	{
   			// 全部转换成小写
   		//	*dst = tolower(*src);
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
	memset(buffer, 0, size);
	
	// 消除前面的空格
	const char* first = file_name;
	
	while ((*first == ' ') || (*first == '\t'))
	{
		++first;
	}
	
	// 是否已经包含完整的路径
	if (*first == '/')
	{
		size_t file_name_len = strlen(first);
	
		if (file_name_len >= size)
		{
			// 缓冲区不够
			return 0;
		}
		
		memcpy(buffer, first, file_name_len + 1);
	}
	else
	{
		// 获取当前目录
		char cur_dir[512] = { 0 };
		
		if (getcwd(cur_dir, sizeof(cur_dir) - 1) == NULL)
		{
			return 0;
		}
		
		// 解析相对路径
		const char* p = first;
		
		while (*p)
		{
			if (*p == '.')
			{
				// (.\)当前目录
				if ((p[1] == '\\') || (p[1] == '/'))
				{
					p += 2;
					continue;
				}
				
				if (p[1] != '.')
				{
					break;
				}
				
				if ((p[2] != '\\') && (p[2] != '/'))
				{
					break;
				}
				
				// (..\)取上一级目录
				char* prev = strrchr(cur_dir, '/');
				
				if (prev)
				{
					*(prev + 1) = 0;
				}
				
				p += 3;
			}
			else
			{
				break;
			}
		}
		
		size_t cur_dir_len = strlen(cur_dir);
		
		if (cur_dir[cur_dir_len - 1] != '/')
		{
			cur_dir[cur_dir_len] = '/';
		}
	
		if (strlen(cur_dir) + strlen(p) >= size)
		{
			return 0;
		}
	
		strcpy(buffer, cur_dir);
		strcat(buffer, p);
	}

	if (file_part)
	{
		// 分割路径和文件名
		char* separator = strrchr(buffer, '/');
		
		if (separator)
		{
			*file_part = separator + 1;
		}
		else
		{
			// 文件名中没有包含路径
			*file_part = buffer;
		}
	}
	
	return strlen(buffer);
}

// 获得当前工作目录
inline bool Port_GetCurrentDir(char* buffer, size_t size)
{
	char* res = getcwd(buffer, size);
	
	if (NULL == res)
	{
		buffer[0] = 0;
		return false;
	}
	
	return true;
}

// 设置当前工作目录
inline bool Port_SetCurrentDir(const char* dir_name)
{
	return chdir(dir_name) == 0;
}

// 目录是否存在
inline bool Port_DirExists(const char* dir_name)
{
	char sname[512];
	
	Port_GetStandardPath(dir_name, sname, sizeof(sname));
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFDIR;
}

// 创建目录
inline bool Port_DirCreate(const char* dir_name)
{
	char sname[512];
	
	Port_GetStandardPath(dir_name, sname, sizeof(sname));
	
	return mkdir(sname, 0700) == 0;
}

// 删除目录
inline bool Port_DirDelete(const char* dir_name)
{
	char sname[512];
	
	Port_GetStandardPath(dir_name, sname, sizeof(sname));
	
	return rmdir(sname) == 0;
}

// 文件是否存在
inline bool Port_FileExists(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFREG;
}

// 打开文件
inline FILE* Port_FileOpen(const char* file_name, const char* mode)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	return fopen(sname, mode);
}

// 文件删除
inline bool Port_FileDelete(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	return unlink(sname) == 0;
}

// 获得文件长度
inline bool Port_GetFileSize(const char* file_name, 
	port_file_size_t* file_size)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	file_size->nSizeHigh = st.st_size >> 32;
	file_size->nSizeLow = st.st_size & 0xFFFFFFFF;

	return true;
}

// 获得文件最后修改时间
inline bool Port_GetFileTime(const char* file_name, 
	port_file_time_t* file_time)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	// 使用国际标准时间
	tm* pt = gmtime(&st.st_mtime);
	
	file_time->nYear = pt->tm_year + 1900;
	file_time->nMonth = pt->tm_mon + 1;
	file_time->nDay = pt->tm_mday;
	file_time->nHour = pt->tm_hour;
	file_time->nMinute = pt->tm_min;
	file_time->nSecond = pt->tm_sec;
	
	return true;
}

// 设置文件时间
inline bool Port_SetFileTime(const char* file_name, 
	const port_file_time_t* file_time)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	struct utimbuf ut;
	struct tm tm1;
	
	memset(&tm1, 0, sizeof(tm1));
	
	tm1.tm_year = file_time->nYear;
	tm1.tm_mon = file_time->nMonth;
	tm1.tm_mday = file_time->nDay;
	tm1.tm_hour = file_time->nHour;
	tm1.tm_min = file_time->nMinute;
	tm1.tm_sec = file_time->nSecond;
	
	ut.actime = mktime(&tm1);
	ut.modtime = ut.actime;
	
	return utime(sname, &ut) == 0;
}

// 改变文件属性，保证文件可被修改
inline bool Port_EnableFileModify(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(sname, sname, sizeof(sname));
	
	return chmod(sname, 0777) == 0;
}

// 在必要时修改动态链接库的名称以保证能够正确加载
inline const char* Port_DllName(const char* name, char* buf, size_t size, 
	const char* linux_so_ext)
{
	char* p = buf;
	bool has_path = (strchr(name, '/') != NULL) 
		|| (strchr(name, '\\') != NULL);
	
	if (!has_path)
	{
		// 添加当前目录标记
		*p++ = '.';
		*p++ = '/';
		size -= 2;
	}

	size_t ext_len = strlen(linux_so_ext);

	size -= ext_len;

	size_t name_size = strlen(name) + 1;

	if (name_size > size)
	{
		memcpy(p, name, size - 1);
		p[size - 1] = 0;
	}
	else
	{
		memcpy(p, name, name_size);
	}

	if (ext_len > 0)
	{
		char* dot = strrchr(p, '.');

		if (dot)
		{
			// 替换掉文件扩展名
			memcpy(dot + 1, linux_so_ext, ext_len + 1);
		}
	}
	
	return buf;
}

// 加载动态链接库
inline port_dll_handle_t Port_DllOpen(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	return dlopen(sname, RTLD_NOW);
}

// 返回加载动态链接库时的错误
inline const char* Port_DllError(char* buffer, size_t size)
{
	char* err = dlerror();
	
	if (NULL == buffer)
	{
		// 只为清除错误
		return NULL;
	}		
	
	if (NULL == err)
	{
		return NULL;
	}
	
	size_t err_len = strlen(err);
	
	if (err_len >= size)
	{
		memcpy(buffer, err, size - 1);
		buffer[size - 1] = 0;
	}
	else
	{
		memcpy(buffer, err, err_len + 1);
	}
	
	return buffer;
}

// 获得动态链接库中的函数地址
inline void* Port_DllSymbol(port_dll_handle_t handle, const char* proc_name)
{
	dlerror();
	
	void* addr = dlsym(handle, proc_name);
	char* err = dlerror();
	
	if (err)
	{
		return NULL;
	}
	
	return addr;
}

// 释放动态链接库
inline bool Port_DllClose(port_dll_handle_t handle)
{
	return dlclose(handle) == 0;
}

// 开始搜索文件
inline bool Port_FindFirst(port_file_search_t* file_search, const char* path,
	const char* name)
{
	char sname[512];
	
	Port_GetStandardPath(path, sname, sizeof(sname));
	
	size_t path_len = strlen(sname);
	
	if (path_len >= sizeof(file_search->strPath))
	{
		return false;
	}
	
	// 文件扩展名，只支持(*.*)或(*.xxx)的格式
	const char* ext = name;
	const char* name_dot = strchr(name, '.');
	
	if (name_dot)
	{
		if (*(name_dot + 1) == '*')
		{
			// 认为是(*.*)
			ext = "";
		}
		else
		{
			// (*.xxx)
			ext = name_dot + 1;
		}
	}
	
	size_t ext_len = strlen(ext);
	
	if (ext_len >= sizeof(file_search->strExt))
	{
		return false;
	}
	
	DIR* pDir = opendir(sname);
	
	if (NULL == pDir)
	{
		return false;
	}
	
	dirent* pEnt = readdir(pDir);
	
	if (ext[0] != 0)
	{
		while (pEnt)
		{
			const char* dot = strchr(pEnt->d_name, '.');
			
			if (dot)
			{
				if (strcasecmp(ext, dot + 1) == 0)
				{
					// 文件扩展名匹配
					break;
				}
			}
			
			// 读取下一个 
			pEnt = readdir(pDir);
		}
	}
	
	if (NULL == pEnt)
	{
		closedir(pDir);
		return false;
	}
	
	file_search->pDir = pDir;
	file_search->pEnt = pEnt;
	memcpy(file_search->strPath, path, path_len + 1);
	memcpy(file_search->strExt, ext, ext_len + 1);
	
	return true;
}

// 获得搜索到的文件名
inline const char* Port_GetFindName(port_file_search_t* file_search)
{
	return file_search->pEnt->d_name;
}

// 获得搜索到的是否文件名
inline bool Port_GetFindIsFile(port_file_search_t* file_search)
{
	char fname[512];
	
	snprintf(fname, sizeof(fname) - 1, "%s/%s", file_search->strPath,
		file_search->pEnt->d_name);
	fname[sizeof(fname) - 1] = 0;
	
	struct stat st;
	
	if (stat(fname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFREG;
}

// 获得搜索到的是否目录名
inline bool Port_GetFindIsDir(port_file_search_t* file_search)
{
	char fname[512];
	
	snprintf(fname, sizeof(fname) - 1, "%s/%s", file_search->strPath,
		file_search->pEnt->d_name);
	fname[sizeof(fname) - 1] = 0;
	
	struct stat st;
	
	if (stat(fname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFDIR;
}

// 搜索下一个文件
inline bool Port_FindNext(port_file_search_t* file_search)
{
	dirent* pEnt = readdir(file_search->pDir);
	const char* ext = file_search->strExt;
	
	if (ext[0] != 0)
	{
		while (pEnt)
		{
			const char* dot = strchr(pEnt->d_name, '.');
			
			if (dot)
			{
				if (strcasecmp(ext, dot + 1) == 0)
				{
					// 文件扩展名匹配
					break;
				}
			}
			
			// 读取下一个 
			pEnt = readdir(file_search->pDir);
		}
	}
	
	if (NULL == pEnt)
	{
		return false;
	}
	
	file_search->pEnt = pEnt;
	
	return true;
}

// 结束搜索文件
inline bool Port_FindClose(port_file_search_t* file_search)
{
	return closedir(file_search->pDir) == 0;
}

#endif // _SYSTEM_IOS_FILESYS_H
