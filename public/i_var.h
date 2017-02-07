//--------------------------------------------------------------------
// �ļ���:		i_var.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_VAR_H
#define _PUBLIC_I_VAR_H

#include "macros.h"
#include "persistid.h"
#include "var_type.h"
#include <string.h>

// �ɱ���������

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

	// ����
	virtual int GetType() const = 0;
	virtual bool IsInteger() const = 0;
	virtual bool IsReal() const = 0;
	virtual bool IsNumber() const = 0;

	// �������
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

	// ��������
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

	// ����
	virtual void Assign(const IVar& src) = 0;
	
private:
	// ��ֹ����
	IVar& operator=(const IVar&);
};

inline IVar::~IVar() {}

#endif // _PUBLIC_I_VAR_H
