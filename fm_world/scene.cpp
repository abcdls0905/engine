//--------------------------------------------------------------------
// �ļ���:		scene.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��28��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "scene.h"
#include "weather.h"
#include "camera.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

#include "pssm.h"

//#include "../fm_render/render_utils.h"

/// \file scene.cpp
/// \brief 3D����

/// entity: Scene
/// \brief 3D����ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(Scene, 0, IEntity);

/// readonly: object Weather
/// \brief ������������
DECLARE_PROPERTY_GET(PERSISTID, Scene, Weather, GetWeatherID);
/// readonly: object Camera
/// \brief ���������
DECLARE_PROPERTY_GET(PERSISTID, Scene, Camera, GetCameraID);

/// property: float FarClipDistance
/// \brief Զ������ľ���
DECLARE_PROPERTY(float, Scene, FarClipDistance, GetFarClipDistance, 
	SetFarClipDistance);
/// property: int BlendDownLevel
/// \brief ��͸�����彵����ʾ
DECLARE_PROPERTY(int, Scene, BlendDownLevel, GetBlendDownLevel, 
	SetBlendDownLevel);
/// property: bool EnableRealizeRefraction
/// \brief �Ƿ�Ҫ��Ⱦ����ͼ
DECLARE_PROPERTY(bool, Scene, EnableRealizeRefraction, 
	GetEnableRealizeRefraction, SetEnableRealizeRefraction);
/// property: bool EnableRealizeTempRT
/// \brief �Ƿ�Ҫ������ʱ��Ⱦ������
DECLARE_PROPERTY(bool, Scene, EnableRealizeTempRT, GetEnableRealizeTempColorRT, 
	SetEnableRealizeTempColorRT);

/// property: bool EnableRealizeTempRT
/// \brief �Ƿ�Ҫ������ʱ��Ⱦ������
DECLARE_PROPERTY(bool, Scene, EnableRealizeTempDepthRT, GetEnableRealizeTempDepthRT, 
	SetEnableRealizeTempDepthRT);

/// property: bool EnableRealizeTempRT
/// \brief �Ƿ�Ҫ����������ʱ��Ⱦ������
DECLARE_PROPERTY(bool, Scene, EnableRealizeCopyTempRT, GetEnableRealizeCopyTempColorRT, 
	SetEnableRealizeCopyTempColorRT);

/// property: bool EnableRealizeTempRT
/// \brief �Ƿ�Ҫ����������ʱ��Ⱦ������
DECLARE_PROPERTY(bool, Scene, EnableRealizeCopyTempDepthRT, GetEnableRealizeCopyTempDepthRT, 
	SetEnableRealizeCopyTempDepthRT);

/// property: bool SkinSphereAmbient
/// \brief ��������ģ���Ƿ������򻷾���
DECLARE_PROPERTY(bool, Scene, SkinSphereAmbient, GetSkinSphereAmbient, 
	SetSkinSphereAmbient);
/// property: bool ModelSphereAmbient
/// \brief ��ͨģ���Ƿ������򻷾���
DECLARE_PROPERTY(bool, Scene, ModelSphereAmbient, GetModelSphereAmbient, 
	SetModelSphereAmbient);
/// property: string GlobalEnvMap
/// \brief ȫ�ֻ�����ͼ
DECLARE_PROPERTY(const char*, Scene, GlobalEnvMap, GetGlobalEnvMap, 
	SetGlobalEnvMap);
/// property: float ReflectFactor
/// \brief ��������ĳ˷�ϵ��
DECLARE_PROPERTY(float, Scene, ReflectFactor, GetReflectFactor, 
	SetReflectFactor);

/// property: string RoleDiffuseFactor
/// \brief ������չ�Դ��ɫ��������
DECLARE_PROPERTY(float, Scene, RoleDiffuseFactor, GetRoleDiffuseFactor, 
	SetRoleDiffuseFactor);
/// property: string RoleAmbientFactor
/// \brief ������ջ������������
DECLARE_PROPERTY(float, Scene, RoleAmbientFactor, GetRoleAmbientFactor, 
	SetRoleAmbientFactor);

/// property: bool EnableRoleFallOff
/// \brief ָ����Ⱦ����ʱ�Ƿ�ʹ�ø��ӵĵ��Դ����
DECLARE_PROPERTY(bool, Scene, EnableRoleFallOff, GetEnableRoleFallOff, 
	SetEnableRoleFallOff);
/// property: string RoleFallOffColor
/// \brief ָ�������Ե�������ɫ
DECLARE_PROPERTY(result_string, Scene, RoleFallOffColor, 
	GetRoleFallOffColorString, SetRoleFallOffColorString);
/// property: float RoleFallOffInten
/// \brief ָ����Ⱦ�����Ե�����ɫ��ǿ��
DECLARE_PROPERTY(float, Scene, RoleFallOffInten, GetRoleFallOffInten, 
	SetRoleFallOffInten);
/// property: float RoleFallOffPower
/// \brief ָ����Ⱦ�����Ե�������ɫ����ָ��
DECLARE_PROPERTY(float, Scene, RoleFallOffPower, GetRoleFallOffPower, 
	SetRoleFallOffPower);

/// property: bool EnableEarlyZ
/// \brief �Ƿ���early-z�Ż�
DECLARE_PROPERTY(bool, Scene, EnableEarlyZ, GetEnableEarlyZ, SetEnableEarlyZ);

/// property: bool EnableDynamicShadow
/// \brief �Ƿ�Ҫ������̬��ӰЧ��
DECLARE_PROPERTY(bool, Scene, EnableDynamicShadow, GetEnableDynamicShadow, SetEnableDynamicShadow);

// Scene

Scene::Scene()
{
	m_pRender = NULL;
	m_pContext = NULL;
	m_pSceneView = NULL;
	m_pWeather = NULL;
	m_pCamera = NULL;
	m_pObjects = NULL;
	m_pPSSM = NULL;
}

Scene::~Scene()
{
	object_t* p = m_pObjects;
	object_t* t;

	while (p)
	{
		t = p;
		p = p->pNext;
		CORE_FREE(t, sizeof(object_t));
	}

	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pSceneView);
}

bool Scene::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}

	// ������Ⱦ������
	m_pContext = m_pRender->CreateContext();
	// ����������ͼ
	m_pSceneView = m_pRender->CreateSceneView(ISceneView::TYPE_NORMAL,"MainSceneView");

	// ���û���������
	m_pRender->SetContext(m_pContext);
	m_pRender->SetSceneView(m_pSceneView);

	return true;
}

bool Scene::Shut()
{
	object_t* p = m_pObjects;
	object_t* t;

	while (p)
	{
		t = p;
		p = p->pNext;

		IEntity* pEntity = GetCore()->GetEntity(t->ObjectId);

		SAFE_RELEASE(pEntity);
		CORE_FREE(t, sizeof(object_t));
	}

	m_pObjects = NULL;
	SAFE_RELEASE(m_pWeather);
	SAFE_RELEASE(m_pCamera);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pSceneView);
	SAFE_RELEASE(m_pRender);

	if (m_pPSSM)
	{
		DeletePSSM();
	}

	return true;
}

void Scene::Update(float seconds)
{
	// ���û���������
	m_pRender->SetContext(m_pContext);
	m_pRender->SetSceneView(m_pSceneView);

	bool bDynamicShadow = m_pContext->GetEnableDynamicShadow();

	if (bDynamicShadow && m_pPSSM)
	{
		m_pPSSM->SetRenderObjets(m_pObjects);
	}

	object_t* prev = NULL;
	object_t* p = m_pObjects;

	while (p)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(p->ObjectId);

		if (pVisBase != NULL)
		{
			pVisBase->Update(seconds);
			prev = p;
			p = p->pNext;
		}
		else
		{
			object_t* t = p;

			p = p->pNext;
			CORE_FREE(t, sizeof(object_t));

			if (prev != NULL)
			{
				prev->pNext = p;
			}
			else
			{
				m_pObjects = p;
			}
		}
	}

	// �ָ�����������
	m_pRender->SetContext(NULL);
	m_pRender->SetSceneView(NULL);
}

// Ӧ����Ч����
void apply_fog_param(IRenderContext* pContext)
{
	// �Ƿ���ˮ��
	if (pContext->GetInt(IRenderContext::I_UNDER_WATER) != 0) 
	{
		float fog_start = pContext->GetFloat(
			IRenderContext::F_FOG_START_UNDERWATER);
		pContext->SetFloat(IRenderContext::F_FOG_START, fog_start);

		// ����ˮ����Ч
		unsigned int fog_color = pContext->GetColor(
			IRenderContext::C_FOG_COLOR_UNDERWATER);
		pContext->SetColor(IRenderContext::C_FOG_COLOR, fog_color);

		float fog_end = pContext->GetFloat(
			IRenderContext::F_FOG_END_UNDERWATER);
		pContext->SetFloat(IRenderContext::F_FOG_END, fog_end);

		float fog_density = pContext->GetFloat(
			IRenderContext::F_FOG_DENSITY_UNDERWATER);
		pContext->SetFloat(IRenderContext::F_FOG_DENSITY, fog_density);
	}
	else
	{

		float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START_GROUND);
		pContext->SetFloat(IRenderContext::F_FOG_START, fog_start);

		// ���õ�����Ч
		unsigned int fog_color = pContext->GetColor(IRenderContext::C_FOG_COLOR_GROUND);
		pContext->SetColor(IRenderContext::C_FOG_COLOR, fog_color);

		float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END_GROUND);
		pContext->SetFloat(IRenderContext::F_FOG_END, fog_end);

		float fog_density = pContext->GetFloat(
			IRenderContext::F_FOG_DENSITY_GROUND);
		pContext->SetFloat(IRenderContext::F_FOG_DENSITY, fog_density);
	}

	unsigned int fog_color = pContext->GetColor(IRenderContext::C_FOG_COLOR);
	unsigned int fog_exp_color = pContext->GetColor(IRenderContext::C_FOG_EXP_COLOR);
	FmVec4 fog_current;
	FmVec4 fog_exp_current;


	fog_current = VisUtil_ColorToVector4(fog_color);
	fog_exp_current = VisUtil_ColorToVector4(fog_exp_color);

	pContext->SetVector4(IRenderContext::V_FOG_CURRENT, fog_current);
	
}

void Scene::Realize()
{
	// ������һ֡�����������
	m_pContext->SavePrevCamera();

	// ���û���������
	m_pRender->SetContext(m_pContext);
	m_pRender->SetSceneView(m_pSceneView);
	
	// Ӧ����Ч����
	apply_fog_param(m_pContext);

	/*
	IColorRT* under_water_rt = m_pContext->GetRealizeTempRT();
	IColorRT* blend_color_rt = m_pContext->GetBlendColorRT();
	IDepthRT* blend_depth_rt = m_pContext->GetBlendDepthRT();
	IColorRT* refraction_rt = m_pContext->GetRefractionRT();
	*/

	m_pSceneView->SetContext(m_pContext);
	if(m_scenetype != IScene::SCENETYPE_GUI)
		m_pRender->AddSceneView(m_pSceneView);

	if (m_pCamera)
	{
		m_pCamera->Realize();
	}

	bool bDynamicShadow = m_pContext->GetEnableDynamicShadow();

	if (bDynamicShadow && m_pPSSM)
	{
		m_pContext->CalculateShadowMapMatrix();
	}
	
	object_t* p = m_pObjects;

	while (p)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(p->ObjectId);

		if (pVisBase)
		{
			pVisBase->Realize();
           
		//	TestErr( pVisBase->GetEntInfo()->GetEntityName() );
		}

		p = p->pNext;
	}

	if (bDynamicShadow && m_pPSSM)
	{
		m_pPSSM->GenerateShadowMap();
	}

	// �ָ�����������
	m_pRender->SetContext(NULL);
	m_pRender->SetSceneView(NULL);
}

PERSISTID Scene::GetWeatherID()
{
	if (NULL == m_pWeather)
	{
		m_pWeather = (Weather*)GetCore()->CreateEntityArgs("Weather",
			CVarList() << this->GetID());
	}

	return m_pWeather->GetID();
}

PERSISTID Scene::GetCameraID()
{
	if (NULL == m_pCamera)
	{
		m_pCamera = (Camera*)GetCore()->CreateEntityArgs("Camera",
			CVarList() << this->GetID());
	}

	return m_pCamera->GetID();
}

void Scene::SetFarClipDistance(float value)
{
	m_pContext->SetFarClipDistance(value);
}

float Scene::GetFarClipDistance() const
{
	return m_pContext->GetFarClipDistance();
}

void Scene::SetBlendDownLevel(int value)
{
	if ((value < 0) || (value > 3))
	{
		return;
	}

	m_pContext->SetBlendDownLevel(value);
}

int Scene::GetBlendDownLevel() const
{
	return m_pContext->GetBlendDownLevel();
}

void Scene::SetEnableRealizeRefraction(bool value)
{
	m_pContext->SetEnableRealizeRefraction(value);
}

bool Scene::GetEnableRealizeRefraction() const
{
	return m_pContext->GetEnableRealizeRefraction();
}

void Scene::SetEnableRealizeTempColorRT(bool value)
{
	m_pContext->SetEnableRealizeTempColorRT(value);
	//�Զ������RBO��Ϊ����
	m_pContext->SetEnableRealizeTempDepthTex(value);
}

bool Scene::GetEnableRealizeTempColorRT() const
{
	return m_pContext->GetEnableRealizeTempColorRT();
}

// �Ƿ���Ҫ������ʱ��Ⱦ�������
void Scene::SetEnableRealizeTempDepthRT(bool value)
{
	//������һ���Զ�ʶ��Ӳ���ж��߼�
	// ����Ƿ�֧�ִ������ͼ
	CVar prop;
	//����Ƿ�֧��������ͼ
	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsDepthTextureSupported", prop);
	bool bIsDepthTextureSupported = prop.BoolVal();

	//�Ƿ�֧��D24S8
	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsDepth24Stencil8Supported", prop);
	bool bDepth24Stencil8Supported = prop.BoolVal();

	if( bIsDepthTextureSupported && bDepth24Stencil8Supported )
	{	
		//�Զ������ģ������
		m_pContext->SetEnableRealizeTempDepthStencilRT(value);

		CORE_TRACE("Scene::EnableRealizeTempDepthStencilRT");
	}

 	if(!m_pContext->GetEnableRealizeTempDepthStencilRT() && bIsDepthTextureSupported)
	{
		// ����Ӳ������Զ�ѡȡ�򿪵��������
		m_pContext->SetEnableRealizTempDepthRT(value);
		CORE_TRACE("Scene::SetEnableRealizTempDepthRT");
	}

	// ����ɹ������Զ��رղ��ɲ��������ͼ
	if(GetEnableRealizeTempDepthRT())
	{
	   m_pContext->SetEnableRealizeTempDepthTex(!value);
	}
}

bool Scene::GetEnableRealizeTempDepthRT() const
{
	return m_pContext->GetEnableRealizTempDepthRT() || m_pContext->GetEnableRealizeTempDepthStencilRT();
}

// �Ƿ�Ҫ������ʱ��Ⱦ������
void Scene::SetEnableRealizeCopyTempColorRT(bool value)
{
	m_pContext->SetEnableRealizeCopyTempColorRT(value);
}

bool Scene::GetEnableRealizeCopyTempColorRT() const
{
    return m_pContext->GetEnableRealizeCopyTempColorRT();
}

// �Ƿ���Ҫ������ʱ��Ⱦ�������
void Scene::SetEnableRealizeCopyTempDepthRT(bool value)
{
	m_pContext->SetEnableRealizeCopyTempDepth(value);
}

bool Scene::GetEnableRealizeCopyTempDepthRT() const
{
   return m_pContext->GetEnableRealizeCopyTempDepth();
}

void Scene::SetSkinSphereAmbient(bool value)
{
	m_pContext->SetInt(IRenderContext::I_SKIN_SPHERE_AMBIENT, int(value));
}

bool Scene::GetSkinSphereAmbient() const
{
	return m_pContext->GetInt(IRenderContext::I_SKIN_SPHERE_AMBIENT) != 0;
}

void Scene::SetModelSphereAmbient(bool value)
{
	m_pContext->SetInt(IRenderContext::I_MODEL_SPHERE_AMBIENT, int(value));
}

bool Scene::GetModelSphereAmbient() const
{
	return m_pContext->GetInt(IRenderContext::I_MODEL_SPHERE_AMBIENT) != 0;
}

void Scene::SetGlobalEnvMap(const char* name)
{
	m_pContext->SetGlobalEnvMap(name);
}

const char* Scene::GetGlobalEnvMap() const
{
	return m_pContext->GetGlobalEnvMap();
}

void Scene::SetReflectFactor(float value)
{
	m_pContext->SetFloat(IRenderContext::F_REFLECT_FACTOR, value);
}

float Scene::GetReflectFactor() const
{
	return m_pContext->GetFloat(IRenderContext::F_REFLECT_FACTOR);
}

void Scene::SetEnableEarlyZ(bool value)
{
	m_pContext->SetEnableEarlyZ(value);
}

bool Scene::GetEnableEarlyZ() const
{
	return m_pContext->GetEnableEarlyZ();
}

void Scene::SetRoleDiffuseFactor(float value)
{
	m_pContext->SetFloat(IRenderContext::F_ROLE_DIFFUSE_FACTOR, value);
}

float Scene::GetRoleDiffuseFactor() const
{
	return m_pContext->GetFloat(IRenderContext::F_ROLE_DIFFUSE_FACTOR);
}

void Scene::SetRoleAmbientFactor(float value)
{
	m_pContext->SetFloat(IRenderContext::F_ROLE_AMBIENT_FACTOR, value);
}

float Scene::GetRoleAmbientFactor() const
{
	return m_pContext->GetFloat(IRenderContext::F_ROLE_AMBIENT_FACTOR);
}

void Scene::SetEnableRoleFallOff(bool value)
{
	m_pContext->SetInt(IRenderContext::I_ENABLE_ROLE_FALLOFF, int(value));
}

bool Scene::GetEnableRoleFallOff() const
{
	return m_pContext->GetInt(IRenderContext::I_ENABLE_ROLE_FALLOFF) != 0;
}

void Scene::SetRoleFallOffColorString(const char* value)
{
	Assert(value != NULL);

	unsigned int val = VisUtil_ColorToInt(value);

	m_pContext->SetColor(IRenderContext::C_ROLE_FALLOFF_COLOR, val);
}

result_string Scene::GetRoleFallOffColorString() const
{
	unsigned int val = m_pContext->GetColor(IRenderContext::C_ROLE_FALLOFF_COLOR);

	return VisUtil_IntToColor(val);
}

void Scene::SetRoleFallOffInten(float value)
{
	m_pContext->SetFloat(IRenderContext::F_ROLE_FALLOFF_INTENSITY, value);
}

float Scene::GetRoleFallOffInten() const
{
	return m_pContext->GetFloat(IRenderContext::F_ROLE_FALLOFF_INTENSITY);
}

void Scene::SetRoleFallOffPower(float value)
{
	m_pContext->SetFloat(IRenderContext::F_ROLE_FALLOFF_POWER, value);
}

float Scene::GetRoleFallOffPower() const
{
	return m_pContext->GetFloat(IRenderContext::F_ROLE_FALLOFF_POWER);
}

IVisBase* Scene::GetVisBase(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return NULL;
	}

	return (IVisBase*)pEntity;
}

PERSISTID Scene::Create(const char* name)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->CreateEntity(name);

	if (NULL == pVisBase)
	{
		return PERSISTID();
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		pVisBase->Release();
		return PERSISTID();
	}

	// ���õ�ǰ����������
	pVisBase->SetContext(m_pContext);

	return pVisBase->GetID();
}

bool Scene::Delete(const PERSISTID& id)
{
	IVisBase* pVisBase = GetVisBase(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	pVisBase->Release();

	return true;
}

bool Scene::SetUnique(const PERSISTID& obj)
{
	IEntity* pEntity = GetCore()->GetEntity(obj);

	if (NULL == pEntity)
	{
		return false;
	}

	return m_pContext->SetUnique(pEntity);
}

PERSISTID Scene::GetUnique(const char* entity_name)
{
	IEntity* pEntity = m_pContext->GetUnique(entity_name);

	if (NULL == pEntity)
	{
		return PERSISTID();
	}

	return pEntity->GetID();
}

bool Scene::AddObject(const PERSISTID& obj, int priority)
{
	IVisBase* pVisBase = GetVisBase(obj);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (pVisBase->GetContext() != m_pContext)
	{
		CORE_TRACE("(Scene::AddObject)context not match");
		CORE_TRACE(pVisBase->GetEntInfo()->GetEntityName());
		return false;
	}

	object_t* pObject = (object_t*)CORE_ALLOC(sizeof(object_t));

	pObject->ObjectId = obj;
	pObject->nPrior = priority;

	// ������ȼ����ߵĽڵ�
	object_t* p = m_pObjects;
	object_t* t = NULL;

	while (p)
	{
		// ��ͬ�����ȼ������ǰ��
		if (p->nPrior <= priority)
		{
			t = p;
		}

		p = p->pNext;
	}

	if (t == NULL)
	{
		// ������ǰ
		pObject->pNext = m_pObjects;
		m_pObjects = pObject;
	}
	else
	{
		// �������
		pObject->pNext = t->pNext;
		t->pNext = pObject;
	}

	return true;
}

bool Scene::RemoveObject(const PERSISTID& obj)
{
	object_t* prev = NULL;
	object_t* p = m_pObjects;

	while (p)
	{
		if (p->ObjectId == obj)
		{
			if (prev != NULL)
			{
				prev->pNext = p->pNext;
			}
			else
			{
				m_pObjects = p->pNext;
			}

			CORE_FREE(p, sizeof(object_t));
			return true;
		}
		else
		{
			prev = p;
			p = p->pNext;
		}
	}

	return false;
}

bool Scene::RemoveObjectByPrior(int priority)
{
	object_t* prev = NULL;
	object_t* p = m_pObjects;

	while (p)
	{
		if (p->nPrior == priority)
		{
			if (prev != NULL)
			{
				prev->pNext = p->pNext;
			}
			else
			{
				m_pObjects = p->pNext;
			}

			CORE_FREE(p, sizeof(object_t));
			return true;
		}
		else
		{
			prev = p;
			p = p->pNext;
		}
	}

	return false;
}

bool Scene::LayerAdd(const char* name, const PERSISTID& id)
{
	IVisBase* pVisBase = GetVisBase(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	return m_pContext->LayerAddObject(name, pVisBase);
}

bool Scene::LayerRemove(const char* name, const PERSISTID& id)
{
	IVisBase* pVisBase = GetVisBase(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	return m_pContext->LayerRemoveObject(name, pVisBase);
}

// PSSM
PERSISTID Scene::GetPSSM() const
{
	if (NULL == m_pPSSM)
	{
		return PERSISTID();
	}

	return m_pPSSM->GetID();
}

PERSISTID Scene::CreatePSSM()
{
	if (NULL == m_pPSSM)
	{
		m_pPSSM = (PSSM*)(GetCore()->CreateEntity("PSSM"));
		m_pPSSM->Load();

		m_pPSSM->SetContext(m_pContext);
	}

	return m_pPSSM->GetID();
}

bool Scene::DeletePSSM()
{
	if (NULL == m_pPSSM)
	{
		return false;
	}

	SAFE_RELEASE(m_pPSSM);

	return true;
}

void Scene::SetEnableDynamicShadow(bool value)
{
	m_pContext->SetEnableDynamicShadow(value);

	if (value && (m_pPSSM == NULL))
	{
		CreatePSSM();
	}
	else if (!value && m_pPSSM)
	{
		DeletePSSM();
	}
}

bool Scene::GetEnableDynamicShadow() const
{
	return m_pContext->GetEnableDynamicShadow();
}

void Scene::SetShadowExpandPercent(float value)
{
	m_pContext->SetShadowExpandPercent(value);
}

float Scene::GetShadowExpandPercent()
{
	return m_pContext->GetShadowExpandPercent();
}

void Scene::SetShadowAdjustingRect(FmVec4 vAdjustingRect)
{
	m_pContext->SetShadowAdjustingRect(vAdjustingRect);
}

const FmVec4& Scene::GetShadowAdjustingRect()
{
	return m_pContext->GetShadowAdjustingRect();
}