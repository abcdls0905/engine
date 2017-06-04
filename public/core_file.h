
#ifndef _PUBLIC_CORE_FILE_H
#define _PUBLIC_CORE_FILE_H

#include "i_core.h"
#include "i_file_sys.h"
#include <stdio.h>

extern ICore* g_pCore;

// 文件系统

namespace core_file {

// 将目录分隔符全部转成"\"
inline void get_standard_filename(char* filename, size_t size)
{
    // 标注前一个符号是不是目标分隔符，是则设成true
    bool slash = false;
    char* s = filename;
    
    while (*s != 0)
   	{
        if ((*s == '/') || (*s == '\\'))
        {
        	if (slash)
        	{
	            memmove(s, s + 1, size - (s - filename));
	            --size;
	            continue;
        	}
        	else
        	{
        		if (*s == '/')
        		{
        			*s = '\\';
        		}
        		
        		slash = true;
        	}
    	}
    	else
    	{
    		slash = false;
    	}
   		
   		++s;
	}
}

// 打开文件
inline FILE* fopen(const char* filename, const char* mode)
{
    char dest_filename[260];
    size_t size = strlen(filename);
    
    if (size >= sizeof(dest_filename))
    {
    	return NULL;
	}
	
	memcpy(dest_filename, filename, size + 1);

    get_standard_filename(dest_filename, size);
    
    return (FILE*)g_pCore->GetFileSys()->FileOpen(dest_filename, mode);
}

// 关闭文件
inline int fclose(FILE* stream)
{
	if (g_pCore->GetFileSys()->FileClose(stream))
	{
		return 0;
	}
	else
	{
		return EOF;
	}
}

// 读文件
inline size_t fread(void* buffer, size_t size, size_t count, FILE* stream)
{
	size_t read_size = g_pCore->GetFileSys()->FileRead(stream, buffer, 
		size * count);
	
	return (read_size / size);
}

// 写文件
inline size_t fwrite(const void* buffer, size_t size, size_t count, 
	FILE* stream)
{
	size_t write_size = g_pCore->GetFileSys()->FileWrite(stream, buffer, 
		size * count);
	
	return (write_size / size);
}

// 格式化写文件
inline int fprintf(FILE* stream, const char* format, ...)
{
	va_list vlist;
	
	va_start(vlist, format);
	
	size_t res = g_pCore->GetFileSys()->FilePrintList(stream, format, vlist);
	
	va_end(vlist);
	
	return int(res);
}

// 提交写入数据
inline int fflush(FILE* stream)
{
	if (g_pCore->GetFileSys()->FileFlush(stream))
	{
		return 0;
	}
	else
	{
		return EOF;
	}
}

// 移动文件读写指针
inline int fseek(FILE* stream, long offset, int origin)
{
	if (g_pCore->GetFileSys()->FileSeek(stream, offset, origin))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

// 获得文件读写指针位置
inline long ftell(FILE* stream)
{
	return long(g_pCore->GetFileSys()->FileTell(stream));
}

} // namespace core_file

#endif // _PUBLIC_CORE_FILE_H
