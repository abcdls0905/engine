//--------------------------------------------------------------------
// 文件名:		DynamicWalkLoader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月11日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_walk_loader.h"

#include "block_dynamic_walk.h"
#include "block_file.h"
#include "terrain.h"

#define BLOCK_ID_WALK_1 '1KLW'

CDynamicWalkLoader* CDynamicWalkLoader::NewInstance()
{
	return CORE_NEW(CDynamicWalkLoader);
}

IBlockReader* CDynamicWalkLoader::CreateBlockReader(int block_id, void* context)
{
	CDynamicWalkLoader* pThis = (CDynamicWalkLoader*)context;

	if (block_id == BLOCK_ID_WALK_1)
	{
		return CBlockDynamicWalkReader::NewInstance(&pThis->m_Walk,
			pThis->m_nRows, pThis->m_nCols);
	}

	return NULL;
}

CDynamicWalkLoader::CDynamicWalkLoader()
{
	m_pOwner = NULL;
	m_nRows = 0;
	m_nCols = 0;
	memset(&m_Walk, 0, sizeof(m_Walk));
}

CDynamicWalkLoader::~CDynamicWalkLoader()
{	
	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (m_Walk.pFloorHeights[i])
		{
			CORE_FREE(m_Walk.pFloorHeights[i], m_Walk.nFloorHeightsSize[i]);
		}
		
		if (m_Walk.pSpaceHeights[i])
		{
			CORE_FREE(m_Walk.pSpaceHeights[i], m_Walk.nSpaceHeightsSize[i]);
		}
		
		if (m_Walk.pFloorMarkers[i])
		{
			CORE_FREE(m_Walk.pFloorMarkers[i], m_Walk.nFloorMarkersSize[i]);
		}
	}
}

void CDynamicWalkLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CDynamicWalkLoader::SetOwner(CDynamicWalkable* pOwner)
{
	Assert(pOwner != NULL);
	
	m_pOwner = pOwner;
	m_nRows = pOwner->GetRows();
	m_nCols = pOwner->GetCols();
}

void CDynamicWalkLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

bool CDynamicWalkLoader::LoadBlockWalk(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

const char* CDynamicWalkLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CDynamicWalkLoader::Load(bool async)
{
	if (!m_strFileName.empty())
	{
		// 加载行走信息文件
		return LoadBlockWalk(m_strFileName.c_str());
	}

	return false;
}

bool CDynamicWalkLoader::Create(bool async)
{
	CDynamicWalkable* pWalkable = m_pOwner;

	for (size_t i = 0; i < m_Walk.nFloorCount; ++i)
	{
		if (m_Walk.pFloorHeights[i])
		{
			if (!pWalkable->BuildFloorHeight(m_nRows, m_nCols,
				m_Walk.pFloorHeights[i], m_Walk.nFloorHeightsSize[i], i))
			{
				return false;
			}
			
			// 数据所有权已转移
			m_Walk.pFloorHeights[i] = NULL;
		}
		
		if (m_Walk.pSpaceHeights[i])
		{
			if (!pWalkable->BuildSpaceHeight(m_nRows, m_nCols,
				m_Walk.pSpaceHeights[i], m_Walk.nSpaceHeightsSize[i], i))
			{
				return false;
			}

			// 数据所有权已转移
			m_Walk.pSpaceHeights[i] = NULL;
		}
		
		if (m_Walk.pFloorMarkers[i])
		{
			if (!pWalkable->BuildFloorMarker(m_nRows, m_nCols,
				m_Walk.pFloorMarkers[i], m_Walk.nFloorMarkersSize[i], i))
			{
				return false;
			}

			// 数据所有权已转移
			m_Walk.pFloorMarkers[i] = NULL;
		}
	}

	pWalkable->SetAngleY(m_Walk.fAngleY);
	pWalkable->SetHeight(m_Walk.fHeight);
	pWalkable->SetRadius(m_Walk.fRadius);
	pWalkable->SetBoxSizeX(m_Walk.fBoxSizeX);
	pWalkable->SetBoxSizeZ(m_Walk.fBoxSizeZ);

	// 设置数据加载完成
	//m_pOwner->SetWalkLoaded(true);
	
	return true;
}

bool CDynamicWalkLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
	return true;
}

bool CDynamicWalkLoader::SaveBlockWalk(const char* file_name, CDynamicWalkable* pWalk)
{
	Assert(file_name != NULL);
	Assert(pWalk != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockDynamicWalkWriter writer(pWalk);
	
	//if (!block_file.SaveBlock(handle, BLOCK_ID_WALK_0, &writer))
	if (!block_file.SaveBlock(handle, BLOCK_ID_WALK_1, &writer))
	{
		return false;
	}

	return block_file.SaveEnd(handle);
}
