//--------------------------------------------------------------------
// 文件名:		CacheFile.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年9月19日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "cache_file.h"
#include "cache_loader.h"
//#include "../visual/IDxRender.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

// CCacheFile

CCacheFile* CCacheFile::NewInstace()
{
	return CORE_NEW(CCacheFile);
}

CCacheFile::CCacheFile()
{
	m_pLoader = NULL;
	m_nState = 0;
	m_pData = NULL;
	m_nSize = 0;
}

CCacheFile::~CCacheFile()
{
	SAFE_RELEASE(m_pLoader);
	
	if (m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
	}
}

void CCacheFile::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

bool CCacheFile::Create()
{
	if (!BeginCreate())
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

bool CCacheFile::BeginCreate()
{
	CCacheLoader* loader = CCacheLoader::NewInstance();
	
	loader->SetOwner(this);
	loader->SetFileName(GetName());
	
	m_pLoader = loader;
	m_nState = STATE_LOADING;
	
	return true;
}

bool CCacheFile::EndCreate(int category, bool succeed)
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

IResLoader* CCacheFile::GetLoader() const
{
	return m_pLoader;
}

bool CCacheFile::Build(unsigned char* pdata, size_t size)
{
	Assert(pdata != NULL);

	m_pData = pdata;
	m_nSize = size;

	return true;
}

