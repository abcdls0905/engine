//--------------------------------------------------------------------
// 文件名:		memory_check.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年5月26日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MEMORY_CHECK_H
#define _MEMORY_CHECK_H

#include "../public/macros.h"
#include "../utils/lock_util.h"
#include <string.h>
#include <stdio.h>

// 内存信息检查

class CMemoryCheck
{
private:
	struct mem_node_t
	{
		void* pMemory;
		size_t nSize;
		const char* pFile;
		int nLine;
	};
	
public:
	CMemoryCheck()
	{
		m_nSize = 0;
		m_nCount = 0;
		m_pBuckets = NULL;
	}

	~CMemoryCheck()
	{
		if (m_pBuckets)
		{
			delete[] m_pBuckets;
		}
	}

	size_t GetCount() const
	{
		return m_nCount;
	}
	
	bool Add(void* ptr, size_t size, const char* file, int line)
	{
		CAutoLock auto_lock(m_Lock);

		if ((m_nCount * 2) == m_nSize)
		{
			Expand();
		}
		
		size_t hash = GetHash(ptr);
		size_t size_1 = m_nSize - 1;
			
		for (size_t i = 0; i < m_nSize; ++i)
		{
			size_t bucket = (hash + i) & size_1;
			mem_node_t* pNode = &m_pBuckets[bucket];
			
			if (NULL == pNode->pMemory)
			{
				pNode->pMemory = ptr;
				pNode->nSize = size;
				pNode->pFile = file;
				pNode->nLine = line;
				m_nCount++;
				return true;
			}
			else if (pNode->pMemory == ptr)
			{
				// 相同的数据已存在
				return false;
			}
		}
		
		Assert(0);
		
		return false;
	}
	
	bool Remove(void* ptr, size_t size)
	{
		CAutoLock auto_lock(m_Lock);

		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = GetHash(ptr);
		size_t size_1 = m_nSize - 1;
			
		for (size_t i = 0; i < m_nSize; ++i)
		{
			size_t bucket = (hash + i) & size_1;
			mem_node_t* pNode = &m_pBuckets[bucket];
			
			if ((NULL == pNode->pMemory) && (NULL == pNode->pFile))
			{
				return false;
			}

			if ((pNode->pMemory == ptr) && (pNode->nSize == size))
			{
				pNode->pMemory = NULL;
				m_nCount--;
				return true;
			}
		}
		
		return false;
	}
	
	bool Dump(const char* filename)
	{
		CAutoLock auto_lock(m_Lock);

		FILE* fp = fopen(filename, "wb");
		
		if (NULL == fp)
		{
			return false;
		}
		
		int count = 0;
		int size = 0;
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			mem_node_t* pNode = &m_pBuckets[i];
			
			if (NULL == pNode->pMemory)
			{
				continue;
			}
			
			fprintf(fp, "block pointer:%p size:%d file:%s line:%d\r\n",
				pNode->pMemory, pNode->nSize, pNode->pFile, pNode->nLine);
			count++;
			size += (int)pNode->nSize;	
		}
		
		fprintf(fp, "total %d blocks %d bytes", count, size);
		fclose(fp);
		
		return true;
	}

private:
	CMemoryCheck(const CMemoryCheck&);
	CMemoryCheck& operator=(const CMemoryCheck&);
	
	size_t GetHash(void* ptr)
	{
		return (((size_t)ptr) >> 3) * 31;
	}
	
	void Expand()
	{
		size_t new_size;
		
		if (0 == m_nSize)
		{
			new_size = 0x10000;
		}
		else
		{
			new_size = m_nSize * 2;
		}
		
		size_t new_size_1 = new_size - 1;
		mem_node_t* new_buckets = NEW mem_node_t[new_size];
		
		memset(new_buckets, 0, sizeof(mem_node_t) * new_size); 
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			mem_node_t* p = &m_pBuckets[i];
			
			if (p->pMemory)
			{
				for (size_t k = 0; k < new_size; ++k)
				{
					size_t hash = GetHash(p->pMemory);
					size_t bucket = (hash + k) & new_size_1;
					
					if (NULL == new_buckets[bucket].pMemory)
					{
						new_buckets[bucket] = *p;
						break;
					}
				}
			}
		}
		
		if (m_pBuckets)
		{
			delete[] m_pBuckets;
		}
		
		m_pBuckets = new_buckets;
		m_nSize = new_size;
	}

private:
	CLockUtil m_Lock;
	mem_node_t* m_pBuckets;
	size_t m_nSize;
	size_t m_nCount;
};

#endif // _MEMORY_CHECK_H
