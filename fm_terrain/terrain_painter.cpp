//--------------------------------------------------------------------
// 文件名:		terrain_painter.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_painter.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_light.h"
#include "terrain_chunk.h"
#include "terrain_height.h"
#include "terrain_visuals.h"
#include "zone_manager.h"
#include "tex_alpha_map.h"
#include "tex_light_map.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "quad_visual.h"
#include "quad_node.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"
#include "../visual/i_texture.h"
#include "../visual/i_render.h"
//#include "../visual/dx_scene_batch.h"
#include "../visual/vis_utils.h"
#include "../visual/i_scene_view.h"
#include "../public/var.h"
#include "../public/core_log.h"

// 每个地块不同的常量
#define CB_DETAIL1_PARAM		0
#define CB_DETAIL2_PARAM		4
#define CB_DETAIL3_PARAM		8
#define CB_DETAIL4_PARAM		12
#define CB_CHUNK_POS			16
#define CB_ZONE_OFFSET_SIZE		17
#define CB_BASE_SPECULAR		18
#define CB_BLEND_SPECULAR_LEVEL 19
#define CB_BLEND_SPECULAR_POWER 20
#define CB_BLEND_HEIGHT_SCALE	21
#define CB_CHUNK_MAX			22

// 所有地块相同的常量
#define CB_GRADUALLY			0
#define CB_BLOCK_SIZE			1
#define CB_DETAIL0_PARAM		2
#define CB_AMBIENT_MATERIAL		3
#define CB_DIFFUSE_MATERIAL		4
#define CB_SPECULAR_MATERIAL	5
#define CB_TESSELLATION_FACTOR	6
#define CB_GROUP_MAX			7

// CTerrainPainter

CTerrainPainter::CTerrainPainter()
{
	m_pTerrain = NULL;
	m_pRender = NULL;
	m_pGBufferTessVS = NULL;
	m_pShadowMapVS = NULL;
	m_pShadowMapTessVS = NULL;
	m_pShadowMapPS = NULL;
	m_pInFogVS = NULL;
	m_pInFogPS = NULL;
	m_pInFogDepthVS = NULL;
	m_pInFogDepthPS = NULL;
	m_pInFogGBufferVS = NULL;
	m_pInFogGBufferPS = NULL;
	m_pInFogVB = NULL;
	m_pInFogIB = NULL;
	m_nInFogVBSize = 0;
	m_nInFogIBSize = 0;
}

CTerrainPainter::~CTerrainPainter()
{
	SAFE_RELEASE(m_pInFogVB);
	SAFE_RELEASE(m_pInFogIB);

#define shadermap 	TPodHashMap<unsigned int, CTerrainChunk::chunk_cb_object_shader_handle_t*, TPodTraits<unsigned int>,TTerrainPainterShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMapHandle.Begin(); iter != m_ShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE( iter.GetData(), sizeof(CTerrainChunk::chunk_cb_object_shader_handle_t));
	}
#undef shadermap
}

bool CTerrainPainter::Initialize(IRender* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	
	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("terrain.vsh", "vs_main_terrain");
	m_VSList.SetOption(0, "#define BLEND0 1\n");
	m_VSList.SetOption(1, "#define BLEND0 1\n #define BLEND1 1\n");
	m_VSList.SetOption(2, "#define BLEND0 1\n #define BLEND1 1\n #define BLEND2 1\n");
	m_VSList.SetOption(3, "#define BLEND0 1\n #define BLEND1 1\n #define BLEND2 1\n #define BLEND3 1\n");
	m_VSList.SetOption(4, "#define BLEND0 1\n #define BLEND1 1\n #define BLEND2 1\n #define BLEND3 1\n #define BLEND4 1\n");
	m_VSList.SetDefine(FLAG_LIGHTMAP, "#define LIGHTMAP 1\n");
	m_VSList.SetDefine(FLAG_SHADOWEDMAP, "#define SHADOWEDMAP 1\n");
	m_VSList.SetDefine(FLAG_GRADUALLY, "#define GRADUALLY_DISAPPEAR 1\n");
	m_VSList.SetDefine(FLAG_FOGLINEAR, "#define FOGLINEAR 1\n");
	m_VSList.SetDefine(FLAG_NORMALMAP, "#define NORMALMAP 1\n");
	m_VSList.SetDefine(FLAG_SPECULAR, "#define SPECULAR 1\n");
	m_VSList.SetDefine(FLAG_PRELIGHT, "#define PRELIGHT 1\n");
	m_VSList.SetDefine(FLAG_SMOOTHCLIP, "#define SMOOTHCLIP 1\n");
	m_VSList.SetDefine(FLAG_FIXSHADOW, "#define FIXSHADOW 1\n");
	m_VSList.SetDefine(FLAG_FOGEXP, "#define FOGEXP 1\n");
	m_VSList.SetDefine(FLAG_SPHEREAMBIENT, "#define SPHEREAMBIENT 1\n");
	m_VSList.SetDefine(FLAG_CLIPPLANE, "#define CLIPPLANE 1\n");
	m_VSList.SetDefine(FLAG_HEIGHT_FOG, "#define HEIGHT_FOG 1\n");

	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("terrain.fsh", "ps_main_terrain");
	m_PSList.SetOption(0, "#define BLEND0 1\n");
	m_PSList.SetOption(1, "#define BLEND0 1\n#define BLEND1 1\n");
	m_PSList.SetOption(2, "#define BLEND0 1\n#define BLEND1 1\n#define BLEND2 1\n");
	m_PSList.SetOption(3, "#define BLEND0 1\n#define BLEND1 1\n#define BLEND2 1\n#define BLEND3 1\n");
	m_PSList.SetOption(4, "#define BLEND0 1\n#define BLEND1 1\n#define BLEND2 1\n#define BLEND3 1\n#define BLEND4 1\n");
	m_PSList.SetDefine(FLAG_LIGHTMAP, "#define LIGHTMAP 1\n");
	m_PSList.SetDefine(FLAG_SHADOWEDMAP, "#define SHADOWEDMAP 1\n");
	m_PSList.SetDefine(FLAG_GRADUALLY, "#define GRADUALLY_DISAPPEAR 1\n");
	m_PSList.SetDefine(FLAG_FOGLINEAR, "#define FOGLINEAR 1\n");
	m_PSList.SetDefine(FLAG_NORMALMAP, "#define NORMALMAP 1\n");
	m_PSList.SetDefine(FLAG_SPECULAR, "#define SPECULAR 1\n");
	m_PSList.SetDefine(FLAG_PRELIGHT, "#define PRELIGHT 1\n");
	m_PSList.SetDefine(FLAG_SMOOTHCLIP, "#define SMOOTHCLIP 1\n");
	m_PSList.SetDefine(FLAG_FIXSHADOW, "#define FIXSHADOW 1\n");
	m_PSList.SetDefine(FLAG_FOGEXP, "#define FOGEXP 1\n");
	m_PSList.SetDefine(FLAG_SPHEREAMBIENT, "#define SPHEREAMBIENT 1\n");
	m_PSList.SetDefine(FLAG_CLIPPLANE, "#define CLIPPLANE 1\n");
	m_PSList.SetDefine(FLAG_HEIGHT_FOG, "#define HEIGHT_FOG 1\n");
	
	m_nInFogVBSize = 0x1000 * sizeof(FmVec4);
	m_nInFogIBSize = 0x2000 * sizeof(unsigned short);
	m_pInFogVB = m_pRender->CreateDynamicVB((unsigned int)m_nInFogVBSize,"CTerrainPainter::Initialize");
	m_pInFogIB = m_pRender->CreateDynamicIB((unsigned int)m_nInFogIBSize);

	return true;
}

void CTerrainPainter::DrawDesignHelper()
{
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	size_t render_chunk_num = pCulling->GetRenderChunkCount();
	CTerrainChunk** render_chunks = pCulling->GetRenderChunks();
	float collide_radius = m_pTerrain->GetCollideRadius();
	
	for (size_t k = 0; k < render_chunk_num; ++k)
	{
		CTerrainChunk* chunk = render_chunks[k];
		
		if (chunk->GetInDesign())
		{
			if (0 != m_pTerrain->GetShowDesignLine())
			{
				chunk->PrepareDesignLine();
				chunk->PaintDesignLine();
			}

			if (m_pTerrain->GetShowWalkable() 
				&& GetShowDesignHelper(chunk, collide_radius))
			{
				chunk->PaintWalkable();
			}

			if (m_pTerrain->GetShowRegion() 
				&& GetShowDesignHelper(chunk, collide_radius))
			{
				chunk->PaintRegion(m_pTerrain->GetShowRegionName());
			}

			if (m_pTerrain->GetShowArea() 
				&& GetShowDesignHelper(chunk, collide_radius))
			{
				chunk->PaintArea();
			}

			if (chunk->GetShowBoundBox() 
				&& GetShowDesignHelper(chunk, 100.0F))
			{
				chunk->PaintBoundBox();
			}
		}
	}
}

bool CTerrainPainter::GetShowDesignHelper(const CTerrainChunk* chunk, 
	const float distance) const
{
	// Chunk的中心位置
	FmVec2 chunk_pos(chunk->GetCenter().x, chunk->GetCenter().z);

	// 如果存在玩家，仅判断玩家和Chunk的平面距离即可
	if (!m_pTerrain->GetPlayerID().IsNull())
	{
		// 存在玩家的情况
		IVisBase* pPlayer = (IVisBase*)m_pTerrain->GetCore()->GetEntity(
			m_pTerrain->GetPlayerID());

		if (pPlayer)
		{
			FmVec2 player_pos(pPlayer->GetPosition().x, 
				pPlayer->GetPosition().y);

			FmVec2 vec2Temp = player_pos - chunk_pos;
			if (FmVec2Length(&vec2Temp) < distance)
			{
				// Chunk与玩家的平面距离如果小于指定的距离
				// 则允许显示消耗大的辅助线条
				return true;
			}

			return false;
		}
	}

	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& cam = pContext->GetCamera();
	FmVec2 cam_pos(cam.vPosition.x, cam.vPosition.z);

	FmVec2 vec2Temp = cam_pos - chunk_pos;
	if (FmVec2Length(&vec2Temp) < distance)
	{
		// Chunk的平面距离如果小于指定的距离，则允许显示消耗大的辅助线条
		return true;
	}

	return false;
}

// 显示辅助模型
static inline void draw_helper_model(IVisBase* pVisBase, 
	IVisBase* pHelperModel)
{
	FmVec3 pos = pVisBase->GetPosition();
	FmVec3 ang = pVisBase->GetAngle();

	pHelperModel->SetPosition(pos.x, pos.y, pos.z);
	pHelperModel->SetAngle(ang.x, ang.y, ang.z);
	pHelperModel->SetScale(1.0F, 1.0F, 1.0F);
	pHelperModel->Realize();
}

// 显示大小适中的辅助模型
static inline void draw_scaled_helper_model(IVisBase* pVisBase, 
	IVisBase* pHelperModel)
{
	FmVec3 pos = pVisBase->GetPosition();
	FmVec3 ang = pVisBase->GetAngle();
	float scale = pVisBase->GetRadius();

	if (scale < 0.1F)
	{
		scale = 0.1F;
	}

	if (scale > 1.0F)
	{
		scale = 1.0F;
	}

	pHelperModel->SetPosition(pos.x, pos.y, pos.z);
	pHelperModel->SetAngle(ang.x, ang.y, ang.z);
	pHelperModel->SetScale(scale, scale, scale);
	pHelperModel->Realize();
}

// 更新物件的描述信息
static inline void update_visual_desc(IVisBase* pVisBase, Terrain* pTerrain)
{
	// 取物体的描述信息
	const char* desc = VisUtil_GetCustomString(pVisBase, "desc");

	if (!StringEmpty(desc))
	{
		// 描述信息不为空才调用
//		VisUtil_RunCallback(pTerrain, "on_visual_update_desc", 
//			CVarList() << pVisBase->GetID());
	}
}

void CTerrainPainter::DrawVisuals()
{
	Terrain* pTerrain = m_pTerrain;
	CTerrainCulling* pCulling = pTerrain->GetCulling();
	size_t render_visual_num = pCulling->GetRenderVisualCount();
	render_visual_t* render_visuals = pCulling->GetRenderVisuals();
	size_t render_role_num = pCulling->GetRenderRoleCount();
	render_role_t* render_roles = pCulling->GetRenderRoles();
	bool visual_fade_in = pTerrain->GetVisualFadeIn();
	bool design_mode = pTerrain->GetDesignMode();
	bool show_icon = pTerrain->GetShowIcon();
	bool show_collide = pTerrain->GetShowCollide();
	bool show_tree = pTerrain->GetShowTree();
	ICore* pCore = pTerrain->GetCore();

	// 渲染人物时要重新设置光照
	IRenderContext* pContext = m_pRender->GetContext();
	bool camera_under_water = pContext->GetInt(IRenderContext::I_UNDER_WATER) != 0;
	
	for (size_t k = 0; k < render_role_num; ++k)
	{
		if (!render_roles[k].bVisible)
		{
			continue;
		}
		
		IVisBase* pVisBase = render_roles[k].pVisBase;

		pVisBase->SetUnderWater(
			camera_under_water || render_roles[k].bUnderWater);
		pVisBase->Realize();
	}

	// 渲染静态场景物体时关闭轮廓光和摄像机光源
	int old_enable_role_falloff = pContext->GetInt(
		IRenderContext::I_ENABLE_ROLE_FALLOFF);
	int old_enable_camera_light = pContext->GetInt(
		IRenderContext::I_ENABLE_CAMERA_LIGHT);

	pContext->SetInt(IRenderContext::I_ENABLE_ROLE_FALLOFF, 0);
	pContext->SetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT, 0);

	if (design_mode)
	{
		for (size_t i = 0; i < render_visual_num; ++i)
		{
			IVisBase* pVisBase = render_visuals[i].pVisBase;
			bool need_fade_in = true;

			// 显示不可见的物体
			if (pVisBase->GetVisible())
			{
				if (pVisBase->GetEntInfo()->IsKindOf("LightSource"))
				{
					IVisBase* pModel = m_pTerrain->GetLightModel();

					if (show_icon && pModel)
					{
						draw_helper_model(pVisBase, pModel);
					}

					// 是否显示点光源范围
					CVar prop(VTYPE_BOOL, m_pTerrain->GetShowLightRange());

					pCore->SetProperty(pVisBase, "ShowRange", prop);

					// 不可使用淡入
					need_fade_in = false;
				}
				else if (pVisBase->GetEntInfo()->IsKindOf("Sound"))
				{
					IVisBase* pModel = m_pTerrain->GetSoundModel();

					if (show_icon && pModel)
					{
						draw_helper_model(pVisBase, pModel);
					}
				}
				else if (pVisBase->GetEntInfo()->IsKindOf("Particle"))
				{
					IVisBase* pModel = m_pTerrain->GetParticleModel();

					if (show_icon && pModel)
					{
						draw_scaled_helper_model(pVisBase, pModel);
					}
				}
				else if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
				{
					IVisBase* pModel = m_pTerrain->GetEffectModel();

					if (show_icon && pModel)
					{
						draw_scaled_helper_model(pVisBase, pModel);
					}
				}
				else if (pVisBase->GetEntInfo()->IsKindOf("Trigger"))
				{
					IVisBase* pModel = m_pTerrain->GetTriggerModel();

					if (show_icon && pModel)
					{
						draw_helper_model(pVisBase, pModel);
					}
					
					// 是否显示边框
					CVar prop(VTYPE_BOOL, show_icon);

					pCore->SetProperty(pVisBase, "ShowBound", prop);
				}
				else if (show_collide 
					&& pVisBase->GetEntInfo()->IsKindOf("Model"))
				{
					bool walkable = VisUtil_GetCustomBool(pVisBase, 
						"co_walkable");
					bool gen_roof = VisUtil_GetCustomBool(pVisBase, 
						"co_gen_roof");
					bool gen_wall = VisUtil_GetCustomBool(pVisBase, 
						"co_gen_wall");
					bool through = VisUtil_GetCustomBool(pVisBase, 
						"co_through");

					if (walkable || gen_roof || gen_wall || through)
					{
						unsigned int color = 0xFFFFFFFF;
						
						if (walkable && gen_roof && gen_wall)
						{
							color = 0xFFFF0080;
						}
						else if (walkable && gen_roof)
						{
							color = 0xFF00FFFF;
						}
						else if (walkable && gen_wall)
						{
							color = 0xFF8000FF;
						}
						else if (gen_roof && gen_wall)
						{
							color = 0xFFFF00FF;
						}
						else if (walkable)
						{
							color = 0xFF00FF00;
						}
						else if (gen_roof)
						{
							color = 0xFF0000FF;
						}
						else if (gen_wall)
						{
							color = 0xFF800080;
						}

						if (through)
						{
							color &= 0x80FFFFFF;
						}

						unsigned int old_color = pVisBase->GetColor();

						pVisBase->SetColor(color);
						pVisBase->SetUnderWater(camera_under_water 
							|| render_visuals[i].bUnderWater);
						pVisBase->Realize();
						pVisBase->SetColor(old_color);
						continue;
					}
				}
			}

			// 更新物件的描述信息
			update_visual_desc(pVisBase, m_pTerrain);

			if (VisUtil_GetCustomBool(pVisBase, "is_tree"))
			{
				if (!show_tree)
				{
					// 显示树木的替代模型
					IVisBase* pModel = m_pTerrain->GetTreeModel();

					if (pModel)
					{
						draw_helper_model(pVisBase, pModel);
					}
					
					continue;
				}
			}

			if (visual_fade_in && need_fade_in)
			{
				float alpha = pVisBase->GetAlphaValue();

				if (alpha < 1.0F)
				{
					unsigned int old_color = pVisBase->GetColor();
					unsigned int color = ((unsigned int)(alpha * 255.0F) << 24) 
						+ 0xFFFFFF;

					pVisBase->SetColor(color);
					pVisBase->SetUnderWater(
						camera_under_water || render_visuals[i].bUnderWater);
					pVisBase->Realize();
					pVisBase->SetColor(old_color);
					continue;
				}
			}

			pVisBase->SetUnderWater(
				camera_under_water || render_visuals[i].bUnderWater);
			pVisBase->Realize();
		}
	}
	else if (visual_fade_in)
	{
		for (size_t i = 0; i < render_visual_num; ++i)
		{
			IVisBase* pVisBase = render_visuals[i].pVisBase;
			float alpha = pVisBase->GetAlphaValue();

			if (alpha < 1.0F)
			{
				unsigned int old_color = pVisBase->GetColor();
				unsigned int color = ((unsigned int)(alpha * 255.0F) << 24) 
					+ 0xFFFFFF;

				pVisBase->SetColor(color);
				pVisBase->SetUnderWater(
					camera_under_water || render_visuals[i].bUnderWater);
				pVisBase->Realize();
				pVisBase->SetColor(old_color);
			}
			else
			{
				pVisBase->SetUnderWater(
					camera_under_water || render_visuals[i].bUnderWater);
				pVisBase->Realize();
			}
		}
	}
	else
	{
		for (size_t i = 0; i < render_visual_num; ++i)
		{
			IVisBase* pVisBase = render_visuals[i].pVisBase;

			pVisBase->SetUnderWater(
				camera_under_water || render_visuals[i].bUnderWater);
			pVisBase->Realize();
		}
	}
	
	pContext->SetInt(IRenderContext::I_ENABLE_ROLE_FALLOFF, 
		old_enable_role_falloff);
	pContext->SetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT, 
		old_enable_camera_light);
}

void CTerrainPainter::DrawSimpleGround()
{
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	render_zone_t* pRenderZones = pCulling->GetRenderZones();
	size_t nRenderZoneNum = pCulling->GetRenderZoneCount();
	bool horizon_cull = m_pTerrain->GetHorizontalCulling();

	DrawChunksShader(pRenderZones, nRenderZoneNum, horizon_cull,
		true, -1);

	if (m_pTerrain->GetSmoothHorizontal())
	{
		if (pCulling->GetInFogChunkCount() > 0)
		{
			DrawInFog(pRenderZones, nRenderZoneNum, horizon_cull);
		}
	}

	// 绘制设计线
	if (m_pTerrain->GetDesignMode()) 
	{
		DrawDesignHelper();
	}
}

void CTerrainPainter::DrawChunks()
{
	Terrain* pTerrain = m_pTerrain;
	CTerrainCulling* pCulling = pTerrain->GetCulling();
	render_zone_t* pRenderZones = pCulling->GetRenderZones();
	size_t nRenderZoneNum = pCulling->GetRenderZoneCount();
	bool horizon_cull = pTerrain->GetHorizontalCulling();

	//绘制地表
	DrawChunksShader(pRenderZones, nRenderZoneNum, horizon_cull,
		false, -1);

	
	if (pTerrain->GetSmoothHorizontal())
	{
		if (pCulling->GetInFogChunkCount() > 0)
		{
			DrawInFog(pRenderZones, nRenderZoneNum, horizon_cull);
		}
	}

	// 绘制设计线
	if (pTerrain->GetDesignMode()) 
	{
		DrawDesignHelper();
	}
	else if (pTerrain->GetShowWalkable())
	{
		size_t render_chunk_num = pCulling->GetRenderChunkCount();
		CTerrainChunk** render_chunks = pCulling->GetRenderChunks();

		for (size_t k = 0; k < render_chunk_num; ++k)
		{
			CTerrainChunk* chunk = render_chunks[k];

			if (chunk->GetCameraDistance() < 25.0F 
				|| GetShowDesignHelper(chunk, 25.0F))
			{
				chunk->PaintWalkable();
			}
		}
	}
}

void CTerrainPainter::DrawEnvMapSimpleGround(size_t index)
{
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	env_map_collect_t* pCollect = pCulling->GetEnvMapCollect(index);

	DrawChunksShader(pCollect->pZones, pCollect->nZoneNum, 
		false, true, -1);
}

void CTerrainPainter::DrawEnvMapChunks(size_t index)
{
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	env_map_collect_t* pCollect = pCulling->GetEnvMapCollect(index);

	DrawChunksShader(pCollect->pZones, pCollect->nZoneNum, 
		false, false, -1);
}

void CTerrainPainter::DrawEnvMapVisuals(size_t index)
{
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	env_map_collect_t* pCollect = pCulling->GetEnvMapCollect(index);
	IVisBase** visuals = pCollect->pVisuals;
	size_t visual_num = pCollect->nVisualNum;

	for (size_t i = 0; i < visual_num; ++i)
	{
		visuals[i]->Realize();
	}
}


void CTerrainPainter::UpdateChunkGroupCB(CTerrainChunk* chunk)
{
	IRenderContext* pContext = m_pRender->GetContext();
	Terrain::material_t& mat = m_pTerrain->GetMaterial();
	float unit_size = m_pTerrain->GetUnitSize();
	float tex_unit = m_pTerrain->GetTextureUnits() * unit_size;
	float chunk_size = m_pTerrain->GetChunkScale() * unit_size;
	chunk->SetMaterial(&mat);

	// 地块尺寸
	FmVec4 terrain_block_size = ( chunk_size, chunk_size, chunk_size, chunk_size );
	// 设置渐消距离参数
	FmVec2 gradually = ( m_pTerrain->GetAlphaFadeRadius(),
		m_pTerrain->GetAlphaHideRadius() - m_pTerrain->GetAlphaFadeRadius());
	// 基础贴图的坐标转换参数
	FmVec2 detail0_param= ( 1 / tex_unit, -1 / tex_unit);
	
	float t_factor = m_pTerrain->GetTessellationFactor();
	FmVec4 tess_factor = ( t_factor, t_factor, t_factor / 2.0F, 0.2F );
    chunk->SetChunkGroupCB(terrain_block_size,gradually,detail0_param,tess_factor);

}

const char* terrain_vertex_t_name[] = 
{
	"iPos",
	"iDiffuseNormal",
};


void CTerrainPainter::DrawChunksShader(render_zone_t* zones,
	size_t zone_num, bool horizon_cull, bool simple, int level)
{
	IRenderContext* pContext = m_pRender->GetContext();
	bool smooth_horizon = m_pTerrain->GetSmoothHorizontal();
	bool enable_normalmap = m_pTerrain->GetEnableNormalMap();
	// 是否开启指数雾效
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
	// 是否开启线性雾效
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;
	// 高度雾效
	bool height_fog = pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0;

	// 雾的起始位置
	float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
	// 是否使用动态阴影
	bool dynamic_shadow = pContext->GetEnableDynamicShadow();//pContext->GetEnableRealizeDepth() 
  //		&& pContext->GetEnableDynamicShadow();
	// 是否预渲染光照图
	bool prelight = false;//dynamic_shadow && pContext->GetEnableLightPrepass();
	// 是否开启高光
	bool use_specular = m_pTerrain->GetMaterial().bSpecularEnable;
	// 修正开启抗锯齿时的阴影边缘问题
	bool fix_shadow = false;//dynamic_shadow && pContext->GetEnableFixShadow()
//		&& (m_pRender->GetMultiSampleType() > 1);
	// 半球环境光
	bool sphere_ambient = dynamic_shadow && m_pTerrain->GetSphereAmbient();
	// 是否使用剪裁面
	bool use_clip_plane = pContext->GetInt(IRenderContext::I_USE_CLIP_PLANE) != 0;

	if (simple)
	{
		dynamic_shadow = false;
		prelight = false;
		use_specular = false;
		enable_normalmap = false;
		fix_shadow = false;
		sphere_ambient = false;
	}
  
	const FmVec3& refer_pos = pContext->GetReferPosition();

 
	// 扫描每个zone
	CBlendTexSet* pBaseTexSet = m_pTerrain->GetBaseTexSet();
	CBlendTexSet* pBlendTexSet = m_pTerrain->GetBlendTexSet();
	float alpha_fade_radius = m_pTerrain->GetAlphaFadeRadius();
	float alpha_hide_radius = m_pTerrain->GetAlphaHideRadius();
	float far_clip_distance = pContext->GetFarClipDistance();
	float shadow_distance = pContext->GetShadowDistance();
	float light_distance = 0.0;
	float zone_width = m_pTerrain->GetZoneWidth();
	float zone_height = m_pTerrain->GetZoneHeight();
//	CBlendTex* cur_base_tex = NULL;
	CZoneManager* pZoneManager = m_pTerrain->GetZoneManager();


	for (size_t zone = 0; zone < zone_num; zone++)
	{
		render_zone_t* render_zone = zones + zone;
		CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
			render_zone->nZoneIndex);

		if (NULL == pZone)
		{
			continue;
		}

		// 清除静默时间计数
		pZone->SetSilenceTime(0.0F);
        
		// 为区域设置光贴图
		CTerrainLight* pLight = pZone->GetLight();
		CTexLightMap* pTexLightMap = pLight->GetLightTex();
		
		if (NULL == pTexLightMap)
		{
			pTexLightMap = pLight->CreateLightTex();

			if (NULL == pTexLightMap)
			{
				continue;
			}
		}

		ITexture* pLightTex = pTexLightMap->GetTexture();

		// 是否使用光贴图
		bool light_map = (!dynamic_shadow)&&m_pRender->GetUseLightMap() && (pLightTex != NULL) && (!pLight->GetIsNull()) && pLightTex->IsReady();

		// 非动态光照不能使用法线贴图
		bool use_normal = enable_normalmap && (!light_map);
		// 扫描每个chunk并渲染
		size_t render_zone_chunk_num = render_zone->nChunkNum;

		for (size_t k = 0; k < render_zone_chunk_num; ++k)
		{
			CTerrainChunk* chunk = render_zone->pChunks[k];
			if (smooth_horizon)
			{
				if (chunk->GetInFog())
				{
					// 完全在雾中
					continue;
				}
			}

			if (horizon_cull)
			{
				// 地平线剪裁
				if (m_pTerrain->IsHorizonClip(chunk->GetMin(), 
					chunk->GetMax()))
				{
					continue;
				}
			}

			//更新公共chunk渲染想的数据
            UpdateChunkGroupCB(chunk);

			// 计算Chunk到摄影机之间的距离
			float dist_to_camera = chunk->GetCameraDistance();
			// 设置混合纹理
			size_t blend_num;

			if (simple)
			{
				blend_num = 0;
			}
			else
			{
				blend_num = chunk->GetBlendTexCount();

				if (blend_num > 4)
				{
					blend_num = 4;
				}
			}

			m_pRender->GetSceneView()->AddGroundBatch(DrawChunksShaderBatch,chunk,"Terrain_Chunk");

			// 设置基础纹理
			int base_tex_index = chunk->GetBaseTexIndex();

			Assert(base_tex_index != -1);

			CBlendTex* pBaseTex = pBaseTexSet->GetByIndex(base_tex_index);

			ITexture* pNormalTex = NULL;
			if(use_normal)
			{
				pNormalTex = pBaseTex->GetNormalMap();
			}

			/*
			if (pBaseTex != cur_base_tex)
			{
				cur_base_tex = pBaseTex;
			}
			*/

			if (dist_to_camera >= alpha_hide_radius)
			{
				// 超过混合贴图消隐距离
				blend_num = 0;
			}

			chunk->SetBlendNum(blend_num);

			float max_distance = dist_to_camera + chunk->GetRadius();
			float min_distance = dist_to_camera - chunk->GetRadius();
			// 是否消隐混合贴图
			bool gradually = (dist_to_camera >= alpha_fade_radius)
				&& (dist_to_camera < alpha_hide_radius);
			// 是否计算线性雾效
			bool use_fog = fog_linear && (max_distance >= fog_start);
			// 平滑剪裁
			bool smooth_clip = smooth_horizon 
				&& (max_distance >= far_clip_distance);
			bool use_shadow = dynamic_shadow 
				&& (min_distance < shadow_distance);
			bool use_prelight = prelight && (min_distance < light_distance);
			bool has_normal = (pNormalTex != NULL);

			// 设置着色器
			size_t shader_flag = (size_t(light_map) << FLAG_LIGHTMAP)
				+ (size_t(use_shadow) << FLAG_SHADOWEDMAP)
				+ (size_t(gradually) << FLAG_GRADUALLY) 
				+ (size_t(use_fog) << FLAG_FOGLINEAR)
				+ (size_t(has_normal) << FLAG_NORMALMAP) 
				+ (size_t(use_specular) << FLAG_SPECULAR)
				+ (size_t(use_prelight) << FLAG_PRELIGHT) 
				+ (size_t(smooth_clip) << FLAG_SMOOTHCLIP)
				+ (size_t(use_shadow && fix_shadow) << FLAG_FIXSHADOW) 
				+ (size_t(fog_exp) << FLAG_FOGEXP) 
				+ (size_t(sphere_ambient) << FLAG_SPHEREAMBIENT)
				+ (size_t(use_clip_plane) << FLAG_CLIPPLANE)
				+ (size_t(height_fog) << FLAG_HEIGHT_FOG);

			size_t flagindex = shader_flag + (size_t(blend_num) << FLAG_MAX);
           
			CTerrainChunk::chunk_cb_object_shader_handle_t* pShaderHandle = GetShaderHandle(flagindex,shader_flag,blend_num);

			chunk->SetChunkObject();
			chunk->SetChunkNormalSceneCB();
			chunk->SetZoneLightMap(pLightTex);
			chunk->SetChunkLevel(level);
			chunk->SetShader( pShaderHandle->pShader );
			chunk->SetChunkShader(pShaderHandle);
		}
	}
}

// 开始绘制的地表函数
void CTerrainPainter::DrawChunksShaderBatch(void* pdata)
{
	CTerrainChunk* chunk = (CTerrainChunk*)pdata; 
	//开始渲染一个chunk
    //glUseProgram(ShaderID);
    chunk->GetShader()->UsedShader();
    chunk->UseNormalSceneCB(); 
    chunk->UseChunkGroupCB();
	chunk->UseChunkObject();
	//
	chunk->UseTexture();

	chunk->SetBatch(NULL,chunk->GetChunkLevel(),chunk->GetShader());
	//glUseProgram(0);
}
 

void CTerrainPainter::DrawChunksGBuffer(render_zone_t* zones,
	size_t zone_num, bool horizon_cull, int level)
{
	IRenderContext* pContext = m_pRender->GetContext();
	bool smooth_horizon = m_pTerrain->GetSmoothHorizontal();
	bool enable_normalmap = m_pTerrain->GetEnableNormalMap();
	bool enable_tessellation = m_pTerrain->GetEnableTessellation();
	// 相对坐标参考位置
	const FmVec3& refer_pos = pContext->GetReferPosition();
	// 扫描每个zone
	CBlendTexSet* pBaseTexSet = m_pTerrain->GetBaseTexSet();
	CBlendTexSet* pBlendTexSet = m_pTerrain->GetBlendTexSet();
	float alpha_fade_radius = m_pTerrain->GetAlphaFadeRadius();
	float alpha_hide_radius = m_pTerrain->GetAlphaHideRadius();
	float far_clip_distance = pContext->GetFarClipDistance();
	float shadow_distance = pContext->GetShadowDistance();
	float light_distance = 0.0;//pContext->GetMaxLightDistance();
	float zone_width = m_pTerrain->GetZoneWidth();
	float zone_height = m_pTerrain->GetZoneHeight();
	bool occlusion_query = false;//pContext->GetEnableOcclusionQuery();
	bool initialized = false;
	CBlendTex* cur_base_tex = NULL;
	CZoneManager* pZoneManager = m_pTerrain->GetZoneManager();

	for (size_t zone = 0; zone < zone_num; zone++)
	{
		render_zone_t* render_zone = zones + zone;
		CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
			render_zone->nZoneIndex);

		if (NULL == pZone)
		{
			continue;
		}

		// 清除静默时间计数
		pZone->SetSilenceTime(0.0F);

		// 扫描每个chunk并渲染
		size_t render_zone_chunk_num = render_zone->nChunkNum;

		for (size_t k = 0; k < render_zone_chunk_num; ++k)
		{
			CTerrainChunk* chunk = render_zone->pChunks[k];

			if (smooth_horizon)
			{
				if (chunk->GetInFog())
				{
					// 完全在雾中
					continue;
				}
			}

			if (horizon_cull)
			{
				// 地平线剪裁
				if (m_pTerrain->IsHorizonClip(chunk->GetMin(), 
					chunk->GetMax()))
				{
					continue;
				}
			}

			// 计算Chunk到摄影机之间的距离
			float dist_to_camera = chunk->GetCameraDistance();
			// 设置混合纹理
			size_t blend_num = chunk->GetBlendTexCount();

			if (blend_num > 4)
			{
				blend_num = 4;
			}
 
			// 设置基础纹理
			int base_tex_index = chunk->GetBaseTexIndex();

			Assert(base_tex_index != -1);

			CBlendTex* pBaseTex = pBaseTexSet->GetByIndex(base_tex_index);
			ITexture* pNormalTex = pBaseTex->GetNormalMap();
			ITexture* pHeightTex = pBaseTex->GetHeightMap();
			bool use_tessellation = enable_tessellation 
				&& (pHeightTex->GetShaderTex() != NULL);

			if (use_tessellation)
			{
//				pBatch->SetType(DxSceneBatch::TYPE_TESSELLATION);
			}

			if (pBaseTex != cur_base_tex)
			{
				/*
				pBatch->SetDiffuseTexture(0, 
					pBaseTex->GetTex()->GetShaderTex());

				if (enable_normalmap && (pNormalTex != NULL))
				{
					pBatch->SetPSTexture(6, pNormalTex->GetShaderTex());
				}

				if (use_tessellation)
				{
					pBatch->SetDSTexture(0, pHeightTex->GetShaderTex());
				}
				*/

				cur_base_tex = pBaseTex;
			}

			if (dist_to_camera >= alpha_hide_radius)
			{
				// 超过混合贴图消隐距离
				blend_num = 0;
			}

			if (blend_num > 0)
			{
				IStaticTex* pAlphaTex = chunk->GetAlphaTex();

				if (NULL == pAlphaTex)
				{
					pAlphaTex = chunk->CreateAlphaTex(m_pRender);

					if (NULL == pAlphaTex)
					{
						continue;
					}
				}

				// 设置混合权值贴图
			//	pBatch->SetPSTexture(5, pAlphaTex);

				if (use_tessellation)
				{
			//		pBatch->SetDSTexture(5, pAlphaTex);
				}

				const CTerrainChunk::blend_tex_t* blend = chunk->GetBlendTexs();

				for (unsigned int m = 0; m < blend_num; ++m)
				{
					CBlendTex* pBlendTex = pBlendTexSet->GetByIndex(
						blend[m].nTexIndex);

					/*

					if (enable_normalmap)
					{
						// 设置细节纹理的法线贴图
						ITexture* pTex = pBlendTex->GetNormalMap();

						if (pTex)
						{
							pBatch->SetPSTexture(7 + m, pTex->GetShaderTex());
						}
						else
						{
							pBatch->SetPSTexture(7 + m, NULL);
						}
					}

					if (use_tessellation)
					{
						ITexture* pTex = pBlendTex->GetHeightMap();

						if (pTex)
						{
							pBatch->SetDSTexture(1 + m, pTex->GetShaderTex());
						}
						else
						{
							pBatch->SetDSTexture(1 + m, NULL);
						}
					}

					// 设置细节纹理
					pBatch->SetDiffuseTexture(1 + m, 
						pBlendTex->GetTex()->GetShaderTex());
                    */
				}
			}

			float max_distance = dist_to_camera + chunk->GetRadius();
			float min_distance = dist_to_camera - chunk->GetRadius();
			// 是否消隐混合贴图
			bool gradually = (dist_to_camera >= alpha_fade_radius)
				&& (dist_to_camera < alpha_hide_radius);
			// 平滑剪裁
			bool smooth_clip = smooth_horizon 
				&& (max_distance >= far_clip_distance);
			bool has_normal = (pNormalTex != NULL);
			// 设置着色器
			size_t shader_flag = (size_t(has_normal) << GBUFFER_VS_NOMRALMAP) 
				+ (size_t(gradually) << GBUFFER_VS_GRADUALLY) 
				+ (size_t(smooth_clip) << GBUFFER_VS_SMOOTHCLIP);
			IPixelShader* pPS = m_GBufferPSList.GetShader(blend_num, 
				shader_flag);
//			IDxStaticCB* pChunkCB = GetChunkCB(chunk);

			/*
			pBatch->SetVSConstBuffer(0, pChunkCB);
			pBatch->SetPSConstBuffer(0, pChunkCB);
			pBatch->SetPixelShader(pPS);

			if (use_tessellation)
			{
				size_t flag1 = (size_t(gradually) << GBUFFER_DS_GRADUALLY) 
					+ (size_t(smooth_clip) << GBUFFER_DS_SMOOTHCLIP);
				IDxDomainShader* pDS = m_GBufferDSList.GetShader(blend_num,
					flag1);
				
				pBatch->SetHSConstBuffer(0, pChunkCB);
				pBatch->SetDSConstBuffer(0, pChunkCB);
				pBatch->SetVertexShader(m_pGBufferTessVS);
				pBatch->SetHullShader(m_pGBufferHS);
				pBatch->SetDomainShader(pDS);
			}
			else
			{
				IVertexShader* pVS = m_GBufferVSList.GetShader(blend_num, 
					shader_flag);

				pBatch->SetVertexShader(pVS);
			}

			chunk->SetBatch(pBatch, level, true);
			*/

			if (occlusion_query)
			{
				FmVec3 bound_min = chunk->GetMin() - refer_pos;
				FmVec3 bound_max = chunk->GetMax() - refer_pos;

//				pBatch->SetBoundBox(bound_min, bound_max);
//				pBatch->SetUniqueId(chunk->GetUid());
			}
		}
	}
}

void CTerrainPainter::DrawInFog(render_zone_t* zones, size_t zone_num, 
	bool horizon_cull)
{
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	size_t vb_size = m_nInFogVBSize;
	size_t ib_size = m_nInFogIBSize;
	size_t vertex_num = pCulling->GetInFogVertexCount();
	size_t index_num = pCulling->GetInFogIndexCount();
	size_t need_vb_size = vertex_num * sizeof(FmVec4);
	size_t need_ib_size = index_num * sizeof(unsigned short);

	if (need_vb_size > vb_size)
	{
		size_t new_vb_size = vb_size * 2;

		if (new_vb_size < need_vb_size)
		{
			new_vb_size = need_vb_size;
		}
		
		SAFE_RELEASE(m_pInFogVB);
		m_pInFogVB = m_pRender->CreateDynamicVB((unsigned int)new_vb_size,"CTerrainPainter::DrawInFog");
		m_nInFogVBSize = new_vb_size;
	}

	if (need_ib_size > ib_size)
	{
		size_t new_ib_size = ib_size * 2;

		if (new_ib_size < need_ib_size)
		{
			new_ib_size = need_ib_size;
		}
		
		SAFE_RELEASE(m_pInFogIB);
		m_pInFogIB = m_pRender->CreateDynamicIB((unsigned int)new_ib_size);
		m_nInFogIBSize = new_ib_size;
	}

	/*
	void* pVertex = m_pRender->LockDynamicVB(m_pInFogVB);

	memcpy(pVertex, pCulling->GetInFogVertices(), need_vb_size);
	m_pRender->UnlockDynamicVB(m_pInFogVB);

	void* pIndex = m_pRender->LockDynamicIB(m_pInFogIB);

	memcpy(pIndex, pCulling->GetInFogIndices(), need_ib_size);
	m_pRender->UnlockDynamicIB(m_pInFogIB);
	*/
	IRenderContext* pContext = m_pRender->GetContext();
	/*
	DxSceneBatch* pBatch = m_pRender->AddSolidBatch("Terrain_InFog");

	pBatch->SetInputLayout(m_pInFogLayout);
	pBatch->SetVertexBuffer(0, m_pInFogVB, sizeof(FmVec4), 0);
	pBatch->SetIndexBuffer(m_pInFogIB, 0);
	pBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, (unsigned int)vertex_num, 
		(unsigned int)index_num);

	if (pContext->GetEnableDeferredShading())
	{
		pBatch->SetVertexShader(m_pInFogGBufferVS);
		pBatch->SetPixelShader(m_pInFogGBufferPS);
	}
	else
	{
		pBatch->SetVertexShader(m_pInFogDepthVS);
		pBatch->SetPixelShader(m_pInFogDepthPS);

		//if (pContext->GetEnableRealizeDepth())
		//{
		//	pBatch->SetVertexShader(m_pInFogVS);
		//	pBatch->SetPixelShader(m_pInFogPS);
		//}
		//else
		//{
		//	// 需要做剪裁
		//	pBatch->SetVertexShader(m_pInFogDepthVS);
		//	pBatch->SetPixelShader(m_pInFogDepthPS);
		//}
		
		//if (pContext->GetEnableRealizeDepth())
		//{
		//	// 深度批次
		//	DxSceneBatch* pDepthBatch = m_pRender->AddDepthBatch(
		//		"Depth_Terrain_InFog");
		//	pDepthBatch->SetInputLayout(m_pInFogLayout);
		//	pDepthBatch->SetVertexShader(m_pInFogDepthVS);
		//	pDepthBatch->SetPixelShader(m_pInFogDepthPS);
		//	pDepthBatch->SetVertexBuffer(0, m_pInFogVB, sizeof(FmVec4), 0);
		//	pDepthBatch->SetIndexBuffer(m_pInFogIB, 0);
		//	pDepthBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, 
		//		(unsigned int)vertex_num, (unsigned int)index_num);
		//}
	}
	*/
}

CTerrainChunk::chunk_cb_object_shader_handle_t* CTerrainPainter::GetShaderHandle(size_t flagindex,size_t shader_flag,size_t blend_num)
{
	//如果没有找到则添加进去
	if(!m_ShaderMapHandle.Exists(flagindex))
	{
		IVertexShader* pVS = m_VSList.GetShader(blend_num, shader_flag);
		IPixelShader* pPS = m_PSList.GetShader(blend_num, shader_flag);
		IShaderProgram* pShaderPro = m_pRender->CreateShaderProgram(pVS,pPS,terrain_vertex_t_name,2); 
        CTerrainChunk::chunk_cb_object_shader_handle_t* pShaderHandle = (CTerrainChunk::chunk_cb_object_shader_handle_t*)CORE_ALLOC(sizeof(CTerrainChunk::chunk_cb_object_shader_handle_t));
		IShaderParamOp* pShaderOp = pShaderPro->GetParamOp();

		pShaderHandle->pShader = pShaderPro;
		
		//UseNormalSceneCB
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
		pShaderHandle->mat4ShadowViewProjHandle =  pShaderOp->FindParamIdByName("c_mtxShadowViewProj");	
		pShaderHandle->vec4HeightFogColor =  pShaderOp->FindParamIdByName("c_HeightFogColor");	
		pShaderHandle->vec4HeightFogParam =  pShaderOp->FindParamIdByName("c_HeightFogParam");	
		//UseChunkGroupCB
		pShaderHandle->vec4TerrainBlockSizeHandle = pShaderOp->FindParamIdByName("c_TerrainBlockSize");	
		pShaderHandle->vec2GraduallyDisthandle = pShaderOp->FindParamIdByName("c_GraduallyDist");	
		pShaderHandle->vec2Detail0ParamHandle = pShaderOp->FindParamIdByName("c_Detail0Param");	
		pShaderHandle->vec4TessellationFactorHandle = pShaderOp->FindParamIdByName("c_TessellationFactor");	
		pShaderHandle->vec4MaterialAmbientHandle = pShaderOp->FindParamIdByName("c_MaterialAmbient");	
		pShaderHandle->vec4MaterialDiffuseHandle = pShaderOp->FindParamIdByName("c_MaterialDiffuse");	
		pShaderHandle->vec4MaterialSpecularHandle = pShaderOp->FindParamIdByName("c_MaterialSpecular");	
		pShaderHandle->fTerrainBlockSizeHandle = pShaderOp->FindParamIdByName("c_fTerrainBlockSize");	
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

		pShaderHandle->tex_Detailmap0Handle = pShaderOp->FindParamIdByName("tex_Detailmap0");
		pShaderHandle->tex_Detailmap1Handle = pShaderOp->FindParamIdByName("tex_Detailmap1");
		pShaderHandle->tex_Detailmap2Handle = pShaderOp->FindParamIdByName("tex_Detailmap2");
		pShaderHandle->tex_Detailmap3Handle = pShaderOp->FindParamIdByName("tex_Detailmap3");
		pShaderHandle->tex_Detailmap4Handle = pShaderOp->FindParamIdByName("tex_Detailmap4");
		pShaderHandle->tex_BlendmapHandle = pShaderOp->FindParamIdByName("tex_Blendmap");
		pShaderHandle->tex_LightmapHandle = pShaderOp->FindParamIdByName("tex_Lightmap");
		pShaderHandle->tex_ShadowHandle = pShaderOp->FindParamIdByName("tex_Shadow");
		pShaderHandle->tex_PrelightHandle = pShaderOp->FindParamIdByName("tex_Prelight");
		pShaderHandle->tex_DepthmapHandle = pShaderOp->FindParamIdByName("tex_Depthmap");
		pShaderHandle->tex_Bumpmap0Handle = pShaderOp->FindParamIdByName("tex_Bumpmap0");
		pShaderHandle->tex_Bumpmap1Handle = pShaderOp->FindParamIdByName("tex_Bumpmap1");
		pShaderHandle->tex_Bumpmap2Handle = pShaderOp->FindParamIdByName("tex_Bumpmap2");
		pShaderHandle->tex_Bumpmap3Handle = pShaderOp->FindParamIdByName("tex_Bumpmap3");
		pShaderHandle->tex_Bumpmap4Handle = pShaderOp->FindParamIdByName("tex_Bumpmap4");

		m_ShaderMapHandle.Add(flagindex,pShaderHandle);
	}	

	return m_ShaderMapHandle.Find(flagindex).GetData();
}

void CTerrainPainter::DrawShadowMapChunks()
{
	IRenderContext* pContext = m_pRender->GetContext();
	int shadow_map_index = 0;//pContext->GetShadowMapIndex();
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	render_collect_t* pCollect;
	
	if (shadow_map_index >= 0)
	{
		pCollect = pCulling->GetShadowMapCollect(shadow_map_index);
	}
	else
	{
		pCollect = pCulling->GetShadowMapCollect();
	}

	bool enable_tessellation = m_pTerrain->GetEnableTessellation();


	// 太阳高度角的正切
	FmVec3 sun_dir = pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);
	float tan_sun = 0.0F;

	/*
	if (!pContext->GetVarianceShadowMap())
	{
		float dx = FmVec2Length(&FmVec2(sun_dir.x, sun_dir.z));

		if (dx == 0.0F)
		{
			tan_sun = 1e8;
		}
		else
		{
			tan_sun = fabsf(sun_dir.y) / dx;
		}
	}
	*/

	CBlendTexSet* pBaseTexSet = m_pTerrain->GetBaseTexSet();
	CBlendTexSet* pBlendTexSet = m_pTerrain->GetBlendTexSet();
	float alpha_hide_radius = m_pTerrain->GetAlphaHideRadius();
	float half_chunk_size = m_pTerrain->GetChunkScale() 
		* m_pTerrain->GetUnitSize() * 0.5F;
	float inv_half_chunk_size = 1.0F / half_chunk_size;
	size_t chunk_num = pCollect->nChunkNum;
	render_collect_t::chunk_t* chunks = pCollect->pChunks;

 
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i].pChunk;

		if (enable_tessellation)
		{
			UpdateChunkGroupCB(pChunk);
		}
		/*
		// 地块的高度差
		float tan_chunk = (pChunk->GetMax().y - pChunk->GetMin().y) 
			* inv_half_chunk_size;

		if (tan_chunk < tan_sun)
		{
			// 不太可能形成有效的投影
			continue;
		}

		DxSceneBatch* pBatch = m_pRender->AddSolidBatch("SM_Terrain_Chunk");
		IDxStaticCB* pChunkCB = GetChunkCB(pChunk);

		pBatch->SetVSConstBuffer(0, pChunkCB);
		//pBatch->SetVertexShader(m_pShadowMapVS);
		pBatch->SetPixelShader(m_pShadowMapPS);
		pBatch->SetInputLayout(m_pInputLayout);

		int base_tex_index = pChunk->GetBaseTexIndex();

		Assert(base_tex_index != -1);

		CBlendTex* pBaseTex = pBaseTexSet->GetByIndex(base_tex_index);
		ITexture* pHeightTex = pBaseTex->GetHeightMap();
		bool use_tessellation = enable_tessellation 
			&& (pHeightTex->GetShaderTex() != NULL);

		if (use_tessellation)
		{
			size_t blend_num = pChunk->GetBlendTexCount();

			if (blend_num > 4)
			{
				blend_num = 4;
			}

			if (pChunk->GetCameraDistance() >= alpha_hide_radius)
			{
				// 超过混合贴图消隐距离
				blend_num = 0;
			}

			pBatch->SetDSTexture(0, pHeightTex->GetShaderTex());

			if (blend_num > 0)
			{
				IStaticTex* pAlphaTex = pChunk->GetAlphaTex();

				if (NULL == pAlphaTex)
				{
					pAlphaTex = pChunk->CreateAlphaTex(m_pRender);

					if (NULL == pAlphaTex)
					{
						continue;
					}
				}

				// 设置混合权值贴图
				pBatch->SetDSTexture(5, pAlphaTex);

				const CTerrainChunk::blend_tex_t* blend = pChunk->GetBlendTexs();

				for (unsigned int m = 0; m < blend_num; ++m)
				{
					CBlendTex* pBlendTex = pBlendTexSet->GetByIndex(
						blend[m].nTexIndex);
					ITexture* pTex = pBlendTex->GetHeightMap();

					if (pTex)
					{
						pBatch->SetDSTexture(1 + m, pTex->GetShaderTex());
					}
					else
					{
						pBatch->SetDSTexture(1 + m, NULL);
					}
				}
			}

			IDxDomainShader* pDS = m_ShadowMapDSList.GetShader(blend_num, 0);

			pBatch->SetType(DxSceneBatch::TYPE_TESSELLATION);
			pBatch->SetHSConstBuffer(0, pChunkCB);
			pBatch->SetDSConstBuffer(0, pChunkCB);
			pBatch->SetHSConstBuffer(1, m_pChunkGroupCB);
			pBatch->SetDSConstBuffer(1, m_pChunkGroupCB);
			pBatch->SetVertexShader(m_pShadowMapTessVS);
			pBatch->SetHullShader(m_pShadowMapHS);
			pBatch->SetDomainShader(pDS);
		}
		else
		{
			pBatch->SetVertexShader(m_pShadowMapVS);
		}

		pChunk->SetBatch(pBatch, -1, false);
		*/
	}
 
}

void CTerrainPainter::DrawShadowMapVisuals()
{
	IRenderContext* pContext = m_pRender->GetContext();
	int shadow_map_index = 0;// pContext->GetShadowMapIndex();
	CTerrainCulling* pCulling = m_pTerrain->GetCulling();
	render_collect_t* pCollect;

	if (shadow_map_index >= 0)
	{
		pCollect = pCulling->GetShadowMapCollect(shadow_map_index);
	}
	else
	{
		pCollect = pCulling->GetShadowMapCollect();
	}

	size_t visual_num = pCollect->nVisualNum;
	render_collect_t::visual_t* visuals = pCollect->pVisuals;


	for (size_t i = 0; i < visual_num; ++i)
	{
		visuals[i].pVisBase->RealizeShadowMap();
	}
}

// 显示四叉树节点的包围盒
static void draw_quad_node(IRender* pRender, CQuadNode* pNode, int level)
{
	static unsigned int s_nColor[8] = { 0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00,
		0xFF0000FF, 0xFFFFFF00, 0xFF00FFFF, 0xFFFF00FF, 0xFF008080 };
	
	FmMat4 mat;

	FmMat4Identity(&mat);

	FmVec3 center = (pNode->m_vMin + pNode->m_vMax) * 0.5F;
	FmVec3 box_size = pNode->m_vMax - pNode->m_vMin;

	if ((box_size.x > 0.0F) && (box_size.y > 0.0F) && (box_size.z > 0.0F))
	{
		pRender->DrawBoundBox( mat, center, box_size, 
			s_nColor[level % 8]);
	 
	}

	++level;

	if (pNode->m_pChild0)
	{
		draw_quad_node(pRender, pNode->m_pChild0, level);
	}
	
	if (pNode->m_pChild1)
	{
		draw_quad_node(pRender, pNode->m_pChild1, level);
	}
	
	if (pNode->m_pChild2)
	{
		draw_quad_node(pRender, pNode->m_pChild2, level);
	}
	
	if (pNode->m_pChild3)
	{
		draw_quad_node(pRender, pNode->m_pChild3, level);
	}
}

void CTerrainPainter::DrawZoneQuadTree()
{
	IRenderContext* pContext = m_pRender->GetContext();
	const FmVec3& cam_pos = pContext->GetCamera().vPosition;
	CZoneManager* pZoneManager = m_pTerrain->GetZoneManager();
	CTerrainZone* pZone = pZoneManager->GetInZone(cam_pos.x, cam_pos.z);

	if (NULL == pZone)
	{
		return;
	}

	int level = 0;

	draw_quad_node(m_pRender, pZone->GetQuadNode(), level);
}
