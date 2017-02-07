//--------------------------------------------------------------------
// 文件名:		tex_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年7月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tex_loader.h"
#include "texture.h"
#include "render.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/inlines.h"
#include "tex_tools.h"

#include "../public/portable.h"

#ifdef GPU_MULTITHREADING
extern IDeviceContext* g_ResourceContext;
#endif

#define performance_time Port_GetPerformanceTime
// CTexLoader

inline bool load_file_to_buffer(const char* name, unsigned char*& pbuffer, unsigned int & data_size)
{
	FILE* fp = core_file::fopen(name, "rb");
	if(fp == NULL)
		return false;
	core_file::fseek(fp, 0, SEEK_END);
	size_t dds_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);
	
	IFileSys* pFileSys = g_pCore->GetFileSys();
	// 尝试直接获得文件数据，避免临时申请大块内存
	const void* dds_data = pFileSys->FileContent(fp);
    
	data_size  = dds_size;
    
	if (NULL == dds_data)
	{
		pbuffer = (unsigned char*)CORE_ALLOC(data_size);
        
		if (core_file::fread(pbuffer, sizeof(unsigned char), dds_size, fp)
			!= dds_size)
		{
			CORE_TRACE("(CTexLoader::Load)read tex data failed");
			CORE_TRACE(name);
			core_file::fclose(fp);
			return false;
		}
	}
	core_file::fclose(fp);
	return true;
}

CTexLoader* CTexLoader::NewInstance()
{
	return CORE_NEW(CTexLoader);
}

CTexLoader::CTexLoader()
{
	m_pOwner = NULL;
	m_pRender = NULL;
	m_bOriginSize = false;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_pData = NULL;
	m_nDataSize = 0;
	m_pTexture = NULL;
	m_nDataMaskSize = 0;
	m_pDataMask = 0;
	//m_pTraceTex = NULL;
}

CTexLoader::~CTexLoader()
{
	if (m_pData)
	{
		CORE_FREE(m_pData, m_nDataSize);
	}

	if(m_pDataMask)
	{
		CORE_FREE(m_pDataMask, m_nDataMaskSize);
	}

	//SAFE_RELEASE(m_pTexture);
	//SAFE_RELEASE(m_pTraceTex);
}

void CTexLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CTexLoader::SetOwner(CTexture* pOwner)
{
	m_pOwner = pOwner;
	m_pRender = pOwner->GetRender();
}

void CTexLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

void CTexLoader::SetOriginSize(bool value)
{
	m_bOriginSize = value;
}

const char* CTexLoader::GetName()
{
	return m_strFileName.c_str();
}

// 文件是否存在
static bool file_exists(const char* name)
{
	// 为了提高查找速度，只在打包的文件中搜索
	return g_pCore->GetFileSys()->FileInPack(name);
}

bool CTexLoader::load_file(const char* name)
{
	if(!::load_file_to_buffer(name, m_pData, m_nDataSize))
		return false;

	if(strstr(name, ".jpg"))
	{
		char buf[256];
		sprintf(buf, "%s.mask", name);
		if(!::load_file_to_buffer(buf, m_pDataMask, m_nDataMaskSize))
		{
			m_pDataMask = 0;
			m_nDataMaskSize = 0;
		}
	}
	 
	return true;
}

bool CTexLoader::Load(bool async)
{
  //printf("file:%s\n", m_strFileName.c_str());
	if(file_exists(m_strFileName.c_str()))
	{
#ifdef GPU_MULTITHREADING
		if(g_ResourceContext && async)
		{
			if(!load_file(m_strFileName.c_str()))
				return false;
			m_pOwner->Build(m_strFileName.c_str(), m_pData, this->m_nDataSize, this->m_pDataMask, this->m_nDataMaskSize);
			m_pOwner->WaitThread();
			return true;
		}
		else
#endif
		{
			return load_file(m_strFileName.c_str());
		}
	}
	else
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CTexLoader::Load)texture %s not found", m_strFileName.c_str());
		CORE_TRACE(info);
		return false;
	}
}

bool CTexLoader::Create(bool async)
{
#ifdef PERFORMANCE_DEBUG
	double dTime = performance_time();
#endif
#ifdef GPU_MULTITHREADING
    if(!(g_ResourceContext && async))
	{
		m_pOwner->Build(m_strFileName.c_str(), m_pData, this->m_nDataSize, this->m_pDataMask, this->m_nDataMaskSize);
	}
#else
	m_pOwner->Build(m_strFileName.c_str(), m_pData, this->m_nDataSize, this->m_pDataMask, this->m_nDataMaskSize);
#endif
    
#ifdef PERFORMANCE_DEBUG
    m_pRender->GetPerformance()->dTexBuildGPUTime += performance_time() - dTime;
	m_pRender->GetPerformance()->dLoadTextureData += m_nDataSize/1024.0f;
#endif
	
	return true;
}

bool CTexLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}
