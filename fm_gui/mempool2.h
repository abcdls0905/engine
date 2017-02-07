//--------------------------------------------------------------------
// 文件名:		MemPool.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年2月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MEMPOOL2_H
#define _MEMPOOL2_H

#include <stddef.h>

// 内存池

class CMemPool
{
private:
	enum { ALIGN = 8 };
	enum { ALIGN_SHIFT = 3 };
	enum { MAX_BYTES = 128 };
	enum { FREELIST_NUM = 16 };

	struct alloc_obj_t
	{
		alloc_obj_t* free_list_link;
	};

	struct alloc_chunk_t
	{
		alloc_chunk_t* chunk_link;
		size_t chunk_size;
	};

public:
	CMemPool();
	~CMemPool();

	// 申请内存
	void* Alloc(size_t size);
	// 释放内存
	void Free(void* ptr, size_t size);
	// 获得内存池总内存数
	size_t GetPoolSize() const;
	// 获得内存池可用内存数
	size_t GetFreeSize() const;
	
private:
	CMemPool(const CMemPool&);
	CMemPool& operator=(const CMemPool&);
	
	// 释放所有内存块
	void FreeAll();
	// 申请内存块
	char* ChunkAlloc(size_t size, int& nobjs);
	// 获得内存
	void* Refill(size_t n);

private:
	alloc_chunk_t* m_pFirstChunk;
	alloc_obj_t* m_pFreeList[FREELIST_NUM]; 
	char* m_pStartFree;
	char* m_pEndFree;
};

#endif // _MEMPOOL2_H

