//-------------------------------------------------------------------- 
// 文件名:		var_setter.h 
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月5日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_SETTER_H
#define _PUBLIC_VAR_SETTER_H

#include "persistid.h"
#include "i_var.h"
#include "i_var_list.h"
#include "fast_str.h"

// TVarSetter

template<typename TYPE>
struct TVarSetter
{
	static void Set(IVar& v, TYPE value)
	{ 
		// 不使用此缺省方法
		Assert(0);
	}

	static void Set(IVarList& vl, TYPE value)
	{
		// 不使用此缺省方法
		Assert(0);
	}
};

template<>
struct TVarSetter<bool>
{
	static void Set(IVar& v, bool value)
	{ 
		v.SetBool(value);
	}
	
	static void Set(IVarList& vl, bool value)
	{
		vl.AddBool(value);
	}
};

template<>
struct TVarSetter<char>
{
	static void Set(IVar& v, char value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, char value)
	{
		vl.AddInt(value);
	}
};

template<>
struct TVarSetter<unsigned char>
{
	static void Set(IVar& v, unsigned char value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, unsigned char value)
	{
		vl.AddInt(value);
	}
};

template<>
struct TVarSetter<short>
{
	static void Set(IVar& v, short value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, short value)
	{
		vl.AddInt(value);
	}
};

template<>
struct TVarSetter<unsigned short>
{
	static void Set(IVar& v, unsigned short value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, unsigned short value)
	{
		vl.AddInt(value);
	}
};

template<>
struct TVarSetter<int>
{
	static void Set(IVar& v, int value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, int value)
	{
		vl.AddInt(value);
	}
};

template<>
struct TVarSetter<unsigned int>
{
	static void Set(IVar& v, unsigned int value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, unsigned int value)
	{
		vl.AddInt(value);
	}
};

template<>
struct TVarSetter<long>
{
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	static void Set(IVar& v, long value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, long value)
	{
		vl.AddInt(value);
	}
#else
	static void Set(IVar& v, long value)
	{ 
		v.SetInt64(value);
	}

	static void Set(IVarList& vl, long value)
	{
		vl.AddInt64(value);
	}
#endif 
};

template<>
struct TVarSetter<unsigned long>
{
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	static void Set(IVar& v, unsigned long value)
	{ 
		v.SetInt(value);
	}
	
	static void Set(IVarList& vl, unsigned long value)
	{
		vl.AddInt(value);
	}
#else
	static void Set(IVar& v, unsigned long value)
	{ 
		v.SetInt64(value);
	}

	static void Set(IVarList& vl, unsigned long value)
	{
		vl.AddInt64(value);
	}
#endif 
};

#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
template<>
struct TVarSetter<int64_t>
{
	static void Set(IVar& v, int64_t value)
	{ 
		v.SetInt64(value);
	}
	
	static void Set(IVarList& vl, int64_t value)
	{
		vl.AddInt64(value);
	}
};

template<>
struct TVarSetter<uint64_t>
{
	static void Set(IVar& v, uint64_t value)
	{ 
		v.SetInt64(value);
	}
	
	static void Set(IVarList& vl, uint64_t value)
	{
		vl.AddInt64(value);
	}
};
#endif 

template<>
struct TVarSetter<float>
{
	static void Set(IVar& v, float value)
	{ 
		v.SetFloat(value);
	}
	
	static void Set(IVarList& vl, float value)
	{
		vl.AddFloat(value);
	}
};

template<>
struct TVarSetter<double>
{
	static void Set(IVar& v, double value)
	{ 
		v.SetDouble(value);
	}
	
	static void Set(IVarList& vl, double value)
	{
		vl.AddDouble(value);
	}
};

template<>
struct TVarSetter<char*>
{
	static void Set(IVar& v, char* value)
	{ 
		v.SetString(value);
	}
	
	static void Set(IVarList& vl, char* value)
	{
		vl.AddString(value);
	}
};

template<>
struct TVarSetter<const char*>
{
	static void Set(IVar& v, const char* value)
	{ 
		v.SetString(value);
	}
	
	static void Set(IVarList& vl, const char* value)
	{
		vl.AddString(value);
	}
};

template<>
struct TVarSetter<result_string>
{
	static void Set(IVar& v, result_string value)
	{ 
		v.SetString(value.c_str());
	}
	
	static void Set(IVarList& vl, result_string value)
	{
		vl.AddString(value.c_str());
	}
};

template<>
struct TVarSetter<const result_string&>
{
	static void Set(IVar& v, const result_string& value)
	{ 
		v.SetString(value.c_str());
	}
	
	static void Set(IVarList& vl, const result_string& value)
	{
		vl.AddString(value.c_str());
	}
};

template<>
struct TVarSetter<wchar_t*>
{
	static void Set(IVar& v, wchar_t* value)
	{ 
		v.SetWideStr(value);
	}
	
	static void Set(IVarList& vl, wchar_t* value)
	{
		vl.AddWideStr(value);
	}
};

template<>
struct TVarSetter<const wchar_t*>
{
	static void Set(IVar& v, const wchar_t* value)
	{ 
		v.SetWideStr(value);
	}
	
	static void Set(IVarList& vl, const wchar_t* value)
	{
		vl.AddWideStr(value);
	}
};

template<>
struct TVarSetter<result_wstring>
{
	static void Set(IVar& v, result_wstring value)
	{ 
		v.SetWideStr(value.c_str());
	}
	
	static void Set(IVarList& vl, result_wstring value)
	{
		vl.AddWideStr(value.c_str());
	}
};

template<>
struct TVarSetter<const result_wstring&>
{
	static void Set(IVar& v, const result_wstring& value)
	{ 
		v.SetWideStr(value.c_str());
	}
	
	static void Set(IVarList& vl, const result_wstring& value)
	{
		vl.AddWideStr(value.c_str());
	}
};

template<>
struct TVarSetter<const PERSISTID&>
{
	static void Set(IVar& v, const PERSISTID& value)
	{ 
		v.SetObject(value);
	}
	
	static void Set(IVarList& vl, const PERSISTID& value)
	{
		vl.AddObject(value);
	}
};

template<>
struct TVarSetter<PERSISTID>
{
	static void Set(IVar& v, PERSISTID value)
	{ 
		v.SetObject(value);
	}
	
	static void Set(IVarList& vl, PERSISTID value)
	{
		vl.AddObject(value);
	}
};

template<>
struct TVarSetter<void*>
{
	static void Set(IVar& v, void* value)
	{ 
		v.SetRawUserData(value);
	}
	
	static void Set(IVarList& vl, void* value)
	{
		vl.AddRawUserData(value);
	}
};

#endif // _PUBLIC_VAR_SETTER_H
