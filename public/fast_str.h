//--------------------------------------------------------------------
// 文件名:		fast_str.h
// 内  容:		小字符串优化
// 说  明:		
// 创建日期:	2007年2月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_FAST_STR_H
#define _PUBLIC_FAST_STR_H

#include "../public/macros.h"
#include "../public/char_traits.h"

class TFastStrAlloc
{
public:
	TFastStrAlloc() {}
	~TFastStrAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TFastStrAlloc& src) {}
};

// TFastStr

template<typename TYPE, size_t SIZE, typename TRAITS = TCharTraits<TYPE>, 
	typename ALLOC = TFastStrAlloc>
class TFastStr
{
private:
	typedef TFastStr<TYPE, SIZE, TRAITS, ALLOC> self_type;

public:
	TFastStr()
	{
		m_Stack[0] = 0;
		m_pData = m_Stack;
		m_nCapacity = SIZE;
		m_nSize = 0;
	}

	TFastStr(const self_type& src)
	{
		init(src.c_str(), src.length());
	}

	TFastStr(const TYPE* src)
	{
		init(src, TRAITS::Length(src));
	}

	TFastStr(const TYPE* src, size_t len)
	{
		init(src, len);
	}

	TFastStr(const TYPE* s1, const TYPE* s2)
	{
		init2(s1, TRAITS::Length(s1), s2, TRAITS::Length(s2));
	}

	TFastStr(const TYPE* s1, size_t len1, const TYPE* s2, size_t len2)
	{
		init2(s1, len1, s2, len2);
	}

    // 重载构造函数，适应不同size的类别转换
    template<size_t _SIZE>
    TFastStr(const TFastStr<TYPE, _SIZE, TRAITS, ALLOC>& src)
    {
        init(src.c_str(), src.length());
    }

	~TFastStr()
	{
		if (m_nCapacity > SIZE)
		{
			m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
		}
	}

	self_type& operator=(const self_type& src)
	{
		return inner_assign(src.c_str(), src.length());
	}

    // 重载operator=，适应不同size的类别转换
    template<size_t _SIZE>
    self_type& operator=(const TFastStr<TYPE, _SIZE, TRAITS, ALLOC>& src)
    {
        return inner_assign(src.c_str(), src.length());
    }

	self_type& operator=(const TYPE* src)
	{
		return inner_assign(src, TRAITS::Length(src));
	}

	self_type& operator=(TYPE ch)
	{
		return inner_assign(&ch, 1);
	}

	self_type& operator+=(const self_type& src)
	{
		return inner_append(src.c_str(), src.length());
	}

	self_type& operator+=(const TYPE* src)
	{
		return inner_append(src, TRAITS::Length(src));
	}

	self_type& operator+=(TYPE ch)
	{
		return inner_append(&ch, 1);
	}

	void swap(self_type& src)
	{
		size_t temp_size = src.m_nSize;
		size_t temp_capacity = src.m_nCapacity;
		TYPE* temp_pdata = src.m_pData;
		TYPE temp_stack[SIZE];

		if (temp_capacity <= SIZE)
		{
			TRAITS::Copy(temp_stack, src.m_Stack, temp_size + 1);
		}

		src.m_nSize = m_nSize;
		src.m_nCapacity = m_nCapacity;

		if (m_nCapacity <= SIZE)
		{
			TRAITS::Copy(src.m_Stack, m_Stack, m_nSize + 1);
			src.m_pData = src.m_Stack;
		}
		else
		{
			src.m_pData = m_pData;
		}

		m_nSize = temp_size;
		m_nCapacity = temp_capacity;

		if (temp_capacity <= SIZE)
		{
			TRAITS::Copy(m_Stack, temp_stack, temp_size + 1);
			m_pData = m_Stack;
		}
		else
		{
			m_pData = temp_pdata;
		}

		m_Alloc.Swap(src.m_Alloc);
	}

	bool empty() const
	{
		return (0 == m_nSize);
	}

	size_t length() const
	{
		return m_nSize;
	}

	size_t size() const
	{
		return m_nSize;
	}

	size_t capacity() const
	{
		return m_nCapacity;
	}

	void clear()
	{
		if (m_pData != m_Stack)
		{
			m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
		}

		m_Stack[0] = 0;
		m_pData = m_Stack;
		m_nCapacity = SIZE;
		m_nSize = 0;
	}

	const TYPE* c_str() const
	{
		return m_pData;
	}

	int compare(const self_type& src) const
	{
		return TRAITS::Compare(c_str(), src.c_str());
	}

	int compare(const TYPE* src) const
	{
		return TRAITS::Compare(c_str(), src);
	}

	self_type& append(const self_type& src)
	{
		return inner_append(src.c_str(), src.length());
	}

	self_type& append(const TYPE* src)
	{
		return inner_append(src, TRAITS::Length(src));
	}

	self_type& append(TYPE ch)
	{
		return inner_append(&ch, 1);
	}

	self_type& append(const TYPE* src, size_t len)
	{
		return inner_append(src, len);
	}

	size_t find(const TYPE* src, const size_t begin = 0)
	{
		return TRAITS::Find(m_pData, src, begin);
	}

	size_t find_last_of(TYPE value)
	{
		for (int i = int(m_nSize -  1); i >= 0; --i)
		{
			if (m_pData[i] == value)
			{
				return i;
			}
		}

		return npos();
	}

	self_type substr(size_t begin, size_t len)
	{
		return self_type(&m_pData[begin], len);
	}

	size_t npos()
	{
		return TRAITS::NPos();
	}

	size_t get_memory_usage() const
	{
		size_t size = sizeof(self_type);

		if (m_nCapacity > SIZE)
		{
			size += m_nCapacity * sizeof(TYPE);
		}	

		return size;
	}

private:
	void init(const TYPE* src, size_t len)
	{
		m_nSize = len;

		if (m_nSize < SIZE)
		{
			m_nCapacity = SIZE;
			m_pData = m_Stack;
		}
		else
		{
			m_nCapacity = (m_nSize + 1) * 2;
			m_pData = (TYPE*)m_Alloc.Alloc(m_nCapacity * sizeof(TYPE));
		}

		TRAITS::Copy(m_pData, src, m_nSize);
		TRAITS::Put(m_pData + m_nSize, 0);
	}

	void init2(const TYPE* s1, size_t SIZE1, const TYPE* s2, size_t SIZE2)
	{
		m_nSize = SIZE1 + SIZE2;

		if (m_nSize < SIZE)
		{
			m_nCapacity = SIZE;
			m_pData = m_Stack;
		}
		else
		{
			m_nCapacity = (m_nSize + 1) * 2;
			m_pData = (TYPE*)m_Alloc.Alloc(m_nCapacity * sizeof(TYPE));
		}

		TRAITS::Copy(m_pData, s1, SIZE1);
		TRAITS::Copy(m_pData + SIZE1, s2, SIZE2);
		TRAITS::Put(m_pData + SIZE1 + SIZE2, 0);
	}

	self_type& inner_assign(const TYPE* s, size_t len)
	{
		if (len < m_nCapacity)
		{
			TRAITS::Copy(m_pData, s, len);
			TRAITS::Put(m_pData + len, 0);
			m_nSize = len;
		}
		else
		{
			self_type temp(s, len);
			swap(temp);
		}

		return *this;
	}

	self_type& inner_append(const TYPE* s, size_t len)
	{
		const size_t NEW_SIZE = m_nSize + len;

		if (NEW_SIZE < m_nCapacity)
		{
			TRAITS::Copy(m_pData + m_nSize, s, len);
			TRAITS::Put(m_pData + m_nSize + len, 0);
			m_nSize = NEW_SIZE;
		}
		else
		{
			self_type temp(c_str(), length(), s, len);
			swap(temp);
		}

		return *this;
	}

private:
	ALLOC m_Alloc;
	TYPE m_Stack[SIZE];
	TYPE* m_pData;
	size_t m_nCapacity;
	size_t m_nSize;
};

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return (s1.compare(s2) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TYPE* s2)
{
	return (s1.compare(s2) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TYPE* s1,
	const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return (s2.compare(s1) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return (s1.compare(s2) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TYPE* s2)
{
	return (s1.compare(s2) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TYPE* s1,
	const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return (s2.compare(s1) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TFastStr<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return TFastStr<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TFastStr<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2)
{
	return TFastStr<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TFastStr<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s1, TYPE ch)
{
	return TFastStr<TYPE, SIZE, TRAITS, ALLOC>(s1).append(ch);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TFastStr<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TYPE* s1, const TFastStr<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return TFastStr<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

typedef TFastStr<char, 128> result_string;
typedef TFastStr<wchar_t, 128> result_wstring;

#endif // _PUBLIC_FAST_STR_H
