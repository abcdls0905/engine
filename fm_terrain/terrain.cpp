//--------------------------------------------------------------------
// 文件名:		terrain.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "terrain_light.h"
#include "terrain_editor.h"
#include "terrain_brush.h"
#include "terrain_material.h"
#include "terrain_water.h"
#include "terrain_grass.h"
#include "terrain_culling.h"
#include "terrain_painter.h"
#include "terrain_visuals.h"
#include "terrain_region.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "chunk_style.h"
#include "sea_water.h"
#include "sea_base.h"
#include "grass_type.h"
#include "zone_manager.h"
#include "visual_manager.h"
#include "quad_collide.h"
#include "quad_visual.h"
#include "quad_tree.h"
#include "light_map_config.h"
#include "dynamic_walkable_manager.h"
#include "dynamic_walk_loader.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_file.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime
// 地形实体属性和方法的定义见TerrainAttr.cpp

void get_local_pos(IVisBase* pVisual, size_t collide_per_unit, float gx, float gz, float angleY, int& row, int& col);
void get_pos(IVisBase* pVisBase, size_t collide_per_unit, int row, int col, float angleY, float& gx, float& gz);
void extern_to_2n(size_t& row, size_t& col);
size_t get2n(size_t value);

// Terrain

Terrain::Terrain()
{
	m_pRender = NULL;
	m_pContext = NULL;
	m_pEditor = NULL;
	m_pBrush = NULL;
	m_pMaterial = NULL;
	m_pLightModel = NULL;
	m_pSoundModel = NULL;
	m_pSoundRangeModel = NULL;
	m_pParticleModel = NULL;
	m_pEffectModel = NULL;
	m_pTriggerModel = NULL;
	m_pTreeModel = NULL;

	SetParameter(256, 32, 256, 4, 2, 2);
	
	m_Material.vDiffuse.w = 1.0F;
	m_Material.vDiffuse.x = 0.59F;
	m_Material.vDiffuse.y = 0.59F;
	m_Material.vDiffuse.z = 0.59F;	
	m_Material.vAmbient.w = 1.0F;
	m_Material.vAmbient.x = 0.59F;
	m_Material.vAmbient.y = 0.59F;
	m_Material.vAmbient.z = 0.59F;
	m_Material.vSpecular.w = 1.0F;
	m_Material.vSpecular.x = 0.9F;
	m_Material.vSpecular.y = 0.9F;
	m_Material.vSpecular.z = 0.9F;	
	m_Material.vEmissive.w = 1.0F;
	m_Material.vEmissive.x = 0.0F;
	m_Material.vEmissive.y = 0.0F;
	m_Material.vEmissive.z = 0.0F;
	m_Material.fSpecularPower = 10.0F;
	m_Material.bSpecularEnable = false;
	
	m_nLightTexScale = 512;
	m_nLightPerUnit = 2;
	m_nCollideScale = 8;
	m_nCollidePerUnit = 2;
	m_bDesignMode = false;
	m_nShowDesignLine = 0;
	m_bShowWalkable = false;
	m_bShowSpaceHeight = false;
	m_bShowLightRange = false;
	m_bShowRegion = false;
	m_bShowArea = false;
	m_bShowIcon = true;
	m_bShowCollide = false;
	m_bShowDesignNoEdge = false;
	m_bShowTree = true;
	m_bShowQuadTree = false;
	m_nFarLodLevel = -1;
	m_fLodRadius = 300.0F;
	m_fNoLodRadius = 50.0F;
	m_fDesignRadius = 200.0F;
	m_fCollideRadius = 50.0F;
	m_fAlphaFadeRadius = 300.0F;
	m_fAlphaHideRadius = 350.0F;
	m_fGrassRadius = 100.0F;
	m_fGrassLod = 1.0F;
	m_fVisualLodBegin = 20.0F;
	m_fVisualLodEnd = 200.0F;
	m_bWaterVisible = true;
	m_bGroundVisible = true;
	m_bVisualVisible = true;
	m_bHelperVisible = true;
	m_bWaterWire = false;
	m_bGroundWire = false;
	m_bVisualWire = false;
	m_bNoLight = false;
	m_bSimpleGround = false;
	m_bHorizontalCulling = true;
	m_bVisualFadeIn = true;
	m_bSmoothHorizontal = true;
	m_bWaterWave = true;
	m_bWaterDepth = true;
	m_bWaterBorder = true;
	m_bWaterReflect = true;
	m_bWaterEnvMapReusable = false;
	m_bWaterSunReflect = true;
	m_bUpdateFocus = true;
	m_bOnlyCullTerrain = false;
	m_bSphereAmbient = false;
	m_bVerifyTraceResult = false;
	m_bGenWalkTrace = false;
	m_nGroundLevelDown = 0;
	m_nGroundEnvMap = 0;
	m_nVisualEnvMap = 0;
	m_nTraceMask = 0;
	m_nGroundSortOrder = 1;
	m_nVisualSortOrder = 1;
	m_fClipRadiusNear = 100.0F;
	m_fClipRadiusFar = 200.0F;
	m_fVisualRadiusSmall = 1.0F;
	m_fVisualRadiusBig = 30.0F;
	m_fLoadRadiusAdd = 50.0F;
	m_fUnloadRadiusAdd = 50.0F;
	m_fLeft = 0.0F;
	m_fTop = 0.0F;
	m_fUnitSize = 1.0F;
	m_fInvUnitSize = 1.0F / m_fUnitSize;
	m_fCollideSize = 0.5F;
	m_fInvCollideSize = 1.0F / m_fCollideSize;
	m_fPixelError = 20.0F;
	m_fInitHeight = 0.0F;
	m_nStyleNum = 0;
	m_pStyles = NULL;
//	m_pGrassInputLayout = NULL;
	m_pGrassIndexBuffer = NULL;
	m_pBaseTexSet = CORE_NEW(CBlendTexSet);
	m_pBaseTexSet->SetTerrain(this);
	m_pBlendTexSet = CORE_NEW(CBlendTexSet);
	m_pBlendTexSet->SetTerrain(this);
	m_pZoneManager = CORE_NEW(CZoneManager);
	m_pZoneManager->SetTerrain(this);
	m_pVisualManager = CORE_NEW(CVisualManager);
	m_pVisualManager->SetTerrain(this);
	m_pCulling = CORE_NEW(CTerrainCulling);
	m_pCulling->SetTerrain(this);
	m_pPainter = CORE_NEW(CTerrainPainter);
	m_pPainter->SetTerrain(this);
	m_pWater = CORE_NEW(CTerrainWater);
	m_pWater->SetTerrain(this);
	m_pGrass = CORE_NEW(CTerrainGrass);
	m_pGrass->SetTerrain(this);
	m_pCollide = CORE_NEW(CQuadCollide);
	m_pCollide->SetTerrain(this);
	m_pDynamicWalkableManager = CORE_NEW(CDynamicWalkableManager);
	m_pDynamicWalkableManager->SetTerrain(this);

	m_bEnableNormalMap = false;
	m_bEnableTessellation = false;
	m_fTessellationFactor = 20.0F;
	m_fNormalMapRadius = 16.0F;

	m_pLightMapConfig = CORE_NEW(CLightMapConfig);
	m_pLightMapConfig->SetTerrain(this);

	m_bCastShadow = false;
	m_bReceiveShadow = false;
}

Terrain::~Terrain()
{
	ReleaseStyleIndexBuffer();
//	SAFE_RELEASE(m_pGrassInputLayout);
	SAFE_RELEASE(m_pGrassIndexBuffer);

	if (m_pStyles)
	{
		for (size_t i = 0; i < m_nStyleNum; ++i)
		{
			if (m_pStyles[i])
			{
				CORE_DELETE(m_pStyles[i]);
				m_pStyles[i] = NULL;
			}
		}

		CORE_FREE(m_pStyles, sizeof(CChunkStyle*) * m_nStyleNum);
		m_pStyles = NULL;
	}

	for (size_t k = 0; k < m_RegionColors.size(); ++k)
	{
		char* pName = m_RegionColors[k].pName;

		CORE_FREE(pName, strlen(pName) + 1);
	}

	for (size_t j = 0; j < m_AreaColors.size(); ++j)
	{
		char* pName = m_AreaColors[j].pName;

		CORE_FREE(pName, strlen(pName) + 1);
	}
	
	CORE_DELETE(m_pBaseTexSet);
	CORE_DELETE(m_pBlendTexSet);
	CORE_DELETE(m_pZoneManager);
	CORE_DELETE(m_pVisualManager);
	CORE_DELETE(m_pCulling);
	CORE_DELETE(m_pPainter);
	CORE_DELETE(m_pGrass);
	CORE_DELETE(m_pWater);
	CORE_DELETE(m_pCollide);
	CORE_DELETE(m_pLightMapConfig);
	CORE_DELETE(m_pDynamicWalkableManager);
}

bool Terrain::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}

	return true;
}

bool Terrain::Shut()
{
	// 删除天空对象
	GetCore()->DeleteEntity(m_SkyID);
	
	// 释放地形中的所有可见对象
	m_pZoneManager->ReleaseAllVisuals();

	SAFE_RELEASE(m_pEditor);
	SAFE_RELEASE(m_pBrush);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pLightModel);
	SAFE_RELEASE(m_pSoundModel);
	SAFE_RELEASE(m_pSoundRangeModel);
	SAFE_RELEASE(m_pParticleModel);
	SAFE_RELEASE(m_pEffectModel);
	SAFE_RELEASE(m_pTriggerModel);
	SAFE_RELEASE(m_pTreeModel);

	// 在此释放才能保证对象还是有效的
	m_pWater->Clear();
	m_pGrass->Clear();
	m_pLightMapConfig->Clear();

	SAFE_RELEASE(m_pRender);

	return true;
}

void Terrain::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
	// 设置自身为场景唯一对象
	m_pContext->SetUnique(this);
}

IRenderContext* Terrain::GetContext() const
{ 
	return m_pContext;
}

void Terrain::GetWorldMatrix(FmMat4& mat)
{
	FmMat4Identity(&mat);
}

bool Terrain::Load()
{
	// 生成地形分块的样式
	m_nStyleNum = m_nLodLevels * 16;
	m_pStyles = (CChunkStyle**)CORE_ALLOC(sizeof(CChunkStyle*) * m_nStyleNum);
	memset(m_pStyles, 0, sizeof(CChunkStyle*) * m_nStyleNum);

	// 生成各种可能的分块组成形式
	for (size_t i = 0; i < m_nLodLevels; i++)
	{
		for (size_t k = 0; k < 16; k++)
		{
			// 最高级只有一种样式
			if ((i == (m_nLodLevels - 1)) && (k > 0))
			{
				continue;
			}

			CChunkStyle* style = CORE_NEW(CChunkStyle);

			// 指定LOD样式树的深度
			style->Generate(m_nChunkScale, (int)i, (int)k, 
				(int)m_nCollideScale);
			m_pStyles[i * 16 + k] = style;
		}
	}

	m_pCulling->Initialize(m_pRender);
	m_pPainter->Initialize(m_pRender);

	// 生成每种分块风格所需的索引缓冲
	CreateStyleIndexBuffer();

	m_pWater->Load();
	m_pGrass->Load();
	m_pLightMapConfig->Load();

	return true;
}

PERSISTID QueryCustomObject(IEntity* pEntity, const char* name)
{
	if (pEntity  == NULL)
	{
		return PERSISTID();
	}

	if (pEntity->GetCustoms()  == NULL)
	{
		return PERSISTID();
	}

	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	if (NULL == pVar)
	{
		return PERSISTID();
	}

	return pVar->ObjectVal();
}

void Terrain::Update(float seconds)
{
	m_offset_seconds = seconds;
	m_pCulling->Update(seconds);
	//m_pWater->Update(seconds);
	//m_pGrass->Update(seconds);

	if (m_pBrush)
	{
		m_pBrush->Update(seconds);
	}

	if (!m_SkyID.IsNull())
	{
		IVisBase* pSky = (IVisBase*)GetCore()->GetEntity(m_SkyID);

		if (pSky)
		{
			pSky->Update(seconds);
		}
	}
	
	// 加载数据
	m_pZoneManager->CheckLoading();
	m_pVisualManager->CheckLoading();
}

void Terrain::RealizeReflection(size_t water_index)
{
#ifdef PERFORMANCE_DEBUG
	double dRealizeReflectionMap = performance_time();
#endif
	
	if (m_nVisualEnvMap == 1)
	{
		// 只画设置为有倒影的地面物体
		m_pCulling->RefreshEnvMapVisuals(water_index, true);
		m_pPainter->DrawEnvMapVisuals(water_index);
	}
	else if (m_nVisualEnvMap == 2)
	{
		// 画所有的地面物体
		m_pCulling->RefreshEnvMapVisuals(water_index, false);
		m_pPainter->DrawEnvMapVisuals(water_index);
	}

	// 渲染环境贴图
	if (m_nGroundEnvMap == 1)
	{
		// 画不带混合贴图的地表
		m_pCulling->RefreshEnvMapChunks(water_index);
		m_pPainter->DrawEnvMapSimpleGround(water_index);
	}
	else if (m_nGroundEnvMap == 2)
	{
		// 画带有混合贴图的地表
		m_pCulling->RefreshEnvMapChunks(water_index);
		m_pPainter->DrawEnvMapChunks(water_index);
	}

	if (!m_SkyID.IsNull())
	{
		// 画天空倒影
		IVisBase* pSky = (IVisBase*)GetCore()->GetEntity(m_SkyID);

		if (pSky)
		{
    		pSky->Realize();
		}
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dReflectionRealizeTime +=
		performance_time() - dRealizeReflectionMap;
#endif
}

void Terrain::Realize()
{
  float offset_seconds = m_offset_seconds ;
#ifdef PERFORMANCE_DEBUG
	double dPrepareTerrain = performance_time();
#endif

	// 加载数据
	//m_pZoneManager->CheckLoading();
	//m_pVisualManager->CheckLoading();

	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& cam = pContext->GetCamera();
	float cam_x = cam.vPosition.x;
	float cam_y = cam.vPosition.y;
	float cam_z = cam.vPosition.z;

	// 加快四叉树搜索速度
	m_pCollide->GetQuadTree()->SetLocalRoot(cam_x, cam_z, 
		pContext->GetFarClipDistance());


	// 刷新视野
	m_pCulling->RefreshAll();

	// 更新摄像机焦点位置
	if (m_bUpdateFocus && !m_PlayerID.IsNull())
	{
		IVisBase* pPlayer = (IVisBase*)GetCore()->GetEntity(m_PlayerID);

		if (pPlayer)
		{
			float focus_radius = pPlayer->GetRadius();

			pContext->SetVector(IRenderContext::V_FOCUS_POSITION, 
				pPlayer->GetCenter());
			pContext->SetVector(IRenderContext::V_FOCUS_RADIUS, 
				FmVec3(focus_radius, focus_radius, focus_radius));
		}
	}

	pContext->SetInt(IRenderContext::I_UNDER_WATER, 0);

	CTerrainZone* pCameraZone = m_pZoneManager->GetInZone(cam_x, cam_z);

	if (pCameraZone)
	{
		CTerrainChunk* pCameraChunk = pCameraZone->GetInChunk(cam_x, cam_z);
    if (pCameraChunk)
    {
      int water_index = pCameraChunk->GetWaterIndex();

		    if (size_t(water_index) < m_pWater->GetCount())
		    {
			    // 水面的高度
			    SeaWater* pWater = m_pWater->GetByIndex(water_index);
			    float water_height = pWater->GetCurrentHeight(cam_x, cam_z);

			    if (cam_y <= water_height + 1.0F)
			    {
				    unsigned int fog_color = VisUtil_ColorToInt(
					    pWater->GetWaterColorString().c_str());
				    float fog_distance = 
					    (1.0F - pWater->GetRefractionWeight()) * 200.0F;
				    float fog_start = -fog_distance * 0.5F;
				    float fog_end = fog_start + fog_distance;
				    float fog_density = pWater->GetRefractionWeight();

				    if (cam_y <= water_height)
				    {
					    // 摄像机低于水面
					    pContext->SetInt(IRenderContext::I_UNDER_WATER, 1);
					    // 设置场景批次集的水下标志
    //					m_pRender->SetSceneBatchUnderWater(true);
				    }

				    // 水下雾效
				    pContext->SetColor(IRenderContext::C_FOG_COLOR_UNDERWATER, 
					    fog_color);
				    pContext->SetFloat(IRenderContext::F_FOG_START_UNDERWATER, 
					    fog_start);
				    pContext->SetFloat(IRenderContext::F_FOG_END_UNDERWATER, 
					    fog_end);
				    pContext->SetFloat(IRenderContext::F_FOG_DENSITY_UNDERWATER, 
					    fog_density);
			    }
		    }
        }
	}

	// 复位阴影图相关资源索引
	//m_pPainter->ResetShadowMapIndex();

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dTerrainRefreshTime +=
		performance_time() - dPrepareTerrain;
#endif

	// 地面物体在地表之前渲染可以减少像素填充
	// 画地面物体
	if (m_bVisualVisible)
	{
#ifdef PERFORMANCE_DEBUG
		double dRealizeVisual = performance_time();
#endif

		m_pPainter->DrawVisuals();

#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dVisualRealizeTime +=
			performance_time() - dRealizeVisual;
#endif

#ifdef PERFORMANCE_DEBUG
		double dRealizeGrass = performance_time();
#endif

		m_pGrass->Realize();

#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dGrassRealizeTime +=
			performance_time() - dRealizeGrass;
#endif
	}

	// 画地表
	if (m_bGroundVisible)
	{
#ifdef PERFORMANCE_DEBUG
		double dRealizeGround = performance_time();
#endif

		if (m_bSimpleGround)
		{
			// 绘制简单的地表
			m_pPainter->DrawSimpleGround();
		}
		else
		{
			m_pPainter->DrawChunks();
		}

#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dGroundRealizeTime +=
			performance_time() - dRealizeGround;
#endif
	}

	// 画完地表再画天空，这样可以减少像素填充
	if (!m_SkyID.IsNull())
	{
		IVisBase* pSky = (IVisBase*)GetCore()->GetEntity(m_SkyID);

		if (pSky)
		{
			pSky->Realize();
		}
	}

	// 画水面
	if (m_bWaterVisible)
	{
#ifdef PERFORMANCE_DEBUG
		double dRealizeWater = performance_time();
#endif

		m_pWater->Realize();

#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dWaterRealizeTime +=
			performance_time() - dRealizeWater;
#endif
	}

	// 画刷子
	if (m_pBrush)
	{
		m_pBrush->Realize(offset_seconds);
	}

	if (m_bShowQuadTree)
	{
		m_pPainter->DrawZoneQuadTree();
	}
}

void Terrain::RealizeShadowMap()
{

	int shadow_map_index = 0;//m_pRender->GetContext()->GetShadowMapIndex();

	if (0 == shadow_map_index)
	{
		// 刷新阴影图
		m_pCulling->RefreshShadowMapCollects();
	}

	// 地面物体在地表之前渲染可以减少像素填充
	if (m_bVisualVisible)
	{
		if (shadow_map_index < 0)
		{
			m_pCulling->RefreshShadowMapVisuals();
		}

		m_pPainter->DrawShadowMapVisuals();
	}

	// 地形无需投影
	//if (m_bGroundVisible)
	//{
	//	if (shadow_map_index < 0)
	//	{
	//		m_pCulling->RefreshShadowMapChunks();
	//	}

	//	m_pPainter->DrawShadowMapChunks();
	//}
}

bool Terrain::GetLoadFinish()
{
	return m_pZoneManager->GetLoadFinish() 
		&& m_pBaseTexSet->GetLoadFinish()
		&& m_pBlendTexSet->GetLoadFinish()
		&& m_pGrass->GetLoadFinish()
		&& m_pWater->GetLoadFinish()
		&& m_pVisualManager->GetLoadFinish();
}

float Terrain::GetLoadPercent()
{
	float percent = m_pZoneManager->GetLoadPercent();

	if (percent > 0.999F)
	{
		// 要等地形区域数据全加载完后再考虑地面物体的加载进度
		percent += m_pVisualManager->GetLoadPercent();
	}

	return percent * 0.5F;
}

// 碰撞点距离比较
static int __cdecl trace_hit_compare(const void* p1, const void* p2)
{
	trace_info_t::hit_t* pVal1 = (trace_info_t::hit_t*)p1;
	trace_info_t::hit_t* pVal2 = (trace_info_t::hit_t*)p2;

	if (pVal1->fDistance > pVal2->fDistance)
	{
		return 1;
	}

	if (pVal1->fDistance < pVal2->fDistance)
	{
		return -1;
	}

	return 0;
}

void Terrain::VerifyTraceResult(const FmVec3& src, const FmVec3& dst,
	trace_info_t& result)
{
	if (result.nHitCount <= 1)
	{
		return;
	}

	bool old_trace_hide_model = false;//m_pRender->GetTraceHideModel();

	if (!old_trace_hide_model)
	{
		return;
	}

	// 排序碰撞点
	qsort(result.Hits, result.nHitCount, sizeof(trace_info_t::hit_t), 
		trace_hit_compare);

//	m_pRender->SetTraceHideModel(false);

	TAutoMem<char, 32, TCoreAlloc> auto_buf(sizeof(trace_info_t) 
		+ sizeof(trace_info_t::hit_t) * (result.nHitCount - 1));
	char* buffer = auto_buf.GetBuffer();

	memset(buffer, 0, sizeof(buffer));

	trace_info_t* ti = (trace_info_t*)buffer;

	ti->nHitMax = result.nHitCount;
	ti->nHitCount = 0;
	ti->fDistance = 1e8;

	CVar prop;

	for (int t = 0; t < result.nHitCount; ++t)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			result.Hits[t].TraceID);

		//Assert(NULL != pVisBase);
		if (pVisBase)
		{
			GetCore()->GetProperty(pVisBase, "TraceMainModel", prop);

			bool old_trace = prop.BoolVal();

			if (old_trace)
			{
				GetCore()->SetProperty(pVisBase, "TraceMainModel", 
					CVar(VTYPE_BOOL, false));

				pVisBase->TraceDetail(src, dst, *ti);

				GetCore()->SetProperty(pVisBase, "TraceMainModel", 
					CVar(VTYPE_BOOL, old_trace));
			}
		}
	}

	if (ti->nHitCount < 1)
	{
//		m_pRender->SetTraceHideModel(old_trace_hide_model);
		return;
	}

	// 排序碰撞点
	qsort(ti->Hits, ti->nHitCount, sizeof(trace_info_t::hit_t), 
		trace_hit_compare);

	float dist = 1e8;
	int nearest = -1;

	for (int k = 0; k < ti->nHitCount; ++k)
	{
		for (int j = 0; j < result.nHitCount; ++j)
		{
			if (result.Hits[j].TraceID == ti->Hits[k].TraceID)
			{
				if (ti->Hits[k].fDistance < dist)
				{
					nearest = k;
					dist = ti->Hits[k].fDistance;
				}
				break;
			}
		}
	}

	if (nearest >= 0)
	{
		result.nHitCount = 1;
		result.fDistance = dist;
		memcpy(&result.Hits[0], &ti->Hits[nearest], 
			sizeof(trace_info_t::hit_t));
	}

//	m_pRender->SetTraceHideModel(old_trace_hide_model);
}

bool Terrain::TraceDetail(const FmVec3& src, const FmVec3& dst,
	trace_info_t& result)
{
#ifdef PERFORMANCE_DEBUG
	double dTraceTime = performance_time();
#endif

	bool ret = m_pCollide->TraceDetail(src, dst, result);

	if (m_bVerifyTraceResult)
	{
		VerifyTraceResult(src, dst, result);
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dRayTraceTime +=
		performance_time() - dTraceTime;
#endif

	return ret;
}

bool Terrain::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
#ifdef PERFORMANCE_DEBUG
	double dTraceTime = performance_time();
#endif

	bool ret = m_pCollide->TraceHitted(src, dst);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dRayTraceTime +=
		performance_time() - dTraceTime;
#endif

	return ret;
}

bool Terrain::TraceSphere(const FmVec3& center, float radius)
{
#ifdef PERFORMANCE_DEBUG
	double dTraceTime = performance_time();
#endif

	bool ret = m_pCollide->TraceSphere(center, radius);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dSphereTraceTime +=
		performance_time() - dTraceTime;
#endif

	return ret;
}

bool Terrain::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
#ifdef PERFORMANCE_DEBUG
	double dCullTime = performance_time();
#endif

	bool ret = m_pCollide->Cull(planes, plane_num, center, radius, cull_func, 
		context);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dCullTime +=
		performance_time() - dCullTime;
#endif

	return ret;
}

PERSISTID Terrain::GetEditorID()
{
	if (NULL == m_pEditor)
	{
		m_pEditor = (TerrainEditor*)GetCore()->CreateEntityArgs("TerrainEditor", CVarList());
		m_pEditor->SetTerrain(this);
	}

	return m_pEditor->GetID();
}

PERSISTID Terrain::GetBrushID()
{
	if (NULL == m_pBrush)
	{
		m_pBrush = (TerrainBrush*)GetCore()->CreateEntityArgs("TerrainBrush", CVarList());
		m_pBrush->SetTerrain(this);
	}
	
	return m_pBrush->GetID();
}

PERSISTID Terrain::GetMaterialID()
{
	if (NULL == m_pMaterial)
	{
		m_pMaterial = (TerrainMaterial*)GetCore()->CreateEntityArgs("TerrainMaterial", CVarList());
		m_pMaterial->SetTerrain(this);
	}
	
	return m_pMaterial->GetID();
}

void Terrain::SetWaterVisible(bool value)
{
	m_bWaterVisible = value;
//	m_pRender->SetShowWaterModel(value);
}

void Terrain::SetUnitSize(float value)
{ 
	int v = int(value);

	if ((v != 1) && (v != 2) && (v != 4))
	{
		return;
	}
	
	m_fUnitSize = float(v);
	m_fInvUnitSize = 1.0F / m_fUnitSize;
	m_fCollideSize = m_fUnitSize / m_nCollidePerUnit;
	m_fInvCollideSize = 1.0F / m_fCollideSize;
}

void Terrain::SetLightModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}

	SAFE_RELEASE(m_pLightModel);
	m_pLightModel = pVisBase;
}

PERSISTID Terrain::GetLightModelID() const
{
	if (NULL == m_pLightModel)
	{
		return PERSISTID();
	}
	
	return m_pLightModel->GetID();
}

void Terrain::SetSoundModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);
	
	if (NULL == pVisBase)
	{
		return;
	}
	
	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}
	
	SAFE_RELEASE(m_pSoundModel);
	m_pSoundModel = pVisBase;
}

PERSISTID Terrain::GetSoundModelID() const
{
	if (NULL == m_pSoundModel)
	{
		return PERSISTID();
	}

	return m_pSoundModel->GetID();
}

void Terrain::SetSoundRangeModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}

	SAFE_RELEASE(m_pSoundRangeModel);
	m_pSoundRangeModel = pVisBase;
}

PERSISTID Terrain::GetSoundRangeModelID() const
{
	if (NULL == m_pSoundRangeModel)
	{
		return PERSISTID();
	}

	return m_pSoundRangeModel->GetID();
}

void Terrain::SetParticleModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}

	SAFE_RELEASE(m_pParticleModel);
	m_pParticleModel = pVisBase;
}

PERSISTID Terrain::GetParticleModelID() const
{
	if (NULL == m_pParticleModel)
	{
		return PERSISTID();
	}

	return m_pParticleModel->GetID();
}

void Terrain::SetEffectModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}

	SAFE_RELEASE(m_pEffectModel);
	m_pEffectModel = pVisBase;
}

PERSISTID Terrain::GetEffectModelID() const
{
	if (NULL == m_pEffectModel)
	{
		return PERSISTID();
	}

	return m_pEffectModel->GetID();
}

void Terrain::SetTriggerModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}

	SAFE_RELEASE(m_pTriggerModel);
	m_pTriggerModel = pVisBase;
}

PERSISTID Terrain::GetTriggerModelID() const
{
	if (NULL == m_pTriggerModel)
	{
		return PERSISTID();
	}

	return m_pTriggerModel->GetID();
}

void Terrain::SetTreeModelID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}

	SAFE_RELEASE(m_pTreeModel);
	m_pTreeModel = pVisBase;
}

PERSISTID Terrain::GetTreeModelID() const
{
	if (NULL == m_pTreeModel)
	{
		return PERSISTID();
	}

	return m_pTreeModel->GetID();
}

int Terrain::GetZoneRows() const
{
	return (int)m_pZoneManager->GetRows();
}

int Terrain::GetZoneCols() const
{
	return (int)m_pZoneManager->GetCols();
}

int Terrain::GetZoneOriginRow() const
{
	return (int)m_pZoneManager->GetOriginRow();
}

int Terrain::GetZoneOriginCol() const
{
	return (int)m_pZoneManager->GetOriginCol();
}

int Terrain::GetZoneRange() const
{
	return (int)m_pZoneManager->GetZoneRange();
}

bool Terrain::AddTexturePath(const char* value)
{
	Assert(value != NULL);
	
	const char* s = value;
	const char* p = strchr(value, '|');
	
	while (p != NULL)
	{
		result_string path(s, p - s);
		
		if (!path.empty())
		{
			m_TexPaths.AddName(path.c_str());
		}
		
		s = p + 1;
		p = strchr(s, '|');
	}
	
	if (!StringEmpty(s))
	{
		m_TexPaths.AddName(s);
	}

	m_strTexPath = "";
	
	for (size_t i = 0; i < m_TexPaths.GetCount(); ++i)
	{
		if (i > 0)
		{
			m_strTexPath += "|";
		}

		m_strTexPath += m_TexPaths.GetByIndex(i);
	}

	return true;
}

bool Terrain::ClearTexturePath()
{
	m_TexPaths.Clear();
	m_strTexPath = "";
	
	return true;
}

int Terrain::GetTexturePathCount() const
{
	return (int)m_TexPaths.GetCount();
}

void Terrain::GetTexturePathList(const IVarList& args, 
	IVarList& result) const
{
	result.Clear();
	
	size_t SIZE1 = m_TexPaths.GetCount();
	
	for (size_t i = 0; i < SIZE1; ++i)
	{
		result.AddString(m_TexPaths.GetByIndex(i));
	}
}

const char* Terrain::GetTexturePathByIndex(size_t index) const
{
	Assert(index < m_TexPaths.GetCount());
	
	return m_TexPaths.GetByIndex(index);
}

int Terrain::GetBaseTexIndex(const char* name)
{
	size_t index;
	
	if (!m_pBaseTexSet->FindIndex(name, &index))
	{
		return -1;
	}
	
	return (int)index;
}

bool Terrain::FindBaseTex(const char* name)
{
	return m_pBaseTexSet->Find(name);
}

bool Terrain::AddBaseTex(const char* name, const char* path)
{
	if (m_pBaseTexSet->GetCount() >= MAX_TEX_NUM)
	{
		return false;
	}
	
	return m_pBaseTexSet->Add(name, path);
}

bool Terrain::ClearBaseTex()
{
	m_pBaseTexSet->ReleaseAll();

	return true;
}

int Terrain::GetBaseTexCount()
{
	return (int)m_pBaseTexSet->GetCount();
}

void Terrain::GetBaseTexList(const IVarList& args, IVarList& result)
{
	result.Clear();

	for (size_t i = 0; i < m_pBaseTexSet->GetCount(); ++i)
	{
		result.AddString(m_pBaseTexSet->GetByIndex(i)->GetName());
	}
}

const char* Terrain::GetBaseTexName(int index)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return "";
	}

	return m_pBaseTexSet->GetByIndex(index)->GetName();
}

const char* Terrain::GetBaseTexPath(int index)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return "";
	}
	
	return m_pBaseTexSet->GetByIndex(index)->GetPath();
}

bool Terrain::SetBaseTexSpecularLevel(int index, float value)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return false;
	}

	m_pBaseTexSet->GetByIndex(index)->SetSpecularLevel(value);

	return true;
}

float Terrain::GetBaseTexSpecularLevel(int index)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return 0.0F;
	}

	return m_pBaseTexSet->GetByIndex(index)->GetSpecularLevel();
}

bool Terrain::SetBaseTexSpecularPower(int index, float value)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return false;
	}

	m_pBaseTexSet->GetByIndex(index)->SetSpecularPower(value);

	return true;
}

float Terrain::GetBaseTexSpecularPower(int index)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return 0.0F;
	}

	return m_pBaseTexSet->GetByIndex(index)->GetSpecularPower();
}

bool Terrain::SetBaseTexHeightScale(int index, float value)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return false;
	}

	m_pBaseTexSet->GetByIndex(index)->SetHeightScale(value);

	return true;
}

float Terrain::GetBaseTexHeightScale(int index)
{
	if (size_t(index) >= m_pBaseTexSet->GetCount())
	{
		return 0.0F;
	}

	return m_pBaseTexSet->GetByIndex(index)->GetHeightScale();
}


int Terrain::GetBaseFormatScaleU(const char* name, int index)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return 0;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}

	return p->GetFormatByIndex(index)->nScaleU;
}

int Terrain::GetBaseFormatScaleV(const char* name, int index)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return 0;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}

	return p->GetFormatByIndex(index)->nScaleV;
}

int Terrain::GetBaseFormatAngleX(const char* name, int index)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return 0;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}

	return p->GetFormatByIndex(index)->nAngleX;
}

int Terrain::GetBaseFormatAngleY(const char* name, int index)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return 0;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}

	return p->GetFormatByIndex(index)->nAngleY;
}

int Terrain::GetBaseFormatAngleZ(const char* name, int index)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return 0;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}

	return p->GetFormatByIndex(index)->nAngleZ;
}

bool Terrain::SetBaseFormatScaleU(const char* name, int index, int value)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return false;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}

	p->GetFormatByIndex(index)->nScaleU = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();

	return true;
}

bool Terrain::SetBaseFormatScaleV(const char* name, int index, int value)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return false;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}

	p->GetFormatByIndex(index)->nScaleV = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();

	return true;
}

bool Terrain::SetBaseFormatAngleX(const char* name, int index, int value)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return false;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}

	p->GetFormatByIndex(index)->nAngleX = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();

	return true;
}

bool Terrain::SetBaseFormatAngleY(const char* name, int index, int value)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return false;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}

	p->GetFormatByIndex(index)->nAngleY = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();

	return true;
}

bool Terrain::SetBaseFormatAngleZ(const char* name, int index, int value)
{
	size_t k;

	if (!m_pBaseTexSet->FindIndex(name, &k))
	{
		return false;
	}

	CBlendTex* p = m_pBaseTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}

	p->GetFormatByIndex(index)->nAngleZ = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();

	return true;
}

int Terrain::GetBlendTexIndex(const char* name)
{
	size_t index;

	if (!m_pBlendTexSet->FindIndex(name, &index))
	{
		return -1;
	}

	return (int)index;
}

bool Terrain::FindBlendTex(const char* name)
{
	return m_pBlendTexSet->Find(name);
}

bool Terrain::AddBlendTex(const char* name, const char* path)
{
	if (m_pBlendTexSet->GetCount() >= MAX_TEX_NUM)
	{
		return false;
	}
	
	return m_pBlendTexSet->Add(name, path);
}

bool Terrain::ClearBlendTex()
{
	m_pBlendTexSet->ReleaseAll();
	
	return true;
}

int Terrain::GetBlendTexCount()
{
	return (int)m_pBlendTexSet->GetCount();
}

void Terrain::GetBlendTexList(const IVarList& args, IVarList& result)
{
	result.Clear();
	
	for (size_t i = 0; i < m_pBlendTexSet->GetCount(); ++i)
	{
		result.AddString(m_pBlendTexSet->GetByIndex(i)->GetName());
	}
}

const char* Terrain::GetBlendTexName(int index)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return "";
	}
	
	return m_pBlendTexSet->GetByIndex(index)->GetName();
}

const char* Terrain::GetBlendTexPath(int index)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return "";
	}
	
	return m_pBlendTexSet->GetByIndex(index)->GetPath();
}

bool Terrain::SetBlendTexSpecularLevel(int index, float value)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return false;
	}

	m_pBlendTexSet->GetByIndex(index)->SetSpecularLevel(value);

	return true;
}

float Terrain::GetBlendTexSpecularLevel(int index)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return 0.0F;
	}

	return m_pBlendTexSet->GetByIndex(index)->GetSpecularLevel();
}

bool Terrain::SetBlendTexSpecularPower(int index, float value)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return false;
	}

	m_pBlendTexSet->GetByIndex(index)->SetSpecularPower(value);

	return true;
}

float Terrain::GetBlendTexSpecularPower(int index)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return 0.0F;
	}

	return m_pBlendTexSet->GetByIndex(index)->GetSpecularPower();
}

bool Terrain::SetBlendTexHeightScale(int index, float value)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return false;
	}

	m_pBlendTexSet->GetByIndex(index)->SetHeightScale(value);

	return true;
}

float Terrain::GetBlendTexHeightScale(int index)
{
	if (size_t(index) >= m_pBlendTexSet->GetCount())
	{
		return 0.0F;
	}

	return m_pBlendTexSet->GetByIndex(index)->GetHeightScale();
}

bool Terrain::AddBlendFormat(const char* name, int scale_u, int scale_v, 
	int angle_x, int angle_y, int angle_z)
{
	size_t index;
	
	if (!m_pBlendTexSet->FindIndex(name, &index))
	{
		return false;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(index);

	if (!p->AddFormat(scale_u, scale_v, angle_x, angle_y, angle_z))
	{
		return false;
	}

	p->UpdateMatrix(p->GetFormatCount() - 1, this);

	return true;
}

int Terrain::GetBlendFormatCount(const char* name)
{
	size_t index;

	if (!m_pBlendTexSet->FindIndex(name, &index))
	{
		return 0;
	}

	CBlendTex* p = m_pBlendTexSet->GetByIndex(index);

	return (int)p->GetFormatCount();
}

int Terrain::GetBlendFormatScaleU(const char* name, int index)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return 0;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);

	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}
	
	return p->GetFormatByIndex(index)->nScaleU;
}

int Terrain::GetBlendFormatScaleV(const char* name, int index)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return 0;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}
	
	return p->GetFormatByIndex(index)->nScaleV;
}

int Terrain::GetBlendFormatAngleX(const char* name, int index)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return 0;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}
	
	return p->GetFormatByIndex(index)->nAngleX;
}

int Terrain::GetBlendFormatAngleY(const char* name, int index)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return 0;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}
	
	return p->GetFormatByIndex(index)->nAngleY;
}

int Terrain::GetBlendFormatAngleZ(const char* name, int index)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return 0;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return 0;
	}
	
	return p->GetFormatByIndex(index)->nAngleZ;
}

bool Terrain::SetBlendFormatScaleU(const char* name, int index, int value)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return false;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}
	
	p->GetFormatByIndex(index)->nScaleU = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();

	return true;
}

bool Terrain::SetBlendFormatScaleV(const char* name, int index, int value)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return false;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}
	
	p->GetFormatByIndex(index)->nScaleV = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();
	
	return true;
}

bool Terrain::SetBlendFormatAngleX(const char* name, int index, int value)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return false;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}
	
	p->GetFormatByIndex(index)->nAngleX = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();
	
	return true;
}

bool Terrain::SetBlendFormatAngleY(const char* name, int index, int value)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return false;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}
	
	p->GetFormatByIndex(index)->nAngleY = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();
	
	return true;
}

bool Terrain::SetBlendFormatAngleZ(const char* name, int index, int value)
{
	size_t k;
	
	if (!m_pBlendTexSet->FindIndex(name, &k))
	{
		return false;
	}
	
	CBlendTex* p = m_pBlendTexSet->GetByIndex(k);
	
	if (size_t(index) >= p->GetFormatCount())
	{
		return false;
	}
	
	p->GetFormatByIndex(index)->nAngleZ = value;
	p->UpdateMatrix(index, this);
	m_pZoneManager->ReleaseAllChunkCB();
	
	return true;
}

void Terrain::SetGlobalEnvironmentMap(const char* name)
{
	IRenderContext* pContext = this->GetContext();

	if (NULL == pContext)
	{
		return;
	}

//	pContext->SetGlobalEnvironmentMap(name);
}

const char* Terrain::GetGlobalEnvironmentMap() const
{
	IRenderContext* pContext = this->GetContext();

	if (NULL == pContext)
	{
		return "";
	}

	return NULL;//pContext->GetGlobalEnvironmentMap();
}

bool Terrain::SetFocusPosition(float x, float y, float z)
{
	IRenderContext* pContext = this->GetContext();

	if (NULL == pContext)
	{
		return false;
	}

	pContext->SetVector(IRenderContext::V_FOCUS_POSITION, FmVec3(x, y, z));

	return true;
}

bool Terrain::SetFocusRadius(float x, float y, float z)
{
	IRenderContext* pContext = this->GetContext();

	if (NULL == pContext)
	{
		return false;
	}

	pContext->SetVector(IRenderContext::V_FOCUS_RADIUS, FmVec3(x, y, z));

	return true;
}

PERSISTID Terrain::CreateWater(const char* name)
{
	SeaWater* pWater = m_pWater->Create(name);

	if (NULL == pWater)
	{
		return PERSISTID();
	}

	return pWater->GetID();
}

PERSISTID Terrain::GetWater(const char* name)
{
	size_t index;

	if (!m_pWater->FindIndex(name, index))
	{
		return PERSISTID();
	}

	SeaWater* pWater = m_pWater->GetByIndex(index);

	return pWater->GetID();
}

bool Terrain::DeleteWater(const char* name)
{
	size_t index;
	
	if (!m_pWater->FindIndex(name, index))
	{
		return false;
	}
	
	return m_pWater->DeleteByIndex(index);
}

bool Terrain::ClearWater()
{
	m_pWater->Clear();

	return true;
}

const char* Terrain::GetWaterName(int index)
{
	if (size_t(index) >= m_pWater->GetCount())
	{
		return "";
	}

	SeaWater* pWater = m_pWater->GetByIndex(index);

	return pWater->GetName();
}

int Terrain::GetWaterIndex(const char* name)
{
	size_t index;
	
	if (!m_pWater->FindIndex(name, index))
	{
		return -1;
	}
	
	return (int)index;
}

int Terrain::GetWaterCount() const
{
	return (int)m_pWater->GetCount();
}

void Terrain::GetWaterList(const IVarList& args, IVarList& result) const
{
	result.Clear();
	
	const size_t SIZE1 = m_pWater->GetCount();

	for (size_t i = 0; i < SIZE1; ++i)
	{
		result.AddObject(m_pWater->GetByIndex(i)->GetID());
	}
}

bool Terrain::RefreshWater()
{
	return true;
}

bool Terrain::SetWaterCubeMapStatic(bool value)
{
	const size_t SIZE1 = m_pWater->GetCount();

	for (size_t i = 0; i < SIZE1; ++i)
	{
		m_pWater->GetByIndex(i)->SetCubeMapStatic(value);
	}

	return true;
}

bool Terrain::DumpWaterCubeMap(int index, const char* file_name)
{
	if (size_t(index) >= m_pWater->GetCount())
	{
		return false;
	}

	return m_pWater->GetByIndex(index)->GetSeaBase()->DumpReflectMap(file_name);
}

PERSISTID Terrain::CreateGrass(const char* name)
{
	GrassType* pGrass = m_pGrass->Create(name);
	
	if (NULL == pGrass)
	{
		return PERSISTID();
	}
	
	return pGrass->GetID();
}

PERSISTID Terrain::GetGrass(const char* name)
{
	size_t index;
	
	if (!m_pGrass->FindIndex(name, index))
	{
		return PERSISTID();
	}
	
	GrassType* pGrass = m_pGrass->GetByIndex(index);
	
	return pGrass->GetID();
}

bool Terrain::DeleteGrass(const char* name)
{
	size_t index;
	
	if (!m_pGrass->FindIndex(name, index))
	{
		return false;
	}
	
	return m_pGrass->DeleteByIndex(index);
}

bool Terrain::ClearGrass()
{
	m_pGrass->Clear();

	return true;
}

const char* Terrain::GetGrassName(int index)
{
	if (size_t(index) >= m_pGrass->GetCount())
	{
		return "";
	}
	
	GrassType* pGrass = m_pGrass->GetByIndex(index);
	
	return pGrass->GetName();
}

int Terrain::GetGrassIndex(const char* name)
{
	size_t index;
	
	if (!m_pGrass->FindIndex(name, index))
	{
		return -1;
	}
	
	return (int)index;
}

int Terrain::GetGrassCount() const
{
	return (int)m_pGrass->GetCount();
}

void Terrain::GetGrassList(const IVarList& args, IVarList& result) const
{
	result.Clear();
	
	const size_t SIZE1 = m_pGrass->GetCount();
	
	for (size_t i = 0; i < SIZE1; ++i)
	{
		result.AddObject(m_pGrass->GetByIndex(i)->GetID());
	}
}

bool Terrain::RefreshGrass()
{
	const size_t SIZE1 = m_pGrass->GetCount();
	
	for (size_t i = 0; i < SIZE1; ++i)
	{
		m_pGrass->GetByIndex(i)->SetMustRefresh(true);
	}
	
	return true;
}

bool Terrain::RefreshGround()
{
	return true;
}

bool Terrain::RefreshVisual()
{
	return true;
}

static unsigned int get_track_mask_type(const char* type)
{
	if (stricmp(type, "Ground") == 0)
	{
		return Terrain::TRACEMASK_GROUND;
	}
	else if (stricmp(type, "Model") == 0)
	{
		return Terrain::TRACEMASK_MODEL;
	}
	else if (stricmp(type, "Particle") == 0)
	{
		return Terrain::TRACEMASK_PARTICLE;
	}
	else if (stricmp(type, "Light") == 0)
	{
		return Terrain::TRACEMASK_LIGHT;
	}
	else if (stricmp(type, "Sound") == 0)
	{
		return Terrain::TRACEMASK_SOUND;
	}
	else if (stricmp(type, "Actor") == 0)
	{
		return Terrain::TRACEMASK_ACTOR;
	}
	else if (stricmp(type, "Trigger") == 0)
	{
		return Terrain::TRACEMASK_TRIGGER;
	}
	else if (stricmp(type, "Role") == 0)
	{
		return Terrain::TRACEMASK_ROLE;
	}
	else if (stricmp(type, "Helper") == 0)
	{
		return Terrain::TRACEMASK_HELPER;
	}
	else if (stricmp(type, "Through") == 0)
	{
		return Terrain::TRACEMASK_THROUGH;
	}
	else if (stricmp(type, "EffectModel") == 0)
	{
		return Terrain::TRACEMASK_EFFECTMODEL;
	}

	return 0;
}

bool Terrain::SetTraceMask(const char* type, bool value)
{
	Assert(type != NULL);
	
	unsigned int mask = get_track_mask_type(type);
	
	if (0 == mask)
	{
		return false;
	}

	if (value)
	{
		m_nTraceMask |= mask;
	}
	else
	{
		m_nTraceMask &= ~mask;
	}

	return true;
}

bool Terrain::GetTraceMask(const char* type)
{
	Assert(type != NULL);
	
	unsigned int mask = get_track_mask_type(type);

	if (0 == mask)
	{
		return false;
	}

	return (m_nTraceMask & mask) > 0;
}

bool Terrain::SetRegionColor(const char* name, unsigned int value)
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_RegionColors.size(); ++i)
	{
		if (strcmp(m_RegionColors[i].pName, name) == 0)
		{
			m_RegionColors[i].nColor = value;
			return true;
		}
	}

	m_RegionColors.push_back(area_color_t());

	area_color_t& data = m_RegionColors.back();
	size_t name_size = strlen(name) + 1;
	char* pName = (char*)CORE_ALLOC(name_size);

	memcpy(pName, name, name_size);
	data.pName = pName;
	data.nColor = value;

	return true;
}

unsigned int Terrain::GetRegionColor(const char* name) const
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_RegionColors.size(); ++i)
	{
		if (strcmp(m_RegionColors[i].pName, name) == 0)
		{
			return m_RegionColors[i].nColor;
		}
	}

	return 0;
}

void Terrain::GetRegionList(const IVarList& args, IVarList& result)
{
	result.Clear();

	for (size_t i = 0; i < m_RegionColors.size(); ++i)
	{
		result.AddString(m_RegionColors[i].pName);
	}
}

bool Terrain::GetRegionEnable(const char* name, float x, float z)
{
	Assert(name != NULL);

	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvUnitSize);
	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvUnitSize);

	return pZone->GetRegion()->GetRegionEnable(name, row, col);
}

bool Terrain::ChangeRegionName(const char* ori_name, const char* new_name)
{
	Assert(ori_name != NULL);

	for (size_t i = 0; i < m_RegionColors.size(); ++i)
	{
		if (strcmp(m_RegionColors[i].pName, ori_name) == 0)
		{
			size_t name_size = strlen(new_name) + 1;
			char* pName = (char*)CORE_ALLOC(name_size);
			char* old_name = m_RegionColors[i].pName;

			memcpy(pName, new_name, name_size);
			CORE_FREE(old_name, strlen(old_name) + 1);
			m_RegionColors[i].pName = pName;
			return true;
		}
	}

	return false;
}

bool Terrain::SetAreaColor(const char* name, unsigned int value)
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_AreaColors.size(); ++i)
	{
		if (strcmp(m_AreaColors[i].pName, name) == 0)
		{
			m_AreaColors[i].nColor = value;
			return true;
		}
	}

	m_AreaColors.push_back(area_color_t());

	area_color_t& data = m_AreaColors.back();
	size_t name_size = strlen(name) + 1;
	char* pName = (char*)CORE_ALLOC(name_size);

	memcpy(pName, name, name_size);
	data.pName = pName;
	data.nColor = value;

	return true;
}

unsigned int Terrain::GetAreaColor(const char* name) const
{
	Assert(name != NULL);
	
	for (size_t i = 0; i < m_AreaColors.size(); ++i)
	{
		if (strcmp(m_AreaColors[i].pName, name) == 0)
		{
			return m_AreaColors[i].nColor;
		}
	}

	return 0;
}

void Terrain::GetAreaList(const IVarList& args, IVarList& result)
{
	result.Clear();

	for (size_t i = 0; i < m_AreaColors.size(); ++i)
	{
		result.AddString(m_AreaColors[i].pName);
	}
}

const char* Terrain::GetAreaName(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return "";
	}

	int row = VisUtil_FloatToInt((z - pZone->GetTop()) * m_fInvUnitSize);
	int col = VisUtil_FloatToInt((x - pZone->GetLeft()) * m_fInvUnitSize);

	return pZone->GetRegion()->GetValidArea(row, col);
}

bool Terrain::ChangeAreaName(const char* ori_name, const char* new_name)
{
	Assert(ori_name != NULL);

	for (size_t i = 0; i < m_AreaColors.size(); ++i)
	{
		if (strcmp(m_AreaColors[i].pName, ori_name) == 0)
		{
			size_t name_size = strlen(new_name) + 1;
			char* pName = (char*)CORE_ALLOC(name_size);
			char* old_name = m_AreaColors[i].pName;

			memcpy(pName, new_name, name_size);
			CORE_FREE(old_name, strlen(old_name) + 1);
			m_AreaColors[i].pName = pName;
			return true;
		}
	}

	return false;
}

bool Terrain::RemoveAreaColor(const char* name)
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_AreaColors.size(); ++i)
	{
		if (strcmp(m_AreaColors[i].pName, name) == 0)
		{
			char* pName = m_AreaColors[i].pName;

			CORE_FREE(pName, strlen(pName) + 1);
			m_AreaColors.remove(i);
			return true;
		}
	}

	return false;
}

bool Terrain::IsViewClip(float x, float y, float z, float radius)
{
	FmVec3 pos(x, y, z);

	return m_pContext->IsViewClip(pos, radius);
}

bool Terrain::IsHorizonClip(const FmVec3& v_min, 
	const FmVec3& v_max)
{
	if (!m_bHorizontalCulling)
	{
		return false;
	}

	return m_pCulling->IsHorizonClip(v_min, v_max);
}

bool Terrain::DumpHorizon(const char* file_name)
{
	return m_pCulling->DumpHorizon(file_name);
}

bool Terrain::DumpZones(const char* file_name)
{
	return m_pZoneManager->DumpZones(file_name);
}

size_t get2n(size_t value)
{
	size_t ret = 1;

	while (value)
	{
		value >>= 1;
		ret <<= 1;
	}

	return ret;
}

void extern_to_2n(size_t& row, size_t& col)
{
	size_t max = row;
	if (max < col)
	{
		max = col;
	}

	max = get2n(max);

	if (max < 4)
	{
		max = 4;
	}

	row = col = max;
}

bool Terrain::AddDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);
	Assert(NULL != m_pDynamicWalkableManager);

	return m_pDynamicWalkableManager->AddDynamicWalkable(pVisBase);
}

bool Terrain::DeleteDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);
	Assert(NULL != m_pDynamicWalkableManager);

	return m_pDynamicWalkableManager->DeleteDynamicWalkable(pVisBase);
}

bool Terrain::LoadDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);
	Assert(NULL != m_pDynamicWalkableManager);

	return m_pDynamicWalkableManager->LoadDynamicWalkable(pVisBase);
}

bool Terrain::UnloadDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);
	Assert(NULL != m_pDynamicWalkableManager);

	return m_pDynamicWalkableManager->UnloadDynamicWalkable(pVisBase);
}

bool Terrain::AddDynamicWalkable(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	return AddDynamicWalkable(pVisBase);
}

bool Terrain::DeleteDynamicWalkable(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	return DeleteDynamicWalkable(pVisBase);
}

bool Terrain::LoadDynamicWalkable(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	return LoadDynamicWalkable(pVisBase);
}

bool Terrain::UnloadDynamicWalkable(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	return UnloadDynamicWalkable(pVisBase);
}

void prepare_calc_local_pos(float& x, float& z, float angleY)
{
	FmVec3 vec3_offset0;
	vec3_offset0.x = x;
	vec3_offset0.y = 0.0f;
	vec3_offset0.z = z;

	FmMat4 mat;

	FmMat4Identity(&mat);
	FmMat4RotationY(&mat, angleY);

	FmMat4Inverse(&mat,NULL, &mat);

	FmVec4 result_vec4;
	FmVec3Transform(&result_vec4, &vec3_offset0, &mat);

	x = result_vec4.x;
	z = result_vec4.z;
}

void get_local_pos(IVisBase* pVisual, size_t collide_per_unit, float gx,
				   float gz, float angleY, int& row, int& col)
{
	float ox = gx - pVisual->GetCenterX();
	float oz = gz - pVisual->GetCenterZ();

	float angleY_offset = pVisual->GetAngleY() - angleY;

	prepare_calc_local_pos(ox, oz, angleY_offset);

	float c = ((ox + pVisual->GetBoxSizeX()/2) * collide_per_unit);
	float r = ((oz + pVisual->GetBoxSizeZ()/2) * collide_per_unit);
	if (c < 0.0f)
	{
		c = -1.0f;
	}

	if (r < 0.0f)
	{
		r = -1.0f;
	}
	col = (int)c;
	row = (int)r;
}

void prepare_calc_pos(float& x, float& z, float angleY)
{
	FmVec3 vec3_offset0;
	vec3_offset0.x = x;
	vec3_offset0.y = 0.0f;
	vec3_offset0.z = z;

	FmMat4 mat;

	FmMat4Identity(&mat);
	FmMat4RotationY(&mat, angleY);

	FmVec4 result_vec4;
	FmVec3Transform(&result_vec4, &vec3_offset0, &mat);

	x = result_vec4.x;
	z = result_vec4.z;
}

void get_pos(IVisBase* pVisBase, size_t collide_per_unit, int row, int col,
			 float angleY, float& gx, float& gz)
{
	float lx = (col + 0.5f) / collide_per_unit;
	float lz = (row + 0.5f) / collide_per_unit;

	float ox = lx - pVisBase->GetBoxSizeX() / 2.0f;
	float oz = lz - pVisBase->GetBoxSizeZ() / 2.0f;

	float angleY_offset = pVisBase->GetAngleY() - angleY;

	prepare_calc_pos(ox, oz, angleY_offset);

	gx = pVisBase->GetCenterX() + ox;
	gz = pVisBase->GetCenterZ() + oz;
}
