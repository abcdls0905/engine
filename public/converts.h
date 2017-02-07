//--------------------------------------------------------------------
// 文件名:		converts.h
// 内  容:		常用转换函数
// 说  明:		
// 创建日期:	2007年3月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CONVERTS_H
#define _PUBLIC_CONVERTS_H

#include "macros.h"
#include "persistid.h"
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_encoding.h"
	#include "../system/win_string.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_encoding.h"
	#include "../system/linux_string.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_encoding.h"
	#include "../system/ios_string.h"
#endif // FX_SYSTEM_IOS

// 字符串转换到宽字符串
inline const wchar_t* ToWideStr(const char* info, wchar_t* buf, 
	size_t byte_size)
{
	Assert(info != NULL);
	Assert(buf != NULL);
	Assert(byte_size >= sizeof(wchar_t));
	
	return Port_StringToWideStr(info, buf, byte_size);


}

// 宽字符串转换到字符串
inline const char* ToString(const wchar_t* info, char* buf, size_t byte_size)
{
	Assert(info != NULL);
	Assert(buf != NULL);
	Assert(byte_size >= sizeof(char));

	return Port_WideStrToString(info, buf, byte_size);
}

// 获得转换到宽字符串后长度（包括结束符）
inline size_t GetToWideStrLen(const char* s)
{
	Assert(s != NULL);

	return Port_GetToWideStrLen(s);
}

// 获得转换到字符串后的长度（包括结束符）
inline size_t GetToStringLen(const wchar_t* ws)
{
	Assert(ws != NULL);

	return Port_GetToStringLen(ws);
}

// 对象号转换成字符串
inline const char* ObjectToString(const PERSISTID& id, char* buf, 
	size_t byte_size)
{
	Assert(buf != NULL);
	Assert(byte_size > 0);
	
	Port_SafeSprintf(buf, byte_size, "%u-%u", id.nIdent, id.nSerial);

	return buf;
}

// 对象号转换成宽字符串
inline const wchar_t* ObjectToWideStr(const PERSISTID& id, wchar_t* buf, 
	size_t byte_size)
{
	Assert(buf != NULL);
	Assert(byte_size >= sizeof(wchar_t));
	
	Port_SafeSwprintf(buf, byte_size, L"%u-%u", id.nIdent, id.nSerial);
	
	return buf;
}

// 字符串转换成对象号
inline PERSISTID StringToObject(const char* s)
{
	char buf[128];
	size_t size = strlen(s) + 1;
	
	if (size > 128)
	{
		memcpy(buf, s, 127);
		buf[127] = 0;
	}
	else
	{
		memcpy(buf, s, size);
	}
	
	char* splitter = strchr(buf, '-');

	if (NULL == splitter)
	{
		return PERSISTID();
	}

	*splitter = 0;
	
	unsigned int ident = atoi(buf);
	unsigned int serial = atoi(splitter + 1);

	return PERSISTID(ident, serial);
}

// 宽字符串转换成对象号
inline PERSISTID WideStrToObject(const wchar_t* s)
{
	wchar_t buf[128];
	size_t size = wcslen(s) + 1;
	
	if (size > 128)
	{
		memcpy(buf, s, 127 * sizeof(wchar_t));
		buf[127] = 0;
	}
	else
	{
		memcpy(buf, s, size * sizeof(wchar_t));
	}
	
	wchar_t* splitter = wcschr(buf, L'-');

	if (NULL == splitter)
	{
		return PERSISTID();
	}

	*splitter = 0;
	
	unsigned int ident = wcstol(buf, NULL, 10);
	unsigned int serial = wcstol(splitter + 1, NULL, 10);

	return PERSISTID(ident, serial);
}

#endif // _PUBLIC_CONVERTS_H
