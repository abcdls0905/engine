//--------------------------------------------------------------------
// �ļ���:		render_tex.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "render_tex.h"
#include "weather.h"
#include "camera.h"
#include "../visual/i_vis_base.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

/// \file render_tex.cpp
/// \brief ��Ϊ��ȾĿ�����ͼ

/// entity: RenderTex
/// \brief ��Ϊ��ȾĿ�����ͼʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(RenderTex, 0, IVisBase);

/// property: string Name
/// \brief ��ͼ��Դ��
DECLARE_PROPERTY(const char*, RenderTex, Name, GetName, SetName);
/// property: int Width
/// \brief ��ͼ���
DECLARE_PROPERTY(int, RenderTex, Width, GetWidth, SetWidth);
/// property: int Height
/// \brief ��ͼ�߶�
DECLARE_PROPERTY(int, RenderTex, Height, GetHeight, SetHeight);

/// readonly: object Weather
/// \brief ������������
DECLARE_PROPERTY_GET(PERSISTID, RenderTex, Weather, GetWeatherID);
/// readonly: object Camera
/// \brief ���������
DECLARE_PROPERTY_GET(PERSISTID, RenderTex, Camera, GetCameraID);

/// property: string BackColor
/// \brief ������ɫ
DECLARE_PROPERTY(result_string, RenderTex, BackColor, GetBackColorString, 
	SetBackColorString);
/// property: float FarClipDistance
/// \brief Զ������ľ���
DECLARE_PROPERTY(float, RenderTex, FarClipDistance, GetFarClipDistance, 
	SetFarClipDistance);

// RenderTex

RenderTex::RenderTex()
{
	m_pRender = NULL;
	m_pContext = NULL;
	m_pSceneView = NULL;
	m_pWeather = NULL;
	m_pCamera = NULL;
	m_pObjects = NULL;
	m_nWidth = 64;
	m_nHeight = 64;
	m_nBackColor = 0;
	m_pRenderTex = NULL;
	m_pDepthRT = NULL;
}

RenderTex::~RenderTex()
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
	SAFE_RELEASE(m_pRenderTex);
	SAFE_RELEASE(m_pDepthRT);
}

bool RenderTex::Init(const IVarList& args)
{
	const char* render_name = args.StringVal(0);

	if (render_name[0] == 0)
	{
		render_name = "Render";
	}

	m_pRender = (IRender*)GetCore()->GetInterface(render_name);

	if (NULL == m_pRender)
	{
		return false;
	}

	// ������Ⱦ������
	m_pContext = m_pRender->CreateContext();
	// ����������ͼ
	m_pSceneView = m_pRender->CreateSceneView(ISceneView::TYPE_NORMAL);

	return true;
}

bool RenderTex::Shut()
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
	
	return true;
}

bool RenderTex::Load()
{
	SAFE_RELEASE(m_pRenderTex);
	SAFE_RELEASE(m_pDepthRT);

	m_pRenderTex = m_pRender->CreateRenderTexture(m_strName.c_str(), 
		m_nWidth, m_nHeight, TEX_FORMAT_RGBA8888_UNORM);

	if (NULL == m_pRenderTex)
	{
		CORE_TRACE("(RenderTex::Load)create render texture failed");
		return false;
	}

	if (m_pRenderTex->GetShaderTex()->GetResourceType() 
		!= IResource::TYPE_COLOR_RT)
	{
		SAFE_RELEASE(m_pRenderTex);
		CORE_TRACE("(RenderTex::Load)create render texture failed");
		return false;
	}

	m_pDepthRT = m_pRender->CreateDepthRT(m_nWidth, m_nHeight, TEX_FORMAT_D24S8_UINT, RT_TYPE_FIXED);

	if (NULL == m_pDepthRT)
	{
		SAFE_RELEASE(m_pRenderTex);
		CORE_TRACE("(RenderTex::Load)create depth rt failed");
		return false;
	}

	m_pContext->SetViewSize(m_nWidth, m_nHeight);

	return true;
}

void RenderTex::Update(float seconds)
{
	if ((NULL == m_pRenderTex) || (NULL == m_pDepthRT))
	{
		return;
	}

	// ���û���������
	m_pRender->SetContext(m_pContext);
	m_pRender->SetSceneView(m_pSceneView);

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
void render_tex_apply_fog_param(IRenderContext* pContext)
{
	// �Ƿ���ˮ��
	if (pContext->GetInt(IRenderContext::I_UNDER_WATER) != 0) 
	{
		// ����ˮ����Ч
		unsigned int fog_color = pContext->GetColor(
			IRenderContext::C_FOG_COLOR_UNDERWATER);
		pContext->SetColor(IRenderContext::C_FOG_COLOR, fog_color);

		float fog_start = pContext->GetFloat(
			IRenderContext::F_FOG_START_UNDERWATER);
		pContext->SetFloat(IRenderContext::F_FOG_START, fog_start);

		float fog_end = pContext->GetFloat(
			IRenderContext::F_FOG_END_UNDERWATER);
		pContext->SetFloat(IRenderContext::F_FOG_END, fog_end);

		float fog_density = pContext->GetFloat(
			IRenderContext::F_FOG_DENSITY_UNDERWATER);
		pContext->SetFloat(IRenderContext::F_FOG_DENSITY, fog_density);
	}
	else
	{
		// ���õ�����Ч
		unsigned int fog_color = pContext->GetColor(
			IRenderContext::C_FOG_COLOR_GROUND);
		pContext->SetColor(IRenderContext::C_FOG_COLOR, fog_color);

		float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START_GROUND);

		pContext->SetFloat(IRenderContext::F_FOG_START, fog_start);

		float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END_GROUND);
		pContext->SetFloat(IRenderContext::F_FOG_END, fog_end);

		float fog_density = pContext->GetFloat(
			IRenderContext::F_FOG_DENSITY_GROUND);
		pContext->SetFloat(IRenderContext::F_FOG_DENSITY, fog_density);
	}
}

void RenderTex::Realize()
{
	if ((NULL == m_pRenderTex) || (NULL == m_pDepthRT))
	{
		return;
	}
	
	// ������һ֡�����������
	m_pContext->SavePrevCamera();

	// ���û���������
	m_pRender->SetContext(m_pContext);
	m_pRender->SetSceneView(m_pSceneView);

	// Ӧ����Ч����
	render_tex_apply_fog_param(m_pContext);

	IColorRT* pColorRT = (IColorRT*)m_pRenderTex->GetShaderTex();

	Assert(pColorRT->GetResourceType() == IResource::TYPE_COLOR_RT);

	m_pSceneView->SetContext(m_pContext);
	m_pRender->AddSceneView(m_pSceneView);

	if (m_pCamera)
	{
		m_pCamera->Realize();
	}

	object_t* p = m_pObjects;

	while (p)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(p->ObjectId);

		if (pVisBase)
		{
			pVisBase->Realize();
		}

		p = p->pNext;
	}

	// �ָ�����������
	m_pRender->SetContext(NULL);
	m_pRender->SetSceneView(NULL);
}

PERSISTID RenderTex::GetWeatherID()
{
	if (NULL == m_pWeather)
	{
		m_pWeather = (Weather*)GetCore()->CreateEntityArgs("Weather", 
			CVarList() << this->GetID());
	}

	return m_pWeather->GetID();
}

PERSISTID RenderTex::GetCameraID()
{
	if (NULL == m_pCamera)
	{
		m_pCamera = (Camera*)GetCore()->CreateEntityArgs("Camera",
			CVarList() << this->GetID());
	}

	return m_pCamera->GetID();
}

void RenderTex::SetName(const char* value)
{
	Assert(value != NULL);

	m_strName = value;
}

const char* RenderTex::GetName() const
{
	return m_strName.c_str();
}

void RenderTex::SetWidth(int value)
{
	if ((value <= 0) || (value > 4096))
	{
		return;
	}
	
	m_nWidth = value;
}

int RenderTex::GetWidth() const
{
	return m_nWidth;
}

void RenderTex::SetHeight(int value)
{
	if ((value <= 0) || (value > 4096))
	{
		return;
	}

	m_nHeight = value;
}

int RenderTex::GetHeight() const
{
	return m_nHeight;
}

void RenderTex::SetBackColorString(const char* value)
{
	m_nBackColor = VisUtil_ColorToInt(value);
}

result_string RenderTex::GetBackColorString() const
{
	return VisUtil_IntToColor(m_nBackColor);
}

void RenderTex::SetFarClipDistance(float value)
{
	m_pContext->SetFarClipDistance(value);
}

float RenderTex::GetFarClipDistance() const
{
	return m_pContext->GetFarClipDistance();
}

IVisBase* RenderTex::GetVisBase(const PERSISTID& id) const
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

PERSISTID RenderTex::Create(const char* name)
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

bool RenderTex::Delete(const PERSISTID& id)
{
	IVisBase* pVisBase = GetVisBase(id);

	if (NULL == pVisBase)
	{
		return false;
	}

	pVisBase->Release();

	return true;
}

bool RenderTex::SetUnique(const PERSISTID& obj)
{
	IEntity* pEntity = GetCore()->GetEntity(obj);

	if (NULL == pEntity)
	{
		return false;
	}

	return m_pContext->SetUnique(pEntity);
}

PERSISTID RenderTex::GetUnique(const char* entity_name)
{
	IEntity* pEntity = m_pContext->GetUnique(entity_name);

	if (NULL == pEntity)
	{
		return PERSISTID();
	}

	return pEntity->GetID();
}

bool RenderTex::AddObject(const PERSISTID& obj, int priority)
{
	IVisBase* pVisBase = GetVisBase(obj);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (pVisBase->GetContext() != m_pContext)
	{
		CORE_TRACE("(RenderTex::AddObject)context not match");
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

bool RenderTex::RemoveObject(const PERSISTID& obj)
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

bool RenderTex::RemoveObjectByPrior(int priority)
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
