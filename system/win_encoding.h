//--------------------------------------------------------------------
// �ļ���:		win_encoding.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_ENCODING_H
#define _SYSTEM_WIN_ENCODING_H

#include <windows.h>

// ���ת�������ַ����󳤶ȣ�������������
inline size_t Port_GetToWideStrLen(const char* s)
{
	return MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
}

// ���ת�����ַ�����ĳ��ȣ�������������
inline size_t Port_GetToStringLen(const wchar_t* ws)
{
	return WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL);
}

// �ַ���ת�������ַ���
inline const wchar_t* Port_StringToWideStr(const char* info, wchar_t* buf, 
	size_t byte_size)
{
	const size_t len = byte_size / sizeof(wchar_t);
	int res = MultiByteToWideChar(CP_ACP, 0, info, -1, buf, int(len));
	
	if (res == 0)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			buf[len - 1] = 0;
		}
		else
		{
			buf[0] = 0;
		}
	}
	
	return buf;
}

// ���ַ���ת�����ַ���
inline const char* Port_WideStrToString(const wchar_t* info, char* buf, 
	size_t byte_size)
{
	int res = WideCharToMultiByte(CP_ACP, 0, info, -1, buf, int(byte_size), 
		NULL, NULL);
	
	if (0 == res)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			buf[byte_size - 1] = 0;
		}
		else
		{
			buf[0] = 0;
		}
	}
	
	return buf;
}

// ���UTF8ת�������ַ����󳤶ȣ�������������
inline size_t Port_GetUTF8ToWideStrLen(const char* s)
{
	return MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
}

// ��ÿ��ַ���ת����UTF8��ĳ��ȣ�������������
inline size_t Port_GetWideStrToUTF8Len(const wchar_t* ws)
{
	return WideCharToMultiByte(CP_UTF8, 0, ws, -1, NULL, 0, NULL, NULL);
}

// UTF8ת�������ַ���
inline const wchar_t* Port_UTF8ToWideStr(const char* info, wchar_t* buf, 
	size_t byte_size)
{
	const size_t len = byte_size / sizeof(wchar_t);
	int res = MultiByteToWideChar(CP_UTF8, 0, info, -1, buf, int(len));
	
	if (res == 0)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			buf[len - 1] = 0;
		}
		else
		{
			buf[0] = 0;
		}
	}
	
	return buf;
}

// ���ַ���ת����UTF8
inline const char* Port_WideStrToUTF8(const wchar_t* info, char* buf, 
	size_t byte_size)
{
	int res = WideCharToMultiByte(CP_UTF8, 0, info, -1, buf, int(byte_size), 
		NULL, NULL);
	
	if (0 == res)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			buf[byte_size - 1] = 0;
		}
		else
		{
			buf[0] = 0;
		}
	}
	
	return buf;
}

// UCS2ת�������ַ���
inline const wchar_t* Port_UCS2ToWideStr(const unsigned short* info, 
	wchar_t* buf, size_t byte_size)
{
	// ��WINDOWSϵͳ���ַ���2���ֽڣ�����Ҫת��
	size_t buf_len = byte_size / sizeof(wchar_t);
	size_t info_len = wcslen((const wchar_t*)info);
	
	if (info_len >= buf_len)
	{
		memcpy(buf, info, (buf_len - 1) * sizeof(wchar_t));
		buf[buf_len - 1] = 0;
	}
	else
	{
		memcpy(buf, info, (info_len + 1) * sizeof(wchar_t));
	}
	
	return buf;
}

// ���ַ���ת����UCS2ת����
inline const unsigned short* Port_WideStrToUCS2(const wchar_t* info, 
	unsigned short* buf, size_t byte_size)
{
	// ��WINDOWSϵͳ���ַ���2���ֽڣ�����Ҫת��
	size_t buf_len = byte_size / sizeof(unsigned short);
	size_t info_len = wcslen(info);
	
	if (info_len >= buf_len)
	{
		memcpy(buf, info, (buf_len - 1) * sizeof(unsigned short));
		buf[buf_len - 1] = 0;
	}
	else
	{
		memcpy(buf, info, (info_len + 1) * sizeof(unsigned short));
	}
	
	return buf;
}

// ���UCS2�ַ�������
inline size_t Port_GetUCS2Len(const unsigned short* info)
{
	return wcslen((const wchar_t*)info);
}

#endif // _SYSTEM_WIN_ENCODING_H
