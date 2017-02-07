//--------------------------------------------------------------------
// 文件名:		grass_type.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "grass_type.h"
#include "grass_data.h"
#include "terrain.h"
#include "terrain_chunk.h"
#include "terrain_zone.h"
#include "terrain_light.h"
#include "tex_light_map.h"
#include "terrain_culling.h"
#include "terrain_painter.h"
#include "../visual/i_texture.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"

/// \file grass_type.cpp
/// \brief 花草类型

/// entity: GrassType
/// \brief 花草类型实体
DECLARE_ENTITY(GrassType, 0, IEntity);

/// readonly: string Name
/// \brief 花草类型名称
DECLARE_PROPERTY_GET(const char*, GrassType, Name, GetName);

/// property: string Texture
/// \brief 贴图文件名
DECLARE_PROPERTY(const char*, GrassType, Texture, GetTexture, SetTexture);
/// property: int TexScale
/// \brief 贴图尺寸
DECLARE_PROPERTY(int, GrassType, TexScale, GetTexScale, SetTexScale);
/// property: int FrameWidth
/// \brief 宽度方向的贴图分格数量
DECLARE_PROPERTY(int, GrassType, FrameWidth, GetFrameWidth, SetFrameWidth);
/// property: int FrameHeight
/// \brief 高度方向的贴图分格数量
DECLARE_PROPERTY(int, GrassType, FrameHeight, GetFrameHeight, SetFrameHeight);

/// property: float MinWidth
/// \brief 最小宽度
DECLARE_PROPERTY(float, GrassType, MinWidth, GetMinWidth, SetMinWidth);
/// property: float MaxWidth
/// \brief 最大宽度
DECLARE_PROPERTY(float, GrassType, MaxWidth, GetMaxWidth, SetMaxWidth);
/// property: float MinHeight
/// \brief 最小高度
DECLARE_PROPERTY(float, GrassType, MinHeight, GetMinHeight, SetMinHeight);
/// property: float MaxHeight
/// \brief 最大高度
DECLARE_PROPERTY(float, GrassType, MaxHeight, GetMaxHeight, SetMaxHeight);
/// property: float MinPitch
/// \brief 最小倾斜角度
DECLARE_PROPERTY(float, GrassType, MinPitch, GetMinPitch, SetMinPitch);
/// property: float MaxPitch
/// \brief 最大倾斜角度
DECLARE_PROPERTY(float, GrassType, MaxPitch, GetMaxPitch, SetMaxPitch);
/// property: float ScaleFactor
/// \brief 范围乘数因子
DECLARE_PROPERTY(float, GrassType, ScaleFactor, GetScaleFactor, 
	SetScaleFactor);



TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>* GrassType::p_s_gassrender = NULL;


GrassType::GrassType()
{
	m_pTerrain = NULL;
	m_pRender = NULL;
	m_nHash = 0;
	m_nIndex = 0;
	m_bActive = false;
	m_bMustRefresh = true;
	m_nTexScale = 512;
	m_nFrameWidth = 4;
	m_nFrameHeight = 4;
	m_fMinWidth = 1.0F;
	m_fMaxWidth = 1.6F;
	m_fMinHeight = 1.0F;
	m_fMaxHeight = 1.6F;
	m_fMinPitch = 0.5F;
	m_fMaxPitch = 0.8F;
	m_fScaleFactor = 0.1F;
	m_fWinForce = 0.3F;
	m_vWinDir = FmVec3(0.0F, 0.0F, 1.0F);
	m_pTexture = NULL;
}

GrassType::~GrassType()
{

#define shadermap 	TPodHashMap<unsigned int, CTerrainChunk::grass_shader_handle_t*, TPodTraits<unsigned int>,TTerrainGrassShaderAlloc>
	for(shadermap::iterator iter = m_ShaderHandleMap.Begin(); iter != m_ShaderHandleMap.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(CTerrainChunk::grass_shader_handle_t));
	}   
#undef shadermap

	SAFE_RELEASE(m_pTexture);
}

void GrassType::SetTerrain(Terrain* value)
{ 
	Assert(value != NULL);
	
	m_pTerrain = value; 
	m_pRender = value->GetRender();
	m_fScaleFactor = value->GetUnitSize() * 0.1F;
}

bool GrassType::Init(const IVarList& args)
{
	return true;
}

bool GrassType::Shut()
{
	return true;
}

const char* grass_vertex_t_name[] = 
{
	"iPos",
	"iTex0",
	"iHeightAngle",
};

bool GrassType::Load()
{
	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("grass.vsh", "vs_main_grass");
	m_VSList.SetDefine(GRASS_FLAG_FOGLINEAR, "#define FOGLINEAR 1\n");
	m_VSList.SetDefine(GRASS_FLAG_FOGEXP, "#define FOGEXP 1\n");
	m_VSList.SetDefine(GRASS_FLAG_SHADOWEDMAP, "#define SHADOWEDMAP 1\n");
	m_VSList.SetDefine(GRASS_FLAG_PRELIGHT, "#define PRELIGHT 1\n");
	m_VSList.SetDefine(GRASS_FLAG_LIGHTMAP, "#define LIGHTMAP 1\n");
	m_VSList.SetDefine(GRASS_FLAG_EARLYZ, "#define EARLYZ 1\n");

	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("grass.fsh", "ps_main_grass");
	m_PSList.SetDefine(GRASS_FLAG_FOGLINEAR, "#define FOGLINEAR 1\n");
	m_PSList.SetDefine(GRASS_FLAG_FOGEXP, "#define FOGEXP 1\n");
	m_PSList.SetDefine(GRASS_FLAG_SHADOWEDMAP, "#define SHADOWEDMAP 1\n");
	m_PSList.SetDefine(GRASS_FLAG_PRELIGHT, "#define PRELIGHT 1\n");
	m_PSList.SetDefine(GRASS_FLAG_LIGHTMAP, "#define LIGHTMAP 1\n");
	m_PSList.SetDefine(GRASS_FLAG_EARLYZ, "#define EARLYZ 1\n");

	m_pTexture = m_pRender->CreateTexture(m_strTexture.c_str(), true);

	if (NULL == m_pTexture)
	{
		CORE_TRACE("(GrassType::Load)load texture failed");
		CORE_TRACE(m_strTexture.c_str());
		return false;
	}

	return true;
}

void GrassType::PrepareRealize()
{
	IRenderContext* pContext = m_pRender->GetContext();
	float wind_angle = pContext->GetFloat(IRenderContext::F_WIND_ANGLE);
	float wind_speed = pContext->GetFloat(IRenderContext::F_WIND_SPEED);

	m_vWinDir.x = sinf(wind_angle);
	m_vWinDir.z = cosf(wind_angle);

	if (wind_speed < 0.0F)
	{
		wind_speed = 0.0F;
	}

	if (wind_speed > 30.0F)
	{
		wind_speed = 30.0F;
	}

	m_fWinForce = powf(wind_speed * (1.0F / 30.0F), 0.5F);
	
	// 玩家当前位置
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
		m_pTerrain->GetPlayerID());

	if (pVisBase)
	{
		m_vPlayerPos = pVisBase->GetPosition();
	}
	else
	{
		m_vPlayerPos = FmVec3(0.0F, 0.0F, 0.0F);
	}
}

IStaticVB* GrassType::Generate(CTerrainChunk* pChunk, size_t& count)
{
	Assert(pChunk != NULL);

	// 花草密度
	float grass_lod = m_pTerrain->GetGrassLod();

	if (grass_lod < 0.01F)
	{
		// 密度近乎为0时不进行描画
		return NULL;
	}

	const grass_data_t* grasses = pChunk->GetGrasses((int)m_nIndex);
	size_t grass_num = pChunk->GetGrassCount((int)m_nIndex);

	// 先计算出实际要生成的花草数量
	count = 0;

	// 花草数量余数
	float remainder = 0.0F;

	for (size_t j = 0; j < grass_num; ++j)
	{
		const grass_data_t* pData = grasses + j;
		// 数量
		float total_amount = float(pData->nAmount) * grass_lod + remainder;
		int amount = int(total_amount);

		// 保留余数
		remainder = total_amount - amount;

		if (amount <= 0)
		{
			continue;
		}

		count += amount * 3;
	}

	if (0 == count)
	{
		return NULL;
	}

	// 贴图坐标步长
	float delta_u = 1.0F / m_nFrameWidth;
	float delta_v = 1.0F / m_nFrameHeight;
	float tex_unit = 1.0F / float(m_nTexScale);
	// 贴图格子数量
	size_t frame_num = m_nFrameWidth * m_nFrameHeight;

	// 花草数量余数
	remainder = 0.0F;

	TAutoMem<grass_vertex_t, 128, TCoreAlloc> auto_buf(count * 4);
	grass_vertex_t* pv = auto_buf.GetBuffer();
	const FmVec3& chunk_min = pChunk->GetMin();

	for (size_t k = 0; k < grass_num; ++k)
	{
		const grass_data_t* pData = grasses + k;
		// 数量
		float total_amount = float(pData->nAmount) * grass_lod + remainder;
		int amount = int(total_amount);

		// 保留余数
		remainder = total_amount - amount;

		if (amount <= 0)
		{
			continue;
		}

		// 范围
		float scale = float(pData->nScale) * m_fScaleFactor;

		// 使用随机种子
		srand(pData->nSeed);

		for (int j = 0; j < amount; ++j)
		{
			// 贴图格子索引
			size_t frame = size_t(rand()) % frame_num; 
			// 方位角度
			float angle = (float(rand()) / RAND_MAX) * FLOAT_PI * 2.0F;
			// 尺寸
			float width = m_fMinWidth + (m_fMaxWidth - m_fMinWidth) 
				* (float(rand() % 128) / 128.0F);
			float height = m_fMinHeight + (m_fMaxHeight - m_fMinHeight) 
				* (float(rand() % 128) / 128.0F);
			// 倾斜
			float pitch = m_fMinPitch + (m_fMaxPitch - m_fMinPitch) 
				* (float(rand()) / RAND_MAX);
			// 中心位置偏移
			float offs_x = (float(rand()) / RAND_MAX - 0.5F) * scale;
			float offs_z = (float(rand()) / RAND_MAX - 0.5F) * scale;
			// 位置
			float x = pData->fPosiX + offs_x;
			float z = pData->fPosiZ + offs_z;
			float y = m_pTerrain->GetPosiY(x, z);
			// 起始贴图坐标
			float start_u = (frame % m_nFrameWidth) * delta_u;
			float start_v = (frame / m_nFrameWidth) * delta_v;

			x -= chunk_min.x;
			z -= chunk_min.z;

			for (int i = 0; i < 12; i += 4)
			{
				float x_offset = sinf(angle + i * FLOAT_PI / 6) * width * 0.5F;
				float z_offset = cosf(angle + i * FLOAT_PI / 6) * width * 0.5F;

				pv[0].x = x + pitch * z_offset + x_offset;
				pv[0].y = y + height;
				pv[0].z = z - pitch * x_offset + z_offset;
				pv[0].tu = start_u + tex_unit;
				pv[0].tv = start_v + tex_unit;
				pv[0].height = height;
				pv[0].angle = angle;
				pv[1].x = x + pitch * z_offset - x_offset;
				pv[1].y = y + height;
				pv[1].z = z - pitch * x_offset - z_offset;
				pv[1].tu = start_u + delta_u - tex_unit;
				pv[1].tv = start_v + tex_unit;
				pv[1].height = height;
				pv[1].angle = angle;
				pv[2].x = x + x_offset;
				pv[2].y = y;
				pv[2].z = z  + z_offset;
				pv[2].tu = start_u + tex_unit;
				pv[2].tv = start_v + delta_v - tex_unit;
				pv[2].height = 0;
				pv[2].angle = angle;
				pv[3].x = x - x_offset;
				pv[3].y = y;
				pv[3].z = z - z_offset;
				pv[3].tu = start_u + delta_u - tex_unit;
				pv[3].tv = start_v + delta_v - tex_unit;
				pv[3].height = 0;
				pv[3].angle = angle;
				pv += 4;
			}
		}
	}

	IStaticVB* pVBuffer = m_pRender->CreateStaticVB(auto_buf.GetBuffer(),
		(unsigned int)count * 4 * sizeof(grass_vertex_t));

	return pVBuffer;
}

void GrassType::Realize(TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>& gassrender)
{
	if ((NULL == m_pTexture) || (!m_pTexture->IsLoadComplete()))
	{
		return;
	}
	
	Terrain* pTerrain = m_pTerrain;
	// 花草密度
	float grass_lod = pTerrain->GetGrassLod();

	if (grass_lod < 0.01F)
	{
		// 密度近乎为0时不进行描画
		return;
	}

	IRenderContext* pContext = m_pRender->GetContext();

 
	DrawGBuffer(gassrender);
 
	m_bMustRefresh = false;
}

void GrassType::RenderChunkGrass(void* pdata)
{
    size_t index = (size_t)pdata;

	CTerrainChunk::chunk_grass_cb_per_render_t*  pthis = &(*p_s_gassrender)[index];// (CTerrainChunk::chunk_grass_cb_per_render_t*)pdata;

	CTerrainChunk* pChunk = (CTerrainChunk*)pthis->pChunk;

	pChunk->SetGrassShader(pthis->pShader,pthis->pIB,pthis->pVB,pthis->grass_num,pthis->pShaderHandle);
	pChunk->SetGrassTex(pthis->pTexture);
 
    //glUseProgram(ShaderID);
	pChunk->GetGrassShader()->UsedShader();
	pChunk->UseGrassObject();
	pChunk->UseGrassChunkObject();
	pChunk->UseGrassTexture();
	pChunk->UseGrassNormalSceneCB();

	pChunk->DrawGrass(pChunk->GetGrassShader());
	//glUseProgram(0);

}

void GrassType::RenderEarlyZChunkGrass(void* pdata)
{
    size_t index = (size_t)pdata;

	CTerrainChunk::chunk_grass_cb_per_render_t*  pthis = &(*p_s_gassrender)[index];// (CTerrainChunk::chunk_grass_cb_per_render_t*)pdata;

	CTerrainChunk* pChunk = (CTerrainChunk*)pthis->pChunk;

	pChunk->SetGrassShader(pthis->pEarlyZPassShader,pthis->pIB,pthis->pVB,pthis->grass_num,pthis->pEarlyZShaderHandle);
	pChunk->SetGrassTex(pthis->pTexture);
 
    //glUseProgram(ShaderID);
	pChunk->GetGrassShader()->UsedShader();
	pChunk->UseGrassObject();
	pChunk->UseGrassChunkObject();
	pChunk->UseGrassTexture();
	pChunk->UseGrassNormalSceneCB();

	pChunk->DrawGrass(pChunk->GetGrassShader());

}

void GrassType::DrawGBuffer(TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>& gassrender)
{ 
	if(m_pTexture == NULL)
		return;	

	p_s_gassrender = &gassrender;
 
	Terrain* pTerrain = m_pTerrain;
	IRenderContext* pContext = m_pRender->GetContext();
	// 是否开启指数雾效
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
	// 是否开启线性雾效
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;
	// 雾的起始位置
	float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);


	float grass_radius = pTerrain->GetGrassRadius();
	CTerrainPainter* pPainter = pTerrain->GetPainter();
	CTerrainCulling* pCulling = pTerrain->GetCulling();
	size_t chunk_num = pCulling->GetRenderChunkCount();
	CTerrainChunk** chunks = pCulling->GetRenderChunks();
	int ground_sort_order = pTerrain->GetGroundSortOrder();
	float max_camera_distance;

	if (ground_sort_order != 0)
	{
		float size = pTerrain->GetChunkScale() * pTerrain->GetUnitSize();

		FmVec2 vec2temp(size, size);
		max_camera_distance = FmVec2Length(&vec2temp) 
			+ grass_radius;
	}

	bool prepared = false;
	bool begin = false;

	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];

		if (ground_sort_order != 0)
		{
			if (pChunk->GetCameraDistance() > max_camera_distance)
			{
				// 结束遍历，因为后面的地块肯定都超出范围
				break;
			}
		}

		if (pChunk->GetNoAnyGrass())
		{
			// 优化，没有任何草信息时立即排除
			continue;
		}

		// 花草数量
		size_t grass_num = pChunk->GetGrassCount((int)m_nIndex);

		if (0 == grass_num)
		{
			continue;
		}

		float radius = pChunk->GetRadius() + grass_radius;

		if (pChunk->GetCameraDistance() > radius)
		{
			// 超出范围
			continue;
		}

		IStaticVB* pVBuffer = pChunk->GetGrassVertexBuffer((int)m_nIndex);

		if (m_bMustRefresh || (NULL == pVBuffer))
		{
			if (!prepared)
			{
				// 这样可以避免无意义的调用此函数
				PrepareRealize();
				prepared = true;
			}

			// 创建花草顶点缓冲
			pVBuffer = Generate(pChunk, grass_num);

			if (NULL == pVBuffer)
			{
				continue;
			}

			// 保存花草顶点缓冲
			pChunk->SetGrassVertexBuffer((int)m_nIndex, pVBuffer, grass_num);
		}

		// 实际生成的矩形数量
		grass_num = pChunk->GetGrassActualCount((int)m_nIndex);

		IStaticIB* pIBuffer = m_pTerrain->GetGrassIndexBuffer(grass_num);

		if (NULL == pIBuffer)
		{
			// 可能花草太多导致索引缓冲溢出
			continue;
		}

		if (!begin)
		{
			if (!prepared)
			{
				// 这样可以避免无意义的调用此函数
				PrepareRealize();
				prepared = true;
			}

//			FillGroupCB(pChunk);
			begin = true;
		}
		FillGroupCB(pChunk);

		bool use_light_map = false;
		bool use_fog = fog_linear && 
			((pChunk->GetCameraDistance() + pChunk->GetRadius()) >= fog_start);

		CTerrainChunk::chunk_grass_cb_per_render_t grass_render;
		grass_render.grass_num = grass_num;
		grass_render.pChunk = pChunk;
		grass_render.pIB = pIBuffer;
		grass_render.pVB = pVBuffer;
		grass_render.pTexture = m_pTexture;
		grass_render.pShaderHandle = GetShaderHandle(use_fog, fog_exp, false, false, use_light_map);
		grass_render.pShader = grass_render.pShaderHandle->pShader;
		if(pContext->GetEnableEarlyZ())
		{
			grass_render.pEarlyZShaderHandle = GetShaderHandle(use_fog, fog_exp, false, false, use_light_map,true);
			grass_render.pEarlyZPassShader = grass_render.pEarlyZShaderHandle->pShader;
		}


		/*
		pChunk->SetGrassShader(pShader,pIBuffer,pVBuffer,grass_num);
		pChunk->SetGrassTex(m_pTexture);
		*/
		gassrender.push_back(grass_render);

		if(pContext->GetEnableEarlyZ())
		{
			m_pRender->GetSceneView()->AddEarlyZBatch(RenderChunkGrass, (void*)(gassrender.size()-1),radius,"ChunkGrassEarlyZ",RenderEarlyZChunkGrass);
		}
		else
		{
			m_pRender->GetSceneView()->AddGrassBatch(RenderChunkGrass, (void*)(gassrender.size()-1),"ChunkGrass");
		}
	}

	m_bMustRefresh = false;
}

void GrassType::FillGroupCB(CTerrainChunk* pChunk)
{
	CTerrainChunk::chunk_grass_cb_per_group_t  grass_cb_per_group;

	// 消隐距离
	float fade_begin = m_pTerrain->GetGrassRadius() * 0.7F;
	float fade_end = m_pTerrain->GetGrassRadius() * 1.3F;
    // 风向
	grass_cb_per_group.vWinDir = FmVec3(m_vWinDir.x, m_vWinDir.y, m_vWinDir.z);
	// 风的强度
	grass_cb_per_group.fWinForce = m_fWinForce;
	grass_cb_per_group.fTimer = (float)fmod(GetCore()->GetTotalSeconds(), 10000.0);
	grass_cb_per_group.FadeParam =  FmVec2(fade_end, 1.0F / (fade_end - fade_begin));
	grass_cb_per_group.vPlayerPoint = FmVec3(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);
 
	// Set material
	Terrain::material_t& m = m_pTerrain->GetMaterial();
 
	grass_cb_per_group.MaterialDiffuse = FmVec4(m.vDiffuse.x, m.vDiffuse.y, m.vDiffuse.z,1.0f);
	grass_cb_per_group.MaterialAmbient = FmVec4(m.vAmbient.x, m.vAmbient.y, m.vAmbient.z,1.0f);
	pChunk->SetGrassPerGroup(grass_cb_per_group);
}

CTerrainChunk::grass_shader_handle_t* GrassType::GetShaderHandle(bool fog_linear, bool fog_exp, bool dynamic_shadow,bool prelight, bool lightmap ,bool bEarlyZ )
{
	size_t shader_flag = (size_t(fog_linear) << GRASS_FLAG_FOGLINEAR)
		+ (size_t(fog_exp) << GRASS_FLAG_FOGEXP) 
		+ (size_t(dynamic_shadow) << GRASS_FLAG_SHADOWEDMAP)	
		+ (size_t(prelight) << GRASS_FLAG_PRELIGHT)
		+ (size_t(lightmap) << GRASS_FLAG_LIGHTMAP)
		+ (size_t(bEarlyZ) << GRASS_FLAG_EARLYZ);

	if(!m_ShaderHandleMap.Exists(shader_flag))
	{
		IVertexShader* pVS = m_VSList.GetShader(shader_flag);
		IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
		IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS,pPS,grass_vertex_t_name,3);
		CTerrainChunk::grass_shader_handle_t* pShaderHandle = (CTerrainChunk::grass_shader_handle_t*)CORE_ALLOC(sizeof(CTerrainChunk::grass_shader_handle_t));

		pShaderHandle->pShader = pShader;

		IShaderParamOp* pShaderOp = pShader->GetParamOp();
		//UseGrassObject
		pShaderHandle->vec3WinDirHandle = pShaderOp->FindParamIdByName("c_vWinDir");
		pShaderHandle->fWinForceHandle = pShaderOp->FindParamIdByName("c_fWinForce");
        pShaderHandle->fTimerHandle =  pShaderOp->FindParamIdByName("c_fTimer");
        pShaderHandle->vec2FadeParamHandle = pShaderOp->FindParamIdByName("c_FadeParam");
		pShaderHandle->vec3PlayerPointHandle = pShaderOp->FindParamIdByName("c_vPlayerPoint");
		pShaderHandle->vec4MaterialDiffuseHandle = pShaderOp->FindParamIdByName("c_MaterialDiffuse");
		pShaderHandle->vec4MaterialAmbientHandle = pShaderOp->FindParamIdByName("c_MaterialAmbient");
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

        //UseGrassTexture
        pShaderHandle->tex_DiffuseHandle =  pShaderOp->FindParamIdByName("tex_Diffuse");
		//UseGrassNormalSceneCB
        pShaderHandle->mat4mtxViewProjHandle =  pShaderOp->FindParamIdByName("c_mtxViewProj");
        pShaderHandle->mat4mtxViewHandle =  pShaderOp->FindParamIdByName("c_mtxView");
        pShaderHandle->mat4mtxProjHandle =  pShaderOp->FindParamIdByName("c_mtxProj");
        pShaderHandle->mat4mtxViewProjInverseHandle =  pShaderOp->FindParamIdByName("c_mtxViewProjInverse");
        pShaderHandle->mat4mtxViewInverseHandle =  pShaderOp->FindParamIdByName("c_mtxViewInverse"); 
        pShaderHandle->mat4mtxProjInverseHandle =  pShaderOp->FindParamIdByName("c_mtxProjInverse");    
        pShaderHandle->vec3ViewPosHandle =  pShaderOp->FindParamIdByName("c_vViewPos");
        pShaderHandle->vec3LightDirHandle =  pShaderOp->FindParamIdByName("c_vLightDir"); 
        pShaderHandle->vec3LightAmbientHandle =  pShaderOp->FindParamIdByName("c_LightAmbient");
        pShaderHandle->fReflectFactorHandle =  pShaderOp->FindParamIdByName("c_fReflectFactor");
        pShaderHandle->vec3LightDiffuseHandle =  pShaderOp->FindParamIdByName("c_LightDiffuse");
        pShaderHandle->vec4FogColorHandle =  pShaderOp->FindParamIdByName("c_FogColor");
        pShaderHandle->vec4FogParamHandle =  pShaderOp->FindParamIdByName("c_FogParam");
        pShaderHandle->vec4FogExpParamHandle =  pShaderOp->FindParamIdByName("c_FogExpParam");
        pShaderHandle->vec2PixelSizeHandle =  pShaderOp->FindParamIdByName("c_PixelSize");
        pShaderHandle->fStereoConvergenceHandle =  pShaderOp->FindParamIdByName("c_fStereoConvergence");
        pShaderHandle->fCameraNearZHandle =  pShaderOp->FindParamIdByName("c_fCameraNearZ");
        pShaderHandle->vec2HalfPixelSizeHandle =  pShaderOp->FindParamIdByName("c_HalfPixelSize");
        pShaderHandle->vec2BlendHalfPixeHandle =  pShaderOp->FindParamIdByName("c_BlendHalfPixel");
        pShaderHandle->vec3ReferPosHandle =  pShaderOp->FindParamIdByName("c_vReferPos");
        pShaderHandle->vec2DepthParamHandle =  pShaderOp->FindParamIdByName("c_DepthParam");
        pShaderHandle->fFarClipDistanceHandle =  pShaderOp->FindParamIdByName("c_fFarClipDistance");
        pShaderHandle->fAlphaRefHandle =  pShaderOp->FindParamIdByName("c_fAlphaRef");
        pShaderHandle->vec3PointLightPosHandle =  pShaderOp->FindParamIdByName("c_vPointLightPos");
        pShaderHandle->fPointLightRangeHandle =  pShaderOp->FindParamIdByName("c_fPointLightRange");
        pShaderHandle->vec4PointLightDiffuseHandle =  pShaderOp->FindParamIdByName("c_fPointLightRange");
        pShaderHandle->vec4CameraLightDiffuseHandle =  pShaderOp->FindParamIdByName("c_CameraLightDiffuse");
        pShaderHandle->vec4ClipPlaneHandle =  pShaderOp->FindParamIdByName("c_ClipPlane");

		m_ShaderHandleMap.Add(shader_flag,pShaderHandle);
	} 

	return m_ShaderHandleMap.Find(shader_flag).GetData();
}