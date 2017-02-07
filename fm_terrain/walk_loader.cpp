//--------------------------------------------------------------------
// �ļ���:		walk_loader.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��8��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "walk_loader.h"
#include "terrain_zone.h"
#include "terrain_walkable.h"
#include "terrain.h"
#include "block_file.h"
#include "block_walk.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"

#define BLOCK_ID_WALK_1 '1KLW'

// CWalkLoader

CWalkLoader* CWalkLoader::NewInstance()
{
	return CORE_NEW(CWalkLoader);
}

IBlockReader* CWalkLoader::CreateBlockReader(int block_id, void* context)
{
	CWalkLoader* pthis = (CWalkLoader*)context;

	if (block_id == BLOCK_ID_WALK_1)
	{
		return CBlockWalkReader::NewInstance(&pthis->m_Walk, pthis->m_nRows, 
			pthis->m_nCols);
	}
	
	return NULL;
}

CWalkLoader::CWalkLoader()
{
	m_pOwner = NULL;
	m_nRows = 0;
	m_nCols = 0;
	memset(&m_Walk, 0, sizeof(m_Walk));
}

CWalkLoader::~CWalkLoader()
{
	if (m_Walk.pWaterHeight)
	{
		CORE_FREE(m_Walk.pWaterHeight, m_Walk.nWaterHeightSize);
	}

	if (m_Walk.pWalkMarker)
	{
		CORE_FREE(m_Walk.pWalkMarker, m_Walk.nWalkMarkerSize);
	}
	
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

void CWalkLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CWalkLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);

	CTerrainWalkable* pWalkable = pOwner->GetWalkable();
	
	m_pOwner = pOwner;
	m_nRows = pWalkable->GetRows();
	m_nCols = pWalkable->GetCols();
}

void CWalkLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

bool CWalkLoader::LoadBlockWalk(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

const char* CWalkLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CWalkLoader::Load(bool async)
{
	if (!m_strFileName.empty())
	{
		// ����������Ϣ�ļ�
		LoadBlockWalk(m_strFileName.c_str());
	}

	return true;
}

bool CWalkLoader::Create(bool async)
{
	CTerrainWalkable* pWalkable = m_pOwner->GetWalkable();

	if (m_Walk.pWaterHeight)
	{
		if (!pWalkable->BuildWaterHeight(m_nRows, m_nCols,
			m_Walk.pWaterHeight, m_Walk.nWaterHeightSize))
		{
			return false;
		}
		
		// ��������Ȩ��ת��
		m_Walk.pWaterHeight = NULL;
		
		// ����������ײ���ݾ���
		pWalkable->SetReady(true);
	}

	if (m_Walk.pWalkMarker)
	{
		if (!pWalkable->BuildWalkMarker(m_nRows, m_nCols,
			m_Walk.pWalkMarker, m_Walk.nWalkMarkerSize))
		{
			return false;
		}

		// ��������Ȩ��ת��
		m_Walk.pWalkMarker = NULL;
	}

	for (size_t i = 0; i < m_Walk.nFloorCount; ++i)
	{
		if (m_Walk.pFloorHeights[i])
		{
			if (!pWalkable->BuildFloorHeight(m_nRows, m_nCols,
				m_Walk.pFloorHeights[i], m_Walk.nFloorHeightsSize[i], (int)i))
			{
				return false;
			}
			
			// ��������Ȩ��ת��
			m_Walk.pFloorHeights[i] = NULL;
		}
		
		if (m_Walk.pSpaceHeights[i])
		{
			if (!pWalkable->BuildSpaceHeight(m_nRows, m_nCols,
				m_Walk.pSpaceHeights[i], m_Walk.nSpaceHeightsSize[i], (int)i))
			{
				return false;
			}

			// ��������Ȩ��ת��
			m_Walk.pSpaceHeights[i] = NULL;
		}
		
		if (m_Walk.pFloorMarkers[i])
		{
			if (!pWalkable->BuildFloorMarker(m_nRows, m_nCols,
				m_Walk.pFloorMarkers[i], m_Walk.nFloorMarkersSize[i], (int)i))
			{
				return false;
			}

			// ��������Ȩ��ת��
			m_Walk.pFloorMarkers[i] = NULL;
		}
	}

	// �������ݼ������
	m_pOwner->SetWalkLoaded(true);
	
	return true;
}

bool CWalkLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

bool CWalkLoader::SaveBlockWalk(const char* file_name, CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockWalkWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_WALK_1, &writer))
	{
		return false;
	}

	return block_file.SaveEnd(handle);
}
