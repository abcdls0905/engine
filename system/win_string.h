//--------------------------------------------------------------------
// 文件名:		win_string.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_STRING_H
#define _SYSTEM_WIN_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// 字符串转换为64位整数
inline __int64 Port_StringToInt64(const char* str)
{
	return _atoi64(str);
}

// 64位整数转换为字符串
inline void Port_Int64ToString(__int64 val, char* buf, size_t byte_size)
{
	_snprintf(buf, byte_size - 1, "%I64d", val);
	buf[byte_size - 1] = 0;
}

// 宽字符串转换为64位整数
inline __int64 Port_WideStrToInt64(const wchar_t* str)
{
	return _wtoi64(str);
}

// 64位整数转换为宽字符串
inline void Port_Int64ToWideStr(__int64 val, wchar_t* buf, size_t byte_size)
{
	size_t len = byte_size / sizeof(wchar_t);
	_snwprintf(buf, len - 1, L"%I64d", val);
	buf[len - 1] = 0;
}

// 字符串转换为大写
inline char* Port_StringUpper(char* str)
{
	return _strupr(str);
}

// 字符串转换为小写
inline char* Port_StringLower(char* str)
{
	return _strlwr(str);
}

// 宽字符串转换为大写
inline wchar_t* Port_WideStrUpper(wchar_t* str)
{
	return _wcsupr(str);
}

// 宽字符串转换为小写
inline wchar_t* Port_WideStrLower(wchar_t* str)
{
	return _wcslwr(str);
}

// 组织字符串
inline size_t Port_SafeSprintf(char* buf, size_t byte_size, const char* info, ...)
{
	va_list args;
	
	va_start(args, info);
	
	size_t size_1 = byte_size - 1;
	int res = _vsnprintf(buf, size_1, info, args);
	
	if ((size_t)res >= size_1)
	{
		buf[size_1] = 0;
		return size_1;
	}
	
	return res;
}

// 组织字符串
inline size_t Port_SafeSprintList(char* buf, size_t byte_size, const char* info, 
	va_list args)
{
	size_t size_1 = byte_size - 1;
	int res = _vsnprintf(buf, size_1, info, args);
	
	if ((size_t)res >= size_1)
	{
		buf[size_1] = 0;
		return size_1;
	}
	
	return res;
}

// 组织宽字符串
inline size_t Port_SafeSwprintf(wchar_t* buf, size_t byte_size, 
	const wchar_t* info, ...)
{
	va_list args;
	
	va_start(args, info);
	
	size_t size_1 = byte_size / sizeof(wchar_t) - 1;
	int res = _vsnwprintf(buf, size_1, info, args);
	
	if ((size_t)res >= size_1)
	{
		buf[size_1] = 0;
		return size_1;
	}
	
	return res;
}

// 组织宽字符串
inline size_t Port_SafeSwprintList(wchar_t* buf, size_t byte_size, 
	const wchar_t* info, va_list args)
{
	size_t size_1 = byte_size / sizeof(wchar_t) - 1;
	int res = _vsnwprintf(buf, size_1, info, args);
	
	if ((size_t)res >= size_1)
	{
		buf[size_1] = 0;
		return size_1;
	}
	
	return res;
}

#endif // _SYSTEM_WIN_STRING_H
