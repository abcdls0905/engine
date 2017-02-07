//--------------------------------------------------------------------
// 文件名:		terrain_region.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_region.h"
#include "terrain.h"
#include "walk_bit_map.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

// CTerrainRegion

CTerrainRegion::CTerrainRegion(Terrain* pTerrain, CTerrainZone* pZone,
	size_t rows, size_t cols)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);

	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_nRows = rows;
	m_nCols = cols;
	
//	for (size_t i = 0; i < MAX_REGION_NUM; ++i)
//	{
//		m_pRegions[i] = NULL;
//	}
}

CTerrainRegion::~CTerrainRegion()
{
	ReleaseAll();
}

void CTerrainRegion::ReleaseAll()
{
//	for (size_t i = 0; i < MAX_REGION_NUM; ++i)
//	{
//		if (m_pRegions[i])
//		{
//			delete m_pRegions[i];
//			m_pRegions[i] = NULL;
//		}
//	}
	for (size_t i = 0; i < m_Regions.size(); ++i)
	{
		char* pName = m_Regions[i].pName;
		
		CORE_FREE(pName, strlen(pName) + 1);
		
		if (m_Regions[i].pMap)
		{
			CORE_DELETE(m_Regions[i].pMap);
		}
	}

	m_Regions.clear();

	for (size_t k = 0; k < m_Areas.size(); ++k)
	{
		char* pName = m_Areas[k].pName;

		CORE_FREE(pName, strlen(pName) + 1);

		if (m_Areas[k].pMap)
		{
			CORE_DELETE(m_Areas[k].pMap);
		}
	}

	m_Areas.clear();
}

/*
bool CTerrainRegion::SetRegionEnable(int region, size_t row, size_t col, 
									 bool value)
{
	Assert(size_t(region) < MAX_REGION_NUM);
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	
	if (NULL == m_pRegions[region])
	{
		m_pRegions[region] = NEW CWalkBitMap;
	}	
	
	if (NULL == m_pRegions[region]->GetData())
	{
		m_pRegions[region]->Create(m_nRows, m_nCols, NULL);
	}
	
	return m_pRegions[region]->SetValue(row, col, unsigned int(value));
}

bool CTerrainRegion::GetRegionEnable(int region, size_t row, size_t col)
{
	Assert(size_t(region) < MAX_REGION_NUM);
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	
	if (NULL == m_pRegions[region])
	{
		return false;
	}
	
	unsigned int value = m_pRegions[region]->GetValue(row, col);
	
	return (value != 0);
}

bool CTerrainRegion::Build(int region, size_t rows, size_t cols, 
						   const unsigned char* pdata, size_t size)
{
	Assert(size_t(region) < MAX_REGION_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	if (m_pRegions[region])
	{
		delete m_pRegions[region];
	}

	m_pRegions[region] = NEW CWalkBitMap;

	return m_pRegions[region]->Create(rows, cols, pdata);
}

bool CTerrainRegion::BuildCompress(int region, size_t rows, size_t cols, 
						   		   unsigned char* pdata, size_t size)
{
	Assert(size_t(region) < MAX_REGION_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	if (m_pRegions[region])
	{
		delete m_pRegions[region];
	}

	m_pRegions[region] = NEW CWalkBitMap;

	return m_pRegions[region]->BuildCompress(rows, cols, pdata, size);
}
*/

bool CTerrainRegion::FindRegionIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);
	size_t region_num = m_Regions.size();

	for (size_t i = 0; i < region_num; ++i)
	{
		if ((m_Regions[i].nHash == hash) 
			&& (strcmp(m_Regions[i].pName, name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

size_t CTerrainRegion::GetRegionCount() const
{
	return m_Regions.size();
}

const char* CTerrainRegion::GetRegionName(size_t index) const
{
	Assert(index < m_Regions.size());

	return m_Regions[index].pName;
}

CWalkBitMap* CTerrainRegion::GetRegionMap(size_t index) const
{
	Assert(index < m_Regions.size());

	return m_Regions[index].pMap;
}

bool CTerrainRegion::GetRegionValue(size_t index, size_t row, size_t col)
{
	Assert(index < m_Regions.size());
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	return m_Regions[index].pMap->GetValue(row, col) != 0;
}

bool CTerrainRegion::ClearRegion(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	size_t region_num = m_Regions.size();

	for (size_t i = 0; i < region_num; ++i)
	{
		CWalkBitMap* pMap = m_Regions[i].pMap;

		if (NULL == pMap->GetData())
		{
			pMap->Create(m_nRows, m_nCols, NULL);
		}

		pMap->SetValue(row, col, 0);
	}

	return true;
}

CWalkBitMap* CTerrainRegion::CreateRegion(const char* name)
{
	size_t index;

	if (FindRegionIndex(name, index))
	{
		return m_Regions[index].pMap;
	}
	else
	{
		CWalkBitMap* pMap = CORE_NEW(CWalkBitMap);

		m_Regions.push_back(region_t());

		region_t& data = m_Regions.back();
		size_t name_size = strlen(name) + 1;
		char* pName = (char*)CORE_ALLOC(name_size);

		memcpy(pName, name, name_size);
		data.pName = pName;
		data.nHash = GetHashValueCase(name);
		data.pMap = pMap;

		return pMap;
	}
}

bool CTerrainRegion::SetRegionEnable(const char* name, size_t row, size_t col, 
	bool value)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	CWalkBitMap* pMap = CreateRegion(name);

	if (NULL == pMap->GetData())
	{
		pMap->Create(m_nRows, m_nCols, NULL);
	}

	return pMap->SetValue(row, col, (unsigned int)(value));
}

bool CTerrainRegion::GetRegionEnable(const char* name, size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	size_t index;

	if (!FindRegionIndex(name, index))
	{
		return false;
	}

	unsigned int value = m_Regions[index].pMap->GetValue(row, col);

	return (value != 0);
}

bool CTerrainRegion::ChangeRegionName(const char* ori_name, 
	const char* new_name)
{
	size_t index;

	if (!FindRegionIndex(ori_name, index))
	{
		return false;
	}

	size_t name_size = strlen(new_name) + 1;
	char* pName = (char*)CORE_ALLOC(name_size);

	memcpy(pName, new_name, name_size);

	char* old_name = m_Regions[index].pName;

	CORE_FREE(old_name, strlen(old_name) + 1);
	m_Regions[index].pName = pName;
	m_Regions[index].nHash = GetHashValueCase(new_name);

	return true;
}

bool CTerrainRegion::BuildRegion(const char* name, size_t rows, size_t cols, 
	const unsigned char* pdata, size_t size)
{
	Assert(name != NULL);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	CWalkBitMap* pMap = CreateRegion(name);

	return pMap->Create(m_nRows, m_nCols, pdata);
}

bool CTerrainRegion::BuildRegionCompress(const char* name, size_t rows, 
	size_t cols, unsigned char* pdata, size_t size)
{
	Assert(name != NULL);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	CWalkBitMap* pMap = CreateRegion(name);

	return pMap->BuildCompress(m_nRows, m_nCols, pdata, size);
}

bool CTerrainRegion::FindAreaIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);
	size_t area_num = m_Areas.size();

	for (size_t i = 0; i < area_num; ++i)
	{
		if ((m_Areas[i].nHash == hash) 
			&& (strcmp(m_Areas[i].pName, name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

size_t CTerrainRegion::GetAreaCount() const
{
	return m_Areas.size();
}

const char* CTerrainRegion::GetAreaName(size_t index) const
{
	Assert(index < m_Areas.size());

	return m_Areas[index].pName;
}

CWalkBitMap* CTerrainRegion::GetAreaMap(size_t index) const
{
	Assert(index < m_Areas.size());

	return m_Areas[index].pMap;
}

bool CTerrainRegion::GetAreaValue(size_t index, size_t row, size_t col)
{
	Assert(index < m_Areas.size());
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	return m_Areas[index].pMap->GetValue(row, col) != 0;
}

bool CTerrainRegion::ClearArea(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	size_t area_num = m_Areas.size();

	for (size_t i = 0; i < area_num; ++i)
	{
		CWalkBitMap* pMap = m_Areas[i].pMap;
		
		if (NULL == pMap->GetData())
		{
			pMap->Create(m_nRows, m_nCols, NULL);
		}

		pMap->SetValue(row, col, 0);
	}

	return true;
}

CWalkBitMap* CTerrainRegion::CreateArea(const char* name)
{
	size_t index;

	if (FindAreaIndex(name, index))
	{
		return m_Areas[index].pMap;
	}
	else
	{
		CWalkBitMap* pMap = CORE_NEW(CWalkBitMap);

		// pMap->Create(m_nRows, m_nCols, NULL);

		m_Areas.push_back(area_t());

		area_t& data = m_Areas.back();
		size_t name_size = strlen(name) + 1;
		char* pName = (char*)CORE_ALLOC(name_size);

		memcpy(pName, name, name_size);
		data.pName = pName;
		data.nHash = GetHashValueCase(name);
		data.pMap = pMap;

		return pMap;
	}
}

bool CTerrainRegion::SetAreaEnable(const char* name, size_t row, size_t col, 
	bool value)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	CWalkBitMap* pMap = CreateArea(name);

	if (NULL == pMap->GetData())
	{
		pMap->Create(m_nRows, m_nCols, NULL);
	}

	return pMap->SetValue(row, col, (unsigned int)(value));
}

bool CTerrainRegion::GetAreaEnable(const char* name, size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	
	size_t index;

	if (!FindAreaIndex(name, index))
	{
		return false;
	}

	unsigned int value = m_Areas[index].pMap->GetValue(row, col);

	return (value != 0);
}

const char* CTerrainRegion::GetValidArea(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	size_t area_num = m_Areas.size();
	
	for (size_t i = 0; i < area_num; ++i)
	{
		unsigned int value = m_Areas[i].pMap->GetValue(row, col);

		if (value != 0)
		{
			return m_Areas[i].pName;
		}
	}

	return "";
}

// 改变地区名称（编辑器用）
bool CTerrainRegion::ChangeAreaName(const char* ori_name, 
	const char* new_name)
{
	size_t index;

	if (!FindAreaIndex(ori_name, index))
	{
		return false;
	}

	size_t name_size = strlen(new_name) + 1;
	char* pName = (char*)CORE_ALLOC(name_size);

	memcpy(pName, new_name, name_size);

	char* old_name = m_Areas[index].pName;

	CORE_FREE(old_name, strlen(old_name) + 1);
	m_Areas[index].pName = pName;
	m_Areas[index].nHash = GetHashValueCase(new_name);

	return true;
}

bool CTerrainRegion::BuildArea(const char* name, size_t rows, size_t cols, 
	const unsigned char* pdata, size_t size)
{
	Assert(name != NULL);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	CWalkBitMap* pMap = CreateArea(name);

	return pMap->Create(m_nRows, m_nCols, pdata);
}

bool CTerrainRegion::BuildAreaCompress(const char* name, size_t rows, 
	size_t cols, unsigned char* pdata, size_t size)
{
	Assert(name != NULL);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	CWalkBitMap* pMap = CreateArea(name);

	return pMap->BuildCompress(m_nRows, m_nCols, pdata, size);
}

