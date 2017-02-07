//--------------------------------------------------------------------
// �ļ���:		win_string.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_STRING_H
#define _SYSTEM_WIN_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// �ַ���ת��Ϊ64λ����
inline __int64 Port_StringToInt64(const char* str)
{
	return _atoi64(str);
}

// 64λ����ת��Ϊ�ַ���
inline void Port_Int64ToString(__int64 val, char* buf, size_t byte_size)
{
	_snprintf(buf, byte_size - 1, "%I64d", val);
	buf[byte_size - 1] = 0;
}

// ���ַ���ת��Ϊ64λ����
inline __int64 Port_WideStrToInt64(const wchar_t* str)
{
	return _wtoi64(str);
}

// 64λ����ת��Ϊ���ַ���
inline void Port_Int64ToWideStr(__int64 val, wchar_t* buf, size_t byte_size)
{
	size_t len = byte_size / sizeof(wchar_t);
	_snwprintf(buf, len - 1, L"%I64d", val);
	buf[len - 1] = 0;
}

// �ַ���ת��Ϊ��д
inline char* Port_StringUpper(char* str)
{
	return _strupr(str);
}

// �ַ���ת��ΪСд
inline char* Port_StringLower(char* str)
{
	return _strlwr(str);
}

// ���ַ���ת��Ϊ��д
inline wchar_t* Port_WideStrUpper(wchar_t* str)
{
	return _wcsupr(str);
}

// ���ַ���ת��ΪСд
inline wchar_t* Port_WideStrLower(wchar_t* str)
{
	return _wcslwr(str);
}

// ��֯�ַ���
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

// ��֯�ַ���
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

// ��֯���ַ���
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

// ��֯���ַ���
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
