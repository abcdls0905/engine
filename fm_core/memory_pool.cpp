//--------------------------------------------------------------------
// �ļ���:		memory_pool.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2011��2��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "memory_pool.h"
#include "../public/inlines.h"
#include <stdlib.h>
#include <string.h>

#define FREELIST_INDEX(bytes) ((bytes - size_t(1)) >> (int)ALIGN_SHIFT)
#define ROUND_UP(bytes) (((bytes) + (size_t)ALIGN - 1) & ~((size_t)ALIGN - 1)) 

CMemoryPool::CMemoryPool()
{
	m_pFirstChunk = NULL;
	m_pStartFree = NULL;
	m_pEndFree = NULL;
	memset(m_pFreeList, 0, sizeof(m_pFreeList));
}

CMemoryPool::~CMemoryPool()
{
	FreeAll();
}

void* CMemoryPool::Alloc(size_t size)
{
	if (0 == size)
	{
		return (void*)"";
	}

	if (size > (size_t)MAX_BYTES)
	{
		void* p = NEW char[size];

		if (NULL == p)
		{
			// �����ڴ�ʧ��
			printf("(CMemoryPool::Alloc)no enough memory, size:%d", size);
			abort();
			return NULL;
		}

		return p;
	}
	else
	{
		alloc_obj_t** my_free_list = m_pFreeList + FREELIST_INDEX(size);
		alloc_obj_t* result = *my_free_list;

		if (result != NULL)
		{
			*my_free_list = result->free_list_link;
			return result;
		} 
		else
		{
			return Refill(size);
		}
	}
}

void CMemoryPool::Free(void* ptr, size_t size)
{
	if ((NULL == ptr) || (0 == size))
	{
		return;
	}

	if (size > (size_t)MAX_BYTES)
	{
		delete[] (char*)ptr;
	}
	else
	{
		alloc_obj_t** my_free_list = m_pFreeList + FREELIST_INDEX(size);

		((alloc_obj_t*)ptr)->free_list_link = *my_free_list;
		*my_free_list = (alloc_obj_t*)ptr;
	}
}

void CMemoryPool::FreeAll()
{
	alloc_chunk_t* p = m_pFirstChunk;
	alloc_chunk_t* t;

	while (p)
	{
		t = p;
		p = p->chunk_link;
		delete[] (char*)t;
	}

	m_pFirstChunk = NULL;
}

size_t CMemoryPool::GetPoolSize() const
{
	size_t count = 0;
	alloc_chunk_t* p = m_pFirstChunk;

	while (p)
	{
		count += p->chunk_size;
		p = p->chunk_link;
	}

	return count;
}

size_t CMemoryPool::GetFreeSize() const
{
	size_t count = 0;
	
	for (size_t i = 0; i < FREELIST_NUM; ++i)
	{
		alloc_obj_t* p = m_pFreeList[i];

		while (p)
		{
			count += (i + 1) * ALIGN;
			p = p->free_list_link;
		}
	}

	return count;
}

char* CMemoryPool::ChunkAlloc(size_t size, int& nobjs)
{
	size_t total_bytes = size * nobjs;
	size_t bytes_left = m_pEndFree - m_pStartFree;

	if (bytes_left >= total_bytes)
	{
		char* result = m_pStartFree;
		m_pStartFree += total_bytes;
		return result;
	}
	else if (bytes_left >= size)
	{
		nobjs = (int)(bytes_left / size);
		total_bytes = size * nobjs;
		char* result = m_pStartFree;
		m_pStartFree += total_bytes;
		return result;
	} 
	else 
	{
		size_t bytes_to_get = 2 * total_bytes;

		if (bytes_left > 0)
		{
			alloc_obj_t** my_free_list = 
				m_pFreeList + FREELIST_INDEX(bytes_left);

			((alloc_obj_t*)m_pStartFree)->free_list_link = *my_free_list;
			*my_free_list = (alloc_obj_t*)m_pStartFree;
		}

		size_t mem_size = sizeof(alloc_chunk_t) + bytes_to_get;
		alloc_chunk_t* pChunk = (alloc_chunk_t*)NEW char[mem_size];

		if (NULL == pChunk)
		{
			// �����ڴ�ʧ��
			printf("(CMemoryPool::ChunkAlloc)no enough memory, size:%d", 
				mem_size);
			abort();
			return NULL;
		}

		pChunk->chunk_link = m_pFirstChunk;
		pChunk->chunk_size = bytes_to_get;
		m_pFirstChunk = pChunk;
		m_pStartFree = (char*)pChunk + sizeof(alloc_chunk_t);
		m_pEndFree = m_pStartFree + bytes_to_get;
		return ChunkAlloc(size, nobjs);
	}
}

void* CMemoryPool::Refill(size_t n)
{
	int nobjs = 20;

	n = ROUND_UP(n);

	char* chunk = ChunkAlloc(n, nobjs);

	if (1 == nobjs)
	{
		return chunk;
	}

	alloc_obj_t** my_free_list = m_pFreeList + FREELIST_INDEX(n);
	alloc_obj_t* result = (alloc_obj_t*)chunk;
	alloc_obj_t* current_obj;
	alloc_obj_t* next_obj;

	*my_free_list = next_obj = (alloc_obj_t*)(chunk + n);

	for (int i = 1; ; i++)
	{
		current_obj = next_obj;
		next_obj = (alloc_obj_t*)((char*)next_obj + n);

		if ((nobjs - 1) == i)
		{
			current_obj->free_list_link = NULL;
			break;
		}
		else
		{
			current_obj->free_list_link = next_obj;
		}
	}

	return result;
}
