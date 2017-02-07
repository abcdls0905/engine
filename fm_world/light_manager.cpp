//--------------------------------------------------------------------
// 文件名:		light_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月29日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "light_manager.h"
#include "light_source.h"
#include "../visual/i_context.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
//#include "../visual/dx_scene_batch.h"
#include "../visual/vis_utils.h"
#include "../public/i_ini_file.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"

/// \file light_manager.cpp
/// \brief 灯光管理

/// entity: LightManager
/// \brief 灯光管理实体
/// \see 继承自IVisBase
DECLARE_ENTITY(LightManager,0 , IVisBase);

/// property: int MaxLightNum
/// \brief 最多光源数量
DECLARE_PROPERTY(int, LightManager, MaxLightNum, GetMaxLightNum, 
	SetMaxLightNum);

/*
/// property: float MaxDistance
/// \brief 最大光源距离
DECLARE_PROPERTY(float, LightManager, MaxDistance, GetMaxDistance, 
	SetMaxDistance);
*/

/// property: bool SunLighting
/// \brief 是否开启主光源
DECLARE_PROPERTY(bool, LightManager, SunLighting, GetSunLighting, 
	SetSunLighting);
/// property: bool RolePointLighting
/// \brief 是否开启静态光照时的人物点光源照明
DECLARE_PROPERTY(bool, LightManager, RolePointLighting, GetRolePointLighting, 
	SetRolePointLighting);
/// property: bool EnableShadow
/// \brief 是否开启光源阴影
DECLARE_PROPERTY(bool, LightManager, EnableShadow, GetEnableShadow, 
	SetEnableShadow);
/// property: bool ShadowMapOptimal
/// \brief 是否优化阴影图的生成
DECLARE_PROPERTY(bool, LightManager, ShadowMapOptimal, GetShadowMapOptimal, 
	SetShadowMapOptimal);
/// property: bool UseDepthBoundTest
/// \brief 是否使用深度边界测试
DECLARE_PROPERTY(bool, LightManager, UseDepthBoundTest, GetUseDepthBoundTest, 
	SetUseDepthBoundTest);
/// property: float DepthSlope
/// \brief 深度斜率
DECLARE_PROPERTY(float, LightManager, DepthSlope, GetDepthSlope, 
	SetDepthSlope);
/// property: float DepthBias
/// \brief 深度偏置
DECLARE_PROPERTY(float, LightManager, DepthBias, GetDepthBias, SetDepthBias);
/// property: int FilterLevel
/// \brief 阴影图过滤级别
DECLARE_PROPERTY(int, LightManager, FilterLevel, GetFilterLevel, 
	SetFilterLevel);
 

LightManager::LightManager()
{
	m_pShadowContext = NULL;
	m_pPointVS = NULL;
	m_pSpotVS = NULL;
	m_pBoxVS = NULL;
	m_pLightMaskVS = NULL;
	m_pLightMaskPS = NULL;
//	m_pLightMaskRasterState = NULL;
//	m_pLightMaskDepthState = NULL;
//	m_pLightMaskBlendState = NULL;
//	m_pLightDrawRasterState = NULL;
//	m_pLightDrawDepthState = NULL;
//	m_pLightDrawBlendState = NULL;
//	m_pShadowRasterState = NULL;
	m_nMaxLightNum = 50;
	m_bSunLighting = true;
	m_bRolePointLighting = true;
	m_bEnableShadow = true;
	m_bShadowMapOptimal = false;
	m_bUseDepthBoundTest = true;
	m_fDepthSlope = 0.0001F;
	m_fDepthBias = 0.00015F;
	m_nFilterLevel = 1;
	m_pRandomTex = NULL;
}

LightManager::~LightManager()
{
//	SAFE_RELEASE(m_pLightMaskRasterState);
//	SAFE_RELEASE(m_pLightMaskDepthState);
//	SAFE_RELEASE(m_pLightMaskBlendState);
//	SAFE_RELEASE(m_pLightDrawRasterState);
//	SAFE_RELEASE(m_pLightDrawDepthState);
//	SAFE_RELEASE(m_pLightDrawBlendState);
//	SAFE_RELEASE(m_pShadowRasterState);
	SAFE_RELEASE(m_pRandomTex);
	SAFE_RELEASE(m_pShadowContext);
}

bool LightManager::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	}

	m_pShadowContext = m_pRender->CreateContext();

    /*
	m_pPointVS = m_pRender->LoadVertexShader("prelight.hlsl", 
		"vs_main_point_light", "");
	m_pSpotVS = m_pRender->LoadVertexShader("prelight.hlsl", 
		"vs_main_spot_light", "");
	m_pBoxVS = m_pRender->LoadVertexShader("prelight.hlsl", 
		"vs_main_box_light", "");

	m_PointPSList.SetRender(m_pRender);
	m_PointPSList.SetShaderName("prelight.hlsl", "ps_main_point_light");
	m_PointPSList.SetDefine(POINT_PS_SHADOW, "SHADOW=1;");
	m_PointPSList.SetDefine(POINT_PS_LOWFILTER, "LOWFILTER=1;");
	m_PointPSList.SetDefine(POINT_PS_HIGHFILTER, "HIGHFILTER=1;");
	m_PointPSList.SetDefine(POINT_PS_VSM, "VSM=1;");
	m_PointPSList.SetDefine(POINT_PS_PCF, "PCF=1;");
	m_PointPSList.SetDefine(POINT_PS_ATTENU2, "ATTENU2=1;");
	m_PointPSList.SetDefine(POINT_PS_STEREO, "STEREO=1;");
	m_PointPSList.LoadShader(0);


	m_SpotPSList.SetRender(m_pRender);
	m_SpotPSList.SetShaderName("prelight.hlsl", "vs_main_spot_light");
	m_SpotPSList.SetDefine(SPOT_PS_SHADOW, "SHADOW=1;");
	m_SpotPSList.SetDefine(SPOT_PS_LOWFILTER, "LOWFILTER=1;");
	m_SpotPSList.SetDefine(SPOT_PS_HIGHFILTER, "HIGHFILTER=1;");
	m_SpotPSList.SetDefine(SPOT_PS_VSM, "VSM=1;");
	m_SpotPSList.SetDefine(SPOT_PS_PCF, "PCF=1;");
	m_SpotPSList.SetDefine(SPOT_PS_OVERLAY, "OVERLAY=1;");
	m_SpotPSList.SetDefine(SPOT_PS_ATTENU2, "ATTENU2=1;");
	m_SpotPSList.SetDefine(SPOT_PS_STEREO, "STEREO=1;");
	m_SpotPSList.LoadShader(0);


	m_BoxPSList.SetRender(m_pRender);
	m_BoxPSList.SetShaderName("prelight.hlsl", "ps_main_box_light");
	m_BoxPSList.SetDefine(BOX_PS_ATTENU2, "ATTENU2=1;");
	m_BoxPSList.SetDefine(BOX_PS_STEREO, "STEREO=1;");
	m_BoxPSList.LoadShader(0);

	m_pLightMaskVS = m_pRender->LoadVertexShader("prelight.hlsl",
		"vs_main_light_mask", "");
	m_pLightMaskPS = m_pRender->LoadPixelShader("prelight.hlsl",
		"ps_main_light_mask", "");
    */

	return true;
}

bool LightManager::Shut()
{
	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		LightSource* pLight = (LightSource*)GetCore()->GetEntity(
			m_Lights[i].LightID);

		if (pLight)
		{
			pLight->Release();
		}
	}

	m_Lights.clear();

	return true;
}

void LightManager::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
	// 设置自身为场景唯一对象
	m_pContext->SetUnique(this);
}

int LightManager::DistanceCompare(const void* elem1, 
	const void* elem2)
{
	float d1 = ((view_light_t*)elem1)->fDistance;
	float d2 = ((view_light_t*)elem2)->fDistance;

	if (d1 < d2)
	{
		return -1;
	}

	if (d1 > d2)
	{
		return 1;
	}

	return 0;
}

void LightManager::Update(float seconds)
{
	IRenderContext* pContext = m_pRender->GetContext();
}

void LightManager::Realize()
{
	ApplyLightParam();
	CheckRoleLight();
}
 
static int get_roles_in_light(LightSource* pLight, IRenderContext* pContext)
{
	IEntity* pTerrain = pContext->GetUnique("Terrain");

	if (NULL == pTerrain)
	{
		return 0;
	}

	FmVec3 center = pLight->GetCenter();
	float radius = pLight->GetRadius();
	CVarList result;

 
	return result.IntVal(0);
}



// 计算最近和最远深度
static void calculate_min_max_z(const FmMat4& mtxTransform, 
	const FmVec4* points, size_t point_num, float& min_z, float& max_z)
{
	Assert(points != NULL);
	Assert((point_num >= 2) && (point_num <= 16));
	
	min_z = 1.0F;
	max_z = 0.0F;

	FmVec4 v[16];

	for(size_t t = 0; t < point_num; t++ )
	{
		FmVec4Transform(&v[t],points,&mtxTransform);
	}


	for (size_t i = 0; i < point_num; ++i)
	{
		float z;
		
		if (v[i].w < 0.0F)
		{
			z = 0.0F;
		}
		else
		{
			z = v[i].z / v[i].w;

			if (z < 0.0F)
			{
				z = 0.0F;
			}
			else if (z > 1.0F)
			{
				z = 1.0F;
			}
		}

		if (z > max_z)
		{
			max_z = z;
		}

		if (z < min_z)
		{
			min_z = z;
		}
	}

	Assert(min_z <= max_z);
}


// 返回0到1之间的随机数
static inline float rand_one()
{
	return float(rand()) / float(RAND_MAX_WIN);
}


void LightManager::ApplyLightParam()
{
	IRenderContext* pContext = m_pRender->GetContext();

	unsigned int ambient_color = pContext->GetColor(IRenderContext::C_AMBIENT_COLOR);
	unsigned int diffuse_color = pContext->GetColor(IRenderContext::C_DIFFUSE_COLOR);
	unsigned int specular_color = pContext->GetColor(IRenderContext::C_SPECULAR_COLOR);
	unsigned int fog_color = pContext->GetColor(IRenderContext::C_FOG_COLOR);
	unsigned int fog_exp_color = pContext->GetColor(IRenderContext::C_FOG_EXP_COLOR);
	unsigned int height_fog_color = pContext->GetColor(IRenderContext::C_HEIGHTFOG_COLOR);

	float ambient_intensity = pContext->GetFloat(IRenderContext::F_AMBIENT_INTENSITY);
	float diffuse_intensity = pContext->GetFloat(IRenderContext::F_DIFFUSE_INTENSITY);
	float specular_intensity = pContext->GetFloat(IRenderContext::F_SPECULAR_INTENSITY);

	FmVec4 light_ambient;
	FmVec4 light_diffuse;
	FmVec4 light_specular;
	FmVec4 fog_current;
	FmVec4 fog_exp_current;
	FmVec4 height_fog_current;

	light_ambient = VisUtil_GetColor2(ambient_color, ambient_intensity);
	light_diffuse = VisUtil_GetColor2(diffuse_color, diffuse_intensity);
	light_specular = VisUtil_GetColor2(specular_color, specular_intensity);
	fog_current = VisUtil_ColorToVector4(fog_color);
	fog_exp_current = VisUtil_ColorToVector4(fog_exp_color);
	height_fog_current = VisUtil_ColorToVector4(height_fog_color);
 
	pContext->SetVector4(IRenderContext::V_LIGHT_AMBIENT, light_ambient);
	pContext->SetVector4(IRenderContext::V_LIGHT_DIFFUSE, light_diffuse);
    pContext->SetVector4(IRenderContext::V_LIGHT_SPECULAR, light_specular);
	pContext->SetVector4(IRenderContext::V_FOG_CURRENT, fog_current);
	pContext->SetVector4(IRenderContext::V_FOG_EXP_CURRENT, fog_exp_current);
	pContext->SetVector4(IRenderContext::V_HEIGHTFOG_CURRENT, height_fog_current);
}

void LightManager::CheckRoleLight()
{
	IRenderContext* pContext = m_pRender->GetContext();

	pContext->SetInt(IRenderContext::I_ENABLE_ROLE_LIGHT, 0);

	if (!m_bRolePointLighting)
	{
		return;
	}


	// 找到一个距离角色最近的光源
	FmVec3 player_pos = pContext->GetVector(IRenderContext::V_FOCUS_POSITION);
	float min_distance_sq = 1e8;
	LightSource* pNearestLight = NULL;
	size_t light_num = m_Lights.size();
	
	for (size_t i = 0; i < light_num; ++i)
	{
		LightSource* pLight = (LightSource*)GetCore()->GetEntity(
			m_Lights[i].LightID);

		if (NULL == pLight)
		{
			continue;
		}

		FmVec3 light_pos = pLight->GetPosition();
		float sx = light_pos.x - player_pos.x;
		float sy = light_pos.y - player_pos.y;
		float sz = light_pos.z - player_pos.z;
		float square = sx * sx + sy * sy + sz * sz;

		if (square < min_distance_sq)
		{
			min_distance_sq = square;
			pNearestLight = pLight;
		}
	}

	if (min_distance_sq < (100.0F * 100.0F))
	{
		pContext->SetInt(IRenderContext::I_ENABLE_ROLE_LIGHT, 1);
		pContext->SetVector(IRenderContext::V_ROLE_LIGHT_POSITION,
			pNearestLight->GetPosition());
		pContext->SetColor(IRenderContext::C_ROLE_LIGHT_COLOR,
			pNearestLight->GetColor());
		pContext->SetFloat(IRenderContext::F_ROLE_LIGHT_INTENSITY, 
			pNearestLight->GetIntensity());
		pContext->SetFloat(IRenderContext::F_ROLE_LIGHT_RANGE,
			pNearestLight->GetRange());
	}
}

PERSISTID LightManager::Create()
{
	LightSource* pLight = (LightSource*)GetCore()->CreateEntityArgs("LightSource", CVarList() );

	pLight->SetContext(m_pContext);
	
	light_t data;

	data.LightID = pLight->GetID();
	data.fDistance = 0.0F;

	m_Lights.push_back(data);

	return pLight->GetID();
}

static inline float read_ini_float(IIniFile* pIni, size_t sect_index, 
	const char* key, float def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return (float)atof(pIni->GetSectionItemValue(sect_index, item_index));
}

static inline const char* read_ini_string(IIniFile* pIni, 
	size_t sect_index, const char* key, const char* def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return pIni->GetSectionItemValue(sect_index, item_index);
}

PERSISTID LightManager::CreateFromIni(const PERSISTID& id, 
	const char* ini_name, const char* light_name, const char* helper_name)
{
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(ini_name), ini_name);

	//IIniFile* pIni = m_pRender->GetCacheIni(fname);
	//bool cached = (pIni != NULL);

	//if (NULL == pIni)
	//{
		IIniFile* pIni = GetCore()->CreateIniFile(fname);

		if (!pIni->LoadFromFile())
		{
			pIni->Release();
			CORE_TRACE("(LightManager::CreateFromIni)no config file");
			CORE_TRACE(ini_name);
			CORE_TRACE(light_name);
			return PERSISTID();
		}
	//}

	size_t sect_index;

	if (!pIni->FindSectionIndex(light_name, sect_index))
	{
		//if (!cached)
		//{
			pIni->Release();
		//}

		CORE_TRACE("(LightManager::CreateFromIni)section not found");
		CORE_TRACE(pIni->GetFileName());
		CORE_TRACE(light_name);
		return PERSISTID();
	}

	// 从配置文件中加载光源信息
	const char* light_type = read_ini_string(pIni, sect_index, "LightType", 
		"point");
	float range = read_ini_float(pIni, sect_index, "Range", 10.0F);
	const char* color = read_ini_string(pIni, sect_index, "Color", 
		"255,255,255,255");
	float intensity = read_ini_float(pIni, sect_index, "Intensity", 1.0F);
	float attenu0 = read_ini_float(pIni, sect_index, "Attenu0", 0.0F);
	float attenu1 = read_ini_float(pIni, sect_index, "Attenu1", 1.0F);
	float attenu2 = read_ini_float(pIni, sect_index, "Attenu2", 0.0F);
	float inner_degree = read_ini_float(pIni, sect_index, "InnerDegree", 15.0F);
	float outer_degree = read_ini_float(pIni, sect_index, "OuterDegree", 30.0F);
	float falloff = read_ini_float(pIni, sect_index, "Falloff", 1.0F);
	float box_scale_x = read_ini_float(pIni, sect_index, "BoxScaleX", 1.0F);
	float box_scale_y = read_ini_float(pIni, sect_index, "BoxScaleY", 1.0F);
	float box_scale_z = read_ini_float(pIni, sect_index, "BoxScaleZ", 1.0F);
	float blink = read_ini_float(pIni, sect_index, "Blink", 1.05F);
	float blink_period = read_ini_float(pIni, sect_index, "BlinkPeriod", 0.0F);
	float blink_tick = read_ini_float(pIni, sect_index, "BlinkTick", 0.0F);

	PERSISTID light_id = this->Create();
	LightSource* pLight = (LightSource*)GetCore()->GetEntity(light_id);

	if (NULL == pLight)
	{
		//if (!cached)
		//{
			pIni->Release();
		//}

		return PERSISTID();
	}

	pLight->SetLightTypeString(light_type);
	pLight->SetRange(range);
	pLight->SetColorString(color);
	pLight->SetIntensity(intensity);
	pLight->SetAttenu0(attenu0);
	pLight->SetAttenu1(attenu1);
	pLight->SetAttenu2(attenu2);
	pLight->SetInnerDegree(inner_degree);
	pLight->SetOuterDegree(outer_degree);
	pLight->SetFalloff(falloff);
	pLight->SetBoxScaleX(box_scale_x);
	pLight->SetBoxScaleY(box_scale_y);
	pLight->SetBoxScaleZ(box_scale_z);
	pLight->SetBlink(blink);
	pLight->SetBlinkPeriod(blink_period);
	pLight->SetBlinkTick(blink_tick);

	pLight->LinkToPoint(id, helper_name);

	//if (!cached)
	//{
		pIni->Release();
	//}

	return light_id;
}

bool LightManager::Delete(const PERSISTID& id)
{
	LightSource* pLight = (LightSource*)GetCore()->GetEntity(id);

	if (NULL == pLight)
	{
		return false;
	}

	pLight->Release();
	
	return true;
}

void LightManager::GetLightList(const IVarList& args, IVarList& result)
{
	result.Clear();

	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		result.AddObject(m_Lights[i].LightID);
	}
}

