//--------------------------------------------------------------------
// 文件名:		array.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "array.h"
#include "../public/core_mem.h"

array_t* array_create(unsigned int element_size)
{
	Assert(element_size != 0);

	array_t* arr = (array_t*)CORE_ALLOC(sizeof(array_t));

	arr->nElementSize = element_size;
	arr->pHead = NULL;
	arr->nUsedCount = 0;
	arr->nTotalCount = 0;

	return arr;
}

void array_release(array_t* arr)
{
	Assert(arr != NULL);

	if (arr->pHead != NULL)
	{
		CORE_FREE(arr->pHead, arr->nElementSize * arr->nTotalCount);
	}

	CORE_FREE(arr, sizeof(array_t));
}

void array_copy(array_t* dst, array_t* src)
{
	Assert(dst != NULL);
	Assert(src != NULL);
	Assert(src->pHead != NULL);

	if (dst->pHead != NULL)
	{
		CORE_FREE(dst->pHead, dst->nElementSize * dst->nTotalCount);
	}

	dst->pHead = (unsigned char*)CORE_ALLOC(
		src->nElementSize * src->nTotalCount);
	dst->nElementSize = src->nElementSize;
	dst->nTotalCount = src->nTotalCount;
	dst->nUsedCount = src->nUsedCount;
	memcpy(dst->pHead, src->pHead, src->nElementSize * src->nTotalCount);
}

void array_destroy(array_t* arr)
{
	Assert(arr != NULL);

	if (arr->pHead != NULL)
	{
		CORE_FREE(arr->pHead, arr->nElementSize * arr->nTotalCount);
	}

	arr->nTotalCount = 0;
	arr->nUsedCount = 0;
	arr->pHead = NULL;
}

bool array_is_empty(array_t* arr)
{
	Assert(arr != NULL);

	if (arr->nUsedCount != 0)
	{
		return true;
	}

	return false;
}

unsigned int array_get_element_size(array_t* arr)
{
	Assert(arr != NULL);

	return arr->nElementSize;
}

void array_set_size(array_t* arr, unsigned int size)
{
	Assert(arr != NULL);

	if (arr->pHead != NULL)
	{
		CORE_FREE(arr->pHead, arr->nElementSize * arr->nTotalCount);
	}

	arr->pHead = (unsigned char*)CORE_ALLOC(arr->nElementSize * size);
	arr->nUsedCount = size;
	arr->nTotalCount = size;
}

void* array_get_buffer(array_t* arr)
{
	Assert(arr != NULL);

	return arr->pHead;
}

void array_clean(array_t* arr)
{
	Assert(arr != NULL);

	arr->nUsedCount = 0;
}

void array_push_back(array_t* arr, void* data)
{
	Assert(arr != NULL);
	Assert(data != NULL);

	if (arr->nUsedCount == arr->nTotalCount)
	{
		unsigned int new_size;

		if (0 == arr->nTotalCount)
		{
			new_size = 2;
		}
		else if (arr->nTotalCount < 128)
		{
			new_size = arr->nTotalCount * 2;
		}
		else
		{
			// 减少内存占用
			new_size = arr->nTotalCount + arr->nTotalCount / 2;
		}
		
		unsigned char* p = (unsigned char*)CORE_ALLOC(
			arr->nElementSize * new_size);

		memcpy(p, arr->pHead, arr->nElementSize * arr->nUsedCount);

		if (arr->pHead)
		{
			CORE_FREE(arr->pHead, arr->nElementSize * arr->nTotalCount);
		}

		arr->pHead = p;
		arr->nTotalCount = new_size;
	}

	memcpy((unsigned char*)arr->pHead + arr->nUsedCount * arr->nElementSize, 
		data, arr->nElementSize);
	arr->nUsedCount++;
}

void array_remove(array_t* arr, int index)
{
	Assert(arr != NULL);
	Assert((index >= 0) && (index < (int)arr->nUsedCount));

	memmove((unsigned char*)arr->pHead + index * arr->nElementSize,
		(unsigned char*)arr->pHead + (index + 1) * arr->nElementSize,
		(arr->nUsedCount - index - 1) * arr->nElementSize);
	arr->nUsedCount--;
}

void array_set(array_t* arr, int index, void* data)
{
	Assert(arr != NULL);
	Assert((index >= 0) && (index < (int)arr->nUsedCount));
	Assert(data != NULL);

	memcpy((unsigned char*)arr->pHead + index * arr->nElementSize, data,
		arr->nElementSize);
}
