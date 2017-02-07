//--------------------------------------------------------------------
// 文件名:		material_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年3月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "material_loader.h"
#include "model_player.h"
#include "../public/i_ini_file.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

// CMaterialLoader

CMaterialLoader* CMaterialLoader::NewInstance()
{
	return CORE_NEW(CMaterialLoader);
}

CMaterialLoader::CMaterialLoader()
{
	m_pOwner = NULL;
	m_pIniFile = NULL;
}

CMaterialLoader::~CMaterialLoader()
{
	SAFE_RELEASE(m_pIniFile);
}

void CMaterialLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CMaterialLoader::SetOwner(CModelPlayer* pOwner)
{
	Assert(pOwner != NULL);

	m_pOwner = pOwner;
}

void CMaterialLoader::SetFileName(const char* file_name)
{
	m_strFileName = file_name;
}

const char* CMaterialLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CMaterialLoader::Load(bool async)
{
	m_pIniFile = g_pCore->CreateIniFile(m_strFileName.c_str());
	m_pIniFile->LoadFromFile();
		
	return true;
}

bool CMaterialLoader::Create(bool async)
{
	if (NULL == m_pIniFile)
	{
		return false;
	}

	if (!m_pOwner->BuildMaterial(m_pIniFile))
	{
		return false;
	}
	
	return true;
}

bool CMaterialLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}
