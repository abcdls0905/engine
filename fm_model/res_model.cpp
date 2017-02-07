//--------------------------------------------------------------------
// �ļ���:		res_model.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "res_model.h"
#include "model_loader.h"
#include "res_manager.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"

extern IRender* g_pRender;

// ģ�͵���ͼ�Ƿ�������
bool model_texture_load_complete(model_t* pInstance);
// ж��ģ�͵�������ͼ
bool unload_model_texture(model_t* pInstance);

// CResModel

CResModel* CResModel::NewInstance()
{
	return CORE_NEW(CResModel);
}

CResModel::CResModel()
{
	m_pManager = NULL;
	m_pModel = NULL;
	m_pLoader = NULL;
	m_nState = 0;
}

CResModel::~CResModel()
{
	SAFE_RELEASE(m_pLoader);

	if (m_pModel)
	{
		unload_model(m_pModel);
	}
}

void CResModel::Destroy()
{
	CORE_DELETE(this);
}

void CResModel::Release()
{
	m_pManager->ReleaseResModel(this);
}

void CResModel::SetTexPaths(const char* value)
{
	Assert(value != NULL);

	m_strTexPaths = value;
}

const char* CResModel::GetTexPaths() const
{
	return m_strTexPaths.c_str();
}

bool CResModel::GetTextureLoadComplete() const
{
	if (m_pModel)
	{
		if (!model_texture_load_complete(m_pModel))
		{
			return false;
		}
	}
	
	return true;
}

void CResModel::ReleaseTextures()
{
	if (m_pModel)
	{
		unload_model_texture(m_pModel);
	}
}

bool CResModel::Create()
{
	if (!BeginCreate())
	{
		return false;
	}
	
	if (!m_pLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_MODEL, false);
		return false;
	}
	
	if (!m_pLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_MODEL, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_MODEL, true))
	{
		return false;
	}
	
	return true;
}

bool CResModel::CanAsync()
{
	return true;
}

bool CResModel::BeginCreate()
{
	Assert(m_pLoader == NULL);
	
	char model_name[256];

	SafeSprintf(model_name, sizeof(model_name), "%s%s",
		g_pRender->GetPathPrefix(GetName()), GetName());

	CModelLoader* loader = CModelLoader::NewInstance();
	
	loader->SetOwner(this);
	loader->SetFileName(model_name);
	
	m_pLoader = loader;

	return true;
}

bool CResModel::EndCreate(int category, bool succeed)
{
	if (category == RES_CATEGORY_MODEL)
	{
		SAFE_RELEASE(m_pLoader);
	}
	
	if (succeed)
	{
		// ������Դ����
		SetState(STATE_READY);
	}
	else
	{
		SetState(STATE_FAILED);
	}
	
	return true;
}

IResLoader* CResModel::GetLoader()
{
	return m_pLoader;
}

bool CResModel::Build(model_t* pModel)
{
	Assert(pModel != NULL);

	m_pModel = pModel;

	return true;
}
