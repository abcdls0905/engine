//--------------------------------------------------------------------
// 文件名:		terrain_walkable.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年8月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_walkable.h"
#include "terrain.h"
#include "walk_height_map.h"
#include "walk_flag_map.h"
#include "walk_i_d_map.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../visual/i_image_access.h"

// CTerrainWalkable

CTerrainWalkable::CTerrainWalkable(Terrain* pTerrain, CTerrainZone* pZone,
								   size_t rows, size_t cols)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);
	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_nRows = rows;
	m_nCols = cols;
	m_nFloorCount = 0;
	m_nDynamicFloorCount = 0;
	m_bReady = false;
	m_pGroundHeight = NULL;
	m_pWaterHeight = NULL;
	m_pWalkMarker = NULL;

	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		m_pFloorHeights[i] = NULL;
		m_pSpaceHeights[i] = NULL;
		m_pFloorMarkers[i] = NULL;

		m_pDynamicFloorHeights[i] = NULL;
		m_pDynamicSpaceHeights[i] = NULL;
		m_pDynamicFloorMarkers[i] = NULL;
		m_pDynamicFloorIDs[i] = NULL;
	}
}

CTerrainWalkable::~CTerrainWalkable()
{
	ReleaseAll();
}

void CTerrainWalkable::ReleaseAll()
{
	if (m_pGroundHeight)
	{
		CORE_DELETE(m_pGroundHeight);
		m_pGroundHeight = NULL;
	}

	if (m_pWaterHeight)
	{
		CORE_DELETE(m_pWaterHeight);
		m_pWaterHeight = NULL;
	}

	if (m_pWalkMarker)
	{
		CORE_DELETE(m_pWalkMarker);
		m_pWalkMarker = NULL;
	}

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

		if (m_pDynamicFloorHeights[i])
		{
			CORE_DELETE(m_pDynamicFloorHeights[i]);
			m_pDynamicFloorHeights[i] = NULL;
		}

		if (m_pDynamicSpaceHeights[i])
		{
			CORE_DELETE(m_pDynamicSpaceHeights[i]);
			m_pDynamicSpaceHeights[i] = NULL;
		}

		if (m_pDynamicFloorMarkers[i])
		{
			CORE_DELETE(m_pDynamicFloorMarkers[i]);
			m_pDynamicFloorMarkers[i] = NULL;
		}

		if (m_pDynamicFloorIDs[i])
		{
			CORE_DELETE(m_pDynamicFloorIDs[i]);
			m_pDynamicFloorIDs[i] = NULL;
		}
	}

	m_nFloorCount = 0;
	m_nDynamicFloorCount = 0;
}

float CTerrainWalkable::GetWaterHeight(size_t row, size_t col)
{
	if (NULL == m_pWaterHeight)
	{
		return HEIGHT_NULL;
	}

	return m_pWaterHeight->GetValue(row, col);
}

unsigned int CTerrainWalkable::GetWalkMarker(size_t row, size_t col)
{
	if (NULL == m_pWalkMarker)
	{
		return 0;
	}

	return m_pWalkMarker->GetValue(row, col);
}

unsigned int CTerrainWalkable::GetFloorMarker(size_t row, size_t col, 
											  int floor)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);

	if ((size_t)floor < m_nFloorCount)
	{
		if (NULL == m_pFloorMarkers[floor])
		{
			return 0;
		}

		if (floor == GetApexStaticFloor(row, col))
		{
			unsigned int floor_marker = GetDynamicFloorMarker(row, col, 0);

			if ((floor_marker & FLOOR_MARKER_EXISTS) != 0)
			{
				return floor_marker;
			}
		}

		return m_pFloorMarkers[floor]->GetValue(row, col);
	}
	else if (floor < GetFloorCount())
	{
		if (NULL == m_pDynamicFloorMarkers[floor - m_nFloorCount + 1])
		{
			return 0;
		}

		return m_pDynamicFloorMarkers[floor - m_nFloorCount + 1]->GetValue(row, col);
	}
	else
	{
		return 0;
	}
}

float CTerrainWalkable::GetFloorHeight(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nFloorCount)
	{
		if (NULL == m_pFloorHeights[floor])
		{
			return HEIGHT_NULL;
		}

		if (floor == GetApexStaticFloor(row, col) &&
			(GetDynamicFloorMarker(row, col, 0) & FLOOR_MARKER_EXISTS) != 0)
		{
			return GetDynamicFloorHeight(row, col, 0);
		}

		return m_pFloorHeights[floor]->GetValue(row, col);
	}
	else if (floor < GetFloorCount())
	{
		if (NULL == m_pDynamicFloorHeights[floor - m_nFloorCount + 1])
		{
			return HEIGHT_NULL;
		}

		return m_pDynamicFloorHeights[floor - m_nFloorCount + 1]->GetValue(row, col);
	}
	else
	{
		return HEIGHT_NULL;
	}
}

float CTerrainWalkable::GetFloorSpace(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nFloorCount)
	{
		if (NULL == m_pSpaceHeights[floor])
		{
			return HEIGHT_NULL;
		}

		if (floor == GetApexStaticFloor(row, col) &&
			(GetDynamicFloorMarker(row, col, 0) & FLOOR_MARKER_EXISTS) != 0)
		{
			return GetDynamicFloorSpace(row, col, 0);
		}

		return m_pSpaceHeights[floor]->GetValue(row, col);
	}
	else if (floor < GetFloorCount())
	{
		if (NULL == m_pDynamicSpaceHeights[floor - m_nFloorCount + 1])
		{
			return HEIGHT_NULL;
		}

		return m_pDynamicSpaceHeights[floor - m_nFloorCount + 1]->GetValue(row, col);
	}
	else
	{
		return HEIGHT_NULL;
	}
}

bool CTerrainWalkable::GetWallExists(size_t row, size_t col, float y)
{
	int floor_num = GetFloorCount();

	for (int i = 0; i < floor_num; ++i)
	{
		unsigned int floor_marker = GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_WALL) != 0)
		{
			if (y <= GetFloorHeight(row, col, i))
			{
				return true;
			}
		}
	}

	return false;
}

IVisBase* CTerrainWalkable::GetFloorObject(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nFloorCount)
	{
		return m_pTerrain;
	}
	else if (floor < GetFloorCount())
	{
		if (NULL == m_pDynamicFloorIDs[floor - m_nFloorCount + 1])
		{
			return NULL;
		}

		const PERSISTID& id = m_pDynamicFloorIDs[floor - m_nFloorCount + 1]->GetValue(row, col);

		IVisBase* pVisBase = (IVisBase*)m_pTerrain->GetCore()->GetEntity(id);

		return pVisBase;
	}
	else
	{
		return NULL;
	}
}

int CTerrainWalkable::GetApexStaticFloor(size_t row, size_t col)
{
	// 层数量
	int floor_num = m_nFloorCount;

	for (int i = floor_num - 1; i >= 0; --i)
	{
		unsigned int floor_marker = GetStaticFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_EXISTS) != 0)
		{
			return i;
		}
	}

	return 0;
}

float CTerrainWalkable::GetStaticFloorHeight(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nFloorCount)
	{
		if (NULL == m_pFloorHeights[floor])
		{
			return HEIGHT_NULL;
		}

		return m_pFloorHeights[floor]->GetValue(row, col);
	}

	return HEIGHT_NULL;
}

float CTerrainWalkable::GetStaticFloorSpace(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nFloorCount)
	{
		if (NULL == m_pSpaceHeights[floor])
		{
			return HEIGHT_NULL;
		}

		return m_pSpaceHeights[floor]->GetValue(row, col);
	}

	return HEIGHT_NULL;
}

unsigned int CTerrainWalkable::GetStaticFloorMarker(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nFloorCount)
	{
		if (NULL == m_pFloorMarkers[floor])
		{
			return 0;
		}

		return m_pFloorMarkers[floor]->GetValue(row, col);
	}

	return 0;
}

float CTerrainWalkable::GetDynamicFloorHeight(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nDynamicFloorCount)
	{
		if (NULL == m_pDynamicFloorHeights[floor])
		{
			return HEIGHT_NULL;
		}

		return m_pDynamicFloorHeights[floor]->GetValue(row, col);
	}

	return HEIGHT_NULL;
}

float CTerrainWalkable::GetDynamicFloorSpace(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nDynamicFloorCount)
	{
		if (NULL == m_pDynamicSpaceHeights[floor])
		{
			return HEIGHT_NULL;
		}

		return m_pDynamicSpaceHeights[floor]->GetValue(row, col);
	}

	return HEIGHT_NULL;
}

unsigned int CTerrainWalkable::GetDynamicFloorMarker(size_t row, size_t col, int floor)
{
	Assert((floor >= 0) && (floor < MAX_FLOOR_NUM));

	if ((size_t)floor < m_nDynamicFloorCount)
	{
		if (NULL == m_pDynamicFloorMarkers[floor])
		{
			return 0;
		}

		return m_pDynamicFloorMarkers[floor]->GetValue(row, col);
	}

	return 0;
}

bool CTerrainWalkable::SaveWalkMap(const char* file_name)
{
	Assert(file_name != NULL);

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s.ground.tga", file_name);
	SaveHeightMap(m_pGroundHeight, fname);
	SafeSprintf(fname, sizeof(fname), "%s.water.tga", file_name);
	SaveHeightMap(m_pWaterHeight, fname);
	SafeSprintf(fname, sizeof(fname), "%s.walk.tga", file_name);
	SaveFlagMap(m_pWalkMarker, fname);

	for (int i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (m_pFloorHeights[i])
		{
			SafeSprintf(fname, sizeof(fname), "%s.floor%d.tga", 
				file_name, i);
			SaveHeightMap(m_pFloorHeights[i], fname);
		}

		if (m_pSpaceHeights[i])
		{
			SafeSprintf(fname, sizeof(fname), "%s.space%d.tga", 
				file_name, i);
			SaveHeightMap(m_pSpaceHeights[i], fname);
		}

		if (m_pFloorMarkers[i])
		{
			SafeSprintf(fname, sizeof(fname), "%s.marker%d.tga", 
				file_name, i);
			SaveFlagMap(m_pFloorMarkers[i], fname);
		}
	}

	return true;
}

bool CTerrainWalkable::SaveHeightMap(CWalkHeightMap* pHeight, 
									 const char* file_name)
{
	Assert(pHeight != NULL);
	Assert(file_name != NULL);


	IImageAccess* pImage = m_pTerrain->GetRender()->CreateImageAccess();

	pImage->SetFormat((unsigned int)m_nCols, (unsigned int)m_nRows, 32);

	size_t size = m_nRows * m_nCols;
	TAutoMem<unsigned int, 1, TCoreAlloc> auto_buf(size);
	unsigned int* data = auto_buf.GetBuffer();

	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			float val = pHeight->GetValue(row, col);

			if (height_is_null(val))
			{
				data[row * m_nCols + col] = 0xFFFFFFFF;
			}
			else
			{
				data[row * m_nCols + col] = *(unsigned int*)&val;
			}
		}
	}

	pImage->SetData((const unsigned char*)data, size * 4);

	if (!pImage->SaveFile(file_name))
	{
		pImage->Release();
		return false;
	}

	pImage->Release();

	return true;
}

bool CTerrainWalkable::SaveFlagMap(CWalkFlagMap* pFlagMap, 
								   const char* file_name)
{
	Assert(pFlagMap != NULL);
	Assert(file_name != NULL);

	IImageAccess* pImage = m_pTerrain->GetRender()->CreateImageAccess();

	pImage->SetFormat((unsigned int)m_nCols, (unsigned int)m_nRows, 8);

	size_t size = m_nRows * m_nCols;
	TAutoMem<unsigned char, 1, TCoreAlloc> auto_buf(size);
	unsigned char* data = auto_buf.GetBuffer();

	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			data[row * m_nCols + col] = pFlagMap->GetValue(row, col) << 4;
		}
	}

	pImage->SetData((const unsigned char*)data, size);

	if (!pImage->SaveFile(file_name))
	{
		pImage->Release();
		return false;
	}

	pImage->Release();

	return true;
}

bool CTerrainWalkable::UpdateGroundHeight(size_t rows, size_t cols, 
										  float* pValues)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pValues != NULL);

	if (m_pGroundHeight)
	{
		CORE_DELETE(m_pGroundHeight);
	}

	m_pGroundHeight = CORE_NEW(CWalkHeightMap);
	m_pGroundHeight->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::UpdateWaterHeight(size_t rows, size_t cols, 
										 float* pValues)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pValues != NULL);

	if (m_pWaterHeight)
	{
		CORE_DELETE(m_pWaterHeight);
	}

	m_pWaterHeight = CORE_NEW(CWalkHeightMap);
	m_pWaterHeight->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::UpdateFloorHeight(int floor, size_t rows, size_t cols, 
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

bool CTerrainWalkable::UpdateSpaceHeight(int floor, size_t rows, size_t cols, 
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

bool CTerrainWalkable::UpdateWalkMarker(size_t rows, size_t cols, 
										unsigned char* pValues)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pValues != NULL);

	if (m_pWalkMarker)
	{
		CORE_DELETE(m_pWalkMarker);
	}

	m_pWalkMarker = CORE_NEW(CWalkFlagMap);
	m_pWalkMarker->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::UpdateFloorMarker(int floor, size_t rows, size_t cols, 
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

bool CTerrainWalkable::UpdateDynamicFloorHeight(int floor, size_t rows,
												size_t cols, float* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	//Assert(pValues != NULL);

	if (m_nDynamicFloorCount <= size_t(floor))
	{
		m_nDynamicFloorCount = floor + 1;
	}

	if (m_pDynamicFloorHeights[floor])
	{
		CORE_DELETE(m_pDynamicFloorHeights[floor]);
	}

	m_pDynamicFloorHeights[floor] = CORE_NEW(CWalkHeightMap);
	m_pDynamicFloorHeights[floor]->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::UpdateDynamicSpaceHeight(int floor, size_t rows,
												size_t cols, float* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	//Assert(pValues != NULL);

	if (m_nDynamicFloorCount <= size_t(floor))
	{
		m_nDynamicFloorCount = floor + 1;
	}

	if (m_pDynamicSpaceHeights[floor])
	{
		CORE_DELETE(m_pDynamicSpaceHeights[floor]);
	}

	m_pDynamicSpaceHeights[floor] = CORE_NEW(CWalkHeightMap);
	m_pDynamicSpaceHeights[floor]->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::UpdateDynamicFloorMarker(int floor, size_t rows,
												size_t cols, unsigned char* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	//Assert(pValues != NULL);

	if (m_nDynamicFloorCount <= size_t(floor))
	{
		m_nDynamicFloorCount = floor + 1;
	}

	if (m_pDynamicFloorMarkers[floor])
	{
		CORE_DELETE(m_pDynamicFloorMarkers[floor]);
	}

	m_pDynamicFloorMarkers[floor] = CORE_NEW(CWalkFlagMap);
	m_pDynamicFloorMarkers[floor]->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::UpdateDynamicFloorID(int floor, size_t rows, size_t cols,
											PERSISTID* pValues)
{
	Assert(size_t(floor) < MAX_FLOOR_NUM);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	//Assert(pValues != NULL);

	if (m_nDynamicFloorCount <= size_t(floor))
	{
		m_nDynamicFloorCount = floor + 1;
	}

	if (m_pDynamicFloorIDs[floor])
	{
		CORE_DELETE(m_pDynamicFloorIDs[floor]);
	}

	m_pDynamicFloorIDs[floor] = CORE_NEW(CWalkIDMap);
	m_pDynamicFloorIDs[floor]->Create(rows, cols, pValues);

	return true;
}

bool CTerrainWalkable::BuildWaterHeight(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	if (m_pWaterHeight)
	{
		CORE_DELETE(m_pWaterHeight);
	}

	m_pWaterHeight = CORE_NEW(CWalkHeightMap);

	return m_pWaterHeight->BuildCompress(rows, cols, pdata, size);
}

bool CTerrainWalkable::BuildFloorHeight(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size, int floor)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);
	Assert(size_t(floor) < MAX_FLOOR_NUM);

	m_nFloorCount = floor + 1;

	if (m_pFloorHeights[floor])
	{
		CORE_DELETE(m_pFloorHeights[floor]);
	}

	m_pFloorHeights[floor] = CORE_NEW(CWalkHeightMap);

	return m_pFloorHeights[floor]->BuildCompress(rows, cols, pdata, size);
}

bool CTerrainWalkable::BuildSpaceHeight(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size, int floor)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);
	Assert(size_t(floor) < MAX_FLOOR_NUM);

	if (m_pSpaceHeights[floor])
	{
		CORE_DELETE(m_pSpaceHeights[floor]);
	}

	m_pSpaceHeights[floor] = CORE_NEW(CWalkHeightMap);

	return m_pSpaceHeights[floor]->BuildCompress(rows, cols, pdata, size);
}

bool CTerrainWalkable::BuildWalkMarker(size_t rows, size_t cols, 
									   unsigned char* pdata, size_t size)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);

	if (m_pWalkMarker)
	{
		CORE_DELETE(m_pWalkMarker);
	}

	m_pWalkMarker = CORE_NEW(CWalkFlagMap);

	return m_pWalkMarker->BuildCompress(rows, cols, pdata, size);
}

bool CTerrainWalkable::BuildFloorMarker(size_t rows, size_t cols, 
										unsigned char* pdata, size_t size, int floor)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(pdata != NULL);
	Assert(size_t(floor) < MAX_FLOOR_NUM);

	if (m_pFloorMarkers[floor])
	{
		CORE_DELETE(m_pFloorMarkers[floor]);
	}

	m_pFloorMarkers[floor] = CORE_NEW(CWalkFlagMap);

	return m_pFloorMarkers[floor]->BuildCompress(rows, cols, pdata, size);
}

