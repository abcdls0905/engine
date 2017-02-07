//--------------------------------------------------------------------
// 文件名:		terrain_water.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_water.h"
#include "terrain.h"
#include "sea_water.h"
#include "sea_base.h"
#include "../visual/i_vis_base.h"
#include "../public/core_mem.h"
#include "../public/var_list.h"
#include "../public/inlines.h"

// CTerrainWater

CTerrainWater::CTerrainWater()
{
	m_pTerrain = NULL;
	m_nWaterCount = 0;
}

CTerrainWater::~CTerrainWater()
{
}

void CTerrainWater::ReleaseAll()
{
}

bool CTerrainWater::Load()
{
	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		m_Waters[i]->GetSeaBase()->Load();
	}
	
	return true;
}

bool CTerrainWater::Unload()
{
	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		m_Waters[i]->GetSeaBase()->Unload();
	}
	
	return true;
}

void CTerrainWater::Realize()
{
	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		m_Waters[i]->GetSeaBase()->Realize();
	}
}

bool CTerrainWater::GetLoadFinish()
{
	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		CSeaBase* pSeaBase = m_Waters[i]->GetSeaBase();
		ITexture* pWaterTex1 = pSeaBase->GetWaterNormalMap1();
		ITexture* pWaterTex2 = pSeaBase->GetWaterNormalMap2();
		ITexture* pWaterTex3 = pSeaBase->GetWaterNormalMap3();
		ITexture* pWaterTex4 = pSeaBase->GetWaterNormalMap4();
		ITexture* pWhiteTex = pSeaBase->GetWhiteWaterMap();
		ITexture* pCausticTex = pSeaBase->GetCausticMap();

		if (pWaterTex1)
		{
			if (!pWaterTex1->IsLoadComplete())
			{
				return false;
			}
		}

		if (pWaterTex2)
		{
			if (!pWaterTex2->IsLoadComplete())
			{
				return false;
			}
		}

		if (pWaterTex3)
		{
			if (!pWaterTex3->IsLoadComplete())
			{
				return false;
			}
		}

		if (pWaterTex4)
		{
			if (!pWaterTex4->IsLoadComplete())
			{
				return false;
			}
		}

		if (pWhiteTex)
		{
			if (!pWhiteTex->IsLoadComplete())
			{
				return false;
			}
		}

		if (pCausticTex)
		{
			if (!pCausticTex->IsLoadComplete())
			{
				return false;
			}
		}
	}

	return true;
}

bool CTerrainWater::FindIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		SeaWater* pWater = m_Waters[i];

		if ((pWater->GetHash() == hash)
			&& (strcmp(pWater->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}
	
	return false;
}

bool CTerrainWater::Find(const char* name) const
{
	size_t index;

	return FindIndex(name, index);
}

size_t CTerrainWater::GetCount() const
{
	return m_nWaterCount;
}

SeaWater* CTerrainWater::GetByIndex(size_t index) const
{
	Assert(index < m_nWaterCount);

	return m_Waters[index];
}

SeaWater* CTerrainWater::Create(const char* name)
{
	Assert(name != NULL);
	
	if (m_nWaterCount == MAX_WATER_NUM)
	{
		return NULL;
	}

	CSeaBase* pSeaBase = CORE_NEW(CSeaBase);
	
	pSeaBase->SetTerrain(m_pTerrain);
	pSeaBase->SetRender(m_pTerrain->GetRender());
	
	ICore* pCore = m_pTerrain->GetCore();
	SeaWater* pWater = (SeaWater*)pCore->CreateEntityArgs("SeaWater", CVarList());

	pWater->SetName(name);
	pWater->SetHash(GetHashValueCase(name));
	pWater->SetTerrain(m_pTerrain);
	pWater->SetSeaBase(pSeaBase);
	pWater->SetIndex(m_nWaterCount);
	pWater->InitSeaData();

	m_Waters[m_nWaterCount++] = pWater;
	
	return pWater;
}

bool CTerrainWater::DeleteByIndex(size_t index)
{
	Assert(index < m_nWaterCount);

	m_Waters[index]->Release();

	memmove(m_Waters + index, m_Waters + index + 1,
		(m_nWaterCount - index - 1) * sizeof(SeaWater*));

	--m_nWaterCount;
	
	// 修正索引值
	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		m_Waters[i]->SetIndex(i);
	}
	
	return true;
}

void CTerrainWater::Clear()
{
	for (size_t i = 0; i < m_nWaterCount; ++i)
	{
		m_Waters[i]->Release();
	}
	
	m_nWaterCount = 0;
}

