//--------------------------------------------------------------------
// 文件名:		array.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ARRAY_H
#define _ARRAY_H

#include "../public/macros.h"

struct array_t
{
	void* pHead;
	unsigned int nElementSize;
	unsigned int nTotalCount;
	unsigned int nUsedCount;
};

array_t* array_create(unsigned int element_size);
void array_release(array_t* arr);
void array_copy(array_t* dst, array_t* src);
void array_destroy(array_t* arr);
bool array_is_empty(array_t* arr);
unsigned int array_get_element_size(array_t* arr);
void array_set_size(array_t* arr, unsigned int size);
void* array_get_buffer(array_t* arr);
void array_clean(array_t* arr);
void array_push_back(array_t* arr, void* data);
void array_remove(array_t* arr, int index);
void array_set(array_t* arr, int index, void* data);

inline unsigned int array_get_size(array_t* arr)
{
	Assert(arr != NULL);
	
	return arr->nUsedCount;
}

inline void* array_get(array_t* arr, int index)
{
	Assert(arr != NULL);
	Assert((index >= 0) && (index < (int)arr->nUsedCount));
	
	return (unsigned char*)arr->pHead + index * arr->nElementSize;
}

#endif // _ARRAY_H
