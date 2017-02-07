//--------------------------------------------------------------------
// 文件名:		var.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_H
#define _PUBLIC_VAR_H
#include <wchar.h>
#include "i_var.h"

// 可变类型数据

class TVarAlloc
{
public:
	TVarAlloc() {}
	~TVarAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TVarAlloc& src) {}
};

template<size_t BUFFER_SIZE, typename ALLOC = TVarAlloc>
class TVar: public IVar
{
public:
	typedef TVar<BUFFER_SIZE, ALLOC> self_type;
	
public:
	TVar()
	{
		m_nType = 0;
		m_int64Value = 0;
	}

	TVar(const self_type& src)
	{
		m_nType = src.m_nType;

		switch (m_nType)
		{
		case VTYPE_BOOL:
			m_boolValue = src.m_boolValue;
			break;
		case VTYPE_INT:
			m_intValue = src.m_intValue;
			break;
		case VTYPE_INT64:
			m_int64Value = src.m_int64Value;
			break;
		case VTYPE_FLOAT:
			m_floatValue = src.m_floatValue;
			break;
		case VTYPE_DOUBLE:
			m_doubleValue = src.m_doubleValue;
			break;
		case VTYPE_STRING:
			InnerSetString(src.StringVal());
			break;
		case VTYPE_WIDESTR:
			InnerSetWideStr(src.WideStrVal());
			break;
		case VTYPE_OBJECT:
			m_objectIdent = src.m_objectIdent;
			m_objectSerial = src.m_objectSerial;
			break;
		case VTYPE_POINTER:
			m_pointerValue = src.m_pointerValue;
			break;
		case VTYPE_USERDATA:
			{
				size_t size;
				const void* pdata = src.UserDataVal(size);

				InnerSetUserData(pdata, size);
			}
			break;
		default:
			break;
		}
	}

	explicit TVar(const IVar& src)
	{
		m_nType = src.GetType();
		
		switch (m_nType)
		{
		case VTYPE_BOOL:
			m_boolValue = src.BoolVal();
			break;
		case VTYPE_INT:
			m_intValue = src.IntVal();
			break;
		case VTYPE_INT64:
			m_int64Value = src.Int64Val();
			break;
		case VTYPE_FLOAT:
			m_floatValue = src.FloatVal();
			break;
		case VTYPE_DOUBLE:
			m_doubleValue = src.DoubleVal();
			break;
		case VTYPE_STRING:
			InnerSetString(src.StringVal());
			break;
		case VTYPE_WIDESTR:
			InnerSetWideStr(src.WideStrVal());
			break;
		case VTYPE_OBJECT:
			m_objectIdent = src.ObjectVal().nIdent;
			m_objectSerial = src.ObjectVal().nSerial;
			break;
		case VTYPE_POINTER:
			m_pointerValue = src.PointerVal();
			break;
		case VTYPE_USERDATA:
			{
				size_t size;
				const void* pdata = src.UserDataVal(size);
				
				InnerSetUserData(pdata, size);
			}
			break;
		default:
			break;
		}
	}
	
	TVar(int type, bool value)
	{
		Assert(type == VTYPE_BOOL);

		m_nType = VTYPE_BOOL;
		m_boolValue = value;
	}

	TVar(int type, int value)
	{
		Assert(type == VTYPE_INT);
		
		m_nType = VTYPE_INT;
		m_intValue = value;
	}

	TVar(int type, int64_t value)
	{
		Assert(type == VTYPE_INT64);

		m_nType = VTYPE_INT64;
		m_int64Value = value;
	}
	
	TVar(int type, float value)
	{
		Assert(type == VTYPE_FLOAT);

		m_nType = VTYPE_FLOAT;
		m_floatValue = value;
	}

	TVar(int type, double value)
	{
		Assert(type == VTYPE_DOUBLE);

		m_nType = VTYPE_DOUBLE;
		m_doubleValue = value;
	}

	TVar(int type, const char* value)
	{
		Assert(type == VTYPE_STRING);

		m_nType = VTYPE_STRING;
		InnerSetString(value);
	}

	TVar(int type, const wchar_t* value)
	{
		Assert(type == VTYPE_WIDESTR);
		
		m_nType = VTYPE_WIDESTR;
		InnerSetWideStr(value);
	}

	TVar(int type, const PERSISTID& value)
	{
		Assert(type == VTYPE_OBJECT);

		m_nType = VTYPE_OBJECT;
		m_objectIdent = value.nIdent;
		m_objectSerial = value.nSerial;
	}

	TVar(int type, const void* pdata, size_t size)
	{
		Assert(type == VTYPE_USERDATA);

		m_nType = VTYPE_USERDATA;
		InnerSetUserData(pdata, size);
	}

	virtual ~TVar()
	{
		ReleaseAll();
	}

	self_type& operator=(const self_type& src)
	{
		self_type temp(src);
		Swap(temp);
		return *this;
	}
	
	self_type& operator=(const IVar& src)
	{
		self_type temp(src);
		Swap(temp);
		return *this;
	}
	
	void Swap(self_type& src)
	{
		int temp_type = m_nType;
		int64_t temp_value = m_int64Value;
		unsigned int temp_alloc_length = m_nAllocLength;
		char temp_buffer[BUFFER_SIZE];
		bool temp_use_buffer = 
			((temp_type == VTYPE_STRING) || (temp_type == VTYPE_WIDESTR)) 
			&& (m_stringValue == m_Buffer);

		if (((src.m_nType == VTYPE_STRING) || (src.m_nType == VTYPE_WIDESTR))
			&& (src.m_stringValue == src.m_Buffer))
		{
			if (temp_use_buffer)
			{
				memcpy(temp_buffer, m_Buffer, BUFFER_SIZE);
			}

			memcpy(m_Buffer, src.m_Buffer, BUFFER_SIZE);
			m_stringValue = m_Buffer;
		}
		else
		{
			m_int64Value = src.m_int64Value;
			m_nAllocLength = src.m_nAllocLength;
		}

		m_nType = src.m_nType;

		if (temp_use_buffer)
		{
			memcpy(src.m_Buffer, temp_buffer, BUFFER_SIZE);
			src.m_stringValue = src.m_Buffer;
		}
		else
		{
			src.m_int64Value = temp_value;
			src.m_nAllocLength = temp_alloc_length;
		}

		src.m_nType = temp_type;
		m_Alloc.Swap(src.m_Alloc);
	}

	// 类型
	virtual int GetType() const
	{
		return m_nType;
	}

	virtual bool IsInteger() const
	{
		return (m_nType == VTYPE_INT) || (m_nType == VTYPE_INT64);
	}

	virtual bool IsReal() const
	{
		return (m_nType == VTYPE_FLOAT) || (m_nType == VTYPE_DOUBLE);
	}
	
	virtual bool IsNumber() const
	{
		return IsInteger() || IsReal();
	}

	// 获得数据
	virtual bool BoolVal() const
	{
		switch (m_nType)
		{
		case VTYPE_BOOL:
			return m_boolValue;
		case VTYPE_INT:
			return (m_intValue != 0);
		case VTYPE_INT64:
			return (m_int64Value != 0);
		default:
			break;
		}
		
		return false;
	}

	virtual int IntVal() const
	{
		switch (m_nType)
		{
		case VTYPE_BOOL:
			return int(m_boolValue);
		case VTYPE_INT:
			return m_intValue;
		case VTYPE_INT64:
			return int(m_int64Value);
		case VTYPE_FLOAT:
			return int(m_floatValue);
		case VTYPE_DOUBLE:
			return int(m_doubleValue);
		default:
			break;
		}

		return 0;
	}

	virtual int64_t Int64Val() const
	{
		switch (m_nType)
		{
		case VTYPE_BOOL:
			return int64_t(m_boolValue);
		case VTYPE_INT:
			return int64_t(m_intValue);
		case VTYPE_INT64:
			return m_int64Value;
		case VTYPE_FLOAT:
			return int64_t(m_floatValue);
		case VTYPE_DOUBLE:
			return int64_t(m_doubleValue);
		default:
			break;
		}
		
		return 0;
	}

	virtual float FloatVal() const
	{
		switch (m_nType)
		{
		case VTYPE_BOOL:
			return float(m_boolValue);
		case VTYPE_INT:
			return float(m_intValue);
		case VTYPE_INT64:
			return float(m_int64Value);
		case VTYPE_FLOAT:
			return m_floatValue;
		case VTYPE_DOUBLE:
			return float(m_doubleValue);
		default:
			break;
		}
		
		return 0.0f;
	}

	virtual double DoubleVal() const
	{
		switch (m_nType)
		{
		case VTYPE_BOOL:
			return double(m_boolValue);
		case VTYPE_INT:
			return double(m_intValue);
		case VTYPE_INT64:
			return double(m_int64Value);
		case VTYPE_FLOAT:
			return double(m_floatValue);
		case VTYPE_DOUBLE:
			return m_doubleValue;
		default:
			break;
		}
		
		return 0.0;
	}

	virtual const char* StringVal() const
	{
		if (m_nType != VTYPE_STRING)
		{
			return "";
		}
		
		return m_stringValue;
	}

	virtual const wchar_t* WideStrVal() const
	{
		if (m_nType != VTYPE_WIDESTR)
		{
			return L"";
		}
		
		return m_widestrValue;
	}

	virtual PERSISTID ObjectVal() const
	{
		if (m_nType != VTYPE_OBJECT)
		{
			return PERSISTID();
		}
		
		return PERSISTID(m_objectIdent, m_objectSerial);
	}

	virtual void* PointerVal() const
	{
		if (m_nType != VTYPE_POINTER)
		{
			return NULL;
		}
		
		return m_pointerValue;
	}

	virtual const void* UserDataVal(size_t& size) const
	{
		if (m_nType != VTYPE_USERDATA)
		{
			size = 0;
			return NULL;
		}
		
		size = IVar::GetUserDataSize(m_userdataValue);
				
		return IVar::GetUserDataPointer(m_userdataValue);
	}

	virtual void* RawUserDataVal() const
	{
		if (m_nType != VTYPE_USERDATA)
		{
			return NULL;
		}
		
		return m_userdataValue;
	}
	
	// 设置数据
	virtual void SetUnknown()
	{
		ReleaseAll();
		m_nType = VTYPE_UNKNOWN;
		m_int64Value = 0;
	}
	
	virtual void SetBool(bool value)
	{
		ReleaseAll();
		m_nType = VTYPE_BOOL;
		m_boolValue = value;
	}

	virtual void SetInt(int value)
	{
		ReleaseAll();
		m_nType = VTYPE_INT;
		m_intValue = value;
	}
	
	virtual void SetInt64(int64_t value)
	{
		ReleaseAll();
		m_nType = VTYPE_INT64;
		m_int64Value = value;
	}
	
	virtual void SetFloat(float value)
	{
		ReleaseAll();
		m_nType = VTYPE_FLOAT;
		m_floatValue = value;
	}
	
	virtual void SetDouble(double value)
	{
		ReleaseAll();
		m_nType = VTYPE_DOUBLE;
		m_doubleValue = value;
	}
	
	virtual void SetString(const char* value)
	{
		ReleaseAll();
		m_nType = VTYPE_STRING;
		InnerSetString(value);
	}
	
	virtual void SetWideStr(const wchar_t* value)
	{
		ReleaseAll();
		m_nType = VTYPE_WIDESTR;
		InnerSetWideStr(value);
	}
	
	virtual void SetObject(const PERSISTID& value)
	{
		ReleaseAll();
		m_nType = VTYPE_OBJECT;
		m_objectIdent = value.nIdent;
		m_objectSerial = value.nSerial;
	}

	virtual void SetPointer(void* value)
	{
		ReleaseAll();
		m_nType = VTYPE_POINTER;
		m_pointerValue = value;
	}
	
	virtual void SetUserData(const void* pdata, size_t size)
	{
		ReleaseAll();
		m_nType = VTYPE_USERDATA;
		InnerSetUserData(pdata, size);
	}
	
	virtual void SetRawUserData(void* value)
	{
		SetUserData(IVar::GetUserDataPointer(value), 
			IVar::GetUserDataSize(value));
	}

	virtual void Assign(const IVar& src)
	{
		self_type temp(src);
		Swap(temp);
	}

private:
	void ReleaseAll()
	{
		switch (m_nType)
		{
		case VTYPE_STRING:
			if (m_stringValue != m_Buffer)
			{
				m_Alloc.Free(m_stringValue, m_nAllocLength);
				m_stringValue = NULL;
			}
			break;
		case VTYPE_WIDESTR:
			if ((char*)m_widestrValue != m_Buffer)
			{
				m_Alloc.Free(m_widestrValue, m_nAllocLength);
				m_widestrValue = NULL;
			}
			break;
		case VTYPE_USERDATA:
			if (m_userdataValue != NULL)
			{
				m_Alloc.Free(m_userdataValue, m_nAllocLength);
				m_userdataValue = NULL;
			}
			break;
		default:
			break;
		}
	}

	void InnerSetString(const char* value)
	{
		const size_t value_size = strlen(value) + 1;
		char* p;
		
		if (value_size > BUFFER_SIZE)
		{
			p = (char*)m_Alloc.Alloc(value_size);
			m_nAllocLength = (unsigned int)value_size;
		}
		else
		{
			p = m_Buffer;
		}
		
		memcpy(p, value, value_size);
		m_stringValue = p;
	}

	void InnerSetWideStr(const wchar_t* value)
	{
		const size_t value_size = (wcslen(value) + 1) * sizeof(wchar_t);
		char* p;
		
		if (value_size > BUFFER_SIZE)
		{
			p = (char*)m_Alloc.Alloc(value_size);
			m_nAllocLength = (unsigned int)value_size;
		}
		else
		{
			p = m_Buffer;
		}
		
		memcpy(p, value, value_size);
		m_widestrValue = (wchar_t*)p;
	}

	void InnerSetUserData(const void* pdata, size_t size)
	{
		size_t value_size = IVar::GetRawUserDataSize(size);
		char* p = (char*)m_Alloc.Alloc(value_size);
		IVar::InitRawUserData(p, pdata, size);
		m_userdataValue = p;
		m_nAllocLength = (unsigned int)value_size;
	}

private:
	ALLOC m_Alloc;
	union
	{
		bool m_boolValue;
		int m_intValue;
		int64_t m_int64Value;
		float m_floatValue;
		double m_doubleValue;
		char* m_stringValue;
		wchar_t* m_widestrValue;
		void* m_pointerValue;
		char* m_userdataValue;
		struct
		{
			unsigned int m_objectIdent;
			unsigned int m_objectSerial;
		};
	};
	int m_nType;
	union
	{
		unsigned int m_nAllocLength;
		char m_Buffer[BUFFER_SIZE];
	};
};

typedef TVar<4> CVar;

#endif // _PUBLIC_VAR_H
