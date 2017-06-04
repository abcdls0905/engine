
#ifndef _MEMPOOL2_H
#define _MEMPOOL2_H

// 内存池
#include <stdio.h>


#define _BIG_MEM_

class CMemPool2
{
private:
#ifdef _BIG_MEM_
	enum { ALIGN = 32 };//对齐大小
	enum { ALIGN_SHIFT = 5 };//对齐位操作数
	enum { MAX_BYTES = 2048 };//内存池最大缓存的对齐数据
	enum { FREELIST_NUM = 64 };//一共分64个等级对齐
#else
	enum { ALIGN = 4 };//对齐大小
	enum { ALIGN_SHIFT = 2 };//对齐位操作数
	enum { MAX_BYTES = 64 };//内存池最大缓存的对齐数据
	enum { FREELIST_NUM = 16 };//一共分16个等级对齐
#endif
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
	CMemPool2();
	~CMemPool2();

	// 申请内存
	void* Alloc(size_t size);
	// 释放内存
	void Free(void* ptr, size_t size);
	// 获得内存池总内存数
	size_t GetPoolSize() const;
	// 获得内存池可用内存数
	size_t GetFreeSize() const;
	
private:
	CMemPool2(const CMemPool2&);
	CMemPool2& operator=(const CMemPool2&);
	
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

