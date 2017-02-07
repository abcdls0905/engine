//--------------------------------------------------------------------
// 文件名:		terrain_height.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_height.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "zone_manager.h"
#include "../visual/vis_utils.h"

// CTerrainHeight

CTerrainHeight::CTerrainHeight(Terrain* pTerrain, CTerrainZone* pZone,
	size_t rows, size_t cols)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);

	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_nRows = rows;
	m_nCols = cols;
	m_fLeft = pZone->GetLeft();
	m_fTop = pZone->GetTop();
	m_fUnitSize = pTerrain->GetUnitSize();
	m_fInvUnitSize = 1.0F / m_fUnitSize;
	m_fMinY = 1e8;
	m_fMaxY = -1e8;
	m_pValues = NULL;
	m_pNormals = NULL;
	m_pBlockMinY = NULL;
	m_pBlockMaxY = NULL;
	m_nBlockScale = 0;
	m_nBlockDims = 0;
}

CTerrainHeight::~CTerrainHeight()
{
	ReleaseAll();
}

void CTerrainHeight::ReleaseAll()
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, sizeof(float) * m_nRows * m_nCols);
		m_pValues = NULL;
	}

	if (m_pNormals)
	{
		CORE_FREE(m_pNormals, sizeof(unsigned int) * m_nRows * m_nCols);
		m_pNormals = NULL;
	}

	if (m_pBlockMinY)
	{
		CORE_FREE(m_pBlockMinY, sizeof(float) * m_nBlockDims * m_nBlockDims);
		m_pBlockMinY = NULL;
	}
	
	if (m_pBlockMaxY)
	{
		CORE_FREE(m_pBlockMaxY, sizeof(float) * m_nBlockDims * m_nBlockDims);
		m_pBlockMaxY = NULL;
	}
}

float CTerrainHeight::GetValueDefault(int row, int col, float def)
{
	if ((size_t(row) < m_nRows) && (size_t(col) < m_nCols))
	{
		return GetValue(row, col);
	}
	else
	{
		int r = int(m_pZone->GetGlobalRow()) + row;
		int c = int(m_pZone->GetGlobalCol()) + col;

		return m_pTerrain->GetGlobalHeightDefault(r, c, def);
	}
}

bool CTerrainHeight::SetValue(int row, int col, float value)
{
	Assert(size_t(row) < m_nRows); 
	Assert(size_t(col) < m_nCols);

	m_pValues[row * m_nCols + col] = value;
	
	if (value < m_fMinY)
	{
		m_fMinY = value;
	}

	if (value > m_fMaxY)
	{
		m_fMaxY = value;
	}
	
	return true;
}

bool CTerrainHeight::ChangeValue(int row, int col, float value)
{
	// 等值判断
	if (GetValue(row, col) == value)
	{
		return false;
	}
	
	SetValue(row, col, value);

	// 释放自身及相关点的法线
	ReleaseNormal(row, col);
	ReleaseNormal(row - 1, col - 1);
	ReleaseNormal(row - 1, col);
	ReleaseNormal(row - 1, col + 1);
	ReleaseNormal(row, col - 1);
	ReleaseNormal(row, col + 1);
	ReleaseNormal(row + 1, col - 1);
	ReleaseNormal(row + 1, col);
	ReleaseNormal(row + 1, col + 1);
	
	return true;
}

float CTerrainHeight::GetY(float x, float z)
{
	float sx = x - m_fLeft;
	float sz = z - m_fTop;
	
	int col = int(sx * m_fInvUnitSize);
	int row = int(sz * m_fInvUnitSize);
	
	if (size_t(col) >= (m_nCols - 1))
	{
		return 0.0F;
	}
	
	if (size_t(row) >= (m_nRows - 1))
	{
		return 0.0F;
	}
	
	float y2 = GetValue(row, col + 1);
	float y3 = GetValue(row + 1, col);
	
	float u = (sx - col * m_fUnitSize) * m_fInvUnitSize;
	float v = (sz - row * m_fUnitSize) * m_fInvUnitSize;
	
	if (u + v <= 1.0F)
	{
		float y1 = GetValue(row, col);

		return y1 + (y2 - y1) * u + (y3 - y1) * v;
	}
	else
	{
		float y4 = GetValue(row + 1, col + 1);

		u = 1.0F - u;
		v = 1.0F - v;
		
		return y4 + (y3 - y4) * u + (y2 - y4) * v;
	}
}

bool CTerrainHeight::SetY(int row, int col, float y)
{
	if (size_t(col) >= (m_nCols - 1))
	{
		return false;
	}
	
	if (size_t(row) >= (m_nRows - 1))
	{
		return false;
	}

	// 同时更新其他区域的右下侧顶点
	CZoneManager* pManager = m_pTerrain->GetZoneManager();
	
	if ((row == 0) && (col == 0))
	{
		CTerrainZone* pLeftUpZone = pManager->GetLeftUpZone(m_pZone);
		
		if (pLeftUpZone != NULL)
		{
			pLeftUpZone->GetHeight()->ChangeValue((int)m_nRows - 1, 
				(int)m_nCols - 1, y);
		}
	}

	if (row == 0)
	{
		CTerrainZone* pUpZone = pManager->GetUpZone(m_pZone);

		if (pUpZone != NULL)
		{
			pUpZone->GetHeight()->ChangeValue((int)m_nRows - 1, col, y);
		}
	}

	if (col == 0)
	{
		CTerrainZone* pLeftZone = pManager->GetLeftZone(m_pZone);
		
		if (pLeftZone != NULL)
		{
			pLeftZone->GetHeight()->ChangeValue(row, (int)m_nCols - 1, y);
		}
	}
	
	return ChangeValue(row, col, y);
}

bool CTerrainHeight::GetPoint(int row, int col, FmVec4& point)
{
	point.x = m_fLeft + m_fUnitSize * col;
	point.y = GetValue(row, col);
	point.z = m_fTop + m_fUnitSize * row;

	return true;
}

float CTerrainHeight::GetRegionMinY(int row, int col, int scale)
{
	Assert(size_t(scale) >= m_nBlockScale);
	
	size_t dims = scale / m_nBlockScale;
	size_t r = row / m_nBlockScale;
	size_t c = col / m_nBlockScale;

	Assert(size_t(r) < m_nBlockDims);
	Assert(size_t(c) < m_nBlockDims);

	float* p = m_pBlockMinY + r * m_nBlockDims + c;
	float min_y = p[0];

	if (dims > 1)
	{
		for (size_t i = 0; i < dims; ++i)
		{
			for (size_t k = 0; k < dims; ++k)
			{
				if (p[k] < min_y)
				{
					min_y = p[k];
				}
			}

			p += m_nBlockDims;
		}
	}

	return min_y;
}

void CTerrainHeight::GetBlockMinMaxY(int row, int col, int scale, 
	float& min_y, float& max_y)
{
	Assert(size_t(scale) == m_nBlockScale);
	
	int r = row / scale;
	int c = col / scale;

	Assert(size_t(r) < m_nBlockDims);
	Assert(size_t(c) < m_nBlockDims);

	size_t index = r * m_nBlockDims + c;

	min_y = m_pBlockMinY[index];
	max_y = m_pBlockMaxY[index];
}

/*
float CTerrainHeight::GetRegionMinY(int row, int col, int scale)
{
	Assert(size_t(row) < m_nRows);
	Assert(size_t(col) < m_nCols);
	Assert(size_t(row + scale) <= m_nRows);
	Assert(size_t(col + scale) <= m_nCols);

	float* p = m_pValues + row * m_nCols + col;
	float min_y = *p;

	for (int r = 0; r < scale; ++r)
	{
		for (int c = 0; c < scale; ++c)
		{
			if (p[c] < min_y)
			{
				min_y = p[c];
			}
		}

		p += m_nCols;
	}

	return min_y;
}
*/

bool CTerrainHeight::ConnectValues()
{
	CZoneManager* pManager = m_pTerrain->GetZoneManager();
	
	CTerrainZone* pLeftUpZone = pManager->GetLeftUpZone(m_pZone);
	
	if (pLeftUpZone != NULL)
	{
		pLeftUpZone->GetHeight()->ChangeValue((int)m_nRows - 1, 
			(int)m_nCols - 1, GetValue(0, 0));
	}

	CTerrainZone* pUpZone = pManager->GetUpZone(m_pZone);
	
	if (pUpZone != NULL)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			pUpZone->GetHeight()->ChangeValue((int)m_nRows - 1, (int)col, 
				GetValue(0, (int)col));
		}
	}

	CTerrainZone* pLeftZone = pManager->GetLeftZone(m_pZone);
	
	if (pLeftZone != NULL)
	{
		for (size_t row = 0; row < m_nRows; ++row)
		{
			pLeftZone->GetHeight()->ChangeValue((int)row, (int)m_nCols - 1, 
				GetValue((int)row, 0));
		}
	}

	return true;
}

bool CTerrainHeight::ReleaseNormal(int row, int col)
{
	if ((size_t(row) < m_nRows) && (size_t(col) < m_nCols))
	{
		m_pNormals[row * m_nCols + col] = CalcVertexNormal(row, col);
		return true;
	}
	else
	{
		int r = int(m_pZone->GetGlobalRow()) + row;
		int c = int(m_pZone->GetGlobalCol()) + col;
		
		return m_pTerrain->ReleaseGlobalNormal(r, c);
	}
}

bool CTerrainHeight::GetNormal(int row, int col, FmVec4& normal)
{
	Assert(size_t(row) < m_nRows); 
	Assert(size_t(col) < m_nCols);
	
	size_t index = row * m_nCols + col;

	if (m_pNormals[index] == 0)
	{
		m_pNormals[index] = CalcVertexNormal(row, col);
	}

	unsigned int n = m_pNormals[index];
	unsigned int x_part = (n >> 16) & 0xFF;
	unsigned int y_part = (n >> 8) & 0xFF;
	unsigned int z_part = (n >> 0) & 0xFF;

	normal.x = (x_part / 255.0F) * 2.0F - 1.0F;
	normal.y = y_part / 255.0F;
	normal.z = (z_part / 255.0F) * 2.0F - 1.0F;

	return true;
}

unsigned int CTerrainHeight::CalcVertexNormal(int row, int col)
{
	Assert(size_t(row) < m_nRows); 
	Assert(size_t(col) < m_nCols);

	float y11 = GetValue(row, col);
	float y00 = GetValueDefault(row - 1, col - 1, y11);
	float y01 = GetValueDefault(row - 1, col, y11);
	float y02 = GetValueDefault(row - 1, col + 1, y11);
	float y10 = GetValueDefault(row, col - 1, y11);
	float y12 = GetValueDefault(row, col + 1, y11);
	float y20 = GetValueDefault(row + 1, col - 1, y11);
	float y21 = GetValueDefault(row + 1, col, y11);
	float y22 = GetValueDefault(row + 1, col + 1, y11);

	FmVec3 n;

	n.x = y00 + y10 * 2 + y20 - y02 - y12 * 2 - y22;
	n.y = 8.0F;
	n.z = y00 + y01 * 2 + y02 - y20 - y21 * 2 - y22;

	FmVec3Normalize(&n, &n);

	int x_part = int((n.x + 1.0F) * 0.5F * 255.0F);
	int y_part = int(n.y * 255.0F);
	int z_part = int((n.z + 1.0F) * 0.5F * 255.0F);

	if (x_part < 0) x_part = 0;
	if (x_part > 255) x_part = 255;
	if (y_part < 0) y_part = 0;
	if (y_part > 255) y_part = 255;
	if (z_part < 0) z_part = 0;
	if (z_part > 255) z_part = 255;

	return COLOR_ARGB(255, x_part, y_part, z_part);
}

bool CTerrainHeight::Build(size_t rows, size_t cols, float min_y, float max_y,
	float* heights, unsigned int* normals)
{
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	Assert(heights != NULL);
	Assert(normals != NULL);

	m_fMinY = min_y;
	m_fMaxY = max_y;
	m_pValues = heights;
	m_pNormals = normals;

	return true;
}

bool CTerrainHeight::BuildBlock(size_t block_scale, size_t block_dims, 
	float* block_min_y, float* block_max_y)
{
	Assert(block_min_y != NULL);
	Assert(block_max_y != NULL);

	m_nBlockScale = block_scale;
	m_nBlockDims = block_dims;
	m_pBlockMinY = block_min_y;
	m_pBlockMaxY = block_max_y;

	return true;
}
