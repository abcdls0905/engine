//--------------------------------------------------------------------
// 文件名:		terrain_chunk.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_chunk.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "terrain_light.h"
#include "terrain_walkable.h"
#include "terrain_region.h"
#include "chunk_style.h"
#include "chunk_vb.h"
#include "tex_alpha_map.h"
#include "height_loader.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/inlines.h"
#include "blend_tex_set.h"
#include "blend_tex.h"

#define INIT_TERRAIN_LINE(p, x1, y1, z1, color1) \
	{ (p)->x = x1; (p)->y = y1; (p)->z = z1; (p)->diffuse = color1; }

// CTerrainChunk

CTerrainChunk::CTerrainChunk(Terrain* pTerrain, CTerrainZone* pZone,
	CTerrainHeight* pHeight, size_t index, size_t row, size_t col)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);
	Assert(pHeight != NULL);

	size_t chunk_scale = pTerrain->GetChunkScale();
	m_pRender = pTerrain->GetRender();
	
	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_pHeight = pHeight;
	m_nIndex = (int)index;
	m_nRow = (int)row;
	m_nCol = (int)col;
	m_nLocalRow = (int)(row * chunk_scale);
	m_nLocalCol = (int)(col * chunk_scale);
	m_nNewLevel = 0;
	//m_nOldLevel = 0;
	m_nStyle = 0;
	m_nChunkLevel = 0;
	m_nBaseTexIndex = 0;
	m_nWaterIndex = -1;
	m_bVisible = false;
	m_bAlphaFade = false;
	m_bAlphaHide = false;
	m_bInDesign = false;
	m_bShowBoundBox = false;
	m_pAlphaTex = NULL;
	m_bHasGrassVertexBuffer = false;
	m_vMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_vMax = FmVec3(0.0F, 0.0F, 0.0F);
	m_vCenter = FmVec3(0.0F, 0.0F, 0.0F);
	m_fRadius = 0.0F;
	m_fCameraDistance = 0.0F;
	m_fViewZ = 0.0F;
	m_nDesignLineNum = 0;
	m_pDesignLines = NULL;
	m_nDesignLineMemSize = 0;
	memset(m_pSiblings, 0, sizeof(m_pSiblings));
	memset(m_fErrors, 0, sizeof(m_fErrors));
	memset(m_pChunkVBs, 0, sizeof(m_pChunkVBs));
//	m_pChunkCB = NULL;
	m_bSaveChunkWalkPos = true;
	m_nShowDesignLine = 0;
	SafeSprintf(m_strUid, sizeof(m_strUid), "chk_%d_%d", pZone->GetIndex(),
		index);
    
	m_pShader = NULL;

	m_pWaterShader = NULL;

	m_waterlevel = 0;

	m_pGrass_Tex0 = NULL;
	m_pGrassShader = NULL;
	m_pGrassIB = NULL;
	m_pGrassVB = NULL;
	m_pZoneLightMap = NULL;

	m_pWaterShaderHandle = NULL;
	m_pGrassShaderHandle = NULL;
}

CTerrainChunk::~CTerrainChunk()
{
	ReleaseAlphaTex();
	ReleaseVertexBuffer();
	ReleaseConstBuffer();

	for (size_t i = 0; i < m_BlendTexs.size(); ++i)
	{
		CORE_DELETE(m_BlendTexs[i].pAlphaMap);
	}

	for (size_t k = 0; k < m_GrassArray.size(); ++k)
	{
		chunk_grass_t* p = m_GrassArray[k];

		if (NULL == p)
		{
			continue;
		}
		
		SAFE_RELEASE(p->pVBuffer);
		CORE_DELETE(p);
	}
}

void CTerrainChunk::SetWaterIndex(int value) 
{ 
	if (value >= 0)
	{
		m_pZone->SetHasWater(true);
	}
	
	m_nWaterIndex = value; 
}

bool CTerrainChunk::Build(const FmVec3& v_min, const FmVec3& v_max, 
	float* lod_errors, size_t lod_levels)
{
	Assert(lod_errors != NULL);
	Assert(lod_levels <= 8);

	m_vMin = v_min;
	m_vMax = v_max;
	m_vCenter = (m_vMin + m_vMax) * 0.5F;
	FmVec3 vecSub = m_vMax - m_vMin;
	m_fRadius = FmVec3Length(&vecSub) * 0.5F;

	for (size_t i = 0; i < lod_levels; ++i)
	{
		m_fErrors[i] = lod_errors[i];
	}

	return true;
}

//void CTerrainChunk::GetCollideTriangle(int* row_col, FmVec4* v1, 
//	FmVec4* v2, FmVec4* v3)
//{
//	CTerrainZone* pZone = m_pZone;
//	CTerrainHeight* pHeight = m_pHeight;
//	size_t height_cols = pHeight->GetCols();
//	const float* height_values = pHeight->GetHeightValues();
//	float left = pZone->GetLeft();
//	float top = pZone->GetTop();
//	float unit_size = m_pTerrain->GetUnitSize();
//	int local_row = m_nLocalRow;
//	int local_col = m_nLocalCol;
//	
//	int row1 = row_col[0] + local_row;
//	int col1 = row_col[1] + local_col;
//	int row2 = row_col[2] + local_row;
//	int col2 = row_col[3] + local_col;
//	int row3 = row_col[4] + local_row;
//	int col3 = row_col[5] + local_col;
//
//	v1->x = left + col1 * unit_size;
//	v2->x = left + col2 * unit_size;
//	v3->x = left + col3 * unit_size;
//	v1->y = height_values[row1 * height_cols + col1];
//	v2->y = height_values[row2 * height_cols + col2];
//	v3->y = height_values[row3 * height_cols + col3];
//	v1->z = top + row1 * unit_size;
//	v2->z = top + row2 * unit_size;
//	v3->z = top + row3 * unit_size;
//}

FmVec3 CTerrainChunk::GetVector(int row, int col)
{
	float unit_size = m_pTerrain->GetUnitSize();
	float x = m_pZone->GetLeft() + col * unit_size;
	float y = m_pHeight->GetValueDefault(row, col, 0.0F);
	float z = m_pZone->GetTop() + row * unit_size;

	return FmVec3(x, y, z);
}

void CTerrainChunk::UpdateAllSiblings()
{
	m_pSiblings[SIBLING_LEFT] = GetLeftSibling();
	m_pSiblings[SIBLING_RIGHT] = GetRightSibling();
	m_pSiblings[SIBLING_UP] = GetUpSibling();
	m_pSiblings[SIBLING_DOWN] = GetDownSibling();
}

void CTerrainChunk::UpdateLeftSibling()
{
	m_pSiblings[SIBLING_LEFT] = GetLeftSibling();
}

void CTerrainChunk::UpdateRightSibling()
{
	m_pSiblings[SIBLING_RIGHT] = GetRightSibling();
}

void CTerrainChunk::UpdateUpSibling()
{
	m_pSiblings[SIBLING_UP] = GetUpSibling();
}

void CTerrainChunk::UpdateDownSibling()
{
	m_pSiblings[SIBLING_DOWN] = GetDownSibling();
}

CTerrainChunk* CTerrainChunk::GetLeftSibling()
{
	CTerrainChunk* chunk = m_pZone->GetLeftChunk(this);

	if (NULL == chunk)
	{
		CTerrainZone* zone = m_pTerrain->GetLeftZone(m_pZone);

		if (NULL == zone)
		{
			return NULL;
		}
		
		chunk = zone->GetRightEdgeChunk(m_nRow);

		if (NULL == chunk)
		{
			return NULL;
		}
	}

	return chunk;
}

CTerrainChunk* CTerrainChunk::GetRightSibling()
{
	CTerrainChunk* chunk = m_pZone->GetRightChunk(this);
	
	if (NULL == chunk)
	{
		CTerrainZone* zone = m_pTerrain->GetRightZone(m_pZone);
		
		if (NULL == zone)
		{
			return NULL;
		}
		
		chunk = zone->GetLeftEdgeChunk(m_nRow);

		if (NULL == chunk)
		{
			return NULL;
		}
	}
	
	return chunk;
}

CTerrainChunk* CTerrainChunk::GetUpSibling()
{
	CTerrainChunk* chunk = m_pZone->GetUpChunk(this);
	
	if (NULL == chunk)
	{
		CTerrainZone* zone = m_pTerrain->GetUpZone(m_pZone);
		
		if (NULL == zone)
		{
			return NULL;
		}
		
		chunk = zone->GetDownEdgeChunk(m_nCol);

		if (NULL == chunk)
		{
			return NULL;
		}
	}
	
	return chunk;
}

CTerrainChunk* CTerrainChunk::GetDownSibling()
{
	CTerrainChunk* chunk = m_pZone->GetDownChunk(this);
	
	if (NULL == chunk)
	{
		CTerrainZone* zone = m_pTerrain->GetDownZone(m_pZone);
		
		if (NULL == zone)
		{
			return NULL;
		}
		
		chunk = zone->GetUpEdgeChunk(m_nCol);
		
		if (NULL == chunk)
		{
			return NULL;
		}
	}
	
	return chunk;
}

void CTerrainChunk::CalcErrors()
{
	CHeightLoader::CalcChunkErrors(m_pHeight->GetHeightValues(),
		m_pHeight->GetCols(), m_nLocalRow, m_nLocalCol, 
		m_pTerrain->GetChunkScale(), m_pTerrain->GetLodLevels(), m_fErrors);
}

void CTerrainChunk::CalcBoundBox()
{
	CHeightLoader::CalcChunkBoundBox(m_pHeight->GetHeightValues(),
		m_pHeight->GetCols(), m_nLocalRow, m_nLocalCol, 
		m_pTerrain->GetChunkScale(), m_pZone->GetLeft(), m_pZone->GetTop(), 
		m_pTerrain->GetUnitSize(), m_vMin, m_vMax);
	
	m_vCenter = (m_vMin + m_vMax) * 0.5F;
	FmVec3 vecSub = m_vMax - m_vMin;
	m_fRadius = FmVec3Length(&vecSub) * 0.5F;
}

void CTerrainChunk::CalcLevel(IRender* pRender, 
	const FmMat4& mtxViewProj)
{
	int level = 0;
	float h_2 = float(pRender->GetDeviceHeight()) * 0.5F;
	float w_2 = float(pRender->GetDeviceWidth()) * 0.5F;
	float pixel_error = m_pTerrain->GetPixelError();
	FmVec3 pos1 = m_vCenter;
	FmVec3 pos2;
	FmVec3 v1;
	FmVec3 v2;
	int levels = (int)m_pTerrain->GetLodLevels();
	
	for (int i = (levels - 1); i > 0; i--)
	{
		float delta = m_fErrors[i];

		if (0 == delta)
		{
			level = i;
			break;
		}

		pos2.x = pos1.x + delta;
		pos2.y = pos1.y + delta;
		pos2.z = pos1.z + delta;
		FmVec3TransformCoord(&v1, &pos1, &mtxViewProj);
		FmVec3TransformCoord(&v2, &pos2, &mtxViewProj);
		
		float err_x = (v2.x - v1.x) * w_2;
		float err_y = (v2.y - v1.y) * h_2;
		float square = err_x * err_x + err_y * err_y;

		if (square <= (pixel_error * pixel_error))
		{
			level = i;
			break;
		}
	}

	SetLevel(level);
}

void CTerrainChunk::CalcStyle()
{
	if (size_t(m_nNewLevel) >= (m_pTerrain->GetLodLevels() - 1))
	{
		// 最高级只有一种连接风格
		m_nStyle = (m_nNewLevel << 4);
		return;
	}
	
	// 根据相邻分块的LOD级别确定连接的方式
	unsigned int left = 0;
	unsigned int right = 0;
	unsigned int top = 0;
	unsigned int bottom = 0;
	CTerrainChunk* nob;

	// 左
	//nob = GetLeftSibling();
	nob = m_pSiblings[SIBLING_LEFT];

	if (nob && nob->GetVisible())
	{
		if (nob->m_nNewLevel > m_nNewLevel)
		{
			left = 1;
		}
	}

	// 右
	//nob = GetRightSibling();
	nob = m_pSiblings[SIBLING_RIGHT];

	if (nob && nob->GetVisible())
	{
		if (nob->m_nNewLevel > m_nNewLevel)
		{
			right = 1;
		}
	}
	
	// 上
	//nob = GetUpSibling();
	nob = m_pSiblings[SIBLING_UP];

	if (nob && nob->GetVisible())
	{
		if (nob->m_nNewLevel > m_nNewLevel)
		{
			top = 1;
		}
	}

	// 下
	//nob = GetDownSibling();
	nob = m_pSiblings[SIBLING_DOWN];

	if (nob && nob->GetVisible())
	{
		if (nob->m_nNewLevel > m_nNewLevel)
		{
			bottom = 1;
		}
	}
	
	m_nStyle = (m_nNewLevel << 4) + (left << 3) + (top << 2) + (right << 1) 
		+ bottom;
}

//bool CTerrainChunk::SetLevel(int level)
//{
//	if (size_t(level) >= m_pTerrain->GetLodLevels())
//	{
//		return false;
//	}
//
//	m_nNewLevel = level;
//	m_nStyle = (level << 4);
//
//	return true;
//}

// 检查LOD级别，保证相邻分块只相差一级
bool CTerrainChunk::ChangeLevel()
{
	if (m_nNewLevel < 2)
	{
		// 0级和1级不需要改变
		return false;
	}

	CTerrainChunk* nob;
	
	//nob = GetLeftSibling();
	nob = m_pSiblings[SIBLING_LEFT];

	if (nob && nob->GetVisible())
	{
		if ((m_nNewLevel - nob->m_nNewLevel) > 1)
		{
			SetLevel(nob->m_nNewLevel + 1);
			return true;
		}
	}

	//nob = GetRightSibling();
	nob = m_pSiblings[SIBLING_RIGHT];

	if (nob && nob->GetVisible())
	{
		if ((m_nNewLevel - nob->m_nNewLevel) > 1)
		{
			SetLevel(nob->m_nNewLevel + 1);
			return true;
		}
	}
	
	//nob = GetUpSibling();
	nob = m_pSiblings[SIBLING_UP];

	if (nob && nob->GetVisible())
	{
		if ((m_nNewLevel - nob->m_nNewLevel) > 1)
		{
			SetLevel(nob->m_nNewLevel + 1);
			return true;
		}
	}

	//nob = GetDownSibling();
	nob = m_pSiblings[SIBLING_DOWN];

	if (nob && nob->GetVisible())
	{
		if ((m_nNewLevel - nob->m_nNewLevel) > 1)
		{
			SetLevel(nob->m_nNewLevel + 1);
			return true;
		}
	}

	return false;
}

//bool CTerrainChunk::CheckVertexBuffer()
//{
//	if (m_nNewLevel != m_nOldLevel)
//	{
		// 此时不能释放顶点缓冲
		//if (m_nOldLevel < 2)
		//{
		//	// 只释放比较大的顶点缓冲
		//	if (m_pChunkVBs[m_nOldLevel])
		//	{
		//		//m_pTerrain->ReleaseChunkVB(m_pChunkVBs[m_nOldLevel]);
		//		CORE_DELETE(m_pChunkVBs[m_nOldLevel]);
		//		m_pChunkVBs[m_nOldLevel] = NULL;
		//	}
		//}

//		m_nOldLevel = m_nNewLevel;
//	}
//
//	return true;
//}

void CTerrainChunk::GenTerrainVertices(void* pv, int level)
{
	CChunkVB::terrain_vertex_t* pVertex = (CChunkVB::terrain_vertex_t*)pv;
	Terrain* pTerrain = m_pTerrain;
	size_t scale = pTerrain->GetChunkScale() + 1;
	size_t step = 1 << level;
	float unit_size = pTerrain->GetUnitSize();
	float step_size = unit_size * step;
	size_t row = m_nLocalRow;
	size_t col = m_nLocalCol;
	size_t row_end = row + scale;
	size_t col_end = col + scale;
	CTerrainHeight* pHeight = m_pHeight;
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	float z = 0.0F;
	CChunkVB::terrain_vertex_t temp_vertex;

	for (size_t r = row; r < row_end; r += step)
	{
		float x = 0.0F;
	
		for (size_t c = col; c < col_end; c += step)
		{
			temp_vertex.x = x;
			temp_vertex.y = height_values[r * height_cols + c];
            temp_vertex.z = z;
			temp_vertex.diffuse_normal = VisUtil_ColorToVector4(pHeight->GetIntNormal((int)r, (int)c));
			memcpy(pVertex,&temp_vertex,sizeof(temp_vertex));
            /*
			pVertex->x = x;
			pVertex->y = height_values[r * height_cols + c];
			pVertex->z = z;
			pVertex->diffuse_normal = VisUtil_ColorToVector4(pHeight->GetIntNormal((int)r, (int)c));
			*/
			pVertex++;
			x += step_size;
		}

		z += step_size;
	}
}

size_t CTerrainChunk::GenInFogVertices(FmVec4* pv)
{
	if (NULL == pv)
	{
		size_t d = (m_pTerrain->GetChunkScale() >> m_nNewLevel) + 1;
		return d * d;
	}
	
	Terrain* pTerrain = m_pTerrain;
	size_t scale = pTerrain->GetChunkScale() + 1;
	size_t step = 1 << m_nNewLevel;
	size_t row = m_nLocalRow;
	size_t col = m_nLocalCol;
	size_t row_end = row + scale;
	size_t col_end = col + scale;
	float unit_size = pTerrain->GetUnitSize();
	float step_size = unit_size * step;
	float left = m_pZone->GetLeft() + col * unit_size;
	float top = m_pZone->GetTop() + row * unit_size;
	CTerrainHeight* pHeight = m_pHeight;
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	FmVec4* pVertex = pv;
	float z = top;

	for (size_t r = row; r < row_end; r += step)
	{
		float x = left; 

		for (size_t c = col; c < col_end; c += step)
		{
			pVertex->x = x;
			pVertex->y = height_values[r * height_cols + c];
			pVertex->z = z;
			pVertex++;
			x += step_size;
		}

		z += step_size;
	}

	return (pVertex - pv);
}

size_t CTerrainChunk::GenInFogIndices(unsigned short* pi, 
	unsigned short start_index)
{
	CChunkStyle* pStyle = m_pTerrain->GetChunkStyle(m_nStyle);
	size_t index_num = pStyle->GetTriangleCount() * 3;
	
	if (NULL == pi)
	{
		return index_num;
	}

	unsigned short* pIndices = pStyle->GetIndices();

	for (size_t i = 0; i < index_num; ++i)
	{
		*pi++ = (*pIndices++) + start_index;
	}

	return index_num;
}

bool CTerrainChunk::CreateVertexBuffer(int level)
{
	Assert(size_t(level) < 8);
	Assert(m_pChunkVBs[level] == NULL);

	size_t scale = (m_pTerrain->GetChunkScale() >> level) + 1;
	size_t vertex_num = scale * scale;
	size_t data_size = vertex_num * sizeof(CChunkVB::terrain_vertex_t);
	TAutoMem<unsigned char, 1024, TCoreAlloc> auto_buf(data_size);
	unsigned char* pv = auto_buf.GetBuffer();

	GenTerrainVertices(pv, level);

	CChunkVB* pChunkVB = CORE_NEW(CChunkVB);

	if (!pChunkVB->Create(m_pTerrain->GetRender(), vertex_num, pv))
	{
		CORE_DELETE(pChunkVB);
		CORE_TRACE("(CTerrainChunk::CreateVertexBuffer)failed");
		return false;
	}

	pChunkVB->SetLevel(level);
	m_pChunkVBs[level] = pChunkVB;

	return true;
}

void CTerrainChunk::ReleaseVertexBuffer()
{
	CChunkVB** pChunkVBs = m_pChunkVBs;
	
	for (size_t i = 0; i < 8; ++i)
	{
		if (pChunkVBs[i])
		{
			CORE_DELETE(pChunkVBs[i]);
			pChunkVBs[i] = NULL;
		}
	}
	
	if (m_pDesignLines)
	{
		CORE_FREE(m_pDesignLines, m_nDesignLineMemSize);
		m_pDesignLines = NULL;
		m_nDesignLineMemSize = 0;
		m_nDesignLineNum = 0;
	}
}

 

void CTerrainChunk::ReleaseConstBuffer()
{
//	SAFE_RELEASE(m_pChunkCB);
}

size_t CTerrainChunk::GetAlphaMapCount() const
{
	return m_BlendTexs.size();
}

size_t CTerrainChunk::GetAlphaMapTexIndex(size_t index) const
{
	Assert(index < m_BlendTexs.size());

	return m_BlendTexs[index].nTexIndex;
}

size_t CTerrainChunk::GetAlphaMapFormat(size_t index) const
{
	Assert(index < m_BlendTexs.size());

	return m_BlendTexs[index].nFormat;
}

CTexAlphaMap* CTerrainChunk::GetAlphaMapByIndex(size_t index) const
{
	Assert(index < m_BlendTexs.size());

	return m_BlendTexs[index].pAlphaMap;
}

CTexAlphaMap* CTerrainChunk::GetAlphaMap(size_t tex_index, size_t format) const
{
	for (size_t i = 0; i < m_BlendTexs.size(); ++i)
	{
		if ((m_BlendTexs[i].nTexIndex == tex_index)
			&& (m_BlendTexs[i].nFormat == format))
		{
			return m_BlendTexs[i].pAlphaMap;
		}
	}
	
	return NULL;
}

CTexAlphaMap* CTerrainChunk::AddAlphaMap(size_t tex_index, size_t format)
{
	CTexAlphaMap* pAlphaMap = CORE_NEW(CTexAlphaMap);
		
	pAlphaMap->SetScale(m_pTerrain->GetAlphaMapScale());
	pAlphaMap->CreateAlpha();

	blend_tex_t data;

	data.nTexIndex = tex_index;
	data.nFormat = format;
	data.pAlphaMap = pAlphaMap;

	m_BlendTexs.push_back(data);

	return pAlphaMap;
}

bool CTerrainChunk::RemoveAlphaMapByIndex(size_t index)
{
	Assert(index < m_BlendTexs.size());

	CORE_DELETE(m_BlendTexs[index].pAlphaMap);
	m_BlendTexs.remove(index);
	
	return true;
}

void CTerrainChunk::ClearEmptyAlphaMap()
{
	size_t i = 0;

	while (i < m_BlendTexs.size())
	{
		if (m_BlendTexs[i].pAlphaMap->IsEmpty())
		{
			CORE_DELETE(m_BlendTexs[i].pAlphaMap);
			m_BlendTexs.remove(i);
		}
		else
		{
			++i;
		}
	}
}

void CTerrainChunk::ReleaseAlphaMap()
{
	for (size_t i = 0; i < m_BlendTexs.size(); ++i)
	{
		CORE_DELETE(m_BlendTexs[i].pAlphaMap);
	}

	m_BlendTexs.clear();
}


void CTerrainChunk::GetBlendTexDataRGBA8888(unsigned char* pdata, size_t data_size)
{
#define ARGB4444ToARGB8888(value) ((( (value&0xF000)>>12)*16 )<<24) | ((( (value&0x0F00)>>8 )*16)<<16) | ((((value&0x00F0)>>4)*16)<<8) | (((value&0x000F)*16) )
#define ARGBtoRGBA32(value)   ((value<<8)|(value>>24))

	size_t alpha_map_scale = m_pTerrain->GetAlphaMapScale();
	size_t blend_num = m_BlendTexs.size();

	if (blend_num > 4)
	{
		blend_num = 4;
	}

	// 图片的数据是从上到下的
	Assert(data_size == (alpha_map_scale * alpha_map_scale * 4));

	unsigned int pitch = (unsigned int)alpha_map_scale * 4;

	if (blend_num == 4)
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		unsigned char* alpha2 = m_BlendTexs[1].pAlphaMap->GetMapData();
		unsigned char* alpha3 = m_BlendTexs[2].pAlphaMap->GetMapData();
		unsigned char* alpha4 = m_BlendTexs[3].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned int* pDst32 = (unsigned int*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned int value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned int)(alpha1[k] & 0xF0) << 8;
					value |= (unsigned int)(alpha2[k] & 0xF0) << 4;
					value |= (unsigned int)(alpha3[k] & 0xF0);
					value |= (unsigned int)(alpha4[k] & 0xF0) >> 4;
				}
				else
				{
					value = (unsigned int)(alpha1[k] & 0x0F)<< 12;
					value |= (unsigned int)(alpha2[k] & 0x0F)<< 8;
					value |= (unsigned int)(alpha3[k] & 0x0F)<< 4;
					value |= (unsigned int)(alpha4[k] & 0x0F);
					k++;
				}
                value = ARGB4444ToARGB8888(value);        
				*pDst32++ = ARGBtoRGBA32(value);
			}

			pdata += pitch;
		}
	}
	else if (blend_num == 3)
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		unsigned char* alpha2 = m_BlendTexs[1].pAlphaMap->GetMapData();
		unsigned char* alpha3 = m_BlendTexs[2].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned int* pDst32 = (unsigned int*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned int value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned int)(alpha1[k] & 0xF0) << 8;
					value |= (unsigned int)(alpha2[k] & 0xF0) << 4;
					value |= (unsigned int)(alpha3[k] & 0xF0) ;
				}
				else
				{
					value = (unsigned int)(alpha1[k] & 0x0F) << 12;
					value |= (unsigned int)(alpha2[k] & 0x0F) << 8;
					value |= (unsigned int)(alpha3[k] & 0x0F) << 4;
					k++;
				}
                value = ARGB4444ToARGB8888(value);   
				*pDst32++ = ARGBtoRGBA32(value);
			}

			pdata += pitch;
		}
	}
	else if (blend_num == 2)
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		unsigned char* alpha2 = m_BlendTexs[1].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned int* pDst32 = (unsigned int*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned int value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned int)(alpha1[k] & 0xF0) << 8;
					value |= (unsigned int)(alpha2[k] & 0xF0) << 4;
				}
				else
				{
					value = (unsigned int)(alpha1[k] & 0x0F) << 12;
					value |= (unsigned int)(alpha2[k] & 0x0F) << 8;
					k++;
				}
                value = ARGB4444ToARGB8888(value);   
				*pDst32++ = ARGBtoRGBA32(value);
			}

			pdata += pitch;
		}
	}
	else
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned int* pDst32 = (unsigned int*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned int value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned int)(alpha1[k] & 0xF0) << 8;
				}
				else
				{
					value = (unsigned int)(alpha1[k] & 0x0F) << 12;
					k++;
				}
                value = ARGB4444ToARGB8888(value);   
				*pDst32++ = ARGBtoRGBA32(value);
			}

			pdata += pitch;
		}
	}
}

void CTerrainChunk::GetBlendTexDataRGBA4444(unsigned char* pdata, size_t data_size)
{
#define ARGBtoRGBA16(value)   ((value<<4)|(value>>12))
	size_t alpha_map_scale = m_pTerrain->GetAlphaMapScale();
	size_t blend_num = m_BlendTexs.size();

	if (blend_num > 4)
	{
		blend_num = 4;
	}

	// 图片的数据是从上到下的
	Assert(data_size == (alpha_map_scale * alpha_map_scale * 2));

	unsigned int pitch = (unsigned int)alpha_map_scale * 2;

	if (blend_num == 4)
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		unsigned char* alpha2 = m_BlendTexs[1].pAlphaMap->GetMapData();
		unsigned char* alpha3 = m_BlendTexs[2].pAlphaMap->GetMapData();
		unsigned char* alpha4 = m_BlendTexs[3].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned short* pDst16 = (unsigned short*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned short value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned short)(alpha1[k] & 0xF0) << 8;
					value |= (unsigned short)(alpha2[k] & 0xF0) << 4;
					value |= (unsigned short)(alpha3[k] & 0xF0);
					value |= (unsigned short)(alpha4[k] & 0xF0) >> 4;
				}
				else
				{
					value = (unsigned short)(alpha1[k] & 0x0F) << 12;
					value |= (unsigned short)(alpha2[k] & 0x0F) << 8;
					value |= (unsigned short)(alpha3[k] & 0x0F) << 4;
					value |= (unsigned short)(alpha4[k] & 0x0F) ;
					k++;
				}

				*pDst16++ = ARGBtoRGBA16(value);
			}

			pdata += pitch;
		}
	}
	else if (blend_num == 3)
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		unsigned char* alpha2 = m_BlendTexs[1].pAlphaMap->GetMapData();
		unsigned char* alpha3 = m_BlendTexs[2].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned short* pDst16 = (unsigned short*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned short value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned short)(alpha1[k] & 0xF0) << 8;
					value |= (unsigned short)(alpha2[k] & 0xF0) << 4;
					value |= (unsigned short)(alpha3[k] & 0xF0) ;
				}
				else
				{
					value = (unsigned short)(alpha1[k] & 0x0F) << 12;
					value |= (unsigned short)(alpha2[k] & 0x0F) << 8;
					value |= (unsigned short)(alpha3[k] & 0x0F) << 4;
					k++;
				}

				*pDst16++ = ARGBtoRGBA16(value);
			}

			pdata += pitch;
		}
	}
	else if (blend_num == 2)
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		unsigned char* alpha2 = m_BlendTexs[1].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned short* pDst16 = (unsigned short*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned short value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned short)(alpha1[k] & 0xF0) << 8;
					value |= (unsigned short)(alpha2[k] & 0xF0) << 4;
				}
				else
				{
					value = (unsigned short)(alpha1[k] & 0x0F) << 12;
					value |= (unsigned short)(alpha2[k] & 0x0F) << 8;
					k++;
				}

				*pDst16++ = ARGBtoRGBA16(value);
			}

			pdata += pitch;
		}
	}
	else
	{
		unsigned char* alpha1 = m_BlendTexs[0].pAlphaMap->GetMapData();
		size_t k = 0;

		for (size_t y = 0; y < alpha_map_scale; ++y)
		{
			unsigned short* pDst16 = (unsigned short*)pdata;

			for (size_t x = 0; x < alpha_map_scale; ++x)
			{
				unsigned short value;

				if ((x & 0x1) == 0)
				{
					value = (unsigned short)(alpha1[k] & 0xF0) <<8;
				}
				else
				{
					value = (unsigned short)(alpha1[k] & 0x0F)<<12 ;
					k++;
				}

				*pDst16++ = ARGBtoRGBA16(value);
			}

			pdata += pitch;
		}
	}


}

IStaticTex* CTerrainChunk::CreateAlphaTex(IRender* pRender)
{

	if (m_pAlphaTex)
	{
		return m_pAlphaTex;
	}

	if (m_BlendTexs.empty())
	{
		return NULL;
	}

	size_t alpha_map_scale = m_pTerrain->GetAlphaMapScale();

//#define __RGBA8888__
 
#ifdef  __RGBA8888__
	// 图片的数据是从上到下的
	size_t data_size = alpha_map_scale * alpha_map_scale * 4;
	TAutoMem<unsigned char, 64 * 64 * 4, TCoreAlloc> auto_buf(data_size+2);
	unsigned char* pdata = auto_buf.GetBuffer();

	GetBlendTexDataRGBA8888(pdata, data_size);


	m_pAlphaTex = pRender->CreateStaticTex((unsigned int)alpha_map_scale, 
		(unsigned int)alpha_map_scale, 1, TEX_FORMAT_RGBA8888_UNORM, 
		auto_buf.GetBuffer(), (unsigned int)data_size);
#else
 
	// 图片的数据是从上到下的
	size_t data_size = alpha_map_scale * alpha_map_scale * 2;
	TAutoMem<unsigned char, 64 * 64 * 2, TCoreAlloc> auto_buf(data_size);
	unsigned char* pdata = auto_buf.GetBuffer();

	GetBlendTexDataRGBA4444(pdata, data_size);


	m_pAlphaTex = pRender->CreateStaticTex((unsigned int)alpha_map_scale, 
		(unsigned int)alpha_map_scale, 1, TEX_FORMAT_RGBA4444_UNORM, 
		auto_buf.GetBuffer(), (unsigned int)data_size);
#endif

	if (NULL == m_pAlphaTex)
	{
		CORE_TRACE("(CTerrainChunk::CreateAlphaTex)create texture failed");
		return NULL;
	}

	return m_pAlphaTex;
}

void CTerrainChunk::ReleaseAlphaTex()
{
	SAFE_RELEASE(m_pAlphaTex);
}

bool CTerrainChunk::Recalculate()
{
	CalcErrors();
	CalcBoundBox();
	// 改变碰撞树的信息
	m_pZone->ReleaseChunkCollide(m_nIndex);
	m_pZone->CreateChunkCollide(m_nIndex);

	return true;
}

// 设置渲染批次
bool CTerrainChunk::SetBatch(void* pBatch, int level,IShaderProgram* pShader)
{
	int style;
	
	if (level < 0)
	{
		level = m_nNewLevel;
		style = m_nStyle;
	}
	else
	{
		style = level << 4;
	}

	IRenderDrawOp* pRenderDrawOp = pShader->GetRenderDrawOp();
	CChunkVB* pVB = m_pChunkVBs[level];

	if( NULL == pVB)
	{
		if (!CreateVertexBuffer(level))
		{
			return false;
		}

		pVB = m_pChunkVBs[level];
	}
	CChunkStyle* pStyle = m_pTerrain->GetChunkStyle(style);

#define terrain_vertex_t_pos 0
#define terrain_vertex_t_diffuse_normal 1

	pRenderDrawOp->SetIB( pStyle->GetIndexBuf()->GetBuffer());
	pRenderDrawOp->SetVB( pVB->GetVertexBuf()->GetBuffer());

	pRenderDrawOp->EnableVertexAttribArray( terrain_vertex_t_pos,true);
	pRenderDrawOp->SetVertexAttribPointer( terrain_vertex_t_pos, 3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(CChunkVB::terrain_vertex_t),0);

	pRenderDrawOp->EnableVertexAttribArray( terrain_vertex_t_diffuse_normal,true);
	pRenderDrawOp->SetVertexAttribPointer( terrain_vertex_t_diffuse_normal, 4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(CChunkVB::terrain_vertex_t),(void*) (3 * sizeof(float)));

    //关闭2,3通道数据
	pRenderDrawOp->EnableVertexAttribArray( 2,false);
	pRenderDrawOp->EnableVertexAttribArray( 3,false);

	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,(unsigned int)pStyle->GetTriangleCount() * 3,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,0);


//	pRenderDrawOp->SetIB(0);
//	pRenderDrawOp->SetVB(0);

	return true;
}

const grass_data_t* CTerrainChunk::GetGrasses(int grass_index) const
{
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return NULL;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return NULL;
	}

	if (pChunkGrass->GrassData.empty())
	{
		return NULL;
	}

	return &(pChunkGrass->GrassData[0]);
}

#define GRASS_POS_EQUAL(v1, v2) ((v1 <= (v2 + 0.001F)) && (v1 >= (v2 - 0.001F)))

grass_data_t* CTerrainChunk::GetGrass(int grass_index, float x, float z)
{
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return NULL;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return NULL;
	}

	size_t grass_num = pChunkGrass->GrassData.size();

	if (0 == grass_num)
	{
		return NULL;
	}

	grass_data_t* pGrassData = &pChunkGrass->GrassData[0];

	for (size_t i = 0; i < grass_num; ++i)
	{
		if (GRASS_POS_EQUAL(pGrassData[i].fPosiX, x)
			&& GRASS_POS_EQUAL(pGrassData[i].fPosiZ, z))
		{
			return &pGrassData[i];
		}
	}

	return NULL;
}

bool CTerrainChunk::AddGrass(const grass_data_t& data)
{
	size_t grass_index = data.nGrassIndex;
	size_t grass_size = m_GrassArray.size();

	if (grass_index >= grass_size)
	{
		m_GrassArray.resize(grass_index + 1, NULL);
	}

	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		pChunkGrass = CORE_NEW(chunk_grass_t);
		pChunkGrass->pVBuffer = NULL;
		pChunkGrass->nActualCount = 0;

		m_GrassArray[grass_index] = pChunkGrass;
	}

	pChunkGrass->GrassData.push_back(data);

	return true;
}

bool CTerrainChunk::RemoveGrass(int grass_index, float x, float z)
{
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return false;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return false;
	}

	size_t grass_num = pChunkGrass->GrassData.size();

	if (0 == grass_num)
	{
		return false;
	}

	grass_data_t* pGrassData = &pChunkGrass->GrassData[0];
	size_t i = 0;

	while (i < grass_num)
	{
		if (GRASS_POS_EQUAL(pGrassData[i].fPosiX, x)
			&& GRASS_POS_EQUAL(pGrassData[i].fPosiZ, z))
		{
			pChunkGrass->GrassData.remove(i);
			--grass_num;
		}
		else
		{
			++i;
		}
	}

	return true;
}

size_t CTerrainChunk::InnerRemoveGrassRect(grass_container_t& grass, 
	float x1, float z1, float x2, float z2)
{
	size_t grass_num = grass.size();

	if (0 == grass_num)
	{
		return 0;
	}

	grass_data_t* pGrassData = &grass[0];
	size_t count = 0;
	size_t i = 0; 

	while (i < grass_num)
	{
		if ((pGrassData[i].fPosiX >= x1) && (pGrassData[i].fPosiX <= x2)
			&& (pGrassData[i].fPosiZ >= z1) && (pGrassData[i].fPosiZ <= z2))
		{
			count += pGrassData[i].nAmount;
			grass.remove(i);
			--grass_num;
		}
		else
		{
			++i;
		}
	}
	
	return count;
}

size_t CTerrainChunk::InnerRemoveGrassCircle(grass_container_t& grass, 
	float x, float z, float radius)
{
	size_t grass_num = grass.size();

	if (0 == grass_num)
	{
		return 0;
	}

	float square = radius * radius;
	grass_data_t* pGrassData = &grass[0];
	size_t count = 0;
	size_t i = 0; 

	while (i < grass_num)
	{
		float sx = pGrassData[i].fPosiX - x;
		float sz = pGrassData[i].fPosiZ - z;

		if ((sx * sx + sz * sz) <= square)
		{
			count += pGrassData[i].nAmount;
			grass.remove(i);
			--grass_num;
		}
		else
		{
			++i;
		}
	}
	
	return count;
}

size_t CTerrainChunk::InnerGetGrassRect(grass_container_t& grass, 
	float x1, float z1, float x2, float z2)
{
	size_t grass_num = grass.size();

	if (0 == grass_num)
	{
		return 0;
	}

	grass_data_t* pGrassData = &grass[0];
	size_t count = 0;

	for (size_t i = 0; i < grass_num; ++i)
	{
		if ((pGrassData[i].fPosiX >= x1) && (pGrassData[i].fPosiX <= x2)
			&& (pGrassData[i].fPosiZ >= z1) && (pGrassData[i].fPosiZ <= z2))
		{
			count += pGrassData[i].nAmount;
		}
	}
	
	return count;
}

size_t CTerrainChunk::InnerGetGrassCircle(grass_container_t& grass, 
	float x, float z, float radius)
{
	size_t grass_num = grass.size();

	if (0 == grass_num)
	{
		return 0;
	}

	float square = radius * radius;
	grass_data_t* pGrassData = &grass[0];
	size_t count = 0;

	for (size_t i = 0; i < grass_num; ++i)
	{
		float sx = pGrassData[i].fPosiX - x;
		float sz = pGrassData[i].fPosiZ - z;

		if ((sx * sx + sz * sz) <= square)
		{
			count += pGrassData[i].nAmount;
		}
	}
	
	return count;
}

static inline void push_unique_index(int* grass_index, 
	const int max_grass_count, int value)
{
	for (int i = 0; i < max_grass_count; ++i)
	{
		if (grass_index[i] != -1)
		{
			if (grass_index[i] == value)
			{
				return;
			}
		}
		else
		{
			grass_index[i] = value;
			return;
		}
	}
}

void CTerrainChunk::InnerGetGrassIndexRect(grass_container_t& grass, 
	float x1, float z1, float x2, float z2, int* grass_index, 
	const int max_grass_count)
{
	size_t grass_num = grass.size();

	if (0 == grass_num)
	{
		return;
	}

	grass_data_t* pGrassData = &grass[0];

	for (size_t i = 0; i < grass_num; ++i)
	{
		if ((pGrassData[i].fPosiX >= x1) && (pGrassData[i].fPosiX <= x2)
			&& (pGrassData[i].fPosiZ >= z1) && (pGrassData[i].fPosiZ <= z2))
		{
			push_unique_index(grass_index, max_grass_count, 
				pGrassData[i].nGrassIndex);
		}
	}
}

void CTerrainChunk::InnerGetGrassIndexCircle(grass_container_t& grass, 
	float x, float z, float radius, int* grass_index, 
	const int max_grass_count)
{
	size_t grass_num = grass.size();

	if (0 == grass_num)
	{
		return;
	}

	float square = radius * radius;
	grass_data_t* pGrassData = &grass[0];

	for (size_t i = 0; i < grass_num; ++i)
	{
		float sx = pGrassData[i].fPosiX - x;
		float sz = pGrassData[i].fPosiZ - z;

		if ((sx * sx + sz * sz) <= square)
		{
			push_unique_index(grass_index, max_grass_count, 
				pGrassData[i].nGrassIndex);
		}
	}
}

int CTerrainChunk::RemoveGrassRect(int grass_index, float x1, float z1, 
	float x2, float z2)
{
	if (grass_index < 0)
	{
		size_t count = 0;
		
		for (size_t i = 0; i < m_GrassArray.size(); ++i)
		{
			chunk_grass_t* pChunkGrass = m_GrassArray[i];
			
			if (pChunkGrass)
			{
				count += InnerRemoveGrassRect(pChunkGrass->GrassData, 
					x1, z1, x2, z2);
			}
		}
		
		return (int)count;
	}
	
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return 0;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];
	
	if (NULL == pChunkGrass)
	{
		return 0;
	}
	
	return (int)InnerRemoveGrassRect(pChunkGrass->GrassData, x1, z1, x2, z2);
}

int CTerrainChunk::RemoveGrassCircle(int grass_index, float x, float z, 
	float radius)
{
	if (grass_index < 0)
	{
		size_t count = 0;
		
		for (size_t i = 0; i < m_GrassArray.size(); ++i)
		{
			chunk_grass_t* pChunkGrass = m_GrassArray[i];

			if (pChunkGrass)
			{
				count += InnerRemoveGrassCircle(pChunkGrass->GrassData, 
					x, z, radius);
			}
		}
		
		return (int)count;
	}
	
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return 0;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return 0;
	}

	return (int)InnerRemoveGrassCircle(pChunkGrass->GrassData, x, z, radius);
}

int CTerrainChunk::GetGrassCountRect(int grass_index, float x1, float z1, 
	float x2, float z2)
{
	if (grass_index < 0)
	{
		size_t count = 0;
		
		for (size_t i = 0; i < m_GrassArray.size(); ++i)
		{
			chunk_grass_t* pChunkGrass = m_GrassArray[i];

			if (pChunkGrass)
			{
				count += InnerGetGrassRect(pChunkGrass->GrassData, 
					x1, z1, x2, z2);
			}
		}
		
		return (int)count;
	}

	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return 0;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return 0;
	}

	return (int)InnerGetGrassRect(pChunkGrass->GrassData, x1, z1, x2, z2);
}

int CTerrainChunk::GetGrassCountCircle(int grass_index, float x, float z, 
	float radius)
{
	if (grass_index < 0)
	{
		size_t count = 0;
		
		for (size_t i = 0; i < m_GrassArray.size(); ++i)
		{
			chunk_grass_t* pChunkGrass = m_GrassArray[i];

			if (pChunkGrass)
			{
				count += InnerGetGrassCircle(pChunkGrass->GrassData, 
					x, z, radius);
			}
		}
		
		return (int)count;
	}

	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return 0;
	}
	
	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return 0;
	}

	return (int)InnerGetGrassCircle(pChunkGrass->GrassData, x, z, radius);
}

void CTerrainChunk::GetGrassRect(float x1, float z1, float x2, float z2, 
	int* grass_index, const int max_grass_count)
{
	for (size_t i = 0; i < m_GrassArray.size(); ++i)
	{
		chunk_grass_t* pChunkGrass = m_GrassArray[i];

		if (pChunkGrass)
		{
			InnerGetGrassIndexRect(pChunkGrass->GrassData, x1, z1, x2, z2, 
				grass_index, max_grass_count);
		}
	}
}

void CTerrainChunk::GetGrassCircle(float x, float z, float radius, 
	int* grass_index, const int max_grass_count)
{
	for (size_t i = 0; i < m_GrassArray.size(); ++i)
	{
		chunk_grass_t* pChunkGrass = m_GrassArray[i];

		if (pChunkGrass)
		{
			InnerGetGrassIndexCircle(pChunkGrass->GrassData, x, z, radius, 
				grass_index, max_grass_count);
		}
	}
}

void CTerrainChunk::SetGrassVertexBuffer(int grass_index, 
	IStaticVB* pVBuffer, size_t grass_num)
{
	Assert(size_t(grass_index) < m_GrassArray.size());
	Assert(pVBuffer != NULL);

	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	Assert(pChunkGrass != NULL);

	SAFE_RELEASE(pChunkGrass->pVBuffer);
	pChunkGrass->pVBuffer = pVBuffer;
	pChunkGrass->nActualCount = grass_num;
	m_bHasGrassVertexBuffer = true;
}

IStaticVB* CTerrainChunk::GetGrassVertexBuffer(int grass_index) const
{
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return NULL;
	}

	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return NULL;
	}

	return pChunkGrass->pVBuffer;
}

size_t CTerrainChunk::GetGrassActualCount(int grass_index) const
{
	if (size_t(grass_index) >= m_GrassArray.size())
	{
		return 0;
	}

	chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

	if (NULL == pChunkGrass)
	{
		return 0;
	}

	return pChunkGrass->nActualCount;
}

void CTerrainChunk::ReleaseGrassVertexBuffer()
{
	if (m_bHasGrassVertexBuffer)
	{
		for (size_t i = 0; i < m_GrassArray.size(); ++i)
		{
			chunk_grass_t* pChunkGrass = m_GrassArray[i];

			if (NULL == pChunkGrass)
			{
				continue;
			}
			
			SAFE_RELEASE(pChunkGrass->pVBuffer);
			pChunkGrass->nActualCount = 0;
		}

		m_bHasGrassVertexBuffer = false;
	}
}

/*
bool CTerrainChunk::SetBatch(DxSceneBatch* pBatch, int level, bool has_normal)
{
	int style;
	
	if (level < 0)
	{
		level = m_nNewLevel;
		style = m_nStyle;
	}
	else
	{
		style = level << 4;
	}
	
	Render* pRender = m_pTerrain->GetRender();
	CChunkVB* pVB = m_pChunkVBs[level];

	if (NULL == pVB)
	{
		if (!CreateVertexBuffer(level))
		{
			return false;
		}

		pVB = m_pChunkVBs[level];
	}

	CChunkStyle* pStyle = m_pTerrain->GetChunkStyle(style);

	Assert(pStyle != NULL);

	pBatch->SetVertexBuffer(0, pVB->GetVertexBuf(), 
		sizeof(CChunkVB::terrain_vertex_t), 0);
	pBatch->SetIndexBuffer(pStyle->GetIndexBuf(), 0);
	pBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, 
		(unsigned int)pVB->GetVertexCount(), 
		(unsigned int)pStyle->GetTriangleCount() * 3);

	return true;
}
*/

bool CTerrainChunk::PaintBoundBox()
{
	struct vertex_line_t
	{
		FmVec3 pos;
		unsigned int diffuse;
	};

	FmVec3 v_min = m_vMin;
	FmVec3 v_max = m_vMax;

	// 保证可见
	v_min.y -= 0.1F;
	v_max.y += 0.1F;

	FmVec3 v1(v_min.x, v_max.y, v_min.z);
	FmVec3 v2(v_max.x, v_max.y, v_min.z);
	FmVec3 v3(v_max.x, v_max.y, v_max.z);
	FmVec3 v4(v_min.x, v_max.y, v_max.z);
	FmVec3 v5(v_min.x, v_min.y, v_min.z);
	FmVec3 v6(v_max.x, v_min.y, v_min.z);
	FmVec3 v7(v_max.x, v_min.y, v_max.z);
	FmVec3 v8(v_min.x, v_min.y, v_max.z);

	vertex_line_t v[24];

	v[0].pos = v1;
	v[1].pos = v2;
	v[2].pos = v2;
	v[3].pos = v3;
	v[4].pos = v3;
	v[5].pos = v4;
	v[6].pos = v4;
	v[7].pos = v1;
	v[8].pos = v1;
	v[9].pos = v5;
	v[10].pos = v2;
	v[11].pos = v6;
	v[12].pos = v3;
	v[13].pos = v7;
	v[14].pos = v4;
	v[15].pos = v8;
	v[16].pos = v5;
	v[17].pos = v6;
	v[18].pos = v6;
	v[19].pos = v7;
	v[20].pos = v7;
	v[21].pos = v8;
	v[22].pos = v8;
	v[23].pos = v5;

	for (size_t i = 0; i < 24; ++i)
	{
		v[i].diffuse = 0xFFFF0000;
	}

	IRender* pRender = m_pTerrain->GetRender();
	FmMat4 mat;

	FmMat4Identity(&mat);
	pRender->DrawDesignLine(mat, PRIMITIVE_LINELIST, 12, v, 
		sizeof(vertex_line_t));

	return true;
}

bool CTerrainChunk::PrepareDesignLine()
{
	// 显示设计线条的方式
	int show_design_line = m_pTerrain->GetShowDesignLine();

	if (show_design_line != m_nShowDesignLine)
	{
		if (m_pDesignLines)
		{
			CORE_FREE(m_pDesignLines, m_nDesignLineMemSize);
			m_pDesignLines = NULL;
			m_nDesignLineMemSize = 0;
			m_nDesignLineNum = 0;
		}

		m_nShowDesignLine = show_design_line;
	}

	if (m_pDesignLines)
	{
		return true;
	}
	
	size_t chunk_scale = m_pTerrain->GetChunkScale();
	size_t chunk_dims = m_pZone->GetChunkDims();
	// 显示分块边缘线
	bool show_edge = show_design_line > 1;
	// 显示单位格子线
	bool show_line = show_design_line > 2;
	// 要移除（不显示）的线条总数
	size_t total_remove_num = 0;
	bool to_show = true;

	m_nDesignLineNum = (chunk_scale + 1) * chunk_scale * 2;
	m_nDesignLineMemSize = sizeof(terrain_line_t) * m_nDesignLineNum * 2;
	m_pDesignLines = (terrain_line_t*)CORE_ALLOC(m_nDesignLineMemSize);
	
	terrain_line_t* plines = m_pDesignLines;
	// 区域边缘线的颜色
	unsigned int color_zone = COLOR_ARGB(192, 0, 255, 0);
	// 分块边缘线的颜色
	unsigned int color_edge = COLOR_ARGB(192, 255, 0, 0);
	// 单位格子线的颜色
	unsigned int color_line = COLOR_ARGB(128, 255, 255, 255);
	unsigned int color;
	
	for (size_t r = 0; r < (chunk_scale + 1); r++)
	{
		for (size_t c = 0; c < (chunk_scale + 1); c++)
		{
			FmVec3 v1 = GetVector(m_nLocalRow + (int)r, 
				m_nLocalCol + (int)c);
			FmVec3 v2 = GetVector(m_nLocalRow + (int)r, 
				m_nLocalCol + (int)c + 1);
			FmVec3 v3 = GetVector(m_nLocalRow + (int)r + 1, 
				m_nLocalCol + (int)c);
			
			// 高于地面
			v1.y += 0.01F;
			v2.y += 0.01F;
			v3.y += 0.01F;

			to_show = true;
			
			if (r == 0)
			{
				if (m_nRow == 0)
				{
					color = color_zone;
				}
				else
				{
					color = color_edge;

					if (!show_edge)
					{
						to_show = false;
					}
				}
			}
			else if (r == chunk_scale)
			{
				if (m_nRow == (chunk_dims - 1))
				{
					color = color_zone;
				}
				else
				{
					color = color_edge;

					if (!show_edge)
					{
						to_show = false;
					}
				}
			}
			else
			{
				color = color_line;

				if (!show_line)
				{
					to_show = false;
				}
			}
			
			if (c != chunk_scale && to_show)
			{
				INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
				plines++;
			}
			else if (c != chunk_scale)
			{
				total_remove_num++;
			}
			
			to_show = true;

			if (c == 0)
			{
				if (m_nCol == 0)
				{
					color = color_zone;
				}
				else
				{
					color = color_edge;

					if (!show_edge)
					{
						to_show = false;
					}
				}
			}
			else if (c == chunk_scale)
			{
				if (m_nCol == (chunk_dims - 1))
				{
					color = color_zone;
				}
				else
				{
					color = color_edge;

					if (!show_edge)
					{
						to_show = false;
					}
				}
			}
			else
			{
				color = color_line;

				if (!show_line)
				{
					to_show = false;
				}
			}
			
			if (r != chunk_scale && to_show)
			{
				INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
				plines++;
			}
			else if (r != chunk_scale)
			{
				total_remove_num++;
			}
		}
	}

	// 重新计算要显示的线条总数
	if (total_remove_num > m_nDesignLineNum)
	{
		Assert(0);

		m_nDesignLineNum = 0;
	}
	else
	{
		m_nDesignLineNum -= total_remove_num;
	}
	
	return true;
}

bool CTerrainChunk::PaintDesignLine()
{
	if (0 == m_nDesignLineNum)
	{
		// 当前Chunk没有要显示的设计线条
		return true;
	}

	IRender* pRender = m_pTerrain->GetRender();
	FmMat4 mat;

	FmMat4Identity(&mat);
	pRender->DrawDesignLine(mat, PRIMITIVE_LINELIST, 
		(unsigned int)m_nDesignLineNum, m_pDesignLines, sizeof(terrain_line_t));
	
	return true;
}

bool CTerrainChunk::PaintWalkable()
{
	CTerrainWalkable* pWalkable = m_pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return false;
	}

	float walk_size = m_pTerrain->GetCollideUnitSize();
	size_t unit_size = m_pTerrain->GetCollidePerUnit();
	size_t chunk_size = m_pTerrain->GetChunkScale() * unit_size;
	size_t grid_num = chunk_size * chunk_size;
	bool no_edge = m_pTerrain->GetShowDesignNoEdge();
	terrain_line_t* pTriangles = (terrain_line_t*)CORE_ALLOC(
		sizeof(terrain_line_t) * grid_num * 6);
	size_t r;
	unsigned int alpha = 255;
	// 水面范围
	terrain_line_t* plines = pTriangles;
	size_t triangle_num = 0;

	for (r = 0; r < chunk_size; ++r)
	{
		for (size_t c = 0; c < chunk_size; ++c)
		{
			size_t row = m_nLocalRow * unit_size + r;
			size_t col = m_nLocalCol * unit_size + c;
			float height = pWalkable->GetWaterHeight(row, col);

			if (height_is_null(height))
			{
				continue;
			}

			height += 0.1F;

			float x1, z1, x2, z2;

			if (no_edge)
			{
				x1 = m_pZone->GetLeft() + col * walk_size;
				z1 = m_pZone->GetTop() + row * walk_size;
				x2 = x1 + walk_size;
				z2 = z1 + walk_size;
			}
			else
			{
				x1 = m_pZone->GetLeft() + col * walk_size + 0.1F;
				z1 = m_pZone->GetTop() + row * walk_size + 0.1F;
				x2 = x1 + walk_size - 0.2F;
				z2 = z1 + walk_size - 0.2F;
			}

			FmVec3 v1(x1, height, z1);
			FmVec3 v2(x1, height, z2);
			FmVec3 v3(x2, height, z1);
			FmVec3 v4(x2, height, z2);
			unsigned int color = COLOR_ARGB(alpha, 0, 255, 255);

			INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v4.x, v4.y, v4.z, color);
			plines++;
			triangle_num += 2;
		}
	}

	IRender* pRender = m_pTerrain->GetRender();
	FmMat4 mat;

	FmMat4Identity(&mat);

	if (triangle_num > 0)
	{
		pRender->DrawDesignLine(mat, 0, 
			(unsigned int)triangle_num, pTriangles, sizeof(terrain_line_t));
	}

	// 层范围
	int floor_num = pWalkable->GetFloorCount();
	int floor;

	for (floor = 0; floor < floor_num; ++floor)
	{
		plines = pTriangles;
		triangle_num = 0;

		for (r = 0; r < chunk_size; ++r)
		{
			for (size_t c = 0; c < chunk_size; ++c)
			{
				size_t row = m_nLocalRow * unit_size + r;
				size_t col = m_nLocalCol * unit_size + c;
				unsigned int walk_marker = pWalkable->GetWalkMarker(row, col);
				unsigned int floor_marker = pWalkable->GetFloorMarker(row, 
					col, floor);

				if ((floor_marker & FLOOR_MARKER_EXISTS) == 0)
				{
					continue;
				}

				float height = pWalkable->GetFloorHeight(row, col, floor);

				if (height_is_null(height))
				{
					//height = m_pZone->GetHeight()->GetY(
					//	m_pZone->GetLeft() + (col + 0.5F) * walk_size, 
					//	m_pZone->GetTop() + (row + 0.5F) * walk_size);

					height = m_pTerrain->GetGroundHeight(
						m_pZone->GetLeft() + (col + 0.5F) * walk_size, 
						m_pZone->GetTop() + (row + 0.5F) * walk_size);
				}

				height += 0.1F;

				//float x1 = m_pZone->GetLeft() + col * walk_size + 0.1F;
				//float z1 = m_pZone->GetTop() + row * walk_size + 0.1F;
				//float x2 = x1 + walk_size - 0.2F;
				//float z2 = z1 + walk_size - 0.2F;

				float x1, z1, x2, z2;

				if (no_edge)
				{
					x1 = m_pZone->GetLeft() + col * walk_size;
					z1 = m_pZone->GetTop() + row * walk_size;
					x2 = x1 + walk_size;
					z2 = z1 + walk_size;
				}
				else
				{
					x1 = m_pZone->GetLeft() + col * walk_size + 0.1F;
					z1 = m_pZone->GetTop() + row * walk_size + 0.1F;
					x2 = x1 + walk_size - 0.2F;
					z2 = z1 + walk_size - 0.2F;
				}

				FmVec3 v1(x1, height, z1);
				FmVec3 v2(x1, height, z2);
				FmVec3 v3(x2, height, z1);
				FmVec3 v4(x2, height, z2);
				unsigned int color = COLOR_ARGB(alpha, 0, 0, 255);

				if ((floor_marker & FLOOR_MARKER_MOVE) == 0)
				{
					if ((floor_marker & FLOOR_MARKER_STAND) != 0)
					{
						// 可站立
						color = COLOR_ARGB(alpha, 192, 128, 0);
					}
					else
					{ 
						// 不可站立
						color = COLOR_ARGB(alpha, 128, 0, 0);
					}
				}

				if ((floor_marker & FLOOR_MARKER_WALL) != 0)
				{
					if ((floor_marker & FLOOR_MARKER_MOVE) != 0)
					{
						color = COLOR_ARGB(alpha, 255, 0, 255);
					}
					else
					{
						color = COLOR_ARGB(alpha, 128, 0, 255);
					}
				}

				if ((floor_marker & FLOOR_MARKER_MOVE) != 0)
				{
					if ((floor_marker & FLOOR_MARKER_STAND) == 0)
					{ 
						// 可行走不可站立
						color = COLOR_ARGB(alpha, 0, 128, 128);
					}
				}

				if (floor == (walk_marker - 1))
				{
					// 主行走层
					color = COLOR_ARGB(alpha, 0, 255, 0);
				}
				
				INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v4.x, v4.y, v4.z, color);
				plines++;
				triangle_num += 2;
			}
		}

		if (triangle_num > 0)
		{
			pRender->DrawDesignLine(mat, 0, 
				(unsigned int)triangle_num, pTriangles, sizeof(terrain_line_t));
		}
	}

	if (!m_pTerrain->GetShowSpaceHeight())
	{
		// 不显示空间高度
		CORE_FREE(pTriangles, sizeof(terrain_line_t) * grid_num * 6);
		return true;
	}

	// 无障碍空间高度
	for (floor = 0; floor < floor_num; ++floor)
	{
		plines = pTriangles;
		triangle_num = 0;

		for (r = 0; r < chunk_size; ++r)
		{
			for (size_t c = 0; c < chunk_size; ++c)
			{
				size_t row = m_nLocalRow * unit_size + r;
				size_t col = m_nLocalCol * unit_size + c;
				float space_height = pWalkable->GetFloorSpace(row, col, 
					floor);

				if (height_is_null(space_height))
				{
					continue;
				}

				bool error = false;

				if (space_height < 1.0F)
				{
					error = true;
				}

				if (space_height < 0.1F)
				{
					space_height = 0.1F;
				}

				float height = pWalkable->GetFloorHeight(row, col, floor);

				if (height_is_null(height) && (floor == 0))
				{
					//height = m_pZone->GetHeight()->GetY(
					//	m_pZone->GetLeft() + (col + 0.5F) * walk_size, 
					//	m_pZone->GetTop() + (row + 0.5F) * walk_size);

					height = m_pTerrain->GetGroundHeight(
						m_pZone->GetLeft() + (col + 0.5F) * walk_size, 
						m_pZone->GetTop() + (row + 0.5F) * walk_size);
				}

				if (height_is_null(height))
				{
					continue;
				}

				height += 0.1F;

				float x1 = m_pZone->GetLeft() + (col + 0.5F) * walk_size 
					+ floor * 0.02F;
				float z1 = m_pZone->GetTop() + (row + 0.5F) * walk_size 
					+ floor * 0.02F;
				FmVec3 v1(x1, height, z1);
				FmVec3 v2(x1, height + space_height, z1);
				unsigned int color = COLOR_ARGB(alpha, 255, 255, 
					(floor & 0x3) * 64);

				if (error)
				{
					color = COLOR_ARGB(alpha, 255, 0, 
						63 + (floor & 0x3) * 64);
				}

				INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
				plines++;
				triangle_num += 1;
			}
		}

		if (triangle_num > 0)
		{
			pRender->DrawDesignLine(mat, 0, 
				(unsigned int)triangle_num, pTriangles, sizeof(terrain_line_t));
		}
	}

	CORE_FREE(pTriangles, sizeof(terrain_line_t) * grid_num * 6);

	return true;
}

bool CTerrainChunk::PaintRegion(const char* region_name)
{
	CTerrainRegion* pRegion = m_pZone->GetRegion();
	IRender* pRender = m_pTerrain->GetRender();
	float unit_size = m_pTerrain->GetUnitSize();
	size_t chunk_scale = m_pTerrain->GetChunkScale();
	size_t grid_num = chunk_scale * chunk_scale;
	bool no_edge = m_pTerrain->GetShowDesignNoEdge();
	terrain_line_t* pTriangles = (terrain_line_t*)CORE_ALLOC(
		sizeof(terrain_line_t) * grid_num * 6);
	// 颜色
	unsigned int color = m_pTerrain->GetRegionColor(region_name);

	if (0 == color)
	{
		color = 0xFF00FFFF;
	}

	terrain_line_t* plines = pTriangles;
	size_t triangle_num = 0;

	for (size_t r = 0; r < chunk_scale; ++r)
	{
		for (size_t c = 0; c < chunk_scale; ++c)
		{
			size_t row = m_nLocalRow + r;
			size_t col = m_nLocalCol + c;

			if (!pRegion->GetRegionEnable(region_name, row, col))
			{
				continue;
			}

			float x1, z1, x2, z2;

			if (no_edge)
			{
				x1 = m_pZone->GetLeft() + col * unit_size;
				z1 = m_pZone->GetTop() + row * unit_size;
				x2 = x1 + unit_size;
				z2 = z1 + unit_size;
			}
			else
			{
				x1 = m_pZone->GetLeft() + col * unit_size + 0.05F;
				z1 = m_pZone->GetTop() + row * unit_size + 0.05F;
				x2 = x1 + unit_size - 0.1F;
				z2 = z1 + unit_size - 0.1F;
			}

			float y1 = m_pTerrain->GetPosiY(x1, z1) + 0.2F;
			float y2 = m_pTerrain->GetPosiY(x1, z2) + 0.2F;
			float y3 = m_pTerrain->GetPosiY(x2, z1) + 0.2F;
			float y4 = m_pTerrain->GetPosiY(x2, z2) + 0.2F;
			FmVec3 v1(x1, y1, z1);
			FmVec3 v2(x1, y2, z2);
			FmVec3 v3(x2, y3, z1);
			FmVec3 v4(x2, y4, z2);

			INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
			plines++;
			INIT_TERRAIN_LINE(plines, v4.x, v4.y, v4.z, color);
			plines++;
			triangle_num += 2;
		}
	}

	if (triangle_num > 0)
	{
		FmMat4 mat;

		FmMat4Identity(&mat);
		pRender->DrawDesignLine(mat, 0, 
			(unsigned int)triangle_num, pTriangles, sizeof(terrain_line_t));
	}

	CORE_FREE(pTriangles, sizeof(terrain_line_t) * grid_num * 6);

	return true;
}

bool CTerrainChunk::PaintArea()
{
	CTerrainRegion* pRegion = m_pZone->GetRegion();
	IRender* pRender = m_pTerrain->GetRender();
	float unit_size = m_pTerrain->GetUnitSize();
	size_t chunk_scale = m_pTerrain->GetChunkScale();
	size_t grid_num = chunk_scale * chunk_scale;
	bool no_edge = m_pTerrain->GetShowDesignNoEdge();
	terrain_line_t* pTriangles = (terrain_line_t*)CORE_ALLOC(
		sizeof(terrain_line_t) * grid_num * 6);
	// 地区标记
	size_t area_num = pRegion->GetAreaCount();

	for (size_t i = 0; i < area_num; ++i)
	{
		const char* name = pRegion->GetAreaName(i);
		// 颜色
		unsigned int color = m_pTerrain->GetAreaColor(name);

		if (0 == color)
		{
			color = 0xFF00FF80;
		}

		terrain_line_t* plines = pTriangles;
		size_t triangle_num = 0;

		for (size_t r = 0; r < chunk_scale; ++r)
		{
			for (size_t c = 0; c < chunk_scale; ++c)
			{
				size_t row = m_nLocalRow + r;
				size_t col = m_nLocalCol + c;

				if (!pRegion->GetAreaValue(i, row, col))
				{
					continue;
				}

				float x1, z1, x2, z2;

				if (no_edge)
				{
					x1 = m_pZone->GetLeft() + col * unit_size;
					z1 = m_pZone->GetTop() + row * unit_size;
					x2 = x1 + unit_size;
					z2 = z1 + unit_size;
				}
				else
				{
					x1 = m_pZone->GetLeft() + col * unit_size + 0.15F;
					z1 = m_pZone->GetTop() + row * unit_size + 0.15F;
					x2 = x1 + unit_size - 0.3F;
					z2 = z1 + unit_size - 0.3F;
				}

				float y1 = m_pTerrain->GetPosiY(x1, z1) + 0.25F;
				float y2 = m_pTerrain->GetPosiY(x1, z2) + 0.25F;
				float y3 = m_pTerrain->GetPosiY(x2, z1) + 0.25F;
				float y4 = m_pTerrain->GetPosiY(x2, z2) + 0.25F;
				FmVec3 v1(x1, y1, z1);
				FmVec3 v2(x1, y2, z2);
				FmVec3 v3(x2, y3, z1);
				FmVec3 v4(x2, y4, z2);

				INIT_TERRAIN_LINE(plines, v1.x, v1.y, v1.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v2.x, v2.y, v2.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v3.x, v3.y, v3.z, color);
				plines++;
				INIT_TERRAIN_LINE(plines, v4.x, v4.y, v4.z, color);
				plines++;
				triangle_num += 2;
			}
		}

		if (triangle_num > 0)
		{
			FmMat4 mat;

			FmMat4Identity(&mat);
			pRender->DrawDesignLine(mat, 0, 
				(unsigned int)triangle_num, pTriangles, sizeof(terrain_line_t));
		}
	}

	CORE_FREE(pTriangles, sizeof(terrain_line_t) * grid_num * 6);

	return true;
}

void CTerrainChunk::SetSaveChunkWalkPos(bool value)
{
	m_bSaveChunkWalkPos = value;
}

bool CTerrainChunk::GetSaveChunkWalkPos() const
{
	return m_bSaveChunkWalkPos;
}

bool CTerrainChunk::SaveChunkWalkPos(const char* file_name)
{
	if (!m_bSaveChunkWalkPos)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = m_pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		m_bSaveChunkWalkPos = false;
		return false;
	}

	FILE* pFile = core_file::fopen(file_name, "ab");

	if (NULL == pFile)
	{
		m_bSaveChunkWalkPos = false;
		return false;
	}

	float walk_size = m_pTerrain->GetCollideUnitSize();
	size_t unit_size = m_pTerrain->GetCollidePerUnit();
	size_t chunk_size = m_pTerrain->GetChunkScale() * unit_size;
	unsigned int alpha = 255;
	// 层范围
	int floor_num = pWalkable->GetFloorCount();

	for (int floor = 0; floor < floor_num; ++floor)
	{
		for (size_t r = 0; r < chunk_size; ++r)
		{
			for (size_t c = 0; c < chunk_size; ++c)
			{
				size_t row = m_nLocalRow * unit_size + r;
				size_t col = m_nLocalCol * unit_size + c;
				unsigned int walk_marker = pWalkable->GetWalkMarker(row, col);
				unsigned int floor_marker = pWalkable->GetFloorMarker(row, 
					col, floor);

				if ((floor_marker & FLOOR_MARKER_EXISTS) == 0)
				{
					continue;
				}

				float height = pWalkable->GetFloorHeight(row, col, floor);

				if (height_is_null(height))
				{
					//height = m_pZone->GetHeight()->GetY(
					//	m_pZone->GetLeft() + (col + 0.5F) * walk_size, 
					//	m_pZone->GetTop() + (row + 0.5F) * walk_size);

					height = m_pTerrain->GetGroundHeight(
						m_pZone->GetLeft() + (col + 0.5F) * walk_size, 
						m_pZone->GetTop() + (row + 0.5F) * walk_size);
				}

				float x = m_pZone->GetLeft() + col * walk_size + walk_size / 2;
				float z = m_pZone->GetTop() + row * walk_size + walk_size / 2;

				if ((floor_marker & FLOOR_MARKER_WALL) != 0)
				{
					continue;
				}

				if ((floor_marker & FLOOR_MARKER_MOVE) == 0)
				{
					if ((floor_marker & FLOOR_MARKER_STAND) != 0)
					{
						// 可站立
						continue;
					}
					else
					{ 
						// 不可站立
						continue;
					}
				}

				// Save
				FmVec3 pos(x, height, z);

				core_file::fwrite(&pos, sizeof(pos), 1, pFile);
			}
		}
	}

	if (pFile)
	{
		core_file::fclose(pFile);
		pFile = NULL;
	}

	m_bSaveChunkWalkPos = false;

	return true;
}

void CTerrainChunk::SetChunkShader(CTerrainChunk::chunk_cb_object_shader_handle_t* pShaderHandle)
{
	m_pChunkShaderHandle = pShaderHandle;
}

CTerrainChunk::chunk_cb_object_shader_handle_t* CTerrainChunk::GetChunkShader() const
{
	return m_pChunkShaderHandle;
}

void CTerrainChunk::SetMaterial(Terrain::material_t* pMaterial)
{
	Terrain::material_t& mat = *pMaterial;
	FmVec4 mat_specular = FmVec4(mat.vSpecular.x, 
		mat.vSpecular.y, mat.vSpecular.z, mat.fSpecularPower);

	m_chunk_cb_per_group.MaterialAmbient = mat.vAmbient;
	m_chunk_cb_per_group.MaterialDiffuse = mat.vDiffuse;
	m_chunk_cb_per_group.MaterialSpecular = mat_specular;
}



//设置渲染的常量数据
void CTerrainChunk::SetChunkGroupCB(FmVec4 terrain_block_size,FmVec2 gradually,FmVec2 detail0_param,FmVec4 tess_factor)
{
	m_chunk_cb_per_group.TerrainBlockSize =  terrain_block_size;
	m_chunk_cb_per_group.GraduallyDist = gradually;
	m_chunk_cb_per_group.TessellationFactor = tess_factor;
	m_chunk_cb_per_group.Detail0Param = detail0_param;
}

//开始设置常量数据
void CTerrainChunk::UseChunkGroupCB()
{
	IShaderParamOp* pShaderOp = m_pChunkShaderHandle->pShader->GetParamOp();
	float fTerrainBlockSize = 1.0f / m_chunk_cb_per_group.TerrainBlockSize.x;
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fTerrainBlockSizeHandle,fTerrainBlockSize);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4TerrainBlockSizeHandle,m_chunk_cb_per_group.TerrainBlockSize);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec2GraduallyDisthandle,m_chunk_cb_per_group.GraduallyDist);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec2Detail0ParamHandle,m_chunk_cb_per_group.Detail0Param);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4TessellationFactorHandle,m_chunk_cb_per_group.TessellationFactor);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4MaterialAmbientHandle,m_chunk_cb_per_group.MaterialAmbient);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4MaterialDiffuseHandle,m_chunk_cb_per_group.MaterialDiffuse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4MaterialSpecularHandle,m_chunk_cb_per_group.MaterialSpecular);
}

void CTerrainChunk::SetChunkNormalSceneCB()
{
    IRenderContext* pContext = m_pRender->GetContext();
    const camera_t& camera = pContext->GetCamera();
	const FmVec3& refer_pos = pContext->GetReferPosition();
	const FmVec3& camera_pos = camera.vPosition;

	// 摄像机位置
	FmVec4 view_pos(camera_pos.x - refer_pos.x, 
		camera_pos.y - refer_pos.y, camera_pos.z - refer_pos.z, 1.0F);
	FmVec4 light_dir = pContext->GetVector4(IRenderContext::V_LIGHT_DIRECTION);
	FmVec4 light_ambient = pContext->GetVector4(
		IRenderContext::V_LIGHT_AMBIENT);
	FmVec4 light_diffuse = pContext->GetVector4(
		IRenderContext::V_LIGHT_DIFFUSE);
	float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
	float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END);
	float fog_height = pContext->GetFloat(IRenderContext::F_FOG_HEIGHT);
	float fog_density = pContext->GetFloat(IRenderContext::F_FOG_DENSITY);
	FmVec4 fog_param(fog_start, 1.0F / (fog_end - fog_start), fog_height, 
		0.0F);
	FmVec4 fog_color = pContext->GetVector4(IRenderContext::V_FOG_CURRENT);
	FmVec4 fog_exp_param = pContext->GetVector4(IRenderContext::V_FOG_EXP_CURRENT);
	FmMat4 mtxViewProj;
	FmMat4 mtxView;
	FmMat4 mtxProj;
	FmMat4 mtxViewProjInverse;
	FmMat4 mtxViewInverse;
	FmMat4 mtxProjInverse;

	FmMat4Transpose(&mtxViewProj, &camera.mtxViewProjRelative);
	FmMat4Transpose(&mtxView, &camera.mtxViewRelative);
	FmMat4Transpose(&mtxProj, &camera.mtxProj);
	FmMat4Inverse(&mtxViewProjInverse, NULL, &camera.mtxViewProjRelative);
	FmMat4Inverse(&mtxViewInverse, NULL, &camera.mtxViewRelative);
	FmMat4Inverse(&mtxProjInverse, NULL, &camera.mtxProj);
	FmMat4Transpose(&mtxViewProjInverse, &mtxViewProjInverse);
	FmMat4Transpose(&mtxViewInverse, &mtxViewInverse);
	FmMat4Transpose(&mtxProjInverse, &mtxProjInverse);
	//const viewport_t& vp = camera.Viewport;
	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	FmVec2 pixel_size(1.0F / (float)view_width, 1.0F / (float)view_height);
	FmVec4 half_pixel_size(0.5F / (float)view_width, 
		0.5F / (float)view_height, 0.0F, 0.0F);
	FmVec4 r_pos(refer_pos.x, refer_pos.y, refer_pos.z, 0.0F);
	float alpha_ref = m_pRender->GetRenderAlphaRef() / 255.0F;
	float factor = camera.fNearZ * camera.fFarZ / 1024.0F;
	FmVec2 depth_param = FmVec2( camera.fFarZ / factor, (camera.fFarZ - camera.fNearZ) / factor);

	FmVec4 clip_plane = pContext->GetVector4(IRenderContext::V_CLIP_PLANE);

	 
	unsigned int role_light_color = pContext->GetColor(IRenderContext::C_ROLE_LIGHT_COLOR);
	float role_light_inten = pContext->GetFloat(IRenderContext::F_ROLE_LIGHT_INTENSITY);
	FmVec3 role_light_pos = pContext->GetVector(IRenderContext::V_ROLE_LIGHT_POSITION);
	float role_light_range = pContext->GetFloat(IRenderContext::F_ROLE_LIGHT_RANGE);

	FmVec4 light_color = VisUtil_GetColor2(role_light_color, role_light_inten);


	FmVec3 light_position= FmVec3( role_light_pos.x - refer_pos.x, role_light_pos.y - refer_pos.y, role_light_pos.z - refer_pos.z);

	unsigned int camera_light_color = pContext->GetColor(IRenderContext::C_CAMERA_LIGHT_COLOR);
	  
	FmVec4 camera_light = VisUtil_ColorToVector4(camera_light_color);
 
	float heightfog_start = pContext->GetFloat(IRenderContext::F_HEIGHT_FOG_START);
	float heightfog_end = pContext->GetFloat(IRenderContext::F_HEIGHT_FOG_END);
	FmVec4 height_fog_param = FmVec4(heightfog_start, 1.0f / (heightfog_end - heightfog_start), 0.0f, 0.0f);

	FmVec4 height_fog_color = pContext->GetVector4(IRenderContext::V_HEIGHTFOG_CURRENT);

	// m_chunk_cb_per_scene
	m_chunk_cb_per_scene.mtxViewProj = mtxViewProj;
	m_chunk_cb_per_scene.mtxView = mtxView;
	m_chunk_cb_per_scene.mtxProj = mtxProj;
	m_chunk_cb_per_scene.mtxViewProjInverse = mtxViewProjInverse;
	m_chunk_cb_per_scene.mtxViewInverse = mtxViewInverse;
	m_chunk_cb_per_scene.mtxProjInverse = mtxProjInverse;
	m_chunk_cb_per_scene.vViewPos = view_pos;
	m_chunk_cb_per_scene.vLightDir = light_dir;
	m_chunk_cb_per_scene.LightAmbient = light_ambient;
	m_chunk_cb_per_scene.LightDiffuse = light_diffuse;
	m_chunk_cb_per_scene.FogColor = fog_color;
	m_chunk_cb_per_scene.FogParam = fog_param;

	m_chunk_cb_per_scene.FogExpParam = fog_exp_param;
	m_chunk_cb_per_scene.vReferPos = r_pos;
	m_chunk_cb_per_scene.DepthParam =  depth_param;
	m_chunk_cb_per_scene.fAlphaRef = alpha_ref;
	m_chunk_cb_per_scene.fFarClipDistance = pContext->GetFarClipDistance();
	m_chunk_cb_per_scene.ClipPlane = clip_plane;

	if (pContext->GetEnableDynamicShadow())
	{
		FmMat4MultiplyTranspose(&m_chunk_cb_per_scene.mtxShadowViewProj, pContext->GetShadowMapViewMatrix(0), pContext->GetShadowMapProjMatrix(0));
	}

	m_chunk_cb_per_scene.vHeightFogColor = height_fog_color;
	m_chunk_cb_per_scene.vHeightFogParam = height_fog_param;
	 
	m_chunk_cb_per_scene.vPointLightPos = light_position;
	m_chunk_cb_per_scene.fPointLightRange = role_light_range;
	m_chunk_cb_per_scene.CameraLightDiffuse = camera_light;
	m_chunk_cb_per_scene.fCameraNearZ = camera.fNearZ;
	m_chunk_cb_per_scene.PixelSize = pixel_size;
	m_chunk_cb_per_scene.fStereoConvergence =  0.0f;//m_pRender->GetStereoConvergence();
	m_chunk_cb_per_scene.BlendHalfPixel = FmVec2(0,0);
}

// 设置区块使用的光贴图
void CTerrainChunk::SetZoneLightMap(ITexture* pLightMap)
{
	m_pZoneLightMap = pLightMap;
}

//设置场景的寄存器值
void CTerrainChunk::UseNormalSceneCB()
{
	IShaderParamOp* pShaderOp = m_pChunkShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4mtxViewProjHandle,m_chunk_cb_per_scene.mtxViewProj);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4mtxViewHandle,m_chunk_cb_per_scene.mtxView);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4mtxProjHandle,m_chunk_cb_per_scene.mtxProj);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4mtxProjInverseHandle,m_chunk_cb_per_scene.mtxViewProjInverse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4mtxViewInverseHandle,m_chunk_cb_per_scene.mtxViewInverse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4mtxProjInverseHandle,m_chunk_cb_per_scene.mtxProjInverse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3ViewPosHandle,m_chunk_cb_per_scene.vViewPos);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3LightDirHandle,m_chunk_cb_per_scene.vLightDir);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3LightAmbientHandle,m_chunk_cb_per_scene.LightAmbient);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fReflectFactorHandle,m_chunk_cb_per_scene.fReflectFactor);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3LightDiffuseHandle,m_chunk_cb_per_scene.LightDiffuse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4FogColorHandle,m_chunk_cb_per_scene.FogColor);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4FogParamHandle,m_chunk_cb_per_scene.FogParam);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4FogExpParamHandle,m_chunk_cb_per_scene.FogExpParam);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec2PixelSizeHandle,m_chunk_cb_per_scene.PixelSize);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fStereoConvergenceHandle,m_chunk_cb_per_scene.fStereoConvergence);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fCameraNearZHandle,m_chunk_cb_per_scene.fCameraNearZ);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec2HalfPixelSizeHandle,m_chunk_cb_per_scene.HalfPixelSize);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec2BlendHalfPixeHandle,m_chunk_cb_per_scene.HalfPixelSize);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3ReferPosHandle,m_chunk_cb_per_scene.vReferPos);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec2DepthParamHandle,m_chunk_cb_per_scene.DepthParam);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fFarClipDistanceHandle,m_chunk_cb_per_scene.fFarClipDistance);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fAlphaRefHandle,m_chunk_cb_per_scene.fAlphaRef);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3PointLightPosHandle,m_chunk_cb_per_scene.vPointLightPos);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fPointLightRangeHandle,m_chunk_cb_per_scene.fPointLightRange);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4PointLightDiffuseHandle,m_chunk_cb_per_scene.PointLightDiffuse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4CameraLightDiffuseHandle,m_chunk_cb_per_scene.CameraLightDiffuse);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4ClipPlaneHandle,m_chunk_cb_per_scene.ClipPlane);   
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4ShadowViewProjHandle, m_chunk_cb_per_scene.mtxShadowViewProj);   
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4HeightFogColor, m_chunk_cb_per_scene.vHeightFogColor);  
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4HeightFogParam, m_chunk_cb_per_scene.vHeightFogParam);  

//   UseNormalSceneObject(m_pShader);
}

//使用纹理设置
void CTerrainChunk::UseTexture()
{
	IShaderParamOp* pShaderOp = m_pChunkShaderHandle->pShader->GetParamOp();

	bool enable_normalmap = m_pTerrain->GetEnableNormalMap();
 
	// 是否使用光贴图
	bool light_map = (m_pZoneLightMap && (m_pZoneLightMap->IsReady()?true:false));

	bool use_normal = enable_normalmap && (!light_map);
	// 扫描每个zone
	CBlendTexSet* pBaseTexSet = m_pTerrain->GetBaseTexSet();
	CBlendTexSet* pBlendTexSet = m_pTerrain->GetBlendTexSet();

	// 设置基础纹理
	int base_tex_index = GetBaseTexIndex();

	Assert(base_tex_index != -1);

	CBlendTex* pBaseTex = pBaseTexSet->GetByIndex(base_tex_index);
	ITexture* pNormalTex = pBaseTex->GetNormalMap();

	int texnum = 0;

	if(light_map)
	{
		ITextureSampler* tex_LightmapSampler = m_pZoneLightMap->GetCanUseShaderTex()->GetTextureSampler();
		tex_LightmapSampler->SetTextureUVWrapMode(ITextureSampler::TWM_CLAMP_TO_EDGE,ITextureSampler::TWM_CLAMP_TO_EDGE);

		pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_LightmapHandle, m_pZoneLightMap->GetCanUseShaderTex()->GetTexture());

		FmVec3 vec3LightMapColorScale = m_pZone->GetLight()->GetLightConfig() * 2.0f;
		pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3LightMapColorScaleHandle, vec3LightMapColorScale);
	}

	if(pBaseTex)
	{
		ITextureSampler* tex_Detailmap0Sampler = pBaseTex->GetTex()->GetCanUseShaderTex()->GetTextureSampler();
		tex_Detailmap0Sampler->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);

		pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Detailmap0Handle,pBaseTex->GetTex()->GetCanUseShaderTex()->GetTexture());
		if(use_normal&&pNormalTex)
		{
			pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Bumpmap0Handle,pNormalTex->GetCanUseShaderTex()->GetTexture());
		}

	}

	IRenderContext* pContext = m_pRender->GetContext();
	if (pContext->GetEnableDynamicShadow())
	{
		IColorRT* pShadowRT = pContext->GetDynamicShadowRT();
		
		if (pShadowRT)
		{
			pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_ShadowHandle, pShadowRT->GetTexture());
			pShadowRT->GetTextureSampler()->SetTextureUVWrapMode(ITextureSampler::TWM_CLAMP_TO_EDGE, ITextureSampler::TWM_CLAMP_TO_EDGE);
		}
	}

	if(m_blend_num > 0)
	{
		IStaticTex* pAlphaTex = GetAlphaTex();

		if (NULL == pAlphaTex)
		{
			pAlphaTex = CreateAlphaTex(m_pRender);

			if (NULL == pAlphaTex)
			{
				return;
			}
		}


		pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_BlendmapHandle,pAlphaTex->GetTexture());

		const CTerrainChunk::blend_tex_t* blend = GetBlendTexs();
		for (unsigned int m = 0; m < m_blend_num; ++m)
		{
			CBlendTex* pBlendTex = pBlendTexSet->GetByIndex(
				blend[m].nTexIndex);
			if (use_normal)
			{
				ITexture* pTex = pBlendTex->GetNormalMap();

				if(pTex)
				{
					switch(m+1)
					{
					case 1:
						pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Bumpmap1Handle,pTex->GetCanUseShaderTex()->GetTexture());
						break;
					case 2:
						pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Bumpmap2Handle,pTex->GetCanUseShaderTex()->GetTexture());
						break;
					case 3:
						pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Bumpmap3Handle,pTex->GetCanUseShaderTex()->GetTexture());
						break;
					case 4:
						pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Bumpmap4Handle,pTex->GetCanUseShaderTex()->GetTexture());
						break;
					};

				}
			}

			if(pBlendTex)
			{
	 			ITextureSampler* tex_DetailmapSampler = pBlendTex->GetTex()->GetCanUseShaderTex()->GetTextureSampler();
	 			tex_DetailmapSampler->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);
				switch(m+1)
				{
				case 1:
					pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Detailmap1Handle,pBlendTex->GetTex()->GetCanUseShaderTex()->GetTexture());
					break;
				case 2:
					pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Detailmap2Handle,pBlendTex->GetTex()->GetCanUseShaderTex()->GetTexture());
					break;
				case 3:
					pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Detailmap3Handle,pBlendTex->GetTex()->GetCanUseShaderTex()->GetTexture());
					break;
				case 4:
					pShaderOp->SetTexture2D(m_pChunkShaderHandle->tex_Detailmap4Handle,pBlendTex->GetTex()->GetCanUseShaderTex()->GetTexture());
					break;
				};
			}
		}
	}
}


void CTerrainChunk::SetGrassShader(IShaderProgram* pShader,IStaticIB* pIB,IStaticVB* pVB,size_t grass_num,CTerrainChunk::grass_shader_handle_t* pShaderHandle)
{
	m_pGrassShader = pShader;
    m_pGrassIB = pIB;
    m_pGrassVB = pVB;
    m_grass_num = grass_num;
	m_pGrassShaderHandle = pShaderHandle;
}

void CTerrainChunk::SetGrassTex(ITexture* pTex0)
{
	 m_pGrass_Tex0 = pTex0;
}

// 设置草渲染相关
void CTerrainChunk::SetGrassPerGroup(CTerrainChunk::chunk_grass_cb_per_group_t& grass_cb_per_group)
{
   m_grass_cb_per_group = grass_cb_per_group;
}

void CTerrainChunk::UseGrassObject()
{
	IShaderParamOp* pShaderOp = m_pGrassShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3WinDirHandle,m_grass_cb_per_group.vWinDir);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fWinForceHandle,m_grass_cb_per_group.fWinForce);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fTimerHandle,m_grass_cb_per_group.fTimer);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec2FadeParamHandle,m_grass_cb_per_group.FadeParam);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3PlayerPointHandle,m_grass_cb_per_group.vPlayerPoint);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4MaterialDiffuseHandle,m_grass_cb_per_group.MaterialDiffuse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4MaterialAmbientHandle,m_grass_cb_per_group.MaterialAmbient);

}

void CTerrainChunk::UseGrassNormalSceneCB()
{ 
	IShaderParamOp* pShaderOp = m_pGrassShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4mtxViewProjHandle,m_chunk_cb_per_scene.mtxViewProj);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4mtxViewHandle,m_chunk_cb_per_scene.mtxView);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4mtxProjHandle,m_chunk_cb_per_scene.mtxProj);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4mtxProjInverseHandle,m_chunk_cb_per_scene.mtxViewProjInverse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4mtxViewInverseHandle,m_chunk_cb_per_scene.mtxViewInverse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4mtxProjInverseHandle,m_chunk_cb_per_scene.mtxProjInverse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3ViewPosHandle,m_chunk_cb_per_scene.vViewPos);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3LightDirHandle,m_chunk_cb_per_scene.vLightDir);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3LightAmbientHandle,m_chunk_cb_per_scene.LightAmbient);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fReflectFactorHandle,m_chunk_cb_per_scene.fReflectFactor);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3LightDiffuseHandle,m_chunk_cb_per_scene.LightDiffuse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4FogColorHandle,m_chunk_cb_per_scene.FogColor);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4FogParamHandle,m_chunk_cb_per_scene.FogParam);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4FogExpParamHandle,m_chunk_cb_per_scene.FogExpParam);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec2PixelSizeHandle,m_chunk_cb_per_scene.PixelSize);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fStereoConvergenceHandle,m_chunk_cb_per_scene.fStereoConvergence);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fCameraNearZHandle,m_chunk_cb_per_scene.fCameraNearZ);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec2HalfPixelSizeHandle,m_chunk_cb_per_scene.HalfPixelSize);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec2BlendHalfPixeHandle,m_chunk_cb_per_scene.HalfPixelSize);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3ReferPosHandle,m_chunk_cb_per_scene.vReferPos);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec2DepthParamHandle,m_chunk_cb_per_scene.DepthParam);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fFarClipDistanceHandle,m_chunk_cb_per_scene.fFarClipDistance);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fAlphaRefHandle,m_chunk_cb_per_scene.fAlphaRef);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3PointLightPosHandle,m_chunk_cb_per_scene.vPointLightPos);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fPointLightRangeHandle,m_chunk_cb_per_scene.fPointLightRange);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4PointLightDiffuseHandle,m_chunk_cb_per_scene.PointLightDiffuse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4CameraLightDiffuseHandle,m_chunk_cb_per_scene.CameraLightDiffuse);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4ClipPlaneHandle,m_chunk_cb_per_scene.ClipPlane);
//	UseNormalSceneObject(m_pGrassShader);
}

void CTerrainChunk::UseGrassChunkObject()
{
	IShaderParamOp* pShaderOp = m_pGrassShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4Detail1ParamHandle,m_chunk_cb_obj.Detail1Param);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4Detail2ParamHandle,m_chunk_cb_obj.Detail2Param);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4Detail3ParamHandle,m_chunk_cb_obj.Detail3Param);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->mat4Detail4ParamHandle,m_chunk_cb_obj.Detail4Param);

	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec3ChunkPosHandle,m_chunk_cb_obj.vChunkPos);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4ZoneOffsetAndSizeHandle,m_chunk_cb_obj.ZoneOffsetAndSize);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fBaseSpecularLevelHandle,m_chunk_cb_obj.fBaseSpecularLevel);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fBaseSpecularPowerHandle,m_chunk_cb_obj.fBaseSpecularPower);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->fBaseHeightScaleHandle,m_chunk_cb_obj.fBaseHeightScale);

	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4BlendSpecularLevelHandle,m_chunk_cb_obj.BlendSpecularLevel);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4BlendSpecularPowerHandle,m_chunk_cb_obj.BlendSpecularPower);
	pShaderOp->SetParamValue(m_pGrassShaderHandle->vec4BlendHeightScaleHandle,m_chunk_cb_obj.BlendHeightScale);
}

void CTerrainChunk::UseGrassTexture()
{
	IShaderParamOp* pShaderOp = m_pGrassShaderHandle->pShader->GetParamOp();
	pShaderOp->SetTexture2D(m_pGrassShaderHandle->tex_DiffuseHandle,m_pGrass_Tex0->GetCanUseShaderTex()->GetTexture());
}

struct grass_vertex_t
{
	float x;
	float y;
	float z;
	float tu;
	float tv;
	float height;		// 顶点相对于底部的高度
	float angle;		// 方向角度
};

#define grass_vertex_t_iPos          0
#define grass_vertex_t_iTex0         1
#define grass_vertex_t_iHeightAngle  2


void CTerrainChunk::DrawGrass(IShaderProgram* pShader)
{
	IRenderDrawOp* pRenderDrawOp = pShader->GetRenderDrawOp();
	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();

//	pRenderStateOp->SetCullFaceModel();
//	pRenderStateOp->EnableCullFace(true);

	pRenderDrawOp->SetVB( m_pGrassVB->GetBuffer());
	pRenderDrawOp->SetIB( m_pGrassIB->GetBuffer());

	pRenderDrawOp->EnableVertexAttribArray(grass_vertex_t_iPos,true);
	pRenderDrawOp->SetVertexAttribPointer(grass_vertex_t_iPos,3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(grass_vertex_t),0);
    
	pRenderDrawOp->EnableVertexAttribArray(grass_vertex_t_iTex0,true);
	pRenderDrawOp->SetVertexAttribPointer(grass_vertex_t_iTex0,3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(grass_vertex_t),(void*) (3 * sizeof(float)));

	pRenderDrawOp->EnableVertexAttribArray( grass_vertex_t_iHeightAngle ,true);
	pRenderDrawOp->SetVertexAttribPointer( grass_vertex_t_iHeightAngle, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(grass_vertex_t),(void*) (5 * sizeof(float)));

	pRenderDrawOp->EnableVertexAttribArray( 3 ,false);

	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,(unsigned int)m_grass_num * 6,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,0);
}

void CTerrainChunk::SetWaterPerGroup(CTerrainChunk::chunk_water_cb_per_group_t& cb_per)
{
	m_water_cb_per_group = cb_per;
}

void CTerrainChunk::UseChunkObject()
{
	IShaderParamOp* pShaderOp = m_pChunkShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4Detail1ParamHandle,m_chunk_cb_obj.Detail1Param);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4Detail2ParamHandle,m_chunk_cb_obj.Detail2Param);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4Detail3ParamHandle,m_chunk_cb_obj.Detail3Param);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->mat4Detail4ParamHandle,m_chunk_cb_obj.Detail4Param);

	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec3ChunkPosHandle,m_chunk_cb_obj.vChunkPos);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4ZoneOffsetAndSizeHandle,m_chunk_cb_obj.ZoneOffsetAndSize);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fBaseSpecularLevelHandle,m_chunk_cb_obj.fBaseSpecularLevel);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fBaseSpecularPowerHandle,m_chunk_cb_obj.fBaseSpecularPower);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->fBaseHeightScaleHandle,m_chunk_cb_obj.fBaseHeightScale);

	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4BlendSpecularLevelHandle,m_chunk_cb_obj.BlendSpecularLevel);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4BlendSpecularPowerHandle,m_chunk_cb_obj.BlendSpecularPower);
	pShaderOp->SetParamValue(m_pChunkShaderHandle->vec4BlendHeightScaleHandle,m_chunk_cb_obj.BlendHeightScale);
}

void CTerrainChunk::SetWaterTexture(bool use_refraction,bool use_reflection,IColorRT* ReflectionTex,
									ITexture* WaterNormalMap1,ITexture* WaterNormalMap2,ITexture* WaterNormalMap3,ITexture* WaterNormalMap4,
									IColorRT* SceneRefract,IColorRT* DepthRT,ITexture* WhiteWaterMap,IDepthRT* ReflectDepthRT)
{
	m_water_tex.use_refraction = use_refraction;
	m_water_tex.use_reflection = use_reflection;
    m_water_tex.ReflectionTex = ReflectionTex;
	m_water_tex.WaterNormalMap1 = WaterNormalMap1;
    m_water_tex.WaterNormalMap2 = WaterNormalMap2;
	m_water_tex.WaterNormalMap3 = WaterNormalMap3;
    m_water_tex.WaterNormalMap4 = WaterNormalMap4;

    m_water_tex.SceneRefract = SceneRefract;
	m_water_tex.DepthRT = DepthRT;
    m_water_tex.WhiteWaterMap = WhiteWaterMap;
 	m_water_tex.ReflectDepthRT = ReflectDepthRT;
}

//绘制水体相关的参数传递
void CTerrainChunk::SetChunkObject()
{
 	size_t blend_num = this->GetBlendTexCount();

	if (blend_num > 4)
	{
		blend_num = 4;
	}

	const FmVec3& chunk_min = this->GetMin();
	CTerrainZone* pZone = this->GetZone();
	
	m_chunk_cb_obj.vChunkPos = FmVec4(chunk_min.x, chunk_min.y, chunk_min.z,
		0.0F);
	// 设置区域的世界坐标偏移值以及区域大小
	m_chunk_cb_obj.ZoneOffsetAndSize= FmVec4(pZone->GetLeft(), 
		pZone->GetTop(), m_pTerrain->GetZoneWidth(), 
		m_pTerrain->GetZoneHeight());

	// 设置基础纹理
	int base_tex_index = this->GetBaseTexIndex();

	Assert(base_tex_index != -1);

	CBlendTexSet* pBaseTexSet = m_pTerrain->GetBaseTexSet();
	CBlendTex* pBaseTex = pBaseTexSet->GetByIndex(base_tex_index);
	float base_specular_level = pBaseTex->GetSpecularLevel();
	float base_specular_power = pBaseTex->GetSpecularPower();
	float base_height_scale = pBaseTex->GetHeightScale();

	if (base_specular_level <= 0.0F)
	{
		base_specular_level = 1.0F;
	}

	if (base_specular_power <= 0.0F)
	{
		base_specular_power = m_pTerrain->GetMaterial().fSpecularPower;
	}

	if (base_height_scale <= 0.0F)
	{
		base_height_scale = 0.2F;
	}

	m_chunk_cb_obj.fBaseSpecularLevel = base_specular_level;
	m_chunk_cb_obj.fBaseSpecularPower = base_specular_power;
	m_chunk_cb_obj.fBaseHeightScale = base_height_scale;

	if (blend_num > 0)
	{
		CBlendTexSet* pBlendTexSet = m_pTerrain->GetBlendTexSet();
		const CTerrainChunk::blend_tex_t* blend = this->GetBlendTexs();
		float blend_specular_level[4] = { 1.0F, 1.0F, 1.0F, 1.0F };
		float blend_specular_power[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		float blend_height_scale[4] = { 0.2F, 0.2F, 0.2F, 0.2F };

		for (int m = 0; m < (int)blend_num; m++)
		{
			CBlendTex* pBlendTex = pBlendTexSet->GetByIndex(
				blend[m].nTexIndex);
			float blend_sp_level = pBlendTex->GetSpecularLevel();
			float blend_sp_power = pBlendTex->GetSpecularPower();
			float blend_h_scale = pBlendTex->GetHeightScale();

			if (blend_sp_level > 0.0F)
			{
				blend_specular_level[m] = blend_sp_level;
			}

			if (blend_sp_power > 0.0F)
			{
				blend_specular_power[m] = blend_sp_power;
			}
			else
			{
				blend_specular_power[m] = 
					m_pTerrain->GetMaterial().fSpecularPower;
			}

			if (blend_h_scale > 0.0F)
			{
				blend_height_scale[m] = blend_h_scale;
			}

			// 设置细节纹理变换矩阵
			const CBlendTex::format_t* format = 
				pBlendTex->GetFormatByIndex(blend[m].nFormat);
			FmMat4 mtxUVTransform = format->mtxTex;

			mtxUVTransform._41 = 32.0F * this->GetRow() 
				/ (format->nScaleU * 0.01F);
			mtxUVTransform._42 = 32.0F * this->GetCol() 
				/ (format->nScaleV * 0.01F);
			FmMat4Transpose(&mtxUVTransform, &mtxUVTransform);

			switch(m)
			{
			case 0:
                 m_chunk_cb_obj.Detail1Param = mtxUVTransform;
				break;
			case 1:
				 m_chunk_cb_obj.Detail2Param = mtxUVTransform;
				break;
			case 2:
                 m_chunk_cb_obj.Detail3Param = mtxUVTransform;
				break;
			case 3:
                 m_chunk_cb_obj.Detail4Param = mtxUVTransform;
				 break;
			default:
				break;
			};
		}

		m_chunk_cb_obj.BlendSpecularLevel = blend_specular_level;
		m_chunk_cb_obj.BlendSpecularPower = blend_specular_power;
        m_chunk_cb_obj.BlendHeightScale = blend_height_scale;
	}
}

void CTerrainChunk::UseWaterPerGroup()
{
    
	IShaderParamOp* pShaderOp = m_pWaterShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pWaterShaderHandle->mat4mtxViewProjHandle,m_water_cb_per_group.mtxViewProj);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fTimeHandle,m_water_cb_per_group.times);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4ViewPositionHandle,m_water_cb_per_group.view_pos);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam0Handle,m_water_cb_per_group.fWaveParam0);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam1Handle,m_water_cb_per_group.fWaveParam1);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam2Handle,m_water_cb_per_group.fWaveParam2);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam3Handle,m_water_cb_per_group.fWaveParam3);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam4Handle,m_water_cb_per_group.fWaveParam4);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam5Handle,m_water_cb_per_group.fWaveParam5);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam6Handle,m_water_cb_per_group.fWaveParam6);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4WaveParam7Handle,m_water_cb_per_group.fWaveParam7);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->fNormalMapScale0Handle,m_water_cb_per_group.fNormalMapScale0);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fNormalMapScale1Handle,m_water_cb_per_group.fNormalMapScale1);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fNormalMapScale2Handle,m_water_cb_per_group.fNormalMapScale2);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fNormalMapScale3Handle,m_water_cb_per_group.fNormalMapScale3);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2NormalmapSpeed0Handle,m_water_cb_per_group.fNormalmapSpeed0);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2NormalmapSpeed1Handle,m_water_cb_per_group.fNormalmapSpeed1);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2NormalmapSpeed2Handle,m_water_cb_per_group.fNormalmapSpeed2);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2NormalmapSpeed3Handle,m_water_cb_per_group.fNormalmapSpeed3);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec3SunDirectionHandle,m_water_cb_per_group.sun_pos);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4ReferPosHandle,m_water_cb_per_group.refer_position);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec3LightDirHandle,m_water_cb_per_group.light_dir);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4UnderWaterHandle,m_water_cb_per_group.fUnderWater);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4OceanColorHandle,m_water_cb_per_group.water_color);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4SkyColorHandle,m_water_cb_per_group.sky_color);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4SunColorStartHandle,m_water_cb_per_group.sun_color_start);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4SunColorEndHandle,m_water_cb_per_group.sun_color_end);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->fDistanceScaleHandle,m_water_cb_per_group.distance_scale);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fReflectionIntenHandle,m_water_cb_per_group.ref_inten);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fReflectionWeightHandle,m_water_cb_per_group.ref_weight);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fRefractionIntenHandle,m_water_cb_per_group.refraction_inten);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fRefractionWeightHandle,m_water_cb_per_group.refraction_weight);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fSunLevelHandle,m_water_cb_per_group.sun_level);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4DepthParamHandle,m_water_cb_per_group.depth_param);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fReflectionSunIntenHandle,m_water_cb_per_group.ref_sun_inten);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4FogColorHandle,m_water_cb_per_group.fog_color);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4FogParamHandle,m_water_cb_per_group.fog_param);

//	char mbzTemp[256];
//	sprintf(mbzTemp,"%f\n",m_water_cb_per_group.fog_param.x);
//	::OutputDebugStringA(mbzTemp);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4FogExpParamHandle,m_water_cb_per_group.fog_exp_param);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2PixelSizeHandle,m_water_cb_per_group.pixel_size);

}

void CTerrainChunk::SetWaterShader(IShaderProgram* pShader)
{
	m_pWaterShader = pShader;
}

void CTerrainChunk::SetWaterShaderHandle(CTerrainChunk::chunk_water_shader_handle_t* pShaderHandle)
{
	m_pWaterShaderHandle = pShaderHandle;
}

CTerrainChunk::chunk_water_shader_handle_t* CTerrainChunk::GetWaterShaderHandle() const
{
	return m_pWaterShaderHandle;
}

void CTerrainChunk::UseWaterObject()
{
	IShaderParamOp* pShaderOp = m_pWaterShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(m_pWaterShaderHandle->mat4Detail1ParamHandle,m_chunk_cb_obj.Detail1Param);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->mat4Detail2ParamHandle,m_chunk_cb_obj.Detail2Param);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->mat4Detail3ParamHandle,m_chunk_cb_obj.Detail3Param);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->mat4Detail4ParamHandle,m_chunk_cb_obj.Detail4Param);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec3ChunkPosHandle,m_chunk_cb_obj.vChunkPos);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4ZoneOffsetAndSizeHandle,m_chunk_cb_obj.ZoneOffsetAndSize);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fBaseSpecularLevelHandle,m_chunk_cb_obj.fBaseSpecularLevel);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fBaseSpecularPowerHandle,m_chunk_cb_obj.fBaseSpecularPower);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->fBaseHeightScaleHandle,m_chunk_cb_obj.fBaseHeightScale);

	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4BlendSpecularLevelHandle,m_chunk_cb_obj.BlendSpecularLevel);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4BlendSpecularPowerHandle,m_chunk_cb_obj.BlendSpecularPower);
	pShaderOp->SetParamValue(m_pWaterShaderHandle->vec4BlendHeightScaleHandle,m_chunk_cb_obj.BlendHeightScale);

}


// 设置渲染批次
bool CTerrainChunk::SetWaterBatch(void* pBatch,IShaderProgram* pShader)
{
	SetBatch(pBatch,m_waterlevel,pShader);
	return true;
}

void CTerrainChunk::SetWaterLevel(int level)
{
	m_waterlevel = level;
}

// 使用水面相关贴图绘制
void CTerrainChunk::UseWaterTexture()
{
	IShaderParamOp* pShaderOp = m_pWaterShaderHandle->pShader->GetParamOp();
	IRenderStateOp* pRenderStateOp = m_pWaterShaderHandle->pShader->GetRenderStateOp();

	if(!m_water_tex.use_refraction)
	{
		//glEnable(GL_BLEND);
		pRenderStateOp->EnableBlend(true);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
		pRenderStateOp->SetBlendEquation(IRenderStateOp::BLEND_FUNC_ADD);
		pRenderStateOp->SetBlendFunc(IRenderStateOp::BLEND_COLOR_ONE,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA);
	}

	int nTexNum = 0;
	// 反射图
	if(m_water_tex.use_reflection&&m_water_tex.ReflectionTex)
	{
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_SeaReflectHandle,m_water_tex.ReflectionTex->GetTexture());
	}

	if(m_water_tex.WaterNormalMap1)
	{
		ITextureSampler* pSamplerTex = m_water_tex.WaterNormalMap1->GetCanUseShaderTex()->GetTextureSampler();
		pSamplerTex->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_WaterNormalmap1Handle,m_water_tex.WaterNormalMap1->GetCanUseShaderTex()->GetTexture());
	}
	
	if(m_water_tex.WaterNormalMap2)
	{
		ITextureSampler* pSamplerTex = m_water_tex.WaterNormalMap2->GetCanUseShaderTex()->GetTextureSampler();
		pSamplerTex->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_WaterNormalmap2Handle,m_water_tex.WaterNormalMap2->GetCanUseShaderTex()->GetTexture());
	}

	if(m_water_tex.WaterNormalMap3)
	{
		ITextureSampler* pSamplerTex = m_water_tex.WaterNormalMap3->GetCanUseShaderTex()->GetTextureSampler();
		pSamplerTex->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_WaterNormalmap3Handle,m_water_tex.WaterNormalMap3->GetCanUseShaderTex()->GetTexture());
	}

	if(m_water_tex.WaterNormalMap4)
	{
		ITextureSampler* pSamplerTex = m_water_tex.WaterNormalMap4->GetCanUseShaderTex()->GetTextureSampler();
		pSamplerTex->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_WaterNormalmap4Handle,m_water_tex.WaterNormalMap4->GetCanUseShaderTex()->GetTexture());
	}

	//反射图
	if(m_water_tex.SceneRefract)
	{
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_SceneRefractHandle,m_water_tex.SceneRefract->GetTexture());

		//这里需要特殊处理一下折射图的UV问题
		//FmVec2 uvScale = FmVec2((float)m_pRender->GetDeviceWidth()/m_water_tex.SceneRefract->GetWidth(),(float)m_pRender->GetDeviceHeight()/m_water_tex.SceneRefract->GetHeight());
		FmVec2 uvScale = FmVec2(1.0,1.0);
		pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2RefractUVScaleHandle,uvScale);
	}

	if(m_water_tex.DepthRT)
	{
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_DepthmapHandle,m_water_tex.DepthRT->GetTexture());

		//这里需要特殊处理一下深度图的UV问题
		FmVec2 depthuvScale = FmVec2((float)m_pRender->GetDeviceWidth()/m_water_tex.DepthRT->GetWidth(),(float)m_pRender->GetDeviceHeight()/m_water_tex.DepthRT->GetHeight());
		pShaderOp->SetParamValue(m_pWaterShaderHandle->vec2DepthUVScaleHandle,depthuvScale);
	}

	if(m_water_tex.WhiteWaterMap)
	{
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_WhiteWaterHandle,m_water_tex.WhiteWaterMap->GetCanUseShaderTex()->GetTexture());
	}

	if(m_water_tex.ReflectDepthRT)
	{
		pShaderOp->SetTexture2D(m_pWaterShaderHandle->tex_ReflectDepthHandle,m_water_tex.ReflectDepthRT->GetTexture());
	}
}
