//--------------------------------------------------------------------
// 文件名:		tex_factory.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年7月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tex_factory.h"
#include "texture.h"
#include "../public/core_mem.h"
#include <stdio.h>
#include "render.h"

// CTexFactory

CTexFactory* CTexFactory::NewInstance(Render* pRender)
{
	CTexFactory* p = (CTexFactory*)CORE_ALLOC(sizeof(CTexFactory));
	
	new (p) CTexFactory(pRender);

	return p;
}

CTexFactory::CTexFactory(Render* pRender)
{
	m_pRender = pRender;
	m_nSerial = 0;
	m_nCount = 0;
	m_Resources.push_back(NULL);	// 不使用位置0

	m_pTempTex = NULL;
	CreateTempStaticTex();
}

CTexFactory::~CTexFactory()
{
	ReleaseAll();
}

void CTexFactory::ReleaseAll()
{
	for (size_t i = 0; i < m_Resources.size(); ++i)
	{
		if (m_Resources[i] != NULL)
		{
			Destroy(m_Resources[i]);
		}
	}
	
	m_Resources.clear();
	m_Resources.push_back(NULL);
	m_FreeList.clear();
	m_nCount = 0;

	m_pTempTex->Release();
	m_pTempTex = NULL;
}

void CTexFactory::Release()
{
	CORE_DELETE(this);
}

unsigned int CTexFactory::GenSerial()
{
	if (++m_nSerial == 0)
	{
		return ++m_nSerial & 0x7FFFFFFF;
	}
	else
	{
		return m_nSerial & 0x7FFFFFFF;
	}
}

// 默认一个临时可用纹理(模拟D3D采样空纹理方式)
IStaticTex* CTexFactory::CreateTempStaticTex()
{
	unsigned int value = 0x000000FF;
	m_pTempTex = m_pRender->CreateStaticTex(1,1,1,TEX_FORMAT_RGBA8888_UNORM,&value,sizeof(value));
	return m_pTempTex;
}


CTexture* CTexFactory::Create()
{
	CTexture* pTexture = CTexture::NewInstance(m_pRender,m_pTempTex);
	unsigned int serial = GenSerial();
	unsigned int index;
	
	if (m_FreeList.empty())
	{
		index = (unsigned int)m_Resources.size();
		m_Resources.push_back(pTexture);
	}
	else
	{
		index = m_FreeList.back();
		m_FreeList.pop_back();
		m_Resources[index] = pTexture;
	}
	
	++m_nCount;
	pTexture->SetID(PERSISTID(index, serial));

	return pTexture;
}

void CTexFactory::Destroy(CTexture* pResource)
{
	Assert(pResource != NULL);

	size_t index = pResource->GetID().nIdent;
	
	Assert(index < m_Resources.size());
	Assert(m_Resources[index] == pResource);

	pResource->Destroy();
	m_Resources[index] = NULL;
	m_FreeList.push_back(index);
	--m_nCount;
}

CTexture* CTexFactory::Get(const PERSISTID& id) const
{
	size_t index = id.nIdent;

	if (index >= m_Resources.size())
	{
		return NULL;
	}
	
	CTexture* pResource = m_Resources[index];

	if (NULL == pResource)
	{
		return NULL;
	}

	if (pResource->GetID() != id)
	{
		return NULL;
	}

	return pResource;
}

bool CTexFactory::Dump(const char* filename)
{
	Assert(filename != NULL);

	FILE* fp = fopen(filename, "wb");

	if (NULL == fp)
	{
		return false;
	}

	int count = 0;

	for (size_t i = 0; i < m_Resources.size(); ++i)
	{
		CTexture* p = m_Resources[i];

		if (NULL == p)
		{
			continue;
		}

		fprintf(fp, "texture name:%s width:%d height:%d format:%d\r\n",
			p->GetName(), p->GetWidth(), p->GetHeight(), (int)p->GetFormat());
		count++;
	}
	
	fprintf(fp, "total %d textures\r\n", count);
	fclose(fp);

	return true;
}
