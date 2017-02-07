//--------------------------------------------------------------------
// �ļ���:		auto_mem.h
// ��  ��:		
// ˵  ��:		
// ��������:	2002��7��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_AUTO_MEM_H
#define _PUBLIC_AUTO_MEM_H

#include "macros.h"

class TAutoMemAlloc
{
public:
	TAutoMemAlloc() {}
	~TAutoMemAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return NEW char[size]; }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// ����
	void Swap(TAutoMemAlloc& src) {}
};

// �Զ�������ڴ棬�ɸ�����Ҫʹ�þֲ�ջ���

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
