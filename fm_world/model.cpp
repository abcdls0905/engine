//--------------------------------------------------------------------
// 文件名:		model.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model.h"
#include "../visual/i_model_system.h"

#include "../visual/i_render.h"
#include "../visual/i_context.h"

#include "../visual/i_model_player.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

/// \file model.cpp
/// \brief 3D模型

/// entity: Model
/// \brief 3D模型实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Model, 0, IVisBase);

/// readonly: string Name
/// \brief 名称（测试用）
DECLARE_PROPERTY_GET(const char*, Model, Name, GetName);
/// readonly: bool LightLoaded
/// \brief 模型光照是否成功加载
DECLARE_PROPERTY_GET(bool, Model, LightLoaded, GetLightLoaded);
/// readonly: bool HasLightMap
/// \brief 是否可用光贴图
DECLARE_PROPERTY_GET(bool, Model, HasLightMap, GetHasLightMap);
/// readonly: bool HasNormalMap
/// \brief 是否可用法线贴图
DECLARE_PROPERTY_GET(bool, Model, HasNormalMap, GetHasNormalMap);
/// readonly: bool HasVertexColor
/// \brief 是否可用顶点色
DECLARE_PROPERTY_GET(bool, Model, HasVertexColor, GetHasVertexColor);
/// method: int HelperCount
/// \brief 获得辅助点数量
DECLARE_PROPERTY_GET(int, Model, HelperCount, GetHelperCount);

/// property: bool AsyncLoad
/// \brief 是否异步加载
DECLARE_PROPERTY(bool, Model, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool ShowBoundBox
/// \brief 是否显示包围盒
DECLARE_PROPERTY(bool, Model, ShowBoundBox, GetShowBoundBox, SetShowBoundBox);
/// property: bool UseVertexColor
/// \brief 是否使用顶点光照
DECLARE_PROPERTY(bool, Model, UseVertexColor, GetUseVertexColor, 
	SetUseVertexColor);
/// property: bool UseLightMap
/// \brief 是否使用光贴图
DECLARE_PROPERTY(bool, Model, UseLightMap, GetUseLightMap, SetUseLightMap);
/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, Model, Visible, GetVisible, SetVisible);
/// property: bool TraceEanble
/// \brief 是否检测碰撞
DECLARE_PROPERTY(bool, Model, TraceEnable, GetTraceEnable, SetTraceEnable);
/// property: bool CullEanble
/// \brief 是否剪裁
DECLARE_PROPERTY(bool, Model, CullEnable, GetCullEnable, SetCullEnable);
/// property: bool WaterReflect
/// \brief 是否需要水面反射
DECLARE_PROPERTY(bool, Model, WaterReflect, GetWaterReflect, SetWaterReflect);
/// property: bool CastShadow
/// \brief 不需要投射影子
DECLARE_PROPERTY(bool, Model, CastShadow, GetCastShadow, SetCastShadow);
/// \brief 不需要接受投射影子
DECLARE_PROPERTY(bool, Model, ReceiveShadow, GetReceiveShadow, SetReceiveShadow);
/// property: int WriteVelocity
/// \brief 写入速度纹理标志（0-不写入，1-写入，2-写入空值）
DECLARE_PROPERTY(int, Model, WriteVelocity, GetWriteVelocity, 
	SetWriteVelocity);
/// property: string Color
/// \brief 模型颜色
DECLARE_PROPERTY(result_string, Model, Color, GetColorString, SetColorString);
/// property: int ExtraInfo
/// \brief 附加信息
DECLARE_PROPERTY(int, Model, ExtraInfo, GetExtraInfo, SetExtraInfo);
/// property: string ModelFile
/// \brief 模型文件名
DECLARE_PROPERTY(const char*, Model, ModelFile, GetModelFile, SetModelFile);
/// property: string LightFile
/// \brief 光照文件名
DECLARE_PROPERTY(const char*, Model, LightFile, GetLightFile, SetLightFile);
/// property: string MaterialFile
/// \brief 材质文件名
DECLARE_PROPERTY(const char*, Model, MaterialFile, GetMaterialFile, 
	SetMaterialFile);
/// property: string TexPaths
/// \brief 贴图路径
DECLARE_PROPERTY(const char*, Model, TexPaths, GetTexPaths, SetTexPaths);
/// property: bool bShow
/// \brief 决定是否显示该模型上的辅助点
DECLARE_PROPERTY(bool, Model, ShowHelper, IsShowHelper, ShowHelper);
///string:int SelectHelper
/// \brief 选中辅助点
DECLARE_PROPERTY(int, Model, SelectHelper, GetSelectHelper, SetSelectHelper);

/*
/// method: bool LoadLight()
/// \brief 加载光照文件
DECLARE_METHOD_0(bool, Model, LoadLight);
/// method: bool LoadMaterial()
/// \brief 加载材质文件
DECLARE_METHOD_0(bool, Model, LoadMaterial);

/// method: float TraceHeight(float beg_x, float beg_z, float high_y, float low_y)
/// \brief 检测高度
/// \param beg_x 起始X坐标
/// \param beg_z 起始Z坐标
/// \param high_y 最高点
/// \param low_y 最低点
/// \return 0到1.0表示有碰撞
DECLARE_METHOD_4(float, Model, TraceHeight, float, float, float, float);

/// method: table GetHelperGroupList()
/// \brief 获得所有的辅助点的组名列表
DECLARE_METHOD_T(Model, GetHelperGroupList);
/// method: table GetHelperNameList()
/// \brief 获得所有的辅助点的名字列表
DECLARE_METHOD_T(Model, GetHelperNameList);
/// method: [x, y, z] GetHelperPosition(string group, string name)
/// \brief 获得辅助点的当前位置
/// \param group 辅助点组名
/// \param name 辅助点名字
DECLARE_METHOD_N(Model, GetHelperPosition);
/// method: [x, y, z] GetHelperAngle(string group, string name)
/// \brief 获得辅助点的当前方向
/// \param group 辅助点组名
/// \param name 辅助点名字
DECLARE_METHOD_N(Model, GetHelperAngle);
/// method: GetCameraInfo(string name)
/// \brief 获得指定点（摄像机）的信息
/// \param name 辅助点名字
DECLARE_METHOD_N(Model, GetCameraInfo);
/// method: int TraceHelperInScreen(int screen_x, int screen_y)
/// \brief 通过输出屏幕坐标，来进行射线与辅助点时间的碰撞检测
/// \param screen_x 指定屏幕的x坐标
/// \param screen_y 指定屏幕的y坐标
/// \return 如果碰撞到了辅助点，那么返回一个大于等于0的值，它是这个辅助点的id值，否则返回-1
DECLARE_METHOD_2(int, Model, TraceHelperInScreen, int, int);
///string:GetHelperName(int helpID)
/// \brief 由辅助点ID得到辅助点名
DECLARE_METHOD_1(bool, Model, GetHelperName, int);
///int:GetNodeFromName(const char* helperName)
/// \brief 由辅助点辅助点名得到ID
DECLARE_METHOD_1(int, Model, GetNodeFromName, const char*);

/// method: bool Play()
/// \brief 播放模型
DECLARE_METHOD_0(bool, Model, Play);
/// method: bool Stop()
/// \brief 停止模型播放
DECLARE_METHOD_0(bool, Model, Stop);
/// method: bool Pause()
/// \brief 暂停模型播放
DECLARE_METHOD_0(bool, Model, Pause);
/// method: bool Resume()
/// \brief 恢复模型播放
DECLARE_METHOD_0(bool, Model, Resume);
/// method: bool SetLoop(bool loop)
/// \brief 设置模型是否循环播放
/// \param loop 循环标志
//DECLARE_METHOD_1(bool, Model, SetLoop, bool);
/// property: bool Loop
/// \brief 重复播放
DECLARE_PROPERTY(bool, Model, Loop, GetLoop, SetLoop );
/// method: int GetFrameCount()
/// \brief 获得桢总数
DECLARE_METHOD_0(int, Model, GetFrameCount);
/// method: int GetCurrentFrame()
/// \brief获得当前桢
DECLARE_METHOD_0(int, Model, GetCurrentFrame);

/// method: bool GetCurrentFrame(int nFrame)
/// \brief设置当前桢
DECLARE_METHOD_1(bool, Model, SetCurrentFrame, int);

/// method: double GetCurrentTimeFloat()
/// \brief获取当前时间
DECLARE_METHOD_0(double, Model, GetCurrentTimeFloat);
/// method: bool SetCurrentTimeFloat(double fFrame)
/// \brief设置当前时间
DECLARE_METHOD_1(bool, Model, SetCurrentTimeFloat, double);

/// method: float GetDefaultSpeed()
/// \brief 获得默认播放速度
DECLARE_METHOD_0(float, Model, GetDefaultSpeed);
/// method: bool SetSpeed(float speed)
/// \brief 设置播放速度
/// \param speed 速度（正常速度为1.0）
DECLARE_METHOD_1(bool, Model, SetSpeed, float);

/// method: const char* GetHelperIDFromIndex(int nIndex)
/// \brief 由辅助点索引得到辅助点名
DECLARE_METHOD_1(const char*, Model, GetHelperIDFromIndex, int);

/// method: int GetAllVertexCount()
/// \brief 获取顶点数
DECLARE_METHOD_0(int, Model, GetAllVertexCount);
/// method: int GetAllTriangleCount()
/// \brief 获取三角面数
DECLARE_METHOD_0(int, Model, GetAllTriangleCount);

/// method: int GetNodeCount()
/// \brief 获得模型节点个数
DECLARE_METHOD_0(int, Model, GetNodeCount);
/// method: table GetNodeNameList()
/// \brief 获得模型的所有节点名列表
DECLARE_METHOD_T(Model, GetNodeNameList);
/// method: int GetModelNodeAllowModify(string key)
/// \brief 获得模型节点数据是否可编辑
/// \param key 节点键名
DECLARE_METHOD_1(int, Model, GetModelNodeAllowModify, const char*);
/// method: bool SetMaterialValue(string node_name, string key, string val)
/// \brief 设置模型节点数据
/// \param node_name 材质名
/// \param key 节点键名
/// \param val 节点键值
DECLARE_METHOD_3(bool, Model, SetNodeValue, const char*, const char*, 
	const char*);
/// method: string GetMaterialValue(string node_name, string key)
/// \brief 读取模型节点数据
/// \param node_name 节点名
/// \param key 节点键名
DECLARE_METHOD_2(result_string, Model, GetNodeValue, const char*, const char*);

/// method: int GetMaterialCount()
/// \brief 获取材质数
DECLARE_METHOD_0(int, Model, GetMaterialCount);
/// method: int GetMaterialTexCount()
/// \brief 获取指定的材质的纹理数
/// \param mat_index 材质索引
DECLARE_METHOD_1(int, Model, GetMaterialTexCount, int);
/// method: string GetMaterialTexName(int mat_index, int tex_index)
/// \brief 取得指定材质的指定纹理贴图名
/// \param mat_index 材质索引
/// \param tex_index 贴图索引
DECLARE_METHOD_2(const char*, Model, GetMaterialTexName, int, int);
/// method: string GetMaterialAmbient(int mat_index)
/// \brief 取得指定材质的Ambient
/// \param mat_index 材质索引
DECLARE_METHOD_1(result_string, Model, GetMaterialAmbient, int);
/// method: string GetMaterialDiffuse(int mat_index)
/// \brief 取得指定材质的Diffuse
/// \param mat_index 材质索引
DECLARE_METHOD_1(result_string, Model, GetMaterialDiffuse, int);
/// method: string GetVertexDiffuse(size_t vb_index, size_t vertex_index)
/// \brief 取得指定顶点缓冲的指定顶点的颜色数据
/// \param vb_index VB索引
/// \param vertex_index 顶点索引
DECLARE_METHOD_2(result_string, Model, GetVertexDiffuse, size_t, size_t);

/// method: table GetMaterialNameList()
/// \brief 获得模型的所有材质名列表
DECLARE_METHOD_T(Model, GetMaterialNameList);
/// method: bool SetMaterialValue(string mat_name, string key, string val)
/// \brief 设置模型材质数据
/// \param mat_name 材质名
/// \param key 材质键名
/// \param val 材质键值
DECLARE_METHOD_3(bool, Model, SetMaterialValue, const char*, const char*,
	const char*);
/// method: string GetMaterialValue(string mat_name, string key)
/// \brief 读取模型材质数据
/// \param mat_name 材质名
/// \param key 材质键名
DECLARE_METHOD_2(result_string, Model, GetMaterialValue, const char*, 
	const char*);
/// method: bool ReloadMaterialTextures()
/// \brief 重新加载模型材质的所有贴图
DECLARE_METHOD_0(bool, Model, ReloadMaterialTextures);

/// method: bool FindCustomMaterial(string mat_name)
/// \brief 查找是否存在自定义材质数据
/// \param mat_name 材质名
DECLARE_METHOD_1(bool, Model, FindCustomMaterial, const char*);

/// method: bool SetCustomMaterialValue(string mat_name, string key, string val)
/// \brief 设置自定义材质数据
/// \param mat_name 材质名
/// \param key 材质键名
/// \param val 材质键值
DECLARE_METHOD_3(bool, Model, SetCustomMaterialValue, const char*, 
	const char*, const char*);
/// method: string GetCustomMaterialValue(string mat_name, string key)
/// \brief 读取自定义材质数据
/// \param mat_name 材质名
/// \param key 材质键名
DECLARE_METHOD_2(result_string, Model, GetCustomMaterialValue, const char*, 
	const char*);
/// method: bool ReloadCustomMaterialTexture()
/// \brief 重新加载自定义材质的指定贴图
DECLARE_METHOD_2(bool, Model, ReloadCustomMaterialTexture, const char*, 
	const char*);
/// method: bool ReloadCustomMaterialTextures()
/// \brief 重新加载自定义材质的所有贴图
DECLARE_METHOD_0(bool, Model, ReloadCustomMaterialTextures);

/// method: bool AfterLoaded()
/// \brief 加载完成后
DECLARE_METHOD_0(bool, Model, AfterLoaded);

/// property: unsigned int CurLODLevel
/// \brief Current LOD Level
DECLARE_PROPERTY(unsigned int, Model, CurLODLevel, GetCurLODLevel, 
	SetCurLODLevel);
/// readonly: unsigned int LODLevels
/// \brief 模型总共的LOD级数
DECLARE_PROPERTY_GET(unsigned int, Model, LODLevels, GetLODLevels);

/// Warning:This API just for LOD tools use!
/// method: bool LOD()
/// \brief 加载光照文件
DECLARE_METHOD_2(bool, Model, LOD, unsigned int, float);
*/

const unsigned int VISUAL_EXTRA_INFO_CAN_BREAKED = 0x00000001;
const unsigned int VISUAL_EXTRA_INFO_NOT_PLAYING = 0x00000002;

// Model

Model::Model()
{
	m_updateCenter = false;
	m_pModelSystem = NULL;
	m_bAsyncLoad = false;
	m_bShowBoundBox = false;
	m_bUseVertexColor = true;
	m_bUseLightMap = false;
	m_bVisible = true;
	m_bUnderWater = false;
	m_bTraceEnable = true;
	m_bCullEnable = true;
	m_bWaterReflect = false;
	m_bCastShadow = false;
	m_bReceiveShadow = false;
	m_nWriteVelocity = 0;
	m_nColor = 0xFFFFFFFF;
	m_fAlphaValue = 1.0F;
	m_nExtraInfo = 0;
	m_pModelPlayer = NULL;
	m_pRender = NULL;
	FmMat4Identity(&m_mtxWorld);
}

Model::~Model()
{
	SAFE_RELEASE(m_pModelPlayer);
}

bool Model::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	}

	m_pModelSystem = (IModelSystem*)GetCore()->GetInterface("ModelSystem");

	if (NULL == m_pModelSystem)
	{
		CORE_TRACE("(Model::Init)no ModelSystem");
		return false;
	}
	
	return true;
}

bool Model::Shut()
{
	SAFE_RELEASE(m_pModelPlayer);
	SAFE_RELEASE(m_pModelSystem);

	SAFE_RELEASE(m_pRender);
	return true;
}

bool Model::Load()
{
	return Create();
}

void Model::Update(float seconds)
{
	if (m_pModelPlayer)
	{
		m_pModelPlayer->Update(seconds);
	}
	m_offset_seconds = seconds;
}

void Model::Realize()
{
	if (!m_bVisible)
	{
		return;
	}

	if (NULL == m_pModelPlayer)
	{
		return;
	}
	
		// 获取当前视图模式
	ISceneView::SCENE_VIEW_TYPE scene_view_type = m_pRender->GetSceneView()->GetSceneViewType();
	switch(scene_view_type)
	{
	case ISceneView::TYPE_NORMAL:
		RealizeNormal();
		break;
	case ISceneView::TYPE_REFLECTION:
		RealizeReflect();
		break;
	default:
        break;
	}

}

void Model::RealizeNormal()
{
	//if(!m_pModelPlayer->IsLoadComplete())
	//	return;
#ifdef PERFORMANCE_DEBUG
	double dRealizeModel = performance_time();
#endif

	IRenderContext* pContext = m_pRender->GetContext();

  m_bShowBoundBox = true;
	// 设置模型颜色
	m_pModelPlayer->SetColor(m_nColor);
	m_pModelPlayer->SetUnderWater(m_bUnderWater);
	m_pModelPlayer->SetShowBoundBox(m_bShowBoundBox);
	m_pModelPlayer->Draw(pContext->GetClipPlanes(), pContext->GetClipPlaneAmount());

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dModelRealizeTime +=
		performance_time() - dRealizeModel;
#endif
}

void Model::RealizeReflect()
{
#ifdef PERFORMANCE_DEBUG
	double dRealizeModel = performance_time();
#endif

	IRenderContext* pContext = m_pRender->GetContext();

	// 设置模型颜色
	m_pModelPlayer->SetColor(m_nColor);
	m_pModelPlayer->SetUnderWater(m_bUnderWater);
	m_pModelPlayer->SetShowBoundBox(m_bShowBoundBox);
	m_pModelPlayer->DrawReflect(pContext->GetClipPlanes(), pContext->GetClipPlaneAmount());

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dModelRealizeTime +=
		performance_time() - dRealizeModel;
#endif
}

void Model::RealizeShadowMap()
{
	if (!m_bVisible)
	{
		return;
	}

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	// 在淡入淡出时也渲染阴影有助于减少画面突变
	//if (((m_nColor >> 24) & 0xFF) < 255)
	//{
	//	return;
	//}

	IRenderContext* pContext = m_pRender->GetContext();

	// 设置模型颜色
	m_pModelPlayer->SetColor(m_nColor);
	m_pModelPlayer->SetUnderWater(m_bUnderWater);
	m_pModelPlayer->SetShowBoundBox(m_bShowBoundBox);
	m_pModelPlayer->DrawShadowMap(pContext->GetClipPlanes(), pContext->GetClipPlaneAmount());
}

bool Model::GetLoaded()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	if (!m_pModelPlayer->IsReady())
	{
		return false;
	}

	return true;
}

bool Model::GetLoadFinish()
{
	if (m_pModelPlayer != NULL)
	{
		if (!m_pModelPlayer->IsLoadComplete())
		{
			return false;
		}
	}
	
	return true;
}

bool Model::AfterLoaded()
{
	if (m_nExtraInfo & VISUAL_EXTRA_INFO_NOT_PLAYING)
	{
		Pause();

		SetCurrentFrame(0);
	}

	return true;
}

bool Model::SetPosition(float x, float y, float z)
{
	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}

	return true;
}

FmVec3 Model::GetPosition() const
{
	return FmVec3(m_mtxWorld._41, m_mtxWorld._42, 
		m_mtxWorld._43);
}

bool Model::SetAngle(float x, float y, float z)
{
	VisUtil_SetMatrixAngle(&m_mtxWorld, x, y, z);

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}

	return true;
}

FmVec3 Model::GetAngle() const
{
	FmVec3 angle;

	VisUtil_GetMatrixAngle(&angle, &m_mtxWorld);

	return angle;
}

bool Model::SetScale(float x, float y, float z)
{
	VisUtil_SetMatrixScale(&m_mtxWorld, x, y, z);
	
	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}

	return true;
}

FmVec3 Model::GetScale() const
{
	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, &m_mtxWorld);

	return scale;
}

FmVec3 Model::GetCenter()
{ 
	IModelPlayer* pModelPlayer = m_pModelPlayer;

	if (NULL == pModelPlayer)
	{
		return GetPosition();
	}
	if(!m_updateCenter)
	{
		if(GetLoadFinish())
		{
			Update(0);
			m_updateCenter = true;
		}
	}
	
	return pModelPlayer->GetCenter();
}

FmVec3 Model::GetBoxSize()
{ 
	IModelPlayer* pModelPlayer = m_pModelPlayer;

	if (NULL == pModelPlayer)
	{
		return FmVec3(0.0F, 0.0F, 0.0F);
	}

	return pModelPlayer->GetBoxSize();
}

float Model::GetRadius()
{ 
	IModelPlayer* pModelPlayer = m_pModelPlayer;
	
	if (NULL == pModelPlayer)
	{
		return 0.0F;
	}

	return pModelPlayer->GetRadius();
}

void Model::SetWorldMatrix(const FmMat4& mat)
{
	m_mtxWorld = mat;
	
	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}
}

void Model::GetWorldMatrix(FmMat4& mat)
{
	mat = m_mtxWorld;
}

bool Model::Create()
{
	// 分析文件扩展名字
	char model_name[256];
	const char* model_file = m_strModelFile.c_str();
	const char* dot = strrchr(model_file, '.');

	if (NULL == dot)
	{
		SafeSprintf(model_name, sizeof(model_name), "%s%s.xmod", 
			m_pRender->GetPathPrefix(model_file), model_file);
		model_file = model_name;
	}

	SAFE_RELEASE(m_pModelPlayer);
	m_pModelPlayer = m_pModelSystem->CreateModelPlayer(model_file,
		0.0, m_strTexPaths.c_str(), m_bAsyncLoad);

	if (NULL == m_pModelPlayer)
	{
		CORE_TRACE("(Model::Create)can't load the model");
		CORE_TRACE(model_file);
		return false;
	}

	m_pModelPlayer->SetContext(m_pContext);
	m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	m_pModelPlayer->SetUseVertexColor(m_bUseVertexColor);
	m_pModelPlayer->SetUseLightMap(m_bUseLightMap);
	m_pModelPlayer->SetWriteVelocity(m_nWriteVelocity);

	// 加载光照
	if (!m_strLightFile.empty())
	{
		m_pModelPlayer->LoadLight(m_strLightFile.c_str(), m_bAsyncLoad);
	}

	// 加载材质文件
	if (!m_strMaterialFile.empty())
	{
		m_pModelPlayer->LoadMaterial(m_strMaterialFile.c_str(), m_bAsyncLoad);
	}

  SetCastShadow(true);
  SetReceiveShadow(true);

	return true;
}

bool Model::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	if (!m_bVisible)
	{
		return false;
	}

	if (!m_bTraceEnable)
	{
		return false;
	}

	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	if (!GetLoadFinish())
	{
		return false;
	}

	// 先做包围球剪裁
	//FmVec3 center = GetCenter();
	//float radius = GetRadius();
	//FmVec3 dir = dst - src;

	//FmVec3 cross;

	//D3DXVec3Cross(&cross, &(center - src), &dir);

	//float distance = D3DXVec3LengthSq(&cross);
	//float dlmn = D3DXVec3LengthSq(&dir);

	//if (distance > (dlmn * radius * radius))
	//{
	//	return false;
	//}

	int old_hit_count = result.nHitCount;
	
	if (!m_pModelPlayer->TraceDetail(src, dst, result))
	{
		return false;
	}
	
	// 设置碰撞到的对象ID
	if (result.nHitMax > 1)
	{
		for (int t = old_hit_count; t < result.nHitCount; ++t)
		{
			result.Hits[t].TraceID = this->GetID();
		}
	}
	else
	{
		result.Hits[0].TraceID = this->GetID();
	}

	return true;
}

bool Model::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	if (!m_bVisible)
	{
		return false;
	}

	if (!m_bTraceEnable)
	{
		return false;
	}

	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	if (!GetLoadFinish())
	{
		return false;
	}

	// 先做包围球剪裁
	//FmVec3 center = GetCenter();
	//float radius = GetRadius();
	//FmVec3 dir = dst - src;

	//FmVec3 cross;	

	//D3DXVec3Cross(&cross, &(center - src), &dir);

	//float distance = D3DXVec3LengthSq(&cross);
	//float dlmn = D3DXVec3LengthSq(&dir);

	//if (distance > (dlmn * radius * radius))
	//{
	//	return false;
	//}

	return m_pModelPlayer->TraceHitted(src, dst);
}

int Model::TraceHelper(const FmVec3& start, const FmVec3& direction)
{
	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	void* node = m_pModelPlayer->RayTraceHelper(&start, &direction);

	if (node)
	{
		return m_pModelPlayer->GetNodeID(node);
	}

	return -1;
}

int Model::TraceHelperInScreen(int screen_x, int screen_y)
{
	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	IRenderContext* pContext = this->GetContext();

	if (NULL == pContext)
	{
		return false;
	}

	//const dx_viewport_t& vp = pContext->GetCamera().Viewport;
	int view_width = m_pContext->GetViewWidth();
	int view_height = m_pContext->GetViewHeight();
	const FmMat4& mtxProj = pContext->GetCamera().mtxProj;
	const FmMat4& m = pContext->GetCamera().mtxViewInverse;
	//int width = (int)vp.fWidth;
	//int height = (int)vp.fHeight;

	// Compute the vector of the pick ray in screen space
	FmVec3 v1;

	v1.x = (((2.0F * screen_x) / (float)view_width) - 1.0F) / mtxProj._11;
	v1.y = -(((2.0F * screen_y) / (float)view_height) - 1.0F) / mtxProj._22;
	v1.z = 1.0F;

	FmVec3 vPickRayDir;

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x = v1.x * m._11 + v1.y * m._21 + v1.z * m._31;
	vPickRayDir.y = v1.x * m._12 + v1.y * m._22 + v1.z * m._32;
	vPickRayDir.z = v1.x * m._13 + v1.y * m._23 + v1.z * m._33;
	FmVec3Normalize(&vPickRayDir, &vPickRayDir);

	FmVec3 vPickRayOrig;

	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	void* node = m_pModelPlayer->RayTraceHelper(&vPickRayOrig, &vPickRayDir);
	
	if (node)
	{
		return m_pModelPlayer->GetNodeID(node);
	}

	return -1;
}

bool Model::TraceSphere(const FmVec3& center, float radius)
{
	if (!m_bVisible)
	{
		return false;
	}

	if (!m_bTraceEnable)
	{
		return false;
	}

	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	if (!GetLoadFinish())
	{
		return false;
	}

	// 先做包围球剪裁
	//float r = GetRadius();
	//float square = D3DXVec3LengthSq(&(center - GetCenter()));

	//if (square > (r + radius) * (r + radius))
	//{
	//	return false;
	//}

	return m_pModelPlayer->TraceSphere(center, radius);
}

// 三角形碰撞
bool Model::TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2)
{
	if (!m_bVisible)
	{
		return false;
	}

	if (!m_bTraceEnable)
	{
		return false;
	}

	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	if (!GetLoadFinish())
	{
		return false;
	}

	return m_pModelPlayer->TraceTriangle(v0, v1, v2);    
}

bool Model::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	if (!m_bVisible)
	{
		return false;
	}
	
	if (!m_bCullEnable)
	{
		return false;
	}
	
	if (NULL == m_pModelPlayer)
	{
		return false;
	}
	
	if (!GetLoadFinish())
	{
		return false;
	}

	// 先做包围球剪裁
	//FmVec3 v = GetCenter();
	//float r = GetRadius();
	//float square = D3DXVec3LengthSq(&(center - v));

	//if (square > (r + radius) * (r + radius))
	//{
	//	return false;
	//}

	return m_pModelPlayer->Cull(planes, plane_num, center, radius, cull_func, 
		context, m_mtxWorld);
}

bool Model::GetLightLoaded() const
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->GetLightLoaded();
}

bool Model::GetHasLightMap() const
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->GetLightMapEnable();
}

bool Model::GetHasNormalMap() const
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->GetNormalMapEnable();
}

bool Model::GetHasVertexColor() const
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->GetVertexColorEnable();
}

void Model::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool Model::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

void Model::SetShowBoundBox(bool value)
{
	m_bShowBoundBox = value; 
}

bool Model::GetShowBoundBox() const
{
	return m_bShowBoundBox;
}

void Model::SetUseVertexColor(bool value)
{
	m_bUseVertexColor = value;

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetUseVertexColor(value);
	}
}

bool Model::GetUseVertexColor() const
{
	return m_bUseVertexColor;
}

void Model::SetUseLightMap(bool value)
{
	m_bUseLightMap = value;
	
	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetUseLightMap(value);
	}
}

bool Model::GetUseLightMap() const
{
	return m_bUseLightMap;
}

void Model::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Model::GetVisible() const
{
	return m_bVisible;
}

void Model::SetUnderWater(bool value)
{
	m_bUnderWater = value;
}

bool Model::GetUnderWater() const
{
	return m_bUnderWater;
}

void Model::SetColor(unsigned int value)
{
	m_nColor = value;
}

unsigned int Model::GetColor() const
{
	return m_nColor;
}

void Model::SetTraceEnable(bool value)
{
	m_bTraceEnable = value;
}

bool Model::GetTraceEnable() const
{
	return m_bTraceEnable;
}

void Model::SetCullEnable(bool value)
{
	m_bCullEnable = value;
}

bool Model::GetCullEnable() const
{
	return m_bCullEnable;
}

void Model::SetWriteVelocity(int value)
{ 
	m_nWriteVelocity = value; 

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWriteVelocity(value);
	}
}

int Model::GetWriteVelocity() const 
{ 
	return m_nWriteVelocity; 
}

void Model::SetCastShadow(bool value)
{
	m_bCastShadow = value; 

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetCastShadow(m_bCastShadow);
	}
}

void Model::SetReceiveShadow(bool value)
{
	m_bReceiveShadow = value; 

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetReceiveShadow(m_bReceiveShadow);
	}
}

void Model::SetColorString(const char* value)
{
	m_nColor = VisUtil_ColorToInt(value);
}

result_string Model::GetColorString() const
{
	return VisUtil_IntToColor(m_nColor);
}

void Model::SetExtraInfo(int value)
{
	m_nExtraInfo = value;
}

int Model::GetExtraInfo() const
{
	return m_nExtraInfo;
}

void Model::SetModelFile(const char* value)
{
	Assert(value != NULL);

	m_strModelFile = value;
}

const char* Model::GetModelFile() const
{
	return m_strModelFile.c_str();
}

void Model::SetLightFile(const char* value)
{
	Assert(value != NULL);

	m_strLightFile = value;
}

const char* Model::GetLightFile() const
{
	return m_strLightFile.c_str();
}

void Model::SetMaterialFile(const char* value)
{
	Assert(value != NULL);

	m_strMaterialFile = value;
}

const char* Model::GetMaterialFile() const
{
	return m_strMaterialFile.c_str();
}

void Model::SetTexPaths(const char* value)
{
	Assert(value != NULL);

	m_strTexPaths = value;
}

const char* Model::GetTexPaths() const
{
	return m_strTexPaths.c_str();
}

bool Model::LoadLight()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}
	
	return m_pModelPlayer->LoadLight(m_strLightFile.c_str(), m_bAsyncLoad);
}

bool Model::LoadMaterial()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->LoadMaterial(m_strMaterialFile.c_str(), m_bAsyncLoad);
}

float Model::TraceHeight(float x, float z, float high_y, float low_y)
{
	trace_info_t result;

	if (!TraceDetail(FmVec3(x, high_y, z), FmVec3(x, low_y, z), 
		result))
	{
		return 2.0F;
	}

	return result.fDistance;
}

void Model::GetHelperGroupList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	size_t label_num = m_pModelPlayer->GetLabelCount();

	for (size_t i = 0; i < label_num; ++i)
	{
		result.AddString(m_pModelPlayer->GetLabelGroup(i));
	}
}

void Model::GetHelperNameList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (NULL == m_pModelPlayer)
	{
		return;
	}
	
	unsigned int label_num =  m_pModelPlayer->GetHelperCount();

	for (unsigned int i = 0; i < label_num; ++i)
	{
		result.AddString(m_pModelPlayer->GetHelperIDFromIndex(i));
	}
}

void Model::GetHelperPosition(const IVarList& args, IVarList& result)
{
	const char* group = args.StringVal(0);
	const char* name = args.StringVal(1);

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	int index;
	
	if (strlen(group) == 0)
	{
		// 只使用名字进行查找
		index = m_pModelPlayer->FindLabelNameIndex(name);
	}
	else
	{
		index = m_pModelPlayer->FindLabelIndex(group, name);
	}

	if (index < 0)
	{
		return;
	}

	FmMat4 local_matrix;
	FmVec3 pos;

	m_pModelPlayer->GetLabelMatrix(index, local_matrix);
	pos.x = local_matrix.m[3][0];
	pos.y = local_matrix.m[3][1];
	pos.z = local_matrix.m[3][2];
	FmVec3TransformCoord(&pos, &pos, &m_mtxWorld);

	result.AddFloat(pos.x);
	result.AddFloat(pos.y);
	result.AddFloat(pos.z);
}

void Model::GetHelperAngle(const IVarList& args, IVarList& result)
{
	const char* group = args.StringVal(0);
	const char* name = args.StringVal(1);

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	int index;

	if (strlen(group) == 0)
	{
		// 只使用名字进行查找
		index = m_pModelPlayer->FindLabelNameIndex(name);
	}
	else
	{
		index = m_pModelPlayer->FindLabelIndex(group, name);
	}

	if (index < 0)
	{
		return;
	}

	FmMat4 local_matrix;
	FmMat4 mat;
	FmVec3 angle;

	m_pModelPlayer->GetLabelMatrix(index, local_matrix);
	//VisUtil_MatrixMultiply(&mat, &local_matrix, &m_mtxWorld);
	FmMat4Multiply(&mat,&local_matrix, &m_mtxWorld);
	VisUtil_GetMatrixAngle(&angle, &mat);

	result.AddFloat(angle.x);
	result.AddFloat(angle.y);
	result.AddFloat(angle.z);
}

void Model::GetCameraInfo(const IVarList& args, IVarList& result)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	const char* name = args.StringVal(0);
	void* pNode = m_pModelPlayer->GetNodeFromName(name);

	if (NULL == pNode)
	{
		return;
	}

	if (m_pModelPlayer->GetNodeType(pNode) != FXNODE_TYPE_HELPER)
	{
		return;
	}

	result.AddFloat(m_pModelPlayer->GetCameraNodeFov(pNode));
	result.AddFloat(m_pModelPlayer->GetCameraNodeFarClip(pNode));
	result.AddFloat(m_pModelPlayer->GetCameraNodeNearClip(pNode));
}

void Model::ShowHelper(bool bShow)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	m_pModelPlayer->SetShowHelper(bShow);
}

bool Model::IsShowHelper() const
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->GetShowHelper();
}

void Model::GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM)
{
	Assert(pTM != NULL);
	Assert(nNodeID >= 0);

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	void* pNode = m_pModelPlayer->GetNodeFromID(nNodeID);

	m_pModelPlayer->GetCurrentNodeTM(pNode, pTM);
}

bool Model::GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM)
{
	Assert(pNodeName != NULL);
	Assert(pTM != NULL);

	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	void* pNode = m_pModelPlayer->GetNodeFromName(pNodeName);

	if (NULL == pNode)
	{
		return false;
	}

	m_pModelPlayer->GetCurrentNodeTM(pNode, pTM);

	return true;
}

int Model::GetNodeFromName(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	void* pNode = m_pModelPlayer->GetNodeFromName(pszName);

	if (pNode == NULL)
	{
		return -1;
	}

	return m_pModelPlayer->GetNodeID(pNode);
}

const char* Model::GetHelperName(int helpID)
{
	if (NULL == m_pModelPlayer)
	{
		return "";
	}

	void* pNode = m_pModelPlayer->GetNodeFromID(helpID);

	if (NULL == pNode)
	{
		return "";
	}

	return m_pModelPlayer->GetNodeName(pNode);
}

bool Model::Play()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->Play();

	return true;
}

bool Model::Stop()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->Stop();

	return true;
}

bool Model::Pause()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->Pause();

	return true;
}

bool Model::Resume()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->Resume();

	return true;
}

void Model::SetLoop(bool bLoop)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	m_pModelPlayer->SetLoop(bLoop);
}

bool Model::GetLoop()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	if (m_pModelPlayer->GetLoop())
	{
		return true;
	}

	return false;
}

int Model::GetFrameCount()
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetFrameCount();
}

int Model::GetCurrentFrame()
{
	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	return m_pModelPlayer->GetCurrentFrame();
}

bool Model::SetCurrentFrame(int nFrame)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->SetCurrentFrame(nFrame);

	return true;
}

double Model::GetCurrentTimeFloat()
{
	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	return m_pModelPlayer->GetCurrentTimeFloat();
}

bool Model::SetCurrentTimeFloat(double fFrame)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->SetCurrentTimeFloat(fFrame);

	return true;
}

float Model::GetDefaultSpeed()
{
	if (NULL == m_pModelPlayer)
	{
		return 0.0F;
	}

	return m_pModelPlayer->GetSpeed();
}

bool Model::SetSpeed(float speed)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->SetSpeed(speed);
	
	return true;
}

void Model::SetSelectHelper(int nHelper)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	m_pModelPlayer->SetSelectHelper(nHelper);
}

int Model::GetSelectHelper()
{
	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	return m_pModelPlayer->GetSelectHelper();
}

int Model::GetHelperCount()
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetHelperCount();
}

const char* Model::GetHelperIDFromIndex(int nIndex)
{
	if (NULL == m_pModelPlayer)
	{
		return "";
	}

	return m_pModelPlayer->GetHelperIDFromIndex(nIndex);
}

size_t Model::GetObjectCount() const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetObjectCount();
}

size_t Model::GetObjectVertexCount(size_t index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetObjectVertexCount(index);
}

size_t Model::GetObjectTriangleCount(size_t index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetObjectTriangleCount(index);
}

int Model::GetAllVertexCount() const
{
	size_t ver_count = 0;
	size_t obj_count = GetObjectCount();

	for (size_t i = 0; i < obj_count; ++i)
	{
		ver_count += GetObjectVertexCount(i);
	}

	return (int)ver_count;
}

int Model::GetAllTriangleCount()
{
	size_t tri_count = 0;
	size_t obj_count = GetObjectCount();

	for (size_t i = 0; i < obj_count; ++i)
	{
		tri_count += GetObjectTriangleCount(i);
	}

	return (int)tri_count;
}

int Model::GetNodeCount()
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return (int)m_pModelPlayer->GetNodeCount();
}

void Model::GetNodeNameList(const IVarList& args, IVarList& result)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	int node_num = m_pModelPlayer->GetNodeCount();

	for (int i = 0; i < node_num; ++i)
	{
		void* node = m_pModelPlayer->GetNodeFromID(i);

		if (node)
		{
			const char* node_name = m_pModelPlayer->GetNodeName(node);

			result.AddString(node_name);
		}
	}
}

bool Model::GetModelNodeAllowModify(const char* key)
{
	return m_pModelPlayer->GetModelNodeAllowModify(key);
}

bool Model::SetNodeValue(const char* node_name, const char* key, const char* val)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->SetModelNodeValue(node_name, key, val);
}

result_string Model::GetNodeValue(const char* node_name, const char* key)
{
	if (NULL == m_pModelPlayer)
	{
		return result_string();
	}

	char val[256];

	if (!m_pModelPlayer->GetModelNodeValue(node_name, key, val, sizeof(val)))
	{
		return result_string();
	}

	return result_string(val);
}

int Model::GetMaterialCount() const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return (int)m_pModelPlayer->GetMaterialCount();
}

int Model::GetMaterialTexCount(int index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return (int)m_pModelPlayer->GetMaterialTextureCount(index);
}

const char* Model::GetMaterialTexName(int mat_index, int tex_index) const
{
	if (NULL == m_pModelPlayer)
	{
		return "";
	}

	const char* pName = m_pModelPlayer->GetMaterialTextureName(mat_index, 
		tex_index);

	if (NULL == pName)
	{
		return "";
	}

	return pName;
}

result_string Model::GetMaterialAmbient(size_t mat_index) const
{
	if (NULL == m_pModelPlayer)
	{
		return "0,0,0,0";
	}

	unsigned int ambient = m_pModelPlayer->GetMaterialAmbient(mat_index);

	return VisUtil_IntToColor(ambient);
}

result_string Model::GetMaterialDiffuse(size_t mat_index) const
{
	if (NULL == m_pModelPlayer)
	{
		return "0,0,0,0";
	}

	unsigned int diffuse = m_pModelPlayer->GetMaterialDiffuse(mat_index);

	return VisUtil_IntToColor(diffuse);
}

result_string Model::GetVertexDiffuse(size_t vb_index, size_t vertex_index) const
{
	if (NULL == m_pModelPlayer)
	{
		return "0";
	}

	unsigned int diffuse = m_pModelPlayer->GetVertexDiffuse(vb_index, 
		vertex_index);

	return VisUtil_IntToColor(diffuse);
}

void Model::GetMaterialNameList(const IVarList& args, IVarList& result)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	int material_num = (int)m_pModelPlayer->GetMaterialCount();

	for (int i = 0; i < material_num; ++i)
	{
		void* mat = m_pModelPlayer->GetMaterialFromID(i);

		if (mat)
		{
			const char* mat_name = m_pModelPlayer->GetMaterialName(mat);

			result.AddString(mat_name);
		}
	}
}

bool Model::SetMaterialValue(const char* mat_name, const char* key, 
	const char* val)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->SetModelMaterialValue(mat_name, key, val);
}

result_string Model::GetMaterialValue(const char* mat_name, const char* key)
{
	if (NULL == m_pModelPlayer)
	{
		return result_string();
	}

	char val[256];

	if (!m_pModelPlayer->GetModelMaterialValue(mat_name, key, val, sizeof(val)))
	{
		return result_string();
	}

	return result_string(val);
}

bool Model::ReloadMaterialTextures()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}
	
	return m_pModelPlayer->ReloadMaterialTextures( 
		m_strTexPaths.c_str(), m_bAsyncLoad);
}

bool Model::FindCustomMaterial(const char* mat_name)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->FindCustomMaterial(mat_name);
}

bool Model::SetCustomMaterialValue(const char* mat_name, const char* key, 
	const char* val)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->SetCustomMaterialValue(mat_name, key, val);
}

result_string Model::GetCustomMaterialValue(const char* mat_name, 
	const char* key)
{
	if (NULL == m_pModelPlayer)
	{
		return result_string();
	}

	char val[256];

	if (!m_pModelPlayer->GetCustomMaterialValue(mat_name, key, val, sizeof(val)))
	{
		return result_string();
	}

	return result_string(val);
}

bool Model::ReloadCustomMaterialTexture(const char* mat_name, const char* key)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->ReloadCustomMaterialTexture( 
		mat_name, key, m_strTexPaths.c_str(), m_bAsyncLoad);
}

bool Model::ReloadCustomMaterialTextures()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->ReloadCustomMaterialTextures( 
		m_strTexPaths.c_str(), m_bAsyncLoad);
}

void Model::SetCurLODLevel(unsigned int lod_level)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	m_pModelPlayer->SetCurLODLevel(lod_level);
}

unsigned int Model::GetCurLODLevel()
{
	if (NULL == m_pModelPlayer)
	{
		return 0xFFFFFFFF;
	}

	return m_pModelPlayer->GetCurLODLevel();
}

unsigned int Model::GetLODLevels()
{
	if (NULL == m_pModelPlayer)
	{
		return 0xFFFFFFFF;
	}

	return m_pModelPlayer->GetLODLevels();
}

bool Model::LOD(unsigned int lod_levels, float dec_ratio)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	//if (m_pModelPlayer->LOD(lod_levels, dec_ratio))
	//{
	//	return true;
	//}

	return false;
}

void Model::OnLinkEvent(bool bLink)
{
	if (bLink)
	{
	}
	else
	{
	}
}

size_t Model::GetMemoryUsage()
{
	size_t size = sizeof(IModelSystem*) + sizeof(IModelSystem*) 
		+ sizeof(m_bAsyncLoad) + sizeof(m_bShowBoundBox) 
		+ sizeof(m_bUseVertexColor) + sizeof(m_bUseLightMap) 
		+ sizeof(m_bVisible) + sizeof(m_bUnderWater) + sizeof(m_bTraceEnable) 
		+ sizeof(m_bCullEnable) + sizeof(m_bWaterReflect) 
		+ sizeof(m_bCastShadow) + sizeof(m_nWriteVelocity) + sizeof(m_nColor) 
		+ sizeof(m_fAlphaValue) + sizeof(m_nExtraInfo) + sizeof(m_bReceiveShadow);

	size += m_strModelFile.length() * sizeof(char);
	size += m_strLightFile.length() * sizeof(char);
	size += m_strMaterialFile.length() * sizeof(char);
	size += m_strTexPaths.length() * sizeof(char);
	size += sizeof(IModelPlayer*) + sizeof(FmMat4);

	return size;
}
