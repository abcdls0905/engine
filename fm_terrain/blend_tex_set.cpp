//--------------------------------------------------------------------
// 文件名:		blend_tex_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "blend_tex_set.h"
#include "blend_tex.h"
#include "terrain.h"
#include "../visual/i_texture.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

// CBlendTexSet

CBlendTexSet::CBlendTexSet()
{
	m_pTerrain = NULL;
}

CBlendTexSet::~CBlendTexSet()
{
	ReleaseAll();
}

void CBlendTexSet::ReleaseAll()
{
	for (size_t i = 0; i < m_Textures.size(); ++i)
	{
		CORE_DELETE(m_Textures[i]);
	}

	m_Textures.clear();
}

bool CBlendTexSet::Find(const char* name) const
{
	size_t index;

	return FindIndex(name, &index);
}

bool CBlendTexSet::FindIndex(const char* name, size_t* index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);
	const size_t SIZE1 = m_Textures.size();

	for (size_t i = 0; i < SIZE1; ++i)
	{
		if ((m_Textures[i]->GetHash() == hash)
			&& (strcmp(m_Textures[i]->GetName(), name) == 0))
		{
			*index = i;
			return true;
		}
	}
	
	return false;
}

bool CBlendTexSet::Add(const char* name, const char* path)
{
	Assert(path != NULL);
	
	if (StringEmpty(name))
	{
		CORE_TRACE("(CBlendTexSet::Add)name empty");
		return false;
	}
	
	if (Find(name))
	{
		CORE_TRACE("(CBlendTexSet::Add)already exists");
		CORE_TRACE(path);
		CORE_TRACE(name);
		return false;
	}

	ITexture* pTex = m_pTerrain->GetRender()->CreateTexture(path, true);

	if (NULL == pTex)
	{
		CORE_TRACE("(CBlendTexSet::Add)create failed");
		CORE_TRACE(path);
		CORE_TRACE(name);
		return false;
	}

	CBlendTex* p = CORE_NEW(CBlendTex);
	
	p->SetName(name);
	p->SetHash(GetHashValueCase(name));
	p->SetPath(path);
	p->SetTex(pTex);

	if (m_pTerrain->GetEnableNormalMap())
	{
		// 尝试加载法线贴图
		char fname[256];
		const char* dot = strrchr(path, '.');

		if (NULL == dot)
		{
			SafeSprintf(fname, sizeof(fname), "%s_n", path);
		}
		else
		{
			char buf[256];
			ptrdiff_t len = dot - path;

			memcpy(buf, path, len);
			buf[len] = 0;
			SafeSprintf(fname, sizeof(fname), "%s_n%s", buf, dot);
		}

		ITexture* pNormalMap = m_pTerrain->GetRender()->CreateTexture(
			fname, true);

		if (pNormalMap)
		{
			p->SetNormalMap(pNormalMap);
		}
	}

	if (m_pTerrain->GetEnableTessellation())
	{
		// 尝试加载高度贴图
		char fname[256];
		const char* dot = strrchr(path, '.');

		if (NULL == dot)
		{
			SafeSprintf(fname, sizeof(fname), "%s_h", path);
		}
		else
		{
			char buf[256];
			ptrdiff_t len = dot - path;

			memcpy(buf, path, len);
			buf[len] = 0;
			SafeSprintf(fname, sizeof(fname), "%s_h%s", buf, dot);
		}

		ITexture* pHeightMap = m_pTerrain->GetRender()->CreateTexture(
			fname, true);

		if (pHeightMap)
		{
			p->SetHeightMap(pHeightMap);
		}
	}

	// 添加样式0
	p->AddFormat(100, 100, 0, 0, 0);
	p->UpdateMatrix(p->GetFormatCount() - 1, m_pTerrain);

	m_Textures.push_back(p);
	
	return true;
}

bool CBlendTexSet::GetLoadFinish()
{
	for (size_t i = 0; i < m_Textures.size(); ++i)
	{
		CBlendTex* pBlendTex = m_Textures[i];
		ITexture* pDiffuseMap = pBlendTex->GetTex();
		ITexture* pNormalMap = pBlendTex->GetNormalMap();
		ITexture* pHeightMap = pBlendTex->GetHeightMap();

		if (pDiffuseMap)
		{
			if (!pDiffuseMap->IsLoadComplete())
			{
				return false;
			}
		}

		if (pNormalMap)
		{
			if (!pNormalMap->IsLoadComplete())
			{
				return false;
			}
		}
		
		if (pHeightMap)
		{
			if (!pHeightMap->IsLoadComplete())
			{
				return false;
			}
		}
	}

	return true;
}
