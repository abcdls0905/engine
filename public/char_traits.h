//--------------------------------------------------------------------
// 文件名:		char_traits.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CHAR_TRAITS_H
#define _PUBLIC_CHAR_TRAITS_H

#include <string.h>
#include <wchar.h>

// TCharTraits

template<typename TYPE>
struct TCharTraits
{
};

template<>
struct TCharTraits<char>
{
	static size_t Length(const char* s)
	{
		return strlen(s);
	}

	static int Compare(const char* s1, const char* s2)
	{
		return strcmp(s1, s2);
	}
	
	static void Copy(char* dst, const char* src, size_t len)
	{
		memcpy(dst, src, len);
	}
	
	static void Put(char* dst, char ch)
	{
		*dst = ch;
	}
	
	static const char* EmptyValue()
	{
		return "";
	}
    
    static size_t Find(const char* dst, const char* find, size_t begin = 0)
    {
        const char* pos = strstr(&dst[begin], find);

        if (NULL == pos)
        {
            return size_t(-1);
        }

        return size_t(pos - dst);
    }

    static size_t NPos()
    {
        return size_t(-1);
    }
};

template<>
struct TCharTraits<wchar_t>
{
	static size_t Length(const wchar_t* s)
	{
		return wcslen(s);
	}
	
	static int Compare(const wchar_t* s1, const wchar_t* s2)
	{
		return wcscmp(s1, s2);
	}
	
	static void Copy(wchar_t* dst, const wchar_t* src, size_t len)
	{
		memcpy(dst, src, len * sizeof(wchar_t));
	}
	
	static void Put(wchar_t* dst, wchar_t ch)
	{
		*dst = ch;
	}
	
	static const wchar_t* EmptyValue()
	{
		return L"";
	}
    
    static size_t Find(const wchar_t* dst, const wchar_t* find, size_t begin = 0)
    {
        const wchar_t* pos = wcsstr(&dst[begin], find);

        if (NULL == pos)
        {
            return size_t(-1);
        }

        return size_t(pos - dst);
    }

    static size_t NPos()
    {
        return size_t(-1);
    }
};

#endif // _PUBLIC_CHAR_TRAITS_H
