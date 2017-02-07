//--------------------------------------------------------------------
// 文件名:		skin.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月10日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "skin.h"
#include "../visual/i_model_player.h"
#include "../visual/i_action_player.h"
#include "../visual/i_context.h"
#include "../visual/i_model_system.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"

/// \file skin.cpp
/// \brief 骨骼动画模型

/// entity: Skin
/// \brief 骨骼动画模型实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Skin, 0, IVisBase);

/// readonly: string Name
/// \brief 名称（测试用）
DECLARE_PROPERTY_GET(const char*, Skin, Name, GetName);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, Skin, Visible, GetVisible, SetVisible);
/// property: bool TraceEanble
/// \brief 是否检测碰撞
DECLARE_PROPERTY(bool, Skin, TraceEnable, GetTraceEnable, SetTraceEnable);
/// property: bool CullEanble
/// \brief 是否剪裁
DECLARE_PROPERTY(bool, Skin, CullEnable, GetCullEnable, SetCullEnable);
/// property: bool AsyncLoad
/// \brief 是否异步加载
DECLARE_PROPERTY(bool, Skin, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool ShowBoundBox
/// \brief 是否显示包围盒
DECLARE_PROPERTY(bool, Skin, ShowBoundBox, GetShowBoundBox, SetShowBoundBox);
/// property: string ModelFile
/// \brief 模型文件名
DECLARE_PROPERTY(const char*, Skin, ModelFile, GetModelFile, SetModelFile);
/// property: string MaterialFile
/// \brief 材质文件名
DECLARE_PROPERTY(const char*, Skin, MaterialFile, GetMaterialFile, 
	SetMaterialFile);
/// property: string TexPaths
/// \brief 贴图路径
DECLARE_PROPERTY(const char*, Skin, TexPaths, GetTexPaths, SetTexPaths);
/// property: string Color
/// \brief 颜色
DECLARE_PROPERTY(result_string, Skin, Color, GetColorString, SetColorString);

/*
/// method: table GetRootNodeList()
/// \brief 获得根节点名列表
DECLARE_METHOD_T(Skin, GetRootNodeList);

/// method: table GetHelperNodeList()
/// \brief 获得辅助节点名列表
DECLARE_METHOD_T(Skin, GetHelperNodeList);
/// method: [x, y, z] GetHelperPosition(string group, string name)
/// \brief 获得辅助点的当前位置
/// \param group 辅助点组名
/// \param name 辅助点名字
DECLARE_METHOD_N(Skin, GetHelperPosition);
/// method: [x, y, z] GetHelperAngle(string group, string name)
/// \brief 获得辅助点的当前方向
/// \param group 辅助点组名
/// \param name 辅助点名字
DECLARE_METHOD_N(Skin, GetHelperAngle);

/// method: GetCameraInfo(string name)
/// \brief 获得指定摄像机类型节点的信息
/// \param name 摄像机类型节点名字
DECLARE_METHOD_N(Skin, GetCameraInfo);

/// method: int GetAllVertexCount()
/// \brief 获取顶点数
DECLARE_METHOD_0(int, Skin, GetAllVertexCount);
/// method: int GetAllTriangleCount()
/// \brief 获取三角面数
DECLARE_METHOD_0(int, Skin, GetAllTriangleCount);

/// method: int GetNodeCount()
/// \brief 获得模型节点个数
DECLARE_METHOD_0(int, Skin, GetNodeCount);
/// method: table GetNodeNameList()
/// \brief 获得模型的所有节点名列表
DECLARE_METHOD_T(Skin, GetNodeNameList);
/// method: int GetModelNodeAllowModify(string key)
/// \brief 获得模型节点数据是否可编辑
/// \param key 节点键名
DECLARE_METHOD_1(int, Skin, GetModelNodeAllowModify, const char*);
/// method: bool SetMaterialValue(string node_name, string key, string val)
/// \brief 设置模型节点数据
/// \param node_name 材质名
/// \param key 节点键名
/// \param val 节点键值
DECLARE_METHOD_3(bool, Skin, SetNodeValue, const char*, const char*, 
	const char*);
/// method: string GetMaterialValue(string node_name, string key)
/// \brief 读取模型节点数据
/// \param node_name 节点名
/// \param key 节点键名
DECLARE_METHOD_2(result_string, Skin, GetNodeValue, const char*, const char*);

/// method: bool LoadMaterial()
/// \brief 加载材质文件
DECLARE_METHOD_0(bool, Skin, LoadMaterial);

/// method: int GetMaterialCount()
/// \brief 获取材质数
DECLARE_METHOD_0(int, Skin, GetMaterialCount);
/// method: int GetMaterialTexCount()
/// \brief 获取指定的材质的纹理数
/// \param mat_index 材质索引
DECLARE_METHOD_1(int, Skin, GetMaterialTexCount, int);
/// method: string GetMaterialTexName(int mat_index, int tex_index)
/// \brief 取得指定材质的指定纹理贴图名
/// \param mat_index 材质索引
/// \param tex_index 贴图索引
DECLARE_METHOD_2(const char*, Skin, GetMaterialTexName, int, int);

/// method: table GetMaterialNameList()
/// \brief 获得模型的所有材质名列表
DECLARE_METHOD_T(Skin, GetMaterialNameList);
/// method: bool SetMaterialValue(string mat_name, string key, string val)
/// \brief 设置模型材质数据
/// \param mat_name 材质名
/// \param key 材质键名
/// \param val 材质键值
DECLARE_METHOD_3(bool, Skin, SetMaterialValue, const char*, const char*,
	const char*);
/// method: string GetMaterialValue(string mat_name, string key)
/// \brief 读取模型材质数据
/// \param mat_name 材质名
/// \param key 材质键名
DECLARE_METHOD_2(result_string, Skin, GetMaterialValue, const char*, 
	const char*);
/// method: bool ReloadMaterialTextures()
/// \brief 重新加载模型材质的所有贴图
DECLARE_METHOD_0(bool, Skin, ReloadMaterialTextures);

/// method: bool FindCustomMaterial(string mat_name)
/// \brief 查找是否存在自定义材质数据
/// \param mat_name 材质名
DECLARE_METHOD_1(bool, Skin, FindCustomMaterial, const char*);

/// method: bool SetCustomMaterialValue(string mat_name, string key, string val)
/// \brief 设置自定义材质数据
/// \param mat_name 材质名
/// \param key 材质键名
/// \param val 材质键值
DECLARE_METHOD_3(bool, Skin, SetCustomMaterialValue, const char*, 
	const char*, const char*);
/// method: string GetCustomMaterialValue(string mat_name, string key)
/// \brief 读取自定义材质数据
/// \param mat_name 材质名
/// \param key 材质键名
DECLARE_METHOD_2(result_string, Skin, GetCustomMaterialValue, const char*, 
	const char*);
/// method: bool ReloadCustomMaterialTexture()
/// \brief 重新加载自定义材质的指定贴图
DECLARE_METHOD_2(bool, Skin, ReloadCustomMaterialTexture, const char*, 
const char*);
/// method: bool ReloadCustomMaterialTextures()
/// \brief 重新加载自定义材质的所有贴图
DECLARE_METHOD_0(bool, Skin, ReloadCustomMaterialTextures);

/// method: bool ResetSkinMap()
/// \brief 重新设置蒙皮模型的骨骼map
DECLARE_METHOD_0(bool, Skin, ResetSkinMap);*/

// Skin

Skin::Skin()
{
	m_pModelSystem = NULL;
	m_bVisible = true;
	m_bTraceEnable = true;
	m_bCullEnable = false;
	m_bAsyncLoad = false;
	m_bShowBoundBox = false;
	m_nColor = 0xFFFFFFFF;
	m_bUnderWater = false;
	m_pModelPlayer = NULL;
	m_pActionPlayer = NULL;
	m_vBoundMax = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoundMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_fRadius = 0.0F;
	m_bUpdatedBoundingBox = false;
	FmMatrixIdentity(&m_mtxWorld);
}

Skin::~Skin()
{
	SAFE_RELEASE(m_pModelPlayer);
}

bool Skin::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
		return false;

	m_pModelSystem = (IModelSystem*)GetCore()->GetInterface("ModelSystem");

	if (NULL == m_pModelSystem)
	{
		return false;
	}

	return true;
}

bool Skin::Shut()
{
	SAFE_RELEASE(m_pModelPlayer);
	SAFE_RELEASE(m_pModelSystem);

	return true;
}

bool Skin::Load()
{
	SAFE_RELEASE(m_pModelPlayer);
	m_pModelPlayer = m_pModelSystem->CreateModelPlayer(m_strModelFile.c_str(),
		GetCore()->GetTotalSeconds(), m_strTexPaths.c_str(), m_bAsyncLoad);

	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	m_pModelPlayer->SetContext(m_pContext);
	m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	m_pModelPlayer->SetActionPlayer(m_pActionPlayer);

	if (!m_strMaterialFile.empty())
	{
		m_pModelPlayer->LoadMaterial(m_strMaterialFile.c_str(), m_bAsyncLoad);
	}

	return true;
}

void Skin::Update(float seconds)
{
	if (m_pModelPlayer)
	{
		m_pModelPlayer->Update(seconds);
	}
	
	m_bUpdatedBoundingBox = false;
}

void Skin::Realize()
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

void Skin::RealizeNormal()
{
	IRenderContext* pContext = m_pRender->GetContext();

	m_pModelPlayer->SetColor(m_nColor);
	m_pModelPlayer->SetUnderWater(m_bUnderWater);
	m_pModelPlayer->SetShowBoundBox(m_bShowBoundBox);
	float offset_seconds = (float)GetCore()->GetFrameSeconds();
	m_pModelPlayer->Draw(pContext->GetClipPlanes(), pContext->GetClipPlaneAmount());
}

void Skin::RealizeReflect()
{
	IRenderContext* pContext = m_pRender->GetContext();

	m_pModelPlayer->SetColor(m_nColor);
	m_pModelPlayer->SetUnderWater(m_bUnderWater);
	m_pModelPlayer->SetShowBoundBox(m_bShowBoundBox);
	float offset_seconds = (float)GetCore()->GetFrameSeconds();
	m_pModelPlayer->DrawReflect(pContext->GetClipPlanes(), pContext->GetClipPlaneAmount());
}

void Skin::RealizeShadowMap()
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

	m_pModelPlayer->SetColor(m_nColor);
	//m_pModelPlayer->SetUnderWater(m_bUnderWater);
	//m_pModelPlayer->SetShowBoundBox(m_bShowBoundBox);
	m_pModelPlayer->DrawShadowMap(pContext->GetClipPlanes(), pContext->GetClipPlaneAmount());
}

bool Skin::GetLoaded()
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

bool Skin::GetLoadFinish()
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

void Skin::SetWorldMatrix(const FmMat4& mat)
{
	m_mtxWorld = mat;

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(mat);
	}
	
	m_bUpdatedBoundingBox = false;
}

void Skin::GetWorldMatrix(FmMat4& mat)
{
	mat = m_mtxWorld;
}

bool Skin::SetPosition(float x, float y, float z)
{
	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

FmVec3 Skin::GetPosition() const
{
	return FmVec3(m_mtxWorld._41, m_mtxWorld._42, 
		m_mtxWorld._43);
}

bool Skin::SetAngle(float x, float y, float z)
{
	VisUtil_SetMatrixAngle(&m_mtxWorld, x, y, z);

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

FmVec3 Skin::GetAngle() const
{
	FmVec3 angle;

	VisUtil_GetMatrixAngle(&angle, &m_mtxWorld);

	return angle;
}

bool Skin::SetScale(float x, float y, float z)
{
	VisUtil_SetMatrixScale(&m_mtxWorld, x, y, z);
	
	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetWorldMatrix(m_mtxWorld);
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

FmVec3 Skin::GetScale() const
{
	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, &m_mtxWorld);

	return scale;
}

FmVec3 Skin::GetCenter()
{
	if (NULL == m_pModelPlayer)
	{
		return GetPosition();
	}

	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return (m_vBoundMax + m_vBoundMin) * 0.5F;
}

FmVec3 Skin::GetBoxSize()
{
	if (NULL == m_pModelPlayer)
	{
		return FmVec3(0.0F, 0.0F, 0.0F);
	}

	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return m_vBoundMax - m_vBoundMin;
}

float Skin::GetRadius()
{
	if (NULL == m_pModelPlayer)
	{
		return 0.0F;
	}

	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return m_fRadius;
}

void Skin::SetColor(unsigned int value)
{
	m_nColor = value;
}

unsigned int Skin::GetColor() const
{
	return m_nColor;
}

void Skin::SetColorString(const char* value)
{
	Assert(value != NULL);

	m_nColor = VisUtil_ColorToInt(value);
}

result_string Skin::GetColorString() const
{
	return VisUtil_IntToColor(m_nColor);
}

void Skin::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Skin::GetVisible() const
{
	return m_bVisible;
}

void Skin::SetUnderWater(bool value)
{
	m_bUnderWater = value;
}

bool Skin::GetUnderWater() const
{
	return m_bUnderWater;
}

void Skin::SetTraceEnable(bool value)
{
	m_bTraceEnable = value;
}

bool Skin::GetTraceEnable() const
{
	return m_bTraceEnable;
}

void Skin::SetCullEnable(bool value)
{
}

bool Skin::GetCullEnable() const
{
	return false;
}

void Skin::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool Skin::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

bool Skin::ExistMainModel()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->ExistMainModel();
}

void Skin::SetTraceMainModel(bool value)
{
	if (NULL == m_pModelPlayer)
	{
		return;
	}

	m_pModelPlayer->SetTraceMainModel(value);
}

bool Skin::GetTraceMainModel()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->GetTraceMainModel();
}

void Skin::SetMaterialFile(const char* value)
{
	Assert(value != NULL);

	m_strMaterialFile = value;
}

const char* Skin::GetMaterialFile() const
{
	return m_strMaterialFile.c_str();
}

bool Skin::LoadMaterial()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->LoadMaterial(m_strMaterialFile.c_str(), m_bAsyncLoad);
}

void Skin::SetActionPlayer(IActionPlayer* value)
{
	m_pActionPlayer = value;

	if (m_pModelPlayer)
	{
		m_pModelPlayer->SetActionPlayer(m_pActionPlayer);
	}
}

IActionPlayer* Skin::GetActionPlayer() const
{
	return m_pActionPlayer;
}

bool Skin::AddModel(IActionPlayer* pActionPlayer, const char* pszModelFile)
{
	Assert(pszModelFile != NULL);

	m_pActionPlayer = pActionPlayer;
	m_strModelFile = pszModelFile;

	return Load();
}

bool Skin::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
 	if (!m_bVisible || !m_bTraceEnable)
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

	int old_hit_count = result.nHitCount;
	
	if (m_pModelPlayer->TraceDetail(src, dst, result))
	{
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

	return false;
}

bool Skin::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	if (!m_bVisible || !m_bTraceEnable)
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

	return m_pModelPlayer->TraceHitted(src, dst);
}

bool Skin::TraceSphere(const FmVec3& center, float radius)
{
	if (!m_bVisible || !m_bTraceEnable)
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

	return m_pModelPlayer->TraceSphere(center, radius);
}

// 三角形碰撞
bool Skin::TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2)
{
    if (!m_bVisible || !m_bTraceEnable)
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

bool Skin::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	return false;
}

void Skin::SetModelFile(const char* pszModelFile)
{
	Assert(pszModelFile != NULL);

	m_strModelFile = pszModelFile;
}

const char* Skin::GetModelFile()
{
	return m_strModelFile.c_str();
}

void Skin::SetTexPaths(const char* value)
{
	Assert(value != NULL);

	m_strTexPaths = value;
}

const char* Skin::GetTexPaths() const
{
	return m_strTexPaths.c_str();
}

void Skin::GetRootNodeList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	unsigned int count = m_pModelPlayer->GetRootNodeCount();

	for (unsigned int i = 0; i < count; i++)
	{
		void* pNode = m_pModelPlayer->GetRootNodeFromID(i);

		if (NULL == pNode)
		{
			continue;
		}

		const char* pNodeName = m_pModelPlayer->GetNodeName(pNode);

		if (pNodeName)
		{
			result.AddString(pNodeName);
		}
	}
}

void Skin::GetHelperNodeList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (NULL == m_pModelPlayer)
	{
		return;
	}

	unsigned int nCount = m_pModelPlayer->GetNodeCount();

	for (unsigned int i = 0; i < nCount; i++)
	{
		void* pNode = m_pModelPlayer->GetNodeFromID(i);

		//if (!m_pModelPlayer->IsAttachedNode(pNode))
		//{
		//	continue;
		//}

		const char* pNodeName = m_pModelPlayer->GetNodeName(pNode);

		if (pNodeName)
		{
			result.AddString(pNodeName);
		}
	}
}

void Skin::GetHelperPosition(const IVarList& args, IVarList& result)
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

	m_pModelPlayer->GetLabelMatrix(index, local_matrix);

	FmVec3 pos;

	pos.x = local_matrix.m[3][0];
	pos.y = local_matrix.m[3][1];
	pos.z = local_matrix.m[3][2];

	FmVec3TransformCoord(&pos, &pos, &m_mtxWorld);

	result.AddFloat(pos.x);
	result.AddFloat(pos.y);
	result.AddFloat(pos.z);
}

void Skin::GetHelperAngle(const IVarList& args, IVarList& result)
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
	FmMat4Multiply(&mat, &local_matrix, &m_mtxWorld);
	VisUtil_GetMatrixAngle(&angle, &mat);

	result.AddFloat(angle.x);
	result.AddFloat(angle.y);
	result.AddFloat(angle.z);
}

int Skin::GetNodeFromName(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pModelPlayer)
	{
		return -1;
	}

	void* pNode = m_pModelPlayer->GetNodeFromName(pszName);

	if (NULL == pNode)
	{
		return -1;
	}

	return m_pModelPlayer->GetNodeID(pNode);
}

void Skin::GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM)
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

bool Skin::GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM)
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

void Skin::GetCameraInfo(const IVarList& args, IVarList& result)
{
	Assert(VTYPE_STRING == args.GetType(0));

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

void Skin::UpdateBoundingBox()
{
	// 修正角色模型为加载完成就updateboundingbox，导致出错的问题
	if (!GetLoadFinish())
	{
		m_vBoundMin = GetPosition();
		m_vBoundMax = m_vBoundMin;
		m_fRadius = 0.0F;

		return;
	}

	//if ((NULL == m_pModelPlayer) || (NULL == m_pActionPlayer))
	if (NULL == m_pModelPlayer)
	{
		m_vBoundMin = GetPosition();
		m_vBoundMax = m_vBoundMin;
		m_fRadius = 0.0F;
		return;
	}

	// 获取指定模型的当前包围盒，这个包围盒是指定模型的世界空间轴对齐包围盒
	FmVec3 center = m_pModelPlayer->GetCenter();
	float radius = m_pModelPlayer->GetRadius();

	m_vBoundMin.x = center.x - radius;
	m_vBoundMin.y = center.y - radius;
	m_vBoundMin.z = center.z - radius;
	m_vBoundMax.x = center.x + radius;
	m_vBoundMax.y = center.y + radius;
	m_vBoundMax.z = center.z + radius;
	m_fRadius = radius;
	m_bUpdatedBoundingBox = true;
//	FmVec3 bbMin;
//	FmVec3 bbMax;
//
//	m_pModelPlayer->GetCurrentBoundingBox(&bbMin, &bbMax);
//
//	FmVec3 delta = (bbMax - bbMin) * 0.5F;
//	FmVec3 center = (bbMax + bbMin) * 0.5F;
//	float fMaxRadius = max(max(delta.x, delta.y), delta.z);
//	FmVec3 vRadius = FmVec3(fMaxRadius, fMaxRadius, fMaxRadius);
//
//	m_bbMin = center - vRadius;
//	m_bbMax = center + vRadius;
//	m_fRadius = VisUtil_Vec3Length(&vRadius) * 0.5F;
//	m_bUpdatedBoundingBox = true;
}

size_t Skin::GetObjectCount() const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetObjectCount();
}
size_t Skin::GetObjectVertexCount(size_t index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetObjectVertexCount(index);
}

size_t Skin::GetObjectTriangleCount(size_t index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetObjectTriangleCount(index);
}

int Skin::GetAllVertexCount() const
{
	size_t ver_count = 0;
	size_t obj_count = GetObjectCount();

	for (size_t i = 0; i < obj_count; ++i)
	{
		ver_count += GetObjectVertexCount(i);
	}

	return (int)ver_count;
}

int Skin::GetAllTriangleCount()
{
	size_t tri_count = 0;
	size_t obj_count = GetObjectCount();

	for (size_t i = 0; i < obj_count; ++i)
	{
		tri_count += GetObjectTriangleCount(i);
	}

	return (int)tri_count;
}

int Skin::GetNodeCount()
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return (int)m_pModelPlayer->GetNodeCount();
}

void Skin::GetNodeNameList(const IVarList& args, IVarList& result)
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

bool Skin::GetModelNodeAllowModify(const char* key)
{
	return m_pModelPlayer->GetModelNodeAllowModify(key);
}

bool Skin::SetNodeValue(const char* node_name, const char* key, 
	const char* val)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->SetModelNodeValue(node_name, key, val);
}

result_string Skin::GetNodeValue(const char* node_name, const char* key)
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

int Skin::GetMaterialCount() const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return (int)m_pModelPlayer->GetMaterialCount();
}

int Skin::GetMaterialTexCount(int index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return (int)m_pModelPlayer->GetMaterialTextureCount(index);
}

const char* Skin::GetMaterialTexName(int mat_index, int tex_index) const
{
	if (NULL == m_pModelPlayer)
	{
		return 0;
	}

	return m_pModelPlayer->GetMaterialTextureName(mat_index, tex_index);
}

void Skin::GetMaterialNameList(const IVarList& args, IVarList& result)
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

bool Skin::SetMaterialValue(const char* mat_name, const char* key, 
	const char* val)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->SetModelMaterialValue(mat_name, key, val);
}

result_string Skin::GetMaterialValue(const char* mat_name, const char* key)
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

bool Skin::ReloadMaterialTextures()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->ReloadMaterialTextures( 
		m_strTexPaths.c_str(), m_bAsyncLoad);
}

bool Skin::FindCustomMaterial(const char* mat_name)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->FindCustomMaterial(mat_name);
}

bool Skin::SetCustomMaterialValue(const char* mat_name, const char* key, 
	const char* val)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->SetCustomMaterialValue(mat_name, key, val);
}

result_string Skin::GetCustomMaterialValue(const char* mat_name, 
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

bool Skin::ReloadCustomMaterialTexture(const char* mat_name, const char* key)
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->ReloadCustomMaterialTexture( 
		m_strTexPaths.c_str(), mat_name, key, m_bAsyncLoad);
}

bool Skin::ReloadCustomMaterialTextures()
{
	if (NULL == m_pModelPlayer)
	{
		return false;
	}

	return m_pModelPlayer->ReloadCustomMaterialTextures( 
		m_strTexPaths.c_str(), m_bAsyncLoad);
}

bool Skin::ResetSkinMap()
{
	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsReady())
		{
			return false;
		}
	}

	m_pModelPlayer->ReleaseSkeletonMappingTable();
	m_pModelPlayer->InitSkeletonMappingTable();

	return true;
}
