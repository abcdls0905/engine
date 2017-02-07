//--------------------------------------------------------------------
// 文件名:		i_var.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_VAR_H
#define _PUBLIC_I_VAR_H

#include "macros.h"
#include "persistid.h"
#include "var_type.h"
#include <string.h>

// 可变类型数据

class IVar
{
public:
	static inline const void* GetUserDataPointer(void* value)
	{
		return (char*)value + sizeof(size_t);
	}
	
	static inline size_t GetUserDataSize(void* value)
	{
		return *(size_t*)value;
	}
	
	static inline size_t GetRawUserDataSize(size_t size)
	{
		return sizeof(size_t) + size;
	}
	
	static inline void InitRawUserData(void* p, 
		const void* pdata, size_t size)
	{
		*(size_t*)p = size;
		memcpy((char*)p + sizeof(size), pdata, size);
	}
	
public:
	virtual ~IVar() = 0;

	// 类型
	virtual int GetType() const = 0;
	virtual bool IsInteger() const = 0;
	virtual bool IsReal() const = 0;
	virtual bool IsNumber() const = 0;

	// 获得数据
	virtual bool BoolVal() const = 0;
	virtual int IntVal() const = 0;
	virtual int64_t Int64Val() const = 0;
	virtual float FloatVal() const = 0;
	virtual double DoubleVal() const = 0;
	virtual const char* StringVal() const = 0;
	virtual const wchar_t* WideStrVal() const = 0;
	virtual PERSISTID ObjectVal() const = 0;
	virtual void* PointerVal() const = 0;
	virtual const void* UserDataVal(size_t& size) const = 0;
	virtual void* RawUserDataVal() const = 0;

	// 设置数据
	virtual void SetUnknown() = 0;
	virtual void SetBool(bool value) = 0;
	virtual void SetInt(int value) = 0;
	virtual void SetInt64(int64_t value) = 0;
	virtual void SetFloat(float value) = 0;
	virtual void SetDouble(double value) = 0;
	virtual void SetString(const char* value) = 0;
	virtual void SetWideStr(const wchar_t* value) = 0;
	virtual void SetObject(const PERSISTID& value) = 0;
	virtual void SetPointer(void* value) = 0;
	virtual void SetUserData(const void* pdata, size_t size) = 0;
	virtual void SetRawUserData(void* value) = 0;

	// 复制
	virtual void Assign(const IVar& src) = 0;
	
private:
	// 禁止拷贝
	IVar& operator=(const IVar&);
};

inline IVar::~IVar() {}

#endif // _PUBLIC_I_VAR_H
