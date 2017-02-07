//--------------------------------------------------------------------
// 文件名:		string_traits.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年4月10日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_STRING_TRAITS_H
#define _UTILS_STRING_TRAITS_H

#include "../public/macros.h"
#include "../public/inlines.h"
#include <string.h>

// TStringTraits

template<typename TYPE>
class TStringTraits
{
public:
	static size_t GetHash(const TYPE* s);
	static size_t Length(const TYPE* s);
	static bool Equal(const TYPE* s1, const TYPE* s2);
	static void Copy(TYPE* dst, const TYPE* src, size_t len);
};

template<>
class TStringTraits<char>
{
public:
	static size_t GetHash(const char* s)
	{
		return GetHashValueCase(s);
	}

	static size_t Length(const char* s)
	{
		return strlen(s);
	}

	static bool Equal(const char* s1, const char* s2)
	{
		return (strcmp(s1, s2) == 0);
	}
	
	static void Copy(char* dst, const char* src, size_t len)
	{
		memcpy(dst, src, len);
	}
};

template<>
class TStringTraits<wchar_t>
{
public:
	static size_t GetHash(const wchar_t* s)
	{
		return GetHashValueCaseW(s);
	}
	
	static size_t Length(const wchar_t* s)
	{
		return wcslen(s);
	}
	
	static bool Equal(const wchar_t* s1, const wchar_t* s2)
	{
		return (wcscmp(s1, s2) == 0);
	}
	
	static void Copy(wchar_t* dst, const wchar_t* src, size_t len)
	{
		memcpy(dst, src, len * sizeof(wchar_t));
	}
};

template<typename TYPE>
class TStringTraitsCI: public TStringTraits<TYPE>
{
};

template<>
class TStringTraitsCI<char>: public TStringTraits<char>
{
public:
	static size_t GetHash(const char* s)
	{
		return GetHashValue(s);
	}
	
	static bool Equal(const char* s1, const char* s2)
	{
		return (stricmp(s1, s2) == 0);
	}
};

template<>
class TStringTraitsCI<wchar_t>: public TStringTraits<wchar_t>
{
public:
	static size_t GetHash(const wchar_t* s)
	{
		return GetHashValueW(s);
	}
	
	static bool Equal(const wchar_t* s1, const wchar_t* s2)
	{
		return (wcsicmp(s1, s2) == 0);
	}
};

#endif // _UTILS_STRING_TRAITS_H
