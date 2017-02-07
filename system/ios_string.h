//--------------------------------------------------------------------
// �ļ���:		ios_string.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_STRING_H
#define _SYSTEM_IOS_STRING_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>

// �ַ���ת��Ϊ64λ����
inline int64_t Port_StringToInt64(const char* str)
{
	return atoll(str);
}

// 64λ����ת��Ϊ�ַ���
inline void Port_Int64ToString(int64_t val, char* buf, size_t byte_size)
{
	snprintf(buf, byte_size - 1, "%lld", val);
	buf[byte_size - 1] = 0;
}

// ���ַ���ת��Ϊ64λ����
inline int64_t Port_WideStrToInt64(const wchar_t* str)
{
	return wcstoll(str, NULL, 10);
}

// 64λ����ת��Ϊ���ַ���
inline void Port_Int64ToWideStr(int64_t val, wchar_t* buf, size_t byte_size)
{
	size_t len = byte_size / sizeof(wchar_t);
	swprintf(buf, len - 1, L"%lld", val);
	buf[len - 1] = 0;
}

// �ַ���ת��Ϊ��д
inline char* Port_StringUpper(char* str)
{
	char* p = str;
	
	while (*p)
	{
		*p = toupper(*p);
		p++;
	}

	return str;
}

// �ַ���ת��ΪСд
inline char* Port_StringLower(char* str)
{
	char* p = str;
	
	while (*p)
	{
		*p = tolower(*p);
		p++;
	}

	return str;
}

// ���ַ���ת��Ϊ��д
inline wchar_t* Port_WideStrUpper(wchar_t* str)
{
	wchar_t* p = str;
	
	while (*p)
	{
		*p = towupper(*p);
		p++;
	}

	return str;
}

// ���ַ���ת��ΪСд
inline wchar_t* Port_WideStrLower(wchar_t* str)
{
	wchar_t* p = str;
	
	while (*p)
	{
		*p = towlower(*p);
		p++;
	}

	return str;
}

// ��֯�ַ���
inline size_t Port_SafeSprintf(char* buf, size_t byte_size, const char* info, ...)
{
	va_list args;
	
	va_start(args, info);
	
	size_t size_1 = byte_size - 1;
	int res = vsnprintf(buf, size_1, info, args);
	
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
	int res = vsnprintf(buf, size_1, info, args);
	
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
	int res = vswprintf(buf, size_1, info, args);
	
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
	int res = vswprintf(buf, size_1, info, args);
	
	if ((size_t)res >= size_1)
	{
		buf[size_1] = 0;
		return size_1;
	}
	
	return res;
}

#endif // _SYSTEM_IOS_STRING_H
