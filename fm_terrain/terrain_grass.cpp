//--------------------------------------------------------------------
// 文件名:		terrain_grass.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_grass.h"
#include "terrain.h"
#include "grass_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_texture.h"
#include "../public/var_list.h"
#include "../public/inlines.h"

// CTerrainGrass

CTerrainGrass::CTerrainGrass()
{
	m_pTerrain = NULL;
	m_nGrassCount = 0;
	m_gassrender.clear();
}

CTerrainGrass::~CTerrainGrass()
{
}

void CTerrainGrass::ReleaseAll()
{
}

bool CTerrainGrass::Load()
{
	// 延迟加载
	//for (size_t i = 0; i < m_nGrassCount; ++i)
	//{
	//	m_GrassTypes[i]->Load();
	//}
	
	return true;
}

void CTerrainGrass::Realize()
{
	//先清除所有渲染状态
	m_gassrender.clear();
	for (size_t i = 0; i < m_nGrassCount; ++i)
	{
		GrassType* p = m_GrassTypes[i];
		
		if (p->GetActive())
		{
			p->Realize(m_gassrender);
		}
	}
}

bool CTerrainGrass::GetLoadFinish()
{
	for (size_t i = 0; i < m_nGrassCount; ++i)
	{
		ITexture* pTex = m_GrassTypes[i]->GetTex();

		if (pTex)
		{
			if (!pTex->IsLoadComplete())
			{
				return false;
			}
		}
	}

	return true;
}

bool CTerrainGrass::FindIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_nGrassCount; ++i)
	{
		if ((m_GrassTypes[i]->GetHash() == hash)
			&& (strcmp(m_GrassTypes[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CTerrainGrass::Find(const char* name) const
{
	size_t index;

	return FindIndex(name, index);
}

//size_t CTerrainGrass::GetCount() const
//{
//	return m_nGrassCount;
//}

//GrassType* CTerrainGrass::GetByIndex(size_t index) const
//{
//	Assert(index < m_nGrassCount);
//
//	return m_GrassTypes[index];
//}

GrassType* CTerrainGrass::Create(const char* name)
{
	Assert(name != NULL);
	
	if (m_nGrassCount == MAX_GRASS_NUM)
	{
		return NULL;
	}
	
	ICore* pCore = m_pTerrain->GetCore();
	
	GrassType* pGrass = (GrassType*)pCore->CreateEntityArgs("GrassType", CVarList());

	pGrass->SetName(name);
	pGrass->SetHash(GetHashValueCase(name));
	pGrass->SetTerrain(m_pTerrain);
	pGrass->SetIndex(m_nGrassCount);

	m_GrassTypes[m_nGrassCount++] = pGrass;
	
	return pGrass;
}

bool CTerrainGrass::DeleteByIndex(size_t index)
{
	Assert(index < m_nGrassCount);

	m_GrassTypes[index]->Release();

	memmove(m_GrassTypes + index, m_GrassTypes + index + 1, 
		(m_nGrassCount - index - 1) * sizeof(GrassType*));

	--m_nGrassCount;
	
	// 修正索引值
	for (size_t i = 0; i < m_nGrassCount; ++i)
	{
		m_GrassTypes[i]->SetIndex(i);
	}
	
	return true;
}

void CTerrainGrass::Clear()
{
	for (size_t i = 0; i < m_nGrassCount; ++i)
	{
		m_GrassTypes[i]->Release();
	}
	
	m_nGrassCount = 0;
}

