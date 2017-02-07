//--------------------------------------------------------------------
// 文件名:		lighter.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "lighter.h"
#include "light_info.h"
#include "light_geometry.h"
#include "light_oct_tree.h"
#include "light_set.h"
#include "light_processor.h"
#include "terrain.h"
#include "../visual/i_dx_render.h"
#include "../visual/i_dx_context.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

/// \file lighter.cpp
/// \brief 光照渲染

/// entity: Lighter
/// \brief 光照渲染实体
DECLARE_ENTITY("", Lighter, IEntity);

/// property: bool ShadowFinish
/// \brief 生成投影是否完成
DECLARE_PROPERTY_GET(bool, Lighter, ShadowFinish, GetShadowFinish);
/// property: bool SmoothFinish
/// \brief 平滑投影是否完成
DECLARE_PROPERTY_GET(bool, Lighter, SmoothFinish, GetSmoothFinish);
/// property: bool BlurFinish
/// \brief 模糊处理是否完成
DECLARE_PROPERTY_GET(bool, Lighter, BlurFinish, GetBlurFinish);

/// property: float ShadowPercent
/// \brief 生成投影完成百分比（0.0-1.0)
DECLARE_PROPERTY_GET(float, Lighter, ShadowPercent, GetShadowPercent);
/// property: float SmoothPercent
/// \brief 平滑投影完成百分比（0.0-1.0)
DECLARE_PROPERTY_GET(float, Lighter, SmoothPercent, GetSmoothPercent);
/// property: float BlurPercent
/// \brief 模糊处理完成百分比（0.0-1.0)
DECLARE_PROPERTY_GET(float, Lighter, BlurPercent, GetBlurPercent);

/// property: bool UseColor
/// \brief 是否使用模型顶点色
DECLARE_PROPERTY(bool, Lighter, UseColor, GetUseColor, SetUseColor);
/// property: bool OnlyGround
/// \brief 是否只渲染地面光照
DECLARE_PROPERTY(bool, Lighter, OnlyGround, GetOnlyGround, SetOnlyGround);
/// property: bool SaveOcclusion
/// \brief 是否在光贴图中保存遮挡信息
DECLARE_PROPERTY(bool, Lighter, SaveOcclusion, GetSaveOcclusion, 
	SetSaveOcclusion);
/// property: bool LightMapDegrade
/// \brief 是否是否根据面积自动选择光贴图大小降级
DECLARE_PROPERTY(bool, Lighter, LightMapDegrade, GetLightMapDegrade, 
	SetLightMapDegrade);
/// property: bool UseSkyLight
/// \brief 是否使用天空光
DECLARE_PROPERTY(bool, Lighter, UseSkyLight, GetUseSkyLight, SetUseSkyLight);
/// property: bool UsePointLight
/// \brief 是否使用点光源
DECLARE_PROPERTY(bool, Lighter, UsePointLight, GetUsePointLight, 
	SetUsePointLight);
/// property: int LightMapSize
/// \brief 物体光贴图大小
DECLARE_PROPERTY(int, Lighter, LightMapSize, GetLightMapSize, SetLightMapSize);
/// property: int LightMapFill
/// \brief 物体光贴图填充范围
DECLARE_PROPERTY(int, Lighter, LightMapFill, GetLightMapFill, SetLightMapFill);
/// property: int LightPerUnit
/// \brief 物体光照颜色分辨率
DECLARE_PROPERTY(size_t, Lighter, LightPerUnit, GetLightPerUnit, 
	SetLightPerUnit);

/// property: string ZonePath
/// \brief 区域光照文件路径
DECLARE_PROPERTY(const char*, Lighter, ZonePath, GetZonePath, SetZonePath);
/// property: string ModelPath
/// \brief 模型光照文件路径
DECLARE_PROPERTY(const char*, Lighter, ModelPath, GetModelPath, SetModelPath);
/// property: string ModelSuffix
/// \brief 模型光照文件后缀
DECLARE_PROPERTY(const char*, Lighter, ModelSuffix, GetModelSuffix, 
	SetModelSuffix);

/// property: bool TraceShadows
/// \brief 开始生成投影
DECLARE_PROPERTY(bool, Lighter, TraceShadows, GetTraceShadows, 
	SetTraceShadows);
/// property: bool SmoothShadows
/// \brief 开始平滑投影
DECLARE_PROPERTY(bool, Lighter, SmoothShadows, GetSmoothShadows, 
	SetSmoothShadows);
/// property: bool BlurLight
/// \brief 开始模糊处理
DECLARE_PROPERTY(bool, Lighter, BlurLight, GetBlurLight, SetBlurLight);
/// property: bool ResetBlurLight
/// \brief 光照模糊复位
DECLARE_PROPERTY(bool, Lighter, ResetBlurLight, GetResetBlurLight, 
	SetResetBlurLight);
/// property: bool UpdateLight
/// \brief 光源参数改变，刷新光照
DECLARE_PROPERTY(bool, Lighter, UpdateLight, GetUpdateLight, SetUpdateLight);

/// property: bool SmoothNormal
/// \brief 是否使用法线进行平滑处理
DECLARE_PROPERTY(bool, Lighter, SmoothNormal, GetSmoothNormal, 
	SetSmoothNormal);
/// property: float SmoothRadius
/// \brief 平滑半径
DECLARE_PROPERTY(float, Lighter, SmoothRadius, GetSmoothRadius, 
	SetSmoothRadius);

/// property: bool BlurTrace
/// \brief 模糊处理是否使用光线追踪
DECLARE_PROPERTY(bool, Lighter, BlurTrace, GetBlurTrace, SetBlurTrace);
/// property: float BlurRadius
/// \brief 模糊处理半径
DECLARE_PROPERTY(float, Lighter, BlurRadius, GetBlurRadius, SetBlurRadius);
/// property: float BlurAtt
/// \brief 模糊处理衰减
DECLARE_PROPERTY(float, Lighter, BlurAtt, GetBlurAtt, SetBlurAtt);
/// property: float BlurCos
/// \brief 模糊处理角度余弦
DECLARE_PROPERTY(float, Lighter, BlurCos, GetBlurCos, SetBlurCos);
/// property: float BlurCoeff
/// \brief 模糊处理系数
DECLARE_PROPERTY(float, Lighter, BlurCoeff, GetBlurCoeff, SetBlurCoeff);

/// property: float ShadowDistance
/// \brief 产生阴影的最大距离
DECLARE_PROPERTY(float, Lighter, ShadowDistance, GetShadowDistance, 
	SetShadowDistance);

/// property: object Terrain
/// \brief 地形对象
DECLARE_PROPERTY(PERSISTID, Lighter, Terrain, GetTerrainID, SetTerrainID);

/// method: bool AddZone(int zone_index, int density, float add_size)
/// \brief 添加要渲染的区域
/// \param zone_index 区域索引
/// \param density 光照密度
/// \param add_size 附加宽度
DECLARE_METHOD_3(bool, Lighter, AddZone, int, int, float);
/// method: bool AddZoneTest(int zone_index, int density, float add_size, int tex_scale)
/// \brief 添加要渲染的测试区域
/// \param zone_index 区域索引
/// \param density 光照密度
/// \param add_size 附加宽度
/// \param tex_scale 指定贴图尺寸
DECLARE_METHOD_4(bool, Lighter, AddZoneTest, int, int, float, int);
/// method: bool AddModel(string name, object model, string light_type)
/// \brief 添加要渲染的模型
/// \param name 模型名称
/// \param model 模型对象
/// \param light_type 光照类型（"VertexColor"-顶点色, "LightMap"-光贴图）
/// \param light_map_size 光贴图尺寸
/// \param light_map_fill 光贴图填充像素数量
DECLARE_METHOD_5(bool, Lighter, AddModel, const char*, const PERSISTID&, 
	const char*, int, int);
/// method: bool AddLight(object light)
/// \brief 添加光源
/// \param light 光源参数对象
DECLARE_METHOD_1(bool, Lighter, AddLight, const PERSISTID&);
/// method: bool SetLight(object light)
/// \brief 设置光源参数
/// \param light 光源参数对象
DECLARE_METHOD_1(bool, Lighter, SetLight, const PERSISTID&);
/// method: bool PrepareData()
/// \brief 预处理
DECLARE_METHOD_0(bool, Lighter, PrepareData);
/// method: bool SaveModelLight()
/// \brief 保存模型光照文件
DECLARE_METHOD_0(bool, Lighter, SaveModelLight);
/// method: bool SaveZoneLight()
/// \brief 保存区域光照文件
DECLARE_METHOD_0(bool, Lighter, SaveZoneLight);

// Lighter

Lighter::Lighter()
{
	m_pRender = NULL;
	m_pGeometry = CORE_NEW(CLightGeometry);
	m_pOctTree = CORE_NEW(CLightOctTree);
	m_pLights = CORE_NEW(CLightSet);
	m_pProcessor = CORE_NEW(CLightProcessor);

	m_bTraceShadows = false;
	m_bSmoothShadows = false;
	m_bBlurLight = false;
	m_bResetBlurLight = false;
	m_bUpdateLight = false;
	m_bUseSkyLight = false;
	m_bUsePointLight = false;
	
	m_bSmoothNormal = true;
	m_bBlurTrace = true;
	
	m_fSmoothRadius = 0.2f;
	m_fBlurRadius = 5.0f;
	m_fBlurAtt = 2.0f;
	m_fBlurCos = 0.5f;
	m_fBlurCoeff = 0.3f;

	m_fShadowDistance = 0.0f;
}

Lighter::~Lighter()
{
	CORE_DELETE(m_pGeometry);
	CORE_DELETE(m_pOctTree);
	CORE_DELETE(m_pLights);
	CORE_DELETE(m_pProcessor);
}

bool Lighter::Init(const IVarList& args)
{
	const char* render_name = args.StringVal(0);
	
	if (render_name[0] == 0)
	{
		render_name = "DxRender";
	}
	
	m_pRender = (Render*)GetCore()->GetInterface(render_name);
	
	if (NULL == m_pRender)
	{
		CORE_TRACE("(Lighter::Init)no render");
		CORE_TRACE(render_name);
		return false;
	}

	m_pGeometry->SetRender(m_pRender);
	m_pProcessor->Init(this, m_pGeometry, m_pLights, m_pOctTree);

	GetCore()->AddExecute(this);
	
	return true;
}

bool Lighter::Shut()
{
	SAFE_RELEASE(m_pRender);
	
	return true;
}

void Lighter::Execute(float seconds)
{
	m_pProcessor->Process();
}

void Lighter::SetTerrainID(const PERSISTID& id)
{
	Terrain* p = (Terrain*)GetCore()->GetEntity(id);

	if (NULL == p)
	{
		return;
	}

	if (!p->GetEntInfo()->IsKindSameSpace(this, "Terrain"))
	{
		return;
	}

	m_pGeometry->SetTerrain(p);
	
	m_TerrainID = id;
}

PERSISTID Lighter::GetTerrainID() const
{
	return m_TerrainID;
}

bool Lighter::GetShadowFinish() const
{
	int triangle_num = m_pGeometry->GetTriangleCount();
	
	if (0 == triangle_num)
	{
		return true;
	}

	int shadow_num = m_pProcessor->GetShadowTriangleCount();

	if (shadow_num < 0)
	{
		if (!m_bTraceShadows)
		{
			return true;
		}
	}

	return false;
}

bool Lighter::GetSmoothFinish() const
{
	int vertex_num = m_pGeometry->GetVertexCount();
	
	if (0 == vertex_num)
	{
		return true;
	}

	int smooth_num = m_pProcessor->GetSmoothVertexCount();

	if (smooth_num < 0)
	{
		if (!m_bSmoothShadows)
		{
			return true;
		}
	}

	return false;
}

bool Lighter::GetBlurFinish() const
{
	int vertex_num = m_pGeometry->GetVertexCount();
	
	if (0 == vertex_num)
	{
		return true;
	}

	int blur_num = m_pProcessor->GetBlurVertexCount();

	if (blur_num < 0)
	{
		if (!m_bBlurLight)
		{
			return true;
		}
	}
	
	return false;
}

float Lighter::GetShadowPercent() const
{
	int triangle_num = m_pGeometry->GetTriangleCount();
	
	if (0 == triangle_num)
	{
		return 0.0f;
	}

	int shadow_num = m_pProcessor->GetShadowTriangleCount();

	if (shadow_num < 0)
	{
		if (m_bTraceShadows)
		{
			return 0.0f;
		}
		else
		{
			return 1.0f;
		}
	}

	return float(shadow_num) / float(triangle_num);
}

float Lighter::GetSmoothPercent() const
{
	int vertex_num = m_pGeometry->GetVertexCount();
	
	if (0 == vertex_num)
	{
		return 0.0f;
	}

	int smooth_num = m_pProcessor->GetSmoothVertexCount();
	
	if (smooth_num < 0)
	{
		if (m_bSmoothShadows)
		{
			return 0.0f;
		}
		else
		{
			return 1.0f;
		}
	}

	return float(smooth_num) / float(vertex_num);
}

float Lighter::GetBlurPercent() const
{
	int vertex_num = m_pGeometry->GetVertexCount();
	
	if (0 == vertex_num)
	{
		return 0.0f;
	}
	
	int blur_num = m_pProcessor->GetBlurVertexCount();
	
	if (blur_num < 0)
	{
		if (m_bBlurLight)
		{
			return 0.0f;
		}
		else
		{
			return 1.0f;
		}
	}
	
	return float(blur_num) / float(vertex_num);
}

void Lighter::SetUseColor(bool value)
{
	m_pGeometry->SetUseColor(value);
}

bool Lighter::GetUseColor() const
{
	return m_pGeometry->GetUseColor();
}

void Lighter::SetOnlyGround(bool value)
{
	m_pGeometry->SetOnlyGround(value);
}

bool Lighter::GetOnlyGround() const
{
	return m_pGeometry->GetOnlyGround();
}

void Lighter::SetSaveOcclusion(bool value)
{
	m_pGeometry->SetSaveOcclusion(value);
}

bool Lighter::GetSaveOcclusion() const
{
	return m_pGeometry->GetSaveOcclusion();
}

void Lighter::SetLightMapDegrade(bool value)
{ 
	m_pGeometry->SetLightMapDegrade(value);
}

bool Lighter::GetLightMapDegrade() const 
{ 
	return m_pGeometry->GetLightMapDegrade();
}

void Lighter::SetLightMapSize(int value)
{ 
	m_pGeometry->SetLightMapSize(value);
}

int Lighter::GetLightMapSize() const
{ 
	return m_pGeometry->GetLightMapSize();
}

void Lighter::SetLightMapFill(int value)
{ 
	m_pGeometry->SetLightMapFill(value);
}

int Lighter::GetLightMapFill() const
{ 
	return m_pGeometry->GetLightMapFill();
}

void Lighter::SetLightPerUnit(size_t value)
{ 
	m_pGeometry->SetLightPerUnit(value);
}

size_t Lighter::GetLightPerUnit() const
{ 
	return m_pGeometry->GetLightPerUnit();
}

void Lighter::SetZonePath(const char* value)
{
	m_pGeometry->SetZonePath(value);
}

const char* Lighter::GetZonePath() const
{
	return m_pGeometry->GetZonePath();
}

void Lighter::SetModelPath(const char* value)
{
	m_pGeometry->SetModelPath(value);
}

const char* Lighter::GetModelPath() const
{
	return m_pGeometry->GetModelPath();
}

void Lighter::SetModelSuffix(const char* value)
{
	m_pGeometry->SetModelSuffix(value);
}

const char* Lighter::GetModelSuffix() const
{
	return m_pGeometry->GetModelSuffix();
}

bool Lighter::AddZone(int zone_index, int density, float add_size)
{
	return m_pGeometry->AddZone(zone_index, density, add_size, 0);
}

bool Lighter::AddZoneTest(int zone_index, int density, float add_size, 
	int tex_scale)
{
	return m_pGeometry->AddZone(zone_index, density, add_size, tex_scale);
}

bool Lighter::AddModel(const char* name, const PERSISTID& id,
	const char* light_type, int light_map_size, int light_map_fill)
{
	return m_pGeometry->AddObject(name, id, true, light_type, light_map_size,
		light_map_fill);
}

bool Lighter::AddLight(const PERSISTID& info)
{
	LightInfo* pInfo = (LightInfo*)GetCore()->GetEntity(info);

	if (NULL == pInfo)
	{
		return false;
	}

	if (!pInfo->GetEntInfo()->IsKindSameSpace(this, "LightInfo"))
	{
		return false;
	}

	return m_pLights->AddLight(pInfo);
}

bool Lighter::SetLight(const PERSISTID& info)
{
	LightInfo* pInfo = (LightInfo*)GetCore()->GetEntity(info);
	
	if (NULL == pInfo)
	{
		return false;
	}
	
	if (!pInfo->GetEntInfo()->IsKindSameSpace(this, "LightInfo"))
	{
		return false;
	}
	
	return m_pLights->SetLight(pInfo);
}

bool Lighter::PrepareData()
{
	// 主光源	
	Terrain* pTerrain = (Terrain*)GetCore()->GetEntity(m_TerrainID);

	if (NULL == pTerrain)
	{
		return false;
	}

	IDxContext* pContext = pTerrain->GetContext();

	//int light_enable = pContext->GetInt(IDxContext::I_LIGHT_ENABLE);

	//if (light_enable)
	//{
		FmVec4 sun_dir = pContext->GetVector(
			IDxContext::V_LIGHT_DIRECTION);

		m_pLights->SetSunDirection(sun_dir);
	//}

	if (!m_pGeometry->Process(m_pLights->GetCount()))
	{
		// 复位
		m_pGeometry->ReleaseAll();
		return false;
	}
	
	m_pOctTree->Init(m_pGeometry);

	m_pProcessor->UpdateLightsParam();
	
	return true;
}

bool Lighter::SaveModelLight()
{
	return m_pGeometry->SaveModelLight();
}

bool Lighter::SaveZoneLight()
{
	return m_pGeometry->SaveZoneLight();
}
*/
