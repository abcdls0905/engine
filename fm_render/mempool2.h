//--------------------------------------------------------------------
// �ļ���:		MemPool.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��2��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MEMPOOL2_H
#define _MEMPOOL2_H

// �ڴ��
#include <stdio.h>


#define _BIG_MEM_

class CMemPool2
{
private:
#ifdef _BIG_MEM_
	enum { ALIGN = 32 };//�����С
	enum { ALIGN_SHIFT = 5 };//����λ������
	enum { MAX_BYTES = 2048 };//�ڴ����󻺴�Ķ�������
	enum { FREELIST_NUM = 64 };//һ����64���ȼ�����
#else
	enum { ALIGN = 4 };//�����С
	enum { ALIGN_SHIFT = 2 };//����λ������
	enum { MAX_BYTES = 64 };//�ڴ����󻺴�Ķ�������
	enum { FREELIST_NUM = 16 };//һ����16���ȼ�����
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

	// �����ڴ�
	void* Alloc(size_t size);
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size);
	// ����ڴ�����ڴ���
	size_t GetPoolSize() const;
	// ����ڴ�ؿ����ڴ���
	size_t GetFreeSize() const;
	
private:
	CMemPool2(const CMemPool2&);
	CMemPool2& operator=(const CMemPool2&);
	
	// �ͷ������ڴ��
	void FreeAll();
	// �����ڴ��
	char* ChunkAlloc(size_t size, int& nobjs);
	// ����ڴ�
	void* Refill(size_t n);

private:
	alloc_chunk_t* m_pFirstChunk;
	alloc_obj_t* m_pFreeList[FREELIST_NUM]; 
	char* m_pStartFree;
	char* m_pEndFree;
};

#endif // _MEMPOOL2_H

