//--------------------------------------------------------------------
// 文件名:		zone_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "zone_manager.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_light.h"
#include "terrain_visuals.h"
#include "tex_light_map.h"
#include "terrain_factory.h"
#include "terrain_chunk.h"
#include "terrain_height.h"
#include "quad_collide.h"
#include "quad_tree.h"
#include "quad_node.h"
#include "../visual/i_vis_base.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/core_file.h"
#include "../public/var_list.h"
#include "../public/var_table.h"
#include "../public/inlines.h"

// CZoneManager

CZoneManager::CZoneManager()
{
	m_pTerrain = NULL;
	m_pZones = NULL;
	m_nZoneRows = 0;
	m_nZoneCols = 0;
	m_nZoneNum = 0;
	m_nZoneOriginRow = 0;
	m_nZoneOriginCol = 0;
	m_nZoneCurrentRow = 0;
	m_nZoneCurrentCol = 0;
	m_nZoneRange = 2;
	m_fGlobalLeft = 0.0F;
	m_fGlobalTop = 0.0F;
	m_fZoneWidth = 0.0F;
	m_fZoneHeight = 0.0F;
	m_fInvZoneSize = 0.0F;
	m_nLoadProgress = 0;
	m_pFactory = CORE_NEW(CTerrainFactory);
	memset(m_nRangeZones, 0, sizeof(m_nRangeZones));
	memset(m_NearZones, 0, sizeof(m_NearZones));
}

CZoneManager::~CZoneManager()
{
	ReleaseAll();
	CORE_DELETE(m_pFactory);
}

void CZoneManager::ReleaseAll()
{
	if (m_pZones)
	{
		for (size_t i = 0; i < m_nZoneNum; ++i)
		{
			SAFE_RELEASE(m_pZones[i]);
		}
		
		CORE_FREE(m_pZones, sizeof(CTerrainZone*) * m_nZoneNum);
		m_pZones = NULL;
	}
}

bool CZoneManager::Initialize()
{
	//m_UnloadTimer.Initialize();
	
	for (int j = 0; j < MAX_RANGE_NUM; ++j)
	{
		m_nRangeZones[j] = (j * 2 + 1) * (j * 2 + 1);
	}

	size_t count = 0;
	
	for (int r = 0; r < MAX_RANGE_NUM; ++r)
	{
		for (int i = -r; i <= r; ++i)
		{
			for (int k = -r; k <= r; ++k)
			{
				if ((abs(i) == r) || (abs(k) == r))
				{
					near_zone_t* p = m_NearZones + count++;
					p->nRow = i;
					p->nCol = k;
				}
			}
		}
	}

	// 优化
	m_fGlobalLeft = m_pTerrain->GetLeft();
	m_fGlobalTop = m_pTerrain->GetTop();
	m_fZoneWidth = m_pTerrain->GetZoneWidth();
	m_fZoneHeight = m_pTerrain->GetZoneHeight();
	m_fInvZoneSize = 1.0F / m_fZoneWidth;
	
	return true;
}

//bool CZoneManager::Shut()
//{
//	ReleaseAll();
//	
//	return true;
//}

void CZoneManager::ReleaseAllZoneLoader()
{
	if (m_pZones)
	{
		for (size_t i = 0; i < m_nZoneNum; ++i)
		{
			if (m_pZones[i])
			{
				m_pZones[i]->ReleaseAllLoader();
			}
		}
	}
}

void CZoneManager::ReleaseAllVisuals()
{
	if (m_pZones)
	{
		for (size_t i = 0; i < m_nZoneNum; ++i)
		{
			if (m_pZones[i])
			{
				m_pZones[i]->ReleaseVisualObjects();
			}
		}
	}
}

void CZoneManager::ReleaseAllChunkCB()
{
	size_t zone_num = m_ExistZones.size();
	CTerrainZone** pZones = m_ExistZones.data();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* zone = pZones[i];

		if (NULL == zone)
		{
			continue;
		}

		size_t chunk_num = zone->GetChunkCount();
		CTerrainChunk* pChunks = zone->GetChunks();

		for (size_t k = 0; k < chunk_num; ++k)
		{
			pChunks[k].ReleaseConstBuffer();
		}
	}
}

void CZoneManager::CheckLoading()
{
	//m_LoadCounter.Reset();
	
	// 加载进度
	if (m_nLoadProgress == LOAD_BEGIN)
	{
		if (GetLoadAllHeight(m_nZoneRange + 1))
		{
#ifdef PHYSICS_HAVOK_TERRAIN
			// 要在加载Visual前生成刚体地形
			AsyncLoadRigid(m_nZoneRange + 1);
#endif // PHYSICS_HAVOK_TERRAIN
			AsyncLoadLight(m_nZoneRange + 1);
			AsyncLoadAlpha(m_nZoneRange + 1);
			AsyncLoadVisual(m_nZoneRange + 1);
			AsyncLoadWalk(m_nZoneRange + 1);
			AsyncLoadRegion(m_nZoneRange + 1);

			m_nLoadProgress = LOAD_ALLHEIGHT;
		}
	}
	else if (m_nLoadProgress == LOAD_ALLHEIGHT)
	{
		if (GetLoadComplete(m_nZoneRange))
		{
			LoadConnectHeight(m_nZoneRange);

			m_nLoadProgress = LOAD_COMPLETE;
		}
		else
		{
			CheckLoadCollide(m_nZoneRange + 1);
		}
	}
	else if (m_nLoadProgress == LOAD_COMPLETE)
	{
		// 定时检查卸载区域
		int tick = m_UnloadTimer.GetElapseMillisec(500);

		if (tick >= 500)
		{
			CheckUnload(tick * 0.001F, m_nZoneRange + 1);
		}
	}
}

CTerrainZone* CZoneManager::GetSiblingZoneByIndex(CTerrainZone* zone,
	size_t index)
{
	Assert(index < SIBLING_MAX);
	
	switch (index)
	{
	case SIBLING_LEFT:
		return GetLeftZone(zone);
	case SIBLING_RIGHT:
		return GetRightZone(zone);
	case SIBLING_UP:
		return GetUpZone(zone);
	case SIBLING_DOWN:
		return GetDownZone(zone);
	case SIBLING_LEFT_UP:
		return GetLeftUpZone(zone);
	case SIBLING_RIGHT_UP:
		return GetRightUpZone(zone);
	case SIBLING_LEFT_DOWN:
		return GetLeftDownZone(zone);
	case SIBLING_RIGHT_DOWN:
		return GetRightDownZone(zone);
	default:
		break;
	}
	
	return NULL;
}

//CTerrainZone* CZoneManager::GetInZone(float x, float z)
//{
//	float offset_x = x - m_fGlobalLeft;
//	float offset_z = z - m_fGlobalTop;
//	int col = int(offset_x / m_fZoneWidth);
//	int row = int(offset_z / m_fZoneHeight);
//	size_t zone_cols = m_nZoneCols;
//	
//	if ((size_t(row) >= m_nZoneRows) || (size_t(col) >= zone_cols))
//	{
//		return NULL;
//	}
//
//	return m_pZones[row * zone_cols + col];
//}

bool CZoneManager::GetZonePos(float x, float z, int& row, int& col)
{
	col = int((x - m_fGlobalLeft) / m_fZoneWidth);
	row = int((z - m_fGlobalTop) / m_fZoneHeight);

	return true;
}

bool CZoneManager::SetZones(size_t rows, size_t cols)
{
	Assert(rows > 0);
	Assert(cols > 0);

	size_t zone_num = rows * cols;
	CTerrainZone** p = (CTerrainZone**)CORE_ALLOC(
		sizeof(CTerrainZone*) * zone_num);

	memset(p, 0, sizeof(CTerrainZone*) * zone_num);

	Assert(m_pZones == NULL);

	ReleaseAll();
	
	m_pZones = p;
	m_nZoneRows = rows;
	m_nZoneCols = cols;
	m_nZoneNum = zone_num;

	return true;
}

bool CZoneManager::SetZoneOrigin(size_t row, size_t col)
{
	Assert(row < m_nZoneRows);
	Assert(col < m_nZoneCols);
	
	m_nZoneOriginRow = row;
	m_nZoneOriginCol = col;
	
	// 将地形的（0，0）置于此区域的中心位置
	float width = m_pTerrain->GetZoneWidth();
	float height = m_pTerrain->GetZoneHeight();
	float left = -((col + 0.5F) * width);
	float top = -((row + 0.5F) * height);

	// 根据地形中心的变化，调整左上角的坐标
	m_pTerrain->SetLeft(left);
	m_pTerrain->SetTop(top);
	
	return true;
}

bool CZoneManager::SetZoneRange(size_t value)
{
	// 保留一层预加载的区域
	if (value >= (MAX_RANGE_NUM - 1))
	{
		return false;
	}
	
	m_nZoneRange = value;
	
	return true;
}

bool CZoneManager::SetZoneCurrent(size_t row, size_t col)
{
	if (m_nLoadProgress != 0)
	{
		if ((row == m_nZoneCurrentRow) && (col == m_nZoneCurrentCol))
		{
			return true;
		}
	}
	
	m_nZoneCurrentRow = row;
	m_nZoneCurrentCol = col;

	// 创建区域
	size_t zone_num = m_nRangeZones[m_nZoneRange + 1];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CreateZone(r, c);
	}

	// 先加载高度
	AsyncLoadHeight(m_nZoneRange + 1);
	
	m_nLoadProgress = LOAD_BEGIN;
	
	return true;
}

CTerrainZone* CZoneManager::CreateZone(size_t row, size_t col)
{
	Assert(row < m_nZoneRows);
	Assert(col < m_nZoneCols);
	
	size_t index = row * m_nZoneCols + col;
	
	if (m_pZones[index])
	{
		return m_pZones[index];
	}
	
	CTerrainZone* pZone = m_pFactory->Create(m_pTerrain, index, row, col);
	char name[64];

	SafeSprintf(name, sizeof(name), "zone_%d_%d", row, col);
	pZone->SetName(name);
	pZone->Init();

	// 区域对应的四叉树节点
	float zone_center_x = (pZone->GetLeft() + pZone->GetRight()) * 0.5F;
	float zone_center_z = (pZone->GetTop() + pZone->GetBottom()) * 0.5F;
	size_t scale = m_pTerrain->GetZoneScale() * (size_t)m_pTerrain->GetUnitSize();
	CQuadNode* pRoot = m_pTerrain->GetCollide()->GetQuadTree()->GetRoot();
	CQuadNode* pNode = pRoot->GetInNewNode(scale, zone_center_x, zone_center_z);
	
	pZone->SetQuadNode(pNode);
	
	m_pZones[index] = pZone;

	// 添加到记录表
	size_t exist_size = m_ExistZones.size();
	CTerrainZone** exist_zones = m_ExistZones.data();
	size_t k;

	for (k = 0; k < exist_size; ++k)
	{
		if (NULL == exist_zones[k])
		{
			exist_zones[k] = pZone;
			break;
		}
	}

	if (k == exist_size)
	{
		m_ExistZones.push_back(pZone);
	}

	return pZone;
}

size_t CZoneManager::GetRangeZoneList(CTerrainZone** zones, size_t max_num, 
	size_t zone_range)
{
	Assert(max_num >= m_nRangeZones[zone_range]);

	size_t count = 0;
	size_t zone_rows = m_nZoneRows;
	size_t zone_cols = m_nZoneCols;
	int cur_row = (int)m_nZoneCurrentRow;
	int cur_col = (int)m_nZoneCurrentCol;
	CTerrainZone** all_zones = m_pZones;
	size_t range_zone_num = m_nRangeZones[zone_range];
	near_zone_t* pNearZone = m_NearZones;

	for (size_t i = 0; i < range_zone_num; ++i)
	{
		int r = cur_row + pNearZone[i].nRow;
		int c = cur_col + pNearZone[i].nCol;

		if ((size_t(r) >= zone_rows) || (size_t(c) >= zone_cols))
		{
			continue;
		}

		zones[count++] = all_zones[r * zone_cols + c];
	}

	return count;
}

float CZoneManager::GetLoadPercent()
{
	float load_count = 0;
	int zone_count = 0;
	int range_zone_num = (int)m_nRangeZones[m_nZoneRange];

	for (int i = 0; i < range_zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;

		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}

		zone_count++;

		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];

		if (NULL == zone)
		{
			continue;
		}

		load_count += zone->GetLoadPercent();
	}

	if (0 == zone_count)
	{
		// 没有任何区域
		return 1.0F;
	}

	return load_count / float(zone_count);
}

bool CZoneManager::LoadConnectHeight(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];

		zone->GetHeight()->ConnectValues();

		size_t chunk_dims = zone->GetChunkDims();

		for (size_t row = 0; row < chunk_dims; ++row)
		{
			CTerrainChunk* pChunk = zone->GetChunk(row, chunk_dims - 1);

			pChunk->ReleaseVertexBuffer();
		}
		
		for (size_t col = 0; col < chunk_dims; ++col)
		{
			CTerrainChunk* pChunk = zone->GetChunk(chunk_dims - 1, col);
			
			pChunk->ReleaseVertexBuffer();
		}

		// 更新相邻分块
		zone->UpdateChunkSibling();
	}

	return true;
}

bool CZoneManager::AsyncLoadHeight(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];
		
		if (zone->GetHeightLoaded())
		{
			continue;
		}
		
		if (zone->GetHeightLoader() != NULL)
		{
			continue;
		}
		
		if (!zone->BeginCreateHeight())
		{
			continue;
		}
		
		m_pTerrain->GetRender()->RequestLoadResource( 
			CTerrainZone::RES_CATEGORY_HEIGHT, zone->GetHeightLoader());
	}
	
	return true;
}

 
bool CZoneManager::AsyncLoadLight(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];
		
		if (zone->GetLightLoaded())
		{
			continue;
		}
		
		if (zone->GetLightLoader() != NULL)
		{
			continue;
		}
		
		if (!zone->BeginCreateLight())
		{
			continue;
		}
		
		m_pTerrain->GetRender()->RequestLoadResource( 
			CTerrainZone::RES_CATEGORY_LIGHT, zone->GetLightLoader());
	}
	
	return true;
}


bool CZoneManager::AsyncLoadAlpha(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];
		
		if (zone->GetAlphaLoaded())
		{
			continue;
		}
		
		if (zone->GetAlphaLoader() != NULL)
		{
			continue;
		}
		
		if (!zone->BeginCreateAlpha())
		{
			continue;
		}
		
		m_pTerrain->GetRender()->RequestLoadResource( 
			CTerrainZone::RES_CATEGORY_ALPHA, zone->GetAlphaLoader());
	}
	
	return true;
}

bool CZoneManager::AsyncLoadVisual(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];
		
		if (zone->GetVisualLoaded())
		{
			continue;
		}
		
		if (zone->GetVisualLoader() != NULL)
		{
			continue;
		}
		
		if (!zone->BeginCreateVisual())
		{
			continue;
		}
		
		m_pTerrain->GetRender()->RequestLoadResource( 
			CTerrainZone::RES_CATEGORY_VISUAL, zone->GetVisualLoader());
	}
	
	return true;
}

bool CZoneManager::AsyncLoadWalk(size_t range)
{
	Assert(range < MAX_RANGE_NUM);

	size_t zone_num = m_nRangeZones[range];

	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;

		if ((size_t(r) >= m_nZoneRows)	|| (size_t(c) >= m_nZoneCols))
		{
			continue;
		}

		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];

		if (zone->GetWalkLoaded())
		{
			continue;
		}

		if (zone->GetWalkLoader() != NULL)
		{
			continue;
		}

		if (!zone->BeginCreateWalk())
		{
			continue;
		}

		m_pTerrain->GetRender()->RequestLoadResource( 
			CTerrainZone::RES_CATEGORY_WALK, zone->GetWalkLoader());
	}

	return true;
}

bool CZoneManager::AsyncLoadRegion(size_t range)
{
	Assert(range < MAX_RANGE_NUM);

	size_t zone_num = m_nRangeZones[range];

	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;

		if ((size_t(r) >= m_nZoneRows)	|| (size_t(c) >= m_nZoneCols))
		{
			continue;
		}

		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];

		if (zone->GetRegionLoaded())
		{
			continue;
		}

		if (zone->GetRegionLoader() != NULL)
		{
			continue;
		}

		if (!zone->BeginCreateRegion())
		{
			continue;
		}

		m_pTerrain->GetRender()->RequestLoadResource( 
			CTerrainZone::RES_CATEGORY_REGION, zone->GetRegionLoader());
	}

	return true;
}

#ifdef PHYSICS_HAVOK_TERRAIN
bool CZoneManager::AsyncLoadRigid(size_t range)
{
	if (!m_pTerrain->GetRender()->GetSupportPhysics())
	{
		return true;
	}

	Assert(range < MAX_RANGE_NUM);

	size_t zone_num = m_nRangeZones[range];

	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;

		if ((size_t(r) >= m_nZoneRows)	|| (size_t(c) >= m_nZoneCols))
		{
			continue;
		}

		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];

		if (zone->GetRigidLoaded())
		{
			continue;
		}

		if (zone->GetRigidLoader() != NULL)
		{
			continue;
		}

		if (!zone->BeginCreateRigid())
		{
			continue;
		}

		m_pTerrain->GetRender()->RequestLoadResource(
			CTerrainZone::RES_CATEGORY_RIGID, zone->GetRigidLoader());
	}

	return true;
}
#endif // PHYSICS_HAVOK_TERRAIN

bool CZoneManager::GetLoadAllHeight(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];
		
		if (!zone->GetHeightLoaded())
		{
			return false;
		}
	}
	
	return true;
}

bool CZoneManager::GetLoadComplete(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t zone_num = m_nRangeZones[range];
	
	for (size_t i = 0; i < zone_num; ++i)
	{
		int r = int(m_nZoneCurrentRow) + m_NearZones[i].nRow;
		int c = int(m_nZoneCurrentCol) + m_NearZones[i].nCol;
		
		if ((size_t(r) >= m_nZoneRows) || (size_t(c) >= m_nZoneCols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * m_nZoneCols + c];
		
		if (!zone->GetAllLoaded())
		{
			return false;
		}
	}
	
	return true;
}

bool CZoneManager::CheckLoadCollide(size_t range)
{
	Assert(range < MAX_RANGE_NUM);
	
	size_t count = 0;
	size_t near_zone_num = m_nRangeZones[range];
	near_zone_t* near_zones = m_NearZones;
	size_t zone_rows = m_nZoneRows;
	size_t zone_cols = m_nZoneCols;
	int current_row = (int)m_nZoneCurrentRow;
	int current_col = (int)m_nZoneCurrentCol;
	
	for (size_t i = 0; i < near_zone_num; ++i)
	{
		int r = current_row + near_zones[i].nRow;
		int c = current_col + near_zones[i].nCol;
		
		if ((size_t(r) >= zone_rows) || (size_t(c) >= zone_cols))
		{
			continue;
		}
		
		CTerrainZone* zone = m_pZones[r * zone_cols + c];
		
		if (zone->GetCollideLoaded())
		{
			continue;
		}
		
		while (zone->LoadNextCollide())
		{
			// 控制每帧运行的时间
			//if (m_LoadCounter.GetElapseMillisec() > 10)
			//{
			//	return true;
			//}
			
			// 控制每帧加载的数量
			if (++count >= 32)
			{
				return true;
			}
		}
	}
	
	return (count > 0);
}

bool CZoneManager::CheckUnload(float seconds, size_t range)
{
	Assert(range < MAX_RANGE_NUM);

	bool design_mode = m_pTerrain->GetDesignMode();
	// 检查所有的区域
	size_t zone_num = m_ExistZones.size();
	CTerrainZone** pZones = m_ExistZones.data();
	size_t del_count = 0;

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* zone = pZones[i];
		
		if (NULL == zone)
		{
			continue;
		}

		// 与当前中心区域的相对距离
		size_t zone_row = zone->GetRow();
		size_t zone_col = zone->GetCol();
		int r = int(zone_row) - int(m_nZoneCurrentRow);
		int c = int(zone_col) - int(m_nZoneCurrentCol);

		// 取绝对值
		if (r < 0)
		{
			r = -r;
		}

		if (c < 0)
		{
			c = -c;
		}

		// 是否长时间无显示
		if (zone->GetSilenceTime() < 10.0F)
		{
			if (zone->IncSilenceTime(seconds) >= 10.0F)
			{
				// 释放AlphaMap占用的贴图资源
				zone->ReleaseChunkAlphaTex();
				// 释放花草的顶点缓冲
				zone->ReleaseChunkGrassVB();
				// 释放顶点缓冲
				zone->ReleaseChunkVB();
				// 清除计数
				//zone->SetSilenceTime(0.0F);
			}
		}

		// 是否超出可用范围
		if ((size_t(r) > range) || (size_t(c) > range))
		{
			if (design_mode)
			{
				// 在设计状态时要保留有被改变的区域数据	
				if (zone->GetAnyChanged())
				{
					continue;
				}

				// 在设计状态时保留指定不区域卸载
				if (zone->GetNoUnload())
				{
					continue;
				}
			}

			// 删除碰撞树上的相关信息
			zone->ReleaseChunkObjects();
			// 删除区域中的可见物体
			zone->ReleaseVisualObjects();
			// 释放所有加载器以避免再进行不必要的加载
			zone->ReleaseAllLoader();

			// 更新相邻分块信息
			size_t chunk_dims = zone->GetChunkDims();

			// 获得相邻的区域
			CTerrainZone* pLeftZone = this->GetLeftZone(zone);
			CTerrainZone* pRightZone = this->GetRightZone(zone);
			CTerrainZone* pUpZone = this->GetUpZone(zone);
			CTerrainZone* pDownZone = this->GetDownZone(zone);

			// 从记录表中清除
			pZones[i] = NULL;
			
			// 卸载这个区域
			zone->Release();
			
			m_pZones[zone_row * m_nZoneCols + zone_col] = NULL;

			// 删除碰撞树上的无用节点
			//m_pTerrain->GetCollide()->Compress();

			// 必须在删除区域之后再更新相邻分块信息
			if (pLeftZone)
			{
				for (size_t k = 0; k < chunk_dims; ++k)
				{
					CTerrainChunk* pChunk = pLeftZone->GetRightEdgeChunk(k);

					if (pChunk)
					{
						pChunk->UpdateAllSiblings();
					}
				}
			}
			
			if (pRightZone)
			{
				for (size_t k = 0; k < chunk_dims; ++k)
				{
					CTerrainChunk* pChunk = pRightZone->GetLeftEdgeChunk(k);

					if (pChunk)
					{
						pChunk->UpdateAllSiblings();
					}
				}
			}

			if (pUpZone)
			{
				for (size_t k = 0; k < chunk_dims; ++k)
				{
					CTerrainChunk* pChunk = pUpZone->GetDownEdgeChunk(k);

					if (pChunk)
					{
						pChunk->UpdateAllSiblings();
					}
				}
			}

			if (pDownZone)
			{
				for (size_t k = 0; k < chunk_dims; ++k)
				{
					CTerrainChunk* pChunk = pDownZone->GetUpEdgeChunk(k);

					if (pChunk)
					{
						pChunk->UpdateAllSiblings();
					}
				}
			}

			++del_count;
		}
	}

	if (del_count > 0)
	{
		// 删除碰撞树上的无用节点
		m_pTerrain->GetCollide()->Compress();
	}
	
	return true;
}

bool CZoneManager::DumpZones(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	size_t count = 0;
	
	for (size_t i = 0; i < m_nZoneNum; ++i)
	{
		if (NULL == m_pZones[i])
		{
			continue;
		}

		core_file::fprintf(fp, "%s\r\n", m_pZones[i]->GetName());
		++count;
	}

	core_file::fprintf(fp, "total %d zones, factory resource number is %d", count,
		m_pFactory->GetCount());
	core_file::fclose(fp);

	return true;
}
