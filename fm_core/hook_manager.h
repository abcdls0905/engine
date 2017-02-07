//--------------------------------------------------------------------
// 文件名:		hook_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月25日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _HOOK_MANAGER_H
#define _HOOK_MANAGER_H

#include "../public/macros.h"
#include "memory_pool.h"

extern CMemoryPool* g_pMemoryPool;

class THookManagerAlloc
{
public:
	THookManagerAlloc() {}
	~THookManagerAlloc() {}

	// 申请内存
	void* Alloc(size_t size) 
	{ 
		if (g_pMemoryPool)
		{
			return g_pMemoryPool->Alloc(size); 
		}
		else
		{
			return NEW char[size];
		}
	}
	// 释放内存
	void Free(void* ptr, size_t size) 
	{ 
		if (g_pMemoryPool)
		{
			g_pMemoryPool->Free(ptr, size); 
		}
		else
		{
			delete[] (char*)ptr;
		}
	}
	// 交换
	void Swap(THookManagerAlloc& src) {}
};

// 窗口消息处理管理

class IEntity;

class CHookManager
{
private:
	enum { MAX_HOOK_NUM = 0x1000 };
	
	struct hook_t
	{
		IEntity* pEntity;
		int nPrior;
		bool bInstant;
		hook_t* pNext;
	};

public:
	CHookManager();
	~CHookManager();

	// 添加
	bool AddHook(IEntity* pEntity, unsigned int msg, int prior, bool instant);
	// 删除
	bool RemoveHook(IEntity* pEntity, unsigned int msg);
	// 删除实体相关
	void RemoveAll(IEntity* pEntity);
	// 消息处理
	bool ProcessMsg(unsigned int msg, size_t param1, size_t param2, 
		int& result);
	bool ProcessMsgInstant(unsigned int msg, size_t param1, size_t param2, 
		int& result);

private:
	CHookManager(const CHookManager&);
	CHookManager& operator=(const CHookManager&);

private:
	THookManagerAlloc m_Alloc;
	hook_t* m_Hooks[MAX_HOOK_NUM];
};

#endif // _HOOK_MANAGER_H
