//--------------------------------------------------------------------
// 文件名:		sock_buf.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月24日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "sock_buf.h"
#include "../public/core_mem.h"
#include <string.h>

// CSockBuf

CSockBuf::CSockBuf(size_t data_num, size_t buf_len)
{
	m_nIn = 0;
	m_nOut = 0;
	m_nGet = 0;
	m_nPut = 0;
	m_nCount = 0;
	m_nMsgSize = 0;
	m_nDataNum = data_num;
	m_nBufLen = buf_len;
	m_nMaxDataNum = MAX_MESSAGEDATA_NUM;
	m_nMaxBufLen = MAX_MESSAGEBUF_LEN;
	m_pBuffer = (char*)CORE_ALLOC(m_nBufLen);
	m_pData = (msg_data_t*)CORE_ALLOC(sizeof(msg_data_t) * m_nDataNum);
	memset(m_pData, 0, sizeof(msg_data_t) * m_nDataNum);
}

CSockBuf::~CSockBuf()
{
	CORE_FREE(m_pBuffer, m_nBufLen);
	CORE_FREE(m_pData, sizeof(msg_data_t) * m_nDataNum);
}

size_t CSockBuf::GetDataNum() const
{
	return m_nDataNum;
}

size_t CSockBuf::GetBufLen() const
{
	return m_nBufLen;
}

void CSockBuf::SetMaxDataNum(size_t value)
{
	Assert(value > 0);
	
	m_nMaxDataNum = value;
}

size_t CSockBuf::GetMaxDataNum() const
{
	return m_nMaxDataNum;
}

void CSockBuf::SetMaxBufLen(size_t value)
{
	Assert(value > 0);
	
	m_nMaxBufLen = value;
}

size_t CSockBuf::GetMaxBufLen() const
{
	return m_nMaxBufLen;
}

bool CSockBuf::IsFull() const	
{ 
	return (m_nGet == ((m_nPut + 1) % m_nDataNum)); 
}

size_t CSockBuf::GetCount() const 
{ 
	return m_nCount; 
}

size_t CSockBuf::GetMsgSize() const
{
	return m_nMsgSize;
}

bool CSockBuf::Require(size_t len, size_t& in, size_t OUTPTR)
{
	Assert(len >= 0);

	if (len >= m_nBufLen)
	{
		return false;
	}
	
	if (in >= OUTPTR)
	{
		if ((in + len) >= m_nBufLen)
		{
			if (0 == OUTPTR)
			{
				return false;
			}
			in = 0;
			return Require(len, in, OUTPTR);
		}
	}
	else
	{
		if ((in + len) >= OUTPTR)
		{
			return false;
		}
	}

	return true;
}

bool CSockBuf::AddDataNum()
{
	if ((m_nDataNum * 2) > m_nMaxDataNum)
	{
		return false;
	}

	const size_t SIZE = m_nDataNum * 2;
	msg_data_t* pData = (msg_data_t*)CORE_ALLOC(sizeof(msg_data_t) * SIZE);

	memset(pData, 0, sizeof(msg_data_t) * SIZE);
	
	size_t k = m_nGet;
	size_t count = 0;

	while (k != m_nPut)
	{
		pData[count++] = m_pData[k];

		k = (k + 1) % m_nDataNum;
	}
	
	CORE_FREE(m_pData, sizeof(msg_data_t) * m_nDataNum);
	m_pData = pData;
	m_nDataNum = SIZE;
	m_nGet = 0;
	m_nPut = count;

	return true;
}

bool CSockBuf::AddBufLen()
{
	if ((m_nBufLen * 2) > m_nMaxBufLen)
	{
		return false;
	}
	
	const size_t SIZE = m_nBufLen * 2;
	char* buffer = (char*)CORE_ALLOC(SIZE);
	size_t k = m_nGet;
	char* p = buffer;

	while (k != m_nPut)
	{
		memcpy(p, m_pData[k].pData, m_pData[k].nLen);
		m_pData[k].pData = p;
		p += m_pData[k].nLen;
		k = (k + 1) % m_nDataNum;
	}
	
	CORE_FREE(m_pBuffer, m_nBufLen);
	m_pBuffer = buffer;
	m_nBufLen = SIZE;
	m_nIn = (p - buffer);
	m_nOut = 0;

	return true;
}

void CSockBuf::Clear()
{
	m_nIn = 0;
	m_nOut = 0;
	m_nGet = 0;
	m_nPut = 0;
	m_nCount = 0;
	m_nMsgSize = 0;
}

bool CSockBuf::Put(int src,const void* pdata, size_t len)
{
	size_t tail = m_nPut;
	
	if (m_nGet == ((tail + 1) % m_nDataNum))
	{
		if (!AddDataNum())
		{
			return false;
		}

		tail = m_nPut;
	}

	size_t in = m_nIn;
	
	while (!Require(len, in, m_nOut))
	{
		if (!AddBufLen())
		{
			return false;
		}

		in = m_nIn;
	}
	
	m_pData[tail].pData = m_pBuffer + in;
	m_pData[tail].nLen = len;
	m_pData[tail].nSrc = src;

	if (len > 0)
	{
		Assert(pdata != NULL);
		
		memcpy(m_pData[tail].pData, pdata, len);
	}

	m_nIn = in + len;
	m_nPut = (tail + 1) % m_nDataNum;
	m_nCount++;
	m_nMsgSize += len;
	
	return true;
}

bool CSockBuf::Put2(int src,const void* pdata1, size_t len1, const void* pdata2, 
	size_t len2)
{
	const size_t len = len1 + len2;
	
	size_t tail = m_nPut;
	
	if (m_nGet == ((tail + 1) % m_nDataNum))
	{
		if (!AddDataNum())
		{
			return false;
		}

		tail = m_nPut;
	}

	size_t in = m_nIn;
	
	while (!Require(len, in, m_nOut))
	{
		if (!AddBufLen())
		{
			return false;
		}

		in = m_nIn;
	}
	
	m_pData[tail].pData = m_pBuffer + in;
	m_pData[tail].nLen = len;
	m_pData[tail].nSrc = src;

	if (len1 > 0)
	{
		Assert(pdata1 != NULL);
		
		memcpy(m_pData[tail].pData, pdata1, len1);
	}

	if (len2 > 0)
	{
		Assert(pdata2 != NULL);
		
		memcpy(m_pData[tail].pData + len1, pdata2, len2);
	}

	m_nIn = in + len;
	m_nPut = (tail + 1) % m_nDataNum;
	m_nCount++;
	m_nMsgSize += len;

	return true;
}

bool CSockBuf::Put3(int src,const void* pdata1, size_t len1, const void* pdata2, 
	size_t len2, const void* pdata3, size_t len3)
{
	const size_t len = len1 + len2 + len3;
	
	size_t tail = m_nPut;
	
	if (m_nGet == ((tail + 1) % m_nDataNum))
	{
		if (!AddDataNum())
		{
			return false;
		}

		tail = m_nPut;
	}

	size_t in = m_nIn;
	
	while (!Require(len, in, m_nOut))
	{
		if (!AddBufLen())
		{
			return false;
		}

		in = m_nIn;
	}
	
	m_pData[tail].pData = m_pBuffer + in;
	m_pData[tail].nLen = len;
	m_pData[tail].nSrc = src;

	if (len1 > 0)
	{
		Assert(pdata1 != NULL);
		
		memcpy(m_pData[tail].pData, pdata1, len1);
	}

	if (len2 > 0)
	{
		Assert(pdata2 != NULL);
		
		memcpy(m_pData[tail].pData + len1, pdata2, len2);
	}

	if (len3 > 0)
	{
		Assert(pdata3 != NULL);
		
		memcpy(m_pData[tail].pData + len1 + len2, pdata3, len3);
	}

	m_nIn = in + len;
	m_nPut = (tail + 1) % m_nDataNum;
	m_nCount++;
	m_nMsgSize += len;

	return true;
}

bool CSockBuf::Peek(int &src,void*& pdata, size_t& len)
{
	Assert((m_nPut < m_nDataNum) && (m_nGet < m_nDataNum));
	
	if (m_nGet == m_nPut)
	{
		return false;
	}
	
	pdata = m_pData[m_nGet].pData;
	len = m_pData[m_nGet].nLen;
	src = m_pData[m_nGet].nSrc;

	return true;
}

void CSockBuf::Erase()
{
	if (m_nGet == m_nPut)
	{
		return;
	}

	Assert(m_pData[m_nGet].pData != NULL);
	
	m_nOut = (size_t)m_pData[m_nGet].pData - (size_t)m_pBuffer 
		+ m_pData[m_nGet].nLen;
	m_nMsgSize -= m_pData[m_nGet].nLen;
	m_nGet = (m_nGet + 1) % m_nDataNum;
	m_nCount--;
}

bool CSockBuf::ShrinkDataNum(size_t size)
{
	Assert(size > 0);

	if (m_nDataNum <= size)
	{
		return false;
	}
	
	Assert(IsEmpty());

	msg_data_t* p = (msg_data_t*)CORE_ALLOC(sizeof(msg_data_t) * size);

	memset(p, 0, sizeof(msg_data_t) * size);
	CORE_FREE(m_pData, sizeof(msg_data_t) * m_nDataNum);
	m_pData = p;
	m_nDataNum = size;
	m_nGet = 0;
	m_nPut = 0;
	
	return true;
}

bool CSockBuf::ShrinkBufLen(size_t size)
{
	Assert(size > 0);
	
	if (m_nBufLen <= size)
	{
		return false;
	}

	Assert(IsEmpty());

	char* p = (char*)CORE_ALLOC(size);

	CORE_FREE(m_pBuffer, m_nBufLen);
	m_pBuffer = p;
	m_nBufLen = size;
	m_nIn = 0;
	m_nOut = 0;
	
	return true;
}

