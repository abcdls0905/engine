//--------------------------------------------------------------------
// �ļ���:		shadow_manager.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��4��2��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "shadow_manager.h"
#include "shadow.h"
#include "../visual/i_context.h"
#include "../public/var_list.h"
#include "../public/core_log.h"

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

/// \file shadow_manager.cpp
/// \brief Ӱ�ӹ���

/// entity: ShadowManager
/// \brief Ӱ�ӹ���ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(ShadowManager,0,IVisBase);

/// property: bool Visible
/// \brief �Ƿ�ɼ�Ӱ��
DECLARE_PROPERTY(bool, ShadowManager, Visible, GetVisible, SetVisible);
/// property: int MaxShadowNum
/// \brief ���Ӱ������
DECLARE_PROPERTY(int, ShadowManager, MaxShadowNum, GetMaxShadowNum, 
	SetMaxShadowNum);
/// property: object PlayerShadow
/// \brief ������Ҷ����Ӱ��
DECLARE_PROPERTY(PERSISTID, ShadowManager, PlayerShadow, GetPlayerShadowID, 
	SetPlayerShadowID);
/// property: int TextureSize
/// \brief ��Ӱ��ͼ�ߴ�
DECLARE_PROPERTY(int, ShadowManager, TextureSize, GetTextureSize, 
	SetTextureSize);
/// property: bool UseSimpleShadow
/// \brief �Ƿ�ʹ�ü�����Ӱ
DECLARE_PROPERTY(bool, ShadowManager, UseSimpleShadow, GetUseSimpleShadow, 
	SetUseSimpleShadow);

/*
/// method: bool Create(object obj)
/// \brief ����Ӱ��
/// \param obj ģ�Ͷ���
DECLARE_METHOD_1(bool, ShadowManager, Create, const PERSISTID&);
/// method: bool Delete(object obj)
/// \brief ɾ��Ӱ��
/// \param obj Ӱ�Ӷ���
DECLARE_METHOD_1(bool, ShadowManager, Delete, const PERSISTID&);
/// method: table GetShadowList()
/// \brief ����Ӱ���б�
DECLARE_METHOD_T(ShadowManager, GetShadowList);
*/



// ShadowManager

ShadowManager::ShadowManager()
{
	m_bVisible = true;
	m_nMaxShadowNum = -1;
	m_nTextureSize = 256;
	/*
	m_pInputLayout = NULL;
	m_pBlendState = NULL;
	m_pShadowVS = NULL;
	m_pShadowPS = NULL;
	m_pSimpleVS = NULL;
	m_pSimplePS = NULL;
	*/
	m_bUseSimpleShadow = false;
//	m_pSampler = NULL;
	m_pShadowDepth = NULL;

	m_pSimpleShadowShaderHandle = NULL;

	m_pShadowShader = NULL;
	m_pSimpleShadowShader = NULL;
}

ShadowManager::~ShadowManager()
{
//	SAFE_RELEASE(m_pInputLayout);
//	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pShadowDepth);
	SAFE_RELEASE(m_pShadowShader);
	SAFE_RELEASE(m_pSimpleShadowShader);
	SAFE_RELEASE(m_pSimpleShadowShaderHandle->pShader);
	CORE_FREE(m_pSimpleShadowShaderHandle,sizeof(ShadowManager::shadow_shaderhandle_t ));
}

bool ShadowManager::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}

	/*

	D3D11_INPUT_ELEMENT_DESC ied[1] = { 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
			D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	};

	m_pInputLayout = m_pRender->CreateInputLayout(ied, 1, "shadow.hlsl", 
		"vs_main_shadow", "");

	D3D11_BLEND_DESC bd;

	memset(&bd, 0, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pBlendState = m_pRender->CreateBlendState(&bd);
	*/

	return true;
}

bool ShadowManager::Shut()
{
	for (size_t i = 0; i < m_Shadows.size(); ++i)
	{
		Shadow* pShadow = (Shadow*)GetCore()->GetEntity(
			m_Shadows[i].ShadowID);

		if (pShadow)
		{
			pShadow->Release();
		}
	}

	m_Shadows.clear();
	
	return true;
}

void ShadowManager::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
	// ��������Ϊ����Ψһ����
	m_pContext->SetUnique(this);
}


#define VERTEX_ARRAY	0

const char* shadow_vertex_t_name[] = 
{
	"iPos",
};



bool ShadowManager::Load()
{
	IVertexShader* pSimpleVS = m_pRender->LoadVertexShader("simple_shadow.vsh", 
		"vs_main_simple_shadow", ""); 
	IPixelShader* pSimplePS = m_pRender->LoadPixelShader("simple_shadow.fsh", 
		"ps_main_simple_shadow", ""); 

	IShaderProgram* pShadowShader = m_pRender->CreateShaderProgram( pSimpleVS , pSimplePS,shadow_vertex_t_name,1);

	m_pSimpleShadowShaderHandle = (ShadowManager::shadow_shaderhandle_t*)CORE_ALLOC( sizeof(ShadowManager::shadow_shaderhandle_t ));

	m_pSimpleShadowShaderHandle->pShader = pShadowShader;
	IShaderParamOp* pShaderOp = pShadowShader->GetParamOp();
	
	m_pSimpleShadowShaderHandle->c_BottomColorHandle = pShaderOp->FindParamIdByName("c_BottomColor");
	m_pSimpleShadowShaderHandle->c_fLightDispersionHandle = pShaderOp->FindParamIdByName("c_fLightDispersion");
	m_pSimpleShadowShaderHandle->c_mtxLightMVPHandle = pShaderOp->FindParamIdByName("c_mtxLightMVP");
	m_pSimpleShadowShaderHandle->c_mtxViewProjHandle = pShaderOp->FindParamIdByName("c_mtxViewProj");
	m_pSimpleShadowShaderHandle->c_TopColorHandle = pShaderOp->FindParamIdByName("c_TopColor");


	m_pShadowDepth = m_pRender->CreateDepthRT(m_nTextureSize,
		m_nTextureSize, TEX_FORMAT_D_DEFAULT, RT_TYPE_FIXED);

	if (NULL == m_pShadowDepth)
	{
		CORE_TRACE("(ShadowManager::Load)create shadow depth failed");
		return false;
	}

	return true;
}

int __cdecl ShadowManager::DistanceCompare(const void* elem1, 
	const void* elem2)
{
	float d1 = ((shadow_t*)elem1)->fDistance;
	float d2 = ((shadow_t*)elem2)->fDistance;

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

void ShadowManager::Update(float seconds)
{
	m_f_offset_seconds = seconds;
	IRenderContext* pContext = m_pRender->GetContext();
	
	if (pContext->GetEnableDynamicShadow())
	{
		return;
	}

	if (m_Shadows.empty())
	{
		return;
	}

	FmVec3 camera_pos = pContext->GetCamera().vPosition;
	shadow_t* shadows = &m_Shadows[0];
	size_t shadow_num = m_Shadows.size();
	size_t it = 0;
	
	while (it < shadow_num)
	{
		shadow_t* pData = &shadows[it];
		Shadow* pShadow = (Shadow*)GetCore()->GetEntity(pData->ShadowID);

		if (NULL == pShadow)
		{
			m_Shadows.remove(it);
			--shadow_num;
			continue;
		}

		PERSISTID bind_id = pShadow->GetBindID();

		if (!bind_id.IsNull())
		{
			IEntity* pEntity = GetCore()->GetEntity(bind_id);

			if (NULL == pEntity)
			{
				// ��󶨶���һ��ɾ��
				pShadow->Release();
				m_Shadows.remove(it);
				--shadow_num;
				continue;
			}
		}

		pShadow->Update(seconds);

		FmVec3 v = camera_pos - pShadow->GetPosition();

		// ����Ӱ�ӵ�������ľ���
		pData->fDistance = VisUtil_Vec3Length(&v);
		++it;
	}

	if (!m_Shadows.empty())
	{
		shadow_t* shadows = &m_Shadows[0];
		size_t shadow_num = m_Shadows.size();

		// ����
		qsort(shadows, shadow_num, sizeof(shadow_t), DistanceCompare);

		int shadow_count = 0;

		if (!m_PlayerShadowID.IsNull())
		{
			// ����Ⱦ���ǵ�Ӱ��
			Shadow* pShadow = (Shadow*)GetCore()->GetEntity(m_PlayerShadowID);

			if (pShadow)
			{
				if (m_nMaxShadowNum > 0)
				{
					shadow_count++;
				}
			}
		}

		for (size_t i = 0; i < shadow_num; ++i)
		{
			PERSISTID shadow_id = shadows[i].ShadowID;

			if (shadow_id == m_PlayerShadowID)
			{
				if (0 == m_nMaxShadowNum)
				{
					shadows[i].bShow = false;
				}
				else
				{
					shadows[i].bShow = true;
				}

				continue;
			}

			Shadow* pShadow = (Shadow*)GetCore()->GetEntity(shadow_id);

			if (NULL == pShadow)
			{
				continue;
			}

			bool show = true;

			if ((m_nMaxShadowNum >= 0) 
				&& (shadow_count >= m_nMaxShadowNum))
			{
				show = false;
			}
			else if (pShadow->IsViewClip())
			{
				show = false;
			}

			if (show)
			{
				shadow_count++;
			}

			shadows[i].bShow = show;
		}
	}
}

void ShadowManager::Realize()
{
	if (m_pRender->GetContext()->GetEnableDynamicShadow())
	{
		return;
	}
	
	if (!m_bVisible)
	{
		return;
	}
	
	if (m_Shadows.empty())
	{
		return;
	}

#ifdef PERFORMANCE_DEBUG
	double dRealizeShadow = performance_time();
#endif

	shadow_t* shadows = &m_Shadows[0];
	size_t shadow_num = m_Shadows.size();

	for (size_t i = 0; i < shadow_num; ++i)
	{
		if (!shadows[i].bShow)
		{
			continue;
		}

		Shadow* pShadow = (Shadow*)GetCore()->GetEntity(shadows[i].ShadowID);

		if (pShadow)
		{
			pShadow->Realize(m_f_offset_seconds);
		}
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dShadowRealizeTime += 
		performance_time() - dRealizeShadow;
#endif
}

void ShadowManager::SetVisible(bool value)
{
	m_bVisible = value;
}

bool ShadowManager::GetVisible() const
{
	return m_bVisible;
}

void ShadowManager::SetMaxShadowNum(int value)
{
	m_nMaxShadowNum = value;
}

int ShadowManager::GetMaxShadowNum() const
{
	return m_nMaxShadowNum;
}

void ShadowManager::SetPlayerShadowID(const PERSISTID& value)
{
	Shadow* pShadow = (Shadow*)GetCore()->GetEntity(value);

	if (NULL == pShadow)
	{
		return;
	}

	if (!pShadow->GetEntInfo()->IsKindOf("Shadow"))
	{
		return;
	}
	
	m_PlayerShadowID = value;
}

PERSISTID ShadowManager::GetPlayerShadowID() const
{
	return m_PlayerShadowID;
}

void ShadowManager::SetTextureSize(int value)
{
	if ((value != 32) && (value != 64) && (value != 128) && (value != 256)
		&& (value != 512) && (value != 1024) && (value != 2048))
	{
		return;
	}

	m_nTextureSize = value;
}

int ShadowManager::GetTextureSize() const
{
	return m_nTextureSize;
}

PERSISTID ShadowManager::Create(const PERSISTID& id)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);
	
	if (NULL == pVisBase)
	{
		return PERSISTID();
	}
	
	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return PERSISTID();
	}
	
	Shadow* pShadow = (Shadow*)GetCore()->CreateEntity("Shadow");

	pShadow->SetContext(m_pContext);
	pShadow->SetShadowManager(this);
	pShadow->Load();
	pShadow->SetBindID(pVisBase->GetID());

	shadow_t data;

	data.ShadowID = pShadow->GetID();
	data.fDistance = 0.0F;
	data.bShow = false;

	m_Shadows.push_back(data);
	
	return pShadow->GetID();
}

bool ShadowManager::Delete(const PERSISTID& id)
{
	Shadow* pShadow = (Shadow*)GetCore()->GetEntity(id);

	if (NULL == pShadow)
	{
		return false;
	}

	if (!pShadow->GetEntInfo()->IsKindOf("Shadow"))
	{
		return false;
	}

	pShadow->Release();

	return true;
}

void ShadowManager::GetShadowList(const IVarList& args, IVarList& result)
{
	result.Clear();

	for (size_t i = 0; i < m_Shadows.size(); ++i)
	{
		result.AddObject(m_Shadows[i].ShadowID);
	}
}

void ShadowManager::SetUseSimpleShadow(bool value)
{
	m_bUseSimpleShadow = value;

}

bool ShadowManager::GetUseSimpleShadow() const
{
	return m_bUseSimpleShadow;
}
