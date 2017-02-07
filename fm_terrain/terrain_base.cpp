//--------------------------------------------------------------------
// 文件名:		terrain_base.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain.h"
#include "terrain_painter.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "terrain_light.h"
#include "terrain_chunk.h"
#include "terrain_visuals.h"
#include "terrain_walkable.h"
#include "terrain_water.h"
#include "zone_manager.h"
#include "chunk_style.h"
#include "chunk_vb.h"
#include "quad_visual.h"
#include "quad_collide.h"
#include "sea_water.h"
#include "tex_alpha_map.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "../visual/vis_utils.h"
#include "../visual/i_context.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"
#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

// Terrain

/*
CChunkVB* Terrain::CreateChunkVB(int level)
{
	size_t scale = (m_nChunkScale >> level) + 1;
	CChunkVB* pChunkVB = CORE_NEW(CChunkVB);

	if (!pChunkVB->Create(m_pRender, scale * scale))
	{
		CORE_DELETE(pChunkVB);
		return NULL;
	}

	pChunkVB->SetLevel(level);
	
	return pChunkVB;
}

void Terrain::ReleaseChunkVB(CChunkVB* pChunkVB)
{
	Assert(pChunkVB != NULL);

	CORE_DELETE(pChunkVB);
}

size_t Terrain::GetFreeChunkVBCount()
{
	size_t count = 0;
	
	for (size_t level = 0; level < m_nLodLevels; ++level)
	{
		CChunkVB* p = m_pChunkVBList[level];
		
		while (p)
		{
			count++;
			p = p->GetNext();
		}
	}

	return count;
}

void Terrain::DeleteChunkVBList()
{
	for (size_t level = 0; level < m_nLodLevels; ++level)
	{
		CChunkVB* p = m_pChunkVBList[level];

		while (p)
		{
			CChunkVB* t = p;

			p = p->GetNext();
			CORE_DELETE(t);
		}

		m_pChunkVBList[level] = NULL;
		p = m_pPrevChunkVBList[level];

		while (p)
		{
			CChunkVB* t = p;

			p = p->GetNext();
			CORE_DELETE(t);
		}

		m_pPrevChunkVBList[level] = NULL;
		m_nChunkVBCount[level] = 0;
	}
}

void Terrain::AddPrevChunkVBList()
{
	for (size_t level = 0; level < m_nLodLevels; ++level)
	{
		CChunkVB* p = m_pPrevChunkVBList[level];

		while (p)
		{
			CChunkVB* t = p;

			p = p->GetNext();
			t->SetNext(m_pChunkVBList[level]);
			m_pChunkVBList[level] = t;
		}

		m_pPrevChunkVBList[level] = NULL;
	}
}
*/

void Terrain::SetCollideScale(int value)
{
	size_t k = m_nChunkScale;

	while (k > 0)
	{
		if (value == k)
		{
			m_nCollideScale = value; 
			break;
		}

		k = k / 2;
	}
}

float Terrain::GetFullWidth() const
{ 
	return m_pZoneManager->GetCols() * GetZoneWidth(); 
}

float Terrain::GetFullHeight() const
{ 
	return m_pZoneManager->GetRows() * GetZoneHeight(); 
}

float Terrain::GetGlobalHeightDefault(size_t row, size_t col, float def)
{
	size_t zone_row = row / m_nZoneScale;
	size_t zone_col = col / m_nZoneScale;

	CTerrainZone* zone = m_pZoneManager->GetZone(zone_row, zone_col);

	if (NULL == zone)
	{
		return def;
	}

	if (!zone->GetHeightLoaded())
	{
		return def;
	}
	
	size_t r = row - zone->GetGlobalRow();
	size_t c = col - zone->GetGlobalCol();
	
	return zone->GetHeight()->GetValue((int)r, (int)c);
}

bool Terrain::ReleaseGlobalNormal(size_t row, size_t col)
{
	size_t zone_row = row / m_nZoneScale;
	size_t zone_col = col / m_nZoneScale;
	
	CTerrainZone* zone = m_pZoneManager->GetZone(zone_row, zone_col);
	
	if (NULL == zone)
	{
		return false;
	}
	
	if (!zone->GetHeightLoaded())
	{
		return false;
	}
	
	size_t r = row - zone->GetGlobalRow();
	size_t c = col - zone->GetGlobalCol();
	
	return zone->GetHeight()->ReleaseNormal((int)r, (int)c);
}

/*
IDxInputLayout* Terrain::GetGrassInputLayout()
{
	if (NULL == m_pGrassInputLayout)
	{
		D3D11_INPUT_ELEMENT_DESC ied[3] = { 
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
				D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, 
				D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, 
				D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		};

		m_pGrassInputLayout = m_pRender->CreateInputLayout(ied, 3, 
			"grass.hlsl", "vs_main_grass", "");
	}

	return m_pGrassInputLayout;
}
*/

#define GRASS_PIECE_NUM (0x10000 / 4)

IStaticIB* Terrain::GetGrassIndexBuffer(size_t grass_num)
{
	if (grass_num > GRASS_PIECE_NUM)
	{
		// 超过最大可能的索引数值
		return NULL;
	}

	if (NULL == m_pGrassIndexBuffer)
	{
		TAutoMem<unsigned short, 1, TCoreAlloc> auto_buf(GRASS_PIECE_NUM * 6);
		unsigned short* pIndex = auto_buf.GetBuffer();
		size_t count = 0;

		for (size_t i = 0; i < GRASS_PIECE_NUM; ++i)
		{
			*pIndex++ = (unsigned short)count + 0;
			*pIndex++ = (unsigned short)count + 1;
			*pIndex++ = (unsigned short)count + 2;
			*pIndex++ = (unsigned short)count + 1;
			*pIndex++ = (unsigned short)count + 3;
			*pIndex++ = (unsigned short)count + 2;
			count += 4;
		}

		m_pGrassIndexBuffer = m_pRender->CreateStaticIB(auto_buf.GetBuffer(), 
			GRASS_PIECE_NUM * 6 * sizeof(unsigned short));
		
		if (NULL == m_pGrassIndexBuffer)
		{
			CORE_TRACE("(Terrain::GetGrassIndexBuffer)create failed");
			return NULL;
		}
	}

	return m_pGrassIndexBuffer;
}

CTerrainZone* Terrain::GetLeftZone(CTerrainZone* zone)
{
	return m_pZoneManager->GetLeftZone(zone);
}

CTerrainZone* Terrain::GetRightZone(CTerrainZone* zone)
{
	return m_pZoneManager->GetRightZone(zone);
}

CTerrainZone* Terrain::GetUpZone(CTerrainZone* zone)
{
	return m_pZoneManager->GetUpZone(zone);
}

CTerrainZone* Terrain::GetDownZone(CTerrainZone* zone)
{
	return m_pZoneManager->GetDownZone(zone);
}

bool Terrain::CreateStyleIndexBuffer()
{
	for (size_t i = 0; i < m_nStyleNum; i++)
	{
		if (NULL == m_pStyles[i])
		{
			continue;
		}
		
		m_pStyles[i]->CreateIndexBuffer(m_pRender);
	}
	
	return true;
}

void Terrain::ReleaseStyleIndexBuffer()
{
	for (size_t i = 0; i < m_nStyleNum; i++)
	{
		if (NULL == m_pStyles[i])
		{
			continue;
		}
		
		m_pStyles[i]->ReleaseIndexBuffer();
	}
}

// 根据分块尺寸决定LOD级别数
static int get_lod_levels(int chunk_scale)
{
	switch (chunk_scale)
	{
	case 8:
		return 3;
	case 16:
		return 4;
	case 32:
		return 5;
	case 64:
		return 6;
	case 128:
		return 7;
	default:
		break;
	}
	
	return 0;
}

bool Terrain::SetParameter(size_t zone_scale, size_t chunk_scale,
	size_t tex_scale, size_t tex_units, size_t alpha_per_unit, 
	size_t collide_per_unit)
{
	if ((tex_units != 1) && (tex_units != 2) && (tex_units != 4) 
		&& (tex_units != 8) && (tex_units != 16))
	{
		return false;
	}
	
	if ((alpha_per_unit != 1) && (alpha_per_unit != 2)
		&& (alpha_per_unit != 4) && (alpha_per_unit != 8))
	{
		return false;
	}
	
	if ((collide_per_unit != 1) && (collide_per_unit != 2)
		&& (collide_per_unit != 4) && (collide_per_unit != 8))
	{
		return false;
	}

	m_nZoneScale = zone_scale;
	m_nChunkScale = chunk_scale;
	m_nTextureScale = tex_scale;
	m_nTextureUnits = tex_units;
	m_nAlphaPerUnit = alpha_per_unit;
	m_nAlphaMapScale = m_nChunkScale * m_nAlphaPerUnit;
	m_nAlphaScale = m_nZoneScale * m_nAlphaPerUnit;
	m_nLodLevels = get_lod_levels((int)m_nChunkScale);
	m_nCollidePerUnit = collide_per_unit;
	m_fCollideSize = m_fUnitSize / collide_per_unit;
	m_fInvCollideSize = 1.0F / m_fCollideSize;

	return true;
}

bool Terrain::InitZones(size_t rows, size_t cols, size_t origin_row, 
	size_t origin_col, size_t range)
{
	if (!m_pZoneManager->SetZones(rows, cols))
	{
		CORE_TRACE("(Terrain::InitZones)SetZones failed");
		return false;
	}

	if (!m_pZoneManager->SetZoneOrigin(origin_row, origin_col))
	{
		CORE_TRACE("(Terrain::InitZones)SetZoneOrigin failed");
		return false;
	}

	if (!m_pZoneManager->SetZoneRange(range))
	{
		CORE_TRACE("(Terrain::InitZones)SetZoneRange failed");
		return false;
	}
	
	if (!m_pZoneManager->Initialize())
	{
		CORE_TRACE("(Terrain::InitZones)init manager failed");
		return false;
	}

	// 初始化碰撞树
	m_pCollide->Init();
	
	return true;
}

bool Terrain::ReleaseAllZoneLoader()
{
	m_pZoneManager->ReleaseAllZoneLoader();

	return true;
}

CTerrainChunk* Terrain::GetInChunk(float x, float z)
{
	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == zone)
	{
		return NULL;
	}
	
	return zone->GetInChunk(x, z);
}

float Terrain::GetPosiY(float x, float z)
{
	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);

	if (NULL == zone)
	{
		return 0.0F;
	}
	
	if (!zone->GetHeightLoaded())
	{
		return 0.0F;
	}
	
	return zone->GetHeight()->GetY(x, z);
}

//float Terrain::GetDiffuse(float x, float z)
//{
//	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);
//	
//	if (NULL == zone)
//	{
//		return 1.0F;
//	}
//	
//	float u = (x - zone->GetLeft()) / (m_nZoneScale * m_fUnitSize);
//	float v = (z - zone->GetTop()) / (m_nZoneScale * m_fUnitSize);
//	
//	return zone->GetLight()->GetDiffuse(u, v);
//}

const char* Terrain::GetMainTexture(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return "";
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return "";
	}

	float offset_x = pZone->GetLeft() 
		+ pChunk->GetCol() * m_nChunkScale * m_fUnitSize;
	float offset_z = pZone->GetTop() 
		+ pChunk->GetRow() * m_nChunkScale * m_fUnitSize;
	float factor = m_fUnitSize / m_nAlphaPerUnit;
	int row = VisUtil_FloatToInt((z - offset_z) / factor);
	int col = VisUtil_FloatToInt((x - offset_x) / factor);

	if ((size_t(row) >= m_nAlphaMapScale) 
		|| (size_t(col) >= m_nAlphaMapScale))
	{
		return "";
	}

	// 返回混合值最大的贴图名称
	float value = 0.0F;
	int tex_index = -1;
	size_t map_count = pChunk->GetAlphaMapCount();

	for (size_t i = 0; i < map_count; ++i)
	{
		CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMapByIndex(i);
		float v = pAlphaMap->GetAlpha(row, col) / 15.0F;

		if (v > value)
		{
			value = v;
			tex_index = (int)pChunk->GetAlphaMapTexIndex(i);
		}
	}

	if (size_t(tex_index) < m_pBlendTexSet->GetCount())
	{
		return m_pBlendTexSet->GetByIndex(tex_index)->GetPath();
	}

	// 否则返回基础贴图名
	tex_index = pChunk->GetBaseTexIndex();

	if (size_t(tex_index) < m_pBaseTexSet->GetCount())
	{
		return m_pBaseTexSet->GetByIndex(tex_index)->GetPath();
	}
	
	return "";
}

float Terrain::TraceHeight(float x, float z, float high_y, float low_y)
{
	trace_info_t ti;

	bool succeed = TraceDetail(FmVec3(x, high_y, z), 
		FmVec3(x, low_y, z), ti);

	if (!succeed)
	{
		return 2.0F;
	}

	return ti.fDistance;
}

void Terrain::TraceCollide(const IVarList& args, IVarList& result)
{
	float beg_x = args.FloatVal(0);
	float beg_y = args.FloatVal(1);
	float beg_z = args.FloatVal(2);
	float end_x = args.FloatVal(3);
	float end_y = args.FloatVal(4);
	float end_z = args.FloatVal(5);
	
	trace_info_t ti;

	bool succeed = TraceDetail(FmVec3(beg_x, beg_y, beg_z), 
		FmVec3(end_x, end_y, end_z), ti);
	
	if (!succeed)
	{
		return;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v = pHit->Vertex[0] 
		+ (pHit->Vertex[1] - pHit->Vertex[0]) * pHit->fTraceU 
		+ (pHit->Vertex[2] - pHit->Vertex[0]) * pHit->fTraceV;

	if (pHit->TraceID != this->GetID())
	{
		// 点击到地面对象
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);
			FmVec3TransformCoord(&v, &v, &mat);
		}
	}

	result.AddObject(pHit->TraceID);
	result.AddFloat(v.x);
	result.AddFloat(v.y);
	result.AddFloat(v.z);
}

void Terrain::TraceTexture(const IVarList& args, IVarList& result)
{
	float beg_x = args.FloatVal(0);
	float beg_y = args.FloatVal(1);
	float beg_z = args.FloatVal(2);
	float end_x = args.FloatVal(3);
	float end_y = args.FloatVal(4);
	float end_z = args.FloatVal(5);

	m_pRender->SetTraceTextureName(true);
	
	trace_info_t ti;

	bool succeed = TraceDetail(FmVec3(beg_x, beg_y, beg_z), 
		FmVec3(end_x, end_y, end_z), ti);

	m_pRender->SetTraceTextureName(false);

	if (!succeed)
	{
		return;
	}

	const char* tex_name = "";

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v = pHit->Vertex[0] 
		+ (pHit->Vertex[1] - pHit->Vertex[0]) * pHit->fTraceU 
		+ (pHit->Vertex[2] - pHit->Vertex[0]) * pHit->fTraceV;

	if (pHit->TraceID != this->GetID())
	{
		// 点击到地面对象
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);
			FmVec3TransformCoord(&v, &v, &mat);

			if (pVisBase->GetEntInfo()->IsKindOf("Model") 
				&& pHit->strTexName != NULL)
			{
				tex_name = pHit->strTexName;
			}
		}
	}
	else
	{
		tex_name = this->GetMainTexture(v.x, v.z);
	}

	result.AddString(tex_name);
	result.AddObject(pHit->TraceID);
	result.AddFloat(v.x);
	result.AddFloat(v.y);
	result.AddFloat(v.z);
}

bool Terrain::TraceHit(float x1, float y1, float z1, float x2, float y2, 
	float z2)
{
	// 只对三角形的正面做碰撞检测
	m_pRender->SetTraceOnlyFront(true);
	
	bool succeed = TraceHitted(FmVec3(x1, y1, z1), 
		FmVec3(x2, y2, z2));

	m_pRender->SetTraceOnlyFront(false);

	return succeed;
}

float Terrain::GetGroundHeight(float x, float z)
{
	// 优化的取高度算法
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	if (!pZone->GetHeightLoaded())
	{
		return 0.0F;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return 0.0F;
	}

	CTerrainHeight* pHeight = pZone->GetHeight();
	float y = pHeight->GetY(x, z);
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	float left = pZone->GetLeft();
	float top = pZone->GetTop();
	int local_row = pChunk->GetLocalRow();
	int local_col = pChunk->GetLocalCol();
	int col = VisUtil_FloatToInt((x - left) * m_fInvUnitSize);
	int row = VisUtil_FloatToInt((z - top) * m_fInvUnitSize);
	int relate_row = row - local_row;
	int relate_col = col - local_col;

	if ((size_t(relate_row) >= m_nChunkScale)
		|| (size_t(relate_col) >= m_nChunkScale))
	{
		return y;
	}

	int collide_dims = (int)(m_nChunkScale / m_nCollideScale);
	int collide_row = relate_row / (int)m_nCollideScale;
	int collide_col = relate_col / (int)m_nCollideScale;
	int collide_index = collide_row * collide_dims + collide_col;
	CChunkStyle* style = GetChunkStyle(pChunk->GetStyle());
	CChunkStyle::collide_info_t* ci = style->GetCollideInfo(collide_index);
	size_t triangle_num = ci->triangles.size();
	CChunkStyle::triangle_info_t* triangle_info = &ci->triangles[0];
	float unit_size = m_fUnitSize;

	FmVec3 src(x, y + 50.0F, z);
	FmVec3 dir(0.0F, -100.0F, 0.0F);

	// 逐个三角形做碰撞检测
	FmVec3 v0;
	FmVec3 v1;
	FmVec3 v2;

	for (size_t k = 0; k < triangle_num; ++k)
	{
		CChunkStyle::triangle_info_t& ti = triangle_info[k];

		if ((relate_col < ti.nLeft) || (relate_col > ti.nRight)
			|| (relate_row < ti.nTop) || (relate_row > ti.nBottom))
		{
			continue;
		}

		//pChunk->GetCollideTriangle(&ti.nVertexRow1, &v1, &v2, &v3);
		int row1 = ti.nVertexRow1 + local_row;
		int col1 = ti.nVertexCol1 + local_col;
		int row2 = ti.nVertexRow2 + local_row;
		int col2 = ti.nVertexCol2 + local_col;
		int row3 = ti.nVertexRow3 + local_row;
		int col3 = ti.nVertexCol3 + local_col;

		v0.x = left + col1 * unit_size;
		v1.x = left + col2 * unit_size;
		v2.x = left + col3 * unit_size;
		v0.y = height_values[row1 * height_cols + col1];
		v1.y = height_values[row2 * height_cols + col2];
		v2.y = height_values[row3 * height_cols + col3];
		v0.z = top + row1 * unit_size;
		v1.z = top + row2 * unit_size;
		v2.z = top + row3 * unit_size;

		float t;
		float u;
		float v;

		// 射线与三角形相交
		if (VisUtil_IntersectTri(&v0, &v1, &v2, &src, &dir, &u, &v, &t))
		{
			if ((t >= 0.0F) && (t <= 1.0F))
			{
				return v0.y + (v1.y - v0.y) * u + (v2.y - v0.y) * v + 0.0001F;
			}
		}
	}
	
	return y;
}

static bool get_water_model_exists(Terrain* pTerrain, float x, float z)
{
	float pos_y = pTerrain->GetPosiY(x, z);
	float high_y = pos_y + 50.0F;
	float low_y = pos_y - 50.0F;

	trace_info_t ti;

	bool only_trace_water_model = false;// pTerrain->GetRender()->GetTraceWaterModel();
	
//	pTerrain->GetRender()->SetTraceWaterModel(true);

	bool succeed = pTerrain->TraceDetail(FmVec3(x, high_y, z), 
		FmVec3(x, low_y, z), ti);

//	pTerrain->GetRender()->SetTraceWaterModel(only_trace_water_model);

	if (!succeed)
	{
		return false;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	if (pHit->TraceID != pTerrain->GetID())
	{
		return true;
	}

	return false;
}

bool Terrain::GetWaterExists(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return false;
	}

	int water_index = pChunk->GetWaterIndex();

	if (size_t(water_index) >= m_pWater->GetCount())
	{
		if (get_water_model_exists(this, x, z))
		{
			return true;
		}

		return false;
	}

	return true;
}

int Terrain::GetWaterCurrent(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return false;
	}

	return pChunk->GetWaterIndex();
}

static float get_water_model_height(Terrain* pTerrain, float x, float z)
{
	float pos_y = pTerrain->GetPosiY(x, z);

	float high_y = pos_y + 50.0F;
	float low_y = pos_y - 50.0F;

	trace_info_t ti;

//	bool only_trace_water_model = pTerrain->GetRender()->GetTraceWaterModel();
//	pTerrain->GetRender()->SetTraceWaterModel(true);

	bool succeed = pTerrain->TraceDetail(FmVec3(x, high_y, z), 
		FmVec3(x, low_y, z), ti);

//	pTerrain->GetRender()->SetTraceWaterModel(only_trace_water_model);

	if (!succeed)
	{
		return 0.0F;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v = pHit->Vertex[0] + (pHit->Vertex[1] - pHit->Vertex[0]) 
		* pHit->fTraceU + (pHit->Vertex[2] - pHit->Vertex[0]) * pHit->fTraceV;

	if (pHit->TraceID != pTerrain->GetID())
	{
		// 点击到地面对象
		IVisBase* pVisBase = (IVisBase*)
			pTerrain->GetCore()->GetEntity(pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);

			FmVec3TransformCoord(&v, &v, &mat);

 			return v.y;
		}
	}

	return 0.0F;
}

float Terrain::GetWaterBaseHeight(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return 0.0F;
	}

	int water_index = pChunk->GetWaterIndex();

	if (size_t(water_index) >= m_pWater->GetCount())
	{
		if (get_water_model_exists(this, x, z))
		{
			return get_water_model_height(this, x, z);
		}

		// 没有水面
		return 0.0F;
	}

	return m_pWater->GetByIndex(water_index)->GetBaseHeight();
}

float Terrain::GetWaterMaxHeightDefault(float x, float z, float default_y)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return default_y;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return default_y;
	}

	int water_index = pChunk->GetWaterIndex();

	if (size_t(water_index) >= m_pWater->GetCount())
	{
		return default_y;
	}

	SeaWater* pWater = m_pWater->GetByIndex(water_index);

	return pWater->GetBaseHeight() + pWater->GetMaxSeaHeight() * 0.5F;
}

float Terrain::GetWaterWaveHeight(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return -FLT_MAX;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return -FLT_MAX;
	}

	int water_index = pChunk->GetWaterIndex();

	if (size_t(water_index) >= m_pWater->GetCount())
	{
//		if (get_water_model_exists(this, x, z))
//		{
//			return get_water_model_height(this, x, z);
//		}
//		
//		// 没有水面
//		return -FLT_MAX;
		CTerrainWalkable* pWalkable = pZone->GetWalkable();

		if (!pWalkable->GetReady())
		{
			return -FLT_MAX;
		}

		int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
		int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
		float height = pWalkable->GetWaterHeight(row, col);	

		if (height_is_null(height))
		{
			return -FLT_MAX;
		}

		return height;
	}

	return m_pWater->GetByIndex(water_index)->GetCurrentHeight(x, z);
}

bool Terrain::GetCollideEnable(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	return pZone->GetWalkable()->GetReady();
}

float Terrain::GetApexHeight(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return GetGroundHeight(x, z);
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	// 层数量
	int floor_num = pWalkable->GetFloorCount();

	for (int i = floor_num - 1; i >= 0; --i)
	{
		unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_EXISTS) != 0)
		{
			float y = pWalkable->GetFloorHeight(row, col, i);

			if (!height_is_null(y))
			{
				return y;
			}
			else
			{
				return pZone->GetHeight()->GetY(x, z);
			}
		}
	}

	// 使用地面高度
	return pZone->GetHeight()->GetY(x, z);
}

int Terrain::GetApexFloor(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return 0;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	// 层数量
	int floor_num = pWalkable->GetFloorCount();

	for (int i = floor_num - 1; i >= 0; --i)
	{
		unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_EXISTS) != 0)
		{
			return i;
		}
	}

	return 0;
}

float Terrain::GetBottomHeight(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return GetGroundHeight(x, z);
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	// 层数量
	int floor_num = pWalkable->GetFloorCount();

	for (int i = 0; i < floor_num; ++i)
	{
		unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_MOVE) != 0)
		{
			float y = pWalkable->GetFloorHeight(row, col, i);

			if (!height_is_null(y))
			{
				return y;
			}
			else
			{
				return pZone->GetHeight()->GetY(x, z);
			}
		}
	}

	// 使用地面高度
	return pZone->GetHeight()->GetY(x, z);
}

int Terrain::GetBottomFloor(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return 0;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);

	// 层数量
	int floor_num = pWalkable->GetFloorCount();

	for (int i = 0; i < floor_num; ++i)
	{
		unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, i);

		if ((floor_marker & FLOOR_MARKER_MOVE) != 0)
		{
			return i;
		}
	}

	return 0;
}

bool Terrain::GetWalkEnable(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode)
		{
			// 未生成碰撞的区域缺省可以行走
			return true;
		}

		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	unsigned int walk_marker = pWalkable->GetWalkMarker(row, col);

	return walk_marker > 0;
}

float Terrain::GetWalkHeight(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return GetGroundHeight(x, z);
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	float y = HEIGHT_NULL;
	unsigned int walk_marker = pWalkable->GetWalkMarker(row, col);

	if (walk_marker > 0)
	{
		y = pWalkable->GetFloorHeight(row, col, walk_marker - 1);
	}

	if (height_is_null(y))
	{
		// 使用地面高度
		y = pZone->GetHeight()->GetY(x, z);
	}

	return y;
}

int Terrain::GetWalkFloor(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return -1;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode)
		{
			// 未生成碰撞的区域缺省可以行走
			return 0;
		}

		return -1;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	unsigned int walk_marker = pWalkable->GetWalkMarker(row, col);

	if (walk_marker == 0)
	{
		return -1;
	}

	return walk_marker - 1;
}

bool Terrain::GetWalkWaterExists(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	float height = pWalkable->GetWaterHeight(row, col);

	return !height_is_null(height);
}

float Terrain::GetWalkWaterHeight(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return 0.0F;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);

	return pWalkable->GetWaterHeight(row, col);	
}

int Terrain::GetFloorCount(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode)
		{
			return 1;
		}
		
		return 0;
	}

	return pWalkable->GetFloorCount();
}

bool Terrain::GetFloorExists(float x, float z, int floor)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode)
		{
			return true;
		}

		return false;
	}

	if ((floor < 0) || (floor >= pWalkable->GetFloorCount()))
	{
		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, floor);

	return (floor_marker & FLOOR_MARKER_EXISTS) != 0;
}

bool Terrain::GetFloorCanMove(float x, float z, int floor)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode)
		{
			return true;
		}

		return false;
	}

	if ((floor < 0) || (floor >= pWalkable->GetFloorCount()))
	{
		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, floor);

	return (floor_marker & FLOOR_MARKER_MOVE) != 0;
}

bool Terrain::GetFloorCanStand(float x, float z, int floor)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode)
		{
			return true;
		}

		return false;
	}

	if ((floor < 0) || (floor >= pWalkable->GetFloorCount()))
	{
		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, floor);

	return (floor_marker & FLOOR_MARKER_STAND) != 0;
}

float Terrain::GetFloorHeight(float x, float z, int floor)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		if (m_bDesignMode && pZone->GetHeightLoaded())
		{
			return pZone->GetHeight()->GetY(x, z);
		}

		return 0.0F;
	}

	if ((floor < 0) || (floor >= pWalkable->GetFloorCount()))
	{
		return 0.0F;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);

	float y = pWalkable->GetFloorHeight(row, col, floor);

	if (height_is_null(y))
	{
		// 使用地面高度
		//y = pZone->GetHeight()->GetY(x, z);
		y = GetGroundHeight(x, z);
	}

	return y;
}

float Terrain::GetFloorSpace(float x, float z, int floor)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return HEIGHT_NULL;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return HEIGHT_NULL;
	}

	if ((floor < 0) || (floor >= pWalkable->GetFloorCount()))
	{
		return HEIGHT_NULL;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);

	return pWalkable->GetFloorSpace(row, col, floor);
}

bool Terrain::GetFloorHasWall(float x, float z, int floor)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return false;
	}

	if ((floor <= 0) || (floor >= pWalkable->GetFloorCount()))
	{
		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);
	unsigned int floor_marker = pWalkable->GetFloorMarker(row, col, floor);

	return (floor_marker & FLOOR_MARKER_WALL) != 0;
}

bool Terrain::GetWallExists(float x, float y, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainWalkable* pWalkable = pZone->GetWalkable();

	if (!pWalkable->GetReady())
	{
		return false;
	}

	float ground_y = pZone->GetHeight()->GetY(x, z);

	if (y < ground_y)
	{
		// 低于地面高度
		return false;
	}

	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvCollideSize);

	return pWalkable->GetWallExists(row, col, y);
}

// 计算欧拉角
static float calc_angle(float x, float z)
{
	FmVec2 vec2Temp(x, z);
	float dist = FmVec2Length(&vec2Temp);

	if (dist < 0.000001F)
	{
		return 0.0F;
	}

	float angle = acosf(z / dist);

	if (Port_IsNaN(angle))
	{
		return 0.0F;
	}

	if (x < 0.0F)
	{
		return -angle;
	}

	return angle;
}

void Terrain::GetWalkWallAngle(const IVarList& args, IVarList& result)
{
	float beg_x = args.FloatVal(0);
	float beg_y = args.FloatVal(1);
	float beg_z = args.FloatVal(2);
	float end_x = args.FloatVal(3);
	float end_y = args.FloatVal(4);
	float end_z = args.FloatVal(5);

	FmVec3 src(beg_x, beg_y, beg_z);
	FmVec3 dst(end_x, end_y, end_z);

	trace_info_t ti;

	if (!TraceDetail(src, dst, ti))
	{
		return;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v1 = pHit->Vertex[0];
	FmVec3 v2 = pHit->Vertex[1];
	FmVec3 v3 = pHit->Vertex[2];

	//FmVec4 pos = v1 + (v2 - v1) * pHit->fTraceU 
	//	+ (v3 - v1) * pHit->fTraceV;

	if (pHit->TraceID != this->GetID())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);

			FmVec3TransformCoord(&v1, &v1, &mat);
			FmVec3TransformCoord(&v2, &v2, &mat);
			FmVec3TransformCoord(&v3, &v3, &mat);
			//FmVec3TransformCoord(&pos, &pos, &mat);
		}
	}

	FmVec3 pos = v1 + (v2 - v1) * pHit->fTraceU 
		+ (v3 - v1) * pHit->fTraceV;

	// 获得碰撞面的法线
	FmPlane plane;
	
	FmPlaneFromPoints(&plane, &v1, &v2, &v3);

	FmVec3 normal(plane.a, plane.b, plane.c);

	FmVec3Normalize(&normal, &normal);

	//FmVec4 up(0.0F, 1.0F, 0.0F);
	//float angle = acosf(D3DXVec3Dot(&normal, &up));
	//FmVec4 axis;
	//FmVec3Cross(&axis, &normal, &up);
	//D3DXQUATERNION quater_rotate;
	//D3DXQuaternionRotationAxis(&quater_rotate, &axis, -angle);

	FmQuat quater_rotate;

	VisUtil_QuaternionFromNormal(&quater_rotate, &normal);

	float angle_x;
	float angle_y;
	float angle_z;

	VisUtil_QuaternionToPitchYawRoll(&quater_rotate, &angle_x, &angle_y, 
		&angle_z);

	result.AddFloat(pos.x);
	result.AddFloat(pos.y);
	result.AddFloat(pos.z);
	result.AddFloat(angle_x);
	result.AddFloat(angle_y);
	result.AddFloat(angle_z);
}

void Terrain::GetWalkWallCollide(const IVarList& args, IVarList& result)
{
	float beg_x = args.FloatVal(0);
	float beg_y = args.FloatVal(1);
	float beg_z = args.FloatVal(2);
	float end_x = args.FloatVal(3);
	float end_y = args.FloatVal(4);
	float end_z = args.FloatVal(5);

	FmVec3 src(beg_x, beg_y, beg_z);
	FmVec3 dst(end_x, end_y, end_z);

	trace_info_t ti;

	if (!TraceDetail(src, dst, ti))
	{
		return;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v1 = pHit->Vertex[0];
	FmVec3 v2 = pHit->Vertex[1];
	FmVec3 v3 = pHit->Vertex[2];

	FmVec3 pos = v1 + (v2 - v1) * pHit->fTraceU 
		+ (v3 - v1) * pHit->fTraceV;

	if (pHit->TraceID != this->GetID())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);
			FmVec3TransformCoord(&v1, &v1, &mat);
			FmVec3TransformCoord(&v2, &v2, &mat);
			FmVec3TransformCoord(&v3, &v3, &mat);
			FmVec3TransformCoord(&pos, &pos, &mat);
		}
	}

	// 获得碰撞面的法线
	FmVec3 normal;
	
	FmVec3 vec3Temp1 = v2 - v1;
	FmVec3 vec3Temp2 = v3 - v1;
	FmVec3Cross(&normal, &vec3Temp1, &vec3Temp2);
	FmVec3Normalize(&normal, &normal);
				  
	float angle_y = calc_angle(normal.x, normal.z);

	result.AddFloat(pos.x);
	result.AddFloat(pos.y);
	result.AddFloat(pos.z);
	result.AddFloat(angle_y);
}

#define FLOAT_EQUAL(v1, v2) \
	((v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON)))

// 判断射线如何离开矩形
static int line_hit_back(float x1, float z1, float x2, float z2, float src_x, 
	float src_z, float dst_x, float dst_z)
{
	int result = 0;

	float dir_x = dst_x - src_x;
	float dir_z = dst_z - src_z;

	if (dir_x < 0)
	{
		float dist1 = x1 - src_x;
		float dist2 = x1 - dst_x;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// left
				result |= 0x1;
			}
		}
		//else if ((dist1 < 0) != (dist2 < 0))
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float z = src_z + dir_z * (dist1 / (dist1 - dist2));

			if ((z >= z1) && (z <= z2))
			{
				//if (dist1 < 0)
				//{
					// left
					result |= 0x1;
				//}
			}
		}
	}
	else
	{
		float dist1 = src_x - x2;
		float dist2 = dst_x - x2;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// right
				result |= 0x4;
			}
		}
		//else if ((dist1 < 0) != (dist2 < 0))
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float z = src_z + dir_z * (dist1 / (dist1 - dist2));

			if ((z >= z1) && (z <= z2))
			{
				//if (dist1 < 0)
				//{
					// right
					result |= 0x4;
				//}
			}
		}
	}

	if (dir_z < 0)
	{
		float dist1 = z1 - src_z;
		float dist2 = z1 - dst_z;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// top
				result |= 0x2;
			}
		}
		//else if ((dist1 < 0) != (dist2 < 0))
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float x = src_x + dir_x * (dist1 / (dist1 - dist2));

			if ((x >= x1) && (x <= x2))
			{
				//if (dist1 < 0)
				//{
					// top
					result |= 0x2;
				//}
			}
		}
	}
	else
	{
		float dist1 = src_z - z2;
		float dist2 = dst_z - z2;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// bottom
				result |= 0x8;
			}
		}
		//else if ((dist1 < 0) != (dist2 < 0))
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float x = src_x + dir_x * (dist1 / (dist1 - dist2));

			if ((x >= x1) && (x <= x2))
			{
				//if (dist1 < 0)
				//{
					// bottom
					result |= 0x8;
				//}
			}
		}
	}

	return result;
}

void Terrain::GetLineTraverse(const IVarList& args, IVarList& result)
{
	float src_x = args.FloatVal(0);
	float src_z = args.FloatVal(1);
	float dst_x = args.FloatVal(2);
	float dst_z = args.FloatVal(3);

	int col = VisUtil_FloatToInt(src_x * m_fInvCollideSize);
	int row = VisUtil_FloatToInt(src_z * m_fInvCollideSize);

	if (src_x < 0.0F)
	{
		--col;
	}

	if (src_z < 0.0F)
	{
		--row;
	}

	float unit_size = m_fCollideSize;
	// 起始碰撞格的中心坐标
	float x = (float(col) + 0.5F) * unit_size;
	float z = (float(row) + 0.5F) * unit_size;

	result.AddFloat(x);
	result.AddFloat(z);

	while (true)
	{
		float x1 = x - unit_size * 0.5F;
		float z1 = z - unit_size * 0.5F;
		float x2 = x1 + unit_size;
		float z2 = z1 + unit_size;

		int res = line_hit_back(x1, z1, x2, z2, src_x, src_z, dst_x, dst_z);

		if (0 == res)
		{
			break;
		}

		switch (res)
		{
		case 1: // left
			{
				x -= unit_size;

				result.AddFloat(x);
				result.AddFloat(z);
			}
			break;
		case 2: // top
			{
				z -= unit_size;

				result.AddFloat(x);
				result.AddFloat(z);
			}
			break;
		case 4: // right
			{
				x += unit_size;

				result.AddFloat(x);
				result.AddFloat(z);
			}
			break;
		case 8: // bottom
			{
				z += unit_size;

				result.AddFloat(x);
				result.AddFloat(z);
			}
			break;
		case 3: // left & top
			{
				float nx = x - unit_size;
				float nz = z - unit_size;

				result.AddFloat(nx);
				result.AddFloat(z);
				result.AddFloat(x);
				result.AddFloat(nz);
				result.AddFloat(nx);
				result.AddFloat(nz);

				x = nx;
				z = nz;
			}
			break;
		case 6: // right & top
			{
				float nx = x + unit_size;
				float nz = z - unit_size;

				result.AddFloat(nx);
				result.AddFloat(z);
				result.AddFloat(x);
				result.AddFloat(nz);
				result.AddFloat(nx);
				result.AddFloat(nz);

				x = nx;
				z = nz;
			}
			break;
		case 9: // left & bottom
			{
				float nx = x - unit_size;
				float nz = z + unit_size;

				result.AddFloat(nx);
				result.AddFloat(z);
				result.AddFloat(x);
				result.AddFloat(nz);
				result.AddFloat(nx);
				result.AddFloat(nz);

				x = nx;
				z = nz;
			}
			break;
		case 12: // right & bottom
			{
				float nx = x + unit_size;
				float nz = z + unit_size;

				result.AddFloat(nx);
				result.AddFloat(z);
				result.AddFloat(x);
				result.AddFloat(nz);
				result.AddFloat(nx);
				result.AddFloat(nz);

				x = nx;
				z = nz;
			}
			break;
		default:
			Assert(0);
			break;
		}
	}
}

void Terrain::GetLineIntersect(const IVarList& args, IVarList& result)
{
	float src_x = args.FloatVal(0);
	float src_z = args.FloatVal(1);
	float dst_x = args.FloatVal(2);
	float dst_z = args.FloatVal(3);
	float grid_x = args.FloatVal(4);
	float grid_z = args.FloatVal(5);

	CTerrainZone* pZone = m_pZoneManager->GetInZone(grid_x, grid_z);

	if (NULL == pZone)
	{
		return;
	}

	int col = VisUtil_FloatToInt((grid_x - pZone->GetLeft()) * m_fInvCollideSize);
	int row = VisUtil_FloatToInt((grid_z - pZone->GetTop()) * m_fInvCollideSize);

	float unit_size = m_fCollideSize;
	// 碰撞格的矩形范围
	float x1  = pZone->GetLeft() + col * unit_size;
	float z1 = pZone->GetTop() + row * unit_size;
	float x2 = x1 + unit_size;
	float z2 = z1 + unit_size;

	float intersect_x = 1e8;
	float intersect_z = 1e8;

	float dir_x = dst_x - src_x;
	float dir_z = dst_z - src_z;

	if (dir_x < 0)
	{
		float dist1 = src_x - x2;
		float dist2 = dst_x - x2;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				intersect_x = x2;
				
				if (dir_z < 0)
				{
					intersect_z = z1;
				}
				else
				{
					intersect_z = z2;
				}
			}
		}
		else if ((dist1 < 0) != (dist2 < 0))
		{
			// 与此边框的交点位置
			float z = src_z + dir_z * (-dist1 / dir_x);

			if ((z >= z1) && (z <= z2))
			{
				intersect_x = x2;
				intersect_z = z;
			}
		}
	}
	else
	{
		float dist1 = src_x - x1;
		float dist2 = dst_x - x1;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				intersect_x = x1;

				if (dir_z < 0)
				{
					intersect_z = z1;
				}
				else
				{
					intersect_z = z2;
				}
			}
		}
		else if ((dist1 < 0) != (dist2 < 0))
		{
			// 与此边框的交点位置
			float z = src_z + dir_z * (-dist1 / dir_x);

			if ((z >= z1) && (z <= z2))
			{
				intersect_x = x1;
				intersect_z = z;
			}
		}
	}

	if (dir_z < 0)
	{
		float dist1 = src_z - z2;
		float dist2 = dst_z - z2;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				intersect_z = z2;

				if (dir_x < 0)
				{
					intersect_x = x1;
				}
				else
				{
					intersect_x = x2;
				}
			}
		}
		else if ((dist1 < 0) != (dist2 < 0))
		{
			// 与此边框的交点位置
			float x = src_x + dir_x * (-dist1 / dir_z);

			if ((x >= x1) && (x <= x2))
			{
				intersect_x = x;
				intersect_z = z2;
			}
		}
	}
	else
	{
		float dist1 = src_z - z1;
		float dist2 = dst_z - z1;

		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				intersect_z = z1;

				if (dir_x < 0)
				{
					intersect_x = x1;
				}
				else
				{
					intersect_x = x2;
				}
			}
		}
		else if ((dist1 < 0) != (dist2 < 0))
		{
			// 与此边框的交点位置
			float x = src_x + dir_x * (-dist1 / dir_z);

			if ((x >= x1) && (x <= x2))
			{
				intersect_x = x;
				intersect_z = z1;
			}
		}
	}

	if ((intersect_x < 1e7) && (intersect_z < 1e7))
	{
		result.AddFloat(intersect_x);
		result.AddFloat(intersect_z);
	}
}

void Terrain::Pick(const IVarList& args, IVarList& result)
{
	int x = args.IntVal(0);
	int y = args.IntVal(1);
	float distance = args.FloatVal(2);

	trace_info_t ti;

	if (!InnerPick(x, y, distance, ti))
	{
		result.AddBool(false);
		return;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v = pHit->Vertex[0] 
	+ (pHit->Vertex[1] - pHit->Vertex[0]) * pHit->fTraceU 
		+ (pHit->Vertex[2] - pHit->Vertex[0]) * pHit->fTraceV;

	if (pHit->TraceID != this->GetID())
	{
		// 点击到地面对象
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);

			FmVec3TransformCoord(&v, &v, &mat);
		}
	}

	result.AddBool(true);
	result.AddFloat(v.x);
	result.AddFloat(v.y);
	result.AddFloat(v.z);
}

void Terrain::PickGround(const IVarList& args, IVarList& result)
{
	int x = args.IntVal(0);
	int y = args.IntVal(1);
	float distance = args.FloatVal(2);

	trace_info_t ti;

	if (!InnerPick(x, y, distance, ti))
	{
		result.AddBool(false);
		return;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	if (pHit->TraceID != this->GetID())
	{
		// 未点击到地面对象
		result.AddBool(false);
		return;
	}

	FmVec3 v = pHit->Vertex[0] 
		+ (pHit->Vertex[1] - pHit->Vertex[0]) * pHit->fTraceU 
		+ (pHit->Vertex[2] - pHit->Vertex[0]) * pHit->fTraceV;

	result.AddBool(true);
	result.AddFloat(v.x);
	result.AddFloat(v.y);
	result.AddFloat(v.z);
}

PERSISTID Terrain::PickVisual(int x, int y, float distance)
{
	trace_info_t ti;

	if (!InnerPick(x, y, distance, ti))
	{
		return PERSISTID();
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	if (pHit->TraceID == this->GetID())
	{
		return PERSISTID();
	}

	return pHit->TraceID;
}

bool Terrain::InnerPick(int x, int y, float distance, trace_info_t& result)
{
	IRenderContext* pContext = this->GetContext();
	
	if (NULL == pContext)
	{
		return false;
	}
	
	const camera_t& camera = pContext->GetCamera();
	//const dx_viewport_t& vp = camera.Viewport;
	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	const FmMat4& mtxProj = camera.mtxProj;
	const FmMat4& m = camera.mtxViewInverse;
	//int width = (int)vp.fWidth;
	//int height = (int)vp.fHeight;
	
	if ((view_width == 0) || (view_height == 0))
	{
		return false;
	}

	// Compute the vector of the pick ray in screen space
	FmVec3 v1;
	
	v1.x = (((2.0F * x) / (float)view_width) - 1.0F) / mtxProj._11;
	v1.y = -(((2.0F * y) / (float)view_height) - 1.0F) / mtxProj._22;
	v1.z = 1.0F;
	
	FmVec3 vPickRayDir;

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x = v1.x * m._11 + v1.y * m._21 + v1.z * m._31;
	vPickRayDir.y = v1.x * m._12 + v1.y * m._22 + v1.z * m._32;
	vPickRayDir.z = v1.x * m._13 + v1.y * m._23 + v1.z * m._33;

	FmVec3Normalize(&vPickRayDir, &vPickRayDir);

	vPickRayDir *= distance;

	FmVec3 vPickRayOrig;

	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;
	
#ifdef PERFORMANCE_DEBUG
	double dTraceTime = performance_time();
#endif

	bool succeed = m_pCollide->TraceAll(vPickRayOrig, 
		vPickRayOrig + vPickRayDir, result);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dRayTraceTime +=
		performance_time() - dTraceTime;
#endif

	return succeed;
}

PERSISTID Terrain::GetVisual(const char* name)
{
	size_t zone_num = m_pZoneManager->GetZoneNum();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* pZone = m_pZoneManager->GetZoneByIndex(i);

		if (NULL == pZone)
		{
			continue;
		}

		CTerrainVisuals* pVisuals = pZone->GetVisuals();
		
		size_t index;

		if (pVisuals->FindNameIndex(name, index))
		{
			CQuadVisual* pObject = pVisuals->GetVisualByIndex(index);

			return pObject->GetVisualID();
		}
	}

	return PERSISTID();
}

bool Terrain::GetVisualExists(const PERSISTID& id)
{
	size_t zone_num = m_pZoneManager->GetZoneNum();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* pZone = m_pZoneManager->GetZoneByIndex(i);

		if (NULL == pZone)
		{
			continue;
		}

		CTerrainVisuals* pVisuals = pZone->GetVisuals();
		size_t index;

		if (pVisuals->FindVisualIndex(id, index))
		{
			return true;
		}
	}

	return false;
}

bool Terrain::AddVisual(const char* name, const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	return AddVisBase(name, pVisBase, false, 0.0F) == RESULT_SUCCESS;
}

bool Terrain::AddVisualRole(const char* name, const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	return AddVisBase(name, pVisBase, true, 0.0F) == RESULT_SUCCESS;
}

bool Terrain::RemoveVisual(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	return RemoveVisBase(pVisBase) == RESULT_SUCCESS;
}

bool Terrain::RelocateVisual(const PERSISTID& id, float x, float y, float z)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	return RelocateVisBase(pVisBase, x, y, z) == RESULT_SUCCESS;
}

bool Terrain::VisualInView(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	IRenderContext* pContext = this->GetContext();
	FmVec3 center = pVisBase->GetCenter();
	float radius = 0.0F;

	if (pContext->IsFarClip(center, radius))
	{
		return false;
	}
	
	if (pContext->IsViewClip(center, radius))
	{
		return false;
	}

	return true;
}

bool Terrain::SetVisualClipRadius(const PERSISTID& id, float clip_radius)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	FmVec3 pos = pVisBase->GetPosition();
	CTerrainZone* pZone = m_pZoneManager->GetInZone(pos.x, pos.z);

	if (NULL == pZone)
	{
		return false;
	}

	CTerrainVisuals* pVisuals = pZone->GetVisuals();
	size_t index;

	if (!pVisuals->FindVisualIndex(pVisBase->GetID(), index))
	{
		return false;
	}

	CQuadVisual* pObject = pVisuals->GetVisualByIndex(index);

	pObject->SetClipRadius(clip_radius);

	return true;
	//return pZone->GetVisuals()->SetVisualClipRadius(pVisBase, clip_radius);
}

void Terrain::GetNearActorList(const IVarList& args, IVarList& result)
{
	float beg_x = args.FloatVal(0);
	float beg_z = args.FloatVal(1);
	float end_x = args.FloatVal(2);
	float end_z = args.FloatVal(3);

	CTerrainZone* zone1 = m_pZoneManager->GetInZone(beg_x, beg_z);
	
	zone1->GetVisuals()->GetRectActors(beg_x, beg_z, end_x, end_z, result);
	
	CTerrainZone* zone2 = m_pZoneManager->GetInZone(end_x, end_z);

	if (zone2 != zone1)
	{
		zone1->GetVisuals()->GetRectActors(beg_x, beg_z, end_x, end_z, result);

		CTerrainZone* zone3 = m_pZoneManager->GetInZone(beg_x, end_z);
		CTerrainZone* zone4 = m_pZoneManager->GetInZone(end_x, beg_z);

		if ((zone3 != zone1) && (zone3 != zone2))
		{
			zone3->GetVisuals()->GetRectActors(beg_x, beg_z, end_x, end_z, 
				result);
		}

		if ((zone4 != zone1) && (zone4 != zone2))
		{
			zone4->GetVisuals()->GetRectActors(beg_x, beg_z, end_x, end_z, 
				result);
		}
	}
}

int Terrain::GetRolesInSphere(float x, float y, float z, float radius)
{
	int count = 0;
	size_t role_num = m_pCulling->GetRenderRoleCount();
	render_role_t* roles = m_pCulling->GetRenderRoles();

	for (size_t i = 0; i < role_num; ++i)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			roles[i].VisualID);

		if (NULL == pVisBase)
		{
			continue;
		}

		FmVec3 cen = pVisBase->GetCenter();
		float rad = pVisBase->GetRadius();
		float sx = cen.x - x;
		float sy = cen.y - y;
		float sz = cen.z - z;

		if ((sx * sx + sy * sy + sz * sz) < (radius + rad) * (radius + rad))
		{
			count++;
		}
	}

	return count;
}

extern void get_range_zones(Terrain* pTerrain, float beg_x, float beg_z, 
	float end_x, float end_z, TArrayPod<CTerrainZone*, 1, TCoreAlloc>& zones);
//TODO: 待用四叉树优化
void Terrain::GetCircleVisualList(const IVarList& args, IVarList& result)
{
	float center_x = args.FloatVal(0);
	float center_z = args.FloatVal(1);
	float radius = args.FloatVal(2);

	float beg_x = center_x - radius;
	float beg_z = center_z - radius;
	float end_x = center_x + radius;
	float end_z = center_z + radius;

	TArrayPod<CTerrainZone*, 1, TCoreAlloc> zones;

	get_range_zones(this, beg_x, beg_z, end_x, end_z, zones);

	for (size_t i = 0; i < zones.size(); ++i)
	{
		zones[i]->GetVisuals()->GetCircleVisuals(center_x, center_z, radius, 
			result);
	}
}

int Terrain::AddVisBase(const char* name, IVisBase* pVisBase, bool is_role, 
	float clip_radius)
{
	Assert(pVisBase != NULL);

	if (pVisBase->GetContext() != this->GetContext())
	{
		CORE_TRACE("(Terrain::AddVisBase)context not match");
		CORE_TRACE(pVisBase->GetEntInfo()->GetEntityName());
		CORE_TRACE(name);
		return RESULT_CONTEXT_NOT_MATCH;
	}
	
	FmVec3 pos = pVisBase->GetPosition();
	CTerrainZone* zone = m_pZoneManager->GetInZone(pos.x, pos.z);

	if (NULL == zone)
	{
		return RESULT_ZONE_NOT_EXIST;
	}

	if (!zone->GetVisuals()->AddVisual(name, pVisBase, is_role, clip_radius))
	{
		return RESULT_VISUAL_HAD_EXIST;
	}

	// 必须刷新视野
	//m_pCulling->SetMustRefreshVisual(true);

	return RESULT_SUCCESS;
}

int Terrain::RemoveVisBase(IVisBase* pVisBase)
{
	Assert(pVisBase != NULL);

	FmVec3 pos = pVisBase->GetPosition();
	
	CTerrainZone* zone = m_pZoneManager->GetInZone(pos.x, pos.z);
	
	if (NULL == zone)
	{
		return RESULT_ZONE_NOT_EXIST;
	}

	CTerrainVisuals* pVisuals = zone->GetVisuals();

	size_t index;

	if (!pVisuals->FindVisualIndex(pVisBase->GetID(), index))
	{
		return RESULT_VISUAL_NOT_EXIST;
	}

	if (!pVisuals->RemoveByIndex(index))
	{
		return RESULT_UNKOWN;
	}

	//if (!zone->GetVisuals()->RemoveVisual(pVisBase))
	//{
	//	return false;
	//}

	// 必须刷新视野
	//m_pCulling->SetMustRefreshVisual(true);

	return RESULT_SUCCESS;
}

int Terrain::RelocateVisBase(IVisBase* pVisBase, float x, float y, float z)
{
	Assert(pVisBase != NULL);

	CTerrainZone* new_zone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == new_zone)
	{
		return RESULT_ZONE_NOT_EXIST;
	}
	
	FmVec3 pos = pVisBase->GetPosition();
	CTerrainZone* old_zone = m_pZoneManager->GetInZone(pos.x, pos.z);
	
	pVisBase->SetPosition(x, y, z);
	
	if (new_zone != old_zone)
	{
		core_string name;
		bool is_role = true;
		float clip_radius = 0.0F;
		
		if (old_zone)
		{
			CTerrainVisuals* pOldVisuals = old_zone->GetVisuals();
			size_t index;

			if (!pOldVisuals->FindVisualIndex(pVisBase->GetID(), index))
			{
				return RESULT_VISUAL_NOT_EXIST;
			}

			CQuadVisual* pObject = pOldVisuals->GetVisualByIndex(index);
			
			// 保存名字
			name = pObject->GetName();
			is_role = pObject->GetIsRole();
			clip_radius = pObject->GetClipRadius();

			// 从此区域删除
			pOldVisuals->RemoveByIndex(index);
		}

		// 重新添加到其他区域
		new_zone->GetVisuals()->AddVisual(name.c_str(), pVisBase, is_role, 
			clip_radius);
	}
	else
	{
		new_zone->GetVisuals()->MoveVisual(pVisBase, x, y, z);
	}
	
	// 必须刷新视野
	//m_pCulling->SetMustRefreshVisual(true);

	return RESULT_SUCCESS;
}
