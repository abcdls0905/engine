//--------------------------------------------------------------------
// 文件名:		ios_encoding.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_ENCODING_H
#define _SYSTEM_IOS_ENCODING_H

#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

// 获得转换到宽字符串后长度（包括结束符）
inline size_t Port_GetToWideStrLen(const char* s)
{
	setlocale(LC_ALL, "");
	
	size_t res = mbstowcs(NULL, s, 0);
	
	if (res == (size_t)(-1))
	{
		// 无法转换
		return 1;
	}
	
	return res + 1;
}

// 获得转换到字符串后的长度（包括结束符）
inline size_t Port_GetToStringLen(const wchar_t* ws)
{
	setlocale(LC_ALL, "");
	
	size_t res = wcstombs(NULL, ws, 0);
	
	if (res == (size_t)(-1))
	{
		// 无法转换
		return 1;
	}
	
	return res + 1;
}

// 字符串转换到宽字符串
inline const wchar_t* Port_StringToWideStr(const char* info, wchar_t* buf, 
	size_t byte_size)
{
	setlocale(LC_ALL, "");
	
	const size_t len = byte_size / sizeof(wchar_t);
	size_t res = mbstowcs(buf, info, len);
	
	if (res == (size_t)(-1))
	{
		// 无法转换
		buf[0] = 0;
	}
	else if (res == len)
	{
		buf[len - 1] = 0;
	}
	
	return buf;
}

// 宽字符串转换到字符串
inline const char* Port_WideStrToString(const wchar_t* info, char* buf, 
	size_t byte_size)
{
	setlocale(LC_ALL, "");
	
	size_t res = wcstombs(buf, info, byte_size);
	
	if (res == (size_t)(-1))
	{
		// 无法转换
		buf[0] = 0;
	}
	else if (res == byte_size)
	{
		buf[byte_size - 1] = 0;
	}
	
	return buf;
}

// 获得UTF8转换到宽字符串后长度（包括结束符）
inline size_t Port_GetUTF8ToWideStrLen(const char* s)
{
	/*	
	setlocale(LC_ALL, "en_US.UTF-8");
	
	size_t res = mbstowcs(NULL, s, 0) + 1;
	
	setlocale(LC_ALL, "");
	
	return res;
	*/
	size_t src_len = strlen(s);
	
	if (0 == src_len)
	{
		return 1;
	}

	size_t count = 0;
	size_t pos = 0;

	while (pos < src_len)
	{
		unsigned char c = s[pos];
		
		if ((c & 0x80) == 0)
		{
			count += 1;
			pos += 1;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			count += 1;
			pos += 2;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			count += 1;
			pos += 3;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			count += 1;
			pos += 4;
		}
		else
		{
			count += 1;
			pos += 5;
		}
	}

	return count + 1;
}

// 获得宽字符串转换到UTF8后的长度（包括结束符）
inline size_t Port_GetWideStrToUTF8Len(const wchar_t* ws)
{
	/*
	setlocale(LC_ALL, "en_US.UTF-8");
	
	sizt_t wcstombs(NULL, ws, 0) + 1;
	
	setlocale(LC_ALL, "");
	
	return res;
	*/
	size_t src_len = wcslen(ws);
	
	if (0 == src_len)
	{
		return 1;
	}

	size_t count = 0;
	
	for (size_t i = 0; i < src_len; ++i)
	{
		wchar_t wch = ws[i];
		
		if (wch < 0x80)	// 0000 - 007F
		{
			count += 1;
		}
		else if (wch < 0x800) // 0080 - 07FF
		{
			count += 2;
		}
		else if (wch < 0x10000)	// 0800 - FFFF
		{
			count += 3;
		}
		else if (wch < 0x110000) //	0001 0000 - 0010 FFFF
		{
			count += 4;
		}
		else
		{
			count += 5;
		}
	}

	return count + 1;
}

// UTF8转换到宽字符串
inline const wchar_t* Port_UTF8ToWideStr(const char* info, wchar_t* buf, 
	size_t byte_size)
{
	/*
	setlocale(LC_ALL, "en_US.UTF-8");
	
	const size_t len = byte_size / sizeof(wchar_t);
	size_t res = mbstowcs(buf, info, len);
	
	if (res == (size_t)(-1))
	{
		buf[0] = 0;
	}
	else if (res == len)
	{
		buf[len - 1] = 0;
	}
	
	setlocale(LC_ALL, "");
	
	return buf;
	*/
	size_t src_len = strlen(info);
	
	if (0 == src_len)
	{
		buf[0] = 0;
		return buf;
	}

	size_t max_len = byte_size / sizeof(wchar_t) - 1;	
	size_t count = 0;
	size_t pos = 0;

	while (pos < src_len)
	{
		if (count >= max_len)
		{
			break;
		}
		
		wchar_t wchar;
		unsigned char c = info[pos];
		
		if ((c & 0x80) == 0) // 0xxx xxxx
		{
			wchar = info[pos];
			pos += 1;
		}
		else if ((c & 0xE0) == 0xC0) // 110x xxxx 10xx xxxx
		{
			wchar = (info[pos + 0] & 0x3F) << 6;
			wchar |= (info[pos + 1] & 0x3F);
			pos += 2;
		}
		else if ((c & 0xF0) == 0xE0) // 1110 xxxx 10xx xxxx 10xx xxxx
		{
			wchar = (info[pos + 0] & 0x1F) << 12;
			wchar |= (info[pos + 1] & 0x3F) << 6;
			wchar |= (info[pos + 2] & 0x3F);
			pos += 3;
		}
		else if ((c & 0xF8) == 0xF0) // UCS4: 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
		{
			wchar = (info[pos + 0] & 0x0F) << 18;
			wchar |= (info[pos + 1] & 0x3F) << 12;
			wchar |= (info[pos + 2] & 0x3F) << 6;
			wchar |= (info[pos + 3] & 0x3F);
			pos += 4;
		}
		else // UCS4: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx 
		{
			wchar = (info[pos + 0] & 0x07) << 24;
			wchar |= (info[pos + 1] & 0x3F) << 18;
			wchar |= (info[pos + 2] & 0x3F) << 12;
			wchar |= (info[pos + 3] & 0x3F) << 6;
			wchar |= (info[pos + 4] & 0x3F);
			pos += 5;
		}
		
		buf[count++] = wchar;
	}

	buf[count] = 0;

	return buf;
}

// 宽字符串转换到UTF8
inline const char* Port_WideStrToUTF8(const wchar_t* info, char* buf, 
	size_t byte_size)
{
	/*
	setlocale(LC_ALL, "en_US.UTF-8");
	
	size_t res = wcstombs(buf, info, byte_size);
	
	if (res == (size_t)(-1))
	{
		buf[0] = 0;
	}
	else if (res == byte_size)
	{
		buf[byte_size - 1] = 0;
	}
	
	setlocale(LC_ALL, "");
	
	return buf;
	*/
	size_t src_len = wcslen(info);
	
	if (0 == src_len)
	{
		buf[0] = 0;
		return buf;
	}

	size_t max_len = byte_size - 1;
	size_t count = 0;
	
	for (size_t i = 0; i < src_len; ++i)
	{
		wchar_t wch = info[i];
		
		if (wch < 0x80)	// 0xxx xxxx
		{
			if ((count + 1) > max_len)
			{
				break;
			}
			
			buf[count++] = (char)wch;
		}
		else if (wch < 0x800) // 110x xxxx 10xx xxxx
		{
			if ((count + 2) > max_len)
			{
				break;
			}
			
			buf[count++] = 0xC0 | (wch >> 6);
			buf[count++] = 0x80 | (wch & 0x3F);
		}
		else if (wch < 0x10000)	// 1110 xxxx 10xx xxxx 10xx xxxx
		{
			if ((count + 3) > max_len)
			{
				break;
			}
			
			buf[count++] = 0xE0 | (wch >> 12);
			buf[count++] = 0x80 | ((wch >> 6) & 0x3F);
			buf[count++] = 0x80 | (wch & 0x3F);
		}
		else if (wch < 0x110000) // UCS4: 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
		{
			if ((count + 4) > max_len)
			{
				break;
			}
			
			buf[count++] = 0xF0 | (wch >> 18);
			buf[count++] = 0x80 | ((wch >> 12) & 0x3F);
			buf[count++] = 0x80 | ((wch >> 6) & 0x3F);
			buf[count++] = 0x80 | (wch & 0x3F);
		}
		else // UCS4: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx
		{
			if ((count + 5) > max_len)
			{
				break;
			}
			
			buf[count++] = 0xF8 | (wch >> 24);
			buf[count++] = 0x80 | ((wch >> 18) & 0x3F);
			buf[count++] = 0x80 | ((wch >> 12) & 0x3F);
			buf[count++] = 0x80 | ((wch >> 6) & 0x3F);
			buf[count++] = 0x80 | (wch & 0x3F);
		}
	}

	buf[count] = 0;

	return buf;
}

// UCS2转换到宽字符串
inline const wchar_t* Port_UCS2ToWideStr(const unsigned short* info, 
	wchar_t* buf, size_t byte_size)
{
	size_t max_len = byte_size / sizeof(wchar_t) - 1;
	const unsigned short* p = info;
	size_t count = 0;
	
	while (*p)
	{
		if (count >= max_len)
		{
			break;
		}
		
		buf[count++] = (wchar_t)*p;
		++p;
	}
	
	buf[count] = 0;
	
	return buf;
}

// 宽字符串转换到UCS2
inline const unsigned short* Port_WideStrToUCS2(const wchar_t* info, 
	unsigned short* buf, size_t byte_size)
{
	size_t max_len = byte_size / sizeof(unsigned short) - 1;
	const wchar_t* p = info;
	size_t count = 0;
	
	while (*p)
	{
		if (count >= max_len)
		{
			break;
		}
		
		buf[count++] = (unsigned short)*p;
		++p;
	}
	
	buf[count] = 0;
	
	return buf;
}

// 获得UCS2字符串长度
inline size_t Port_GetUCS2Len(const unsigned short* info)
{
	const unsigned short* p = info;
	
	while (*p)
	{
		++p;
	}
	
	return p - info;
}

#endif // _SYSTEM_IOS_ENCODING_H
