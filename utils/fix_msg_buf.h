//--------------------------------------------------------------------
// �ļ���:		fix_msg_buf.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_FIX_MSG_BUF_H
#define _UTILS_FIX_MSG_BUF_H

#include "../public/macros.h"
#include <string.h>

class TFixMsgBufAlloc
{
public:
	TFixMsgBufAlloc() {}
	~TFixMsgBufAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return NEW char[size]; }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// ����
	void Swap(TFixMsgBufAlloc& src) {}
};

// �̶�������Ϣ������

template<typename TYPE, typename ALLOC = TFixMsgBufAlloc>
class TFixMsgBuf  
{
private:
	typedef TFixMsgBuf<TYPE, ALLOC> self_type;
	
public:
	explicit TFixMsgBuf(size_t size = 0)
	{
		m_nBufSize = size;
		m_nMsgIn = 0;
		m_nMsgOut = 0;
		m_nMsgCount = 0;
		m_nMsgLimit = 0;
		
		if (size > 0)
		{
			m_pBuffer = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * size);
		}
		else
		{
			m_pBuffer = NULL;
		}
	}
	
	TFixMsgBuf(const self_type& src)
	{
		m_nBufSize = src.m_nBufSize;
		m_nMsgIn = src.m_nMsgIn;
		m_nMsgOut = src.m_nMsgOut;
		m_nMsgCount = src.m_nMsgCount;
		m_nMsgLimit = src.m_nMsgLimit;
		
		if (m_nBufSize > 0)
		{
			m_pBuffer = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * m_nBufSize);
			memcpy(m_pBuffer, src.m_pBuffer, m_nBufSize * sizeof(TYPE));
		}
		else
		{
			m_pBuffer = NULL;
		}
	}
	
	~TFixMsgBuf()
	{
		if (m_pBuffer)
		{
			m_Alloc.Free(m_pBuffer, sizeof(TYPE) * m_nBufSize);
		}
	}
	
	self_type& operator=(const self_type& src)
	{
		self_type temp(src);
		
		Swap(temp);
		
		return *this;
	}

	void Swap(self_type& src)
	{
		TYPE* temp_buffer = m_pBuffer;
		size_t temp_bufsize = m_nBufSize;
		size_t temp_msgin = m_nMsgIn;
		size_t temp_msgout = m_nMsgOut;
		size_t temp_msgcount = m_nMsgCount;
		size_t temp_msglimit = m_nMsgLimit;
		
		m_pBuffer = src.m_pBuffer;
		m_nBufSize = src.m_nBufSize;
		m_nMsgIn = src.m_nMsgIn;
		m_nMsgOut = src.m_nMsgOut;
		m_nMsgCount = src.m_nMsgCount;
		m_nMsgLimit = src.m_nMsgLimit;
		src.m_pBuffer = temp_buffer;
		src.m_nBufSize = temp_bufsize;
		src.m_nMsgIn = temp_msgin;
		src.m_nMsgOut = temp_msgout;
		src.m_nMsgCount = temp_msgcount;
		src.m_nMsgLimit = temp_msglimit;
		m_Alloc.Swap(src.m_Alloc);
	}	

	// ��Ϣ��������
	size_t GetMsgLimit() const 
	{ 
		return m_nMsgLimit; 
	}
	
	void SetMsgLimit(size_t value) 
	{ 
		m_nMsgLimit = value; 
	}
	
	// �Ƿ�Ϊ��
	bool IsEmpty() const
	{ 
		return (m_nMsgIn == m_nMsgOut); 
	}
	
	// ��ǰ��Ϣ����
	size_t GetMsgCount() const
	{ 
		return m_nMsgCount; 
	}

	// ���
	void Clear()
	{
		m_nMsgIn = 0;
		m_nMsgOut = 0;
		m_nMsgCount = 0;
	}

	// ������Ϣ������
	bool Put(const TYPE& msg)
	{
		if (CheckOverflow())
		{
			return false;
		}

		if (0 == m_nBufSize)
		{
			Expand();
		}
		else if (((m_nMsgIn + 1) % m_nBufSize) == m_nMsgOut)
		{
			Expand();
		}
		
		m_pBuffer[m_nMsgIn] = msg;
		m_nMsgIn = (m_nMsgIn + 1) % m_nBufSize;
		m_nMsgCount++;
		
		return true;
	}
	
	// �Ӷ��п�����ɾ����Ϣ
	bool Get(TYPE& msg)
	{
		if (m_nMsgIn == m_nMsgOut)
		{
			return false;
		}
		
		msg = m_pBuffer[m_nMsgOut];
		m_nMsgOut = (m_nMsgOut + 1) % m_nBufSize;
		m_nMsgCount--;
		
		return true;
	}
	
	// ���һ����Ϣ
	bool Peek(TYPE& msg)
	{
		if (m_nMsgIn == m_nMsgOut)
		{
			return false;
		}
		
		msg = m_pBuffer[m_nMsgOut];
		
		return true;
	}
	
	// ɾ��һ����Ϣ
	bool Erase()
	{
		if (m_nMsgIn == m_nMsgOut)
		{
			return false;
		}
		
		m_nMsgOut = (m_nMsgOut + 1) % m_nBufSize;
		m_nMsgCount--;
		
		return true;
	}	

private:
	bool CheckOverflow() const
	{
		// ��������
		if (m_nMsgLimit > 0) 
		{
			if (m_nMsgCount >= m_nMsgLimit)
			{
				return true;
			}
		}
		
		return false;
	}

	bool Expand()
	{
		size_t new_size = m_nBufSize * 2;
	
		if (0 == new_size)
		{
			new_size = 256;
		}
		
		self_type temp(new_size);
		TYPE data;
		
		while (Get(data))
		{
			temp.Put(data);
		}
		
		Swap(temp);
		
		return true;
	}

private:
	ALLOC m_Alloc;
	TYPE* m_pBuffer;
	size_t m_nBufSize;
	size_t m_nMsgIn;
	size_t m_nMsgOut;
	size_t m_nMsgCount;
	size_t m_nMsgLimit;
};

#endif // _UTILS_FIX_MSG_BUF_H
