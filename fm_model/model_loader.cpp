
#include "model_loader.h"
#include "res_model.h"
#include "model_data.h"
#include "../visual/i_render.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// 加载模型的所有贴图
bool load_model_texture(model_t* pInstance, const char* tex_paths, bool async);

// CModelLoader

CModelLoader* CModelLoader::NewInstance()
{
	return CORE_NEW(CModelLoader);
}

CModelLoader::CModelLoader()
{
	m_pOwner = NULL;
	m_pModel = NULL;
}

CModelLoader::~CModelLoader()
{
	if (m_pModel)
	{
		unload_model(m_pModel);
	}
}

void CModelLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CModelLoader::SetOwner(CResModel* pOwner)
{
	m_pOwner = pOwner;
}

void CModelLoader::SetFileName(const char* file_name)
{
	m_strFileName = file_name;
}

const char* CModelLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CModelLoader::Load(bool async)
{
	m_pModel = load_model(m_strFileName.c_str());

	if (NULL == m_pModel)
	{
		return false;
	}
	
	return true;
}

bool CModelLoader::Create(bool async)
{
	// 加载模型的贴图
	load_model_texture(m_pModel, m_pOwner->GetTexPaths(), async);

	if (!m_pOwner->Build(m_pModel))
	{
		return false;
	}

	// 数据所有权已经转移
	m_pModel = NULL;

	return true;
}

bool CModelLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

