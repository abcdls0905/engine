//--------------------------------------------------------------------
// 文件名:		visual_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年9月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "visual_manager.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_visuals.h"
#include "zone_manager.h"
#include "quad_visual.h"
#include "trigger.h"
#include "light_map_config.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"

#include "../visual/i_model.h"

#include "../visual/i_model_player.h"
#include "../visual/vis_utils.h"
#include "../public/var.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../visual/i_effect_model.h"
#include "../visual/i_particle_manager.h"
#include "../visual/i_light_manager.h"
#include "../visual/i_model.h"
#include "../visual/i_actor.h"
// CVisualManager

CVisualManager::CVisualManager()
{
	m_pTerrain = NULL;
	m_pZoneManager = NULL;
	m_bNeedCheck = false;
	m_nCheckIndex = 0;
}

CVisualManager::~CVisualManager()
{
	Shut();
}

void CVisualManager::SetTerrain(Terrain* value)
{ 
	Assert(value != NULL);

	m_pTerrain = value; 
	m_pZoneManager = m_pTerrain->GetZoneManager();
}

bool CVisualManager::Init()
{
	//m_CheckTimer.Initialize();

	return true;
}
 
bool CVisualManager::Shut()
{
	size_t visual_size = m_Visuals.size();

	for (size_t i = 0; i < visual_size; ++i)
	{
		visual_info_t* p = m_Visuals[i];

		if (p)
		{
			CORE_FREE(p, p->nSize);
		}
	}

	m_Visuals.clear();
	m_FreeList.clear();
	m_Indices.Clear();

	return true;
}

bool CVisualManager::AddLoadVisual(int zone_index, visual_info_t* pInfo)
{
	Assert(zone_index >= 0);
	Assert(pInfo != NULL);

	CTerrainZone* pZone = m_pZoneManager->GetZoneByIndex(zone_index);

	if (NULL == pZone)
	{
		CORE_FREE(pInfo, pInfo->nSize);
		// 物体所在区域已经被卸载
		return false;
	}

	const char* name = pInfo->pName;
	size_t old_index;

	if (m_Indices.GetData(name, old_index))
	{
		visual_info_t* pOld = m_Visuals[old_index];

		if (pOld->nZoneIndex == zone_index)
		{
			// 同一区域的同名物体
			IVisBase* pVisBase = (IVisBase*)m_pTerrain->GetCore()->GetEntity(
				pOld->VisualID);

			if (NULL == pVisBase)
			{
				// 删除过时的同一物体信息
				RemoveByIndex(old_index);
			}
			else
			{
				// 同一个区域的同名物体
				CORE_TRACE("(CVisualManager::AddLoadVisual)repeat name");
				CORE_TRACE(name);
				CORE_FREE(pInfo, pInfo->nSize);
				return false;
			}
		}
		else
		{
			CORE_TRACE("(CVisualManager::AddLoadVisual)name exists");
			CORE_TRACE(name);
			CORE_FREE(pInfo, pInfo->nSize);
			return false;
		}
	}

	// 物体位置
	float unit_size = m_pTerrain->GetUnitSize();
	FmVec3 position = pInfo->vPosition;

	position.x *= unit_size;
	position.z *= unit_size;
	position.x += pZone->GetLeft();
	position.z += pZone->GetTop();

	size_t index;

	if (!m_FreeList.empty())
	{
		index = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else
	{
		index = m_Visuals.size();
		m_Visuals.push_back(NULL);
	}

	pInfo->nZoneIndex = zone_index;
	pInfo->vPosition = position;
	pInfo->VisualID = PERSISTID();
	// 添加时假设其在视野内
	pInfo->bInView = true;

	m_Visuals[index] = pInfo;
	m_Indices.Add(name, index);

	// 添加了新的物体，需要检查加载状态
	m_bNeedCheck = true;

	return true;
}

bool CVisualManager::RemoveByIndex(size_t index)
{
	Assert(index < m_Visuals.size());

	visual_info_t* pInfo = m_Visuals[index];

	if (NULL == pInfo)
	{
		return false;
	}

	m_Visuals[index] = NULL;
	m_FreeList.push_back(index);
	m_Indices.RemoveData(pInfo->pName, index);
	CORE_FREE(pInfo, pInfo->nSize);

	return true;
}

size_t CVisualManager::GetVisualCount() const
{
	return m_Visuals.size() - m_FreeList.size();
}

bool CVisualManager::GetLoadFinish()
{
	if (m_bNeedCheck)
	{
		return false;
	}

	size_t visual_size = m_Visuals.size();

	if (0 == visual_size)
	{
		return true;
	}

	visual_info_t** visuals = &m_Visuals[0];

	for (size_t i = 0; i < visual_size; ++i)
	{
		visual_info_t* p = visuals[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->nState == LOADSTATE_LOADING)
		{
			// 还有物体在加载中
			return false;
		}
	}

	return true;
}

float CVisualManager::GetLoadPercent()
{
	size_t visual_size = m_Visuals.size();

	if (0 == visual_size)
	{
		return 1.0F;
		// 没有任何物体时返回0
		//return 0.0F;
	}

	int in_view_count = 0;
	int loaded_count = 0;
	visual_info_t** visuals = &m_Visuals[0];

	for (size_t i = 0; i < visual_size; ++i)
	{
		visual_info_t* p = visuals[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->bInView)
		{
			++in_view_count;
		}

		if (p->nState == LOADSTATE_SUCCEED)
		{
			++loaded_count;
		}
	}

	if (in_view_count == 0)
	{
		return 1.0F;
		// 没有任何物体时返回0
		//return 0.0F;
	}

	return float(loaded_count) / float(in_view_count);
}

void CVisualManager::CheckLoading()
{
	int tick = m_CheckTimer.GetElapseMillisec(50);

	if ((tick >= 50) && (GetVisualCount() > 0))
	{
		CheckVisuals(tick * 0.001F);
	}
}

IVisBase* CVisualManager::LoadModel(visual_info_t* visual, CTerrainZone* pZone, 
									const char* tex_paths)
{
	const char* name = visual->pName;
	const char* angle = "";
	const char* scale = "";
	const char* model = "";
	const char* material = "";
	const char* rigid_body_file = "";
	const char* light_map_size = "";
	const char* only_design = "";
	const char* water_reflect = "";
	const char* no_shadow = "";
	const char* extra_info = "";
	const char* widget = "";
	const char* tag = "";
	const char* receive_shadow = "";
	bool use_vertex_color = true;
	bool use_light_map = false;

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "scale") == 0)
		{
			scale = value;
		}
		else if (strcmp(key, "model") == 0)
		{
			model = value;
		}
		else if (strcmp(key, "material") == 0)
		{
			material = value;
		}
		else if (strcmp(key, "rigid_body_file") == 0)
		{
			rigid_body_file = value;
		}
		else if (strcmp(key, "light_map_size") == 0)
		{
			light_map_size = value;
		}
		else if (strcmp(key, "only_design") == 0)
		{
			only_design = value;
		}
		else if (strcmp(key, "water_reflect") == 0)
		{
			water_reflect = value;
		}
		else if (strcmp(key, "no_shadow") == 0)
		{
			no_shadow = value;
		}
		else if (strcmp(key, "extra_info") == 0)
		{
			extra_info = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		else if (strcmp(key, "vertex_color") == 0)
		{
			use_vertex_color = stricmp(value, "true") == 0;
		}
		else if (strcmp(key, "light_map") == 0)
		{
			use_light_map = stricmp(value, "true") == 0;
		}
		else if (strcmp(key, "receive_shadow") == 0)
		{
			receive_shadow = value;
		}
	}

	if (StringEmpty(model))
	{
		CORE_TRACE("(CVisualManager::LoadModel)no model");
		CORE_TRACE(name);
		return NULL;
	}

	ICore* pCore = m_pTerrain->GetCore();
	IVisBase* pVisBase = (IVisBase*)pCore->CreateEntityArgs("Model", CVarList() );

	if (NULL == pVisBase)
	{
		return NULL;
	}

	pVisBase->SetContext(m_pTerrain->GetContext());

	// 位置
	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 缩放
	if (!StringEmpty(scale))
	{
		FmVec3 scl = VisUtil_StringToVector(scale);

		pVisBase->SetScale(scl.x, scl.y, scl.z);
	}

	// 异步加载
	pCore->SetProperty(pVisBase, "AsyncLoad", CVar(VTYPE_BOOL, true));

	bool design_mode = m_pTerrain->GetDesignMode();

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);

		// 剪裁距离
		float clip_r = visual->fClipRadius;

		VisUtil_SetCustomFloat(pVisBase, "clip_radius", clip_r);
		// 光贴图尺寸
		VisUtil_SetCustomInt(pVisBase, "light_map_size", atoi(light_map_size));
		// 辅助物体
		VisUtil_SetCustomBool(pVisBase, "only_design",
			stricmp(only_design, "true") == 0);

		unsigned int collide_info = visual->nCollideInfo;

		// 可行走
		VisUtil_SetCustomBool(pVisBase, "co_walkable",
			(collide_info & COLLIDE_WALKABLE) != 0);
		// 不生成墙面
		VisUtil_SetCustomBool(pVisBase, "co_gen_wall",
			(collide_info & COLLIDE_GEN_WALL) != 0);
		// 不生成屋顶
		VisUtil_SetCustomBool(pVisBase, "co_gen_roof",
			(collide_info & COLLIDE_GEN_ROOF) != 0);
		// 可穿透
		VisUtil_SetCustomBool(pVisBase, "co_through",
			(collide_info & COLLIDE_THROUGH) != 0);

		if (NULL != strstr(model, "mdl\\tree\\"))
		{
			VisUtil_SetCustomBool(pVisBase, "is_tree", true);
		}
		else
		{
			VisUtil_SetCustomBool(pVisBase, "is_tree", false);
		}
	}

	// 附加信息
	if (!StringEmpty(extra_info))
	{
		int info = atoi(extra_info);

		pCore->SetProperty(pVisBase, "ExtraInfo", CVar(VTYPE_INT, info));
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget", 
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	if (!StringEmpty(visual->pszDesc))
	{
		VisUtil_SetCustomString(pVisBase, "desc", visual->pszDesc);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "desc", "");
	}

	// 水面反射
	pVisBase->SetWaterReflect(stricmp(water_reflect, "true") == 0);

	//if (design_mode)
	//{
	//	// 剪裁距离
	//	float clip_r = visual->fClipRadius;

	//	pVisBase->GetVar()->GetChild("clip_radius").SetFloat(clip_r);

	//	// 光贴图尺寸
	//	pVisBase->GetVar()->GetChild("light_map_size").SetInt(
	//		atoi(light_map_size));
	//	
	//	// 辅助物体
	//	pVisBase->GetVar()->GetChild("only_design").SetBool(
	//		stricmp(only_design, "true") == 0);

	//	unsigned int collide_info = visual->nCollideInfo;

	//	// 可行走
	//	pVisBase->GetVar()->GetChild("co_walkable").SetBool(
	//		(collide_info & COLLIDE_WALKABLE) != 0);
	//	// 不生成墙面
	//	pVisBase->GetVar()->GetChild("co_gen_wall").SetBool(
	//		(collide_info & COLLIDE_GEN_WALL) != 0);
	//	// 不生成屋顶
	//	pVisBase->GetVar()->GetChild("co_gen_roof").SetBool(
	//		(collide_info & COLLIDE_GEN_ROOF) != 0);
	//	// 可穿透
	//	pVisBase->GetVar()->GetChild("co_through").SetBool(
	//		(collide_info & COLLIDE_THROUGH) != 0);
	//}

	pCore->SetProperty(pVisBase, "UseVertexColor", 
		CVar(VTYPE_BOOL, use_vertex_color));
	pCore->SetProperty(pVisBase, "UseLightMap", 
		CVar(VTYPE_BOOL, use_light_map));

	//	if ((use_vertex_color || use_light_map) && (!m_pTerrain->GetNoLight()))
	//	{
	//		// 否则要加载光照文件
	//		char fname[256];
	//
	//		SafeSprintf(fname, sizeof(fname), "%s%s\\%s_%s",
	//			m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
	//			pZone->GetName(), name);
	//		pCore->SetProperty(pVisBase, "LightFile", CVar(VTYPE_STRING, fname));
	//	}

	if (!StringEmpty(model))
	{
		// 添加资源附加路径
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", m_pTerrain->GetAppendPath(), 
			model);
		pCore->SetProperty(pVisBase, "ModelFile", CVar(VTYPE_STRING, fname));
	}

	if (!StringEmpty(material))
	{
		// 添加资源附加路径
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pTerrain->GetAppendPath(), material);
		pCore->SetProperty(pVisBase, "MaterialFile", 
			CVar(VTYPE_STRING, fname));
	}

	if (!StringEmpty(rigid_body_file))
	{
		// 添加资源附加路径
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pTerrain->GetAppendPath(), rigid_body_file);
		pCore->SetProperty(pVisBase, "RigidBodyFile", 
			CVar(VTYPE_STRING, fname));
	}

	// 贴图路径
	if (!StringEmpty(tex_paths))
	{
		pCore->SetProperty(pVisBase, "TexPaths", 
			CVar(VTYPE_STRING, tex_paths));
	}

	//if (design_mode)
	//{
	//	if (NULL != strstr(model, "mdl\\tree\\"))
	//	{
	//		pVisBase->GetVar()->GetChild("is_tree").SetBool(true);
	//	}
	//	else
	//	{
	//		pVisBase->GetVar()->GetChild("is_tree").SetBool(false);
	//	}
	//}

	// 加载
	pVisBase->Load();

	// 无投影
	pVisBase->SetCastShadow(stricmp(no_shadow, "false") == 0);
	// 不接受投影
	pVisBase->SetReceiveShadow(stricmp(receive_shadow, "true") == 0);

	if ((use_vertex_color || use_light_map) && (!m_pTerrain->GetNoLight()))
	{
		// 否则要加载光照文件
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s\\%s_%s",
			m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
			pZone->GetName(), name);
		pCore->SetProperty(pVisBase, "LightFile", CVar(VTYPE_STRING, fname));

		IModel* pModel = (IModel*)pVisBase;
		IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

		if (pModelPlayer)
		{
			// 设置光贴图名字映射
			for (size_t k = 0; k < visual->nAtlasMapNum; ++k)
			{
                atlas_map_t AtlasMap;
                memcpy(&AtlasMap, visual->pAtlasMaps + k, sizeof(atlas_map_t));
				atlas_map_t* pAtlasMap = &AtlasMap;
               
				char fname[256];

				SafeSprintf(fname, sizeof(fname), "%s%s\\atlas_%s_%d.dds",
					m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
					pZone->GetName(), pAtlasMap->nAtlasMapIndex);
				pModelPlayer->SetMaterialAtlasLightMap(pAtlasMap->nMaterialIndex,
					fname, pAtlasMap->fScaleU, pAtlasMap->fScaleV, 
					pAtlasMap->fOffsetU, pAtlasMap->fOffsetV);
			}

			// 此时再加载光照
			pModelPlayer->LoadLight(fname, true);

			CLightMapConfig* pLightMapConfig = m_pTerrain->GetLigthMapConfig();

			size_t mat_count = pLightMapConfig->GetModelScaleCount(name);

			float x;
			float y;
			float z;

			for (size_t k = 0; k < mat_count; ++k)
			{
				if (pLightMapConfig->GetModelScale(name, k, x, y, z))
				{
					pModelPlayer->SetMaterialLigthMapColorRangeScale(k, x, y, z);
				}
			}
		}
	}

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

IVisBase* CVisualManager::LoadParticle(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* angle = "";
	const char* scale = "";
	const char* par_name = "";
	const char* config = "";
	const char* widget = "";
	const char* tag = "";
	//const char* desc = "";

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "scale") == 0)
		{
			scale = value;
		}
		else if (strcmp(key, "par_name") == 0)
		{
			par_name = value;
		}
		else if (strcmp(key, "config") == 0)
		{
			config = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		//else if (strcmp(key, "desc") == 0)
		//{
		//	desc = value;
		//}
	}

	if (StringEmpty(par_name))
	{
		CORE_TRACE("(CVisualManager::LoadParticle)no particle name");
		CORE_TRACE(name);
		return NULL;
	}

	if (StringEmpty(config))
	{
		CORE_TRACE("(CVisualManager::LoadParticle)no config");
		CORE_TRACE(name);
		return NULL;
	}

	ICore* pCore = m_pTerrain->GetCore();
	IRenderContext* pContext = m_pTerrain->GetContext();
	IEntity* particle_man = pContext->GetUnique("ParticleManager");

	if (NULL == particle_man)
	{
		CORE_TRACE("(CVisualManager::LoadParticle)no particle manager");
		CORE_TRACE(name);
		return NULL;
	}

	// 添加资源附加路径
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", m_pTerrain->GetAppendPath(), 
		config);

	//CVarList res;

	PERSISTID res = ((IParticleManager*)particle_man)->CreateFromIni(fname, par_name);
	//	pCore->InvokeMethod(particle_man, "CreateFromIni",
	//		CVarList() << fname << par_name, res);

	IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(res);

	if (NULL == pVisBase)
	{
		CORE_TRACE("(CVisualManager::LoadParticle)create particle failed");
		CORE_TRACE(config);
		CORE_TRACE(par_name);
		return NULL;
	}

	// 位置
	FmVec3 pos = visual->vPosition;

	//	// 将水面波纹粒子放在水面上
	//	CVar prop;
	//
	//	if (pCore->GetProperty(pVisBase, "WaterWave", prop))
	//	{
	//		if (prop.BoolVal())
	//		{
	//			pos.y = m_pTerrain->GetWaterMaxHeightDefault(pos.x, pos.z, pos.y);
	//		}
	//	}

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 缩放
	if (!StringEmpty(scale))
	{
		FmVec3 scl = VisUtil_StringToVector(scale);

		pVisBase->SetScale(scl.x, scl.y, scl.z);
	}

	// 剪裁距离
	float clip_r = visual->fClipRadius;

	if (clip_r > 0.0F)
	{
		// 使用剪裁距离来设置LOD结束距离
		pCore->SetProperty(pVisBase, "LodEnd", CVar(VTYPE_FLOAT, clip_r));
	}

	bool design_mode = m_pTerrain->GetDesignMode();

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
		VisUtil_SetCustomString(pVisBase, "config", config);
		VisUtil_SetCustomString(pVisBase, "par_name", par_name);
		VisUtil_SetCustomFloat(pVisBase, "clip_radius", clip_r);
	}
	else
	{
		// 不参与检测碰撞
		pVisBase->SetTraceEnable(false);
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget", 
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	//if (!StringEmpty(desc))
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
	//}
	//else if (design_mode)
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString("");
	//}

	// 加载
	pVisBase->Load();

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

IVisBase* CVisualManager::LoadLight(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* angle = "";
	const char* type = "";
	const char* size = "";
	const char* color = "";
	const char* intensity = "";
	const char* range = "";
	const char* att0 = "";
	const char* att1 = "";
	const char* att2 = "";
	const char* inner_degree = "";
	const char* outer_degree = "";
	const char* falloff = "";
	const char* shadow_map_size = "";
	const char* blink = "";
	const char* period = "";
	const char* tick = "";
	const char* widget = "";
	const char* tag = "";
	//const char* desc = "";
	bool only_design = false;

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "type") == 0)
		{
			type = value;
		}
		else if (strcmp(key, "size") == 0)
		{
			size = value;
		}
		else if (strcmp(key, "color") == 0)
		{
			color = value;
		}
		else if (strcmp(key, "intensity") == 0)
		{
			intensity = value;
		}
		else if (strcmp(key, "range") == 0)
		{
			range = value;
		}
		else if (strcmp(key, "att0") == 0)
		{
			att0 = value;
		}
		else if (strcmp(key, "att1") == 0)
		{
			att1 = value;
		}
		else if (strcmp(key, "att2") == 0)
		{
			att2 = value;
		}
		else if (strcmp(key, "inner_degree") == 0)
		{
			inner_degree = value;
		}
		else if (strcmp(key, "outer_degree") == 0)
		{
			outer_degree = value;
		}
		else if (strcmp(key, "falloff") == 0)
		{
			falloff = value;
		}
		else if (strcmp(key, "shadow_map_size") == 0)
		{
			shadow_map_size = value;
		}
		else if (strcmp(key, "blink") == 0)
		{
			blink = value;
		}
		else if (strcmp(key, "period") == 0)
		{
			period = value;
		}
		else if (strcmp(key, "tick") == 0)
		{
			tick = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		//else if (strcmp(key, "desc") == 0)
		//{
		//	desc = value;
		//}
		else if (strcmp(key, "only_design") == 0)
		{
			only_design = stricmp(value, "true") == 0;
		}
	}

	bool design_mode = m_pTerrain->GetDesignMode();

	if (only_design && (!design_mode))
	{
		// 不需要加载
		return NULL;
	}

	if (StringEmpty(color))
	{
		CORE_TRACE("(CVisualManager::LoadLight)no color");
		CORE_TRACE(name);
		return 0;
	}

	ICore* pCore = m_pTerrain->GetCore();
	IRenderContext* pContext = m_pTerrain->GetContext();
	IEntity* light_man = pContext->GetUnique("LightManager");

	if (NULL == light_man)
	{
		CORE_TRACE("(CVisualManager::LoadParticle)no light manager");
		CORE_TRACE(name);
		return 0;
	}

	ILightManager* pLightManager = (ILightManager*)light_man;
	PERSISTID res = pLightManager->Create();

	IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(res);

	if (NULL == pVisBase)
	{
		CORE_TRACE("(CVisualManager::LoadLight)create light failed");
		CORE_TRACE(name);
		return 0;
	}

	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	if (!StringEmpty(type))
	{
		pCore->SetProperty(pVisBase, "LightType", CVar(VTYPE_STRING, type));

		if (stricmp(type, "box") == 0)
		{
			if (!StringEmpty(size))
			{
				FmVec3 box_scale = VisUtil_StringToVector(size);

				pCore->SetProperty(pVisBase, "BoxScaleX", 
					CVar(VTYPE_FLOAT, box_scale.x));
				pCore->SetProperty(pVisBase, "BoxScaleY", 
					CVar(VTYPE_FLOAT, box_scale.y));
				pCore->SetProperty(pVisBase, "BoxScaleZ", 
					CVar(VTYPE_FLOAT, box_scale.z));
			}
		}
		else if (stricmp(type, "spot") == 0)
		{
			if (!StringEmpty(inner_degree))
			{
				pCore->SetProperty(pVisBase, "InnerDegree", 
					CVar(VTYPE_FLOAT, float(atof(inner_degree))));
			}

			if (!StringEmpty(outer_degree))
			{
				pCore->SetProperty(pVisBase, "OuterDegree", 
					CVar(VTYPE_FLOAT, float(atof(outer_degree))));
			}

			if (!StringEmpty(falloff))
			{
				pCore->SetProperty(pVisBase, "Falloff", 
					CVar(VTYPE_FLOAT, float(atof(falloff))));
			}
		}
	}

	pCore->SetProperty(pVisBase, "Color", CVar(VTYPE_STRING, color));

	if (!StringEmpty(intensity))
	{
		pCore->SetProperty(pVisBase, "Intensity", 
			CVar(VTYPE_FLOAT, float(atof(intensity))));
	}

	if (!StringEmpty(range))
	{
		pCore->SetProperty(pVisBase, "Range", 
			CVar(VTYPE_FLOAT, float(atof(range))));
	}

	pCore->SetProperty(pVisBase, "Attenu0", 
		CVar(VTYPE_FLOAT, float(atof(att0))));
	pCore->SetProperty(pVisBase, "Attenu1", 
		CVar(VTYPE_FLOAT, float(atof(att1))));
	pCore->SetProperty(pVisBase, "Attenu2", 
		CVar(VTYPE_FLOAT, float(atof(att2))));

	if (!StringEmpty(shadow_map_size))
	{
		pCore->SetProperty(pVisBase, "ShadowMapSize", 
			CVar(VTYPE_INT, atoi(shadow_map_size)));
	}

	if (!StringEmpty(blink))
	{
		pCore->SetProperty(pVisBase, "Blink", 
			CVar(VTYPE_FLOAT, float(atof(blink))));
	}

	if (!StringEmpty(period))
	{
		pCore->SetProperty(pVisBase, "BlinkPeriod", 
			CVar(VTYPE_FLOAT, float(atof(period))));
	}

	if (!StringEmpty(tick))
	{
		pCore->SetProperty(pVisBase, "BlinkTick", 
			CVar(VTYPE_FLOAT, float(atof(tick))));
	}

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
		VisUtil_SetCustomBool(pVisBase, "only_design", only_design);
	}
	else
	{
		// 不参与检测碰撞
		pVisBase->SetTraceEnable(false);
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget", 
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	//if (!StringEmpty(desc))
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
	//}
	//else if (design_mode)
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString("");
	//}

	// 加载
	pVisBase->Load();

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

IVisBase* CVisualManager::LoadSound(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* sound = "";
	const char* angle = "";
	const char* volume = "";
	//const char* loop = "";
	const char* min_interval = "";
	const char* max_interval = "";
	const char* in_degree = "";
	const char* out_degree = "";
	const char* out_volume = "";
	const char* min_dist = "";
	const char* max_dist = "";
	const char* widget = "";
	const char* tag = "";
	//const char* desc = "";

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "sound") == 0)
		{
			sound = value;
		}
		else if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "volume") == 0)
		{
			volume = value;
		}
		//else if (strcmp(key, "loop") == 0)
		//{
		//	loop = value;
		//}
		else if (strcmp(key, "min_interval") == 0)
		{
			min_interval = value;
		}
		else if (strcmp(key, "max_interval") == 0)
		{
			max_interval = value;
		}
		else if (strcmp(key, "in_degree") == 0)
		{
			in_degree = value;
		}
		else if (strcmp(key, "out_degree") == 0)
		{
			out_degree = value;
		}
		else if (strcmp(key, "out_volume") == 0)
		{
			out_volume = value;
		}
		else if (strcmp(key, "min_dist") == 0)
		{
			min_dist = value;
		}
		else if (strcmp(key, "max_dist") == 0)
		{
			max_dist = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		//else if (strcmp(key, "desc") == 0)
		//{
		//	desc = value;
		//}
	}

	if (StringEmpty(sound))
	{
		CORE_TRACE("(CVisualManager::LoadSound)no sound");
		CORE_TRACE(name);
		return NULL;
	}

	ICore* pCore = m_pTerrain->GetCore();

	IVisBase* pVisBase = (IVisBase*)pCore->CreateEntityArgs("Sound", CVarList());

	if (NULL == pVisBase)
	{
		return NULL;
	}

	pVisBase->SetContext(m_pTerrain->GetContext());

	// 添加资源附加路径
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pTerrain->GetAppendPath(), sound);

	pCore->SetProperty(pVisBase, "Name", CVar(VTYPE_STRING, fname));

	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 超出范围自动静音
	pCore->SetProperty(pVisBase, "AutoMute", CVar(VTYPE_BOOL, true));
	// 异步加载
	pCore->SetProperty(pVisBase, "AsyncLoad", CVar(VTYPE_BOOL, true));
	// 循环播放
	pCore->SetProperty(pVisBase, "Loop", CVar(VTYPE_BOOL, true));
	// 场景声音需要淡入淡出效果
	pCore->SetProperty(pVisBase, "NeedFadeInFadeOut", CVar(VTYPE_BOOL, true));

	bool design_mode = m_pTerrain->GetDesignMode();

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
	}
	else
	{
		// 不参与检测碰撞
		pVisBase->SetTraceEnable(false);
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget",
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	//if (!StringEmpty(desc))
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
	//}
	//else if (design_mode)
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString("");
	//}

	if (!StringEmpty(volume))
	{
		pCore->SetProperty(pVisBase, "Volume", 
			CVar(VTYPE_FLOAT, float(atof(volume))));
	}

	//	if (!StringEmpty(loop))
	//	{
	//		pCore->SetProperty(pVisBase, "Loop", 
	//			CVar(VTYPE_INT, atoi(loop)));
	//	}
	//
	//	if (!StringEmpty(interval))
	//	{
	//		pCore->SetProperty(pVisBase, "Interval", 
	//			CVar(VTYPE_FLOAT, float(atof(interval))));
	//
	//		const char* max_interval = strchr(interval, ',');
	//
	//		if (NULL != max_interval)
	//		{
	//			max_interval++;
	//
	//			pCore->SetProperty(pVisBase, "MaxInterval", 
	//				CVar(VTYPE_FLOAT, float(atof(max_interval))));
	//		}
	//	}

	if (!StringEmpty(min_interval))
	{
		pCore->SetProperty(pVisBase, "MinInterval",
			CVar(VTYPE_FLOAT, (float)atof(min_interval)));
	}

	if (!StringEmpty(max_interval))
	{
		pCore->SetProperty(pVisBase, "MaxInterval",
			CVar(VTYPE_FLOAT, (float)atof(max_interval)));
	}

	if (!StringEmpty(in_degree))
	{
		pCore->SetProperty(pVisBase, "InDegree", 
			CVar(VTYPE_INT, atoi(in_degree)));
	}

	if (!StringEmpty(out_degree))
	{
		pCore->SetProperty(pVisBase, "OutDegree", 
			CVar(VTYPE_INT, atoi(out_degree)));
	}

	if (!StringEmpty(out_volume))
	{
		pCore->SetProperty(pVisBase, "OutVolume", 
			CVar(VTYPE_FLOAT, float(atof(out_volume))));
	}

	if (!StringEmpty(min_dist))
	{
		pCore->SetProperty(pVisBase, "MinDistance", 
			CVar(VTYPE_FLOAT, float(atof(min_dist))));
	}

	if (!StringEmpty(max_dist))
	{
		pCore->SetProperty(pVisBase, "MaxDistance", 
			CVar(VTYPE_FLOAT, float(atof(max_dist))));
	}

	// 加载
	if (!pVisBase->Load())
	{
		CORE_TRACE("(CVisualManager::LoadSound)load sound failed");
		CORE_TRACE(name);
		CORE_TRACE(sound);
		pVisBase->Release();
		return NULL;
	}

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

IVisBase* CVisualManager::LoadActor(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* config = "";
	const char* angle = "";
	const char* scale = "";
	const char* water_reflect = "";
	const char* widget = "";
	const char* tag = "";
	//const char* desc = "";
	const char* extra_info = "";

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "scale") == 0)
		{
			scale = value;
		}
		else if (strcmp(key, "config") == 0)
		{
			config = value;
		}
		else if (strcmp(key, "water_reflect") == 0)
		{
			water_reflect = value;
		}
		else if (strcmp(key, "extra_info") == 0)
		{
			extra_info = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		//else if (strcmp(key, "desc") == 0)
		//{
		//	desc = value;
		//}
	}

	if (StringEmpty(config))
	{
		CORE_TRACE("(CVisualManager::LoadActor)no config");
		CORE_TRACE(name);
		return NULL;
	}

	ICore* pCore = m_pTerrain->GetCore();
	IVisBase* pVisBase = (IVisBase*)pCore->CreateEntityArgs("Actor", CVarList());

	if (NULL == pVisBase)
	{
		return NULL;
	}

	pVisBase->SetContext(m_pTerrain->GetContext());

	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 缩放
	if (!StringEmpty(scale))
	{
		FmVec3 scl = VisUtil_StringToVector(scale);

		pVisBase->SetScale(scl.x, scl.y, scl.z);
	}

	// 异步加载
	pCore->SetProperty(pVisBase, "AsyncLoad", CVar(VTYPE_BOOL, true));

	bool design_mode = m_pTerrain->GetDesignMode();

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
		VisUtil_SetCustomString(pVisBase, "config", config);

		// 剪裁距离
		float clip_r = visual->fClipRadius;

		VisUtil_SetCustomFloat(pVisBase, "clip_radius", clip_r);
	}

	// 附加信息
	if (!StringEmpty(extra_info))
	{
		int info = atoi(extra_info);

		pCore->SetProperty(pVisBase, "ExtraInfo", CVar(VTYPE_INT, info));
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget", 
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	//if (!StringEmpty(desc))
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
	//}
	//else if (design_mode)
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString("");
	//}

	// 水面反射
	pVisBase->SetWaterReflect(stricmp(water_reflect, "true") == 0);

	//if (design_mode)
	//{
	//	// 剪裁距离
	//	float clip_r = visual->fClipRadius;

	//	pVisBase->GetVar()->GetChild("clip_radius").SetFloat(clip_r);
	//}

	// 添加资源附加路径
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", m_pTerrain->GetAppendPath(), 
		config);

	bool res = false;


	//	pCore->InvokeMethod(pVisBase, "CreateFromIniEx", 
	//		CVarList() << fname << m_pTerrain->GetAppendPath(), res);

	if(pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		res = ((IActor*)pVisBase)->CreateFromIniEx(fname, m_pTerrain->GetAppendPath());
	}

	if (!res)
	{
		CORE_TRACE("(CVisualManager::LoadActor)create failed");
		CORE_TRACE(name);
		CORE_TRACE(config);
		pVisBase->Release();
		return NULL;
	}

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

IVisBase* CVisualManager::LoadDecal(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* angle = "";
	const char* scale = "";
	const char* diffuse = "";
	const char* center_to_pos = "";
	const char* alpha_test = "";
	const char* proj_mode = "";
	const char* proj_angle = "";
	const char* widget = "";
	const char* tag = "";

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "scale") == 0)
		{
			scale = value;
		}
		else if (strcmp(key, "diffuse") == 0)
		{
			diffuse = value;
		}
		else if (strcmp(key, "center_to_pos") == 0)
		{
			center_to_pos = value;
		}
		else if (strcmp(key, "alpha_test") == 0)
		{
			alpha_test = value;
		}
		else if (strcmp(key, "proj_mode") == 0)
		{
			proj_mode = value;
		}
		else if (strcmp(key, "proj_angle") == 0)
		{
			proj_angle = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
	}

	ICore* pCore = m_pTerrain->GetCore();
	IVisBase* pVisBase = (IVisBase*)pCore->CreateEntityArgs("Decal", CVarList());

	if (NULL == pVisBase)
	{
		return NULL;
	}

	pVisBase->SetContext(m_pTerrain->GetContext());

	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 缩放
	if (!StringEmpty(scale))
	{
		FmVec3 scl = VisUtil_StringToVector(scale);

		pVisBase->SetScale(scl.x, scl.y, scl.z);
	}

	pCore->SetProperty(pVisBase, "CullObject", CVar(VTYPE_OBJECT, 
		m_pTerrain->GetID()));
	pCore->SetProperty(pVisBase, "DiffuseMap", CVar(VTYPE_STRING, diffuse));
	pCore->SetProperty(pVisBase, "CenterToPosition", 
		CVar(VTYPE_STRING, center_to_pos));
	pCore->SetProperty(pVisBase, "EnableAlphaTest", 
		CVar(VTYPE_BOOL, stricmp(alpha_test, "true") == 0));

	if (proj_mode[0] != 0)
	{
		pCore->SetProperty(pVisBase, "ProjectMode", 
			CVar(VTYPE_STRING, proj_mode));
	}

	if (proj_angle[0] != 0)
	{
		pCore->SetProperty(pVisBase, "ProjectAngle", 
			CVar(VTYPE_FLOAT, (float)atof(proj_angle)));
	}

	// 模型文件
	char model_file[256];

	SafeSprintf(model_file, sizeof(model_file), "%sdecal\\%s.xmod",
		m_pTerrain->GetFilePath(), name);

	pCore->SetProperty(pVisBase, "ModelFile", CVar(VTYPE_STRING, model_file));

	// 设置贴图路径
	char tex_paths[256];

	SafeSprintf(tex_paths, sizeof(tex_paths), "%s%stex\\decal\\",
		pCore->GetResourcePath(), m_pTerrain->GetAppendPath());

	pCore->SetProperty(pVisBase, "TexPaths", CVar(VTYPE_STRING, tex_paths));

	// 异步加载
	pCore->SetProperty(pVisBase, "AsyncLoad", CVar(VTYPE_BOOL, true));

	bool design_mode = m_pTerrain->GetDesignMode();

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
	}
	else
	{
		// 不参与碰撞
		pVisBase->SetTraceEnable(false);
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget",
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 加载
	CVarList res;

	//	pCore->InvokeMethod(pVisBase, "Load", CVarList(), res);

	if (!res.BoolVal(0))
	{
		CORE_TRACE("(CVisualManager::LoadDecal)create failed");
		CORE_TRACE(name);
		pVisBase->Release();
		return NULL;
	}

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

/*
IVisBase* CVisualManager::LoadDecal(visual_info_t* visual)
{
const char* name = visual->pName;
const char* config = "";
const char* widget = "";
const char* tag = "";
//const char* desc = "";

size_t attr_num = visual->nAttrNum;

for (size_t i = 0; i < attr_num; ++i)
{
const char* key = visual->pAttrKeys[i];
const char* value = visual->pAttrValues[i];

if (strcmp(key, "config") == 0)
{
config = value;
}
else if (strcmp(key, "widget") == 0)
{
widget = value;
}
else if (strcmp(key, "tag") == 0)
{
tag = value;
}
//else if (strcmp(key, "desc") == 0)
//{
//	desc = value;
//}
}

if (StringEmpty(config))
{
CORE_TRACE("(CVisualManager::LoadDecal)no config");
CORE_TRACE(name);
return NULL;
}

ICore* pCore = m_pTerrain->GetCore();

if (NULL == pCore)
{
CORE_TRACE("(CVisualManager::LoadDecal)Get Core Failed");
return NULL;
}

IVisBase* pVisBase = (IVisBase*)pCore->CreateEntityArgs(
"Decal", CVarList() << m_pTerrain->GetRender()->GetRenderName());

if (NULL == pVisBase)
{
CORE_TRACE("(CVisualManager::LoadDecal)Create Decal Failed");
return NULL;
}

pVisBase->SetContext(m_pTerrain->GetContext());

// 异步加载
pCore->SetProperty(pVisBase, "AsyncLoad", CVar(VTYPE_BOOL, true));

bool design_mode = m_pTerrain->GetDesignMode();

// 设计信息
if (design_mode)
{
pVisBase->GetVar()->GetChild("name").SetString(name);
pVisBase->GetVar()->GetChild("config").SetString(config);
}

// 装饰物
if (!StringEmpty(widget))
{
pVisBase->GetVar()->GetChild("widget").SetBool(
stricmp(widget, "true") == 0);
}
else if (design_mode)
{
pVisBase->GetVar()->GetChild("widget").SetBool(false);
}

// 标记信息
if (!StringEmpty(tag))
{
pVisBase->GetVar()->GetChild("tag").SetString(tag);
}
else if (design_mode)
{
pVisBase->GetVar()->GetChild("tag").SetString("");
}

// 描述
//if (!StringEmpty(desc))
//{
//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
//}
//else if (design_mode)
//{
//	pVisBase->GetVar()->GetChild("desc").SetString("");
//}

pVisBase->Load();

// 可以被脚本删除
pCore->SetCanDelByScript(pVisBase, true);

return pVisBase;
}
*/

IVisBase* CVisualManager::LoadTrigger(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* angle = "";
	const char* scale = "";
	const char* size = "";
	const char* script_name = "";
	const char* parameter = "";
	const char* period = "";
	const char* widget = "";
	const char* tag = "";
	//const char* desc = "";

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "scale") == 0)
		{
			scale = value;
		}
		else if (strcmp(key, "size") == 0)
		{
			size = value;
		}
		else if (strcmp(key, "script_name") == 0)
		{
			script_name = value;
		}
		else if (strcmp(key, "parameter") == 0)
		{
			parameter = value;
		}
		else if (strcmp(key, "period") == 0)
		{
			period = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		//else if (strcmp(key, "desc") == 0)
		//{
		//	desc = value;
		//}
	}

	ICore* pCore = m_pTerrain->GetCore();
	Trigger* pVisBase = (Trigger*)pCore->CreateEntityArgs("Trigger", CVarList() );

	if (NULL == pVisBase)
	{
		return NULL;
	}

	pVisBase->SetContext(m_pTerrain->GetContext());
	pVisBase->SetTerrain(m_pTerrain);

	// 位置
	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 缩放
	if (!StringEmpty(scale))
	{
		FmVec3 scl = VisUtil_StringToVector(scale);

		pVisBase->SetScale(scl.x, scl.y, scl.z);
	}

	// 边框尺寸
	if (!StringEmpty(size))
	{
		FmVec3 size1 = VisUtil_StringToVector(size);

		pCore->SetProperty(pVisBase, "SizeX", CVar(VTYPE_FLOAT, size1.x));
		pCore->SetProperty(pVisBase, "SizeY", CVar(VTYPE_FLOAT, size1.y));
		pCore->SetProperty(pVisBase, "SizeZ", CVar(VTYPE_FLOAT, size1.z));
	}

	// 脚本名
	if (!StringEmpty(script_name))
	{
		pCore->SetProperty(pVisBase, "ScriptName", 
			CVar(VTYPE_STRING, script_name));
	}

	// 事件参数
	if (!StringEmpty(parameter))
	{
		pCore->SetProperty(pVisBase, "Parameter", 
			CVar(VTYPE_STRING, parameter));
	}

	// 定时时长
	if (!StringEmpty(period))
	{
		pCore->SetProperty(pVisBase, "Period", 
			CVar(VTYPE_FLOAT, float(atof(period))));
	}

	bool design_mode = m_pTerrain->GetDesignMode();

	//if (!design_mode)
	//{
	//	// 非设计模式不显示边框
	//	pCore->SetProperty(pVisBase, "ShowBound", CVar(VTYPE_BOOL, false));
	//}

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
	}
	else
	{
		// 不参与检测碰撞
		pVisBase->SetTraceEnable(false);

		// 非设计模式不显示边框
		pCore->SetProperty(pVisBase, "ShowBound", CVar(VTYPE_BOOL, false));
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget",
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	//if (!StringEmpty(desc))
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
	//}
	//else if (design_mode)
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString("");
	//}

	// 加载
	pVisBase->Load();

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

IVisBase* CVisualManager::LoadEffectModel(visual_info_t* visual)
{
	const char* name = visual->pName;
	const char* effectmodel_config = "";
	const char* effectmodel_name = "";
	const char* angle = "";
	const char* scale = "";
	const char* water_reflect = "";
	const char* widget = "";
	const char* tag = "";
	//const char* desc = "";

	size_t attr_num = visual->nAttrNum;

	for (size_t i = 0; i < attr_num; ++i)
	{
		const char* key = visual->pAttrKeys[i];
		const char* value = visual->pAttrValues[i];

		if (strcmp(key, "angle") == 0)
		{
			angle = value;
		}
		else if (strcmp(key, "scale") == 0)
		{
			scale = value;
		}
		else if (strcmp(key, "effectmodel_config") == 0)
		{
			effectmodel_config = value;
		}
		else if (strcmp(key, "effectmodel_name") == 0)
		{
			effectmodel_name = value;
		}
		else if (strcmp(key, "water_reflect") == 0)
		{
			water_reflect = value;
		}
		else if (strcmp(key, "widget") == 0)
		{
			widget = value;
		}
		else if (strcmp(key, "tag") == 0)
		{
			tag = value;
		}
		//else if (strcmp(key, "desc") == 0)
		//{
		//	desc = value;
		//}
	}

	if (StringEmpty(effectmodel_config))
	{
		CORE_TRACE("(CVisualManager::LoadEffectModel)no effectmodel_config");
		CORE_TRACE(name);
		return NULL;
	}

	ICore* pCore = m_pTerrain->GetCore();
	IVisBase* pVisBase = (IVisBase*)pCore->CreateEntityArgs("EffectModel", CVarList());

	if (NULL == pVisBase)
	{
		CORE_TRACE("(CVisualManager::LoadEffectModel)Create EffectModel Failed");
		return NULL;
	}

	pVisBase->SetContext(m_pTerrain->GetContext());

	// 异步加载
	pCore->SetProperty(pVisBase, "AsyncLoad", CVar(VTYPE_BOOL, true));

	bool design_mode = m_pTerrain->GetDesignMode();

	// 设计信息
	if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "name", name);
		VisUtil_SetCustomString(pVisBase, "effectmodel_config",
			effectmodel_config);
		VisUtil_SetCustomString(pVisBase, "effectmodel_name",
			effectmodel_name);

		// 剪裁距离
		float clip_r = visual->fClipRadius;

		VisUtil_SetCustomFloat(pVisBase, "clip_radius", clip_r);

		// 进行碰撞检测,可以剪裁
		pVisBase->SetTraceEnable(true);
		pVisBase->SetCullEnable(true);
	}
	else
	{
		// 不进行碰撞检测,不可以剪裁
		pVisBase->SetTraceEnable(false);
		pVisBase->SetCullEnable(false);
	}

	// 装饰物
	if (!StringEmpty(widget))
	{
		VisUtil_SetCustomBool(pVisBase, "widget",
			stricmp(widget, "true") == 0);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomBool(pVisBase, "widget", false);
	}

	// 标记信息
	if (!StringEmpty(tag))
	{
		VisUtil_SetCustomString(pVisBase, "tag", tag);
	}
	else if (design_mode)
	{
		VisUtil_SetCustomString(pVisBase, "tag", "");
	}

	// 描述
	//if (!StringEmpty(desc))
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString(desc);
	//}
	//else if (design_mode)
	//{
	//	pVisBase->GetVar()->GetChild("desc").SetString("");
	//}

	// 水面反射
	pVisBase->SetWaterReflect(stricmp(water_reflect, "true") == 0);

	//if (design_mode)
	//{
	//	// 剪裁距离
	//	float clip_r = visual->fClipRadius;

	//	pVisBase->GetVar()->GetChild("clip_radius").SetFloat(clip_r);
	//}

	// 添加资源附加路径
	char fname[256];
	const char* strAppendPath = m_pTerrain->GetAppendPath();

	SafeSprintf(fname, sizeof(fname), "%s%s", strAppendPath, effectmodel_config);

	CVarList res;

	//	pCore->InvokeMethod(pVisBase, "CreateFromIniEx", 
	//		CVarList() << fname << effectmodel_name << true << strAppendPath, res);
	bool result = false;
	if(pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
	{
		result = ((IEffectModel*)pVisBase)->CreateFromIniEx(fname, effectmodel_name, true, strAppendPath);
	}

	if (!result)
	{
		CORE_TRACE("(CVisualManager::LoadEffectModel)create failed");
		CORE_TRACE(name);
		CORE_TRACE(effectmodel_config);
		CORE_TRACE(effectmodel_name);
		pVisBase->Release();
		return NULL;
	}

	FmVec3 pos = visual->vPosition;

	pVisBase->SetPosition(pos.x, pos.y, pos.z);

	// 角度
	if (!StringEmpty(angle))
	{
		FmVec3 ang = VisUtil_StringToVector(angle);

		pVisBase->SetAngle(ang.x, ang.y, ang.z);
	}

	// 缩放
	if (!StringEmpty(scale))
	{
		FmVec3 scl = VisUtil_StringToVector(scale);

		pVisBase->SetScale(scl.x, scl.y, scl.z);
	}

	//if (design_mode)
	//{
	//	// 进行碰撞检测,可以剪裁
	//	pVisBase->SetTraceEnable(true);
	//	pVisBase->SetCullEnable(true);
	//}
	//else
	//{
	//	// 进行碰撞检测,可以剪裁
	//	pVisBase->SetTraceEnable(false);
	//	pVisBase->SetCullEnable(false);
	//}

	pVisBase->Load();

	// 可以被脚本删除
	//	pCore->SetCanDelByScript(pVisBase, true);

	return pVisBase;
}

bool CVisualManager::CheckVisuals(float seconds)
{
	const size_t visual_num = m_Visuals.size();

	if (0 == visual_num)
	{
		m_bNeedCheck = false;
		return true;
	}

	Terrain* pTerrain = m_pTerrain;
	IRender* pRender = pTerrain->GetRender();
	CZoneManager* pZoneManager = m_pZoneManager;
	ICore* pCore = pTerrain->GetCore();
	// 模型贴图路径
	const char* tex_paths = pTerrain->GetTexturePathString();
	// 摄像机位置
	FmVec3 camera_pos = pTerrain->GetContext()->GetCamera().vPosition;
	// 加载半径
	float load_radius = pTerrain->GetLoadRadius();
	float load_square = load_radius * load_radius;
	// 卸载半径
	float unload_radius = pTerrain->GetUnloadRadius();
	float unload_square = unload_radius * unload_radius;
	// 是否设计状态
	bool design_mode = pTerrain->GetDesignMode();

	// 为优化性能，控制每次检查的物体数量
	if (m_nCheckIndex >= visual_num)
	{
		m_nCheckIndex = 0;
	}

	size_t check_num = (visual_num >> 3) + 7;

	if ((m_nCheckIndex + check_num) > visual_num)
	{
		check_num = visual_num - m_nCheckIndex;
	}

	visual_info_t** pVisualBeg = m_Visuals.data();
	visual_info_t** pVisual = pVisualBeg + m_nCheckIndex;
	visual_info_t** pVisualEnd = pVisual + check_num;

	for (; pVisual < pVisualEnd; ++pVisual)
	{
		// 控制每帧运行的时间
		//if (m_LoadCounter.GetElapseMillisec() > 30)
		//{
		//	break;
		//}

		visual_info_t* visual = *pVisual;

		if (NULL == visual)
		{
			continue;
		}

		switch (visual->nState)
		{
		case LOADSTATE_SUCCEED:
			{
				CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
					visual->nZoneIndex);

				if (NULL == pZone)
				{
					// 区域已不存在则删除之
					IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
						visual->VisualID);

					if (pVisBase)
					{
						// 卸载物体
						pVisBase->Release();
					}

					RemoveByIndex(pVisual - pVisualBeg);
					continue;
				}

				if (design_mode)
				{
					// 设计状态下不可根据范围卸载
					continue;
				}

				// 与摄像机的距离
				float sx = visual->vPosition.x - camera_pos.x;
				float sz = visual->vPosition.z - camera_pos.z;
				float dist_square = sx * sx + sz * sz;

				if (dist_square < unload_square)
				{
					// 还未进入卸载半径
					continue;
				}

				// 进入卸载半径，需要继续检查物体是否设置了剪裁距离
				float clip_radius = visual->fClipRadius;

				if (clip_radius > 0.0F)
				{
					//if (dist_square < (unload_radius + clip_radius) 
					//	* (unload_radius + clip_radius))
					//{
					//	// 还未进入卸载半径加上物体自身附加的剪裁距离
					//	continue;
					//}
					if (dist_square < (clip_radius * clip_radius))
					{
						// 还未进入物体自身的剪裁距离
						continue;
					}
				}

				IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
					visual->VisualID);

				if (pVisBase)
				{
					// 卸载物体
					pVisBase->Release();
				}

				visual->VisualID = PERSISTID();
				visual->nState = LOADSTATE_BEGIN;
			}
			break;
		case LOADSTATE_FAILED:
			{
				CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
					visual->nZoneIndex);

				if (NULL == pZone)
				{
					// 区域已不存在则删除之
					//RemoveByIndex(i);
					RemoveByIndex(pVisual - pVisualBeg);
					continue;
				}
			}
			break;
		case LOADSTATE_BEGIN:
			{
				CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
					visual->nZoneIndex);

				if (NULL == pZone)
				{
					// 区域已不存在则删除之
					//RemoveByIndex(i);
					RemoveByIndex(pVisual - pVisualBeg);
					continue;
				}

				if (!design_mode)
				{
					float sx = visual->vPosition.x - camera_pos.x;
					float sz = visual->vPosition.z - camera_pos.z;
					float dist_square = sx * sx + sz * sz;

					if (dist_square > load_square)
					{
						// 确定此物体的剪裁距离
						float clip_radius = visual->fClipRadius;

						if (clip_radius > 0.0F)
						{
							//if (dist_square < (load_radius + clip_radius) 
							//	* (load_radius + clip_radius))
							if (dist_square < (clip_radius * clip_radius))
							{
								visual->bInView = true;
							}
							else
							{
								// 还未进入加载半径
								visual->bInView = false;
								continue;
							}
						}
						else
						{
							// 还未进入加载半径
							visual->bInView = false;
							continue;
						}
					}
					else
					{
						visual->bInView = true;
					}
				}

				int visual_type = visual->nType;

				 
				if ((visual_type == VISUALTYPE_PARTICLE)
					|| (visual_type == VISUALTYPE_EFFECTMODEL)
					|| (visual_type == VISUALTYPE_ACTOR))
				{
					const char* config = visual->pConfig;

					if (config[0] != 0)
					{
						// 添加资源附加路径
						char fname[256];

						SafeSprintf(fname, sizeof(fname), "%s%s%s", 
							pRender->GetPathPrefix(config),
							pTerrain->GetAppendPath(), config);

						if (pRender->FindCacheIni(fname))
						{
							if (!pRender->GetCacheIniLoadFinish(fname))
							{
								// 等待引导配置文件加载完成
								visual->nState = LOADSTATE_LOADCACHE;
								continue;
							}
						}
						else
						{
							// 异步加载引导配置文件
							pRender->LoadCacheIni(fname, false, true);
							visual->nState = LOADSTATE_LOADCACHE;
							continue;
						}
					}
				}


				IVisBase* pVisBase = NULL;

				switch (visual_type)
				{
				case VISUALTYPE_MODEL:
					pVisBase = LoadModel(visual, pZone, tex_paths);
					break;
				case VISUALTYPE_PARTICLE:
					pVisBase = LoadParticle(visual);
					break;
				case VISUALTYPE_LIGHT:
					pVisBase = LoadLight(visual);
					break;
				case VISUALTYPE_SOUND:
					pVisBase = LoadSound(visual);
					break;
				case VISUALTYPE_ACTOR:
					pVisBase = LoadActor(visual);
					break;
				case VISUALTYPE_DECAL:
					pVisBase = LoadDecal(visual);
					break;
				case VISUALTYPE_TRIGGER:
					pVisBase = LoadTrigger(visual);
					break;
				case VISUALTYPE_EFFECTMODEL:
					pVisBase = LoadEffectModel(visual);
					break;
				default:
					Assert(0);
					break;
				}

				if (NULL == pVisBase)
				{
					visual->nState = LOADSTATE_FAILED;
					visual->bInView = false;
					continue;
				}

				visual->VisualID = pVisBase->GetID();
				visual->nState = LOADSTATE_LOADING;
			}
			break;
		case LOADSTATE_LOADCACHE:
			{
				CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
					visual->nZoneIndex);

				if (NULL == pZone)
				{
					// 区域已不存在则删除之
					//RemoveByIndex(i);
					RemoveByIndex(pVisual - pVisualBeg);
					continue;
				}

				//				const char* config = visual->pConfig;
				//
				//				// 添加资源附加路径
				//				char fname[256];
				//
				//				SafeSprintf(fname, sizeof(fname), "%s%s%s", 
				//					pRender->GetPathPrefix(config), 
				//					pTerrain->GetAppendPath(), config);
				//
				//				if (pRender->FindCacheIni(fname))
				//				{
				//					if (!pRender->GetCacheIniLoadFinish(fname))
				//					{
				//						// 等待引导配置文件加载完成
				//						continue;
				//					}
				//				}

				IVisBase* pVisBase = NULL;

				switch (visual->nType)
				{
				case VISUALTYPE_PARTICLE:
					pVisBase = LoadParticle(visual);
					break;
				case VISUALTYPE_ACTOR:
					pVisBase = LoadActor(visual);
					break;
				case VISUALTYPE_EFFECTMODEL:
					pVisBase = LoadEffectModel(visual);
					break;
				default:
					Assert(0);
					break;
				}

				if (NULL == pVisBase)
				{
					visual->nState = LOADSTATE_FAILED;
					visual->bInView = false;
					continue;
				}

				visual->VisualID = pVisBase->GetID();
				visual->nState = LOADSTATE_LOADING;
			}
			break;
		case LOADSTATE_LOADING:
			{
				IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
					visual->VisualID);

				if (NULL == pVisBase)
				{
					// 删除无效对象
					//RemoveByIndex(i);
					RemoveByIndex(pVisual - pVisualBeg);
					continue;
				}

				CTerrainZone* pZone = pZoneManager->GetZoneByIndex(
					visual->nZoneIndex);

				if (NULL == pZone)
				{
					// 区域已不存在则删除之
					pVisBase->Release();
					//RemoveByIndex(i);
					RemoveByIndex(pVisual - pVisualBeg);
					continue;
				}

				if (!pVisBase->GetLoadFinish())
				{
					// 还未加载完成
					continue;
				}

				if (!pVisBase->GetLoaded())
				{
					// 物体加载失败
					//					VisUtil_RunCallback(m_pTerrain, "on_visual_unload", 
					//						CVarList() << pVisBase->GetID() << visual->vPosition.x
					//						<< visual->vPosition.y << visual->vPosition.z);

					// 删除此物体
					pVisBase->Release();

					visual->VisualID = PERSISTID();
					visual->nState = LOADSTATE_FAILED;
					visual->bInView = false;
					continue;
				}

				if (visual->nType == VISUALTYPE_MODEL 
					|| visual->nType == VISUALTYPE_ACTOR)
				{
					// 加载完成后
					CVarList res;

					//					pCore->InvokeMethod(pVisBase, "AfterLoaded", CVarList(), 
					//						res);
				}

				pTerrain->AddVisBase(visual->pName, pVisBase, false, 
					visual->fClipRadius);

				if (visual->nType == VISUALTYPE_TRIGGER)
				{
					// 初始化触发器
					Trigger* pTrigger = (Trigger*)pVisBase;

					pTrigger->CheckInit();
				}

				// 取物体的标记信息
				const char* tag = VisUtil_GetCustomString(pVisBase, "tag");

				if (!StringEmpty(tag))
				{
					// 标记信息不为空才调用
					//					VisUtil_RunCallback(m_pTerrain, "on_visual_load", 
					//						CVarList() << pVisBase->GetID());
				}

				visual->nState = LOADSTATE_SUCCEED;
			}
			break;
		default:
			Assert(0);
			break;
		}
	}

	m_nCheckIndex += check_num;

	if (m_nCheckIndex >= visual_num)
	{
		m_nCheckIndex = 0;
		m_bNeedCheck = false;
	}

	return true;
}

