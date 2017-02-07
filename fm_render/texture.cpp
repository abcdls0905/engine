//--------------------------------------------------------------------
// 文件名:		texture.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "texture.h"
#include "tex_loader.h"
#include "cube_tex_loader.h"
#include "render.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "static_tex.h"

// CTexture

CTexture* CTexture::NewInstance(Render* pRender,IShaderTex* pTempShaderTex)
{
	CTexture* p = (CTexture*)CORE_ALLOC(sizeof(CTexture));

	new (p) CTexture(pRender,pTempShaderTex);

	return p;
}

CTexture::CTexture(Render* pRender,IShaderTex* pTempShaderTex)
{
	Assert(pRender != NULL);

	m_datasize = 0;
	m_eglSync = 0;
	m_pRender = pRender;
	m_pLoader = NULL;
	m_bOriginSize = false;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = TEX_FORMAT_NONE;
	m_nState = 0;
	m_pShaderTex = NULL;
	m_async = false;
	m_pTempShaderTex = pTempShaderTex;
}

CTexture::~CTexture()
{
	m_pRender->GetMemPerformance()->nUsedTextureData -= m_datasize;
	m_pRender->GetMemPerformance()->nUsedTextureDataGui -= m_datasize;
	SAFE_RELEASE(m_pShaderTex);
	SAFE_RELEASE(m_pLoader);
}

void CTexture::Release()
{
	m_pRender->ReleaseTexture(this);
}

void CTexture::Destroy()
{
	CORE_DELETE(this);
}

bool CTexture::Create()
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

bool CTexture::CanAsync()
{
	return true;
}

bool CTexture::BeginCreate()
{
	Assert(m_pLoader == NULL);

	char fname[256];
	
	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(m_strName.c_str()), m_strName.c_str());

	CTexLoader* p = CTexLoader::NewInstance();
	
	p->SetOwner(this);
	p->SetFileName(fname);
	p->SetOriginSize(m_bOriginSize);

	m_pLoader = p;
	
	return true;
}

bool CTexture::CreateCube()
{
	if (!BeginCreateCube())
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

bool CTexture::BeginCreateCube()
{
	Assert(m_pLoader == NULL);

	char fname[256];
	
	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(m_strName.c_str()), m_strName.c_str());
	
	CCubeTexLoader* p = CCubeTexLoader::NewInstance();
	
	p->SetOwner(this);
	p->SetFileName(fname);

	m_pLoader = p;
	
	return true;
}

bool CTexture::CreateNew(unsigned int width, unsigned int height, 
	unsigned int levels, TEX_FORMAT format, const void* tex_data, 
	unsigned int data_size)
{
	IStaticTex* pStaticTex = m_pRender->CreateStaticTex(width, height,
		levels, format, tex_data, data_size);

	if (NULL == pStaticTex)
	{
		return false;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nOriginWidth = width;
	m_nOriginHeight = height;
	m_nFormat = format;
	m_pShaderTex = pStaticTex;
	
	return true;
}

bool CTexture::CreateDynamic(unsigned int width, unsigned int height, 
	unsigned int levels, TEX_FORMAT format)
{
	IDynamicTex* pDynamicTex = m_pRender->CreateDynamicTex(width, height,
		levels, format);

	if (NULL == pDynamicTex)
	{
		return false;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nOriginWidth = width;
	m_nOriginHeight = height;
	m_nFormat = format;
	m_pShaderTex = pDynamicTex;
	
	return true;
}

bool CTexture::CreateRenderTarget(unsigned int width, unsigned int height,
	TEX_FORMAT format)
{
	IColorRT* pColorRT = m_pRender->CreateColorRT(width, height, format,
		RT_TYPE_FIXED);

	if (NULL == pColorRT)
	{
		return false;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nOriginWidth = width;
	m_nOriginHeight = height;
	m_nFormat = format;
	m_pShaderTex = pColorRT;
	
	return true;
}

bool CTexture::EndCreate(int category, bool succeed)
{
	SAFE_RELEASE(m_pLoader);
	
	if (succeed)
	{
		// 设置资源就绪
		SetState(STATE_READY);
	}
	else
	{
		SetState(STATE_FAILED);
	}

	return true;
}

IResLoader* CTexture::GetLoader()
{
	return m_pLoader;
}

bool CTexture::Build(unsigned int width, unsigned int height, unsigned int origin_width, unsigned int origin_height, TEX_FORMAT format, fm_uint tex)
{
	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_nOriginWidth = origin_width;
	m_nOriginHeight = origin_height;
	
	if (NULL == m_pShaderTex)
	{
		m_pShaderTex = m_pRender->CreateStaticTexFrom(tex);
	}
	else
	{
		SAFE_RELEASE(m_pShaderTex);
	}
	
	return true;
}

// 获取可用贴图(此接口为了模拟D3D的设置空纹理方式,采用默认1X1的一个临时纹理作为代替纹理)
IShaderTex* CTexture::GetCanUseShaderTex()
{
	if(IsReady()&&m_pShaderTex)
	{
		return m_pShaderTex;
	} 
	return m_pTempShaderTex;
}

bool CTexture::Build(const char* file_name, const void* data, unsigned int data_size, const void* data_mask, unsigned int data_mask_size)
{
	fm_uint texture;
	_IMAGE_INFO info;
	if(data_mask)
	{
		if(!CreateTextureFromData(file_name, data, data_size, data_mask,  data_mask_size, texture, &info))
			return false;
	}
	else 
	{
		if(!CreateTextureFromData(file_name, data, data_size, texture, &info))
			return false;
	}

	m_datasize += info.DataSize;
	m_pRender->GetMemPerformance()->nUsedTextureData += info.DataSize;

	int size = 0;//m_TraceTexture.Create(file_name, data, data_size);
	m_pRender->GetMemPerformance()->nUsedTextureData += size;
	m_datasize += size;
	
	if(m_bOriginSize)
		m_pRender->GetMemPerformance()->nUsedTextureDataGui += m_datasize;
	m_nWidth = info.Width;
	m_nHeight = info.Height;
	m_nOriginWidth = info.OriginWidth;
	m_nOriginHeight = info.OriginHeight;
	m_nFormat = info.Format;

	if (NULL == m_pShaderTex)
	{
		m_pShaderTex = m_pRender->CreateStaticTexFrom(texture);
		((CStaticTex*)m_pShaderTex)->SetTexBuilder(this);
	}
	else
	{
		SAFE_RELEASE(m_pShaderTex);
	}

	return true;
}

bool CTexture::BuildCube(unsigned int width, unsigned int height, TEX_FORMAT format, fm_uint tex)
{
	m_nWidth = width;
	m_nHeight = height;
	m_nOriginWidth = width;
	m_nOriginHeight = height;
	m_nFormat = format;
	
	if (tex)
	{
		m_pShaderTex = m_pRender->CreateStaticCubeFrom(tex);
	}
	else
	{
		SAFE_RELEASE(m_pShaderTex);
	}
	return true;
}

bool CTexture::BuildCube(const char* file_name, const void* data, unsigned int data_size)
{
	fm_uint tex;
	_IMAGE_INFO info;
	
	if(!CreateCubeTextureFromFile(file_name, data, data_size, tex, &info))
		return false;

	if (tex)
	{
		m_pShaderTex = m_pRender->CreateStaticCubeFrom(tex);
	}
	else
	{
		SAFE_RELEASE(m_pShaderTex);
	}

	m_nWidth = info.Width;
	m_nHeight = info.Height;
	m_nFormat = info.Format;

	return true;
}

bool CTexture::Build(fm_uint& texture)
{
	char fname[256];
	
	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(m_strName.c_str()), m_strName.c_str());

	_IMAGE_INFO info;
	if(!CreateTextureFromFile(fname, texture, &info))
		return false;
	return true;
}

// 获得测试透明度的贴图
unsigned int CTexture::GetTextureAlpha(float u, float v)
{
	return m_TraceTexture.GetAlpha(u, v, m_nOriginWidth, m_nOriginHeight); 
}