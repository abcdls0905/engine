//--------------------------------------------------------------------
// �ļ���:		hook_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��25��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
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
	// �ͷ��ڴ�
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
	// ����
	void Swap(THookManagerAlloc& src) {}
};

// ������Ϣ�������

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

	// ���
	bool AddHook(IEntity* pEntity, unsigned int msg, int prior, bool instant);
	// ɾ��
	bool RemoveHook(IEntity* pEntity, unsigned int msg);
	// ɾ��ʵ�����
	void RemoveAll(IEntity* pEntity);
	// ��Ϣ����
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
