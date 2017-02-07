//--------------------------------------------------------------------
// 文件名:		game_object_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_object_set.h"
#include "game_object.h"
#include "../public/inlines.h"
#include "../public/var_list.h"

// CGameObjectSet

CGameObjectSet::CGameObjectSet()
{
}

CGameObjectSet::~CGameObjectSet()
{
	// 不需自动删除对象
}

bool CGameObjectSet::FindIndex(const char* ident, size_t& index) const
{
	Assert(ident != NULL);

	size_t size = m_Objects.size();

	if (0 == size)
	{
		return false;
	}

	unsigned int hash = GetHashValueCase(ident);
	GameObject* const* objects = &m_Objects[0];

	for (size_t i = 0; i < size; ++i)
	{
		if ((objects[i]->GetHash() == hash)
			&& (strcmp(objects[i]->GetIdentQuick(), ident) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CGameObjectSet::Find(const char* ident) const
{
	size_t index;

	return FindIndex(ident, index);
}

bool CGameObjectSet::Add(const char* ident, GameObject* object)
{
	Assert(ident != NULL);
	Assert(object != NULL);
	
	object->SetIdent(ident);
	object->SetHash(GetHashValueCase(ident));
	
	m_Objects.push_back(object);
	
	return true;
}

bool CGameObjectSet::Remove(const char* ident)
{
	size_t index;

	if (!FindIndex(ident, index))
	{
		return false;
	}

	m_Objects[index]->Release();
	m_Objects.remove(index);

	return true;
}

bool CGameObjectSet::Change(const char* old_ident, const char* new_ident)
{
	size_t old_index;

	if (!FindIndex(old_ident, old_index))
	{
		return false;
	}

	size_t new_index;

	if (FindIndex(new_ident, new_index))
	{
		GameObject* pOther = m_Objects[new_index];

		pOther->SetIdent(old_ident);
		pOther->SetHash(GetHashValueCase(old_ident));
	}

	GameObject* pObj = m_Objects[old_index];

	pObj->SetIdent(new_ident);
	pObj->SetHash(GetHashValueCase(new_ident));

	return true;
}

void CGameObjectSet::Clear()
{
	for (size_t i = 0; i < m_Objects.size(); ++i)
	{
		m_Objects[i]->Release();
	}

	m_Objects.clear();
}

GameObject* CGameObjectSet::GetObjectByIdent(const char* ident) const
{
	size_t index;

	if (!FindIndex(ident, index))
	{
		return NULL;
	}

	return m_Objects[index];
}

PERSISTID CGameObjectSet::GetObjectID(const char* ident) const
{
	GameObject* pObj = GetObjectByIdent(ident);

	if (NULL == pObj)
	{
		return PERSISTID();
	}

	return pObj->GetID();
}

// 获取对像通过序号
PERSISTID CGameObjectSet::GetObjectIDByIndex(const size_t index) const
{
    Assert(index < m_Objects.size());
    return m_Objects[index]->GetID();
}

size_t CGameObjectSet::GetObjectCount() const
{
	return m_Objects.size();
}

void CGameObjectSet::GetObjectList(IVarList& result)
{
	for (size_t i = 0; i < m_Objects.size(); ++i)
	{
		result.AddObject(m_Objects[i]->GetID());
	}
}

