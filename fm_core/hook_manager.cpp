//--------------------------------------------------------------------
// 文件名:		hook_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月25日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "hook_manager.h"
#include "../public/i_entity.h"

// CHookManager

CHookManager::CHookManager()
{
	memset(m_Hooks, 0, sizeof(m_Hooks));
}

CHookManager::~CHookManager()
{
	for (size_t i = 0; i < MAX_HOOK_NUM; ++i)
	{
		hook_t* p = m_Hooks[i];

		while (p)
		{
			hook_t* t = p;
			p = p->pNext;
			m_Alloc.Free(t, sizeof(hook_t));
		}

		m_Hooks[i] = NULL;
	}
}

bool CHookManager::AddHook(IEntity* pEntity, unsigned int msg, int prior, 
	bool instant)
{
	Assert(pEntity != NULL);

	if (msg >= MAX_HOOK_NUM)
	{
		return false;
	}
	
	hook_t* new_hook = (hook_t*)m_Alloc.Alloc(sizeof(hook_t));

	new_hook->pEntity = pEntity;
	new_hook->nPrior = prior;
	new_hook->bInstant = instant;

	// 获得优先级更高的节点
	hook_t* p = m_Hooks[msg];
	hook_t* t = NULL;

	while (p)
	{
		// 相同级别先加入的在前面
		if (p->nPrior >= prior)
		{
			t = p;
		}
		
		p = p->pNext;
	}

	if (t == NULL)
	{
		// 排在最前
		new_hook->pNext = m_Hooks[msg];
		m_Hooks[msg] = new_hook;
	}
	else
	{
		// 排在其后
		new_hook->pNext = t->pNext;
		t->pNext = new_hook;
	}
	
	return true;
}

bool CHookManager::RemoveHook(IEntity* pEntity, unsigned int msg)
{
	Assert(pEntity != NULL);
	
	if (msg >= MAX_HOOK_NUM)
	{
		return false;
	}

	hook_t* p = m_Hooks[msg];
	hook_t* t = NULL;

	while (p)
	{
		if (p->pEntity == pEntity) 
		{
			if (NULL == t)
			{
				m_Hooks[msg] = p->pNext;
			}
			else
			{
				t->pNext = p->pNext;
			}

			m_Alloc.Free(p, sizeof(hook_t));
			return true;
		}

		t = p;
		p = p->pNext;
	}
	
	return false;
}

void CHookManager::RemoveAll(IEntity* pEntity)
{
	Assert(pEntity != NULL);

	for (unsigned int i = 0; i < MAX_HOOK_NUM; ++i)
	{
		if (NULL == m_Hooks[i])
		{
			continue;
		}

		RemoveHook(pEntity, i);
	}
}

bool CHookManager::ProcessMsg(unsigned int msg, size_t param1, size_t param2, 
	int& result)
{
	if (msg >= MAX_HOOK_NUM)
	{
		return false;
	}

	hook_t* p = m_Hooks[msg];

	while (p)
	{
		if (!p->bInstant)
		{
			if (p->pEntity->MsgProc(msg, param1, param2, result))
			{
				return true;
			}
		}
		
		p = p->pNext;
	}

	return false;
}

bool CHookManager::ProcessMsgInstant(unsigned int msg, size_t param1, 
	size_t param2, int& result)
{
	if (msg >= MAX_HOOK_NUM)
	{
		return false;
	}

	hook_t* p = m_Hooks[msg];

	while (p)
	{
		if (p->bInstant)
		{
			if (p->pEntity->MsgProc(msg, param1, param2, result))
			{
				return true;
			}
		}
		
		p = p->pNext;
	}

	return false;
}
