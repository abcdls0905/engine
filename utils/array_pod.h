//--------------------------------------------------------------------
// 文件名:		array_pod.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年12月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_ARRAY_POD_H
#define _UTILS_ARRAY_POD_H

#include "../public/macros.h"
#include <string.h>

// 高效的POD类型数组容器

class TArrayPodAlloc
{
public:
	TArrayPodAlloc() {}
	~TArrayPodAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TArrayPodAlloc& src) {}
};

template<typename TYPE, size_t SIZE, typename ALLOC = TArrayPodAlloc>
class TArrayPod
{
private:
	typedef TArrayPod<TYPE, SIZE, ALLOC> self_type;

public:
	TArrayPod()
	{
		m_pData = m_Stack;
		m_nCapacity = SIZE;
		m_nSize = 0;
	}

	TArrayPod(const self_type& src)
	{
		m_nSize = src.m_nSize;

		if (m_nSize <= SIZE)
		{
			m_pData = m_Stack;
			m_nCapacity = SIZE;
		}
		else
		{
			m_nCapacity = src.m_nCapacity;
			m_pData = (TYPE*) m_Alloc.Alloc(m_nCapacity * sizeof(TYPE));
		}

		memcpy(m_pData, src.m_pData, m_nSize * sizeof(TYPE));
	}

	~TArrayPod()
	{
		if (m_nCapacity > SIZE)
		{
			m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
		}
	}

	self_type& operator=(const self_type& src)
	{
		self_type temp(src);
		swap(temp);
		return *this;
	}

	void swap(self_type& src)
	{
		size_t temp_size = src.m_nSize;
		size_t temp_capacity = src.m_nCapacity;
		TYPE* temp_pdata = src.m_pData;
		TYPE temp_stack[SIZE];

		if (temp_capacity <= SIZE)
		{
			memcpy(temp_stack, src.m_Stack, temp_size * sizeof(TYPE));
		}

		src.m_nSize = m_nSize;
		src.m_nCapacity = m_nCapacity;

		if (m_nCapacity <= SIZE)
		{
			memcpy(src.m_Stack, m_Stack, m_nSize * sizeof(TYPE));
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
			memcpy(m_Stack, temp_stack, temp_size * sizeof(TYPE));
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
	
	size_t size() const
	{
		return m_nSize;
	}
	
	const TYPE* data() const
	{
		return m_pData;
	}
	
	TYPE* data()
	{
		return m_pData;
	}
	
	void push_back(const TYPE& data)
	{
		if (m_nSize == m_nCapacity)
		{
			size_t new_size = m_nSize * 2;
			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pData, m_nSize * sizeof(TYPE));
				
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
			}
			
			m_pData = p;
			m_nCapacity = new_size;
		}
		
		m_pData[m_nSize++] = data;
	}
	
	void pop_back()
	{
		Assert(m_nSize > 0);
		--m_nSize;
	}
	
	TYPE& back()
	{
		Assert(m_nSize > 0);
		return m_pData[m_nSize - 1];
	}
	
	const TYPE& back() const
	{
		Assert(m_nSize > 0);
		return m_pData[m_nSize - 1];
	}
	
	TYPE& operator[](size_t index)
	{
		Assert(index < m_nSize);
		return m_pData[index];
	}
	
	const TYPE& operator[](size_t index) const
	{
		Assert(index < m_nSize);
		return m_pData[index];
	}
	
	void reserve(size_t size)
	{
		if (size > m_nCapacity)
		{
			TYPE* p = (TYPE*)m_Alloc.Alloc(size * sizeof(TYPE));
			
			memcpy(p, m_pData, m_nSize * sizeof(TYPE));
			
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
			}
			
			m_pData = p;
			m_nCapacity = size;
		}
	}
	
	void resize(size_t size)
	{
		if (size > m_nCapacity)
		{
			size_t new_size = m_nCapacity * 2;
			
			if (new_size < size)
			{
				new_size = size;
			}
			
			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pData, m_nSize * sizeof(TYPE));
				
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
			}
			
			m_pData = p;
			m_nCapacity = new_size;
		}
		
		m_nSize = size;
	}
	
	void resize(size_t size, const TYPE& value)
	{
		if (size > m_nCapacity)
		{
			size_t new_size = m_nCapacity * 2;
			
			if (new_size < size)
			{
				new_size = size;
			}
			
			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pData, m_nSize * sizeof(TYPE));
				
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pData, m_nCapacity * sizeof(TYPE));
			}
			
			m_pData = p;
			m_nCapacity = new_size;
		}
		
		if (size > m_nSize)
		{
			for (size_t i = m_nSize; i < size; ++i)
			{
				m_pData[i] = value;
			}
		}
		
		m_nSize = size;
	}
	
	void insert(size_t index, const TYPE& data)
	{
		Assert(index <= m_nSize);
		
		resize(m_nSize + 1);
		TYPE* p = m_pData + index;
		memmove(p + 1, p, (m_nSize - index - 1) * sizeof(TYPE));
		*p = data;
	}
	
	void remove(size_t index)
	{
		Assert(index < m_nSize);
		
		TYPE* p = m_pData + index;
		memmove(p, p + 1, (m_nSize - index - 1) * sizeof(TYPE));
		--m_nSize;
	}
	
	void remove_from(size_t start, size_t amount)
	{
		Assert((start <= m_nSize) && ((start + amount) <= m_nSize));
		
		TYPE* p = m_pData + start;
		memmove(p, p + amount, (m_nSize - start - amount) * sizeof(TYPE));
		m_nSize -= amount;
	}
	
	void clear()
	{
		m_nSize = 0;
	}

	size_t get_memory_usage() const
	{
		size_t size = sizeof(self_type);
		
		if (m_nCapacity > SIZE)
		{
			size += sizeof(TYPE) * m_nCapacity;
		}
		
		return size;
	}
	
private:
	ALLOC m_Alloc;
	TYPE m_Stack[SIZE];
	TYPE* m_pData;
	size_t m_nCapacity;
	size_t m_nSize;
};

#endif // _UTILS_ARRAY_POD_H
