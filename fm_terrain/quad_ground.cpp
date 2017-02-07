//--------------------------------------------------------------------
// 文件名:		quad_ground.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "quad_ground.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "terrain_height.h"
#include "zone_manager.h"
#include "chunk_style.h"
#include "math_3d.h"
#include "../visual/vis_utils.h"

// CQuadGround

CQuadGround::CQuadGround()
{
	m_pNode = NULL;
}

CQuadGround::~CQuadGround()
{
}

bool CQuadGround::Init(Terrain* pTerrain, size_t scale, 
	const FmVec3& v_min, const FmVec3& v_max, size_t zone, 
	size_t chunk, size_t index)
{
	m_pTerrain = pTerrain;
	m_nScale = scale;
	m_vMin = v_min;
	m_vMax = v_max;
	m_nZone = zone;
	m_nChunk = chunk;
	m_nIndex = index;
	
	return true;
}

bool CQuadGround::IsTraceMasked(unsigned int trace_mask)
{
	return (trace_mask & ITerrain::TRACEMASK_GROUND) > 0;
}

// 测试三角形碰撞
static inline bool trace_triangle(Terrain* pTerrain, const FmVec3& v0, 
	const FmVec3& v1, const FmVec3& v2, const FmVec3& src, 
	const FmVec3& dir, trace_info_t& result, bool& hitted)
{
	float t;
	float u;
	float v;

	// 射线与三角形相交
	if (VisUtil_IntersectTri(&v0, &v1, &v2, &src, &dir, &u, &v, &t))
	{
		if ((t >= 0.0F) && (t <= 1.0F))
		{
			if (result.nHitMax > 1)
			{
				if (result.nHitCount >= result.nHitMax)
				{
					return false;
				}

				hitted = true;

				if (t < result.fDistance)
				{
					// 距离最近的碰撞点
					result.fDistance = t;
				}

				trace_info_t::hit_t* pHit = &result.Hits[result.nHitCount];

				pHit->fDistance = t;
				pHit->fTraceU = u;
				pHit->fTraceV = v;
				// 设置为地形对象
				pHit->TraceID = pTerrain->GetID();
				pHit->nMaterialIndex = 0;
				pHit->Vertex[0] = v0;
				pHit->Vertex[1] = v1;
				pHit->Vertex[2] = v2;
				pHit->strTexName = "";

				if (++result.nHitCount >= result.nHitMax)
				{
					return false;
				}
			}
			else if (t < result.fDistance)
			{
				hitted = true;

				// 距离最近的碰撞点
				result.fDistance = t;

				trace_info_t::hit_t* pHit = &result.Hits[0];

				pHit->fDistance = t;
				pHit->fTraceU = u;
				pHit->fTraceV = v;
				// 设置为地形对象
				pHit->TraceID = pTerrain->GetID();
				pHit->nMaterialIndex = 0;
				pHit->Vertex[0] = v0;
				pHit->Vertex[1] = v1;
				pHit->Vertex[2] = v2;
				pHit->strTexName = "";

				result.nHitCount = 1;
			}
		}
	}

	return true;
}

bool CQuadGround::TraceDetail(const FmVec3& src, const FmVec3& dir,
	trace_info_t& result)
{
	Terrain* pTerrain = m_pTerrain;
	CTerrainZone* zone = pTerrain->GetZoneManager()->GetZoneByIndex(m_nZone);
	
	if (NULL == zone)
	{
		return false;
	}
	
	CTerrainChunk* chunk = zone->GetChunkByIndex(m_nChunk);
	
	if (NULL == chunk)
	{
		return false;
	}
	
	CChunkStyle* style = pTerrain->GetChunkStyle(chunk->GetStyle());
	CChunkStyle::collide_info_t* ci = style->GetCollideInfo(m_nIndex);
	size_t triangle_num = ci->triangles.size();
	CChunkStyle::triangle_info_t* triangle_info = &ci->triangles[0];
	
	CTerrainHeight* pHeight = zone->GetHeight();
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	float left = zone->GetLeft();
	float top = zone->GetTop();
	int local_row = chunk->GetLocalRow();
	int local_col = chunk->GetLocalCol();
	float unit_size = pTerrain->GetUnitSize();
	bool is_unit_one = (unit_size == 1.0F);

	// 逐个三角形做碰撞检测
	FmVec4 v0;
	FmVec4 v1;
	FmVec4 v2;
	bool hitted = false;

	if (is_unit_one)
	{
		for (size_t k = 0; k < triangle_num; ++k)
		{
			CChunkStyle::triangle_info_t& ti = triangle_info[k];

			int row1 = ti.nVertexRow1 + local_row;
			int col1 = ti.nVertexCol1 + local_col;
			int row2 = ti.nVertexRow2 + local_row;
			int col2 = ti.nVertexCol2 + local_col;
			int row3 = ti.nVertexRow3 + local_row;
			int col3 = ti.nVertexCol3 + local_col;

			v0.x = left + col1;
			v1.x = left + col2;
			v2.x = left + col3;
			v0.y = height_values[row1 * height_cols + col1];
			v1.y = height_values[row2 * height_cols + col2];
			v2.y = height_values[row3 * height_cols + col3];
			v0.z = top + row1;
			v1.z = top + row2;
			v2.z = top + row3;

			if (!trace_triangle(pTerrain, v0, v1, v2, src, dir, result, hitted))
			{
				break;
			}
		}
	}
	else
	{
		for (size_t k = 0; k < triangle_num; ++k)
		{
			CChunkStyle::triangle_info_t& ti = triangle_info[k];

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

			if (!trace_triangle(pTerrain, v0, v1, v2, src, dir, result, hitted))
			{
				break;
			}
		}
	}

	return hitted;
}

bool CQuadGround::TraceHitted(const FmVec3& src, const FmVec3& dir)
{
	Terrain* pTerrain = m_pTerrain;
	CTerrainZone* zone = pTerrain->GetZoneManager()->GetZoneByIndex(m_nZone);
	
	if (NULL == zone)
	{
		return false;
	}
	
	CTerrainChunk* chunk = zone->GetChunkByIndex(m_nChunk);
	
	if (NULL == chunk)
	{
		return false;
	}
	
	CChunkStyle* style = pTerrain->GetChunkStyle(chunk->GetStyle());
	CChunkStyle::collide_info_t* ci = style->GetCollideInfo(m_nIndex);
	size_t triangle_num = ci->triangles.size();
	CChunkStyle::triangle_info_t* triangle_info = &ci->triangles[0];

	CTerrainHeight* pHeight = zone->GetHeight();
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	float left = zone->GetLeft();
	float top = zone->GetTop();
	int local_row = chunk->GetLocalRow();
	int local_col = chunk->GetLocalCol();
	float unit_size = pTerrain->GetUnitSize();
	bool is_unit_one = (unit_size == 1.0F);

	// 逐个三角形做碰撞检测
	FmVec3 v0;
	FmVec3 v1;
	FmVec3 v2;

	if (is_unit_one)
	{
		for (size_t k = 0; k < triangle_num; ++k)
		{
			CChunkStyle::triangle_info_t& ti = triangle_info[k];

			int row1 = ti.nVertexRow1 + local_row;
			int col1 = ti.nVertexCol1 + local_col;
			int row2 = ti.nVertexRow2 + local_row;
			int col2 = ti.nVertexCol2 + local_col;
			int row3 = ti.nVertexRow3 + local_row;
			int col3 = ti.nVertexCol3 + local_col;

			v0.x = left + col1;
			v1.x = left + col2;
			v2.x = left + col3;
			v0.y = height_values[row1 * height_cols + col1];
			v1.y = height_values[row2 * height_cols + col2];
			v2.y = height_values[row3 * height_cols + col3];
			v0.z = top + row1;
			v1.z = top + row2;
			v2.z = top + row3;

			float t;
			float u;
			float v;

			// 射线与三角形相交
			if (VisUtil_IntersectTri(&v0, &v1, &v2, &src, &dir, &u, &v, &t))
			{
				if ((t >= 0.0F) && (t <= 1.0F))
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (size_t k = 0; k < triangle_num; ++k)
		{
			CChunkStyle::triangle_info_t& ti = triangle_info[k];

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
					return true;
				}
			}
		}
	}

	return false;
}

bool CQuadGround::TraceSphere(const FmVec3& center, float radius)
{
	Terrain* pTerrain = m_pTerrain;
	CTerrainZone* zone = pTerrain->GetZoneManager()->GetZoneByIndex(m_nZone);

	if (NULL == zone)
	{
		return false;
	}

	CTerrainChunk* chunk = zone->GetChunkByIndex(m_nChunk);

	if (NULL == chunk)
	{
		return false;
	}

	CChunkStyle* style = pTerrain->GetChunkStyle(chunk->GetStyle());
	CChunkStyle::collide_info_t* ci = style->GetCollideInfo(m_nIndex);
	size_t triangle_num = ci->triangles.size();
	CChunkStyle::triangle_info_t* triangle_info = &ci->triangles[0];

	CTerrainHeight* pHeight = zone->GetHeight();
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	float left = zone->GetLeft();
	float top = zone->GetTop();
	int local_row = chunk->GetLocalRow();
	int local_col = chunk->GetLocalCol();
	float unit_size = pTerrain->GetUnitSize();

	// 逐个三角形做碰撞检测
	FmVec4 v0;
	FmVec4 v1;
	FmVec4 v2;

	for (size_t k = 0; k < triangle_num; ++k)
	{
		CChunkStyle::triangle_info_t& ti = triangle_info[k];

		//chunk->GetCollideTriangle(&ti.nVertexRow1, &v0, &v1, &v2);
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

		if (SphereTriangleCollision(center, radius, v0, v1, v2))
		{
			return true;
		}
	}

	return false;
}

bool CQuadGround::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	if (cull_func == NULL)
	{
		return true;
	}

	Terrain* pTerrain = m_pTerrain;
	CTerrainZone* zone = pTerrain->GetZoneManager()->GetZoneByIndex(m_nZone);
	
	if (NULL == zone)
	{
		return false;
	}
	
	CTerrainChunk* chunk = zone->GetChunkByIndex(m_nChunk);
	
	if (NULL == chunk)
	{
		return false;
	}
	
	if (!VisUtil_BoxIntersectSphere(chunk->GetMin(), chunk->GetMax(),
		center, radius))
	{
		// 先与包围盒做测试
		return false;
	}

	CChunkStyle* style = pTerrain->GetChunkStyle(chunk->GetStyle());
	CChunkStyle::collide_info_t* ci = style->GetCollideInfo(m_nIndex);
	size_t triangle_num = ci->triangles.size();
	CChunkStyle::triangle_info_t* triangle_info = &ci->triangles[0];

	CTerrainHeight* pHeight = zone->GetHeight();
	size_t height_cols = pHeight->GetCols();
	const float* height_values = pHeight->GetHeightValues();
	float left = zone->GetLeft();
	float top = zone->GetTop();
	int local_row = chunk->GetLocalRow();
	int local_col = chunk->GetLocalCol();
	float unit_size = pTerrain->GetUnitSize();

	FmMat4 mat;

	FmMat4Identity(&mat);

	FmVec3 v[3];

	// 逐个三角形做拣选
	for (size_t k = 0; k < triangle_num; ++k)
	{
		CChunkStyle::triangle_info_t& ti = triangle_info[k];

		//chunk->GetCollideTriangle(&ti.nVertexRow1, &v[0], &v[1], &v[2]);
		int row1 = ti.nVertexRow1 + local_row;
		int col1 = ti.nVertexCol1 + local_col;
		int row2 = ti.nVertexRow2 + local_row;
		int col2 = ti.nVertexCol2 + local_col;
		int row3 = ti.nVertexRow3 + local_row;
		int col3 = ti.nVertexCol3 + local_col;

		v[0].x = left + col1 * unit_size;
		v[1].x = left + col2 * unit_size;
		v[2].x = left + col3 * unit_size;
		v[0].y = height_values[row1 * height_cols + col1];
		v[1].y = height_values[row2 * height_cols + col2];
		v[2].y = height_values[row3 * height_cols + col3];
		v[0].z = top + row1 * unit_size;
		v[1].z = top + row2 * unit_size;
		v[2].z = top + row3 * unit_size;

		if (plane_num > 0)
		{
			// 使用剪裁面
			bool bInEx = false;

			for (size_t k = 0; k < 3; ++k)
			{
				bool bIn = true;

				for (size_t p = 0; p < plane_num; p++)
				{
					float fDistance = FmPlaneDotCoord(&planes[p], &v[k]);

					if (fDistance > 0.0F)
					{
						bIn = false;
						break;
					}
				}

				if (bIn)
				{
					bInEx = true;
					break;
				}
			}

			if (!bInEx)
			{
				continue;
			}

			if (!cull_func(context, mat, &v[0], 3))
			{
				return true;
			}
		}
		else
		{
			// 只用包围球做碰撞检测
			if (!SphereTriangleAABBCollision(center, radius, v[0], v[1], 
				v[2]))
			{
				continue;
			}
			
			if (!cull_func(context, mat, &v[0], 3))
			{
				return true;
			}
		}
	}

	return false;
}

