//--------------------------------------------------------------------
// 文件名:		cube_tex_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "cube_tex_loader.h"
#include "texture.h"
#include "render.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "tex_tools.h"

// CCubeTexLoader

CCubeTexLoader* CCubeTexLoader::NewInstance()
{
	return CORE_NEW(CCubeTexLoader);
}

CCubeTexLoader::CCubeTexLoader()
{
	m_pOwner = NULL;
	m_pRender = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_pTexture = NULL;
	m_pData = NULL;
	m_nDataSize = 0;
}

CCubeTexLoader::~CCubeTexLoader()
{
	if (m_pData)
	{
		CORE_FREE(m_pData, m_nDataSize);
	}
}

void CCubeTexLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CCubeTexLoader::SetOwner(CTexture* pOwner)
{
	m_pOwner = pOwner;
	m_pRender = pOwner->GetRender();
}

void CCubeTexLoader::SetFileName(const char* file_name)
{
	m_strFileName = file_name;
}

const char* CCubeTexLoader::GetName()
{
	return m_strFileName.c_str();
}

// 文件是否存在
static bool file_exists(const char* name)
{
	// 为了提高查找速度，只在打包的文件中搜索
	return g_pCore->GetFileSys()->FileInPack(name);
}

bool CCubeTexLoader::Load(bool async)
{
	if (file_exists(m_strFileName.c_str()))
	{
		FILE* fp = core_file::fopen(m_strFileName.c_str(), "rb");
		if(fp == NULL)
			return false;
		core_file::fseek(fp, 0, SEEK_END);
		unsigned int dds_size = core_file::ftell(fp);
		core_file::fseek(fp, 0, SEEK_SET);
		
		IFileSys* pFileSys = g_pCore->GetFileSys();
		// 尝试直接获得文件数据，避免临时申请大块内存
		const void* dds_data = pFileSys->FileContent(fp);

		m_nDataSize  = dds_size;

		if (NULL == dds_data)
		{
			m_pData = (unsigned char*)CORE_ALLOC(m_nDataSize);

			if (core_file::fread(m_pData, sizeof(unsigned char), dds_size, fp) 
				!= dds_size)
			{
				CORE_TRACE("(CCubeTexLoader::Load)read tex data failed");
				CORE_TRACE(m_strFileName.c_str());
				core_file::fclose(fp);
				return false;
			}
		}
		core_file::fclose(fp);
		return true;

		//return CreateCubeTextureFromFile(m_strFileName.c_str(), m_pTexture, &m_ImageInfo);
	}
	else
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CCubeLoader::Load)texture %s not found", m_strFileName.c_str());
		CORE_TRACE(info);
		return false;
	}
	return false;
}

bool CCubeTexLoader::Create(bool async)
{
	if (!m_pOwner->BuildCube(m_strFileName.c_str(), m_pData, m_nDataSize))
	{
		return false;
	}

	// 数据所有权已经转移
	return true;
}

bool CCubeTexLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}
