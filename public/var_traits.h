//-------------------------------------------------------------------- 
// 文件名:		var_traits.h 
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月5日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_TRAITS_H
#define _PUBLIC_VAR_TRAITS_H

#include "var_type.h"
#include "persistid.h"
#include "fast_str.h"

// 提取数据类型

template<typename TYPE>
struct TVarTraits
{
	enum { Type = VTYPE_UNKNOWN };
};

template<>
struct TVarTraits<bool>
{
	enum { Type = VTYPE_BOOL };
};

template<>
struct TVarTraits<char>
{
	enum { Type = VTYPE_INT };
};

template<>
struct TVarTraits<unsigned char>
{
	enum { Type = VTYPE_INT };
};

template<>
struct TVarTraits<short>
{
	enum { Type = VTYPE_INT };
};

template<>
struct TVarTraits<unsigned short>
{
	enum { Type = VTYPE_INT };
};

template<>
struct TVarTraits<int>
{
	enum { Type = VTYPE_INT };
};

template<>
struct TVarTraits<unsigned int>
{
	enum { Type = VTYPE_INT };
};

template<>
struct TVarTraits<long>
{
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	enum { Type = VTYPE_INT };
#else
	enum { Type = VTYPE_INT64 };
#endif
};

template<>
struct TVarTraits<unsigned long>
{
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	enum { Type = VTYPE_INT };
#else
	enum { Type = VTYPE_INT64 };
#endif
};

#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
template<>
struct TVarTraits<int64_t>
{
	enum { Type = VTYPE_INT64 };
};

template<>
struct TVarTraits<uint64_t>
{
	enum { Type = VTYPE_INT64 };
};
#endif

template<>
struct TVarTraits<float>
{
	enum { Type = VTYPE_FLOAT };
};

template<>
struct TVarTraits<double>
{
	enum { Type = VTYPE_DOUBLE };
};

template<>
struct TVarTraits<char*>
{
	enum { Type = VTYPE_STRING };
};

template<>
struct TVarTraits<const char*>
{
	enum { Type = VTYPE_STRING };
};

template<>
struct TVarTraits<result_string>
{
	enum { Type = VTYPE_STRING };
};

template<>
struct TVarTraits<const result_string&>
{
	enum { Type = VTYPE_STRING };
};

template<>
struct TVarTraits<wchar_t*>
{
	enum { Type = VTYPE_WIDESTR };
};

template<>
struct TVarTraits<const wchar_t*>
{
	enum { Type = VTYPE_WIDESTR };
};

template<>
struct TVarTraits<result_wstring>
{
	enum { Type = VTYPE_WIDESTR };
};

template<>
struct TVarTraits<const result_wstring&>
{
	enum { Type = VTYPE_WIDESTR };
};

template<>
struct TVarTraits<PERSISTID>
{
	enum { Type = VTYPE_OBJECT };
};

template<>
struct TVarTraits<const PERSISTID&>
{
	enum { Type = VTYPE_OBJECT };
};

template<>
struct TVarTraits<void*>
{
	enum { Type = VTYPE_USERDATA };
};

#endif // _PUBLIC_VAR_TRAITS_H
