//--------------------------------------------------------------------
// �ļ���:		i_var_list.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_VAR_LIST_H
#define _PUBLIC_I_VAR_LIST_H

#include "macros.h"
#include "persistid.h"
#include "fast_str.h"

// �������ӿ�

class IVarList
{
public:
	virtual ~IVarList() = 0;
	
	// �ϲ�
	virtual bool Concat(const IVarList& src) = 0;
	// �������
	virtual bool Append(const IVarList& src, size_t start, size_t count) = 0;
	
	// ���
	virtual void Clear() = 0;
	// �Ƿ�Ϊ��
	virtual bool IsEmpty() const = 0;
	// ��������
	virtual size_t GetCount() const = 0; 
	// ��������
	virtual int GetType(size_t index) const = 0;
	
	// �������
	virtual bool AddBool(bool value) = 0;
	virtual bool AddInt(int value) = 0;
	virtual bool AddInt64(int64_t value) = 0;
	virtual bool AddFloat(float value) = 0;
	virtual bool AddDouble(double value) = 0;
	virtual bool AddString(const char* value) = 0;
	virtual bool AddWideStr(const wchar_t* value) = 0;
	virtual bool AddObject(const PERSISTID& value) = 0;
	virtual bool AddPointer(void* value) = 0;
	virtual bool AddUserData(const void* pdata, size_t size) = 0;
	virtual bool AddRawUserData(void* value) = 0;
	
	// �������
	virtual bool BoolVal(size_t index) const = 0;
	virtual int IntVal(size_t index) const = 0;
	virtual int64_t Int64Val(size_t index) const = 0;
	virtual float FloatVal(size_t index) const = 0;
	virtual double DoubleVal(size_t index) const = 0;
	virtual const char* StringVal(size_t index) const = 0;
	virtual const wchar_t* WideStrVal(size_t index) const = 0;
	virtual PERSISTID ObjectVal(size_t index) const = 0;
	virtual void* PointerVal(size_t index) const = 0;
	virtual const void* UserDataVal(size_t index, size_t& size) const = 0;
	virtual void* RawUserDataVal(size_t index) const = 0;

	inline IVarList& operator<<(bool value)
	{ 
		AddBool(value); 
		return *this; 
	}
	inline IVarList& operator<<(char value)
	{ 
		AddInt(value); 
		return *this; 
	}
	inline IVarList& operator<<(unsigned char value)
	{ 
		AddInt(value); 
		return *this; 
	}
	inline IVarList& operator<<(short value)
	{ 
		AddInt(value); 
		return *this; 
	}
	inline IVarList& operator<<(unsigned short value)
	{ 
		AddInt(value); 
		return *this; 
	}
	inline IVarList& operator<<(int value)
	{ 
		AddInt(value); 
		return *this; 
	}
	inline IVarList& operator<<(unsigned int value)
	{ 
		AddInt(value); 
		return *this; 
	}
	inline IVarList& operator<<(long value)
	{ 
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
		AddInt(value); 
#else
		AddInt64(value);
#endif
		return *this; 
	}
	inline IVarList& operator<<(unsigned long value)
	{ 
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
		AddInt(value); 
#else
		AddInt64(value);
#endif
		return *this; 
	}
#if defined(FX_SYSTEM_WINDOWS) || defined(FX_SYSTEM_32BIT)
	inline IVarList& operator<<(int64_t value) 
	{ 
		AddInt64(value); 
		return *this; 
	}
	inline IVarList& operator<<(uint64_t value) 
	{ 
		AddInt64(value); 
		return *this; 
	}
#endif
	inline IVarList& operator<<(float value) 
	{ 
		AddFloat(value); 
		return *this; 
	}
	inline IVarList& operator<<(double value) 
	{ 
		AddDouble(value); 
		return *this; 
	}
	inline IVarList& operator<<(const char* value) 
	{ 
		AddString(value); 
		return *this; 
	}
	inline IVarList& operator<<(const result_string& value) 
	{ 
		AddString(value.c_str()); 
		return *this; 
	}
	inline IVarList& operator<<(const wchar_t* value) 
	{ 
		AddWideStr(value); 
		return *this; 
	}
	inline IVarList& operator<<(const result_wstring& value) 
	{ 
		AddWideStr(value.c_str()); 
		return *this; 
	}
	inline IVarList& operator<<(const PERSISTID& value) 
	{ 
		AddObject(value); 
		return *this; 
	}
	inline IVarList& operator<<(const IVarList& value) 
	{ 
		Concat(value); 
		return *this; 
	}
};

inline IVarList::~IVarList() {}

#endif // _PUBLIC_I_VAR_LIST_H

