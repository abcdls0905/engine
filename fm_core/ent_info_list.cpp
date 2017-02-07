//--------------------------------------------------------------------
// 文件名:		ent_info_list.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "ent_info_list.h"
#include "ent_info.h"
#include "memory_pool.h"

extern CMemoryPool* g_pMemoryPool;

// CEntInfoList

CEntInfoList::CEntInfoList()
{
	m_nSize = 0;
	m_pBuckets = NULL;
}

CEntInfoList::~CEntInfoList()
{
	if (m_pBuckets)
	{
		if (g_pMemoryPool)
		{
			g_pMemoryPool->Free(m_pBuckets, sizeof(CEntInfo*) * m_nSize);
		}
		else
		{
			delete[] m_pBuckets;
		}
	}
}

void CEntInfoList::Expand()
{
	size_t new_size;

	if (0 == m_nSize)
	{
		new_size = 32;
	}
	else
	{
		new_size = m_nSize * 2;
	}

	size_t new_size_1 = new_size - 1;
	CEntInfo** new_buckets;
	
	if (g_pMemoryPool)
	{
		new_buckets = (CEntInfo**)g_pMemoryPool->Alloc(
			sizeof(CEntInfo*) * new_size);
	}
	else
	{
		new_buckets = NEW CEntInfo*[new_size];
	}
	
	memset(new_buckets, 0, sizeof(CEntInfo*) * new_size); 

	for (size_t i = 0; i < m_nSize; ++i)
	{
		CEntInfo* p = m_pBuckets[i];

		while (p)
		{
			CEntInfo* t = p->GetNext();
			size_t bucket = p->GetHash() & new_size_1;

			p->SetNext(new_buckets[bucket]);
			new_buckets[bucket] = p;
			p = t;
		}
	}

	if (m_pBuckets)
	{
		if (g_pMemoryPool)
		{
			g_pMemoryPool->Free(m_pBuckets, sizeof(CEntInfo*) * m_nSize);
		}
		else
		{
			delete[] m_pBuckets;
		}
	}

	m_pBuckets = new_buckets;
	m_nSize = new_size;
}

bool CEntInfoList::Add(CEntInfo* pEntInfo)
{
	Assert(pEntInfo != NULL);
	
	if (m_EntInfos.size() == m_nSize)
	{
		Expand();
	}
	
	size_t hash = GetHashValueCase(pEntInfo->GetEntityName());
	size_t bucket = hash & (m_nSize - 1);

	pEntInfo->SetHash(hash);
	pEntInfo->SetNext(m_pBuckets[bucket]);
	m_pBuckets[bucket] = pEntInfo;
	m_EntInfos.push_back(pEntInfo);

	return true;
}

size_t CEntInfoList::GetCount() const
{
	return m_EntInfos.size();
}

CEntInfo* CEntInfoList::GetByIndex(size_t index) const
{
	Assert(index < m_EntInfos.size());
	
	return m_EntInfos[index];
}

CEntInfo* CEntInfoList::Get(const char* name) const
{
	Assert(name != NULL);
	
	if (0 == m_nSize)
	{
		return NULL;
	}
	
	size_t hash = GetHashValueCase(name);
	size_t bucket = hash & (m_nSize - 1);
	CEntInfo* pEntInfo = m_pBuckets[bucket];

	while (pEntInfo)
	{
		if (pEntInfo->GetHash() == hash)
		{
			if (strcmp(pEntInfo->GetEntityName(), name) == 0)
			{
				return pEntInfo;
			}
		}
		
		pEntInfo = pEntInfo->GetNext();
	}

	return NULL;
}
