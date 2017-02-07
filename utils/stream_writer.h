//--------------------------------------------------------------------
// �ļ���:		stream_writer.h
// ��  ��:		
// ˵  ��:		
// ����޸�:	2012��6��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_STREAM_WRITER_H
#define _UTILS_STREAM_WRITER_H

#include "../public/macros.h"

class TStreamWriterAlloc
{
public:
	TStreamWriterAlloc() {}
	~TStreamWriterAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return NEW char[size]; }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// ����
	void Swap(TStreamWriterAlloc& src) {}
};

template<typename TYPE, size_t BUFFER_SIZE, typename ALLOC = TStreamWriterAlloc>
class TStreamWriter;

// TStreamWriterIterator

template<typename TYPE, size_t BUFFER_SIZE, typename ALLOC>
class TStreamWriterIterator
{
private:
	typedef TStreamWriter<TYPE, BUFFER_SIZE, ALLOC> stream_type;

public:
	TStreamWriterIterator(stream_type* pStream, TYPE* pCurrent)
	{
		m_pStream = pStream;
		m_pCurrent = pCurrent;
	}
	
	TYPE& operator*() const
	{
		return *m_pCurrent;
	}
	
	TStreamWriterIterator& operator++()
	{
		m_pCurrent = m_pStream->Increment();
		
		return *this;
	}
	
	const TStreamWriterIterator operator++(int)
	{
		m_pCurrent = m_pStream->Increment();
		
		return TStreamWriterIterator(m_pStream, m_pCurrent - 1);
	}
	
private:
	TStreamWriterIterator();
	
private:
	stream_type* m_pStream;
	TYPE* m_pCurrent;
};

// TStreamWriter

template<typename TYPE, size_t BUFFER_SIZE, typename ALLOC>
class TStreamWriter
{
private:
	typedef TStreamWriter<TYPE, BUFFER_SIZE, ALLOC> self_type;
	
public:
	typedef TStreamWriterIterator<TYPE, BUFFER_SIZE, ALLOC> iterator;
	
public:
	TStreamWriter()
	{
		m_pData = m_Stack;
		m_nSize = BUFFER_SIZE;
		m_nCount = 0;
	}
	
	~TStreamWriter()
	{
		if (m_pData != m_Stack)
		{
			m_Alloc.Free(m_pData, sizeof(TYPE) * m_nSize);
		}
	}

	// ��ʼ������
	iterator Begin()
	{
		return iterator(this, m_pData);
	}
	
	// �������ָ��
	TYPE* GetData() const
	{
		return m_pData;
	}
	
	// ��õ�ǰ���ݳ���
	size_t GetCount() const
	{
		return m_nCount;
	}
	
	// ����
	TYPE* Increment()
	{
		++m_nCount;
		
		if (m_nCount == m_nSize)
		{
			size_t new_size = m_nSize * 2;
			TYPE* pdata = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * new_size);
			
			memcpy(pdata, m_pData, sizeof(TYPE) * m_nSize);
			
			if (m_pData != m_Stack)
			{
				m_Alloc.Free(m_pData, sizeof(TYPE) * m_nSize);
			}
			
			m_pData = pdata;
			m_nSize = new_size;
		}
		
		return m_pData + m_nCount;
	}
	
private:
	TStreamWriter(const self_type&);
	self_type& operator=(const self_type&);
	
private:
	ALLOC m_Alloc;
	TYPE m_Stack[BUFFER_SIZE];
	TYPE* m_pData;
	size_t m_nSize;
	size_t m_nCount;
};

#endif // _UTILS_STREAM_WRITER_H
