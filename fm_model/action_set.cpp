//--------------------------------------------------------------------
// 文件名:		action_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_set.h"
#include "action_set_loader.h"
#include "action_data.h"
#include "skeleton.h"
#include "res_manager.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"
#include "../public/portable.h"
extern IRender* g_pRender;

// CActionSet

CActionSet* CActionSet::NewInstance()
{
	return CORE_NEW(CActionSet);
}

CActionSet::CActionSet()
{
	m_pManager = NULL;
	m_pActionSet = NULL;
	m_pLoader = NULL;
	m_nState = 0;
}

CActionSet::~CActionSet()
{
	SAFE_RELEASE(m_pLoader);

	if (m_pActionSet)
	{
		unload_action_set(m_pActionSet);
	}
}

void CActionSet::Destroy()
{
	CORE_DELETE(this);
}

void CActionSet::Release()
{
	m_pManager->ReleaseActionSet(this);
}

bool CActionSet::Create()
{
	if (!BeginCreate())
	{
		return false;
	}
	
	if (!m_pLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_ACTION_SET, false);
		return false;
	}
	
	if (!m_pLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_ACTION_SET, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_ACTION_SET, true))
	{
		return false;
	}
	
	return true;
}

bool CActionSet::CanAsync()
{
	// 可以异步加载
	return true;
}

bool CActionSet::BeginCreate()
{
	Assert(m_pLoader == NULL);
	
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		g_pRender->GetPathPrefix(GetName()), GetName());

	CActionSetLoader* loader = CActionSetLoader::NewInstance();
	
	loader->SetOwner(this);
	loader->SetFileName(fname);
	loader->SetAppendPath(m_strAppendPath.c_str());
	
	m_pLoader = loader;

	return true;
}

bool CActionSet::EndCreate(int category, bool succeed)
{
	if (category == RES_CATEGORY_ACTION_SET)
	{
		SAFE_RELEASE(m_pLoader);
	}
	
    // 预加载处理
    int action_num = array_get_size(m_pActionSet->pActionNodeList);
    for (int i = 0; i < action_num; ++i)
    {
        action_node_t* pActionNode = (action_node_t*)array_get(
            m_pActionSet->pActionNodeList, i);

        if (pActionNode->bPreload && pActionNode->pSkeleton == NULL)
        {
            pActionNode->pSkeleton = m_pManager->CreateSkeleton(
                pActionNode->pszSkeletonName, true);
        }
    }

	if (succeed)
	{
		// 设置资源就绪
		SetState(STATE_READY);
	}
	else
	{
		SetState(STATE_FAILED);
	}
	
	return true;
}

IResLoader* CActionSet::GetLoader()
{
	return m_pLoader;
}

bool CActionSet::Build(action_set_t* pActionSet, skeleton_t* pMainSkeleton)
{
	Assert(pActionSet != NULL);

	m_pActionSet = pActionSet;

	//char skeleton_name[256];

	//SafeSprintf(skeleton_name, sizeof(skeleton_name), "%s%s",  
	//	m_strAppendPath.c_str(), pActionSet->pszMainSkeletonName);

	//SafeSprintf(skeleton_name, sizeof(skeleton_name), "%s", 
	//	pActionSet->pszMainSkeletonName);

	m_pActionSet->pMainSkeleton = m_pManager->GetSkeleton(
		pActionSet->pszMainSkeletonName, pMainSkeleton);

//	// 创建动作名索引表
//	m_ActionIndex.Clear();
//
//	int action_num = array_get_size(pActionSet->pActionNodeList);
//
//	for (int i = 0; i < action_num; ++i)
//	{
//		action_node_t* pActionNode = (action_node_t*)array_get(
//			pActionSet->pActionNodeList, i);
//
//		m_ActionIndex.Add(pActionNode->pszActionName, i);
//	}

	return true;
}

int CActionSet::GetActionIndex(const char* action_name)
{
	Assert(action_name != NULL);

	unsigned int action_num = array_get_size(m_pActionSet->pActionNodeList);

	if (0 == action_num)
	{
		return -1;
	}
	
	unsigned int hash = GetHashValue(action_name);
	action_node_t* action_nodes = (action_node_t*)array_get(
		m_pActionSet->pActionNodeList, 0);

	if (m_pActionSet->pHashBuckets)
	{
		unsigned int bucket = hash % m_pActionSet->nBucketSize;
		int k = m_pActionSet->pHashBuckets[bucket];

		while (k >= 0)
		{
			if ((action_nodes[k].nActionHash == hash)
				&& (stricmp(action_nodes[k].pszActionName, action_name) == 0))
			{
				return k;
			}
			
			k = action_nodes[k].nHashNext;
		}
	}
	else
	{
		for (unsigned int k = 0; k < action_num; ++k)
		{
			if ((action_nodes[k].nActionHash == hash)
				&& (stricmp(action_nodes[k].pszActionName, action_name) == 0))
			{
				return (int)k;
			}
		}
	}
	
	return -1;
//	int index;
//
//	if (!m_ActionIndex.GetData(action_name, index))
//	{
//		return -1;
//	}
//
//	return index;
}

bool CActionSet::AddActionIndex(const char* action_name, int action_index)
{
	Assert(action_name != NULL);

	unsigned int action_num = array_get_size(m_pActionSet->pActionNodeList);

	if (0 == action_num)
	{
		return false;
	}

	if (m_pActionSet->pHashBuckets)
	{
		unsigned int hash = GetHashValue(action_name);
		unsigned int bucket = hash % m_pActionSet->nBucketSize;
		int* pBuckets = m_pActionSet->pHashBuckets;
		action_node_t* action_nodes = (action_node_t*)array_get(
			m_pActionSet->pActionNodeList, 0);

		action_nodes[action_index].nHashNext = pBuckets[bucket];
		pBuckets[bucket] = action_index;
		return true;
	}

	return false;
	//return m_ActionIndex.Add(action_name, action_index);
}

bool CActionSet::RemoveActionIndex(const char* action_name, int action_index)
{
	Assert(action_name != NULL);

	unsigned int action_num = array_get_size(m_pActionSet->pActionNodeList);

	if (0 == action_num)
	{
		return false;
	}

	if (m_pActionSet->pHashBuckets)
	{
		unsigned int hash = GetHashValue(action_name);
		unsigned int bucket = hash % m_pActionSet->nBucketSize;
		int* pBuckets = m_pActionSet->pHashBuckets;
		action_node_t* action_nodes = (action_node_t*)array_get(
			m_pActionSet->pActionNodeList, 0);
		int k = pBuckets[bucket];
		int prev = -1;

		while (k >= 0)
		{
			if (k == action_index)
			{
				if (prev < 0)
				{
					pBuckets[bucket] = action_nodes[k].nHashNext;
				}
				else
				{
					action_nodes[prev].nHashNext = action_nodes[k].nHashNext;
				}

				action_nodes[k].nHashNext = -1;
				return true;
			}
			
			prev = k;
			k = action_nodes[k].nHashNext;
		}
	}

	return false;
	//return m_ActionIndex.RemoveData(action_name, action_index);
}

bool CActionSet::RemoveActionLastUse(int action_index)
{
	Assert((unsigned int)action_index 
		< array_get_size(m_pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		m_pActionSet->pActionNodeList, action_index);

	Assert(pActionNode->pSkeleton != NULL);

	// 移出队列
	action_node_t* pPrev = pActionNode->pLastUsePrev;
	action_node_t* pNext = pActionNode->pLastUseNext;

	if (pPrev)
	{
		pPrev->pLastUseNext = pNext;
	}

	if (pNext)
	{
		pNext->pLastUsePrev = pPrev;
	}

	if (m_pActionSet->pLastUseHead == pActionNode)
	{
		m_pActionSet->pLastUseHead = pNext;
	}

	if (m_pActionSet->pLastUseTail == pActionNode)
	{
		m_pActionSet->pLastUseTail = pPrev;
	}

	pActionNode->pLastUseNext = NULL;
	pActionNode->pLastUsePrev = NULL;

	return true;
}

bool CActionSet::UpdateActionLastUse(int action_index)
{
	Assert((unsigned int)action_index 
		< array_get_size(m_pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		m_pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode->pSkeleton)
	{
		return false;
	}
	//Assert(pActionNode->pSkeleton != NULL);

	pActionNode->nLastUseTick = Port_GetTickCount();

	// 移出队列
	action_node_t* pPrev = pActionNode->pLastUsePrev;
	action_node_t* pNext = pActionNode->pLastUseNext;

	if (pPrev)
	{
		pPrev->pLastUseNext = pNext;
	}

	if (pNext)
	{
		pNext->pLastUsePrev = pPrev;
	}

	if (m_pActionSet->pLastUseHead == pActionNode)
	{
		m_pActionSet->pLastUseHead = pNext;
	}

	if (m_pActionSet->pLastUseTail == pActionNode)
	{
		m_pActionSet->pLastUseTail = pPrev;
	}

	// 添加到队列末尾
	if (NULL == m_pActionSet->pLastUseHead) 
	{
		pActionNode->pLastUseNext = NULL;
		pActionNode->pLastUsePrev = NULL;
		m_pActionSet->pLastUseHead = pActionNode;
		m_pActionSet->pLastUseTail = pActionNode;
	}
	else
	{
		m_pActionSet->pLastUseTail->pLastUseNext = pActionNode;
		pActionNode->pLastUsePrev = m_pActionSet->pLastUseTail;
		pActionNode->pLastUseNext = NULL;
		m_pActionSet->pLastUseTail = pActionNode;
	}

	return true;
}

int CActionSet::CollectUnuseAction(unsigned int cur_tick, unsigned int ms)
{
	if (NULL == m_pActionSet)
	{
		return 0;
	}

	int count = 0;
	action_node_t* pNode = m_pActionSet->pLastUseHead;

	while (pNode)
	{
		if ((cur_tick - pNode->nLastUseTick) < ms)
		{
			// 后面的节点不需要再检查了
			break;
		}
		
		if (pNode->pSkeleton)
		{
			// 释放动作数据
			pNode->pSkeleton->Release();
			pNode->pSkeleton = NULL;
		}

		action_node_t* pNext = pNode->pLastUseNext;

		m_pActionSet->pLastUseHead = pNext;

		if (pNext)
		{
			pNext->pLastUsePrev = NULL;
		}
		else
		{
			m_pActionSet->pLastUseTail = NULL;
		}

		pNode->nLastUseTick = 0;
		pNode->pLastUseNext = NULL;
		pNode->pLastUsePrev = NULL;
		pNode = pNext;
		++count;
	}

	return count;
}
