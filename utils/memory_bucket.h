//--------------------------------------------------------------------
// �ļ���:		memory_bucket.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��4��15��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_MEMORY_BUCKET_H
#define _UTILS_MEMORY_BUCKET_H

#include "../public/macros.h"
#include <stdlib.h>

class TMemoryBucketAlloc
{
public:
	TMemoryBucketAlloc() {}
	~TMemoryBucketAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return NEW char[size]; }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// ����
	void Swap(TMemoryBucketAlloc& src) {}
};

// �ڴ����ʹ����ֻ�ܻ�ȡ������Ҫ�ͷ��ڴ�
// ֻ����δ��ʼ�����ڴ��

template<typename TYPE, size_t BUCKET_SIZE = 256, 
	typename ALLOC = TMemoryBucketAlloc>
class TMemoryBucket
{
private:
	typedef TMemoryBucket<TYPE, BUCKET_SIZE, ALLOC> self_type;
	
	struct bucket_t
	{
		TYPE* pData;
		size_t nCount;
		bucket_t* pNext;
	};
	
public:
	TMemoryBucket()
	{
		m_pBucket = NULL;
	}

	~TMemoryBucket()
	{
		Clear();
	}

	// ����ڴ�
	TYPE* New()
	{
		if (m_pBucket && (m_pBucket->nCount < BUCKET_SIZE))
		{
			return m_pBucket->pData + m_pBucket->nCount++;
		}

		size_t data_size = BUCKET_SIZE * sizeof(TYPE);
		bucket_t* p = (bucket_t*)m_Alloc.Alloc(sizeof(bucket_t) + data_size);
		
		p->pData = (TYPE*)((char*)p + sizeof(bucket_t));
		p->nCount = 1;
		p->pNext = m_pBucket;
		
		m_pBucket = p;
		
		return p->pData;
	}

	// �Ƿ�Ϊ��
	bool Empty() const
	{
		return (NULL == m_pBucket);
	}

	// �����ʹ�õ�����
	size_t GetCount() const
	{
		size_t count = 0;
		bucket_t* p = m_pBucket;

		while (p)
		{
			count += p->nCount;
			p = p->pNext;
		}

		return count;
	}

	// ��գ����뱣֤�Ѿ����κ�ʹ����
	void Clear()
	{
		bucket_t* p = m_pBucket;
		
		while (p)
		{
			bucket_t* t = p;
			
			p = p->pNext;
			m_Alloc.Free(t, sizeof(bucket_t) + BUCKET_SIZE * sizeof(TYPE));
		}
		
		m_pBucket = NULL;
	}
	
	// ����ڴ�ռ��
	size_t GetMemoryUsage() const
	{
		size_t size = sizeof(self_type);
		bucket_t* p = m_pBucket;
		
		while (p)
		{
			size += sizeof(bucket_t) + sizeof(TYPE) * BUCKET_SIZE;
			p = p->pNext;
		}
		
		return size;
	}
	
private:
	TMemoryBucket(const self_type&);
	self_type& operator=(const self_type&);

private:
	ALLOC m_Alloc;
	bucket_t* m_pBucket;
};

#endif // _UTILS_MEMORY_BUCKET_H
