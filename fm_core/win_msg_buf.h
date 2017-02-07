//--------------------------------------------------------------------
// 文件名:		win_msg_buf.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年7月17日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WIN_MSG_BUF_H
#define _WIN_MSG_BUF_H

#include "../public/macros.h"
#include "memory_pool.h"

extern CMemoryPool* g_pMemoryPool;

class TWinMsgBufAlloc
{
public:
	TWinMsgBufAlloc() {}
	~TWinMsgBufAlloc() {}

	// 申请内存
	void* Alloc(size_t size) 
	{ 
		if (g_pMemoryPool)
		{
			return g_pMemoryPool->Alloc(size); 
		}
		else
		{
			return NEW char[size];
		}
	}
	// 释放内存
	void Free(void* ptr, size_t size) 
	{ 
		if (g_pMemoryPool)
		{
			g_pMemoryPool->Free(ptr, size); 
		}
		else
		{
			delete[] (char*)ptr;
		}
	}
	// 交换
	void Swap(TWinMsgBufAlloc& src) {}
};

// 窗口消息缓冲区

class CWinMsgBuf
{
public:
	struct msg_data_t
	{
		unsigned int nMsg;
		size_t nParam1;
		size_t nParam2;
	};

public:
	CWinMsgBuf()
	{
		m_nCapacity = 256;
		m_nPutPointer = 0;
		m_nGetPointer = 0;
		m_pMsgData = (msg_data_t*)m_Alloc.Alloc(
			sizeof(msg_data_t) * m_nCapacity);
	}
	
	~CWinMsgBuf()
	{
		m_Alloc.Free(m_pMsgData, sizeof(msg_data_t) * m_nCapacity);
	}
	
	// 是否为空
	bool Empty() const
	{
		return m_nPutPointer == m_nGetPointer;
	}

	// 添加消息
	bool Put(unsigned int msg, size_t param1, size_t param2)
	{
		size_t new_pointer = (m_nPutPointer + 1) & (m_nCapacity - 1);
		
		if (new_pointer == m_nGetPointer)
		{
			// 需要扩大缓冲区
			size_t new_size = m_nCapacity * 2;
			msg_data_t* new_data = (msg_data_t*)m_Alloc.Alloc(
				sizeof(msg_data_t) * new_size);
			
			if (m_nPutPointer >= m_nGetPointer)
			{
				size_t msg_num = m_nPutPointer - m_nGetPointer;
				
				memcpy(new_data, m_pMsgData + m_nGetPointer, 
					msg_num * sizeof(msg_data_t));
				m_nPutPointer = msg_num;
				m_nGetPointer = 0;
			}
			else
			{
				size_t num1 = m_nCapacity - m_nGetPointer;
				size_t num2 = m_nPutPointer;
				
				memcpy(new_data, m_pMsgData + m_nGetPointer,
					num1 * sizeof(msg_data_t));
				memcpy(new_data + num1, m_pMsgData, 
					num2 * sizeof(msg_data_t));
				m_nPutPointer = num1 + num2;
				m_nGetPointer = 0;
			}	

			m_Alloc.Free(m_pMsgData, sizeof(msg_data_t) * m_nCapacity);
			m_pMsgData = new_data;
			m_nCapacity = new_size;
			new_pointer = m_nPutPointer + 1;
		}
		
		msg_data_t* p = m_pMsgData + m_nPutPointer;
		
		p->nMsg = msg;
		p->nParam1 = param1;
		p->nParam2 = param2;
		
		m_nPutPointer = new_pointer;
		
		return true;
	}

	// 获取消息
	msg_data_t* Get()
	{
		if (m_nGetPointer == m_nPutPointer)
		{
			return NULL;
		}
		
		msg_data_t* p = m_pMsgData + m_nGetPointer;
		
		m_nGetPointer = (m_nGetPointer + 1) & (m_nCapacity - 1);
		
		return p;
	}

private:
	CWinMsgBuf(const CWinMsgBuf&);
	CWinMsgBuf& operator=(const CWinMsgBuf&);

private:
	TWinMsgBufAlloc m_Alloc;
	size_t m_nCapacity;
	size_t m_nPutPointer;
	size_t m_nGetPointer;
	msg_data_t* m_pMsgData;
};

#endif // _WIN_MSG_BUF_H
