//--------------------------------------------------------------------
// �ļ���:		converts.h
// ��  ��:		����ת������
// ˵  ��:		
// ��������:	2007��3��28��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// �ַ���ת�������ַ���
inline const wchar_t* ToWideStr(const char* info, wchar_t* buf, 
	size_t byte_size)
{
	Assert(info != NULL);
	Assert(buf != NULL);
	Assert(byte_size >= sizeof(wchar_t));
	
	return Port_StringToWideStr(info, buf, byte_size);


}

// ���ַ���ת�����ַ���
inline const char* ToString(const wchar_t* info, char* buf, size_t byte_size)
{
	Assert(info != NULL);
	Assert(buf != NULL);
	Assert(byte_size >= sizeof(char));

	return Port_WideStrToString(info, buf, byte_size);
}

// ���ת�������ַ����󳤶ȣ�������������
inline size_t GetToWideStrLen(const char* s)
{
	Assert(s != NULL);

	return Port_GetToWideStrLen(s);
}

// ���ת�����ַ�����ĳ��ȣ�������������
inline size_t GetToStringLen(const wchar_t* ws)
{
	Assert(ws != NULL);

	return Port_GetToStringLen(ws);
}

// �����ת�����ַ���
inline const char* ObjectToString(const PERSISTID& id, char* buf, 
	size_t byte_size)
{
	Assert(buf != NULL);
	Assert(byte_size > 0);
	
	Port_SafeSprintf(buf, byte_size, "%u-%u", id.nIdent, id.nSerial);

	return buf;
}

// �����ת���ɿ��ַ���
inline const wchar_t* ObjectToWideStr(const PERSISTID& id, wchar_t* buf, 
	size_t byte_size)
{
	Assert(buf != NULL);
	Assert(byte_size >= sizeof(wchar_t));
	
	Port_SafeSwprintf(buf, byte_size, L"%u-%u", id.nIdent, id.nSerial);
	
	return buf;
}

// �ַ���ת���ɶ����
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

// ���ַ���ת���ɶ����
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
