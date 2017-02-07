//--------------------------------------------------------------------
// �ļ���:		ios_filesys.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// �ļ�·���ָ�����
#define PORT_PATH_SEPARATOR '/'

// �ļ�����
struct port_file_size_t
{
	unsigned int nSizeHigh;
	unsigned int nSizeLow;
};

// �ļ�����
struct port_file_time_t
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
};

// �ļ�����
struct port_file_search_t
{
	DIR* pDir;
	dirent* pEnt;
	char strPath[256];
	char strExt[16];
};

// ��̬���ӿ���
typedef void* port_dll_handle_t;

// ��ñ�׼��·��
inline const char* Port_GetStandardPath(const char* path, char* buffer, 
	size_t size)
{
    // ��עǰһ�������ǲ���Ŀ��ָ������������true
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
	            // �����ظ���б��
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
   			// ȫ��ת����Сд
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

// ����ļ����е�����·��
inline int Port_GetFullPathName(const char* file_name, char* buffer, 
	size_t size, char** file_part)
{
	memset(buffer, 0, size);
	
	// ����ǰ��Ŀո�
	const char* first = file_name;
	
	while ((*first == ' ') || (*first == '\t'))
	{
		++first;
	}
	
	// �Ƿ��Ѿ�����������·��
	if (*first == '/')
	{
		size_t file_name_len = strlen(first);
	
		if (file_name_len >= size)
		{
			// ����������
			return 0;
		}
		
		memcpy(buffer, first, file_name_len + 1);
	}
	else
	{
		// ��ȡ��ǰĿ¼
		char cur_dir[512] = { 0 };
		
		if (getcwd(cur_dir, sizeof(cur_dir) - 1) == NULL)
		{
			return 0;
		}
		
		// �������·��
		const char* p = first;
		
		while (*p)
		{
			if (*p == '.')
			{
				// (.\)��ǰĿ¼
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
				
				// (..\)ȡ��һ��Ŀ¼
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
		// �ָ�·�����ļ���
		char* separator = strrchr(buffer, '/');
		
		if (separator)
		{
			*file_part = separator + 1;
		}
		else
		{
			// �ļ�����û�а���·��
			*file_part = buffer;
		}
	}
	
	return strlen(buffer);
}

// ��õ�ǰ����Ŀ¼
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

// ���õ�ǰ����Ŀ¼
inline bool Port_SetCurrentDir(const char* dir_name)
{
	return chdir(dir_name) == 0;
}

// Ŀ¼�Ƿ����
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

// ����Ŀ¼
inline bool Port_DirCreate(const char* dir_name)
{
	char sname[512];
	
	Port_GetStandardPath(dir_name, sname, sizeof(sname));
	
	return mkdir(sname, 0700) == 0;
}

// ɾ��Ŀ¼
inline bool Port_DirDelete(const char* dir_name)
{
	char sname[512];
	
	Port_GetStandardPath(dir_name, sname, sizeof(sname));
	
	return rmdir(sname) == 0;
}

// �ļ��Ƿ����
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

// ���ļ�
inline FILE* Port_FileOpen(const char* file_name, const char* mode)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	return fopen(sname, mode);
}

// �ļ�ɾ��
inline bool Port_FileDelete(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	return unlink(sname) == 0;
}

// ����ļ�����
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

// ����ļ�����޸�ʱ��
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
	
	// ʹ�ù��ʱ�׼ʱ��
	tm* pt = gmtime(&st.st_mtime);
	
	file_time->nYear = pt->tm_year + 1900;
	file_time->nMonth = pt->tm_mon + 1;
	file_time->nDay = pt->tm_mday;
	file_time->nHour = pt->tm_hour;
	file_time->nMinute = pt->tm_min;
	file_time->nSecond = pt->tm_sec;
	
	return true;
}

// �����ļ�ʱ��
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

// �ı��ļ����ԣ���֤�ļ��ɱ��޸�
inline bool Port_EnableFileModify(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(sname, sname, sizeof(sname));
	
	return chmod(sname, 0777) == 0;
}

// �ڱ�Ҫʱ�޸Ķ�̬���ӿ�������Ա�֤�ܹ���ȷ����
inline const char* Port_DllName(const char* name, char* buf, size_t size, 
	const char* linux_so_ext)
{
	char* p = buf;
	bool has_path = (strchr(name, '/') != NULL) 
		|| (strchr(name, '\\') != NULL);
	
	if (!has_path)
	{
		// ��ӵ�ǰĿ¼���
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
			// �滻���ļ���չ��
			memcpy(dot + 1, linux_so_ext, ext_len + 1);
		}
	}
	
	return buf;
}

// ���ض�̬���ӿ�
inline port_dll_handle_t Port_DllOpen(const char* file_name)
{
	char sname[512];
	
	Port_GetStandardPath(file_name, sname, sizeof(sname));
	
	return dlopen(sname, RTLD_NOW);
}

// ���ؼ��ض�̬���ӿ�ʱ�Ĵ���
inline const char* Port_DllError(char* buffer, size_t size)
{
	char* err = dlerror();
	
	if (NULL == buffer)
	{
		// ֻΪ�������
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

// ��ö�̬���ӿ��еĺ�����ַ
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

// �ͷŶ�̬���ӿ�
inline bool Port_DllClose(port_dll_handle_t handle)
{
	return dlclose(handle) == 0;
}

// ��ʼ�����ļ�
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
	
	// �ļ���չ����ֻ֧��(*.*)��(*.xxx)�ĸ�ʽ
	const char* ext = name;
	const char* name_dot = strchr(name, '.');
	
	if (name_dot)
	{
		if (*(name_dot + 1) == '*')
		{
			// ��Ϊ��(*.*)
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
					// �ļ���չ��ƥ��
					break;
				}
			}
			
			// ��ȡ��һ�� 
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

// ������������ļ���
inline const char* Port_GetFindName(port_file_search_t* file_search)
{
	return file_search->pEnt->d_name;
}

// ������������Ƿ��ļ���
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

// ������������Ƿ�Ŀ¼��
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

// ������һ���ļ�
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
					// �ļ���չ��ƥ��
					break;
				}
			}
			
			// ��ȡ��һ�� 
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

// ���������ļ�
inline bool Port_FindClose(port_file_search_t* file_search)
{
	return closedir(file_search->pDir) == 0;
}

#endif // _SYSTEM_IOS_FILESYS_H
