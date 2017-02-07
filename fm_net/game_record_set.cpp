//--------------------------------------------------------------------
// 文件名:		game_record_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_record_set.h"
#include "game_record.h"
#include "../public/core_mem.h"

// CGameRecordSet

CGameRecordSet::CGameRecordSet()
{
}

CGameRecordSet::~CGameRecordSet()
{
	ReleaseAll();
}

void CGameRecordSet::ReleaseAll()
{
	for (size_t i = 0; i < m_Records.size(); ++i)
	{
		CORE_DELETE(m_Records[i]);
	}

	m_Records.clear();
}

bool CGameRecordSet::Find(const char* name) const
{
	return m_Indices.Exists(name);
}

bool CGameRecordSet::FindIndex(const char* name, size_t& index) const
{
	return m_Indices.GetData(name, index);
}

bool CGameRecordSet::Add(CGameRecord* record)
{
	m_Indices.Add(record->GetName(), m_Records.size());

	m_Records.push_back(record);

	return true;
}

void CGameRecordSet::Clear()
{
	ReleaseAll();
	
	m_Indices.Clear();
}

CGameRecord* CGameRecordSet::Get(const char* name)
{
	size_t index;

	if (!m_Indices.GetData(name, index))
	{
		return NULL;
	}

	return m_Records[index];
}

size_t CGameRecordSet::GetCount() const
{
	return m_Records.size();
}

CGameRecord* CGameRecordSet::GetRecord(size_t index) const
{
	Assert(index < m_Records.size());
	
	return m_Records[index];
}

