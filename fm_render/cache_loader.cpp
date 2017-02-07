//--------------------------------------------------------------------
// �ļ���:		CacheLoader.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2010��9��19��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "cache_loader.h"
#include "cache_file.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

// CCacheLoader

CCacheLoader* CCacheLoader::NewInstance()
{
	return CORE_NEW(CCacheLoader);
}

CCacheLoader::CCacheLoader()
{
	m_pOwner = NULL;
	m_pData = NULL;
	m_nSize = 0;
}

CCacheLoader::~CCacheLoader()
{
	if (m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
	}
}

void CCacheLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CCacheLoader::SetOwner(CCacheFile* pOwner)
{
	Assert(pOwner != NULL);

	m_pOwner = pOwner;
}

void CCacheLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

const char* CCacheLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CCacheLoader::Load(bool async)
{
	// �����ļ�
	FILE* fp = core_file::fopen(m_strFileName.c_str(), "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(CCacheLoader::Load)open failed");
		CORE_TRACE(m_strFileName.c_str());
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_len = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);
	unsigned char* pdata = (unsigned char*)CORE_ALLOC(file_len + 1);
	
	if (core_file::fread(pdata, sizeof(unsigned char), file_len, fp) 
		!= file_len)
	{
		CORE_TRACE("(CCacheLoader::Load)read data failed");
		CORE_TRACE(m_strFileName.c_str());
		CORE_FREE(pdata, file_len + 1);
		core_file::fclose(fp);
		return false;
	}
	
	pdata[file_len] = 0;

	core_file::fclose(fp);
		
	m_pData = pdata;
	m_nSize = file_len + 1;

	return true;
}

bool CCacheLoader::Create(bool async)
{
	if (!m_pOwner->Build(m_pData, m_nSize))
	{
		return false;
	}
	
	// ��������Ȩ�Ѿ�ת��
	m_pData = NULL;
	m_nSize = 0;

	return true;
}

bool CCacheLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

