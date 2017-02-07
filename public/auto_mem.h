//--------------------------------------------------------------------
// 文件名:		auto_mem.h
// 内  容:		
// 说  明:		
// 创建日期:	2002年7月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_AUTO_MEM_H
#define _PUBLIC_AUTO_MEM_H

#include "macros.h"

class TAutoMemAlloc
{
public:
	TAutoMemAlloc() {}
	~TAutoMemAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TAutoMemAlloc& src) {}
};

// 自动管理的内存，可根据需要使用局部栈或堆

template<typename TYPE, size_t SIZE, typename ALLOC = TAutoMemAlloc>
class TAutoMem
{
private:
	typedef TAutoMem<TYPE, SIZE, ALLOC> self_type;

public:
	explicit TAutoMem(size_t len)
	{
		if (len > SIZE)
		{
			m_pMem = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * len);
			m_nSize = len;
		}
		else
		{
			m_pMem = m_Stack;
		}
	}

	~TAutoMem()
	{
		if (m_pMem != m_Stack)
		{
			m_Alloc.Free(m_pMem, sizeof(TYPE) * m_nSize);
		}
	}

	TYPE* GetBuffer()
	{
		return m_pMem;
	}

private:
	TAutoMem();
	TAutoMem(const self_type&);
	self_type& operator=(const self_type&);

private:
	ALLOC m_Alloc;
	TYPE m_Stack[SIZE];
	TYPE* m_pMem;
	size_t m_nSize;
};

#endif // _PUBLIC_AUTO_MEM_H
