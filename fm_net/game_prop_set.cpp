//--------------------------------------------------------------------
// 文件名:		game_prop_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_prop_set.h"

// CGamePropSet

CGamePropSet::CGamePropSet()
{
}

CGamePropSet::~CGamePropSet()
{
	Clear();
}

bool CGamePropSet::Find(const char* name) const
{
	return m_Indices.Exists(name);
}

bool CGamePropSet::FindIndex(const char* name, size_t& index) const
{
	return m_Indices.GetData(name, index);
}

bool CGamePropSet::Add(const char* name, const IVar& value)
{
	prop_data_t* data = CORE_NEW(prop_data_t);
	
	data->strName = name;
	data->Value.Assign(value);

	m_Indices.Add(name, m_Props.size());
	m_Props.push_back(data);
	
	return true;
}

void CGamePropSet::Clear()
{
	for (size_t i = 0; i < m_Props.size(); ++i)
	{
		CORE_DELETE(m_Props[i]);
	}

	m_Props.clear();
	m_Indices.Clear();
}

IVar* CGamePropSet::Get(const char* name)
{
	size_t index;

	if (!m_Indices.GetData(name, index))
	{
		return NULL;
	}

	return &(m_Props[index]->Value);
}

size_t CGamePropSet::GetCount() const
{
	return m_Props.size();
}

const char* CGamePropSet::GetName(size_t index) const
{
	Assert(index < m_Props.size());

	return m_Props[index]->strName.c_str();
}

const IVar& CGamePropSet::GetValue(size_t index) const
{
	Assert(index < m_Props.size());
	
	return m_Props[index]->Value;
}

bool CGamePropSet::SetValue(size_t index, const IVar& value)
{
	Assert(index < m_Props.size());

	m_Props[index]->Value.Assign(value);

	return true;
}

