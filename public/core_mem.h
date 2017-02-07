//--------------------------------------------------------------------
// 文件名:		core_mem.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORE_MEM_H
#define _PUBLIC_CORE_MEM_H

#include <new>
#include "i_core.h"

extern ICore* g_pCore;

// 跨模块安全的内存管理

namespace core_mem {
	inline void* CoreAllocDebug(size_t size, const char* file, int line)
	{
		return g_pCore->MemoryCheckAllocDebug(size, file, line);
	}

	inline void* CoreAlloc(size_t size)
	{
		return g_pCore->MemoryCheckAlloc(size);
	}

	inline void CoreFree(void* ptr, size_t size)
	{
		g_pCore->MemoryCheckFree(ptr, size);
	}

	template<typename TYPE>
	TYPE* CoreNewDebug(TYPE*, const char* file, int line)
	{
		TYPE* ptr = (TYPE*)g_pCore->MemoryCheckAllocDebug(
			sizeof(TYPE), file, line);

		new (ptr) TYPE;

		return ptr;
	}

	template<typename TYPE>
	TYPE* CoreNew(TYPE*)
	{
		TYPE* ptr = (TYPE*)g_pCore->MemoryCheckAlloc(sizeof(TYPE));

		new (ptr) TYPE;

		return ptr;
	}

	template<typename TYPE>
	void CoreDelete(TYPE* ptr)
	{
		if (ptr)
		{
			ptr->~TYPE();
		}

		g_pCore->MemoryCheckFree(ptr, sizeof(TYPE));
	}
} // namespace core_mem

// 内存分配
#ifdef _DEBUG
#define CORE_ALLOC(size) \
	core_mem::CoreAllocDebug(size, __FILE__, __LINE__)
#define CORE_FREE(ptr, size) core_mem::CoreFree(ptr, size)
#define CORE_NEW(type) \
	core_mem::CoreNewDebug((type*)0, __FILE__, __LINE__)
#define CORE_DELETE(ptr) core_mem::CoreDelete(ptr)
#else
#define CORE_ALLOC(size) core_mem::CoreAlloc(size)
#define CORE_FREE(ptr, size) core_mem::CoreFree(ptr, size)
#define CORE_NEW(type) core_mem::CoreNew((type*)0)
#define CORE_DELETE(ptr) core_mem::CoreDelete(ptr)
#endif // _DEBUG

// 用于模板容器

class TCoreAlloc
{
public:
	TCoreAlloc() {}
	~TCoreAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TCoreAlloc& src) {}
};

#endif // _PUBLIC_CORE_MEM_H
