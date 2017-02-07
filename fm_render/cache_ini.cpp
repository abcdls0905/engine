//--------------------------------------------------------------------
// 文件名:		CacheIni.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年4月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "cache_ini.h"
#include "cache_ini_loader.h"
//#include "../visual/IDxRender.h"
#include "../public/i_ini_file.h"
#include "../public/core_log.h"
#include "../public/inlines.h"

// CCacheIni

CCacheIni* CCacheIni::NewInstace()
{
	return CORE_NEW(CCacheIni);
}

CCacheIni::CCacheIni()
{
	m_pLoader = NULL;
	m_nState = 0;
	m_pIni = NULL;
}

CCacheIni::~CCacheIni()
{
	SAFE_RELEASE(m_pLoader);
	SAFE_RELEASE(m_pIni);
//	if (m_pIni)
//	{
//		delete m_pIni;
//	}
}

void CCacheIni::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

bool CCacheIni::Create(bool create_hash)
{
	if (!BeginCreate(create_hash))
	{
		return false;
	}
	
	if (!m_pLoader->Load(false))
	{
		EndCreate(0, false);
		return false;
	}
	
	if (!m_pLoader->Create(false))
	{
		EndCreate(0, false);
		return false;
	}
	
	if (!EndCreate(0, true))
	{
		return false;
	}
	
	return true;
}

bool CCacheIni::BeginCreate(bool create_hash)
{
	CCacheIniLoader* loader = CCacheIniLoader::NewInstance();
	
	loader->SetOwner(this);
	loader->SetFileName(GetName());
	loader->SetCreateHash(create_hash);
	
	m_pLoader = loader;
	m_nState = STATE_LOADING;
	
	return true;
}

bool CCacheIni::EndCreate(int category, bool succeed)
{
	SAFE_RELEASE(m_pLoader);

	if (succeed)
	{
		m_nState = STATE_READY;
	}
	else
	{
		m_nState = STATE_FAILED;
	}
	
	return true;
}

IResLoader* CCacheIni::GetLoader() const
{
	return m_pLoader;
}

bool CCacheIni::Build(IIniFile* pIni)
{
	Assert(pIni != NULL);

	m_pIni = pIni;

	return true;
}

