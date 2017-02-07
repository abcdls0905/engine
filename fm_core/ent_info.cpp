//--------------------------------------------------------------------
// 文件名:		ent_info.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include <new>
#include "ent_info.h"
#include "memory_pool.h"
#include "../public/i_entity.h"

extern CMemoryPool* g_pMemoryPool;

// CEntInfo

CEntInfo::CEntInfo()
{
	m_pNext = NULL;
	m_nHash = 0;
	m_nClassID = 0;
	m_pCreator = NULL;
	m_pParent = NULL;
	m_pPropInfos = NULL;
	m_nPropertyCount = 0;
}

CEntInfo::~CEntInfo()
{
	if (m_pPropInfos)
	{
		if (g_pMemoryPool)
		{
			for (size_t i = 0; i < m_nPropertyCount; ++i)
			{
				m_pPropInfos[i].~CPropInfo();
			}

			g_pMemoryPool->Free(m_pPropInfos, 
				sizeof(CPropInfo) * m_nPropertyCount);
		}
		else
		{
			delete[] m_pPropInfos;
		}
	}
}

IEntCreator* CEntInfo::GetCreator() const
{
	return m_pCreator;
}

const char* CEntInfo::GetParentName() const
{
	return m_strParentName.c_str();
}

const char* CEntInfo::GetEntityName() const
{
	return m_strEntityName.c_str();
}

int CEntInfo::GetEntityClassID() const
{
	return m_nClassID;
}

const IEntInfo* CEntInfo::GetParent() const
{
	return m_pParent;
}

bool CEntInfo::IsKindOf(const char* name) const
{
	Assert(name != NULL);

	if (strcmp(m_strEntityName.c_str(), name) == 0)
	{
		return true;
	}

	if (NULL == m_pParent)
	{
		return false;
	}
	else
	{
		return m_pParent->IsKindOf(name);
	}
}

size_t CEntInfo::GetPropertyCount() const
{
	return m_nPropertyCount;
}

void CEntInfo::InnerGetPropertyList(IVarList& result) const
{
	for (size_t i = 0; i < m_nPropertyCount; ++i)
	{
		result.AddString(m_pPropInfos[i].GetName());
	}
}

size_t CEntInfo::GetPropertyList(IVarList& result) const
{
	result.Clear();

	InnerGetPropertyList(result);

	return result.GetCount();
}

const IPropInfo* CEntInfo::GetPropertyInfo(const char* name) const
{
	size_t index;

	if (!m_PropIndices.GetData(name, index))
	{
		return NULL;
	}

	return &m_pPropInfos[index];
}

const IPropInfo* CEntInfo::FindPropertyInfo(const char* name) const
{
	const IPropInfo* pPropInfo = GetPropertyInfo(name);

	if (NULL == pPropInfo)
	{
		const IEntInfo* p = GetParent();
		
		while (p)
		{
			pPropInfo = p->GetPropertyInfo(name);

			if (pPropInfo != NULL)
			{
				break;
			}
			
			p = p->GetParent();
		}
	}

	return pPropInfo;
}

size_t CEntInfo::GetPropertyAll(IVarList& result) const
{
	result.Clear();
	
	InnerGetPropertyList(result);

	CEntInfo* p = (CEntInfo*)GetParent();

	while (p)
	{
		p->InnerGetPropertyList(result);
		
		p = (CEntInfo*)p->GetParent();
	}

	return result.GetCount();
}

void CEntInfo::SetCreator(IEntCreator* value)
{
	Assert(value != NULL);
	
	m_pCreator = value;
}

void CEntInfo::SetParentName(const char* value)
{
	Assert(value != NULL);

	m_strParentName = value;
}

void CEntInfo::SetEntityName(const char* value)
{
	Assert(value != NULL);

	m_strEntityName = value;
}

void CEntInfo::SetEntityClassID(int value)
{
	m_nClassID = value;
}

void CEntInfo::SetParent(IEntInfo* value)
{
	m_pParent = value;
}

size_t CEntInfo::AddPropertyLink(IEntProperty* pPropertyLink)
{
	if (NULL == pPropertyLink)
	{
		return 0;
	}

	// 获得总数
	size_t count = 0;
	IEntProperty* p = pPropertyLink;

	while (p)
	{
		++count;
		p = p->m_pNext;
	}

	Assert(NULL == m_pPropInfos);

	if (g_pMemoryPool)
	{
		m_pPropInfos = (CPropInfo*)g_pMemoryPool->Alloc(
			sizeof(CPropInfo) * count);

		for (size_t i = 0; i < count; ++i)
		{
			new (m_pPropInfos + i) CPropInfo;
		}
	}
	else
	{
		m_pPropInfos = NEW CPropInfo[count];
	}

	m_nPropertyCount = count;

	// 反转顺序
	size_t index = count - 1;

	p = pPropertyLink;

	while (p)
	{
		CPropInfo& data = m_pPropInfos[index];

		data.SetName(p->m_strName);
		data.SetType(p->m_nType);
		data.SetGetFunc(p->m_pGetFunc);
		data.SetSetFunc(p->m_pSetFunc);

		m_PropIndices.Add(data.GetName(), index);
		index--;
		p = p->m_pNext;
	}
		
	return count;
}
