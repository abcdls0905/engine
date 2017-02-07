//--------------------------------------------------------------------
// 文件名:		sea_base.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "sea_base.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "terrain_painter.h"
#include "zone_manager.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../visual/i_scene_view.h"
#include "../public/var.h"
#include "../public/core_log.h"
 
// CSeaBase

CSeaBase::CSeaBase()
{
	m_pTerrain = NULL;
	m_pRender = NULL;
	m_pContext = NULL;
	m_fBaseHeight = 10.0F;
	m_fMaxSeaHeight = 2.0F;
	m_bVisible = true;
	m_bUnderWater = false;
	m_nIndex = 0;
	m_fTimes = 0.0F;
	m_fNearCameraDistance = 0.0F;
	m_nReflectNoFlush = 0;
	m_pCurEnvMapTex = NULL;
	m_pSeaReflectionTex = NULL;
	m_pSeaZBuffer = NULL;
	m_pSeaZReflectionTex = NULL;
	m_pWaterNormalMap1 = NULL;
	m_pWaterNormalMap2 = NULL;
	m_pWaterNormalMap3 = NULL;
	m_pWaterNormalMap4 = NULL;
	m_pWhiteWaterMap = NULL;
	m_pCausticMap = NULL;
	m_pNoiseMap = NULL;
	m_pCausticVS = NULL;
	memset(&m_SeaData, 0, sizeof(m_SeaData));
	m_nRenderBlockNum = 0;
	memset(m_RenderBlocks, 0, sizeof(m_RenderBlocks));

	m_pSeaSceneView = NULL;
}

CSeaBase::~CSeaBase()
{
	ReleaseAll();
}

void CSeaBase::ReleaseAll()
{
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pSeaReflectionTex);
	SAFE_RELEASE(m_pSeaZBuffer);
	SAFE_RELEASE(m_pSeaZReflectionTex);
	SAFE_RELEASE(m_pWaterNormalMap1);
	SAFE_RELEASE(m_pWaterNormalMap2);
	SAFE_RELEASE(m_pWaterNormalMap3);
	SAFE_RELEASE(m_pWaterNormalMap4);
	SAFE_RELEASE(m_pWhiteWaterMap);
	SAFE_RELEASE(m_pCausticMap);
	SAFE_RELEASE(m_pNoiseMap);
	SAFE_RELEASE(m_pSeaSceneView);

#define shadermap 	TPodHashMap<unsigned int, CTerrainChunk::chunk_water_shader_handle_t*, TPodTraits<unsigned int>,TTerrainSeaBaseShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMapHandle.Begin(); iter != m_ShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(CTerrainChunk::chunk_water_shader_handle_t));
	}    
#undef  shadermap

}

bool CSeaBase::CreateNormalMap(const char* pszFileName1, 
	const char* pszFileName2, const char* pszFileName3, 
	const char* pszFileName4)
{
	Assert(pszFileName1 != NULL);
	Assert(pszFileName2 != NULL);
	Assert(pszFileName3 != NULL);
	Assert(pszFileName4 != NULL);
	
	SAFE_RELEASE(m_pWaterNormalMap1);
	SAFE_RELEASE(m_pWaterNormalMap2);
	SAFE_RELEASE(m_pWaterNormalMap3);
	SAFE_RELEASE(m_pWaterNormalMap4);

	result_string strTexPath = m_pRender->GetCore()->GetResourcePath();

	m_pWaterNormalMap1 = m_pRender->CreateTexture(
		(strTexPath + pszFileName1).c_str(), true);
	m_pWaterNormalMap2 = m_pRender->CreateTexture(
		(strTexPath + pszFileName2).c_str(), true);
	m_pWaterNormalMap3 = m_pRender->CreateTexture(
		(strTexPath + pszFileName3).c_str(), true);
	m_pWaterNormalMap4 = m_pRender->CreateTexture(
		(strTexPath + pszFileName4).c_str(), true);

	if ((NULL == m_pWaterNormalMap1) || (NULL == m_pWaterNormalMap2) 
		|| (NULL == m_pWaterNormalMap3) || (NULL == m_pWaterNormalMap4))
	{
		CORE_TRACE("(CSeaBase::CreateNormalMap)failed");
		return false;
	}

	return true;
}

bool CSeaBase::CreateWhiteWaterMap(const char* pszFileName)
{
	Assert(pszFileName != NULL);
	
	SAFE_RELEASE(m_pWhiteWaterMap);

	if (0 == pszFileName[0])
	{
		// 当前水不开启白色水域效果
		return true;
	}

	result_string strTexPath = m_pRender->GetCore()->GetResourcePath();

	m_pWhiteWaterMap = m_pRender->CreateTexture(
		(strTexPath + pszFileName).c_str(), true);

	if (NULL == m_pWhiteWaterMap)
	{
		CORE_TRACE("(CSeaBase::CreateWhiteWaterMap)failed");
		return false;
	}

	return true;
}

bool CSeaBase::CreateCausticMap(const char* pszFileName)
{
	Assert(pszFileName != NULL);

	SAFE_RELEASE(m_pCausticMap);

	if (0 == pszFileName[0])
	{
		return true;
	}

	result_string strTexPath = m_pRender->GetCore()->GetResourcePath();

	m_pCausticMap = m_pRender->CreateVolumeTexture(
		(strTexPath + pszFileName).c_str(), true);

	return true;
}

bool CSeaBase::CreateNoiseMap(const char* pszFileName)
{
	Assert(pszFileName != NULL);

	SAFE_RELEASE(m_pNoiseMap);

	if (0 == pszFileName[0])
	{
		return true;
	}

	result_string strTexPath = m_pRender->GetCore()->GetResourcePath();

	m_pNoiseMap = m_pRender->CreateTexture(
		(strTexPath + pszFileName).c_str(), true);

	return true;
}

bool CSeaBase::Load()
{
	int nDevWidth = m_pRender->GetDeviceWidth() / 2;
	int nDevHeight = m_pRender->GetDeviceHeight() / 2;

    //创建一个张RT颜色
	m_pSeaReflectionTex = m_pRender->CreateColorRT(nDevWidth, nDevHeight,
		TEX_FORMAT_RGB565_UNORM, RT_TYPE_DEVICE);

	if (NULL == m_pSeaReflectionTex)
	{
		CORE_TRACE("(CSeaBase::Load)can't create render texture");
		return false;
	}

	// 检测是否支持创建深度图
	CVar prop;
	//检查是否支持纹理贴图
	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsDepthTextureSupported", prop);
	bool bIsDepthTextureSupported = prop.BoolVal();

	//检查是否支持32位深度图
	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsDepth32Supported", prop);
	bool bIsDepth32Supported = prop.BoolVal();

    //检查是否支持24位深度图
	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsDepth24Supported", prop);
	bool bIsDepth24Supported = prop.BoolVal();

	// 如果支持深度图采样
	if(bIsDepthTextureSupported)
	{
		// 如果支持24位深度就够了
		if(bIsDepth24Supported)
		{
			m_pSeaZBuffer = m_pRender->CreateDepthRT(nDevWidth, nDevHeight, TEX_FORMAT_D24_UINT, RT_TYPE_DEVICE);
		} 
		// 如果24位深度图创建失败 则尝试创建32位深度
		if(m_pSeaZBuffer == NULL && bIsDepth32Supported)
		{
			m_pSeaZBuffer = m_pRender->CreateDepthRT(nDevWidth, nDevHeight, TEX_FORMAT_D32_UINT, RT_TYPE_DEVICE);
		}
		// 只能支持一种默认的可采样格式
		if( m_pSeaZBuffer == NULL )
		{
			m_pSeaZBuffer = m_pRender->CreateDepthRT(nDevWidth, nDevHeight, TEX_FORMAT_D_DEFAULT, RT_TYPE_DEVICE);
		}

		if (NULL == m_pSeaZBuffer)
		{
			CORE_TRACE("(CSeaBase::Load)can't create depth surface");
		}
	}
	else//放弃采样打算
	{
		if(bIsDepth24Supported)
		{
			m_pSeaZReflectionTex = m_pRender->CreateDepthTex(nDevWidth, nDevHeight, TEX_FORMAT_D24_UINT, RT_TYPE_DEVICE);
		}
		// 如果24位深度图创建失败 则尝试创建32位深度
		if(m_pSeaZReflectionTex == NULL && bIsDepth32Supported)
		{
			m_pSeaZReflectionTex = m_pRender->CreateDepthTex(nDevWidth, nDevHeight, TEX_FORMAT_D32_UINT, RT_TYPE_DEVICE);
		}

		if(m_pSeaZReflectionTex == NULL)
		{
			m_pSeaZReflectionTex = m_pRender->CreateDepthTex(nDevWidth, nDevHeight, TEX_FORMAT_D_DEFAULT, RT_TYPE_DEVICE);
		}

		if (NULL == m_pSeaZReflectionTex)
		{
			CORE_TRACE("(CSeaBase::Load)can't create depth surface");
		}		
	}

	m_WaterVSList.SetRender(m_pRender);
	m_WaterVSList.SetShaderName("water.vsh", "vs_main_water");
	m_WaterVSList.SetDefine(WATER_FLAG_WAVELESS, "#define WAVELESS 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_USE_REFLECTION, "#define USE_REFLECTION 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_USE_REFRACTION, "#define USE_REFRACTION 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_USE_DEPTHMAP, "#define USE_DEPTHMAP 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_SUN_REFLECTION, "#define SUN_REFLECTION 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_BORDER, "#define BORDER 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_UNDERWATER, "#define UNDERWATER 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_WHITEWATER, "#define WHITEWATER 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_FOGLINEAR, "#define FOGLINEAR 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_FOGEXP, "#define FOGEXP 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_USE_INTZ, "#define USE_INTZ 1\n");
	m_WaterVSList.SetDefine(WATER_FLAG_EASY,"#define EASY 1\n");

	m_WaterPSList.SetRender(m_pRender);
	m_WaterPSList.SetShaderName("water.fsh", "ps_main_water");
	m_WaterPSList.SetDefine(WATER_FLAG_WAVELESS, "#define WAVELESS 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_USE_REFLECTION, "#define USE_REFLECTION 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_USE_REFRACTION, "#define USE_REFRACTION 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_USE_DEPTHMAP, "#define USE_DEPTHMAP 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_SUN_REFLECTION, "#define SUN_REFLECTION 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_BORDER, "#define BORDER 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_UNDERWATER, "#define UNDERWATER 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_WHITEWATER, "#define WHITEWATER 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_FOGLINEAR, "#define FOGLINEAR 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_FOGEXP, "#define FOGEXP 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_USE_INTZ, "#define USE_INTZ 1\n");
	m_WaterPSList.SetDefine(WATER_FLAG_EASY, "#define EASY 1\n");


	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsAdrenoGPU", prop);
	//根据机器型号
	m_bEasyEffect = prop.BoolVal();

	m_pCausticVS = m_pRender->LoadVertexShader("caustic.vsh", 
		"vs_main_caustic", "");

	m_CausticPSList.SetRender(m_pRender);
	m_CausticPSList.SetShaderName("caustic.hlsl", "ps_main_caustic");
	m_CausticPSList.SetDefine(CAUSTIC_PS_SHADOW, "#define SHADOW 1\n");


	if (!CreateCausticMap(m_SeaData.strCausticMap))
	{
		CORE_TRACE("(CSeaBase::Load)load caustic map failed");
		return false;
	}

	if (!CreateNoiseMap(m_SeaData.strNoiseMap))
	{
		CORE_TRACE("(CSeaBase::Load)load noise map failed");
		return false;
	}

	if (!CreateNormalMap(m_SeaData.strNormalMapFileName1, 
		m_SeaData.strNormalMapFileName2, 
		m_SeaData.strNormalMapFileName3, 
		m_SeaData.strNormalMapFileName4))
	{
		CORE_TRACE("(CSeaBase::Load)load normal map failed");
		return false;
	}

	if (!CreateWhiteWaterMap(m_SeaData.strWhiteWaterMap))
	{
		CORE_TRACE("(CSeaBase::Load)load whitewater map failed");
		return false;
	}

	return true;
}

bool CSeaBase::Unload()
{
	ReleaseAll();
	
	return true;
}

void CSeaBase::RefreshBlocks()
{
	float max_height = 0.0F;

	for (int i = 0; i < 5; i++)
	{
		max_height += m_SeaData.fWaveSwing[i];
	}

	SetMaxSeaHeight(max_height * 2);

	float min_y = m_fBaseHeight - max_height;
	float max_y = m_fBaseHeight + max_height;
	FmVec3 v1(0.0F, min_y, 0.0F);
	FmVec3 v2(0.0F, max_y, 0.0F);
	Terrain* pTerrain = m_pTerrain;
	CZoneManager* pManager = pTerrain->GetZoneManager();
	float zone_size = pTerrain->GetZoneScale() * pTerrain->GetUnitSize();

	v2.x = zone_size;
	v2.z = zone_size;

	FmVec3 vec3Temp = v2 - v1;
	float zone_radius = FmVec3Length(&vec3Temp);
	float chunk_size = pTerrain->GetChunkScale() * pTerrain->GetUnitSize();

	v2.x = chunk_size;
	v2.z = chunk_size;

	float chunk_radius = FmVec3Length(&vec3Temp);
	bool horizon_cull = pTerrain->GetHorizontalCulling();
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();
	FmVec3 cam_pos = camera.vPosition;
	// 离摄像机最近距离的平方
	float near_distance_sq = 1000.0F * 1000.0F;
	// 水面的远裁距离要比地表近一些
	float far_clip_distance = pContext->GetFarClipDistance() - chunk_radius;
	float far_clip_square = far_clip_distance * far_clip_distance;
	size_t render_block_num = 0;
	CTerrainChunk** render_blocks = m_RenderBlocks;
	// 对可见区域内的分块做视锥剪裁
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

		if (!zone->GetHasWater())
		{
			// 此区域没有任何水面
			continue;
		}

		if (!zone->GetAllLoaded())
		{
			continue;
		}

		float zone_left = zone->GetLeft();
		float zone_top = zone->GetTop();

		// 先做区域剪裁
		FmVec3 v_min(zone_left, min_y, zone_top);
		FmVec3 v_max(zone->GetRight(), max_y, zone->GetBottom());
		FmVec3 zone_center = (v_min + v_max) * 0.5F;

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

		for (size_t i = 0; i < chunk_num; ++i)
		{
			CTerrainChunk* chunk = zone_chunks + i;

			if (chunk->GetWaterIndex() != m_nIndex)
			{
				continue;
			}

			v1.x = zone_left + chunk->GetCol() * chunk_size;
			v1.z = zone_top + chunk->GetRow() * chunk_size;
			v2.x = v1.x + chunk_size;
			v2.z = v1.z + chunk_size;

			FmVec3 chunk_center = (v1 + v2) * 0.5F;
			float sx = chunk_center.x - cam_pos.x;
			float sz = chunk_center.z - cam_pos.z;

			// 水面的远裁距离要比地表近一些
			if ((sx * sx + sz * sz) > far_clip_square)
			{
				continue;
			}

			if (pContext->IsViewClip(chunk_center, chunk_radius))
			{
				continue;
			}

			if (horizon_cull)
			{
				if (pTerrain->IsHorizonClip(v1, v2))
				{
					continue;
				}
			}

			if (render_block_num < MAX_RENDER_BLOCK)
			{
				FmVec3 vec3Temp = cam_pos - chunk->GetCenter();
				float distance_sq = FmVec3LengthSq(&vec3Temp);

				if (distance_sq < near_distance_sq)
				{
					near_distance_sq = distance_sq;
				}
				
				render_blocks[render_block_num++] = chunk;
			}
		}
	}

	m_nRenderBlockNum = render_block_num;
	m_fNearCameraDistance = sqrt(near_distance_sq);
}

const char* seabase_vertex_t_name[] = 
{
	"iPos",
	"iDiffuseNormal",
};

void CSeaBase::Realize()
{
	if (!m_bVisible)
	{
		return;
	}

	RefreshBlocks();

	if ((m_nRenderBlockNum > 0) && m_pTerrain->GetWaterReflect() 
		&& m_SeaData.bReflect)
	{
		if (m_pTerrain->GetWaterEnvMapReusable())
		{
			if (0 == m_nReflectNoFlush)
			{
				GenerateEnvMap();
			}

			if (m_fNearCameraDistance < 64.0F)
			{
				// 每帧刷新
				m_nReflectNoFlush = 0;
			}
			else if (m_fNearCameraDistance < 128.0F)
			{
				// 两帧刷新一次
				m_nReflectNoFlush = (m_nReflectNoFlush + 1) & 0x1;
			}
			else if (m_fNearCameraDistance < 256.0F)
			{
				// 四帧刷新一次
				m_nReflectNoFlush = (m_nReflectNoFlush + 1) & 0x3;
			}
			else
			{
				// 八帧刷新一次
				m_nReflectNoFlush = (m_nReflectNoFlush + 1) & 0x7;
			}
		}
		else
		{
			GenerateEnvMap();
		}
	}

	Terrain* pTerrain = m_pTerrain;
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();
	const FmVec3& cam_pos = camera.vPosition;


	if (0 == m_nRenderBlockNum)
	{
		return;
	}

	// 相对坐标参考位置
	const FmVec3& refer_pos = pContext->GetReferPosition();

	m_bUnderWater = (cam_pos.y <= m_fBaseHeight);

	bool waveless = !pTerrain->GetWaterWave();
	bool use_reflection = pTerrain->GetWaterReflect() && m_SeaData.bReflect;

	// 是否使用反射
	bool use_refraction = pContext->GetEnableRealizeCopyTempColorRT();
	// 是否使用反射深度
	bool use_depth = pTerrain->GetWaterDepth() && pContext->GetRealizeCopyTempDepth() && (m_pSeaZBuffer!= NULL);
	
	bool sun_reflection = pTerrain->GetWaterSunReflect() && !m_bUnderWater;
	bool use_border = pTerrain->GetWaterBorder();
	bool whitewater = use_border && (m_pWhiteWaterMap != NULL) 
		&& (m_pWhiteWaterMap->GetShaderTex() != NULL);
	bool use_intz = false;//use_reflection && (m_pSeaZBuffer != NULL);
	// 是否开启指数雾效
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
	// 是否开启线性雾效
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;

	size_t flags = (size_t(waveless) << WATER_FLAG_WAVELESS) 
		+ (size_t(use_reflection) << WATER_FLAG_USE_REFLECTION)
		+ (size_t(use_refraction) << WATER_FLAG_USE_REFRACTION) 
		+ (size_t(use_depth) << WATER_FLAG_USE_DEPTHMAP)
		+ (size_t(sun_reflection) << WATER_FLAG_SUN_REFLECTION) 
		+ (size_t(use_border) << WATER_FLAG_BORDER)
		+ (size_t(m_bUnderWater) << WATER_FLAG_UNDERWATER) 
		+ (size_t(whitewater) << WATER_FLAG_WHITEWATER)
		+ (size_t(fog_linear) << WATER_FLAG_FOGLINEAR) 
		+ (size_t(fog_exp) << WATER_FLAG_FOGEXP) 
		+ (size_t(use_intz) << WATER_FLAG_USE_INTZ)
		+ (size_t(m_bEasyEffect) << WATER_FLAG_EASY);
   
    CTerrainChunk::chunk_water_shader_handle_t* pShaderHandle = GetShaderHandle(flags);

	FmMat4 mtxViewProj;

	FmMat4Transpose(&mtxViewProj, &camera.mtxViewProjRelative);

	// CV_TIMES
	m_fTimes = float(fmod(m_pRender->GetCore()->GetTotalSeconds(), 10000.0));

	float times = m_fTimes;
	// 另外包含水面的高度值
	float sea_height = m_fBaseHeight;

	if (!waveless)
	{
		// 有波动时必须要减去波浪的平均附加高度
		sea_height -= GetAverageWaveHeight();
	}

	float view_pos[4] = { cam_pos.x - refer_pos.x, cam_pos.y - refer_pos.y, 
		cam_pos.z - refer_pos.z, sea_height };
	// CV_WAVEPARAM
	float fWaveParam[32] = { 0.0F };

	for (int i = 0; i < 5; ++i)
	{
		fWaveParam[i * 4 + 0] = m_SeaData.fWaveAngleFreq[i];
		fWaveParam[i * 4 + 1] = m_SeaData.fWaveSwing[i];
		fWaveParam[i * 4 + 2] = m_SeaData.fWavePhaX[i];
		fWaveParam[i * 4 + 3] = m_SeaData.fWaveGradient[i];
		fWaveParam[i * 2 + 20] = m_SeaData.fWaveDirectionX[i];
		fWaveParam[i * 2 + 21] = m_SeaData.fWaveDirectionZ[i];
	}

	// CV_NORMALMAPSPEED 0 ~ 3
	float fNormalmapSpeed0[4] = { m_SeaData.fNormalMapSpeedX[0], 
		m_SeaData.fNormalMapSpeedZ[0], 0.0F, 0.0F };
	float fNormalmapSpeed1[4] = { m_SeaData.fNormalMapSpeedX[1], 
		m_SeaData.fNormalMapSpeedZ[1], 0.0F, 0.0F };
	float fNormalmapSpeed2[4] = { m_SeaData.fNormalMapSpeedX[2], 
		m_SeaData.fNormalMapSpeedZ[2], 0.0F, 0.0F };
	float fNormalmapSpeed3[4] = { m_SeaData.fNormalMapSpeedX[3], 
		m_SeaData.fNormalMapSpeedZ[3], 0.0F, 0.0F };
	// 阳光的方向
	FmVec3 sun_dir = pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);
	// 太阳位置
	FmVec4 sun_pos = FmVec4(sun_dir * 1000.0F, 0.0F);
	// 相对坐标参考位置
	FmVec4 refer_position = FmVec4(refer_pos, 0.0F);
	FmVec4 light_dir = FmVec4(sun_dir, 0.0F);

	CTerrainChunk::chunk_water_cb_per_group_t cb_perf_group;
	cb_perf_group.mtxViewProj = mtxViewProj;
	cb_perf_group.times = times;
	cb_perf_group.view_pos = view_pos;
	cb_perf_group.fWaveParam0 = FmVec4(fWaveParam[0],fWaveParam[1],fWaveParam[2],fWaveParam[3]);
	cb_perf_group.fWaveParam1 = FmVec4(fWaveParam[4],fWaveParam[5],fWaveParam[6],fWaveParam[7]);
	cb_perf_group.fWaveParam2 = FmVec4(fWaveParam[8],fWaveParam[9],fWaveParam[10],fWaveParam[11]);
	cb_perf_group.fWaveParam3 = FmVec4(fWaveParam[12],fWaveParam[13],fWaveParam[14],fWaveParam[15]);
	cb_perf_group.fWaveParam4 = FmVec4(fWaveParam[16],fWaveParam[17],fWaveParam[18],fWaveParam[19]);
	cb_perf_group.fWaveParam5 = FmVec4(fWaveParam[20],fWaveParam[21],fWaveParam[22],fWaveParam[23]);
	cb_perf_group.fWaveParam6 = FmVec4(fWaveParam[24],fWaveParam[25],fWaveParam[26],fWaveParam[27]);
	cb_perf_group.fWaveParam7 = FmVec4(fWaveParam[28],fWaveParam[29],fWaveParam[30],fWaveParam[31]);
	cb_perf_group.fNormalMapScale0 = m_SeaData.fNormalMapScale[0];
	cb_perf_group.fNormalMapScale1 = m_SeaData.fNormalMapScale[1];
	cb_perf_group.fNormalMapScale2 = m_SeaData.fNormalMapScale[2];
	cb_perf_group.fNormalMapScale3 = m_SeaData.fNormalMapScale[3];
	cb_perf_group.fNormalmapSpeed0 = fNormalmapSpeed0;
	cb_perf_group.fNormalmapSpeed1 = fNormalmapSpeed1;
	cb_perf_group.fNormalmapSpeed2 = fNormalmapSpeed2;
	cb_perf_group.fNormalmapSpeed3 = fNormalmapSpeed3;
	cb_perf_group.sun_pos = sun_pos;
	cb_perf_group.refer_position = refer_position;
	cb_perf_group.light_dir = light_dir;


	// CP_WATERCOLOR
	FmVec4 water_color;
	FmVec4 sky_color;
	FmVec4 sun_color_start;
	FmVec4 sun_color_end;
 
	water_color = VisUtil_ColorToVector4(m_SeaData.nWaterColor);
	sky_color = VisUtil_ColorToVector4(m_SeaData.nSkyColor);
	sun_color_start = VisUtil_ColorToVector4(m_SeaData.nSunColorStart);
	sun_color_end = VisUtil_ColorToVector4(m_SeaData.nSunColorEnd);
	
	float distance_scale = m_SeaData.fDistanceScale;
	float ref_inten =  m_SeaData.fReflectionInten;
	float ref_weight =  m_SeaData.fReflectionWeight;
	float refraction_inten = m_SeaData.fRefractionInten;
	float refraction_weight =  m_SeaData.fRefractionWeight;
	float sun_level =   m_SeaData.fSunLevel;
	// 深度反算所需的参数
	float near_plane = pContext->GetCamera().fNearZ;
	float far_plane = pContext->GetCamera().fFarZ;
	float factor = near_plane * far_plane;
	float depth_param[4] = { far_plane / factor, 
		(far_plane - near_plane) / factor, 0.0F, 0.0F };
	float ref_sun_inten = m_SeaData.fReflectionSunInten;
	// 像素尺寸
	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	float pixel_size[4] = { 1.0F / (float)view_width, 
		1.0F / (float)view_height, 0.0F, 0.0F };
	// If under water
	float fUnderWater[4] = { 1.0F, 1.0F, 0.0F, 0.0F };

	if (m_bUnderWater)
	{
		fUnderWater[0] = -1.0F;
	}

	cb_perf_group.water_color = water_color;
	cb_perf_group.sky_color = sky_color;
	cb_perf_group.sun_color_start = sun_color_start;
	cb_perf_group.sun_color_end = sun_color_end;
	cb_perf_group.distance_scale = distance_scale;
	cb_perf_group.ref_inten = ref_inten;
	cb_perf_group.ref_weight = ref_weight;
	cb_perf_group.refraction_inten = refraction_inten;
    cb_perf_group.refraction_weight = refraction_weight;
	cb_perf_group.sun_level = sun_level;
	cb_perf_group.depth_param = depth_param;
	cb_perf_group.ref_sun_inten = ref_sun_inten;
	cb_perf_group.pixel_size = pixel_size;
	cb_perf_group.fUnderWater = fUnderWater;
 

	// Fog
//	if (fog_linear)
	{
		FmVec4 fog_color = pContext->GetVector4(
			IRenderContext::V_FOG_CURRENT);
		float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
		float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END);
		float fog_param[4] = { fog_start, 1.0F / (fog_end - fog_start), 0.0F, 
			0.0F };
        cb_perf_group.fog_color = fog_color;
		cb_perf_group.fog_param = fog_param;
	}

	if (fog_exp)
	{
		FmVec4 fog_exp_param = pContext->GetVector4(
			IRenderContext::V_FOG_EXP_CURRENT);
		float fog_density = pContext->GetFloat(IRenderContext::F_FOG_DENSITY);

		fog_exp_param.w = fog_density;
		cb_perf_group.fog_exp_param = fog_exp_param;
	}


	// 是否可以降低面数
	bool can_level_down = (!use_depth) && (!use_border) && waveless;
	bool occlusion_query = false;//pContext->GetEnableOcclusionQuery();
	float water_min_y = m_fBaseHeight - m_fMaxSeaHeight * 0.5F;
	float water_max_y = water_min_y + m_fMaxSeaHeight;

	CTerrainPainter* pPainter = pTerrain->GetPainter();

	for( size_t i = 0 ; i < m_nRenderBlockNum; ++i)
	{
		CTerrainChunk::chunk_cb_object_t cb_obj;
		CTerrainChunk* chunk = m_RenderBlocks[i];
		//增加渲染批次
		m_pRender->GetSceneView()->AddWaterBatch(DrawChunksShaderWaterBatch, chunk,"SeaBase_Chunk",false);

		chunk->SetWaterShader(pShaderHandle->pShader);

		chunk->SetWaterShaderHandle(pShaderHandle);
		//这里赋值相关全局常量值
		chunk->SetWaterPerGroup(cb_perf_group);

		chunk->SetWaterTexture(use_refraction,use_reflection,m_pSeaReflectionTex,
			                   m_pWaterNormalMap1,m_pWaterNormalMap2,m_pWaterNormalMap3,m_pWaterNormalMap4,
							   pContext->GetRealizeCopyTempColorRT(),pContext->GetRealizeCopyTempDepth(),m_pWhiteWaterMap,m_pSeaZBuffer);

		int level = -1;

		if (can_level_down)
		{
			level = chunk->GetLevel();

			// 降低面数
			if (level < 2)
			{
				level = 2;
			}
		}

		chunk->SetWaterLevel(level);
	}
 
}

static void MirrorViewMatrixByHorizonPlane(const FmMat4& mtxOriginView, 
	FmMat4& mtxMirrorView, float fHeight)
{
	FmMat4Identity(&mtxMirrorView);

	mtxMirrorView._22 = -1.0F;
	mtxMirrorView._42 = fHeight * 2.0F;

	FmMatrixMultiply(&mtxMirrorView, &mtxMirrorView, &mtxOriginView);

	mtxMirrorView._12 = -mtxMirrorView._12;
	mtxMirrorView._22 = -mtxMirrorView._22;
	mtxMirrorView._32 = -mtxMirrorView._32;
	mtxMirrorView._42 = -mtxMirrorView._42;
}

void CSeaBase::GenerateEnvMap()
{
	IRenderContext* pOldContext = m_pRender->GetContext();
	ISceneView* pOldSceneView = m_pRender->GetSceneView();


	sea_data_t* pSeaData = &m_SeaData;

	if (pSeaData->bCubeMapStatic && pSeaData->bCubeMapReady)
	{
		// 不需要再刷新环境贴图
		return;
	}

	if (NULL == m_pContext)
	{
		m_pContext = m_pRender->CreateContext();
	}

	if( NULL == m_pSeaSceneView)
	{
		m_pSeaSceneView = m_pRender->CreateSceneView(ISceneView::TYPE_REFLECTION,"SeaReflection");
	}


	IRenderContext* pContext = m_pContext;

	// 设置渲染环境贴图的剪裁距离
	float far_clip_distance = pOldContext->GetFarClipDistance() 
		* pSeaData->fReflectionRatio;
	bool gamma_correct = false;//pOldContext->GetEnableGammaCorrect();
	int fog_linear = pOldContext->GetInt(IRenderContext::I_FOG_LINEAR);
	int fog_exp = pOldContext->GetInt(IRenderContext::I_FOG_EXP);
	unsigned int fog_exp_color = pOldContext->GetColor(
		IRenderContext::C_FOG_EXP_COLOR);
	float fog_height = pOldContext->GetFloat(IRenderContext::F_FOG_HEIGHT);
	FmVec4 fog_current = pOldContext->GetVector4(
		IRenderContext::V_FOG_CURRENT);
	FmVec4 fog_exp_current = pOldContext->GetVector4(
		IRenderContext::V_FOG_EXP_CURRENT);
	unsigned int fog_color;
	float fog_start;
	float fog_end;
	float fog_density;

	m_pSeaSceneView->SetColorRT(m_pSeaReflectionTex);

	if(m_pSeaZBuffer)
	{
		m_pSeaSceneView->SetDepthRT(m_pSeaZBuffer);
	}
	else
	{
        m_pSeaSceneView->SetDepthTex(m_pSeaZReflectionTex);
	}


	// 雾参数
	if (m_bUnderWater)
	{
		fog_color = pOldContext->GetColor(IRenderContext::C_FOG_COLOR_UNDERWATER);
		fog_start = pOldContext->GetFloat(IRenderContext::F_FOG_START_UNDERWATER);
		fog_end = pOldContext->GetFloat(IRenderContext::F_FOG_END_UNDERWATER);
		fog_density = pOldContext->GetFloat(
			IRenderContext::F_FOG_DENSITY_UNDERWATER);
	}
	else
	{
		fog_color = pOldContext->GetColor(IRenderContext::C_FOG_COLOR);
		fog_start = pOldContext->GetFloat(IRenderContext::F_FOG_START);
		fog_end = pOldContext->GetFloat(IRenderContext::F_FOG_END);
		fog_density = pOldContext->GetFloat(IRenderContext::F_FOG_DENSITY);
	}

	m_pSeaSceneView->SetBackColor( VisUtil_ColorToVector4(fog_color) );

	int view_width = m_pRender->GetDeviceWidth() / 2;
	int view_height = m_pRender->GetDeviceHeight() / 2;

	pContext->SetViewSize(view_width, view_height);
	pContext->SetFarClipDistance(far_clip_distance);

	pContext->SetInt(IRenderContext::I_FOG_LINEAR, fog_linear);
	pContext->SetInt(IRenderContext::I_FOG_EXP, fog_exp);
	pContext->SetColor(IRenderContext::C_FOG_COLOR, fog_color);
	pContext->SetColor(IRenderContext::C_FOG_EXP_COLOR, fog_exp_color);
	pContext->SetFloat(IRenderContext::F_FOG_START, fog_start);
	pContext->SetFloat(IRenderContext::F_FOG_END, fog_end);
	pContext->SetFloat(IRenderContext::F_FOG_DENSITY, fog_density);
	pContext->SetFloat(IRenderContext::F_FOG_HEIGHT, fog_height);
	pContext->SetVector4(IRenderContext::V_FOG_CURRENT, fog_current);
	pContext->SetVector4(IRenderContext::V_FOG_EXP_CURRENT, fog_exp_current);
	// 标识当前在渲染水面倒影
	pContext->SetInt(IRenderContext::I_WATER_ENVMAP, 1);
	// 设置当前水面的高度
	pContext->SetFloat(IRenderContext::F_WATER_HEIGHT, m_fBaseHeight);

	// 设置全局光照
	FmVec3 sun_dir = pOldContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);
	FmVec3 light_diffuse = pOldContext->GetVector(
		IRenderContext::V_LIGHT_DIFFUSE);
	FmVec3 light_ambient = pOldContext->GetVector(
		IRenderContext::V_LIGHT_AMBIENT);
	// 设置环境光照
	unsigned int ambient_color = pOldContext->GetColor(
		IRenderContext::C_AMBIENT_COLOR);
	unsigned int diffuse_color = pOldContext->GetColor(
		IRenderContext::C_DIFFUSE_COLOR);
	float ambient_intensity = pOldContext->GetFloat(
		IRenderContext::F_AMBIENT_INTENSITY);
	float diffuse_intensity = pOldContext->GetFloat(
		IRenderContext::F_DIFFUSE_INTENSITY);

	pContext->SetVector(IRenderContext::V_LIGHT_DIRECTION, sun_dir);
	pContext->SetVector(IRenderContext::V_LIGHT_DIFFUSE, light_diffuse);
	pContext->SetVector(IRenderContext::V_LIGHT_AMBIENT, light_ambient);
	pContext->SetColor(IRenderContext::C_AMBIENT_COLOR, ambient_color);
	pContext->SetColor(IRenderContext::C_DIFFUSE_COLOR, diffuse_color);
	pContext->SetFloat(IRenderContext::F_AMBIENT_INTENSITY, ambient_intensity);
	pContext->SetFloat(IRenderContext::F_DIFFUSE_INTENSITY, diffuse_intensity);

	m_pRender->SetContext(pContext);
	m_pSeaSceneView->SetContext(m_pContext);


	m_pRender->SetSceneView(m_pSeaSceneView);

	//获取原先的VIEW矩阵
	const FmMat4& mtxViewOld = pOldContext->GetCamera().mtxView;
	FmVec3 vWaterNormal = FmVec3(0.0F, 1.0F, 0.0F);
	// 创建新的mtxView
	FmMat4 mtxView;	

	MirrorViewMatrixByHorizonPlane(mtxViewOld, mtxView, m_fBaseHeight);
//	mtxView = mtxViewOld ;

	const FmMat4& mtxProj = pOldContext->GetCamera().mtxProj;
	const FmVec3& refer_pos = pOldContext->GetReferPosition();

	pContext->SetReferPosition(refer_pos.x, refer_pos.y, refer_pos.z);
	pContext->SetViewMatrix(mtxView);
	pContext->SetProjMatrix(mtxProj);
	pContext->MakeHelperMatrix();
	pContext->MakeClipPlane();

	// 渲染倒影
	float fWaterHeight = m_fBaseHeight;
	FmPlane waterPlane;
	FmMat4 mtxViewProjection;

	if (pOldContext->GetCamera().vPosition.y > fWaterHeight)
	{
		waterPlane = FmPlane(0.0F, 1.0F, 0.0F, -fWaterHeight);
	}
	else
	{
		waterPlane = FmPlane(0.0F, -1.0F, 0.0F, fWaterHeight);
	}

	FmPlaneNormalize(&waterPlane, &waterPlane);


	FmMat4 matViewProjection;
	FmMat4 mtxViewProjectionIT;

	FmMat4Multiply(&mtxViewProjection, &mtxView, &mtxProj);
	FmMat4Inverse(&mtxViewProjectionIT,NULL,&mtxViewProjection);
	FmMat4Transpose(&mtxViewProjectionIT,&mtxViewProjectionIT);
	FmPlaneTransform(&waterPlane, &waterPlane, &mtxViewProjectionIT);

	pContext->SetInt(IRenderContext::I_USE_CLIP_PLANE, 1);
	pContext->SetVector4(IRenderContext::V_CLIP_PLANE, *(FmVec4*)&waterPlane);

	m_pTerrain->RealizeReflection(m_nIndex);

	// 环境贴图已经更新（至少有天空盒的倒影）
	pSeaData->bCubeMapReady = true;
	m_pRender->AddSceneView(m_pSeaSceneView);
	m_pRender->SetContext(pOldContext);
    m_pRender->SetSceneView(pOldSceneView);

	//调试水体反射用的
	pOldContext->SetDebugColorRT(m_pSeaReflectionTex);
	//pOldContext->SetDebugDepthRT( m_pSeaZBuffer );
	//pOldContext->SetDebugDepthRT( pOldContext->GetRealizeTempDepthRT() );
}

void CSeaBase::DrawCaustic()
{
	IRenderContext* pContext = m_pRender->GetContext();

	if (NULL == m_pCausticMap)
	{
		return;
	}

	IShaderTex* pCasticTex = m_pCausticMap->GetShaderTex();
	IShaderTex* pNoiseTex = m_pNoiseMap->GetShaderTex();

	if (NULL == pCasticTex)
	{
		return;
	}

	const camera_t& camera = pContext->GetCamera();
	//const dx_viewport_t& vp = camera.Viewport;
	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	float half_pixel_size[4] = { 0.5F / (float)view_width, 
		0.5F / (float)view_height, 0.0F, 0.0F };
	FmMat4 mtxViewInverse;
	FmMat4 mtxProjInverse;

	FmMat4Inverse(&mtxViewInverse, NULL, &camera.mtxViewRelative);
	FmMat4Transpose(&mtxViewInverse, &mtxViewInverse);
	FmMat4Transpose(&mtxProjInverse, &camera.mtxProjInverse);

	float water_height[4] = { m_fBaseHeight, 0.0F, 0.0F, 0.0F };
	float times[4] = { m_fTimes * m_SeaData.fCausticSpeed, 0.0F, 0.0F, 0.0F };
	// 亮度
	float bright_and_focus[4] = { m_SeaData.fCausticBright, 
		m_SeaData.fCausticFocus, 0.0F, 0.0F };
	// 缩放
	float scaling[4] = { 1.0F / m_SeaData.fCausticScale, 
		1.0F / m_SeaData.fCausticScale, 1.0F / m_SeaData.fCausticScale, 0.0F };
	// 阳光的颜色和方向
	FmVec3 vSunColor = pContext->GetVector(IRenderContext::V_LIGHT_DIFFUSE);
	FmVec3 vSunDir = pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);

	FmVec3TransformNormal(&vSunDir, &vSunDir, &camera.mtxView);

	float sun_color[4] = { vSunColor.x, vSunColor.y, vSunColor.z, 1.0F };
	float sun_dir[4] = { vSunDir.x, vSunDir.y, vSunDir.z, 0.0F };


	size_t ps_flags = size_t(1) << CAUSTIC_PS_SHADOW; 

	IPixelShader* pPS = m_CausticPSList.GetShader(ps_flags);

}

bool CSeaBase::DumpReflectMap(const char* file_name)
{
	Assert(file_name != NULL);

	return m_pRender->SaveTextureToFile(m_pSeaReflectionTex, file_name);
}

float CSeaBase::GetCurrentHeight(float x, float y)
{
	FmVec2 WavePos(x, y);
	float seaheight = m_fBaseHeight;

	if (m_pTerrain->GetWaterWave())
	{
		for (int i = 0; i < 5; ++i)
		{
			FmVec2 WaveDir(m_SeaData.fWaveDirectionX[i], 
				m_SeaData.fWaveDirectionZ[i]);

			seaheight += m_SeaData.fWaveSwing[i] 
				* pow(sin(FmVec2Dot(&WaveDir, &WavePos) 
				* m_SeaData.fWaveAngleFreq[i] 
				+ m_fTimes * m_SeaData.fWavePhaX[i]) * 0.5F 
				+ 0.5F, m_SeaData.fWaveGradient[i]);
		}

		seaheight -= GetAverageWaveHeight();
	}

	return seaheight;
}

float CSeaBase::GetAverageWaveHeight()
{
	float height = 0.0F;

	for (int i = 0; i < 5; ++i)
	{
		height += m_SeaData.fWaveSwing[i] 
			* pow(0.5F, m_SeaData.fWaveGradient[i]);
	}

	return height;
}

CTerrainChunk::chunk_water_shader_handle_t* CSeaBase::GetShaderHandle(size_t flags)
{
	if(!m_ShaderMapHandle.Exists(flags))
	{
		IVertexShader* pVS = m_WaterVSList.GetShader(flags);
		IPixelShader* pPS = m_WaterPSList.GetShader(flags);
		IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS,pPS,seabase_vertex_t_name,2);
        CTerrainChunk::chunk_water_shader_handle_t* pShaderHandle = (CTerrainChunk::chunk_water_shader_handle_t*)CORE_ALLOC(sizeof(CTerrainChunk::chunk_water_shader_handle_t));
        
        IShaderParamOp* pShaderOp = pShader->GetParamOp();
		pShaderHandle->pShader = pShader;

		//UseWaterObject
		pShaderHandle->mat4mtxViewProjHandle = pShaderOp->FindParamIdByName("c_mtxViewProj");
		pShaderHandle->fTimeHandle = pShaderOp->FindParamIdByName("c_fTime");
		pShaderHandle->vec4ViewPositionHandle = pShaderOp->FindParamIdByName("c_ViewPosition");
		pShaderHandle->vec4WaveParam0Handle = pShaderOp->FindParamIdByName("c_WaveParam0");
		pShaderHandle->vec4WaveParam1Handle = pShaderOp->FindParamIdByName("c_WaveParam1");
		pShaderHandle->vec4WaveParam2Handle = pShaderOp->FindParamIdByName("c_WaveParam2");
		pShaderHandle->vec4WaveParam3Handle = pShaderOp->FindParamIdByName("c_WaveParam3");
		pShaderHandle->vec4WaveParam4Handle = pShaderOp->FindParamIdByName("c_WaveParam4");
		pShaderHandle->vec4WaveParam5Handle = pShaderOp->FindParamIdByName("c_WaveParam5");
		pShaderHandle->vec4WaveParam6Handle = pShaderOp->FindParamIdByName("c_WaveParam6");
		pShaderHandle->vec4WaveParam7Handle = pShaderOp->FindParamIdByName("c_WaveParam7");
		pShaderHandle->fNormalMapScale0Handle = pShaderOp->FindParamIdByName("c_NormalmapScale0");
		pShaderHandle->fNormalMapScale1Handle = pShaderOp->FindParamIdByName("c_NormalmapScale1");
		pShaderHandle->fNormalMapScale2Handle = pShaderOp->FindParamIdByName("c_NormalmapScale2");
		pShaderHandle->fNormalMapScale3Handle = pShaderOp->FindParamIdByName("c_NormalmapScale3");
		pShaderHandle->vec2NormalmapSpeed0Handle = pShaderOp->FindParamIdByName("c_NormalmapSpeed0");
		pShaderHandle->vec2NormalmapSpeed1Handle = pShaderOp->FindParamIdByName("c_NormalmapSpeed1");
		pShaderHandle->vec2NormalmapSpeed2Handle = pShaderOp->FindParamIdByName("c_NormalmapSpeed2");
		pShaderHandle->vec2NormalmapSpeed3Handle = pShaderOp->FindParamIdByName("c_NormalmapSpeed3");
		pShaderHandle->vec3SunDirectionHandle = pShaderOp->FindParamIdByName("c_SunDirection");
		pShaderHandle->vec4ReferPosHandle = pShaderOp->FindParamIdByName("c_vReferPos");
		pShaderHandle->vec3LightDirHandle = pShaderOp->FindParamIdByName("c_vLightDir");
		pShaderHandle->vec4UnderWaterHandle = pShaderOp->FindParamIdByName("c_UnderWater");
		pShaderHandle->vec4OceanColorHandle = pShaderOp->FindParamIdByName("c_OceanColor");
		pShaderHandle->vec4SkyColorHandle = pShaderOp->FindParamIdByName("c_SkyColor");
		pShaderHandle->vec4SunColorStartHandle = pShaderOp->FindParamIdByName("c_SunColorStart");
		pShaderHandle->vec4SunColorEndHandle = pShaderOp->FindParamIdByName("c_SunColorEnd");
		pShaderHandle->fDistanceScaleHandle = pShaderOp->FindParamIdByName("c_DistanceScale");
		pShaderHandle->fReflectionIntenHandle = pShaderOp->FindParamIdByName("c_ReflectionInten");
		pShaderHandle->fReflectionWeightHandle = pShaderOp->FindParamIdByName("c_ReflectionWeight");
		pShaderHandle->fRefractionIntenHandle = pShaderOp->FindParamIdByName("c_RefractionInten");
		pShaderHandle->fRefractionWeightHandle = pShaderOp->FindParamIdByName("c_RefractionWeight");
		pShaderHandle->fSunLevelHandle = pShaderOp->FindParamIdByName("c_SunLevel");
		pShaderHandle->vec4DepthParamHandle = pShaderOp->FindParamIdByName("c_DepthParam");
		pShaderHandle->fReflectionSunIntenHandle = pShaderOp->FindParamIdByName("c_ReflectionSunInten");
		pShaderHandle->vec4FogColorHandle = pShaderOp->FindParamIdByName("c_FogColor");
		pShaderHandle->vec4FogParamHandle = pShaderOp->FindParamIdByName("c_FogParam");
		pShaderHandle->vec4FogExpParamHandle = pShaderOp->FindParamIdByName("c_FogExpParam");
		pShaderHandle->vec2PixelSizeHandle = pShaderOp->FindParamIdByName("c_PixelSize");

		//UseChunkObject
		pShaderHandle->mat4Detail1ParamHandle =  pShaderOp->FindParamIdByName("c_Detail1Param");
		pShaderHandle->mat4Detail2ParamHandle =  pShaderOp->FindParamIdByName("c_Detail2Param");
		pShaderHandle->mat4Detail3ParamHandle =  pShaderOp->FindParamIdByName("c_Detail3Param");
		pShaderHandle->mat4Detail4ParamHandle =  pShaderOp->FindParamIdByName("c_Detail4Param");
		pShaderHandle->vec3ChunkPosHandle =  pShaderOp->FindParamIdByName("c_vChunkPos");
		pShaderHandle->vec4ZoneOffsetAndSizeHandle =  pShaderOp->FindParamIdByName("c_ZoneOffsetAndSize");
		pShaderHandle->fBaseSpecularLevelHandle =  pShaderOp->FindParamIdByName("c_fBaseSpecularLevel");
		pShaderHandle->fBaseSpecularPowerHandle =  pShaderOp->FindParamIdByName("c_fBaseSpecularPower");
		pShaderHandle->fBaseHeightScaleHandle =  pShaderOp->FindParamIdByName("c_fBaseHeightScale");
		pShaderHandle->vec4BlendSpecularLevelHandle =  pShaderOp->FindParamIdByName("c_BlendSpecularLevel");
		pShaderHandle->vec4BlendSpecularPowerHandle =  pShaderOp->FindParamIdByName("c_BlendSpecularPower");
		pShaderHandle->vec4BlendHeightScaleHandle =  pShaderOp->FindParamIdByName("c_BlendHeightScale");
		pShaderHandle->vec3LightMapColorScaleHandle =  pShaderOp->FindParamIdByName("c_vLightMapColorScale");

		//UseWaterTexture
		pShaderHandle->tex_SeaReflectHandle =  pShaderOp->FindParamIdByName("tex_SeaReflect");
		pShaderHandle->tex_WaterNormalmap1Handle =  pShaderOp->FindParamIdByName("tex_WaterNormalmap1");
		pShaderHandle->tex_WaterNormalmap2Handle =  pShaderOp->FindParamIdByName("tex_WaterNormalmap2");
		pShaderHandle->tex_WaterNormalmap3Handle =  pShaderOp->FindParamIdByName("tex_WaterNormalmap3");
		pShaderHandle->tex_WaterNormalmap4Handle =  pShaderOp->FindParamIdByName("tex_WaterNormalmap4");
		pShaderHandle->tex_SceneRefractHandle =  pShaderOp->FindParamIdByName("tex_SceneRefract");
		pShaderHandle->tex_DepthmapHandle =  pShaderOp->FindParamIdByName("tex_Depthmap");
		pShaderHandle->tex_WhiteWaterHandle =  pShaderOp->FindParamIdByName("tex_WhiteWater");
		pShaderHandle->tex_ReflectDepthHandle =  pShaderOp->FindParamIdByName("tex_ReflectDepth");

		pShaderHandle->vec2RefractUVScaleHandle =  pShaderOp->FindParamIdByName("c_RefractUVScale");
		pShaderHandle->vec2DepthUVScaleHandle =  pShaderOp->FindParamIdByName("c_DepthUVScale");

		m_ShaderMapHandle.Add(flags,pShaderHandle);
	} 

	return m_ShaderMapHandle.Find(flags).GetData();
}

void CSeaBase::DrawChunksShaderWaterBatch(void* pdata)
{
	CTerrainChunk* chunk = (CTerrainChunk*)pdata; 

	//开始渲染一个chunk
    chunk->GetWaterShader()->UsedShader();
	chunk->UseWaterPerGroup(); 
	chunk->UseWaterObject();
	chunk->UseWaterTexture();
	chunk->SetWaterBatch(NULL,chunk->GetWaterShader());
}