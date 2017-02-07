//-------------------------------------------------------------------- 
// 文件名:		var_getter.h 
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月5日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_GETTER_H
#define _PUBLIC_VAR_GETTER_H

#include "persistid.h"
#include "i_var.h"
#include "i_var_list.h"
#include "fast_str.h"

// TVarGetter

template<typename TYPE>
struct TVarGetter
{
	static TYPE Get(const IVar& vl)
	{ 
		// 不使用此缺省方法
		Assert(0);
	}
	
	static TYPE Get(const IVarList& vl, size_t index)
	{ 
		// 不使用此缺省方法
		Assert(0);
	}
};

template<>
struct TVarGetter<bool>
{
	static bool Get(const IVar& v)
	{
		return v.BoolVal();
	}
	
	static bool Get(const IVarList& vl, size_t index)
	{ 
		return vl.BoolVal(index); 
	}
};

template<>
struct TVarGetter<char>
{
	static char Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static char Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
};

template<>
struct TVarGetter<unsigned char>
{
	static unsigned char Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static unsigned char Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
};

template<>
struct TVarGetter<short>
{
	static short Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static short Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
};

template<>
struct TVarGetter<unsigned short>
{
	static unsigned short Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static unsigned short Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
};

template<>
struct TVarGetter<int>
{
	static int Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static int Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
};

template<>
struct TVarGetter<unsigned int>
{
	static unsigned int Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static unsigned int Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
};

template<>
struct TVarGetter<long>
{
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	static long Get(const IVar& v)
	{
		return v.IntVal();
	}
	
	static long Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
#else
	static long Get(const IVar& v)
	{
		return (long)v.Int64Val();
	}

	static long Get(const IVarList& vl, size_t index)
	{ 
		return (long)vl.Int64Val(index); 
	}
#endif 
};

template<>
struct TVarGetter<unsigned long>
{
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	static unsigned long Get(const IVar& v)
	{
		return v.IntVal();
	}

	static unsigned long Get(const IVarList& vl, size_t index)
	{ 
		return vl.IntVal(index); 
	}
#else
	static unsigned long Get(const IVar& v)
	{
		return (unsigned long)v.Int64Val();
	}

	static unsigned long Get(const IVarList& vl, size_t index)
	{ 
		return (unsigned long)vl.Int64Val(index); 
	}
#endif
};

#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
template<>
struct TVarGetter<int64_t>
{
	static int64_t Get(const IVar& v)
	{
		return v.Int64Val();
	}
	
	static int64_t Get(const IVarList& vl, size_t index)
	{ 
		return vl.Int64Val(index); 
	}
};

template<>
struct TVarGetter<uint64_t>
{
	static uint64_t Get(const IVar& v)
	{
		return v.Int64Val();
	}
	
	static uint64_t Get(const IVarList& vl, size_t index)
	{ 
		return vl.Int64Val(index); 
	}
};
#endif

template<>
struct TVarGetter<float>
{
	static float Get(const IVar& v)
	{
		return v.FloatVal();
	}
	
	static float Get(const IVarList& vl, size_t index)
	{ 
		return vl.FloatVal(index); 
	}
};

template<>
struct TVarGetter<double>
{
	static double Get(const IVar& v)
	{
		return v.DoubleVal();
	}
	
	static double Get(const IVarList& vl, size_t index)
	{ 
		return vl.DoubleVal(index); 
	}
};

template<>
struct TVarGetter<const char*>
{
	static const char* Get(const IVar& v)
	{
		return v.StringVal();
	}
	
	static const char* Get(const IVarList& vl, size_t index)
	{ 
		return vl.StringVal(index); 
	}
};

template<>
struct TVarGetter<result_string>
{
	static const char* Get(const IVar& v)
	{
		return v.StringVal();
	}
	
	static const char* Get(const IVarList& vl, size_t index)
	{ 
		return vl.StringVal(index);
	}
};

template<>
struct TVarGetter<const result_string&>
{
	static const char* Get(const IVar& v)
	{
		return v.StringVal();
	}
	
	static const char* Get(const IVarList& vl, size_t index)
	{ 
		return vl.StringVal(index);
	}
};

template<>
struct TVarGetter<const wchar_t*>
{
	static const wchar_t* Get(const IVar& v)
	{
		return v.WideStrVal();
	}
	
	static const wchar_t* Get(const IVarList& vl, size_t index)
	{ 
		return vl.WideStrVal(index); 
	}
};

template<>
struct TVarGetter<result_wstring>
{
	static const wchar_t* Get(const IVar& v)
	{
		return v.WideStrVal();
	}
	
	static const wchar_t* Get(const IVarList& vl, size_t index)
	{ 
		return vl.WideStrVal(index); 
	}
};

template<>
struct TVarGetter<const result_wstring&>
{
	static const wchar_t* Get(const IVar& v)
	{
		return v.WideStrVal();
	}
	
	static const wchar_t* Get(const IVarList& vl, size_t index)
	{ 
		return vl.WideStrVal(index); 
	}
};

template<>
struct TVarGetter<PERSISTID>
{
	static PERSISTID Get(const IVar& v)
	{
		return v.ObjectVal();
	}
	
	static PERSISTID Get(const IVarList& vl, size_t index)
	{ 
		return vl.ObjectVal(index); 
	}
};

template<>
struct TVarGetter<const PERSISTID&>
{
	static PERSISTID Get(const IVar& v)
	{
		return v.ObjectVal();
	}
	
	static PERSISTID Get(const IVarList& vl, size_t index)
	{ 
		return vl.ObjectVal(index); 
	}
};

template<>
struct TVarGetter<void*>
{
	static void* Get(const IVar& v)
	{
		return v.RawUserDataVal();
	}
	
	static void* Get(const IVarList& vl, size_t index)
	{ 
		return vl.RawUserDataVal(index); 
	}
};

#endif // _PUBLIC_VAR_GETTER_H
