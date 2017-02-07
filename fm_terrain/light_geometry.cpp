//--------------------------------------------------------------------
// 文件名:		light_geometry.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "light_geometry.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_light.h"
#include "terrain_visuals.h"
#include "tex_light_map.h"
#include "quad_visual.h"
#include "zone_manager.h"
#include "../visual/i_model.h"
#include "../visual/i_model_player.h"
#include "../visual/i_image_access.h"
#include "../public/var.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/inlines.h"

#define SHADOW_BLOCK_SIZE 64

// CLightGeometry

CLightGeometry::CLightGeometry()
{
	m_pTerrain = NULL;
	m_pRender = NULL;
	m_fRadius = 0.0f;
	m_bUseColor = false;
	m_bOnlyGround = false;
	m_bSaveOcclusion = false;
	m_bLightMapDegrade = false;
	m_nLightNum = 0;
	m_nLightMapSize = 256;
	m_nLightMapFill = 3;
	m_nLightPerUnit = 8;
	//m_pShadows = NULL;
}

CLightGeometry::~CLightGeometry()
{
	ReleaseAll();
}

void CLightGeometry::ReleaseAll()
{
//	if (m_pShadows)
//	{
//		delete[] m_pShadows;
//		m_pShadows = NULL;
//	}

	for (size_t k = 0; k < m_Shadows.size(); ++k)
	{
		CORE_FREE(m_Shadows[k], 
			sizeof(shadow_t) * m_nLightNum * SHADOW_BLOCK_SIZE);
	}

	for (size_t i = 0; i < m_LightMaps.size(); ++i)
	{
		CORE_FREE(m_LightMaps[i]->pPixels, sizeof(light_map_pixel_t) 
			* m_LightMaps[i]->nPixelWidth * m_LightMaps[i]->nPixelHeight);
	}

	for (size_t zone = 0; zone < m_Zones.size(); ++zone)
	{
		CORE_DELETE(m_Zones[zone]);
	}

	for (size_t object = 0; object < m_Objects.size(); ++object)
	{
		CORE_DELETE(m_Objects[object]);
	}

	for (size_t lightmap = 0; lightmap < m_LightMaps.size(); ++lightmap)
	{
		CORE_DELETE(m_LightMaps[lightmap]);
	}

	m_Zones.clear();
	m_Objects.clear();
	m_LightMaps.clear();
	m_Shadows.clear();
	m_Vertices.clear();
	m_Triangles.clear();
	m_VertexBucket.Clear();
	m_TriangleBucket.Clear();
}

void CLightGeometry::SetZonePath(const char* value)
{
	Assert(value != NULL);

	m_strZonePath = value;
}

const char* CLightGeometry::GetZonePath() const
{
	return m_strZonePath.c_str();
}

void CLightGeometry::SetModelPath(const char* value)
{
	Assert(value != NULL);

	m_strModelPath = value;
}

const char* CLightGeometry::GetModelPath() const
{
	return m_strModelPath.c_str();
}

void CLightGeometry::SetModelSuffix(const char* value)
{
	Assert(value != NULL);

	m_strModelSuffix = value;
}

const char* CLightGeometry::GetModelSuffix() const
{
	return m_strModelSuffix.c_str();
}

bool CLightGeometry::AddZone(int zone_index, int density, float add_size, 
	int tex_scale)
{
	if (density <= 0)
	{
		return false;
	}

	if (add_size < 0.0f)
	{
		return false;
	}
	
	if (NULL == m_pTerrain)
	{
		return false;
	}
	
	CZoneManager* pManager = m_pTerrain->GetZoneManager();
	CTerrainZone* zone = pManager->GetZoneByIndex(zone_index);

	if (NULL == zone)
	{
		return false;
	}

	size_t zone_scale = m_pTerrain->GetZoneScale();
	size_t vertex_scale = zone_scale * density;

	double unit_x = double(m_pTerrain->GetZoneWidth()) 
		/ double(vertex_scale - 1);
	double unit_z = double(m_pTerrain->GetZoneHeight()) 
		/ double(vertex_scale - 1);
	
	size_t zone_num = m_Zones.size();
	zone_t* pZone = CORE_NEW(zone_t);

	m_Zones.push_back(pZone);

	// for test
	if ((tex_scale > 0) && (tex_scale < int(vertex_scale)))
	{
		vertex_scale = tex_scale;
	}

	pZone->strName = zone->GetName();
	pZone->strLightName = m_strZonePath + zone->GetName();
	pZone->nZoneIndex = zone_index;
	pZone->nDensity = density;
	pZone->fAddSize = add_size;
	pZone->nBufStart = 0;
	pZone->nBufSize = 0;
	pZone->dUnitX = unit_x;
	pZone->dUnitZ = unit_z;
	pZone->nVertexRows = vertex_scale;
	pZone->nVertexCols = vertex_scale;
	pZone->nHelperRows = int(double(add_size) / unit_z);
	pZone->nHelperCols = int(double(add_size) / unit_x);
	
	return true;
}

bool CLightGeometry::AddObject(const char* name, const PERSISTID& model,
	bool save_light, const char* light_type, int light_map_size, 
	int light_map_fill)
{
	Assert(name != NULL);

	IModel* pModel = (IModel*)m_pRender->GetCore()->GetEntity(model);

	if (NULL == pModel)
	{
		CORE_TRACE("(CLightGeometry::AddObject)id invalid");
		CORE_TRACE(name);
		return false;
	}

	if (!pModel->GetEntInfo()->IsKindOf("Model"))
	{
		// 不是普通模型
		return true;
	}

	size_t object_num = m_Objects.size();

	m_Objects.push_back(CORE_NEW(object_t));

	m_Objects[object_num]->strName = name;
	m_Objects[object_num]->strLightName = m_strModelPath + name 
		+ m_strModelSuffix;
	m_Objects[object_num]->ModelId = model;
	m_Objects[object_num]->pModel = pModel;
	m_Objects[object_num]->bSaveLight = save_light;
	m_Objects[object_num]->bLightMap = stricmp(light_type, "LightMap") == 0;
	m_Objects[object_num]->nLightMapSize = light_map_size;
	m_Objects[object_num]->nLightMapFill = light_map_fill;

	return true;
}

// 球体是否包含在方形范围内
static bool rect_contain_sphere(const FmVec4& v_min, 
	const FmVec4& v_max, const FmVec4& center, float radius)
{
	return (center.x >= (v_min.x - radius)) 
		&& (center.x <= (v_max.x + radius))
		&& (center.z >= (v_min.z - radius)) 
		&& (center.z <= (v_max.z + radius));
}

bool CLightGeometry::AddTriangle(size_t i1, size_t i2, size_t i3, bool trace)
{
	Assert(i1 < m_Vertices.size());
	Assert(i2 < m_Vertices.size());
	Assert(i3 < m_Vertices.size());

	vertex_t* v1 = m_Vertices[i1];
	vertex_t* v2 = m_Vertices[i2];
	vertex_t* v3 = m_Vertices[i3];
	
	FmVec4 nrm;
	
	FmVec3Cross(&nrm, &(v2->p - v1->p), &(v3->p - v1->p));
	
	float sq = D3DXVec3Length(&nrm);
	
	if (sq <= 0.0f)
	{
		CORE_TRACE("(CLightGeometry::AddTriangle)model have zero triangle");
		return false;
	}
	
	triangle_t* pTriangle = m_TriangleBucket.New();

	pTriangle->n = nrm * (1.0f / sq);
	pTriangle->sq = sq;
	pTriangle->i[0] = i1;
	pTriangle->i[1] = i2;
	pTriangle->i[2] = i3;

	m_Triangles.push_back(pTriangle);
	
	for (size_t nv = 0; nv < 3; nv++)
	{
		vertex_t* vr = m_Vertices[pTriangle->i[nv]];
		
		bool isInv = D3DXVec3Dot(&pTriangle->n, &vr->n) < 0.0f;
		
		if (vr->flags & vertex_t::f_set)
		{
			if (((vr->flags& vertex_t::f_inv) != 0) != isInv)
			{
				CORE_TRACE("(CLightGeometry::AddTriangle)"
					"model have bug normals");
				
				vr->flags |= vertex_t::f_bug;
			}
		}
		else
		{
			if (isInv)
			{
				vr->flags |= vertex_t::f_inv;
			}
		}
	}
	
	FmVec4 pos = (v1->p + v2->p + v3->p) / 3.0f;
	
	if (trace)
	{
		pos.y = m_pTerrain->GetGroundHeight(pos.x, pos.z);
	}

	pos += pTriangle->n * 0.001F;

	pTriangle->p = pos;

	return true;
}

bool CLightGeometry::AddLightMapTriangle(size_t i1)
{
	Assert(i1 < m_Vertices.size());

	vertex_t* v1 = m_Vertices[i1];

	float sq = D3DXVec3Length(&v1->n);

	FmVec4 n = v1->n * (1.0f / sq);

	triangle_t* pTriangle = m_TriangleBucket.New();

	pTriangle->n = n;
	pTriangle->sq = sq / 3.0f;
	pTriangle->i[0] = i1;
	pTriangle->i[1] = i1;
	pTriangle->i[2] = i1;
	pTriangle->p = v1->p + n * 0.001F;
	
	m_Triangles.push_back(pTriangle);
	
	return true;
}

float CLightGeometry::GetY(size_t start, size_t rows, size_t cols, int row, 
	int col, float def)
{
	if (size_t(row) >= rows)
	{
		return def;
	}

	if (size_t(col) >= cols)
	{
		return def;
	}

	return m_Vertices[start + row * cols + col]->p.y;
}

bool CLightGeometry::GetTriNormal(size_t start, size_t rows, size_t cols, 
	int row, int col, FmVec4& normal)
{
	if (size_t(row) >= rows)
	{
		return false;
	}

	if (size_t(col) >= cols)
	{
		return false;
	}
	
	normal = m_Triangles[start + row * cols + col]->n;

	return true;
}

bool CLightGeometry::ProcessZones()
{
	CZoneManager* pManager = m_pTerrain->GetZoneManager();
	ICore* pCore = m_pTerrain->GetCore();
	size_t zone_num = m_Zones.size();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* zone = pManager->GetZoneByIndex(m_Zones[i]->nZoneIndex);

		if (NULL == zone)
		{
			CORE_TRACE("(CLightGeometry::ProcessZones)zone invalid");
			CORE_TRACE(m_Zones[i]->strName.c_str());
			return false;
		}

		// 添加地面的顶点
		m_Zones[i]->nBufStart = m_Vertices.size();
			
		// 顶点间距
		double unit_x = m_Zones[i]->dUnitX;
		double unit_z = m_Zones[i]->dUnitZ;
		
		// 范围
		double left = zone->GetLeft() - unit_x * m_Zones[i]->nHelperCols;
		double top = zone->GetTop() - unit_z * m_Zones[i]->nHelperRows;
		size_t vertex_count = 0;
		size_t rows = m_Zones[i]->nVertexRows + m_Zones[i]->nHelperRows * 2;
		size_t cols = m_Zones[i]->nVertexCols + m_Zones[i]->nHelperCols * 2;

		for (size_t r = 0; r < rows; ++r)
		{
			double z = top + r * unit_z;
			
			for (size_t c = 0; c < cols; ++c)
			{
				double x = left + c * unit_x;
				
				float y = m_pTerrain->GetGroundHeight(float(x), float(z));
				
				vertex_t* pVertex = m_VertexBucket.New();
				
				pVertex->p = FmVec4(float(x), y, float(z));
				pVertex->n = FmVec4(0.0f, 1.0f, 0.0f);
				pVertex->c = FmVec4(0.0f, 0.0f, 0.0f);
				pVertex->bc = FmVec4(0.0f, 0.0f, 0.0f);
				pVertex->mc = FmVec4(1.0f, 1.0f, 1.0f);
				pVertex->ao = FmVec4(1.0f, 1.0f, 1.0f);
				pVertex->alpha = 0xFF000000;
				pVertex->flags = vertex_t::f_zero;
				pVertex->shadow = NULL;

				m_Vertices.push_back(pVertex);

				vertex_count++;
			}
		}

		m_Zones[i]->nBufSize = vertex_count;

		size_t start = m_Zones[i]->nBufStart;
		
		// 添加地面的三角形
		size_t tri_start = m_Triangles.size();

		for (size_t r = 0; r < (rows - 1); ++r)
		{
			for (size_t c = 0; c < (cols - 1); ++c)
			{
				size_t i00 = start + r * cols + c;
				size_t i01 = start + (r + 1) * cols + c;
				size_t i10 = start + r * cols + (c + 1);
				size_t i11 = start + (r + 1) * cols + (c + 1);

				AddTriangle(i00, i01, i10, true);
				AddTriangle(i01, i11, i10, true);
			}
		}
		
		// 根据地面三角形计算顶点的法线
		size_t tri_rows = (rows - 1);
		size_t tri_cols = (cols - 1) * 2;
		
		for (int row = 0; row < int(rows); ++row)
		{
			for (int col = 0; col < int(cols); ++col)
			{
				FmVec4 normal = FmVec4(0.0f, 0.0f, 0.0f);

				FmVec4 n;

				if (GetTriNormal(tri_start, tri_rows, tri_cols,
					row - 1, (col - 1) * 2 + 1, n))
				{
					normal += n * 2.0f;
				}

				if (GetTriNormal(tri_start, tri_rows, tri_cols,
					row - 1, col * 2, n))
				{
					normal += n;
				}
				
				if (GetTriNormal(tri_start, tri_rows, tri_cols,
					row - 1, col * 2 + 1, n))
				{
					normal += n;
				}
				
				if (GetTriNormal(tri_start, tri_rows, tri_cols,
					row, (col - 1) * 2, n))
				{
					normal += n;
				}
				
				if (GetTriNormal(tri_start, tri_rows, tri_cols,
					row, (col - 1) * 2 + 1, n))
				{
					normal += n;
				}
				
				if (GetTriNormal(tri_start, tri_rows, tri_cols,
					row, col * 2, n))
				{
					normal += n * 2.0f;
				}
				
				FmVec3Normalize(&normal, &normal);
				
				m_Vertices[start + row * cols + col]->n = normal;
			}
		}

		if (m_bOnlyGround)
		{
			continue;
		}

		size_t start_object_num = m_Objects.size();
		size_t start_vertex_num = m_Vertices.size();
		size_t density = m_Zones[i]->nDensity;
		float add_size = m_Zones[i]->fAddSize;

		// 区域中的地面物体
		CTerrainVisuals* pVisuals = zone->GetVisuals();
		size_t visual_size = pVisuals->GetVisualSize();

		for (size_t k = 0; k < visual_size; ++k)
		{
			CQuadVisual* pObject = pVisuals->GetVisualByIndex(k);

			if (NULL == pObject)
			{
				continue;
			}

			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				pObject->GetVisualID());

			if (NULL == pVisBase)
			{
				continue;
			}

			char name[256];

			SafeSprintf(name, sizeof(name), "%s_%s", 
				m_Zones[i]->strName.c_str(), pObject->GetName());

			//std::string name = m_Zones[i].strName + "_" + pObject->GetName();
			const char* light_type = GetModelLightType(pVisBase);

			AddObject(name, pObject->GetVisualID(), true, light_type, 
				m_nLightMapSize, m_nLightMapFill);
		}

		// 处理区域中的对象
		if (!ProcessObjects(start_object_num))
		{
			CORE_TRACE("(CLightGeometry::ProcessZones)"
				"process light object failed");
			CORE_TRACE(m_Zones[i]->strName.c_str());
			return false;
		}

		// 获得所有区域顶点的包围盒
		FmVec4 v_min;
		FmVec4 v_max;

		v_min.x = zone->GetLeft();
		v_min.y = 0.0f;
		v_min.z = zone->GetTop();
		v_max.x = zone->GetRight();
		v_max.y = 0.0f;
		v_max.z = zone->GetBottom();

		CalcBoundBox(start_vertex_num, v_min, v_max);

		// 相邻区域中的物体
		size_t light_object_num = m_Objects.size();

		for (size_t s = 0; s < 8; ++s)
		{
			CTerrainZone* sibling = 
				pManager->GetSiblingZoneByIndex(zone, s);

			if (NULL == sibling)
			{
				continue;
			}

			CTerrainVisuals* pVisuals = sibling->GetVisuals();
			size_t visual_num = pVisuals->GetVisualSize();

			for (size_t k = 0; k < visual_num; ++k)
			{
				CQuadVisual* pObject = pVisuals->GetVisualByIndex(k);

				if (NULL == pObject)
				{
					continue;
				}

				IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
					pObject->GetVisualID());

				if (NULL == pVisBase)
				{
					continue;
				}

				// 是否在影响范围之内
				if (!rect_contain_sphere(v_min, v_max, pVisBase->GetCenter(), 
					pVisBase->GetRadius() + add_size))
				{
					continue;
				}

				const char* light_type = GetModelLightType(pVisBase);

				// 添加辅助对象
				AddObject(pObject->GetName(), pObject->GetVisualID(), 
					false, light_type, m_nLightMapSize, m_nLightMapFill);
			}
		}

		// 处理辅助物体
		if (!ProcessObjects(light_object_num))
		{
			CORE_TRACE("(CLightGeometry::ProcessZones)"
				"process helper object failed");
			CORE_TRACE(m_Zones[i]->strName.c_str());
			return false;
		}
	}

	return true;
}

const char* CLightGeometry::GetModelLightType(IVisBase* pVisBase)
{
	Assert(pVisBase != NULL);

	CVar has_light_map;

	if (!pVisBase->GetCore()->GetProperty(pVisBase, "HasLightMap", 
		has_light_map))
	{
		return "None";
	}

	if (has_light_map.BoolVal())
	{
		return "LightMap";
	}
	else
	{
		return "None";
	}
}

bool CLightGeometry::ProcessObjects(size_t start_index)
{
	size_t object_num = m_Objects.size();

	for (size_t i = start_index; i < object_num; i++)
	{
		m_Objects[i]->nBufStart = m_Vertices.size();

		const char* light_type = GetModelLightType(m_Objects[i]->pModel);

		if (stricmp(light_type, "None") == 0)
		{
			// 无需光照处理
			m_Objects[i]->bSaveLight = false;
			m_Objects[i]->nBufSize = 0;
			continue;
		}

		IModelPlayer* pModelPlayer = m_Objects[i]->pModel->GetModelPlayer();

		if (NULL == pModelPlayer)
		{
			continue;
		}

		if (!pModelPlayer->GetLightingEnable())
		{
			// 不可实施光照
			m_Objects[i]->bSaveLight = false;
			m_Objects[i]->nBufSize = 0;
			continue;
		}

		if (m_Objects[i]->bSaveLight && m_Objects[i]->bLightMap)
		{
			// 预处理光贴图
			ProcessLightMaps(i);
			continue;
		}

		size_t vertex_count = 0;

		// 模型转换矩阵
		FmMat4 mat;

		m_Objects[i]->pModel->GetWorldMatrix(mat);

		size_t geo_vb_count = pModelPlayer->GetVertexBufferCount();

		if (0 == geo_vb_count)
		{
			CORE_TRACE("(CLightGeometry::ProcessObject)not vertex buffers");
			CORE_TRACE(m_Objects[i]->strName.c_str());
			m_Objects[i]->nBufSize = 0;
			continue;
		}

		TArrayPod<size_t, 1, TLightGeometryAlloc> vb_start;

		for (int vb = 0; vb < int(geo_vb_count); vb++)
		{
			// 记录起始定点位置
			vb_start.push_back(m_Vertices.size());

			int position_offset = pModelPlayer->GetVertexPositionOffset(vb);
			int normal_offset = pModelPlayer->GetVertexNormalOffset(vb);
			int diffuse_offset = pModelPlayer->GetVertexDiffuseOffset(vb);

			if ((position_offset < 0) || (normal_offset < 0)
				|| (diffuse_offset < 0))
			{
				CORE_TRACE("(CLightGeometry::ProcessObject)"
					"incorrect format of vertex buffer");
				CORE_TRACE(m_Objects[i]->strName.c_str());
				return false;
			}

			//int stride = pModelPlayer->GetVertexDataStride(vb);
			int num = pModelPlayer->GetVertexCount(vb);

			if (num <= 0)
			{
				CORE_TRACE("(CLightGeometry::ProcessObject)"
					"incorrect number of vertices in vertex buffer");
				CORE_TRACE(m_Objects[i]->strName.c_str());
				return false;
			}

			//unsigned char* pnt = pModelPlayer->GetVertexData(vb);
			dx_vertex_data_t* pVertexData = pModelPlayer->GetVertexData(vb);

			for (int v = 0; v < num; v++)
			{
				vertex_t* pVertex = m_VertexBucket.New();
				//FmVec4* pos = 
				//	(FmVec4*)(pnt + v * stride + position_offset);
				unsigned char* pv = get_vertex_by_index(pVertexData, v);
				FmVec4* pos = (FmVec4*)(pv + position_offset);

				// 转换后的顶点位置
				D3DXVec3TransformCoord(&pVertex->p, pos, &mat);

				//FmVec4* nrm = 
				//	(FmVec4*)(pnt + v * stride + normal_offset);
				FmVec4* nrm = (FmVec4*)(pv + normal_offset);

				if ((nrm->x == 0.0F) && (nrm->y == 0.0F) && (nrm->z == 0.0F))
				{
					CORE_TRACE("(CLightGeometry::ProcessObject)"
						"normal have zero length");
					CORE_TRACE(m_Objects[i]->strName.c_str());
				}

				FmVec3Normalize(nrm, nrm);

				pVertex->n = *nrm;
				pVertex->c = FmVec4(0.0f, 0.0f, 0.0f);
				pVertex->bc = FmVec4(0.0f, 0.0f, 0.0f);
				pVertex->mc = FmVec4(1.0f, 1.0f, 1.0f);
				pVertex->ao = FmVec4(1.0f, 1.0f, 1.0f);
				pVertex->alpha = 0xFF000000;
				pVertex->flags = vertex_t::f_zero;
				pVertex->shadow = NULL;

				m_Vertices.push_back(pVertex);
				vertex_count++;

				if (m_bUseColor)
				{
					//unsigned int color = 
					//	*(unsigned int*)(pnt + v * stride + diffuse_offset);
					unsigned int color = *(unsigned int*)(pv +  diffuse_offset);

					pVertex->mc.x = 
						((color >> 16) & 255) * (1.0f / 255.0f) * 2.0f;
					pVertex->mc.y = 
						((color >> 8) & 255) * (1.0f / 255.0f) * 2.0f;
					pVertex->mc.z = 
						((color >> 0) & 255) * (1.0f / 255.0f) * 2.0f;
					pVertex->alpha = color & 0xFF000000;
				}
			}
		}

		m_Objects[i]->nBufSize = vertex_count;

		// 添加三角形信息
		size_t geo_obj_count = pModelPlayer->GetObjectCount();

		for (int n = 0; n < int(geo_obj_count); n++)
		{
			int vb_index =	pModelPlayer->GetObjectVerterBufferIndex(n);
			int vb;

			for (vb = 0; vb < int(geo_vb_count); vb++)
			{
				if (vb_index == vb)
				{
					break;
				}
			}

			if (vb >= int(geo_vb_count))
			{
				CORE_TRACE("(CLightGeometry::ProcessObject)"
					"vertex buffer not found");
				CORE_TRACE(m_Objects[i]->strName.c_str());
				return false;
			}

			size_t start = vb_start[vb] + pModelPlayer->GetObjectStartVertex(n);
			unsigned short* obj_triangles = pModelPlayer->GetObjectIndexData(n);
			size_t obj_triangle_num = pModelPlayer->GetObjectTriangleCount(n);
			size_t obj_vertex_num = pModelPlayer->GetObjectVertexCount(n);

			for (int t = 0; t < int(obj_triangle_num); t++)
			{
				int i1 = obj_triangles[t * 3 + 0];
				int i2 = obj_triangles[t * 3 + 1];
				int i3 = obj_triangles[t * 3 + 2];

				if ((i1 >= int(obj_vertex_num)) 
					|| (i2 >= int(obj_vertex_num)) 
					|| (i3 >= int(obj_vertex_num)))
				{
					CORE_TRACE("(CLightGeometry::ProcessObject)"
						"model have incorrect vertex index");
					CORE_TRACE(m_Objects[i]->strName.c_str());
					return false;
				}

				i1 += start; 
				i2 += start; 
				i3 += start;

				if (!AddTriangle(i1, i2, i3, false))
				{
					CORE_TRACE("(CLightGeometry::ProcessObject)"
						"add triangle failed");
					CORE_TRACE(m_Objects[i]->strName.c_str());
				}
			}
		}
	}

	return true;
}

// 获得点与线的关系
inline int IsPointInLineLH(const FmVec2& point, 
	const FmVec2& line_start, const FmVec2& line_end)
{
	float distance = (line_end.x - line_start.x) * (line_start.y - point.y) 
		- (line_start.x - point.x) * (line_end.y - line_start.y);

	if (distance > 0.0F)
	{
		return -1;
	}
	else if (distance == 0.0F)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

// 点是否在三角形中
inline bool	IsPointInTriangleLH2(const FmVec2& point, 
	const FmVec2& p1, const FmVec2& p2, const FmVec2& p3)
{
	int side1 = IsPointInLineLH(point, p1, p2);
	int side2 = IsPointInLineLH(point, p2, p3);
	int side3 = IsPointInLineLH(point, p3, p1);

	if ((side1 == -1) && (side2 == -1) && (side3 == -1))
	{
		return true;
	}

	return false;
}

// 文件是否存在
static bool file_exists(const char* name)
{
	Assert(name != NULL);

	DWORD res = GetFileAttributes(name);

	if (res == DWORD(-1))
	{
		return false;
	}

	return (res & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

// 获取指定浮点数的最小2的幂整数
static inline size_t get2n(float value)
{
	if (value <= 2.0f)
	{
		return 1;
	}

	int n = 1;

	while ((value / 2.0f) > 1.0f)
	{
		n++;

		value = value / 2.0f;
	}

	return size_t(pow(2.0, n));
}

const int MIN_LIGHT_MAP_SIZE = 16;

bool CLightGeometry::ProcessLightMaps(size_t index)
{
	Assert(index < m_Objects.size());
	
	// 模型位置矩阵
	IModelPlayer* pModelPlayer = m_Objects[index]->pModel->GetModelPlayer();

	if (NULL == pModelPlayer)
	{
		return false;
	}

	// 获得模型文件名
	CVar model_name;

	m_pRender->GetCore()->GetProperty(m_Objects[index]->pModel, 
		"ModelFile", model_name);

	const char* model_file = model_name.StringVal();

	// 模型所在的目录
	char model_path[256];

	SafeSprintf(model_path, sizeof(model_path), "%s%s",
		m_pRender->GetPathPrefix(model_file), model_file);

	char* slash = strrchr(model_path, '\\');

	if (slash)
	{
		*(slash + 1) = 0;
	}
	else
	{
		slash = strrchr(model_path, '/');

		if (slash)
		{
			*(slash + 1) = 0;
		}
	}

	size_t material_count = pModelPlayer->GetMaterialCount();

	for (size_t m = 0; m < material_count; ++m)
	{
		//if (pModelPlayer->GetMaterialTextureCount(m) < 2)
		if (!pModelPlayer->GetMaterialLightmapEnable(m))
		{
			// 不是具备光贴图UV坐标的模型
			continue;
		}

		size_t light_map_num = m_LightMaps.size();

		m_LightMaps.push_back(CORE_NEW(light_map_t));
		m_LightMaps[light_map_num]->strName = m_Objects[index]->strName;

		char light_name[256];

		SafeSprintf(light_name, sizeof(light_name), "%s%s_%d",
			m_strModelPath.c_str(), m_Objects[index]->strName.c_str(), m);

		m_LightMaps[light_map_num]->strLightName = light_name;
		m_LightMaps[light_map_num]->nObjectIndex = index;
		m_LightMaps[light_map_num]->nBufSize = 0;
		m_LightMaps[light_map_num]->nBufStart = 0;
		m_LightMaps[light_map_num]->nMaterialIndex = m;
		m_LightMaps[light_map_num]->fFaceAreaSum = 0.0f;
		m_LightMaps[light_map_num]->nFillOffset = m_Objects[index]->nLightMapFill;
		m_LightMaps[light_map_num]->nPixelWidth = m_Objects[index]->nLightMapSize;
		m_LightMaps[light_map_num]->nPixelHeight = m_Objects[index]->nLightMapSize;
		m_LightMaps[light_map_num]->pPixels = NULL;
		m_LightMaps[light_map_num]->pAOMapData = NULL;
		
		// 判断是否有环境遮挡图
		char ao_map_name[256];

		SafeSprintf(ao_map_name, sizeof(ao_map_name), "%s%s",
			model_path, pModelPlayer->GetMaterialLightmapName(m));

		strlwr(ao_map_name);

		if ((strstr(ao_map_name, ".bmp") != NULL) && file_exists(ao_map_name))
		{
			IImageAccess* pImage = m_pRender->CreateImageAccess();

			if (pImage->LoadFile(ao_map_name)
				&& (pImage->GetBitPerPixel() == 24)
				&& (pImage->GetWidth() == pImage->GetHeight()))
			{
				m_LightMaps[light_map_num]->pAOMapData = pImage;
			}
			else
			{
				pImage->Release();
			}
		}
	}

	// 计算光贴图的面积
	size_t i;

	for (i = 0; i < m_LightMaps.size(); ++i)
	{
		if (m_LightMaps[i]->pAOMapData)
		{
			// 由环境遮挡图决定光贴图大小，不需要计算面积
			continue;
		}
		
		CalcLightMapArea(m_LightMaps[i]);
	}

	// 决定每个光贴图的尺寸
	for (i = 0; i < m_LightMaps.size(); ++i)
	{
		int light_map_size = m_Objects[index]->nLightMapSize;
		int light_map_fill = m_Objects[index]->nLightMapFill;

		if (m_LightMaps[i]->pAOMapData)
		{
			// 由环境遮挡图决定光贴图大小
			light_map_size = m_LightMaps[i]->pAOMapData->GetWidth();
		}
		else if (m_bLightMapDegrade)
		{
			int pixel_width = get2n(sqrt(m_LightMaps[i]->fFaceAreaSum)) 
				* m_nLightPerUnit;

			if (pixel_width < MIN_LIGHT_MAP_SIZE)
			{
				pixel_width = MIN_LIGHT_MAP_SIZE;
			}
			else if (pixel_width > light_map_size)
			{
				pixel_width = light_map_size;
			}

			light_map_size = pixel_width;

			if (light_map_size <= 128)
			{
				light_map_fill = 1;
			}
			else if (light_map_size <= 256)
			{
				light_map_fill = 2;
			}
		}

		m_LightMaps[i]->nPixelWidth = light_map_size;
		m_LightMaps[i]->nPixelHeight = light_map_size;
		m_LightMaps[i]->nFillOffset = light_map_fill;

		size_t tex_width = m_LightMaps[i]->nPixelWidth;
		size_t tex_height = m_LightMaps[i]->nPixelHeight;
		size_t pixel_count = tex_width * tex_height;
		light_map_pixel_t* pixels = (light_map_pixel_t*)CORE_ALLOC(
			sizeof(light_map_pixel_t) * pixel_count);

		memset(pixels, 0, sizeof(light_map_pixel_t) * pixel_count);
		m_LightMaps[i]->pPixels = pixels;
	}

	// 定位光贴图像素的空间位置和法线
	for (i = 0; i < m_LightMaps.size(); ++i)
	{
		LocateLightMapPixels(m_LightMaps[i]);
	}

	for (i = 0; i < m_LightMaps.size(); ++i)
	{
		SAFE_RELEASE(m_LightMaps[i]->pAOMapData);
	}

	return true;
}

// 计算三角形面积
inline float CalcTriangleArea(const FmVec4& v1, const FmVec4& v2,
	const FmVec4& v3)
{
	FmVec4 v;
	
	FmVec3Cross(&v, &(v2 - v1), &(v3 - v1));

	return D3DXVec3Length(&v) * 0.5F;
}

// 计算物体的平均半径
inline float CalcAverageRadius(IVisual* pVisual)
{
	FmVec4 scale = pVisual->GetScale();

	return (scale.x + scale.y + scale.z) / 3.0F;
}

bool CLightGeometry::CalcLightMapArea(light_map_t* pLightMap)
{
	Assert(pLightMap != NULL);
	
	size_t obj_index = pLightMap->nObjectIndex;
	size_t mat_index = pLightMap->nMaterialIndex;

	// 模型位置矩阵
	FmMat4 mat;

	m_Objects[obj_index]->pModel->GetWorldMatrix(mat);

	IModelPlayer* pModelPlayer = m_Objects[obj_index]->pModel->GetModelPlayer();

	if (NULL == pModelPlayer)
	{
		return false;
	}

	float sum = 0.0f;

	size_t geo_vb_count = pModelPlayer->GetVertexBufferCount();
	size_t geo_obj_count = pModelPlayer->GetObjectCount();

	for (size_t obj = 0; obj < geo_obj_count; ++obj)
	{
		if (pModelPlayer->GetObjectMaterial(obj) != mat_index)
		{
			continue;
		}

		// 找到顶点缓冲
		size_t vb;

		for (vb = 0; vb < geo_vb_count; ++vb)
		{
			if (pModelPlayer->GetObjectVerterBufferIndex(obj) == vb)
			{
				break;
			}
		}

		if (vb >= geo_vb_count)
		{
			continue;
		}

		//unsigned char* vtx = pModelPlayer->GetVertexData(vb);
		dx_vertex_data_t* vtx = pModelPlayer->GetVertexData(vb);
		//size_t stride = pModelPlayer->GetVertexDataStride(vb);
		size_t start = pModelPlayer->GetObjectStartVertex(obj);
		unsigned short* obj_triangles = pModelPlayer->GetObjectIndexData(obj);
		size_t obj_triangle_num = pModelPlayer->GetObjectTriangleCount(obj);
		size_t obj_vertex_num = pModelPlayer->GetObjectVertexCount(obj);

		for (size_t t = 0; t < obj_triangle_num; t++)
		{
			size_t i1 = obj_triangles[t * 3 + 0];
			size_t i2 = obj_triangles[t * 3 + 1];
			size_t i3 = obj_triangles[t * 3 + 2];

			i1 += start; 
			i2 += start; 
			i3 += start;

			//unsigned char* v1 = vtx + stride * i1;
			//unsigned char* v2 = vtx + stride * i2;
			//unsigned char* v3 = vtx + stride * i3;
			unsigned char* v1 = get_vertex_by_index(vtx, i1);
			unsigned char* v2 = get_vertex_by_index(vtx, i2);
			unsigned char* v3 = get_vertex_by_index(vtx, i3);
			FmVec4 p1;
			FmVec4 p2;
			FmVec4 p3;

			D3DXVec3TransformCoord(&p1, (FmVec4*)v1, &mat);
			D3DXVec3TransformCoord(&p2, (FmVec4*)v2, &mat);
			D3DXVec3TransformCoord(&p3, (FmVec4*)v3, &mat);
			sum += CalcTriangleArea(p1, p2, p3);
		}
	}

	pLightMap->fFaceAreaSum = sum * CalcAverageRadius(
		m_Objects[obj_index]->pModel);

	return true;
}

// 测试点与三角形的交点
bool IntersectUV(const FmVec2& uv, const FmVec2& uv1,
	const FmVec2& uv2, const FmVec2& uv3, float* u, float* v)
{
	if (!IsPointInTriangleLH2(uv, uv3, uv2, uv1))
	{
		// 不在三角形内
		return false;
	}

	FmVec4 uv1Temp = FmVec4(uv1.x, uv1.y, 0.0F);
	FmVec4 uv2Temp = FmVec4(uv2.x, uv2.y, 0.0F);
	FmVec4 uv3Temp = FmVec4(uv3.x, uv3.y, 0.0F);

	FmVec4 triRayStart = FmVec4(uv.x, uv.y, -1.0F);
	FmVec4 triRayDirection = FmVec4(0.0F, 0.0F, 1.0F);

	float dist;

	if (!D3DXIntersectTri(&uv1Temp, &uv2Temp, &uv3Temp, &triRayStart, 
		&triRayDirection, u, v, &dist))
	{
		return false;
	}

	return true;
}

bool CLightGeometry::LocateLightMapPixels(light_map_t* pLightMap)
{
	Assert(pLightMap != NULL);

	unsigned char* ao_data = NULL;

	if (pLightMap->pAOMapData)
	{
		ao_data = pLightMap->pAOMapData->GetData();
	}

	pLightMap->nBufStart = m_Vertices.size();
	pLightMap->nBufSize = 0;

	int vertex_count = 0;
	
	int width = pLightMap->nPixelWidth;
	int height = pLightMap->nPixelHeight;
	int pixel_count = width * height;
	
	light_map_pixel_t* pixels = pLightMap->pPixels;

	for (int k = 0; k < pixel_count; ++k)
	{
		// 清除标志
		pixels[k].nVertexIndex = -1;
	}

	size_t obj_index = pLightMap->nObjectIndex;
	size_t mat_index = pLightMap->nMaterialIndex;

	// 模型位置矩阵
	IModelPlayer* pModelPlayer = m_Objects[obj_index]->pModel->GetModelPlayer();

	if (NULL == pModelPlayer)
	{
		return false;
	}

	size_t geo_vb_count = pModelPlayer->GetVertexBufferCount();
	size_t geo_obj_count = pModelPlayer->GetObjectCount();

	for (size_t obj = 0; obj < geo_obj_count; ++obj)
	{
		if (pModelPlayer->GetObjectMaterial(obj) != mat_index)
		{
			continue;
		}

		// 找到顶点缓冲
		size_t vb;

		for (vb = 0; vb < geo_vb_count; ++vb)
		{
			if (pModelPlayer->GetObjectVerterBufferIndex(obj) == vb)
			{
				break;
			}
		}

		if (vb >= geo_vb_count)
		{
			continue;
		}

		// 当前节点的世界矩阵
		FmMat4 mtxWorld;

		pModelPlayer->GetObjectMatrix(obj, mtxWorld);

		//unsigned char* vtx = pModelPlayer->GetVertexData(vb);
		//size_t stride = pModelPlayer->GetVertexDataStride(vb);
		dx_vertex_data_t* vtx = pModelPlayer->GetVertexData(vb);
		int position_offset = pModelPlayer->GetVertexPositionOffset(vb);
		int normal_offset = pModelPlayer->GetVertexNormalOffset(vb);
		// 光贴图的UV坐标数据偏移
		int uv_offset = pModelPlayer->GetVertexLightmapUVOffset(vb);
		size_t start = pModelPlayer->GetObjectStartVertex(obj);
		unsigned short* obj_triangles = pModelPlayer->GetObjectIndexData(obj);
		size_t obj_triangle_num = pModelPlayer->GetObjectTriangleCount(obj);
		size_t obj_vertex_num = pModelPlayer->GetObjectVertexCount(obj);

		for (size_t t = 0; t < obj_triangle_num; t++)
		{
			size_t i1 = obj_triangles[t * 3 + 0];
			size_t i2 = obj_triangles[t * 3 + 1];
			size_t i3 = obj_triangles[t * 3 + 2];

			i1 += start; 
			i2 += start; 
			i3 += start;

			//unsigned char* v1 = vtx + stride * i1;
			//unsigned char* v2 = vtx + stride * i2;
			//unsigned char* v3 = vtx + stride * i3;
			unsigned char* v1 = get_vertex_by_index(vtx, i1);
			unsigned char* v2 = get_vertex_by_index(vtx, i2);
			unsigned char* v3 = get_vertex_by_index(vtx, i3);
			FmVec4 p1 = *(FmVec4*)(v1 + position_offset);
			FmVec4 p2 = *(FmVec4*)(v2 + position_offset);
			FmVec4 p3 = *(FmVec4*)(v3 + position_offset);
			FmVec4 n1 = *(FmVec4*)(v1 + normal_offset);
			FmVec4 n2 = *(FmVec4*)(v2 + normal_offset);
			FmVec4 n3 = *(FmVec4*)(v3 + normal_offset);
			FmVec2 uv1 = *(FmVec2*)(v1 + uv_offset);
			FmVec2 uv2 = *(FmVec2*)(v2 + uv_offset);
			FmVec2 uv3 = *(FmVec2*)(v3 + uv_offset);

			// 获得三角形的UV范围
			FmVec2 uv_min = uv1;
			FmVec2 uv_max = uv1;

			if (uv2.x < uv_min.x)
			{
				uv_min.x = uv2.x;
			}
			
			if (uv2.x > uv_max.x)
			{
				uv_max.x = uv2.x;
			}

			if (uv2.y < uv_min.y)
			{
				uv_min.y = uv2.y;
			}

			if (uv2.y > uv_max.y)
			{
				uv_max.y = uv2.y;
			}

			if (uv3.x < uv_min.x)
			{
				uv_min.x = uv3.x;
			}

			if (uv3.x > uv_max.x)
			{
				uv_max.x = uv3.x;
			}

			if (uv3.y < uv_min.y)
			{
				uv_min.y = uv3.y;
			}

			if (uv3.y > uv_max.y)
			{
				uv_max.y = uv3.y;
			}

			int row_min = int(uv_min.y * pLightMap->nPixelHeight) - 2;
			int row_max = int(uv_max.y * pLightMap->nPixelHeight) + 2;
			int col_min = int(uv_min.x * pLightMap->nPixelWidth) - 2;
			int col_max = int(uv_max.x * pLightMap->nPixelWidth) + 2;

			if (row_min < 0)
			{
				row_min = 0;
			}

			if (row_max > height)
			{
				row_max = height;
			}

			if (col_min < 0)
			{
				col_min = 0;
			}

			if (col_max > width)
			{
				col_max = width;
			}

			for (int r = row_min; r < row_max; ++r)
			{
				for (int c = col_min; c < col_max; ++c)
				{
					int index = r * width + c;

					if (pixels[index].nVertexIndex >= 0)
					{
						// 已经处理过
						continue;
					}

					FmVec2 uv;

					// 像素中心点
					uv.x = float(c + 0.5F) / float(width);
					uv.y = float(r + 0.5F) / float(height);

					float u, v;

					if (!IntersectUV(uv, uv1, uv2, uv3, &u, &v))
					{
						//continue;
						// 像素左上角
						uv.x = float(c) / float(width);
						uv.y = float(r) / float(height);

						if (!IntersectUV(uv, uv1, uv2, uv3, &u, &v))
						{
							// 像素右上角
							uv.x = float(c + 1) / float(width);
							uv.y = float(r) / float(height);

							if (!IntersectUV(uv, uv1, uv2, uv3, &u, &v))
							{
								// 像素左下角
								uv.x = float(c) / float(width);
								uv.y = float(r + 1) / float(height);

								if (!IntersectUV(uv, uv1, uv2, uv3, &u, &v))
								{
									// 像素右下角
									uv.x = float(c + 1) / float(width);
									uv.y = float(r + 1) / float(height);

									if (!IntersectUV(uv, uv1, uv2, uv3, &u, &v))
									{
										continue;
									}
								}
							}
						}
					}

					// 计算碰撞点的空间位置和法线
					FmVec4 position;
					FmVec4 normal;
					
					float w = 1.0f - u - v;

					position.x = p1.x * w + p2.x * u + p3.x * v;
					position.y = p1.y * w + p2.y * u + p3.y * v;
					position.z = p1.z * w + p2.z * u + p3.z * v;

					normal.x = n1.x * w + n2.x * u + n3.x * v;
					normal.y = n1.y * w + n2.y * u + n3.y * v;
					normal.z = n1.z * w + n2.z * u + n3.z * v;

					D3DXVec3TransformCoord(&position, &position, &mtxWorld);
					D3DXVec3TransformNormal(&normal, &normal, &mtxWorld);
					FmVec3Normalize(&normal, &normal);

					// 添加用于处理的顶点
					size_t vertex_num = m_Vertices.size();

					vertex_t* pVertex = m_VertexBucket.New();

					pVertex->p = position;
					pVertex->n = normal;
					pVertex->c = FmVec4(0.0f, 0.0f, 0.0f);
					pVertex->bc = FmVec4(0.0f, 0.0f, 0.0f);
					pVertex->mc = FmVec4(1.0f, 1.0f, 1.0f);
					pVertex->ao = FmVec4(1.0f, 1.0f, 1.0f);
					pVertex->alpha = 0xFF000000;
					pVertex->flags = vertex_t::f_zero;
					pVertex->shadow = NULL;

					m_Vertices.push_back(pVertex);

					if (ao_data)
					{
						// 读取环境遮挡亮度信息
						float b = ao_data[index * 3] / 255.0f;
						float g = ao_data[index * 3 + 1] / 255.0f;
						float r = ao_data[index * 3 + 2] / 255.0f;
						
						pVertex->ao = FmVec4(r, g, b);
					}

					vertex_count++;

					pixels[index].nVertexIndex = vertex_num;
					
					// 添加用于光线追踪处理的面
					AddLightMapTriangle(vertex_num);
				}
			}
		}
	}

	pLightMap->nBufSize = vertex_count;

	return true;
}

bool CLightGeometry::CalcBoundBox(size_t start_index, FmVec4& v_min,
	FmVec4& v_max)
{
	size_t vertex_num = m_Vertices.size();
	
	for (size_t i = start_index; i < vertex_num; i++)
	{
		vertex_t* v = m_Vertices[i];
		
		if (v->p.x < v_min.x)
		{
			v_min.x = v->p.x;
		}
		
		if (v->p.y < v_min.y)
		{
			v_min.y = v->p.y;
		}
		
		if (v->p.z < v_min.z)
		{
			v_min.z = v->p.z;
		}
		
		if (v->p.x > v_max.x)
		{
			v_max.x = v->p.x;
		}
		
		if (v->p.y > v_max.y)
		{
			v_max.y = v->p.y;
		}
		
		if (v->p.z > v_max.z)
		{
			v_max.z = v->p.z;
		}
	}
	
	return true;
}

bool CLightGeometry::Process(int light_num)
{
	if (!m_Zones.empty())
	{
		if (!ProcessZones())
		{
			CORE_TRACE("(CLightGeometry::Process)process zone failed");
			return false;
		}
	}
	else
	{
		if (!ProcessObjects(0))
		{
			CORE_TRACE("(CLightGeometry::Process)process object failed");
			return false;
		}
	}
	
	size_t vertex_num = m_Vertices.size();
	
	if (0 == vertex_num)
	{
		CORE_TRACE("(CLightGeometry::Process)no any vertex");
		return false;
	}

//	m_pShadows = NEW shadow_t[vertex_num * light_num];
//	memset(m_pShadows, 0, vertex_num * light_num * sizeof(shadow_t));

	// 分块申请，防止一次申请的内存过大
	size_t shadow_block_num = (vertex_num + (SHADOW_BLOCK_SIZE - 1)) 
		/ SHADOW_BLOCK_SIZE;

	for (size_t k = 0; k < shadow_block_num; ++k)
	{
		shadow_t* pShadow = (shadow_t*)CORE_ALLOC(
			sizeof(shadow_t) * light_num * SHADOW_BLOCK_SIZE);

		memset(pShadow, 0, light_num * SHADOW_BLOCK_SIZE * sizeof(shadow_t));
		m_Shadows.push_back(pShadow);
	}

	m_nLightNum = light_num;
	m_vMin = m_Vertices[0]->p;
	m_vMax = m_Vertices[0]->p;
	
	int shadow_block = 0;
	int shadow_count = 0;
	
	for (size_t i = 0; i < vertex_num; i++)
	{
		vertex_t* v = m_Vertices[i];

		//v->shadow = m_pShadows + shadow_count;
		//shadow_count += light_num;
		v->shadow = m_Shadows[shadow_block] + shadow_count;
		shadow_count += light_num;

		if (shadow_count == (light_num * SHADOW_BLOCK_SIZE))
		{
			shadow_block++;
			shadow_count = 0;
		}
		
		if (v->p.x < m_vMin.x)
		{
			m_vMin.x = v->p.x;
		}

		if (v->p.y < m_vMin.y)
		{
			m_vMin.y = v->p.y;
		}

		if (v->p.z < m_vMin.z)
		{
			m_vMin.z = v->p.z;
		}

		if (v->p.x > m_vMax.x)
		{
			m_vMax.x = v->p.x;
		}

		if (v->p.y > m_vMax.y)
		{
			m_vMax.y = v->p.y;
		}

		if (v->p.z > m_vMax.z)
		{
			m_vMax.z = v->p.z;
		}

		if (v->flags & vertex_t::f_inv)
		{
			if ((v->flags & vertex_t::f_bug) == 0)
			{
				v->n.x = -v->n.x;
				v->n.y = -v->n.y;
				v->n.z = -v->n.z;
			}
		}
	}

	if (m_pTerrain != NULL)
	{
		// 使用区域的对角线长度
		float sx = m_pTerrain->GetZoneWidth() * m_pTerrain->GetUnitSize();
		float sz = m_pTerrain->GetZoneHeight() * m_pTerrain->GetUnitSize();

		m_fRadius = FmVec2Length(&FmVec2(sx, sz));
	}
	else
	{
		m_fRadius = D3DXVec3Length(&(m_vMax - m_vMin));
	}
	
	return true;
}

bool CLightGeometry::TraceHitted(const FmVec4& src, 
	const FmVec4& dst)
{
	if (m_pTerrain != NULL)
	{
		// 使用地形碰撞
		return m_pTerrain->TraceHitted(src, dst);
	}
	
	// 使用模型做碰撞
	size_t object_num = m_Objects.size();
	
	for (size_t i = 0; i < object_num; i++)
	{
		if (m_Objects[i]->pModel->TraceHitted(src, dst))
		{
			return true;
		}
	}
	
	return false;
}

bool CLightGeometry::TraceDetail(const FmVec4& src, 
	const FmVec4& dst, trace_info_t& result)
{
	if (m_pTerrain != NULL)
	{
		// 使用地形碰撞
		return m_pTerrain->TraceDetail(src, dst, result);
	}

	// 使用模型做碰撞
	size_t object_num = m_Objects.size();

	for (size_t i = 0; i < object_num; i++)
	{
		if (m_Objects[i]->pModel->TraceDetail(src, dst, result))
		{
			return true;
		}
	}

	return false;
}

bool CLightGeometry::UpdateModelVertexColor()
{
	if (0 == m_Vertices.size())
	{
		return false;
	}

	size_t object_num = m_Objects.size();

	for (size_t i = 0; i < object_num; i++)
	{	
		if (0 == m_Objects[i]->nBufSize)
		{
			continue;
		}

		if (!m_Objects[i]->bSaveLight)
		{
			continue;
		}

		if (m_Objects[i]->bLightMap)
		{
			continue;
		}

		size_t size = m_Objects[i]->nBufSize;
		unsigned int* colors = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * size);
		size_t pnt = m_Objects[i]->nBufStart;

		for (size_t j = 0; j < size; j++)
		{
			vertex_t* v = m_Vertices[pnt];
			FmVec4 c;

			c.x = v->c.x * v->mc.x * 255.0f;
			c.y = v->c.y * v->mc.y * 255.0f;
			c.z = v->c.z * v->mc.z * 255.0f;
			pnt++;

			if (c.x < 0.0f)
			{
				c.x = 0.0f;
			}

			if (c.x > 255.0f)
			{
				c.x = 255.0f;
			}

			if (c.y < 0.0f)
			{
				c.y = 0.0f;
			}

			if (c.y > 255.0f)
			{
				c.y = 255.0f;
			}

			if (c.z < 0.0f)
			{
				c.z = 0.0f;
			}

			if (c.z > 255.0f)
			{
				c.z = 255.0f;
			}

			colors[j] = (unsigned int(c.x) << 16) | (unsigned int(c.y) << 8) 
				| (unsigned int(c.z) << 0) | 0xFF000000;
		}

		m_Objects[i]->pModel->GetModelPlayer()->UpdateVertexColor(colors, size);
		CORE_FREE(colors, sizeof(unsigned int) * size);
	}

	return true;
}

bool CLightGeometry::UpdateModelLightMap()
{
	int light_map_num = m_LightMaps.size();

	for (int i = 0; i < light_map_num; ++i)
	{
		int rows = m_LightMaps[i]->nPixelHeight;
		int cols = m_LightMaps[i]->nPixelWidth;
		light_map_pixel_t* pixels = m_LightMaps[i]->pPixels;

		for (int k = 0; k < (rows * cols); ++k)
		{
			pixels[k].nFlags = 0;
		}

		unsigned int* colors = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * rows * cols);

		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				int index = r * cols + c;
				int vi = pixels[index].nVertexIndex;

				if (vi < 0)
				{
					// 填充不能用黑色
					colors[index] = 0xFF808080;	
					//colors[index] = 0xFF000000;	
					continue;
				}

				vertex_t* v = m_Vertices[vi];
				unsigned int value;

				if (m_bSaveOcclusion)
				{
					// 环境遮挡信息
					float ao = (v->ao.x + v->ao.y + v->ao.z) / 3.0f * 255.0f;
					// 主光源遮挡信息
					float shadow = float(v->shadow[1].sm) * 255.0f;

					if (ao < 0.0f)
					{
						ao = 0.0f;
					}

					if (ao > 255.0f)
					{
						ao = 255.0f;
					}

					if (shadow < 0.0f)
					{
						shadow = 0.0f;
					}

					if (shadow > 255.0f)
					{
						shadow = 255.0f;
					}

					value = (unsigned int(ao) << 16) 
						| (unsigned int(shadow) << 8) | 0xFF0000FF;
				}
				else
				{
					FmVec4 color;

					color.x = v->c.x * v->mc.x * 255.0f;
					color.y = v->c.y * v->mc.y * 255.0f;
					color.z = v->c.z * v->mc.z * 255.0f;

					if (color.x < 0.0f)
					{
						color.x = 0.0f;
					}

					if (color.x > 255.0f)
					{
						color.x = 255.0f;
					}

					if (color.y < 0.0f)
					{
						color.y = 0.0f;
					}

					if (color.y > 255.0f)
					{
						color.y = 255.0f;
					}

					if (color.z < 0.0f)
					{
						color.z = 0.0f;
					}

					if (color.z > 255.0f)
					{
						color.z = 255.0f;
					}

					value = (unsigned int(color.x) << 16) 
						| (unsigned int(color.y) << 8) 
						| (unsigned int(color.z) << 0) | 0xFF000000;
				}
				
				colors[index] = value;
				pixels[index].nFlags = PIXEL_RENDER;

				// 扩展像素范围
				const int offset_fill = m_LightMaps[i]->nFillOffset;

				for (int sr = (r - offset_fill); sr <= (r + offset_fill); ++sr)
				{
					for (int sc = (c - offset_fill); sc <= (c + offset_fill); ++sc)
					{
						if (sr < 0 || sr >= rows)
						{
							continue;
						}

						if (sc < 0 || sc >= cols)
						{
							continue;
						}

						if (pixels[sr * cols + sc].nFlags == PIXEL_RENDER)
						{
							continue;
						}

						colors[sr * cols + sc] = value;
						pixels[sr * cols + sc].nFlags = PIXEL_EXTEND;
					}
				}
			}
		}

		size_t obj = m_LightMaps[i]->nObjectIndex;

		m_Objects[obj]->pModel->GetModelPlayer()->UpdateLightMap(
			m_LightMaps[i]->nMaterialIndex, 
			m_LightMaps[i]->strLightName.c_str(), colors, cols, rows);
		CORE_FREE(colors, sizeof(unsigned int) * rows * cols);
	}

	return true;
}

void CLightGeometry::UpdateModelColors()
{
	if (m_bOnlyGround)
	{
		return;
	}

	//UpdateModelVertexColor();
	UpdateModelLightMap();
}

void CLightGeometry::UpdateZoneColors()
{
	if (NULL == m_pTerrain)
	{
		return;
	}

	size_t zone_num = m_Zones.size();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* pZone = m_pTerrain->GetZoneManager()->GetZoneByIndex(
			m_Zones[i]->nZoneIndex);

		if (NULL == pZone)
		{
			CORE_TRACE("(CLightGeometry::UpdateZoneColors)zone not exists");
			CORE_TRACE(m_Zones[i]->strName.c_str());
			continue;
		}
		
		size_t width = m_Zones[i]->nVertexCols;
		size_t height = m_Zones[i]->nVertexRows;
		unsigned int* buf = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * width * height);
		size_t start = m_Zones[i]->nBufStart;
		size_t help_rows = m_Zones[i]->nHelperRows;
		size_t help_cols = m_Zones[i]->nHelperCols;
		size_t rows = height + help_rows * 2;
		size_t cols = width + help_cols * 2;
		size_t count = 0;

		for (size_t h = 0; h < height; ++h)
		{
			for (size_t w = 0; w < width; ++w)
			{
				size_t pos = start + (h + help_rows) * cols + (w + help_cols);
				vertex_t* v = m_Vertices[pos];
				FmVec4 c;
				
				c.x = v->c.x * v->mc.x * 255.0f;
				c.y = v->c.y * v->mc.y * 255.0f;
				c.z = v->c.z * v->mc.z * 255.0f;

				if (c.x < 0.0f)
				{
					c.x = 0.0f;
				}
				
				if (c.x > 255.0f)
				{
					c.x = 255.0f;
				}
				
				if (c.y < 0.0f)
				{
					c.y = 0.0f;
				}
				
				if (c.y > 255.0f)
				{
					c.y = 255.0f;
				}
				
				if (c.z < 0.0f)
				{
					c.z = 0.0f;
				}
				
				if (c.z > 255.0f)
				{
					c.z = 255.0f;
				}
				
				float alpha = 255.0f;

				if (m_bSaveOcclusion)
				{
					// 保存遮挡信息
					float ao = (v->ao.x + v->ao.y + v->ao.z) / 3.0f;

					alpha = 255.0f * float(v->shadow[1].sm) * ao;

					if (alpha < 0.0f)
					{
						alpha = 0.0f;
					}

					if (alpha > 255.0f)
					{
						alpha = 255.0f;
					}
				}

				buf[count++] = (unsigned int(c.x) << 16) 
					| (unsigned int(c.y) << 8) | (unsigned int(c.z) << 0) 
					| (unsigned int(alpha) << 24);
			}
		}
	
		if (!pZone->GetLight()->BuildTexByColors(height, width, buf))
		{
			CORE_TRACE("(CLightGeometry::UpdateZoneColors)write tex failed");
			CORE_TRACE(m_Zones[i]->strName.c_str());
		}

		CORE_FREE(buf, sizeof(unsigned int) * width * height);
	}
}

bool CLightGeometry::SaveModelVertexColor()
{
	if (0 == m_Vertices.size())
	{
		return false;
	}
	
	bool result = true;
	size_t BUF_SIZE = 16384;
	unsigned int* buf = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * BUF_SIZE);
	size_t object_num = m_Objects.size();

	for (size_t i = 0; i < object_num; i++)
	{	
		if (0 == m_Objects[i]->nBufSize)
		{
			continue;
		}

		if (!m_Objects[i]->bSaveLight)
		{
			continue;
		}

		if (m_Objects[i]->bLightMap)
		{
			continue;
		}

		const char* light_name = m_Objects[i]->strLightName.c_str();
		char fname[512];
		
		SafeSprintf(fname, sizeof(fname), "%s%s.col", 
			m_pRender->GetPathPrefix(light_name), light_name); 
		
		FILE* fp = core_file::fopen(fname, "wb");

		if (NULL == fp)
		{
			result = false;
			continue;
		}

		size_t count = 0;
		size_t pnt = m_Objects[i]->nBufStart;
		size_t n = m_Objects[i]->nBufSize;

		for (size_t j = 0; j < n; j++)
		{
			vertex_t* v = m_Vertices[pnt];
			FmVec4 c;
			
			c.x = v->c.x * v->mc.x * 255.0f;
			c.y = v->c.y * v->mc.y * 255.0f;
			c.z = v->c.z * v->mc.z * 255.0f;
			pnt++;
			
			if (c.x < 0.0f)
			{
				c.x = 0.0f;
			}

			if (c.x > 255.0f)
			{
				c.x = 255.0f;
			}

			if (c.y < 0.0f)
			{
				c.y = 0.0f;
			}

			if (c.y > 255.0f)
			{
				c.y = 255.0f;
			}

			if (c.z < 0.0f)
			{
				c.z = 0.0f;
			}

			if (c.z > 255.0f)
			{
				c.z = 255.0f;
			}

			buf[count++] = (unsigned int(c.x) << 16) | (unsigned int(c.y) << 8) 
				| (unsigned int(c.z) << 0) | 0xFF000000;
			
			if (count >= BUF_SIZE)
			{
				result &= core_file::fwrite(buf, count * sizeof(unsigned int), 
					1, fp) == 1;
				count = 0;
			}
		}

		if (count > 0)
		{
			result &= core_file::fwrite(buf, count * sizeof(unsigned int), 
				1, fp) == 1;
		}

		core_file::fclose(fp);
	}

	CORE_FREE(buf, sizeof(unsigned int) * BUF_SIZE);

	return result;
}

bool CLightGeometry::SaveModelLightMap()
{
	int light_map_num = m_LightMaps.size();

	for (int i = 0; i < light_map_num; ++i)
	{
		int rows = m_LightMaps[i]->nPixelHeight;
		int cols = m_LightMaps[i]->nPixelWidth;
		light_map_pixel_t* pixels = m_LightMaps[i]->pPixels;

		for (int k = 0; k < (rows * cols); ++k)
		{
			pixels[k].nFlags = 0;
		}

		unsigned int* colors = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * rows * cols);

		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				int index = r * cols + c;
				int vi = pixels[index].nVertexIndex;
				
				if (vi < 0)
				{
					colors[index] = 0xFF808080;	
					continue;
				}

				vertex_t* v = m_Vertices[vi];
				unsigned int value;

				if (m_bSaveOcclusion)
				{
					// 环境遮挡信息
					float ao = (v->ao.x + v->ao.y + v->ao.z) / 3.0f * 255.0f;
					// 主光源遮挡信息
					float shadow = float(v->shadow[1].sm) * 255.0f;

					if (ao < 0.0f)
					{
						ao = 0.0f;
					}

					if (ao > 255.0f)
					{
						ao = 255.0f;
					}

					if (shadow < 0.0f)
					{
						shadow = 0.0f;
					}

					if (shadow > 255.0f)
					{
						shadow = 255.0f;
					}
					
					value = (unsigned int(ao) << 16) 
						| (unsigned int(shadow) << 8) | 0xFF0000FF;
				}
				else
				{
					FmVec4 color;

					color.x = v->c.x * v->mc.x * 255.0f;
					color.y = v->c.y * v->mc.y * 255.0f;
					color.z = v->c.z * v->mc.z * 255.0f;

					if (color.x < 0.0f)
					{
						color.x = 0.0f;
					}

					if (color.x > 255.0f)
					{
						color.x = 255.0f;
					}

					if (color.y < 0.0f)
					{
						color.y = 0.0f;
					}

					if (color.y > 255.0f)
					{
						color.y = 255.0f;
					}

					if (color.z < 0.0f)
					{
						color.z = 0.0f;
					}

					if (color.z > 255.0f)
					{
						color.z = 255.0f;
					}

					value = (unsigned int(color.x) << 16) 
						| (unsigned int(color.y) << 8) 
						| (unsigned int(color.z) << 0) | 0xFF000000;
				}

				colors[index] = value;
				pixels[index].nFlags = PIXEL_RENDER;
				
				// 扩展像素范围
				const int offset_fill = m_LightMaps[i]->nFillOffset;
				
				for (int sr = (r - offset_fill); sr <= (r + offset_fill); ++sr)
				{
					for (int sc = (c - offset_fill); sc <= (c + offset_fill); ++sc)
					{
						if (sr < 0 || sr >= rows)
						{
							continue;
						}

						if (sc < 0 || sc >= cols)
						{
							continue;
						}

						if (pixels[sr * cols + sc].nFlags == PIXEL_RENDER)
						{
							continue;
						}

						colors[sr * cols + sc] = value;
						pixels[sr * cols + sc].nFlags = PIXEL_EXTEND;
					}
				}
			}
		}

		IStaticTex* pTexture = m_pRender->CreateStaticTex(cols, rows, 1,
			D3DFMT_X8R8G8B8);

		if (NULL == pTexture)
		{
			CORE_FREE(colors, sizeof(unsigned int) * rows * cols);
			return false;
		}

		D3DLOCKED_RECT lr;

		pTexture->LockRect(0, &lr, 0);

		// 图片的数据是从上到下的
		BYTE* pDstRow = (BYTE*)lr.pBits;

		for (int r = 0; r < rows; ++r)
		{
			DWORD* pDst32 = (DWORD*)pDstRow;

			for (int c = 0; c < cols; ++c)
			{
				*pDst32++ = colors[r * cols + c];
			}

			pDstRow += lr.Pitch;
		}

		pTexture->UnlockRect(0);

		// 转换文件格式为DXT1
		ID3DXBuffer* pTempBuffer = NULL;
		HRESULT hr = D3DXSaveTextureToFileInMemory(&pTempBuffer, D3DXIFF_DDS, 
			pTexture->GetDxTexture(), NULL);

		DxTexture* pTexSave = NULL;

		hr = D3DXCreateTextureFromFileInMemoryEx(m_pRender->GetDevice(), 
			pTempBuffer->GetBufferPointer(), pTempBuffer->GetBufferSize(),
			cols, rows, 0, 0, D3DFMT_DXT1, D3DPOOL_MANAGED, 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexSave);
		
		const char* light_name = m_LightMaps[i]->strLightName.c_str();
		char fname[512];

		SafeSprintf(fname, sizeof(fname), "%s%s.dds", 
			m_pRender->GetPathPrefix(light_name), light_name); 

		hr = D3DXSaveTextureToFile(fname, D3DXIFF_DDS, pTexSave, NULL);

		pTempBuffer->Release();
		pTexSave->Release();
		pTexture->Release();

		CORE_FREE(colors, sizeof(unsigned int) * rows * cols);
	}
	
	return true;
}

bool CLightGeometry::SaveModelLight()
{
	if (m_bOnlyGround)
	{
		return false;
	}

	//SaveModelVertexColor();
	SaveModelLightMap();

	return true;
}

bool CLightGeometry::SaveZoneLight()
{
	if (0 == m_Vertices.size())
	{
		return false;
	}
	
	bool result = true;
	size_t zone_num = m_Zones.size();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* pZone = m_pTerrain->GetZoneManager()->GetZoneByIndex(
			m_Zones[i]->nZoneIndex);

		if (NULL == pZone)
		{
			CORE_TRACE("(CLightGeometry::SaveZoneLight)zone not exists");
			CORE_TRACE(m_Zones[i]->strName.c_str());
			continue;
		}

		const char* light_name = m_Zones[i]->strLightName.c_str();
		char fname[512];

		SafeSprintf(fname, sizeof(fname), "%s%s.bmp", 
			m_pRender->GetPathPrefix(light_name), light_name); 

		size_t width = m_Zones[i]->nVertexCols;
		size_t height = m_Zones[i]->nVertexRows;
		unsigned int* buf = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * width * height);
		size_t start = m_Zones[i]->nBufStart;
		size_t help_rows = m_Zones[i]->nHelperRows;
		size_t help_cols = m_Zones[i]->nHelperCols;
		size_t rows = height + help_rows * 2;
		size_t cols = width + help_cols * 2;
		size_t count = 0;

		for (size_t h = 0; h < height; ++h)
		{
			for (size_t w = 0; w < width; ++w)
			{
				size_t pos = start + (h + help_rows) * cols + (w + help_cols);
				vertex_t* v = m_Vertices[pos];
				FmVec4 c;

				c.x = v->c.x * v->mc.x * 255.0f;
				c.y = v->c.y * v->mc.y * 255.0f;
				c.z = v->c.z * v->mc.z * 255.0f;

				if (c.x < 0.0f)
				{
					c.x = 0.0f;
				}

				if (c.x > 255.0f)
				{
					c.x = 255.0f;
				}

				if (c.y < 0.0f)
				{
					c.y = 0.0f;
				}

				if (c.y > 255.0f)
				{
					c.y = 255.0f;
				}

				if (c.z < 0.0f)
				{
					c.z = 0.0f;
				}

				if (c.z > 255.0f)
				{
					c.z = 255.0f;
				}

				float alpha = 255.0f;

				if (m_bSaveOcclusion)
				{
					// 保存遮挡信息
					float ao = (v->ao.x + v->ao.y + v->ao.z) / 3.0f;

					alpha = 255.0f * float(v->shadow[1].sm) * ao;

					if (alpha < 0.0f)
					{
						alpha = 0.0f;
					}

					if (alpha > 255.0f)
					{
						alpha = 255.0f;
					}
				}

				buf[count++] = (unsigned int(c.x) << 16) 
					| (unsigned int(c.y) << 8) | (unsigned int(c.z) << 0) 
					| (unsigned int(alpha) << 24);
			}
		}

		// 保存原始的光贴图（未合并混合颜色）
		IImageAccess* pImage = m_pRender->CreateImageAccess();

		pImage->SetFormat(width, height, 32);
		pImage->SetData(buf, width * height * sizeof(unsigned int));

		if (!pImage->SaveFile(fname))
		{
			CORE_TRACE("(CLightGeometry::SaveZoneLight)save bmp failed");
			CORE_TRACE(m_Zones[i]->strName.c_str());
		}

		pImage->Release();
		CORE_FREE(buf, sizeof(unsigned int) * width * height);

		// 保存合并混合颜色的光贴图
		SafeSprintf(fname, sizeof(fname), "%s%s.dds", 
			m_pRender->GetPathPrefix(light_name), light_name); 

		if (!pZone->GetLight()->GetLightTex()->SaveDDS(m_pRender, fname, 
			m_bSaveOcclusion))
		{
			CORE_TRACE("(CLightGeometry::SaveZoneLight)save dds failed");
			CORE_TRACE(m_Zones[i]->strName.c_str());
		}
	}
	
	return result;
}
*/
