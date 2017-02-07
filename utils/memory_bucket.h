//--------------------------------------------------------------------
// 文件名:		memory_bucket.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年4月15日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TMemoryBucketAlloc& src) {}
};

// 内存管理，使用者只能获取，不需要释放内存
// 只返回未初始化的内存块

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

	// 获得内存
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

	// 是否为空
	bool Empty() const
	{
		return (NULL == m_pBucket);
	}

	// 获得在使用的数量
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

	// 清空，必须保证已经无任何使用者
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
	
	// 获得内存占用
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
