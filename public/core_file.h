//--------------------------------------------------------------------
// �ļ���:		core_file.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��4��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORE_FILE_H
#define _PUBLIC_CORE_FILE_H

#include "i_core.h"
#include "i_file_sys.h"
#include <stdio.h>

extern ICore* g_pCore;

// �ļ�ϵͳ

namespace core_file {

// ��Ŀ¼�ָ���ȫ��ת��"\"
inline void get_standard_filename(char* filename, size_t size)
{
    // ��עǰһ�������ǲ���Ŀ��ָ������������true
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

// ���ļ�
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

// �ر��ļ�
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

// ���ļ�
inline size_t fread(void* buffer, size_t size, size_t count, FILE* stream)
{
	size_t read_size = g_pCore->GetFileSys()->FileRead(stream, buffer, 
		size * count);
	
	return (read_size / size);
}

// д�ļ�
inline size_t fwrite(const void* buffer, size_t size, size_t count, 
	FILE* stream)
{
	size_t write_size = g_pCore->GetFileSys()->FileWrite(stream, buffer, 
		size * count);
	
	return (write_size / size);
}

// ��ʽ��д�ļ�
inline int fprintf(FILE* stream, const char* format, ...)
{
	va_list vlist;
	
	va_start(vlist, format);
	
	size_t res = g_pCore->GetFileSys()->FilePrintList(stream, format, vlist);
	
	va_end(vlist);
	
	return int(res);
}

// �ύд������
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

// �ƶ��ļ���дָ��
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

// ����ļ���дָ��λ��
inline long ftell(FILE* stream)
{
	return long(g_pCore->GetFileSys()->FileTell(stream));
}

} // namespace core_file

#endif // _PUBLIC_CORE_FILE_H
