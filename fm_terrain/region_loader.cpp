//--------------------------------------------------------------------
// 文件名:		region_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "region_loader.h"
#include "terrain_zone.h"
#include "terrain_region.h"
#include "terrain.h"
#include "block_file.h"
#include "block_region.h"
#include "block_area.h"
#include "walk_bit_compress.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

//#define BLOCK_ID_REGION_0 '0NGR'
#define BLOCK_ID_REGION_1 '1NGR'
#define BLOCK_ID_AREA_0 '0ERA'

// CRegionLoader

CRegionLoader* CRegionLoader::NewInstance()
{
	return CORE_NEW(CRegionLoader);
}

IBlockReader* CRegionLoader::CreateBlockReader(int block_id, void* context)
{
	CRegionLoader* pthis = (CRegionLoader*)context;

	//if (block_id == BLOCK_ID_REGION_0)
	if (block_id == BLOCK_ID_REGION_1)
	{
		return CBlockRegionReader::NewInstance(&pthis->m_Region, 
			pthis->m_nRows, pthis->m_nCols);
	}
	else if (block_id == BLOCK_ID_AREA_0)
	{
		return CBlockAreaReader::NewInstance(&pthis->m_Area, 
			pthis->m_nRows, pthis->m_nCols);
	}
	
	return NULL;
}

CRegionLoader::CRegionLoader()
{
	m_pOwner = NULL;
	m_nRows = 0;
	m_nCols = 0;
}

CRegionLoader::~CRegionLoader()
{
	for (size_t i = 0; i < m_Region.size(); ++i)
	{
		char* pName = m_Region[i].pName;

		CORE_FREE(pName, strlen(pName) + 1);
		
		if (m_Region[i].pData)
		{
			CORE_FREE(m_Region[i].pData, m_Region[i].nSize);
		}
	}

	for (size_t k = 0; k < m_Area.size(); ++k)
	{
		char* pName = m_Area[k].pName;

		CORE_FREE(pName, strlen(pName) + 1);

		if (m_Area[k].pData)
		{
			CORE_FREE(m_Area[k].pData, m_Area[k].nSize);
		}
	}
}

void CRegionLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CRegionLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);

	CTerrainRegion* pRegion = pOwner->GetRegion();
	
	m_pOwner = pOwner;
	m_nRows = pRegion->GetRows();
	m_nCols = pRegion->GetCols();
}

void CRegionLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

void CRegionLoader::SetAreaFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strAreaFile = file_name;
}

bool CRegionLoader::LoadBlockRegion(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

bool CRegionLoader::LoadBlockArea(const char* file_name)
{
	Assert(file_name != NULL);

	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);

	return block_file.Load(file_name);
}

const char* CRegionLoader::GetName()
{
	if (m_strFileName.empty())
	{
		return m_strAreaFile.c_str();
	}
	else
	{
		return m_strFileName.c_str();
	}
}

bool CRegionLoader::Load(bool async)
{
	if (!m_strFileName.empty())
	{
		LoadBlockRegion(m_strFileName.c_str());
	}

	if (!m_strAreaFile.empty())
	{
		LoadBlockArea(m_strAreaFile.c_str());
	}

	return true;
}

bool CRegionLoader::Create(bool async)
{
	Terrain* pTerrain = m_pOwner->GetTerrain();
	CTerrainRegion* pRegion = m_pOwner->GetRegion();

	/*
	for (size_t i = 0; i < m_Region.nRegionCount; ++i)
	{
		if (NULL == m_Region.pRegions[i])
		{
			continue;
		}	
		
		if (m_pOwner->GetTerrain()->GetDesignMode())
		{
			// 在设计状态下使用非压缩的数据
			CWalkBitCompress compress;
			
			if (!compress.Decompress(m_nRows, m_Region.pRegions[i], 
				m_Region.nRegionsSize[i]))
			{
				return false;
			}
			
			if (!pRegion->Build(i, m_nRows, m_nCols, 
				compress.GetResultData(), compress.GetResultSize()))
			{
				return false;
			}
		}
		else
		{
			// 使用压缩的数据
			if (!pRegion->BuildCompress(i, m_nRows, m_nCols,
				m_Region.pRegions[i], m_Region.nRegionsSize[i]))
			{
				return false;
			}
	
			// 数据所有权已转移
			m_Region.pRegions[i] = NULL;
		}
	}
	*/
	for (size_t i = 0; i < m_Region.size(); ++i)
	{
		const char* region_name = m_Region[i].pName;

		if (m_pOwner->GetTerrain()->GetDesignMode())
		{
			// 在设计状态下使用非压缩的数据
			CWalkBitCompress compress;

			if (!compress.Decompress(m_nRows, m_Region[i].pData, 
				m_Region[i].nSize))
			{
				return false;
			}

			if (!pRegion->BuildRegion(region_name, m_nRows, m_nCols, 
				compress.GetResultData(), compress.GetResultSize()))
			{
				return false;
			}

			unsigned int color = pTerrain->GetRegionColor(region_name);

			if (color == 0)
			{
				// 根据名字生成一个随机的颜色
				unsigned int hash = GetHashValueCase(region_name);
				unsigned c1 = hash % 256;
				unsigned c2 = hash % 3;

				if (c1 < 50)
				{
					c1 += 50;
				}

				unsigned int color;

				if (c2 == 0)
				{
					color = COLOR_ARGB(255, c1, 0, 0);
				}
				else if (c2 == 1)
				{
					color = COLOR_ARGB(255, 0, c1, 0);
				}
				else
				{
					color = COLOR_ARGB(255, 0, 0, c1);
				}

				// 设置缺省的显示颜色
				pTerrain->SetRegionColor(region_name, color);
			}
		}
		else
		{
			// 使用压缩的数据
			if (!pRegion->BuildRegionCompress(m_Region[i].pName, 
				m_nRows, m_nCols, m_Region[i].pData, m_Region[i].nSize))
			{
				return false;
			}

			// 数据所有权已转移
			m_Region[i].pData = NULL;
		}
	}

	for (size_t k = 0; k < m_Area.size(); ++k)
	{
		const char* area_name = m_Area[k].pName;

		if (m_pOwner->GetTerrain()->GetDesignMode())
		{
			// 在设计状态下使用非压缩的数据
			CWalkBitCompress compress;

			if (!compress.Decompress(m_nRows, m_Area[k].pData, 
				m_Area[k].nSize))
			{
				return false;
			}

			if (!pRegion->BuildArea(area_name, m_nRows, m_nCols, 
				compress.GetResultData(), compress.GetResultSize()))
			{
				return false;
			}

			unsigned int color = pTerrain->GetAreaColor(area_name);
			
			if (color == 0)
			{
				// 根据名字生成一个随机的颜色
				unsigned int hash = GetHashValueCase(area_name);
				unsigned c1 = hash % 256;
				unsigned c2 = hash % 3;

				if (c1 < 50)
				{
					c1 += 50;
				}

				unsigned int color;

				if (c2 == 0)
				{
					color = COLOR_ARGB(255, c1, 0, 0);
				}
				else if (c2 == 1)
				{
					color = COLOR_ARGB(255, 0, c1, 0);
				}
				else
				{
					color = COLOR_ARGB(255, 0, 0, c1);
				}
				
				// 设置缺省的显示颜色
				pTerrain->SetAreaColor(area_name, color);
			}
		}
		else
		{
			// 使用压缩的数据
			if (!pRegion->BuildAreaCompress(m_Area[k].pName, 
				m_nRows, m_nCols, m_Area[k].pData, m_Area[k].nSize))
			{
				return false;
			}

			// 数据所有权已转移
			m_Area[k].pData = NULL;
		}
	}

	// 设置数据加载完成
	m_pOwner->SetRegionLoaded(true);
	
	return true;
}

bool CRegionLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

bool CRegionLoader::SaveBlockRegion(const char* file_name, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockRegionWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_REGION_1, &writer))
	{
		return false;
	}

	return block_file.SaveEnd(handle);
}

bool CRegionLoader::SaveBlockArea(const char* file_name, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);

	if (pZone->GetRegion()->GetAreaCount() == 0)
	{
		return true;
	}

	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);

	void* handle = block_file.SaveBegin(file_name);

	if (NULL == handle)
	{
		return false;
	}

	CBlockAreaWriter writer(pZone);

	if (!block_file.SaveBlock(handle, BLOCK_ID_AREA_0, &writer))
	{
		return false;
	}

	return block_file.SaveEnd(handle);
}

