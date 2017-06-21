//--------------------------------------------------------------------
// �ļ���:		model.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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
/// \brief 3Dģ��

/// entity: Model
/// \brief 3Dģ��ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(Model, 0, IVisBase);

/// readonly: string Name
/// \brief ���ƣ������ã�
DECLARE_PROPERTY_GET(const char*, Model, Name, GetName);
/// readonly: bool LightLoaded
/// \brief ģ�͹����Ƿ�ɹ�����
DECLARE_PROPERTY_GET(bool, Model, LightLoaded, GetLightLoaded);
/// readonly: bool HasLightMap
/// \brief �Ƿ���ù���ͼ
DECLARE_PROPERTY_GET(bool, Model, HasLightMap, GetHasLightMap);
/// readonly: bool HasNormalMap
/// \brief �Ƿ���÷�����ͼ
DECLARE_PROPERTY_GET(bool, Model, HasNormalMap, GetHasNormalMap);
/// readonly: bool HasVertexColor
/// \brief �Ƿ���ö���ɫ
DECLARE_PROPERTY_GET(bool, Model, HasVertexColor, GetHasVertexColor);
/// method: int HelperCount
/// \brief ��ø���������
DECLARE_PROPERTY_GET(int, Model, HelperCount, GetHelperCount);

/// property: bool AsyncLoad
/// \brief �Ƿ��첽����
DECLARE_PROPERTY(bool, Model, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool ShowBoundBox
/// \brief �Ƿ���ʾ��Χ��
DECLARE_PROPERTY(bool, Model, ShowBoundBox, GetShowBoundBox, SetShowBoundBox);
/// property: bool UseVertexColor
/// \brief �Ƿ�ʹ�ö������
DECLARE_PROPERTY(bool, Model, UseVertexColor, GetUseVertexColor, 
	SetUseVertexColor);
/// property: bool UseLightMap
/// \brief �Ƿ�ʹ�ù���ͼ
DECLARE_PROPERTY(bool, Model, UseLightMap, GetUseLightMap, SetUseLightMap);
/// property: bool Visible
/// \brief �Ƿ�ɼ�
DECLARE_PROPERTY(bool, Model, Visible, GetVisible, SetVisible);
/// property: bool TraceEanble
/// \brief �Ƿ�����ײ
DECLARE_PROPERTY(bool, Model, TraceEnable, GetTraceEnable, SetTraceEnable);
/// property: bool CullEanble
/// \brief �Ƿ����
DECLARE_PROPERTY(bool, Model, CullEnable, GetCullEnable, SetCullEnable);
/// property: bool WaterReflect
/// \brief �Ƿ���Ҫˮ�淴��
DECLARE_PROPERTY(bool, Model, WaterReflect, GetWaterReflect, SetWaterReflect);
/// property: bool CastShadow
/// \brief ����ҪͶ��Ӱ��
DECLARE_PROPERTY(bool, Model, CastShadow, GetCastShadow, SetCastShadow);
/// \brief ����Ҫ����Ͷ��Ӱ��
DECLARE_PROPERTY(bool, Model, ReceiveShadow, GetReceiveShadow, SetReceiveShadow);
/// property: int WriteVelocity
/// \brief д���ٶ������־��0-��д�룬1-д�룬2-д���ֵ��
DECLARE_PROPERTY(int, Model, WriteVelocity, GetWriteVelocity, 
	SetWriteVelocity);
/// property: string Color
/// \brief ģ����ɫ
DECLARE_PROPERTY(result_string, Model, Color, GetColorString, SetColorString);
/// property: int ExtraInfo
/// \brief ������Ϣ
DECLARE_PROPERTY(int, Model, ExtraInfo, GetExtraInfo, SetExtraInfo);
/// property: string ModelFile
/// \brief ģ���ļ���
DECLARE_PROPERTY(const char*, Model, ModelFile, GetModelFile, SetModelFile);
/// property: string LightFile
/// \brief �����ļ���
DECLARE_PROPERTY(const char*, Model, LightFile, GetLightFile, SetLightFile);
/// property: string MaterialFile
/// \brief �����ļ���
DECLARE_PROPERTY(const char*, Model, MaterialFile, GetMaterialFile, 
	SetMaterialFile);
/// property: string TexPaths
/// \brief ��ͼ·��
DECLARE_PROPERTY(const char*, Model, TexPaths, GetTexPaths, SetTexPaths);
/// property: bool bShow
/// \brief �����Ƿ���ʾ��ģ���ϵĸ�����
DECLARE_PROPERTY(bool, Model, ShowHelper, IsShowHelper, ShowHelper);
///string:int SelectHelper
/// \brief ѡ�и�����
DECLARE_PROPERTY(int, Model, SelectHelper, GetSelectHelper, SetSelectHelper);

/*
/// method: bool LoadLight()
/// \brief ���ع����ļ�
DECLARE_METHOD_0(bool, Model, LoadLight);
/// method: bool LoadMaterial()
/// \brief ���ز����ļ�
DECLARE_METHOD_0(bool, Model, LoadMaterial);

/// method: float TraceHeight(float beg_x, float beg_z, float high_y, float low_y)
/// \brief ���߶�
/// \param beg_x ��ʼX����
/// \param beg_z ��ʼZ����
/// \param high_y ��ߵ�
/// \param low_y ��͵�
/// \return 0��1.0��ʾ����ײ
DECLARE_METHOD_4(float, Model, TraceHeight, float, float, float, float);

/// method: table GetHelperGroupList()
/// \brief ������еĸ�����������б�
DECLARE_METHOD_T(Model, GetHelperGroupList);
/// method: table GetHelperNameList()
/// \brief ������еĸ�����������б�
DECLARE_METHOD_T(Model, GetHelperNameList);
/// method: [x, y, z] GetHelperPosition(string group, string name)
/// \brief ��ø�����ĵ�ǰλ��
/// \param group ����������
/// \param name ����������
DECLARE_METHOD_N(Model, GetHelperPosition);
/// method: [x, y, z] GetHelperAngle(string group, string name)
/// \brief ��ø�����ĵ�ǰ����
/// \param group ����������
/// \param name ����������
DECLARE_METHOD_N(Model, GetHelperAngle);
/// method: GetCameraInfo(string name)
/// \brief ���ָ���㣨�����������Ϣ
/// \param name ����������
DECLARE_METHOD_N(Model, GetCameraInfo);
/// method: int TraceHelperInScreen(int screen_x, int screen_y)
/// \brief ͨ�������Ļ���꣬�����������븨����ʱ�����ײ���
/// \param screen_x ָ����Ļ��x����
/// \param screen_y ָ����Ļ��y����
/// \return �����ײ���˸����㣬��ô����һ�����ڵ���0��ֵ����������������idֵ�����򷵻�-1
DECLARE_METHOD_2(int, Model, TraceHelperInScreen, int, int);
///string:GetHelperName(int helpID)
/// \brief �ɸ�����ID�õ���������
DECLARE_METHOD_1(bool, Model, GetHelperName, int);
///int:GetNodeFromName(const char* helperName)
/// \brief �ɸ����㸨�������õ�ID
DECLARE_METHOD_1(int, Model, GetNodeFromName, const char*);

/// method: bool Play()
/// \brief ����ģ��
DECLARE_METHOD_0(bool, Model, Play);
/// method: bool Stop()
/// \brief ֹͣģ�Ͳ���
DECLARE_METHOD_0(bool, Model, Stop);
/// method: bool Pause()
/// \brief ��ͣģ�Ͳ���
DECLARE_METHOD_0(bool, Model, Pause);
/// method: bool Resume()
/// \brief �ָ�ģ�Ͳ���
DECLARE_METHOD_0(bool, Model, Resume);
/// method: bool SetLoop(bool loop)
/// \brief ����ģ���Ƿ�ѭ������
/// \param loop ѭ����־
//DECLARE_METHOD_1(bool, Model, SetLoop, bool);
/// property: bool Loop
/// \brief �ظ�����
DECLARE_PROPERTY(bool, Model, Loop, GetLoop, SetLoop );
/// method: int GetFrameCount()
/// \brief ���������
DECLARE_METHOD_0(int, Model, GetFrameCount);
/// method: int GetCurrentFrame()
/// \brief��õ�ǰ��
DECLARE_METHOD_0(int, Model, GetCurrentFrame);

/// method: bool GetCurrentFrame(int nFrame)
/// \brief���õ�ǰ��
DECLARE_METHOD_1(bool, Model, SetCurrentFrame, int);

/// method: double GetCurrentTimeFloat()
/// \brief��ȡ��ǰʱ��
DECLARE_METHOD_0(double, Model, GetCurrentTimeFloat);
/// method: bool SetCurrentTimeFloat(double fFrame)
/// \brief���õ�ǰʱ��
DECLARE_METHOD_1(bool, Model, SetCurrentTimeFloat, double);

/// method: float GetDefaultSpeed()
/// \brief ���Ĭ�ϲ����ٶ�
DECLARE_METHOD_0(float, Model, GetDefaultSpeed);
/// method: bool SetSpeed(float speed)
/// \brief ���ò����ٶ�
/// \param speed �ٶȣ������ٶ�Ϊ1.0��
DECLARE_METHOD_1(bool, Model, SetSpeed, float);

/// method: const char* GetHelperIDFromIndex(int nIndex)
/// \brief �ɸ����������õ���������
DECLARE_METHOD_1(const char*, Model, GetHelperIDFromIndex, int);

/// method: int GetAllVertexCount()
/// \brief ��ȡ������
DECLARE_METHOD_0(int, Model, GetAllVertexCount);
/// method: int GetAllTriangleCount()
/// \brief ��ȡ��������
DECLARE_METHOD_0(int, Model, GetAllTriangleCount);

/// method: int GetNodeCount()
/// \brief ���ģ�ͽڵ����
DECLARE_METHOD_0(int, Model, GetNodeCount);
/// method: table GetNodeNameList()
/// \brief ���ģ�͵����нڵ����б�
DECLARE_METHOD_T(Model, GetNodeNameList);
/// method: int GetModelNodeAllowModify(string key)
/// \brief ���ģ�ͽڵ������Ƿ�ɱ༭
/// \param key �ڵ����
DECLARE_METHOD_1(int, Model, GetModelNodeAllowModify, const char*);
/// method: bool SetMaterialValue(string node_name, string key, string val)
/// \brief ����ģ�ͽڵ�����
/// \param node_name ������
/// \param key �ڵ����
/// \param val �ڵ��ֵ
DECLARE_METHOD_3(bool, Model, SetNodeValue, const char*, const char*, 
	const char*);
/// method: string GetMaterialValue(string node_name, string key)
/// \brief ��ȡģ�ͽڵ�����
/// \param node_name �ڵ���
/// \param key �ڵ����
DECLARE_METHOD_2(result_string, Model, GetNodeValue, const char*, const char*);

/// method: int GetMaterialCount()
/// \brief ��ȡ������
DECLARE_METHOD_0(int, Model, GetMaterialCount);
/// method: int GetMaterialTexCount()
/// \brief ��ȡָ���Ĳ��ʵ�������
/// \param mat_index ��������
DECLARE_METHOD_1(int, Model, GetMaterialTexCount, int);
/// method: string GetMaterialTexName(int mat_index, int tex_index)
/// \brief ȡ��ָ�����ʵ�ָ��������ͼ��
/// \param mat_index ��������
/// \param tex_index ��ͼ����
DECLARE_METHOD_2(const char*, Model, GetMaterialTexName, int, int);
/// method: string GetMaterialAmbient(int mat_index)
/// \brief ȡ��ָ�����ʵ�Ambient
/// \param mat_index ��������
DECLARE_METHOD_1(result_string, Model, GetMaterialAmbient, int);
/// method: string GetMaterialDiffuse(int mat_index)
/// \brief ȡ��ָ�����ʵ�Diffuse
/// \param mat_index ��������
DECLARE_METHOD_1(result_string, Model, GetMaterialDiffuse, int);
/// method: string GetVertexDiffuse(size_t vb_index, size_t vertex_index)
/// \brief ȡ��ָ�����㻺���ָ���������ɫ����
/// \param vb_index VB����
/// \param vertex_index ��������
DECLARE_METHOD_2(result_string, Model, GetVertexDiffuse, size_t, size_t);

/// method: table GetMaterialNameList()
/// \brief ���ģ�͵����в������б�
DECLARE_METHOD_T(Model, GetMaterialNameList);
/// method: bool SetMaterialValue(string mat_name, string key, string val)
/// \brief ����ģ�Ͳ�������
/// \param mat_name ������
/// \param key ���ʼ���
/// \param val ���ʼ�ֵ
DECLARE_METHOD_3(bool, Model, SetMaterialValue, const char*, const char*,
	const char*);
/// method: string GetMaterialValue(string mat_name, string key)
/// \brief ��ȡģ�Ͳ�������
/// \param mat_name ������
/// \param key ���ʼ���
DECLARE_METHOD_2(result_string, Model, GetMaterialValue, const char*, 
	const char*);
/// method: bool ReloadMaterialTextures()
/// \brief ���¼���ģ�Ͳ��ʵ�������ͼ
DECLARE_METHOD_0(bool, Model, ReloadMaterialTextures);

/// method: bool FindCustomMaterial(string mat_name)
/// \brief �����Ƿ�����Զ����������
/// \param mat_name ������
DECLARE_METHOD_1(bool, Model, FindCustomMaterial, const char*);

/// method: bool SetCustomMaterialValue(string mat_name, string key, string val)
/// \brief �����Զ����������
/// \param mat_name ������
/// \param key ���ʼ���
/// \param val ���ʼ�ֵ
DECLARE_METHOD_3(bool, Model, SetCustomMaterialValue, const char*, 
	const char*, const char*);
/// method: string GetCustomMaterialValue(string mat_name, string key)
/// \brief ��ȡ�Զ����������
/// \param mat_name ������
/// \param key ���ʼ���
DECLARE_METHOD_2(result_string, Model, GetCustomMaterialValue, const char*, 
	const char*);
/// method: bool ReloadCustomMaterialTexture()
/// \brief ���¼����Զ�����ʵ�ָ����ͼ
DECLARE_METHOD_2(bool, Model, ReloadCustomMaterialTexture, const char*, 
	const char*);
/// method: bool ReloadCustomMaterialTextures()
/// \brief ���¼����Զ�����ʵ�������ͼ
DECLARE_METHOD_0(bool, Model, ReloadCustomMaterialTextures);

/// method: bool AfterLoaded()
/// \brief ������ɺ�
DECLARE_METHOD_0(bool, Model, AfterLoaded);

/// property: unsigned int CurLODLevel
/// \brief Current LOD Level
DECLARE_PROPERTY(unsigned int, Model, CurLODLevel, GetCurLODLevel, 
	SetCurLODLevel);
/// readonly: unsigned int LODLevels
/// \brief ģ���ܹ���LOD����
DECLARE_PROPERTY_GET(unsigned int, Model, LODLevels, GetLODLevels);

/// Warning:This API just for LOD tools use!
/// method: bool LOD()
/// \brief ���ع����ļ�
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
	
		// ��ȡ��ǰ��ͼģʽ
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
	// ����ģ����ɫ
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

	// ����ģ����ɫ
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

	// �ڵ��뵭��ʱҲ��Ⱦ��Ӱ�����ڼ��ٻ���ͻ��
	//if (((m_nColor >> 24) & 0xFF) < 255)
	//{
	//	return;
	//}

	IRenderContext* pContext = m_pRender->GetContext();

	// ����ģ����ɫ
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
	// �����ļ���չ����
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

	// ���ع���
	if (!m_strLightFile.empty())
	{
		m_pModelPlayer->LoadLight(m_strLightFile.c_str(), m_bAsyncLoad);
	}

	// ���ز����ļ�
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

	// ������Χ�����
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
	
	// ������ײ���Ķ���ID
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

	// ������Χ�����
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

	// ������Χ�����
	//float r = GetRadius();
	//float square = D3DXVec3LengthSq(&(center - GetCenter()));

	//if (square > (r + radius) * (r + radius))
	//{
	//	return false;
	//}

	return m_pModelPlayer->TraceSphere(center, radius);
}

// ��������ײ
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

	// ������Χ�����
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
		// ֻʹ�����ֽ��в���
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
		// ֻʹ�����ֽ��в���
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
