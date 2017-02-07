//--------------------------------------------------------------------
// 文件名:		DynamicWalkable.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年2月28日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_walkable.h"

#include "dynamic_walk_loader.h"
#include "terrain.h"
#include "terrain_walkable.h"
#include "walk_height_map.h"
#include "walk_flag_map.h"
#include "walk_i_d_map.h"
#include "zone_manager.h"

#include "../public/inlines.h"
#include "../visual/vis_utils.h"
#include "../visual/i_render.h"

extern IRender* g_pRender;

extern void get_pos(IVisBase* pVisBase, size_t collide_per_unit, int row, int col, float angleY, float& gx, float& gz);
extern void get_local_pos(IVisBase* pVisual, size_t collide_per_unit, float gx, float gz, float angleY, int& row, int& col);
// CDynamicWalkable

CDynamicWalkable::CDynamicWalkable(Terrain* pTerrain, IVisBase* pVisBase,
	const char* model_file_name, const char* file_name, size_t rows, size_t cols)
{
	Assert(pTerrain != NULL);
	Assert(pVisBase != NULL);
	Assert(model_file_name != NULL);
	Assert(file_name != NULL);

	m_pTerrain = pTerrain;
	m_nRows = rows;
	m_nCols = cols;
	m_nFloorCount = 0;
	m_bReady = false;
	m_bLoaded = false;
    m_bAsync = true;
    m_bLoading = false;

	m_strModelFileName = model_file_name;
	m_strFileName = file_name;
	m_fHeight = pVisBase->GetPositionY();
	m_fAngleY = pVisBase->GetAngleY();
	m_fRadius = pVisBase->GetRadius();
	m_fBoxSizeX = pVisBase->GetBoxSizeX();
	m_fBoxSizeZ = pVisBase->GetBoxSizeZ();

	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		m_pFloorHeights[i] = NULL;
		m_pSpaceHeights[i] = NULL;
		m_pFloorMarkers[i] = NULL;
	}

    m_pDynamicWalkLoader = NULL;
}

CDynamicWalkable::~CDynamicWalkable()
{
	ReleaseAll();
}

void CDynamicWalkable::ReleaseAll()
{
	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (m_pFloorHeights[i])
		{
			CORE_DELETE(m_pFloorHeights[i]);
			m_pFloorHeights[i] = NULL;
		}

		if (m_pSpaceHeights[i])
		{
			CORE_DELETE(m_pSpaceHeights[i]);
			m_pSpaceHeights[i] = NULL;
		}

		if (m_pFloorMarkers[i])
		{
			CORE_DELETE(m_pFloorMarkers[i]);
			m_pFloorMarkers[i] = NULL;
		}
	}

    SAFE_RELEASE(m_pDynamicWalkLoader);

	m_nFloorCount = 0;
}

result_string CDynamicWalkable::GetWalkFileName() const
{
	char wfname[256];
	SafeSprintf(wfname, 256, "%s_%d_%d", m_strFileName.c_str(), m_nRows, m_nCols);

	result_string res = wfname;
	return res;
}

bool CDynamicWalkable::BuildFloorHeight(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size,
										int floor)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);
	Assert((size_t)floor < MAX_FLOOR_NUM);

	m_nFloorCount = floor + 1;

	if (m_pFloorHeights[floor])
	{
		CORE_DELETE(m_pFloorHeights[floor]);
	}

	m_pFloorHeights[floor] = CORE_NEW(CWalkHeightMap);

	return m_pFloorHeights[floor]->BuildCompress(rows, cols, pdata, size);
}

bool CDynamicWalkable::BuildSpaceHeight(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size,
										int floor)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);
	Assert((size_t)floor < MAX_FLOOR_NUM);

	if (m_pSpaceHeights[floor])
	{
		CORE_DELETE(m_pSpaceHeights[floor]);
	}

	m_pSpaceHeights[floor] = CORE_NEW(CWalkHeightMap);

	return m_pSpaceHeights[floor]->BuildCompress(rows, cols, pdata, size);
}

bool CDynamicWalkable::BuildFloorMarker(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size,
										int floor)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);
	Assert((size_t)floor < MAX_FLOOR_NUM);

	if (m_pFloorMarkers[floor])
	{
		CORE_DELETE(m_pFloorMarkers[floor]);
	}

	m_pFloorMarkers[floor] = CORE_NEW(CWalkFlagMap);

	return m_pFloorMarkers[floor]->BuildCompress(rows, cols, pdata, size);
}

bool CDynamicWalkable::Load()
{
	if (m_bLoaded || m_bLoading)
	{
		return true;
	}

	char full_file_name[512];
	SafeSprintf(full_file_name, 512, "%s%s%s%s\\%s.walk", 
		m_pTerrain->GetCore()->GetResourcePath(), m_pTerrain->GetAppendPath(), 
		m_pTerrain->GetDynamicWalkablePath(), m_strModelFileName.c_str(), 
		m_strFileName.c_str());

    if (!m_pDynamicWalkLoader)
    {
        m_pDynamicWalkLoader = CDynamicWalkLoader::NewInstance();
    }

	m_pDynamicWalkLoader->SetFileName(full_file_name);
	m_pDynamicWalkLoader->SetOwner(this);

    if (m_bAsync)
    {
        // 请求线程加载
        if (!g_pRender->RequestLoadResource(0, m_pDynamicWalkLoader))
        {
            CORE_DELETE(m_pDynamicWalkLoader);
            m_pDynamicWalkLoader = NULL;
            return false;
        }

        m_bLoading = true;
        m_bLoaded = false;
        m_bReady = false;
    }
    else
    {
	    if (!m_pDynamicWalkLoader->Load(false))
	    {
		    return false;
	    }
    	
	    if (!m_pDynamicWalkLoader->Create(false))
	    {
		    return false;
	    }

	    m_bLoaded = true;
	    m_bReady = true;

	    UpdateEntryPoints();
    }

	return true;
}

bool CDynamicWalkable::Unload()
{
	if (!m_bLoaded)
	{
		return true;
	}

	for (size_t i = 0; i < m_nFloorCount; ++i)
	{
		m_EntryPoints[i].clear();
	}

	m_bLoaded = false;
	m_bReady = false;
	return true;
}

bool CDynamicWalkable::EndCreate(int category, bool succeed)
{
    m_bLoading = false;
    m_bLoaded = true;
    m_bReady = succeed;

    if (m_bReady)
    {
        UpdateEntryPoints();

        ICore* pCore = g_pRender->GetCore();
        for (size_t i = 0; i < m_DynamicVisualArr.size(); ++i)
        {
            IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(m_DynamicVisualArr[i]);
            InsertDynamicWalkable(pVisBase);
        }
    }

    SAFE_RELEASE(m_pDynamicWalkLoader);

    return true;
}

bool CDynamicWalkable::AddDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	if (!Load())
	{
		return false;
	}

	for (size_t i = 0; i < m_DynamicVisualArr.size(); ++i)
	{
		if (m_DynamicVisualArr[i] == pVisBase->GetID())
		{
			return false;
		}
	}

	m_DynamicVisualArr.push_back(pVisBase->GetID());

    if (m_bReady)
    {
        if (!InsertDynamicWalkable(pVisBase))
        {
            return false;
        }
    }

	return true;
}

bool CDynamicWalkable::DeleteDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	for (size_t i = 0; i < m_DynamicVisualArr.size(); ++i)
	{
		if (m_DynamicVisualArr[i] == pVisBase->GetID())
		{
			m_DynamicVisualArr.remove(i);
			RemoveDynamicWalkable(pVisBase);

			if (0 == m_DynamicVisualArr.size())
			{
				Unload();
			}

			return true;
		}
	}

	return false;
}

bool CDynamicWalkable::CanInsert(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	float min_x = pVisBase->GetCenterX() - pVisBase->GetRadius();
	float max_x = pVisBase->GetCenterX() + pVisBase->GetRadius();
	float min_z = pVisBase->GetCenterZ() - pVisBase->GetRadius();
	float max_z = pVisBase->GetCenterZ() + pVisBase->GetRadius();

	CTerrainZone* pZone_Min = m_pTerrain->GetZoneManager()->GetInZone(min_x, min_z);

	if (NULL == pZone_Min)
	{
		return false;
	}

	CTerrainZone* pZone_Max = m_pTerrain->GetZoneManager()->GetInZone(max_x, max_z);

	if (NULL == pZone_Max)
	{
		return false;
	}

	int min_col = VisUtil_FloatToInt((min_x - pZone_Min->GetLeft()) / m_pTerrain->GetCollideUnitSize());
	int min_row = VisUtil_FloatToInt((min_z - pZone_Min->GetTop()) / m_pTerrain->GetCollideUnitSize());
	int max_col = VisUtil_FloatToInt((max_x - pZone_Max->GetLeft()) / m_pTerrain->GetCollideUnitSize());
	int max_row = VisUtil_FloatToInt((max_z - pZone_Max->GetTop()) / m_pTerrain->GetCollideUnitSize());

	float fStep = m_pTerrain->GetUnitSize() / m_pTerrain->GetCollidePerUnit();
	float fixed_min_x = pZone_Min->GetLeft() + (min_col + 0.5f) * fStep;
	float fixed_min_z = pZone_Min->GetTop() + (min_row + 0.5f) * fStep;
	float fixed_max_x = pZone_Max->GetLeft() + (max_col + 0.5f) * fStep;
	float fixed_max_z = pZone_Max->GetTop() + (max_row + 0.5f) * fStep;

	for (size_t f = 0; f < m_nFloorCount; ++f)
	{
		for (float x = fixed_min_x; x <= fixed_max_x; x += fStep)
		{
			for (float z = fixed_min_z; z <= fixed_max_z; z += fStep)
			{
				int row;
				int col;

				get_local_pos(pVisBase, m_pTerrain->GetCollidePerUnit(), x, z, GetAngleY(), row, col);

				if (row < 0 || row >= (int)m_nRows || col < 0 || col >= (int)m_nCols)
				{
					continue;
				}

				CTerrainZone* pZone = m_pTerrain->GetZoneManager()->GetInZone(x, z);

				if (NULL == pZone)
				{
					return false;
				}

				CTerrainWalkable* pWalkable = pZone->GetWalkable();

				if (!pWalkable->GetReady())
				{
					if (m_pTerrain->GetDesignMode())
					{
						return true;
					}

					return false;
				}

				int zcol = VisUtil_FloatToInt((x - pZone->GetLeft()) / m_pTerrain->GetCollideUnitSize());
				int zrow = VisUtil_FloatToInt((z - pZone->GetTop()) / m_pTerrain->GetCollideUnitSize());

				CWalkHeightMap* pHeightMap = pWalkable->GetDynamicFloorHeightMap(f);

				if (NULL == pHeightMap)
				{
					break;
				}

				float floor_height = pHeightMap->GetValue(zrow, zcol);

				if (!height_is_null(floor_height))
				{
					return false;
				}

				CWalkHeightMap* pSpaceMap = pWalkable->GetDynamicSpaceHeightMap(f);

				if (NULL == pSpaceMap)
				{
					break;
				}

				float space_height = pSpaceMap->GetValue(zrow, zcol);

				if (!height_is_null(space_height))
				{
					return false;
				}

				CWalkFlagMap* pMarkerMap = pWalkable->GetDynamicFloorMarkerMap(f);

				if (NULL == pMarkerMap)
				{
					break;
				}

				unsigned int floor_marker = pMarkerMap->GetValue(zrow, zcol);

				if (floor_marker != 0)
				{
					return false;
				}

				CWalkIDMap* pIdMap = pWalkable->GetDynamicFloorIDMap(f);

				if (NULL == pIdMap)
				{
					break;
				}

				const PERSISTID& id = pIdMap->GetValue(zrow, zcol);

				if (id != PERSISTID())
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool CDynamicWalkable::InsertDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	if (!CanInsert(pVisBase))
	{
		return false;
	}

	float fStep = m_pTerrain->GetUnitSize() / m_pTerrain->GetCollidePerUnit();

	float min_x = pVisBase->GetCenterX() - pVisBase->GetRadius();
	float max_x = pVisBase->GetCenterX() + pVisBase->GetRadius();
	float min_z = pVisBase->GetCenterZ() - pVisBase->GetRadius();
	float max_z = pVisBase->GetCenterZ() + pVisBase->GetRadius();

	CTerrainZone* pZone_Min = m_pTerrain->GetZoneManager()->GetInZone(min_x, min_z);

	if (NULL == pZone_Min)
	{
		return false;
	}

	CTerrainZone* pZone_Max = m_pTerrain->GetZoneManager()->GetInZone(max_x, max_z);

	if (NULL == pZone_Max)
	{
		return false;
	}

	int min_col = VisUtil_FloatToInt((min_x - pZone_Min->GetLeft()) / m_pTerrain->GetCollideUnitSize());
	int min_row = VisUtil_FloatToInt((min_z - pZone_Min->GetTop()) / m_pTerrain->GetCollideUnitSize());
	int max_col = VisUtil_FloatToInt((max_x - pZone_Max->GetLeft()) / m_pTerrain->GetCollideUnitSize());
	int max_row = VisUtil_FloatToInt((max_z - pZone_Max->GetTop()) / m_pTerrain->GetCollideUnitSize());

	float fixed_min_x = pZone_Min->GetLeft() + (min_col + 0.5f) * fStep;
	float fixed_min_z = pZone_Min->GetTop() + (min_row + 0.5f) * fStep;
	float fixed_max_x = pZone_Max->GetLeft() + (max_col + 0.5f) * fStep;
	float fixed_max_z = pZone_Max->GetTop() + (max_row + 0.5f) * fStep;

	for (size_t f = 0; f < m_nFloorCount; ++f)
	{
		for (float x = fixed_min_x; x <= fixed_max_x; x += fStep)
		{
			for (float z = fixed_min_z; z <= fixed_max_z; z += fStep)
			{
				int row;
				int col;

				get_local_pos(pVisBase, m_pTerrain->GetCollidePerUnit(), x, z, GetAngleY(), row, col);

				if (row < 0 || row >= (int)m_nRows || col < 0 || col >= (int)m_nCols)
				{
					continue;
				}

				CTerrainZone* pZone = m_pTerrain->GetZoneManager()->GetInZone(x, z);

				if (NULL == pZone)
				{
					return false;
				}

				CTerrainWalkable* pWalkable = pZone->GetWalkable();

				if (!pWalkable->GetReady())
				{
					if (m_pTerrain->GetDesignMode())
					{
						return true;
					}

					return false;
				}

				int zcol = VisUtil_FloatToInt((x - pZone->GetLeft()) / m_pTerrain->GetCollideUnitSize());
				int zrow = VisUtil_FloatToInt((z - pZone->GetTop()) / m_pTerrain->GetCollideUnitSize());

				// 更新层高度
				float floor_height = GetFloorHeight(pVisBase->GetPositionY(), row, col, f);

				CWalkHeightMap* pHeightMap = pWalkable->GetDynamicFloorHeightMap(f);
				
				if (NULL == pHeightMap)
				{
					pWalkable->UpdateDynamicFloorHeight(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pHeightMap = pWalkable->GetDynamicFloorHeightMap(f);

					if (NULL == pHeightMap)
					{
						return false;
					}

                    pHeightMap->ResetValue();
				}

				//pHeightMap->SetValue(zrow, zcol, floor_height);

				// 更新层空间高度
				float space_height = GetFloorSpace(row, col, f);

				CWalkHeightMap* pSpaceMap = pWalkable->GetDynamicSpaceHeightMap(f);

				if (NULL == pSpaceMap)
				{
					pWalkable->UpdateDynamicSpaceHeight(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pSpaceMap = pWalkable->GetDynamicSpaceHeightMap(f);

					if (NULL == pSpaceMap)
					{
						return false;
					}

                    pSpaceMap->ResetValue();
				}

				//pSpaceMap->SetValue(zrow, zcol, space_height);

				// 更新层标记
				unsigned int floor_marker = GetFloorMarker(row, col, f);

				CWalkFlagMap* pMarkerMap = pWalkable->GetDynamicFloorMarkerMap(f);

				if (NULL == pMarkerMap)
				{
					pWalkable->UpdateDynamicFloorMarker(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pMarkerMap = pWalkable->GetDynamicFloorMarkerMap(f);

					if (NULL == pMarkerMap)
					{
						return false;
					}
				}

				//pMarkerMap->SetValue(zrow, zcol, floor_marker);

				// 更新层ID
				PERSISTID id = pVisBase->GetID();

				CWalkIDMap* pIDMap = pWalkable->GetDynamicFloorIDMap(f);

				if (NULL == pIDMap)
				{
					pWalkable->UpdateDynamicFloorID(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pIDMap = pWalkable->GetDynamicFloorIDMap(f);

					if (NULL == pIDMap)
					{
						return false;
					}
				}
				
				//pIDMap->SetValue(zrow, zcol, id);

				if (f == 0)
				{
					int floor = pWalkable->GetApexStaticFloor(zrow, zcol);
					float apex_height = pWalkable->GetStaticFloorHeight(zrow, zcol, floor);
					if (height_is_null(apex_height))
					{
						apex_height = m_pTerrain->GetGroundHeight(x, z);
					}
					space_height = floor_height - apex_height;
					floor_height = apex_height;
					floor_marker = pWalkable->GetStaticFloorMarker(zrow, zcol, floor);
					if (space_height < 1.8f)
					{
						floor_marker &= ~FLOOR_MARKER_MOVE;
						floor_marker &= ~FLOOR_MARKER_STAND;
					}
					id = m_pTerrain->GetID();
				}

				if ((floor_marker & FLOOR_MARKER_EXISTS) == 0)
				{
					id = PERSISTID();
				}

				pHeightMap->SetValue(zrow, zcol, floor_height);
				pSpaceMap->SetValue(zrow, zcol, space_height);
				pMarkerMap->SetValue(zrow, zcol, floor_marker);
				pIDMap->SetValue(zrow, zcol, id);
			}
		}
	}
	
	return true;
}

bool CDynamicWalkable::RemoveDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	float min_x = pVisBase->GetCenterX() - pVisBase->GetRadius();
	float max_x = pVisBase->GetCenterX() + pVisBase->GetRadius();
	float min_z = pVisBase->GetCenterZ() - pVisBase->GetRadius();
	float max_z = pVisBase->GetCenterZ() + pVisBase->GetRadius();

	CTerrainZone* pZone_Min = m_pTerrain->GetZoneManager()->GetInZone(min_x, min_z);

	if (NULL == pZone_Min)
	{
		return false;
	}

	CTerrainZone* pZone_Max = m_pTerrain->GetZoneManager()->GetInZone(max_x, max_z);

	if (NULL == pZone_Max)
	{
		return false;
	}

	int min_col = VisUtil_FloatToInt((min_x - pZone_Min->GetLeft()) / m_pTerrain->GetCollideUnitSize());
	int min_row = VisUtil_FloatToInt((min_z - pZone_Min->GetTop()) / m_pTerrain->GetCollideUnitSize());
	int max_col = VisUtil_FloatToInt((max_x - pZone_Max->GetLeft()) / m_pTerrain->GetCollideUnitSize());
	int max_row = VisUtil_FloatToInt((max_z - pZone_Max->GetTop()) / m_pTerrain->GetCollideUnitSize());

	float fStep = m_pTerrain->GetUnitSize() / m_pTerrain->GetCollidePerUnit();
	float fixed_min_x = pZone_Min->GetLeft() + (min_col + 0.5f) * fStep;
	float fixed_min_z = pZone_Min->GetTop() + (min_row + 0.5f) * fStep;
	float fixed_max_x = pZone_Max->GetLeft() + (max_col + 0.5f) * fStep;
	float fixed_max_z = pZone_Max->GetTop() + (max_row + 0.5f) * fStep;

	for (size_t f = 0; f < m_nFloorCount; ++f)
	{
		for (float x = fixed_min_x; x <= fixed_max_x; x += fStep)
		{
			for (float z = fixed_min_z; z <= fixed_max_z; z += fStep)
			{
				int row;
				int col;

				get_local_pos(pVisBase, m_pTerrain->GetCollidePerUnit(), x, z, GetAngleY(), row, col);

				if (row < 0 || row >= (int)m_nRows || col < 0 || col >= (int)m_nCols)
				{
					continue;
				}

				CTerrainZone* pZone = m_pTerrain->GetZoneManager()->GetInZone(x, z);

				if (NULL == pZone)
				{
					return false;
				}

				CTerrainWalkable* pWalkable = pZone->GetWalkable();

				if (!pWalkable->GetReady())
				{
					if (m_pTerrain->GetDesignMode())
					{
						return true;
					}

					return false;
				}

				int zcol = VisUtil_FloatToInt((x - pZone->GetLeft()) / m_pTerrain->GetCollideUnitSize());
				int zrow = VisUtil_FloatToInt((z - pZone->GetTop()) / m_pTerrain->GetCollideUnitSize());

				// 更新层高度
				float floor_height = HEIGHT_NULL;

				CWalkHeightMap* pHeightMap = pWalkable->GetDynamicFloorHeightMap(f);

				if (NULL == pHeightMap)
				{
					pWalkable->UpdateDynamicFloorHeight(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pHeightMap = pWalkable->GetDynamicFloorHeightMap(f);

					if (NULL == pHeightMap)
					{
						return false;
					}
				}

				pHeightMap->SetValue(zrow, zcol, floor_height);

				// 更新层空间高度
				float space_height = HEIGHT_NULL;

				CWalkHeightMap* pSpaceMap = pWalkable->GetDynamicSpaceHeightMap(f);

				if (NULL == pSpaceMap)
				{
					pWalkable->UpdateDynamicSpaceHeight(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pSpaceMap = pWalkable->GetDynamicSpaceHeightMap(f);

					if (NULL == pSpaceMap)
					{
						return false;
					}
				}

				pSpaceMap->SetValue(zrow, zcol, space_height);

				// 更新层标记
				unsigned int floor_marker = 0;

				CWalkFlagMap* pMarkerMap = pWalkable->GetDynamicFloorMarkerMap(f);

				if (NULL == pMarkerMap)
				{
					pWalkable->UpdateDynamicFloorMarker(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pMarkerMap = pWalkable->GetDynamicFloorMarkerMap(f);

					if (NULL == pMarkerMap)
					{
						return false;
					}
				}

				pMarkerMap->SetValue(zrow, zcol, floor_marker);

				// 更新层ID
				PERSISTID id = PERSISTID();

				CWalkIDMap* pIDMap = pWalkable->GetDynamicFloorIDMap(f);

				if (NULL == pIDMap)
				{
					pWalkable->UpdateDynamicFloorID(f, pWalkable->GetRows(),
						pWalkable->GetCols(), NULL);

					pIDMap = pWalkable->GetDynamicFloorIDMap(f);

					if (NULL == pIDMap)
					{
						return false;
					}
				}

				pIDMap->SetValue(zrow, zcol, id);
			}
		}
	}

	return true;
}

unsigned int CDynamicWalkable::GetFloorMarker(size_t row, size_t col, 
											  int floor) const
{
	Assert((size_t)floor < MAX_FLOOR_NUM);

	if (NULL == m_pFloorMarkers[floor])
	{
		return 0;
	}

	return m_pFloorMarkers[floor]->GetDynamicValue(row, col);
}

float CDynamicWalkable::GetApexHeight(float height, size_t row, size_t col) const
{
	// 层数量
	int floor_num = GetFloorCount();

	for (int i = floor_num - 1; i >= 0; --i)
	{
		unsigned int floor_marker = GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_EXISTS) != 0)
		{
			return GetFloorHeight(height, row, col, i);
		}
	}

	return height;
}

int CDynamicWalkable::GetApexFloor(size_t row, size_t col)
{
	// 层数量
	int floor_num = GetFloorCount();

	for (int i = floor_num - 1; i >= 0; --i)
	{
		unsigned int floor_marker = GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_EXISTS) != 0)
		{
			return i;
		}
	}

	return 0;
}

float CDynamicWalkable::GetBottomHeight(float height,
										size_t row, size_t col) const
{
	// 层数量
	int floor_num = GetFloorCount();

	for (int i = 0; i < floor_num; ++i)
	{
		unsigned int floor_marker = GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_MOVE) != 0)
		{
			return GetFloorHeight(height, row, col, i);
		}
	}

	return height;
}

int CDynamicWalkable::GetBottomFloor(size_t row, size_t col)
{
	// 层数量
	int floor_num = GetFloorCount();

	for (int i = 0; i < floor_num; ++i)
	{
		unsigned int floor_marker = GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_MOVE) != 0)
		{
			return i;
		}
	}

	return 0;
}

float CDynamicWalkable::GetFloorHeight(float height, size_t row, size_t col,
									   int floor) const
{
	Assert((size_t)floor < MAX_FLOOR_NUM);

	if (NULL == m_pFloorHeights[floor])
	{
		return HEIGHT_NULL;
	}

	return m_pFloorHeights[floor]->GetValue(row, col) + height - m_fHeight;
}

float CDynamicWalkable::GetFloorSpace(size_t row, size_t col, int floor) const
{
	Assert((size_t)floor < MAX_FLOOR_NUM);

	if (NULL == m_pSpaceHeights[floor])
	{
		return HEIGHT_NULL;
	}

	return m_pSpaceHeights[floor]->GetValue(row, col);
}

bool CDynamicWalkable::GetWallExists(float height, size_t row, size_t col,
									 float y) const
{
	int floor_num = m_nFloorCount;

	for (int i = 0; i < floor_num; ++i)
	{
		unsigned int floor_marker = m_pFloorMarkers[i]->GetValue(row, col);

		if ((floor_marker & FLOOR_MARKER_WALL) != 0)
		{
			if (y <= GetFloorHeight(height, row, col, i))
			{
				return true;
			}
		}
	}

	return false;
}

bool CDynamicWalkable::UpdateFloorHeight(int floor, size_t rows, size_t cols,
										 float* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pValues != NULL);

	if (m_pFloorHeights[floor])
	{
		CORE_DELETE(m_pFloorHeights[floor]);
	}

	m_pFloorHeights[floor] = CORE_NEW(CWalkHeightMap);
	m_pFloorHeights[floor]->Create(rows, cols, pValues);

	return true;
}

bool CDynamicWalkable::UpdateSpaceHeight(int floor, size_t rows, size_t cols, 
										 float* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pValues != NULL);

	if (m_pSpaceHeights[floor])
	{
		CORE_DELETE(m_pSpaceHeights[floor]);
	}

	m_pSpaceHeights[floor] = CORE_NEW(CWalkHeightMap);
	m_pSpaceHeights[floor]->Create(rows, cols, pValues);

	return true;
}

bool CDynamicWalkable::UpdateFloorMarker(int floor, size_t rows, size_t cols, 
										 unsigned char* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pValues != NULL);

	if (m_nFloorCount <= size_t(floor))
	{
		m_nFloorCount = floor + 1;
	}

	if (m_pFloorMarkers[floor])
	{
		CORE_DELETE(m_pFloorMarkers[floor]);
	}

	m_pFloorMarkers[floor] = CORE_NEW(CWalkFlagMap);
	m_pFloorMarkers[floor]->Create(rows, cols, pValues);

	return true;
}

void CDynamicWalkable::GetEntryPoints(IVisBase* pVisBase, int floor, IVarList& result)
{
	Assert(NULL != pVisBase);
	Assert(floor >= 0 && floor < MAX_FLOOR_NUM);

	result.Clear();

	if (pVisBase)
	{
		bool bFind = false;
		for (size_t i = 0; i < m_DynamicVisualArr.size(); ++i)
		{
			if (m_DynamicVisualArr[i] == pVisBase->GetID())
			{
				bFind = true;
				break;
			}
		}

		if (bFind)
		{
			for (size_t j = 0; j < m_EntryPoints[floor].size(); ++j)
			{
				size_t row = m_EntryPoints[floor][j].row;
				size_t col = m_EntryPoints[floor][j].col;

				float gx = 0.0f;
				float gz = 0.0f;

				get_pos(pVisBase, m_pTerrain->GetCollidePerUnit(), row, col, GetAngleY(), gx, gz);

				FmVec3 entry_point;
				entry_point.x = gx;
				entry_point.y = GetFloorHeight(pVisBase->GetPositionY(), row, col, floor);
				entry_point.z = gz;
				
				result.AddFloat(entry_point.x);
				result.AddFloat(entry_point.y);
				result.AddFloat(entry_point.z);
			}
		}
	}
}

int CDynamicWalkable::GetEntryPointsCount(IVisBase* pVisBase, int floor)
{
	Assert(NULL != pVisBase);
	Assert(floor >= 0 && floor < MAX_FLOOR_NUM);

	if (pVisBase)
	{
		bool bFind = false;
		for (size_t i = 0; i < m_DynamicVisualArr.size(); ++i)
		{
			if (m_DynamicVisualArr[i] == pVisBase->GetID())
			{
				bFind = true;
				break;
			}
		}

		if (bFind)
		{
			return m_EntryPoints[floor].size();
		}
	}

	return 0;
}

bool CDynamicWalkable::UpdateEntryPoints()
{
	if (m_bLoaded)
	{
		for (size_t f = 0; f < m_nFloorCount; ++f)
		{
			for (size_t r = 0; r < m_nRows; ++r)
			{
				for (size_t c = 0; c < m_nCols; ++c)
				{
					if ((GetFloorMarker(r, c, f) & FLOOR_MARKER_EDGE) != 0)
					{
						entry_point_t entry_point = {r, c};
						m_EntryPoints[f].push_back(entry_point);
					}
				}
			}
		}

		return true;
	}

	return false;
}
