//--------------------------------------------------------------------
// �ļ���:		CacheIniLoader.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2011��4��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "cache_ini_loader.h"
#include "cache_ini.h"
//#include "../utils/ReadIni.h"
#include "../public/i_ini_file.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

// CCacheIniLoader

CCacheIniLoader* CCacheIniLoader::NewInstance()
{
	return CORE_NEW(CCacheIniLoader);
}

CCacheIniLoader::CCacheIniLoader()
{
	m_pOwner = NULL;
	m_bCreateHash = false;
	m_pIni = NULL;
}

CCacheIniLoader::~CCacheIniLoader()
{
	SAFE_RELEASE(m_pIni);
}

void CCacheIniLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CCacheIniLoader::SetOwner(CCacheIni* pOwner)
{
	Assert(pOwner != NULL);

	m_pOwner = pOwner;
}

void CCacheIniLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

void CCacheIniLoader::SetCreateHash(bool value)
{
	m_bCreateHash = value; 
}

const char* CCacheIniLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CCacheIniLoader::Load(bool async)
{
	// �����ļ�
	//CReadIni* pIni = NEW CReadIni(m_strFileName.c_str());
	IIniFile* pIni = g_pCore->CreateIniFile(m_strFileName.c_str());
		
	if (!pIni->LoadFromFile())
	{
		//delete pIni;
		pIni->Release();
		CORE_TRACE("(CCacheIniLoader::Load)load ini failed");
		CORE_TRACE(m_strFileName.c_str());
		return false;
	}
	
	if (m_bCreateHash)
	{
		pIni->CreateHashIndex();
	}
	
	m_pIni = pIni;

	return true;
}

bool CCacheIniLoader::Create(bool async)
{
	if (!m_pOwner->Build(m_pIni))
	{
		return false;
	}
	
	// ��������Ȩ�Ѿ�ת��
	m_pIni = NULL;

	return true;
}

bool CCacheIniLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

