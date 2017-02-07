//--------------------------------------------------------------------
// 文件名:		terrain_culling.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_culling.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "terrain_chunk.h"
#include "terrain_visuals.h"
#include "terrain_water.h"
#include "sea_water.h"
#include "zone_manager.h"
#include "quad_visual.h"
#include "quad_ground.h"
#include "quad_node.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/var.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/core_file.h"
#include "terrain_cull_sort.h"

// CTerrainCulling

CTerrainCulling::CTerrainCulling()
{
	m_pTerrain = NULL;
	m_pRender = NULL;
	m_nRenderZoneNum = 0;
	m_pRenderZones = NULL;
	m_pRenderChunks = NULL;
	m_nRenderChunkNum = 0;
	m_pRenderVisuals = NULL;
	m_nRenderVisualNum = 0;
	m_pRenderRoles = NULL;
	m_nRenderRoleNum = 0;
	m_pUpdateVisuals = NULL;
	m_nUpdateVisualNum = 0;
	m_nInFogChunkNum = 0;
	m_pShadowMapCollect = NULL;
	memset(m_NearChunkInfo, 0, sizeof(m_NearChunkInfo));
	memset(m_NearGridInfo, 0, sizeof(m_NearGridInfo));
	m_nHorizonBucketCount = 0;
	m_pHorizonBuckets = (horizon_bucket_t*)CORE_ALLOC(
		sizeof(horizon_bucket_t) * (HORIZON_BUCKET_SIZE + 1));
	memset(m_pHorizonBuckets, 0, 
		sizeof(horizon_bucket_t) * (HORIZON_BUCKET_SIZE + 1));
}

CTerrainCulling::~CTerrainCulling()
{
	ReleaseAll();
	CORE_FREE(m_pHorizonBuckets, 
		sizeof(horizon_bucket_t) * (HORIZON_BUCKET_SIZE + 1));
}

void CTerrainCulling::ReleaseAll()
{
	if (m_pRenderChunks)
	{
		CORE_FREE(m_pRenderChunks, sizeof(CTerrainChunk*) * MAX_RENDER_CHUNKS);
		m_pRenderChunks = NULL;
		m_nRenderChunkNum = 0;
	}

	if (m_pRenderZones)
	{
		for (size_t i = 0; i < MAX_RENDER_ZONES; ++i)
		{
			if (m_pRenderZones[i].pChunks)
			{
				CORE_FREE(m_pRenderZones[i].pChunks, 
					sizeof(CTerrainChunk*) * m_pRenderZones[i].nZoneChunkNum);
			}
		}

		CORE_FREE(m_pRenderZones, sizeof(render_zone_t) * MAX_RENDER_ZONES);
		m_pRenderZones = NULL;
		m_nRenderZoneNum = 0;
	}

	if (m_pRenderVisuals)
	{
		CORE_FREE(m_pRenderVisuals, 
			sizeof(render_visual_t) * MAX_RENDER_VISUALS);
		m_pRenderVisuals = NULL;
		m_nRenderVisualNum = 0;
	}
	
	if (m_pRenderRoles)
	{
		CORE_FREE(m_pRenderRoles, sizeof(render_role_t) * MAX_RENDER_ROLES);
		m_pRenderRoles = NULL;
		m_nRenderRoleNum = 0;
	}

	if (m_pUpdateVisuals)
	{
		CORE_FREE(m_pUpdateVisuals, sizeof(PERSISTID) * MAX_UPDATE_VISUALS);
		m_pUpdateVisuals = NULL;
		m_nUpdateVisualNum = 0;
	}

	if (!m_EnvMapCollects.empty())
	{
		for (size_t k = 0; k < m_EnvMapCollects.size(); ++k)
		{
			if (m_EnvMapCollects[k])
			{
				CORE_DELETE(m_EnvMapCollects[k]);
			}
		}

		m_EnvMapCollects.clear();
	}

	if (!m_ShadowMapCollects.empty())
	{
		for (size_t k = 0; k < m_ShadowMapCollects.size(); ++k)
		{
			if (m_ShadowMapCollects[k])
			{
				CORE_DELETE(m_ShadowMapCollects[k]);
			}
		}

		m_ShadowMapCollects.clear();
	}

	if (m_pShadowMapCollect)
	{
		CORE_DELETE(m_pShadowMapCollect);
		m_pShadowMapCollect = NULL;
	}
	
	m_nInFogChunkNum = 0;
}

static int __cdecl near_compare(const void* elem1, const void* elem2)
{
	if (((near_info_t*)elem1)->fDistance > 
		((near_info_t*)elem2)->fDistance)
	{
		return 1;
	}

	if (((near_info_t*)elem1)->fDistance < 
		((near_info_t*)elem2)->fDistance)
	{
		return -1;
	}

	return 0;
}

bool CTerrainCulling::Initialize(IRender* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	
	m_pRenderChunks = (CTerrainChunk**)CORE_ALLOC(
		sizeof(CTerrainChunk*) * MAX_RENDER_CHUNKS);
	m_nRenderChunkNum = 0;
	
	m_nRenderZoneNum = 0;
	m_pRenderZones = (render_zone_t*)CORE_ALLOC(
		sizeof(render_zone_t) * MAX_RENDER_ZONES);
	memset(m_pRenderZones, 0, sizeof(render_zone_t) * MAX_RENDER_ZONES);

	m_pRenderVisuals = (render_visual_t*)CORE_ALLOC(
		sizeof(render_visual_t) * MAX_RENDER_VISUALS);
	m_nRenderVisualNum = 0;

	m_pRenderRoles = (render_role_t*)CORE_ALLOC(
		sizeof(render_role_t) * MAX_RENDER_ROLES);
	m_nRenderRoleNum = 0;

	m_pUpdateVisuals = (PERSISTID*)CORE_ALLOC(
		sizeof(PERSISTID) * MAX_UPDATE_VISUALS);
	m_nUpdateVisualNum = 0;

	float unit_size = m_pTerrain->GetUnitSize();
	float chunk_size = m_pTerrain->GetChunkScale() * unit_size;
	int center_chunk = NEAR_CHUNK_DIMS / 2 + 1;

	for (int r = 0; r < NEAR_CHUNK_DIMS; ++r)
	{
		for (int c = 0; c < NEAR_CHUNK_DIMS; ++c)
		{
			near_info_t* p = m_NearChunkInfo + r * NEAR_CHUNK_DIMS + c;

			p->nRowOffset = r - center_chunk;
			p->nColOffset = c - center_chunk;

			float sx = p->nColOffset * chunk_size;
			float sz = p->nRowOffset * chunk_size;

			FmVec2 vec2Temp(sx, sz);
			p->fDistance = FmVec2Length(&vec2Temp);
		}
	}

	qsort(m_NearChunkInfo, NEAR_CHUNK_DIMS * NEAR_CHUNK_DIMS, 
		sizeof(near_info_t), near_compare);

	float grid_size = m_pTerrain->GetCollideScale() * unit_size;
	int center_grid = NEAR_GRID_DIMS / 2 + 1;

	for (int r1 = 0; r1 < NEAR_GRID_DIMS; ++r1)
	{
		for (int c1 = 0; c1 < NEAR_GRID_DIMS; ++c1)
		{
			near_info_t* p = m_NearGridInfo + r1 * NEAR_GRID_DIMS + c1;

			p->nRowOffset = r1 - center_grid;
			p->nColOffset = c1 - center_grid;

			float sx = p->nColOffset * grid_size;
			float sz = p->nRowOffset * grid_size;
			FmVec2 vec2Temp(sx, sz);
			p->fDistance = FmVec2Length(&vec2Temp);
		}
	}

	qsort(m_NearGridInfo, NEAR_GRID_DIMS * NEAR_GRID_DIMS, 
		sizeof(near_info_t), near_compare);

	return true;
}

//bool CTerrainCulling::Shut()
//{
//	ReleaseAll();
//	
//	return true;
//}

void CTerrainCulling::Update(float seconds)
{
	RefreshVisuals();

	bool visual_fade_in = m_pTerrain->GetVisualFadeIn();
	ICore* pCore = m_pRender->GetCore();

	size_t update_visual_num = m_nUpdateVisualNum;
	PERSISTID* update_visuals = m_pUpdateVisuals;

	if (visual_fade_in)
	{
		for (size_t i = 0; i < update_visual_num; ++i)
		{
			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				update_visuals[i]);

			if (NULL == pVisBase)
			{
				continue;
			}

			pVisBase->Update(seconds);

			float alpha = pVisBase->GetAlphaValue();

			if (alpha < 1.0F)
			{
				pVisBase->SetAlphaValue(alpha + seconds);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < update_visual_num; ++i)
		{
			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				update_visuals[i]);

			if (NULL == pVisBase)
			{
				continue;
			}

			pVisBase->Update(seconds);
		}
	}
}

static inline bool get_under_water(CTerrainWater* pWaters, 
	CTerrainZone* pZone, const FmVec3& center, float radius)
{
	CTerrainChunk* pChunk = pZone->GetInChunk(center.x, center.z);

	if (NULL == pChunk)
	{
		return false;
	}

	int water_index = pChunk->GetWaterIndex();

	if (size_t(water_index) >= pWaters->GetCount())
	{
		return false;
	}

	SeaWater* pWater = pWaters->GetByIndex(water_index);

	return (center.y - radius) < pWater->GetBaseHeight();
}

bool CTerrainCulling::RefreshAll()
{
	FmVec3 cam_pos = m_pRender->GetContext()->GetCamera().vPosition;
	CZoneManager* pManager = m_pTerrain->GetZoneManager();

	// 设置摄像机所在位置为中心区域
	int zone_row;
	int zone_col;

	if (pManager->GetZonePos(cam_pos.x, cam_pos.z, zone_row, zone_col))
	{
		pManager->SetZoneCurrent(zone_row, zone_col);
	}

	RefreshChunks();
	RefreshVisuals();

	return true;
}

/*
static int __cdecl ground_near_to_far(const void* elem1, const void* elem2)
{
	CTerrainChunk* chunk1 = *((CTerrainChunk**)elem1);
	CTerrainChunk* chunk2 = *((CTerrainChunk**)elem2);

	if (chunk1->GetCameraDistance() < chunk2->GetCameraDistance())
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
*/

bool CTerrainCulling::RefreshChunks()
{
	Terrain* pTerrain = m_pTerrain;
	CZoneManager* pManager = pTerrain->GetZoneManager();
	IRenderContext* pContext = m_pRender->GetContext();
	FmVec3 camera_pos = pContext->GetCamera().vPosition;
	// 是否生成在雾中的地平线
	bool gen_in_fog = false;

	if (pTerrain->GetSmoothHorizontal())
	{
		if (pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0)
		{
			gen_in_fog = true;
		}
	}

	// 地块是否排序 
	int ground_sort_order = pTerrain->GetGroundSortOrder();
	// 混合图淡出的范围
	float alpha_fade_radius = pTerrain->GetAlphaFadeRadius();
	// 混合图消隐的范围
	float alpha_hide_radius = pTerrain->GetAlphaHideRadius();
	// 地平线剪裁
	bool horizon_cull =  pTerrain->GetHorizontalCulling();
	size_t zone_num = 0;
	size_t chunk_num = 0;
	render_zone_t* render_zones = m_pRenderZones;
	CTerrainChunk** render_chunks = m_pRenderChunks;
	render_zone_t* pRenderZone = &render_zones[0];
	
	pRenderZone->nZoneIndex = -1;

	// 对可见区域内的分块做视锥剪裁
	CTerrainZone* zone_list[CZoneManager::RANGE_ZONE_MAX];
	size_t zone_count = pManager->GetRangeZoneList(zone_list, 
		CZoneManager::RANGE_ZONE_MAX, pManager->GetZoneRange());

	for (size_t zone_index = 0; zone_index < zone_count; ++zone_index)
	{
		CTerrainZone* zone = zone_list[zone_index];
	
		if (NULL == zone)
		{
			continue;
		}

		if (!zone->GetAllLoaded())
		{
			// 强制在下一帧再刷新
			//m_bMustRefreshChunk = true;
			continue;
		}

		CTerrainHeight* pHeight = zone->GetHeight();
		// 先做区域剪裁
		FmVec3 v_min(zone->GetLeft(), pHeight->GetMinY(), 
			zone->GetTop());
		FmVec3 v_max(zone->GetRight(), pHeight->GetMaxY(), 
			zone->GetBottom());
		FmVec3 zone_center = (v_min + v_max) * 0.5F;
		float dx = v_max.x - v_min.x;
		float dy = v_max.y - v_min.y;
		float dz = v_max.z - v_min.z;
		float zone_radius = sqrt(dx * dx + dy * dy + dz * dz);

		if (pContext->IsFarClip2D(zone_center, zone_radius))
		{
			zone->SetChunkInvisible();
			continue;
		}

		if (pContext->IsViewClip(zone_center, zone_radius))
		{
			zone->SetChunkInvisible();
			continue;
		}

		const size_t zone_chunk_num = zone->GetChunkCount();
		CTerrainChunk* zone_chunks = zone->GetChunks();

		for (size_t i = 0; i < zone_chunk_num; i++)
		{
			CTerrainChunk* chunk = zone_chunks + i;
			const FmVec3& chunk_center = chunk->GetCenter();
			float chunk_radius = chunk->GetRadius();
			
			// 视距剪裁
			if (pContext->IsFarClip2D(chunk_center, chunk_radius))
			{
				chunk->SetVisible(false);
				continue;
			}
			
			// 视锥剪裁
			if (pContext->IsViewClip(chunk_center, chunk_radius))
			{
				chunk->SetVisible(false);
				continue;
			}

			chunk->SetVisible(true);

			// 到摄像机的距离
			float sx = chunk_center.x - camera_pos.x;
			float sy = chunk_center.y - camera_pos.y;
			float sz = chunk_center.z - camera_pos.z;
			float dist_to_camera = sqrt(
				sx * sx + sy * sy + sz * sz);

			chunk->SetCameraDistance(dist_to_camera);
			// 清除在雾中标志
			chunk->SetInFog(false);

			// 决定是否要淡出
			if ((alpha_fade_radius > 0.0F) && (alpha_hide_radius > 0.0F))
			{
				if (dist_to_camera > alpha_fade_radius)
				{
					chunk->SetAlphaFade(true);
				}
				else
				{
					chunk->SetAlphaFade(false);
				}

				if (dist_to_camera > alpha_hide_radius)
				{
					chunk->SetAlphaHide(true);
				}
				else
				{
					chunk->SetAlphaHide(false);
				}
			}
			else
			{
				chunk->SetAlphaFade(false);
				chunk->SetAlphaHide(false);
			}

			render_chunks[chunk_num] = chunk;

			//render_zone_t* pRenderZone = &render_zones[zone_num];

			if (pRenderZone->nZoneIndex != zone->GetIndex())
			{
				pRenderZone = &render_zones[zone_num];
				pRenderZone->nZoneIndex = (int)zone->GetIndex();
				pRenderZone->nChunkNum = 0;

				if (NULL == pRenderZone->pChunks)
				{
					pRenderZone->pChunks = (CTerrainChunk**)CORE_ALLOC(
						sizeof(CTerrainChunk*) * zone_chunk_num);
					pRenderZone->nZoneChunkNum = zone_chunk_num;
				}

				zone_num++;
				//render_zones[zone_num].nZoneIndex = -1;
			}

			pRenderZone->pChunks[pRenderZone->nChunkNum++] = chunk;
			chunk_num++;
		}
	}

	m_nRenderZoneNum = zone_num;
	m_nRenderChunkNum = chunk_num;

	if ((ground_sort_order > 0) && (chunk_num > 0))
	{
		for (size_t i = 0; i < zone_num; ++i)
		{
			//qsort(render_zones[i].pChunks, render_zones[i].nChunkNum, 
			//	sizeof(CTerrainChunk*),	ground_near_to_far);
			qsort_ground(render_zones[i].pChunks, render_zones[i].nChunkNum);
		}
		
		qsort_ground(render_chunks, chunk_num);
	}

	if (horizon_cull && (chunk_num > 0))
	{
		FmVec3 cam_angle = pContext->GetCamera().vAngle;

		if (cam_angle.x < 0.0F)
		{
			cam_angle.x = -cam_angle.x;
		}

		if (cam_angle.z < 0.0F)
		{
			cam_angle.z = -cam_angle.z;
		}

		// 摄像机必须为大致水平视角
		if ((cam_angle.x < (FLOAT_PI / 6)) && (cam_angle.z < 0.01F))
		{
			// 生成地平线信息
	 		GenerateHorizon();
		}
		else
		{
			// 否则清除地平线信息
			m_nHorizonBucketCount = 0;
		}
	}

	// 设置LOD
	CheckRenderChunkLod();

	m_nInFogChunkNum = 0;

	if (gen_in_fog)
	{
		float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END);

		m_InFogVertex.clear();
		m_InFogIndex.clear();

		for (size_t i = 0; i < chunk_num; ++i)
		{
			CTerrainChunk* chunk = render_chunks[i];

			if ((chunk->GetCameraDistance() - chunk->GetRadius()) > fog_end)
			{
				if (horizon_cull)
				{
					// 地平线剪裁
					if (pTerrain->IsHorizonClip(chunk->GetMin(), 
						chunk->GetMax()))
					{
						continue;
					}
				}

				size_t vertex_size = m_InFogVertex.size();
				size_t vertex_num = chunk->GenInFogVertices(NULL);

				if ((vertex_size + vertex_num) >= 0x10000)
				{
					// 超过了顶点缓冲允许的最大容量
					continue;
				}

				size_t index_size = m_InFogIndex.size();
				size_t index_num = chunk->GenInFogIndices(NULL, 0);

				m_InFogVertex.resize(vertex_size + vertex_num);
				m_InFogIndex.resize(index_size + index_num);
				chunk->GenInFogVertices(&m_InFogVertex[vertex_size]);
				chunk->GenInFogIndices(&m_InFogIndex[index_size], 
					(unsigned short)vertex_size);
				++m_nInFogChunkNum;
				// 完全在雾中
				chunk->SetInFog(true);
			}
		}
	}

	return true;
}

void CTerrainCulling::CheckRenderChunkLod()
{
	Terrain* pTerrain = m_pTerrain;
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();
	const FmMat4& mtxViewProj = camera.mtxViewProj;
	FmVec3 camera_pos = camera.vPosition;
	float lod_radius = pTerrain->GetLodRadius();
	float no_lod_radius = pTerrain->GetNoLodRadius();
	float lod_square = lod_radius * lod_radius;
	float level_down_square = lod_square * 0.1F;
	int level_down = pTerrain->GetGroundLevelDown();
	int lod_levels = (int)pTerrain->GetLodLevels();
	int far_lod_level = pTerrain->GetFarLodLevel();

	if ((far_lod_level < 0) || (far_lod_level >= lod_levels))
	{
		far_lod_level = lod_levels - 1;
	}

	size_t render_chunk_num = m_nRenderChunkNum;
	CTerrainChunk** render_chunks = m_pRenderChunks;

	// 计算LOD级别
	if (lod_radius > 0.0F)
	{
		for (size_t i = 0; i < render_chunk_num; i++)
		{
			CTerrainChunk* chunk = render_chunks[i];
			const FmVec3& center = chunk->GetCenter();
			float sx = camera_pos.x - center.x;
			float sz = camera_pos.z - center.z;
			float dist_square = sx * sx + sz * sz;

			if (dist_square < no_lod_radius * no_lod_radius)
			{
				// 设置误差最小的级别
				chunk->SetMinErrorLevel(lod_levels);
			}
			else if (dist_square > lod_square)
			{
				// 使用最大LOD级别
				chunk->SetLevel(far_lod_level);
			}
			else
			{
				chunk->CalcLevel(m_pRender, mtxViewProj);
			}

			if ((level_down > 0) && (dist_square > level_down_square))
			{
				// 在一定距离外的地表网格降级
				int level = chunk->GetLevel() + level_down;

				if (level >= lod_levels)
				{
					level = lod_levels - 1;
				}

				chunk->SetLevel(level);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < render_chunk_num; i++)
		{
			render_chunks[i]->CalcLevel(m_pRender, mtxViewProj);
		}
	}

	if (pTerrain->GetDesignMode())
	{
		float design_radius = pTerrain->GetDesignRadius();
		float design_radius_sq = design_radius * design_radius;

		for (size_t i = 0; i < render_chunk_num; i++)
		{
			CTerrainChunk* chunk = render_chunks[i];
			const FmVec3& center = chunk->GetCenter();
			float sx = camera_pos.x - center.x;
			float sz = camera_pos.z - center.z;

			// 是否在被编辑范围之内
			if ((sx * sx + sz * sz) < design_radius_sq)
			{
				// 可被编辑分块必须为0级
				chunk->SetLevel(0);
				// 在设计范围内
				chunk->SetInDesign(true);
			}
			else
			{
				// 不在设计范围
				chunk->SetInDesign(false);
			}
		}
	}

	// 检查LOD级别，保证相邻的两块LOD级别相差不超过1
	bool change = true;

	while (change)
	{
		// 循环检查直到不再需要改变
		change = false;

		for (size_t k = 0; k < render_chunk_num; ++k)
		{
			if (render_chunks[k]->ChangeLevel())
			{
				change = true;
			}
		}
	}

	// 决定分块使用什么风格
	for (size_t j = 0; j < render_chunk_num; ++j)
	{
		render_chunks[j]->CalcStyle();
	}
	
	//for (size_t n = 0; n < render_chunk_num; ++n)
	//{
	//	render_chunks[n]->CheckVertexBuffer();
	//}
}

env_map_collect_t* CTerrainCulling::GetEnvMapCollect(size_t index)
{
	if (index >= m_EnvMapCollects.size())
	{
		m_EnvMapCollects.resize(index + 1, NULL);
	}

	if (NULL == m_EnvMapCollects[index])
	{
		m_EnvMapCollects[index] = CORE_NEW(env_map_collect_t);
	}

	return m_EnvMapCollects[index];
}

/*
static int __cdecl chunk_near_to_far(const void* elem1, const void* elem2)
{
	CTerrainChunk* chunk1 = *((CTerrainChunk**)elem1);
	CTerrainChunk* chunk2 = *((CTerrainChunk**)elem2);

	if (chunk1->GetViewZ() < chunk2->GetViewZ())
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
*/

bool CTerrainCulling::RefreshEnvMapChunks(size_t index)
{
	env_map_collect_t* pCollect = GetEnvMapCollect(index);

	pCollect->ClearChunk();

	// 对可见区域内的分块做视锥剪裁
	Terrain* pTerrain = m_pTerrain;
	CZoneManager* pManager = pTerrain->GetZoneManager();
	IRenderContext* pContext = m_pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	int ground_sort_order = pTerrain->GetGroundSortOrder();
	CTerrainZone* zone_list[CZoneManager::RANGE_ZONE_MAX];
	size_t zone_num = pManager->GetRangeZoneList(zone_list, 
		CZoneManager::RANGE_ZONE_MAX, pManager->GetZoneRange());

	for (size_t zone_index = 0; zone_index < zone_num; ++zone_index)
	{
		CTerrainZone* zone = zone_list[zone_index];

		if (NULL == zone)
		{
			continue;
		}

		if (!zone->GetAllLoaded())
		{
			continue;
		}

		CTerrainHeight* pHeight = zone->GetHeight();
		// 先做区域剪裁
		FmVec3 v_min(zone->GetLeft(), pHeight->GetMinY(), 
			zone->GetTop());
		FmVec3 v_max(zone->GetRight(), pHeight->GetMaxY(), 
			zone->GetBottom());
		FmVec3 zone_center = (v_min + v_max) * 0.5F;
		float dx = v_max.x - v_min.x;
		float dy = v_max.y - v_min.y;
		float dz = v_max.z - v_min.z;
		float zone_radius = sqrt(dx * dx + dy * dy + dz * dz);

		if (pContext->IsFarClip2D(zone_center, zone_radius))
		{
			continue;
		}

		if (pContext->IsViewClip(zone_center, zone_radius))
		{
			continue;
		}

		const size_t chunk_num = zone->GetChunkCount();
		CTerrainChunk* zone_chunks = zone->GetChunks();

		for (size_t i = 0; i < chunk_num; i++)
		{
			CTerrainChunk* chunk = zone_chunks + i;
			const FmVec3& chunk_center = chunk->GetCenter();
			float chunk_radius = chunk->GetRadius();

			// 视距剪裁
			if (pContext->IsFarClip2D(chunk_center, chunk_radius))
			{
				continue;
			}

			// 视锥剪裁
			if (pContext->IsViewClip(chunk_center, chunk_radius))
			{
				continue;
			}

			// 在视矩阵中的距离
			if (ground_sort_order > 0)
			{
				float view_z = mtxView._13 * chunk_center.x
					+ mtxView._23 * chunk_center.y
					+ mtxView._33 * chunk_center.z + mtxView._43;
				
				chunk->SetViewZ(view_z);
			}

			pCollect->AddChunk((int)zone->GetIndex(), chunk_num, chunk);
		}
	}

	// 对地块进行排序提高渲染速度
	if (ground_sort_order > 0)
	{
		size_t zone_num = pCollect->nZoneNum;
		render_zone_t* zones = pCollect->pZones;
		
		for (size_t i = 0; i < pCollect->nZoneNum; ++i)
		{
			//qsort(zones[i].pChunks, zones[i].nChunkNum, 
			//	sizeof(CTerrainChunk*),	chunk_near_to_far);
			qsort_chunk(zones[i].pChunks, zones[i].nChunkNum);
		}
	}

	return true;
}

bool CTerrainCulling::RefreshEnvMapVisuals(size_t index, bool only_selected)
{
	env_map_collect_t* pCollect = GetEnvMapCollect(index);

	pCollect->ClearVisual();

	Terrain* pTerrain = m_pTerrain;
	float clip_near = pTerrain->GetClipRadiusNear();
	float clip_far = pTerrain->GetClipRadiusFar();
	float visual_small = pTerrain->GetVisualRadiusSmall();
	float visual_big = pTerrain->GetVisualRadiusBig();
	CZoneManager* pManager = pTerrain->GetZoneManager();
	ICore* pCore = pTerrain->GetCore();
	IRenderContext* pContext = m_pRender->GetContext();
	FmVec3 camera_pos = pContext->GetCamera().vPosition;
	float far_clip = pContext->GetFarClipDistance();
	float zone_size = pTerrain->GetZoneScale() * pTerrain->GetUnitSize();
	int zone_range = (int)pManager->GetZoneRange();

	if (pContext->GetFarClipDistance() < zone_size)
	{
		// 尽量减少要扫描的区域
		zone_range = 1;
	}

	CTerrainZone* zone_list[CZoneManager::RANGE_ZONE_MAX];
	size_t zone_num = pManager->GetRangeZoneList(zone_list, 
		CZoneManager::RANGE_ZONE_MAX, zone_range);

	for (size_t zone_index = 0; zone_index < zone_num; ++zone_index)
	{
		CTerrainZone* zone = zone_list[zone_index];

		if (NULL == zone)
		{
			continue;
		}

		CTerrainVisuals* zone_visuals = zone->GetVisuals();
		size_t visual_size = zone_visuals->GetVisualSize();
		CQuadVisual** visuals = zone_visuals->GetVisuals();

		for (size_t i = 0; i < visual_size; ++i)
		{
			CQuadVisual* pVisual = visuals[i];

			if (NULL == pVisual)
			{
				continue;
			}

			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				pVisual->GetVisualID());

			if (NULL == pVisBase)
			{
				continue;
			}

			if (only_selected)
			{
				// 只反射设置为有倒影的地面物体
				if (!pVisBase->GetWaterReflect())
				{
					continue;
				}
			}

			FmVec3 center = pVisBase->GetCenter();
			float radius = pVisBase->GetRadius();

			// 视距剪裁
			if (pContext->IsFarClip(center, radius))
			{
				continue;
			}

			float sx = center.x - camera_pos.x;
			float sz = center.z - camera_pos.z;
			// 确定此物体的剪裁距离
			float clip_radius = pVisual->GetClipRadius();

			if (clip_radius < clip_near)
			{
				// 根据物体的半径大小计算剪裁距离
				if (radius > visual_big)
				{
					clip_radius = clip_far;
				}
				else if (radius < visual_small)
				{
					clip_radius = clip_near;
				}
				else
				{
					float rate = (radius - visual_small) 
						/ (visual_big - visual_small);

					clip_radius = clip_near + (clip_far - clip_near) * rate;
				}
			}

			if ((sx * sx + sz * sz) 
				> (clip_radius + radius) * (clip_radius + radius))
			{
				// 超出剪裁距离
				continue;
			}

			// 视锥剪裁
			if (pContext->IsViewClip(center, radius))
			{
				continue;
			}

			pCollect->AddVisual(pVisBase);
		}
	}

	return true;
}

/*
static int __cdecl sm_chunk_near_to_far(const void* elem1, const void* elem2)
{
	render_collect_t::chunk_t* p1 = (render_collect_t::chunk_t*)elem1;
	render_collect_t::chunk_t* p2 = (render_collect_t::chunk_t*)elem2;

	if (p1->fViewZ < p2->fViewZ)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

static int __cdecl sm_visual_near_to_far(const void* elem1, const void* elem2)
{
	render_collect_t::visual_t* p1 = (render_collect_t::visual_t*)elem1;
	render_collect_t::visual_t* p2 = (render_collect_t::visual_t*)elem2;

	if (p1->fViewZ < p2->fViewZ)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
*/

 

// 生成视域剪裁面
static void make_clip_planes(const FmMat4& view, const FmMat4& proj, 
	FmPlane* planes, size_t plane_num)
{
	// 性能优化
	static FmVec3 vWorld[8];
	static FmVec3 vScreen[8];

	vScreen[0].x = -1.0F; vScreen[0].y = +1.0F; vScreen[0].z = 0.0F;
	vScreen[1].x = -1.0F; vScreen[1].y = +1.0F; vScreen[1].z = 1.0F;
	vScreen[2].x = -1.0F; vScreen[2].y = -1.0F; vScreen[2].z = 0.0F;
	vScreen[3].x = -1.0F; vScreen[3].y = -1.0F; vScreen[3].z = 1.0F;
	vScreen[4].x = +1.0F; vScreen[4].y = -1.0F; vScreen[4].z = 0.0F;
	vScreen[5].x = +1.0F; vScreen[5].y = -1.0F; vScreen[5].z = 1.0F;
	vScreen[6].x = +1.0F; vScreen[6].y = +1.0F; vScreen[6].z = 0.0F;
	vScreen[7].x = +1.0F; vScreen[7].y = +1.0F; vScreen[7].z = 1.0F;

	for(int i = 0; i < 8 ; i++ )
	{
       FmVec3Unproject(&vWorld[i],&vScreen[i],&proj,&view,NULL);
	}

	// up
	FmPlaneFromPoints(&planes[0], &vWorld[0], &vWorld[1], &vWorld[6]);
	// right
	FmPlaneFromPoints(&planes[1], &vWorld[6], &vWorld[7], &vWorld[4]);
	// down
	FmPlaneFromPoints(&planes[2], &vWorld[4], &vWorld[5], &vWorld[2]);
	// left
	FmPlaneFromPoints(&planes[3], &vWorld[0], &vWorld[2], &vWorld[3]);
	// front
	FmPlaneFromPoints(&planes[4], &vWorld[0], &vWorld[6], &vWorld[2]);
	// back
	FmPlaneFromPoints(&planes[5], &vWorld[1], &vWorld[3], &vWorld[7]);

	FmPlaneNormalize(&planes[0], &planes[0]);
	FmPlaneNormalize(&planes[1], &planes[1]);
	FmPlaneNormalize(&planes[2], &planes[2]);
	FmPlaneNormalize(&planes[3], &planes[3]);
	FmPlaneNormalize(&planes[4], &planes[4]);
	FmPlaneNormalize(&planes[5], &planes[5]);
}

// 视域剪裁并判断物体是否完全包含在内
static inline bool is_view_clip_contain(const FmPlane* planes, 
	size_t plane_num, const FmVec3& center, float radius, bool& contain)
{
	// 是否完全包含物体
	contain = true;
	
	for (size_t k = 0; k < plane_num; ++k)
	{
		float len = FmPlaneDotCoord(&planes[k], &center);

		if (len > -radius)
		{
			contain = false;
		}

		if (len > radius)	
		{
			return true;
		}
	}

	return false;
}

render_collect_t* CTerrainCulling::GetShadowMapCollect(size_t index)
{
	Assert(index < m_ShadowMapCollects.size());

	return m_ShadowMapCollects[index];
}

bool CTerrainCulling::RefreshShadowMapCollects()
{
	IRenderContext* pContext = m_pRender->GetContext();
	
	// 阴影图数量
	int shadow_map_num = pContext->GetShadowMapCount();

	if ((0 == shadow_map_num) || (shadow_map_num > 4))
	{
		return false;
	}

	if (size_t(shadow_map_num) > m_ShadowMapCollects.size())
	{
		m_ShadowMapCollects.resize(shadow_map_num, NULL);
	}

	struct sm_context_t
	{
		FmMat4 mtxView;
		FmMat4 mtxProj;
		FmPlane ClipPlanes[6];
	};

	sm_context_t sm_context[4];
	render_collect_t** pCollects = &m_ShadowMapCollects[0];
	
	for (int i = 0; i < shadow_map_num; ++i)
	{
		if (NULL == pCollects[i])
		{
			pCollects[i] = CORE_NEW(render_collect_t);
		}
		else
		{
			pCollects[i]->ClearChunk();
			pCollects[i]->ClearVisual();
		}

		sm_context[i].mtxView = *pContext->GetShadowMapViewMatrix(i);
		sm_context[i].mtxProj = *pContext->GetShadowMapProjMatrix(i);
		make_clip_planes(sm_context[i].mtxView, sm_context[i].mtxProj,
			sm_context[i].ClipPlanes, 6);
	}
	
	Terrain* pTerrain = m_pTerrain;
	ICore* pCore = pTerrain->GetCore();
	CZoneManager* pManager = pTerrain->GetZoneManager();
	int ground_sort_order = pTerrain->GetGroundSortOrder();
	int visual_sort_order = pTerrain->GetVisualSortOrder();
	float zone_size = pTerrain->GetZoneScale() * pTerrain->GetUnitSize();
	CTerrainZone* zone_list[CZoneManager::RANGE_ZONE_MAX];
	size_t zone_range = pManager->GetZoneRange();

	/*
	if (pContext->GetShadowDistance() < (zone_size * 0.5F))
	{
		// 尽量减少要扫描的区域
		if (zone_range > 1)
		{
			zone_range = 1;
		}
	}
	*/

	size_t zone_num = pManager->GetRangeZoneList(zone_list, 
		CZoneManager::RANGE_ZONE_MAX, zone_range);

	for (size_t zone_index = 0; zone_index < zone_num; ++zone_index)
	{
		CTerrainZone* zone = zone_list[zone_index];

		if (NULL == zone)
		{
			continue;
		}

		if (!zone->GetAllLoaded())
		{
			continue;
		}

		const size_t chunk_num = zone->GetChunkCount();
		CTerrainChunk* zone_chunks = zone->GetChunks();

		for (size_t i = 0; i < chunk_num; i++)
		{
			CTerrainChunk* chunk = zone_chunks + i;
			const FmVec3& center = chunk->GetCenter();
			float radius = chunk->GetRadius();

			for (int sm = 0; sm < shadow_map_num; ++sm)
			{
				// 视锥剪裁
				bool contain;

				if (is_view_clip_contain(sm_context[sm].ClipPlanes, 6, 
					center, radius, contain))
				{
					continue;
				}

				// 在视矩阵中的距离
				if (ground_sort_order > 0)
				{
					const FmMat4& mtxView = sm_context[sm].mtxView;
					
					float view_z = mtxView._13 * center.x 
						+ mtxView._23 * center.y 
						+ mtxView._33 * center.z + mtxView._43;

					pCollects[sm]->AddChunk(chunk, view_z);
				}
				else
				{
					pCollects[sm]->AddChunk(chunk, 0.0F);
				}
				
				if (contain)
				{
					// 完全包含在这一级的阴影图里，不需要再渲染到下一级的阴影图
					break;
				}
			}
		}

		CTerrainVisuals* zone_visuals = zone->GetVisuals();
		size_t visual_size = zone_visuals->GetVisualSize();
		CQuadVisual** visuals = zone_visuals->GetVisuals();

		for (size_t i = 0; i < visual_size; ++i)
		{
			CQuadVisual* pVisual = visuals[i];

			if (NULL == pVisual)
			{
				continue;
			}

			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				pVisual->GetVisualID());

			if (NULL == pVisBase)
			{
				continue;
			}

			if (!(pVisBase->GetCastShadow()))
			{
				// 不需要投影的物体
				continue;
			}

			FmVec3 center = pVisBase->GetCenter();
			float radius = pVisBase->GetRadius();

			for (int sm = 0; sm < shadow_map_num; ++sm)
			{
				// 视锥剪裁
				bool contain;

				if (is_view_clip_contain(sm_context[sm].ClipPlanes, 6, 
					center, radius, contain))
				{
					continue;
				}

				// 在视矩阵中的距离
				if (visual_sort_order > 0)
				{
					const FmMat4& mtxView = sm_context[sm].mtxView;
					float view_z = mtxView._13 * center.x 
						+ mtxView._23 * center.y + mtxView._33 * center.z 
						+ mtxView._43;

					pCollects[sm]->AddVisual(pVisBase, view_z);
				}
				else
				{
					pCollects[sm]->AddVisual(pVisBase, 0.0F);
				}

				if (contain)
				{
					// 完全包含在这一级的阴影图里，不需要再渲染到下一级的阴影图
					break;
				}
			}
		}
	}

	if (ground_sort_order > 0)
	{
		for (int k = 0; k < shadow_map_num; ++k)
		{
			qsort_sm_chunk(pCollects[k]->pChunks, pCollects[k]->nChunkNum);
			//qsort(pCollects[k]->pChunks, pCollects[k]->nChunkNum, 
			//	sizeof(render_collect_t::chunk_t), sm_chunk_near_to_far);
		}
	}
	
	if (visual_sort_order > 0)
	{
		for (int k = 0; k < shadow_map_num; ++k)
		{
			qsort_sm_visual(pCollects[k]->pVisuals, pCollects[k]->nVisualNum); 
			//qsort(pCollects[k]->pVisuals, pCollects[k]->nVisualNum, 
			//	sizeof(render_collect_t::visual_t), sm_visual_near_to_far);
		}
	}

	return true;
}

bool CTerrainCulling::RefreshShadowMapChunks()
{
	if (NULL == m_pShadowMapCollect)
	{
		m_pShadowMapCollect = CORE_NEW(render_collect_t);
	}

	m_pShadowMapCollect->ClearChunk();

	Terrain* pTerrain = m_pTerrain;
	CZoneManager* pManager = pTerrain->GetZoneManager();
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();
	const FmMat4& mtxView = camera.mtxView;
	int ground_sort_order = pTerrain->GetGroundSortOrder();
	float chunk_size = pTerrain->GetChunkScale() * pTerrain->GetUnitSize();
	float center_x = camera.vFrustumCenter.x;
	float center_z = camera.vFrustumCenter.z;
	float max_distance = camera.fFrustumRadius2D + chunk_size;

	center_x = (float(int(center_x / chunk_size)) + 0.5F) * chunk_size;
	center_z = (float(int(center_z / chunk_size)) + 0.5F) * chunk_size;

	CTerrainChunk* pChunks = NULL;
	int chunk_dims = 0;
	float left = 1e8;
	float top = 1e8;
	float right = -1e8;
	float bottom = -1e8;

	for (int i = 0; i < NEAR_CHUNK_DIMS * NEAR_CHUNK_DIMS; ++i)
	{
		near_info_t* pNear = m_NearChunkInfo + i;

		if (pNear->fDistance > max_distance)
		{
			break;
		}

		float x = center_x + pNear->nColOffset * chunk_size;
		float z = center_z + pNear->nRowOffset * chunk_size;
		
		if ((x < left) || (x > right) || (z < top) || (z > bottom))
		{
			CTerrainZone* pZone = pManager->GetInZone(x, z);

			if (NULL == pZone)
			{
				continue;
			}

			if (!pZone->GetAllLoaded())
			{
				continue;
			}

			pChunks = pZone->GetChunks();
			chunk_dims = (int)pZone->GetChunkDims();
			left = pZone->GetLeft();
			top = pZone->GetTop();
			right = pZone->GetRight();
			bottom = pZone->GetBottom();
		}

		if (NULL == pChunks)
		{
			continue;
		}

		int col = int((x - left) / chunk_size);
		int row = int((z - top) / chunk_size);
		CTerrainChunk* pChunk = pChunks + row * chunk_dims + col;
		const FmVec3& chunk_center = pChunk->GetCenter();
		float chunk_radius = pChunk->GetRadius();

		// 视距剪裁
		if (pContext->IsFarClip2D(chunk_center, chunk_radius))
		{
			continue;
		}

		// 视锥剪裁
		if (pContext->IsViewClip(chunk_center, chunk_radius))
		{
			continue;
		}

		// 在视矩阵中的距离
		if (ground_sort_order > 0)
		{
			// 在视矩阵中的距离
			float view_z = mtxView._13 * chunk_center.x
				+ mtxView._23 * chunk_center.y
				+ mtxView._33 * chunk_center.z + mtxView._43;
			
			m_pShadowMapCollect->AddChunk(pChunk, view_z);
		}
		else
		{
			m_pShadowMapCollect->AddChunk(pChunk, 0.0F);
		}
	}
	
	if (ground_sort_order > 0)
	{
		//qsort(m_pShadowMapCollect->pChunks, m_pShadowMapCollect->nChunkNum, 
		//	sizeof(render_collect_t::chunk_t), sm_chunk_near_to_far);
		qsort_sm_chunk(m_pShadowMapCollect->pChunks, 
			m_pShadowMapCollect->nChunkNum);
	}

	return true;
}

bool CTerrainCulling::RefreshShadowMapVisuals()
{
	if (NULL == m_pShadowMapCollect)
	{
		m_pShadowMapCollect = CORE_NEW(render_collect_t);
	}

	m_pShadowMapCollect->ClearVisual();

	Terrain* pTerrain = m_pTerrain;
	CZoneManager* pManager = pTerrain->GetZoneManager();
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();
	const FmMat4& mtxView = camera.mtxView;
	int visual_sort_order = pTerrain->GetVisualSortOrder();
	ICore* pCore = pTerrain->GetCore();
	float grid_size = pTerrain->GetCollideScale() * pTerrain->GetUnitSize();
	float center_x = camera.vFrustumCenter.x;
	float center_z = camera.vFrustumCenter.z;
	float max_distance = camera.fFrustumRadius2D + grid_size;

	center_x = (float(int(center_x / grid_size)) + 0.5F) * grid_size;
	center_z = (float(int(center_z / grid_size)) + 0.5F) * grid_size;

	CQuadGround* pCollides = NULL;
	int collide_dims = 0;
	float left = 1e8;
	float top = 1e8;
	float right = -1e8;
	float bottom = -1e8;
	CQuadVisual* processed = NULL;

	for (int i = 0; i < NEAR_GRID_DIMS * NEAR_GRID_DIMS; ++i)
	{
		near_info_t* pNear = m_NearGridInfo + i;

		if (pNear->fDistance > max_distance)
		{
			break;
		}

		float x = center_x + pNear->nColOffset * grid_size;
		float z = center_z + pNear->nRowOffset * grid_size;

		if ((x < left) || (x > right) || (z < top) || (z > bottom))
		{
			CTerrainZone* pZone = pManager->GetInZone(x, z);

			if (NULL == pZone)
			{
				continue;
			}

			pCollides = pZone->GetCollides();
			collide_dims = (int)pZone->GetCollideDims();
			left = pZone->GetLeft();
			top = pZone->GetTop();
			right = pZone->GetRight();
			bottom = pZone->GetBottom();
		}

		if (NULL == pCollides)
		{
			continue;
		}

		int col = int((x - left) / grid_size);
		int row = int((z - top) / grid_size);
		CQuadGround* pGround = pCollides + row * collide_dims + col;
		CQuadNode* pNode = pGround->m_pNode;

		if (NULL == pNode)
		{
			continue;
		}

		size_t object_num = pNode->m_Objects.size();

		if (0 == object_num)
		{
			continue;
		}

		CQuadVisual** objects = &pNode->m_Objects[0];

		for (size_t k = 0; k < object_num; ++k)
		{
			CQuadVisual* pObject = objects[k];

			if (pObject->m_bUsed)
			{
				// 防止重复添加物体
				continue;
			}

			// 设置已处理标志
			pObject->m_bUsed = true;
			pObject->m_pNext = processed;
			processed = pObject;
			
			CQuadVisual* pVisual = (CQuadVisual*)pObject;
			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				pVisual->GetVisualID());

			if (NULL == pVisBase)
			{
				continue;
			}

			FmVec3 center = pVisBase->GetCenter();
			float radius = pVisBase->GetRadius();

			// 视距剪裁
			if (pContext->IsFarClip(center, radius))
			{
				continue;
			}

			// 视锥剪裁
			if (pContext->IsViewClip(center, radius))
			{
				continue;
			}

			if (visual_sort_order > 0)
			{
				// 在视矩阵中的距离
				float view_z = mtxView._13 * center.x
					+ mtxView._23 * center.y
					+ mtxView._33 * center.z + mtxView._43;
				
				m_pShadowMapCollect->AddVisual(pVisBase, view_z);
			}
			else
			{
				m_pShadowMapCollect->AddVisual(pVisBase, 0.0F);
			}
		}
	}

	if (visual_sort_order > 0)
	{
		qsort_sm_visual(m_pShadowMapCollect->pVisuals, 
			m_pShadowMapCollect->nVisualNum);
		//qsort(m_pShadowMapCollect->pVisuals, m_pShadowMapCollect->nVisualNum, 
		//	sizeof(render_collect_t::visual_t), sm_visual_near_to_far);
	}

	// 清除物体的已处理标志
	while (processed)
	{
		processed->m_bUsed = false;
		processed = processed->m_pNext;
	}

	return true;
}

/*
static int __cdecl visual_near_to_far(const void* elem1, const void* elem2)
{
	if (((render_visual_t*)elem1)->fViewZ < ((render_visual_t*)elem2)->fViewZ)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
*/

bool CTerrainCulling::RefreshVisuals()
{
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();
	const FmMat4& mtxView = camera.mtxView;
	Terrain* pTerrain = m_pTerrain;
	CTerrainWater* pWaters = pTerrain->GetWater();
	float clip_near = pTerrain->GetClipRadiusNear();
	float clip_far = pTerrain->GetClipRadiusFar();
	float visual_small = pTerrain->GetVisualRadiusSmall();
	float visual_big = pTerrain->GetVisualRadiusBig();
	// 物体排序方式
	int visual_sort_order = pTerrain->GetVisualSortOrder();
	bool design_mode = pTerrain->GetDesignMode();
	bool visual_fade_in = pTerrain->GetVisualFadeIn();
	bool horizontal_culling = pTerrain->GetHorizontalCulling();
	CZoneManager* pManager = pTerrain->GetZoneManager();
	ICore* pCore = pTerrain->GetCore();
	FmVec3 camera_pos = camera.vPosition;
	FmVec3 old_camera_pos = pContext->GetPrevCamera().vPosition;
	float lod_begin = pTerrain->GetVisualLodBegin();
	float lod_end = pTerrain->GetVisualLodEnd();
	bool need_lod = lod_end > lod_begin;
	float lod_begin_square;
	float lod_end_square;

	if (need_lod)
	{
		lod_begin_square = lod_begin * lod_begin;
		lod_end_square = lod_end * lod_end;
	}
	
	FmVec3 v_min;
	FmVec3 v_max;
	FmVec3 center;
	float radius;
	render_role_t* render_roles = m_pRenderRoles;
	render_visual_t* render_visuals = m_pRenderVisuals;
	PERSISTID* update_visuals = m_pUpdateVisuals;
	size_t render_role_num = 0;
	size_t render_visual_num = 0;
	size_t update_visual_num = 0;
	CTerrainZone* zone_list[CZoneManager::RANGE_ZONE_MAX];
	size_t zone_num = pManager->GetRangeZoneList(zone_list, 
		CZoneManager::RANGE_ZONE_MAX, pManager->GetZoneRange());

	for (size_t zone_index = 0; zone_index < zone_num; ++zone_index)
	{
		CTerrainZone* zone = zone_list[zone_index];

		if (NULL == zone)
		{
			continue;
		}

		CTerrainVisuals* zone_visuals = zone->GetVisuals();
		size_t visual_size = zone_visuals->GetVisualSize();
		CQuadVisual** visuals = zone_visuals->GetVisuals();
		
		for (size_t i = 0; i < visual_size; ++i)
		{
			CQuadVisual* pVisual = visuals[i];
			
			if (NULL == pVisual)
			{
				continue;
			}

			IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
				pVisual->GetVisualID());

			if (NULL == pVisBase)
			{
				// 必须清理一些自动消失的物体，否则会造成内存持续增长
				zone_visuals->RemoveByIndex(i);
				continue;
			}

			if (design_mode)
			{
				if (VisUtil_GetCustomBool(pVisBase, "only_design"))
				{
					if (!pTerrain->GetHelperVisible())
					{
						// 不显示辅助物体
						pVisBase->SetAlphaValue(0.0F);
						pVisBase->SetVisible(false);
						continue;
					}
					else
					{
						// 显示辅助物体
						pVisBase->SetVisible(true);
					}
				}
			}

			center = pVisBase->GetCenter();
			radius = pVisBase->GetRadius();

			if (visuals[i]->GetIsRole())
			{
				bool visible = true;

				// 视距剪裁
				if (pContext->IsFarClip(center, radius))
				{
					visible = false;
				}

				// 视锥剪裁
				if (pContext->IsViewClip(center, radius))
				{
					visible = false;
				}

				if (render_role_num >= MAX_RENDER_ROLES)
				{
					break;
				}

				if (need_lod)
				{
					if (visible)
					{
						float sx = center.x - camera_pos.x;
						float sy = center.y - camera_pos.y;
						float sz = center.z - camera_pos.z;
						float square = sx * sx + sy * sy + sz * sz;
						float lod_value;

						if (square < lod_begin_square)
						{
							lod_value = 1.0F;
						}
						else if (square > lod_end_square)
						{
							lod_value = 0.0F;
						}
						else
						{
							float d = sqrt(square);

							// 根据到摄像机的距离设置LOD因子
							lod_value = (lod_end - d) / (lod_end - lod_begin);
						}

						pVisBase->SetLevelOfDetail(lod_value);
					}
					else
					{
						// 看不见的物体设置为最低细节
						pVisBase->SetLevelOfDetail(0.0F);
					}
				}

				render_role_t* pRenderRole = &render_roles[render_role_num];

				pRenderRole->VisualID = pVisBase->GetID();
				pRenderRole->pVisBase = pVisBase;
				pRenderRole->bVisible = visible;
				pRenderRole->bUnderWater = get_under_water(pWaters, zone, 
					center, radius);
				render_role_num++;
				update_visuals[update_visual_num] = pVisBase->GetID();
				update_visual_num++;
				continue;
			}

			
			// 视距剪裁
			if (pContext->IsFarClip(center, radius))
			{
				pVisBase->SetAlphaValue(0.0F);
				continue;
			}
			

			float sx = center.x - camera_pos.x;
			float sz = center.z - camera_pos.z;

			// 确定此物体的剪裁距离
			float clip_radius = pVisual->GetClipRadius();

			if (clip_radius < clip_near)
			{
				// 根据物体的半径大小计算剪裁距离
				if (radius > visual_big)
				{
					clip_radius = clip_far;
				}
				else if (radius < visual_small)
				{
					clip_radius = clip_near;
				}
				else
				{
					float rate = (radius - visual_small) 
						/ (visual_big - visual_small);

					clip_radius = clip_near + (clip_far - clip_near) * rate;
				}
			}

			float clip_square = sx * sx + sz * sz;

			if (clip_square > (clip_radius + radius) * (clip_radius + radius))
			{
				// 超出剪裁距离
				pVisBase->SetAlphaValue(0.0F);
				continue;
			}
			
			// 视锥剪裁
			if (pContext->IsViewClip(center, radius))
			{
				pVisBase->SetAlphaValue(0.0F);
				continue;
			}

			if (horizontal_culling)
			{
				// 地平线剪裁
				v_min.x = center.x - radius;
				v_min.y = center.y - radius;
				v_min.z = center.z - radius;
				v_max.x = center.x + radius;
				v_max.y = center.y + radius;
				v_max.z = center.z + radius;

				if (IsHorizonClip(v_min, v_max))
				{
					pVisBase->SetAlphaValue(0.0F);
					continue;
				}
			}

			if (render_visual_num >= MAX_RENDER_VISUALS)
			{
				break;
			}

			render_visual_t* pRenderVisual = 
				&render_visuals[render_visual_num];
			
			//pRenderVisual->VisualID = pVisBase->GetID();
			pRenderVisual->pVisBase = pVisBase;
			pRenderVisual->bUnderWater = get_under_water(pWaters, zone, 
				center, radius);
			render_visual_num++;
			update_visuals[update_visual_num] = pVisBase->GetID();
			update_visual_num++;
			
			if (visual_sort_order > 0)
			{
				float view_z = mtxView._13 * center.x
					+ mtxView._23 * center.y 
					+ mtxView._33 * center.z + mtxView._43;

				pRenderVisual->fViewZ = view_z;
			}

			if (visual_fade_in)
			{
				if (clip_square < (clip_near * clip_near))
				{
					// 已经在淡入过程中的物体不可设置立即显示
					if (pVisBase->GetAlphaValue() == 0.0F)
					{
						// 近处的物体立即显示
						pVisBase->SetAlphaValue(1.0F);
					}
				}
				else
				{
					float old_sx = center.x - old_camera_pos.x;
					float old_sz = center.z - old_camera_pos.z;
					float old_square = old_sx * old_sx + old_sz * old_sz;

					// 到摄像机的距离没有发生变化
					if (old_square <= clip_square)
					{
						// 立即显示物体
						pVisBase->SetAlphaValue(1.0F);
					}
				}
			}
			
			if (need_lod)
			{
				float sy = center.y - camera_pos.y;
				float square = sx * sx + sy * sy + sz * sz;
				float lod_value;

				if (square < lod_begin_square)
				{
					lod_value = 1.0F;
				}
				else if (square > lod_end_square)
				{
					lod_value = 0.0F;
				}
				else
				{
					float d = sqrt(square);

					// 根据到摄像机的距离设置LOD因子
					lod_value = (lod_end - d) / (lod_end - lod_begin);
				}

				pVisBase->SetLevelOfDetail(lod_value);
			}
		}
	}

	// 前后排序
	if ((visual_sort_order > 0) && (render_visual_num > 0))
	{
		//qsort(render_visuals, render_visual_num, sizeof(render_visual_t), 
		//	visual_near_to_far);
		qsort_visual(render_visuals, render_visual_num);
	}

	m_nRenderRoleNum = render_role_num;
	m_nRenderVisualNum = render_visual_num;
	m_nUpdateVisualNum = update_visual_num;

	return true;
}

bool CTerrainCulling::GenerateHorizon()
{
	//double t1 = performance_time();
	// 现在的每帧消耗时间约0.25毫秒

	IRenderContext* pContext = m_pRender->GetContext();
	int chunk_scale = (int)m_pTerrain->GetChunkScale();
	int collide_scale = (int)m_pTerrain->GetCollideScale();
	float unit_size = m_pTerrain->GetUnitSize();
	float far_clip_distance = pContext->GetFarClipDistance();
	
	m_mtxViewProj = pContext->GetCamera().mtxViewProj;

	for (size_t j = 0; j <= HORIZON_BUCKET_SIZE; ++j)
	{
		m_pHorizonBuckets[j].nCount = 0;
	}

	m_nHorizonBucketCount = HORIZON_BUCKET_SIZE + 1;

	FmVec3 chunk_min;
	FmVec3 vIn[4];
	FmVec4 v[4];

	for (size_t i = 0; i < m_nRenderChunkNum; ++i)
	{
		CTerrainChunk* pChunk = m_pRenderChunks[i];
		float distance = pChunk->GetCameraDistance();

		if (distance > (far_clip_distance * 0.5F))
		{
			// 不使用距离远的地块做遮挡
			continue;
		}
		
		CTerrainHeight* pHeight = pChunk->GetZone()->GetHeight();
		int chunk_level = pChunk->GetLevel();
		int chunk_row = pChunk->GetLocalRow();
		int chunk_col = pChunk->GetLocalCol();
		
		chunk_min = pChunk->GetMin();

		float rate = distance / far_clip_distance * 16.0F;
		int shift = int(rate);

		if (shift < 3)
		{
			shift = 3;
		}

		if (shift < chunk_level)
		{
			shift = chunk_level;
		}

		if (shift > 8)
		{
			shift = 8;
		}

		int scale = 0x1 << shift;

		if (scale < collide_scale)
		{
			scale = collide_scale;
		}

		if (scale > chunk_scale)
		{
			scale = chunk_scale;
		}

		for (int row = 0; row < chunk_scale; row += scale)
		{
			for (int col = 0; col < chunk_scale; col += scale)
			{
				// 区域的最低点
				float min_y;

				if (scale == chunk_scale)
				{
					min_y = chunk_min.y;
				}
				else
				{
					// 在编辑模式下无法保证最低点位置的即时性
					min_y = pHeight->GetRegionMinY(chunk_row + row, 
						chunk_col + col, scale);
				}

				// 包围盒底面的四个顶点
				float min_x = chunk_min.x + col * unit_size;
				float min_z = chunk_min.z + row * unit_size;
				float max_x = min_x + scale * unit_size;
				float max_z = min_z + scale * unit_size;

				vIn[0].x = min_x;
				vIn[0].y = min_y;
				vIn[0].z = min_z;
				vIn[1].x = max_x;
				vIn[1].y = min_y;
				vIn[1].z = min_z;
				vIn[2].x = min_x;
				vIn[2].y = min_y;
				vIn[2].z = max_z;
				vIn[3].x = max_x;
				vIn[3].y = min_y;
				vIn[3].z = max_z;

				for(int t = 0 ; t < 4; t++ )
				{
					FmVec3Transform(&v[t],&vIn[t],&m_mtxViewProj);
				}

				if ((v[0].w < 0.001F) || (v[1].w < 0.001F) 
					|| (v[2].w < 0.001F) || (v[3].w < 0.001F))
				{
					// 在视点的后面
					continue;
				}

				float rw0 = 1.0F / v[0].w;
				float rw1 = 1.0F / v[1].w;
				float rw2 = 1.0F / v[2].w;
				float rw3 = 1.0F / v[3].w;
				float x0 = v[0].x * rw0;
				float y0 = v[0].y * rw0;
				float z0 = v[0].z * rw0;
				float x1 = v[1].x * rw1;
				float y1 = v[1].y * rw1;
				float z1 = v[1].z * rw1;
				float x2 = v[2].x * rw2;
				float y2 = v[2].y * rw2;
				float z2 = v[2].z * rw2;
				float x3 = v[3].x * rw3;
				float y3 = v[3].y * rw3;
				float z3 = v[3].z * rw3;
				// 在屏幕上的高度
				float screen_y = y0;

				if (y1 < screen_y)
				{
					screen_y = y1;
				}

				if (y2 < screen_y)
				{
					screen_y = y2;
				}

				if (y3 < screen_y)
				{
					screen_y = y3;
				}

				screen_y += 1.0F;

				if (screen_y < 0.0F)
				{
					// 在屏幕之外
					continue;
				}

				// 取最大屏幕深度
				float screen_z = z0; 

				if (z1 > screen_z)
				{
					screen_z = z1;
				}

				if (z2 > screen_z)
				{
					screen_z = z2;
				}

				if (z3 > screen_z)
				{
					screen_z = z3;
				}

				if ((screen_z < 0.0F) || (screen_z > 1.0F))
				{
					continue;
				}

				// 在屏幕上的宽度
				float min_screen_x = x0;

				if (x1 < min_screen_x)
				{
					min_screen_x = x1;
				}
				
				if (x2 < min_screen_x)
				{
					min_screen_x = x2;
				}
				
				if (x3 < min_screen_x)
				{
					min_screen_x = x3;
				}
				
				float max_screen_x = x0;

				if (x1 > max_screen_x)
				{
					max_screen_x = x1;
				}

				if (x2 > max_screen_x)
				{
					max_screen_x = x2;
				}
				
				if (x3 > max_screen_x)
				{
					max_screen_x = x3;
				}

				min_screen_x += 1.0F;
				max_screen_x += 1.0F;

				if ((min_screen_x < 0.0F) && (max_screen_x < 0.0F))
				{
					// 在屏幕之外
					continue;
				}

				if ((min_screen_x > 2.0F) && (max_screen_x > 2.0F))
				{
					// 在屏幕之外
					continue;
				}

				int min_pos = 
					int(min_screen_x * float(HORIZON_BUCKET_SIZE / 2));
				int max_pos = 
					int(max_screen_x * float(HORIZON_BUCKET_SIZE / 2));

				if (min_pos < 0)
				{
					min_pos = 0;
				}

				if (max_pos > HORIZON_BUCKET_SIZE)
				{
					max_pos = HORIZON_BUCKET_SIZE;
				}

				for (int pos = min_pos; pos <= max_pos; ++pos)
				{
					horizon_bucket_t& bucket = m_pHorizonBuckets[pos];
					int count = bucket.nCount;

					if (count == HORIZON_POINT_NUM)
					{
						continue;
					}

					float* pScreenY = bucket.fScreenY;
					float* pScreenZ = bucket.fScreenZ;
					int index = 0;

					for (; index < count; ++index)
					{
						// 比较深度
						if (pScreenZ[index] <= screen_z)
						{
							// 比较高度
							if (pScreenY[index] >= screen_y)
							{
								// 比当前记录的点更低
								index = HORIZON_POINT_NUM;
								break;
							}
						}
						else
						{
							if (pScreenY[index] <= screen_y)
							{
								// 比后面的更高
								count = index;
							}
							
							break;
						}
					}

					if (index >= HORIZON_POINT_NUM)
					{
						continue;
					}

					// 在中间插入数据
					if (index < count)
					{
						// 数据向后平移
						size_t len = sizeof(float) * (count - index);

						memmove(pScreenY + index + 1, pScreenY + index, len);
						memmove(pScreenZ + index + 1, pScreenZ + index, len);
					}

					pScreenY[index] = screen_y;
					pScreenZ[index] = screen_z;
					bucket.nCount = count + 1;
				}
			}
		}
	}
	
	//double t2 = performance_time() - t1;
	//char info[128];
	//SafeSprintf(info, sizeof(info), 
	//	"(CTerrainCulling::GenerateHorizon)use %f seconds", t2);
	//CORE_TRACE(info);

	return true;
}

bool CTerrainCulling::IsHorizonClip(const FmVec3& v_min, 
	const FmVec3& v_max)
{
	if (0 == m_nHorizonBucketCount)
	{
		return false;
	}

	// 包围盒顶部的四个点
	static FmVec3 vIn[4];
	static FmVec4 vOut[4];

	vIn[0].x = v_min.x;
	vIn[0].y = v_max.y;
	vIn[0].z = v_min.z;
	vIn[1].x = v_max.x;
	vIn[1].y = v_max.y;
	vIn[1].z = v_min.z;
	vIn[2].x = v_min.x;
	vIn[2].y = v_max.y;
	vIn[2].z = v_max.z;
	vIn[3].x = v_max.x;
	vIn[3].y = v_max.y;
	vIn[3].z = v_max.z;

	for(int i = 0; i < 4 ; i++ )
	{
		FmVec3Transform(&vOut[i],&vIn[i],&m_mtxViewProj);
	}

	if ((vOut[0].w < 0.001F) || (vOut[1].w < 0.001F) || (vOut[2].w < 0.001F)
		|| (vOut[3].w < 0.001F))
	{
		// 在视点的后面
		return false;
	}

	float rw0 = 1.0F / vOut[0].w;
	float rw1 = 1.0F / vOut[1].w;
	float rw2 = 1.0F / vOut[2].w;
	float rw3 = 1.0F / vOut[3].w;
	float x0 = vOut[0].x * rw0;
	float y0 = vOut[0].y * rw0;
	float z0 = vOut[0].z * rw0;
	float x1 = vOut[1].x * rw1;
	float y1 = vOut[1].y * rw1;
	float z1 = vOut[1].z * rw1;
	float x2 = vOut[2].x * rw2;
	float y2 = vOut[2].y * rw2;
	float z2 = vOut[2].z * rw2;
	float x3 = vOut[3].x * rw3;
	float y3 = vOut[3].y * rw3;
	float z3 = vOut[3].z * rw3;
	// 在屏幕上的高度
	float screen_y = y0;

	if (y1 > screen_y)
	{
		screen_y = y1;
	}

	if (y2 > screen_y)
	{
		screen_y = y2;
	}

	if (y3 > screen_y)
	{
		screen_y = y3;
	}

	screen_y += 1.0F;

	if (screen_y < 0.0F)
	{
		// 在屏幕之外
		return false;
	}

	// 取最小屏幕深度
	float screen_z = z0; 

	if (z1 < screen_z)
	{
		screen_z = z1;
	}

	if (z2 < screen_z)
	{
		screen_z = z2;
	}

	if (z3 < screen_z)
	{
		screen_z = z3;
	}

	if ((screen_z < 0.0F) || (screen_z > 1.0F))
	{
		return false;
	}
	
	// 在屏幕上的宽度
	float min_screen_x = x0;

	if (x1 < min_screen_x)
	{
		min_screen_x = x1;
	}

	if (x2 < min_screen_x)
	{
		min_screen_x = x2;
	}

	if (x3 < min_screen_x)
	{
		min_screen_x = x3;
	}

	float max_screen_x = x0;

	if (x1 > max_screen_x)
	{
		max_screen_x = x1;
	}

	if (x2 > max_screen_x)
	{
		max_screen_x = x2;
	}

	if (x3 > max_screen_x)
	{
		max_screen_x = x3;
	}

	min_screen_x += 1.0F;
	max_screen_x += 1.0F;

	if ((min_screen_x < 0.0F) && (max_screen_x < 0.0F))
	{
		// 在屏幕之外
		return false;
	}

	if ((min_screen_x > 2.0F) && (max_screen_x > 2.0F))
	{
		// 在屏幕之外
		return false;
	}

	int min_pos = int(min_screen_x * float(HORIZON_BUCKET_SIZE / 2));
	int max_pos = int(max_screen_x * float(HORIZON_BUCKET_SIZE / 2));

	if (min_pos < 0)
	{
		min_pos = 0;
	}

	if (max_pos > HORIZON_BUCKET_SIZE)
	{
		max_pos = HORIZON_BUCKET_SIZE;
	}

	for (int pos = min_pos; pos <= max_pos; ++pos)
	{
		horizon_bucket_t& bucket = m_pHorizonBuckets[pos];
		int count = bucket.nCount;
		float* pScreenZ = bucket.fScreenZ;
		int index;

		for (index = 0; index < count; ++index)
		{
			if (pScreenZ[index] > screen_z)
			{
				break;
			}
		}

		if (0 == index)
		{
			// 没有遮挡
			return false;
		}

		if (screen_y > bucket.fScreenY[index - 1])
		{
			// 高于地平线
			return false;
		}
	}

	return true;
}

bool CTerrainCulling::DumpHorizon(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	for (size_t i = 0; i < m_nHorizonBucketCount; ++i)
	{
		size_t count = m_pHorizonBuckets[i].nCount;
		
		core_file::fprintf(fp, "%04d,%02d", i, m_pHorizonBuckets[i].nCount);

		for (size_t k = 0; k < count; ++k)
		{
			core_file::fprintf(fp, ",[%f](%f)", m_pHorizonBuckets[i].fScreenZ[k],
				m_pHorizonBuckets[i].fScreenY[k]);
		}

		core_file::fprintf(fp, "\r\n");
	}

	core_file::fclose(fp);
	
	return true;
}
