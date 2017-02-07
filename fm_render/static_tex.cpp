//--------------------------------------------------------------------
// 文件名:		static_tex.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年12月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include <assert.h>
#include "static_tex.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "texture.h"


CStaticTex::CStaticTex(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nLevels = 0;
	m_nFormat = TEX_FORMAT_NONE; 
	m_nTexture = 0;
	m_pTexSampler = NULL;
	m_pTexBuilder = NULL;

	m_pdata = NULL;
}

CStaticTex::~CStaticTex()
{
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}

	if(m_pdata)
	{
		CORE_FREE(m_pdata, m_data_size);
	}
}

void CStaticTex::Destroy()
{
	CORE_DELETE(this);
}

void CStaticTex::Release()
{
 	SAFE_RELEASE(m_pTexSampler);
	m_pRender->ReleaseResource(m_nIndex);
}

void CStaticTex::Build(unsigned int width, unsigned int height, 
	unsigned int levels, TEX_FORMAT format, const void* pdata,
	unsigned int data_size)
{
	GLenum eTextureFormat = 0;
    GLenum eTextureType = 0;
	GLenum eTextureInternalFormat = 0;	
	unsigned int nBitCount = 0; //number of bits per 
	switch(format)
	{
	case TEX_FORMAT_RGBA4444_UNORM:
		{
			eTextureInternalFormat = eTextureFormat = GL_RGBA;
			eTextureType = GL_UNSIGNED_SHORT_4_4_4_4;
			nBitCount = 16;
		}
		break;
	case TEX_FORMAT_RGB888_UNORM:
		{
			eTextureInternalFormat = eTextureFormat = GL_RGB;
			eTextureType = GL_UNSIGNED_BYTE;
			nBitCount = 24;
		}
		break;
	case TEX_FORMAT_RGBA8888_UNORM:
		{
			eTextureInternalFormat = eTextureFormat = GL_RGBA;
			eTextureType = GL_UNSIGNED_BYTE;
			nBitCount = 32;
		}
		break;
	case TEX_FORMAT_RGBA16161616F_FLOAT:
		{
			eTextureInternalFormat = eTextureFormat = GL_RGBA;
#ifdef RENDER_ES_3_0
			eTextureType = GL_HALF_FLOAT;
#else
			eTextureType = GL_HALF_FLOAT_OES;
#endif
			nBitCount = 64;
		}
		break;
	case TEX_FORMAT_RGBA16161616_SNORM:
		{
			eTextureInternalFormat = eTextureFormat = GL_RGBA;
			eTextureType = GL_FLOAT;
			nBitCount = 64;
		}
		break;
	default:
		{
			CORE_TRACE("CStaticTex  Only Support! TEX_FORMAT_RGBA4444_UNORM,TEX_FORMAT_RGB888_UNORM,TEX_FORMAT_RGBA8888_UNORM,TEX_FORMAT_RGBA16161616F_FLOAT,TEX_FORMAT_RGBA16161616_SNORM");
		}
		break;
	};

	const char* theTexturePtr = (const char*)pdata;
//	const void* theTextureToLoad = 0;
	glGenTextures(1,&m_nTexture);
    glBindTexture(GL_TEXTURE_2D, m_nTexture);

    unsigned int nSizeX = width ;
	unsigned int nSizeY = height;

	for(unsigned int nMIPMapLevel = 0; nMIPMapLevel < levels; nSizeX=FM_MAX(nSizeX/2, (unsigned int)1), nSizeY=FM_MAX(nSizeY/2, (unsigned int)1), nMIPMapLevel++)
	{
		glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel,eTextureInternalFormat,nSizeX,nSizeY,0,eTextureFormat,eTextureType,theTexturePtr);
        theTexturePtr += (nSizeX * nSizeY * nBitCount + 7) / 8;
	}

#ifdef RENDER_ES_3_0
	// 如果是浮点纹理
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT)
#else
	// 如果是浮点纹理
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT_OES)
#endif
	{
		if(levels>0)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
	else
	{
		if(levels>1)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	//如果尺寸不是2的指数
	if( (width & (width-1)) | (height & (height -1) ) )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}

bool CStaticTex::Create(unsigned int width, unsigned int height, 
	unsigned int levels, TEX_FORMAT format, const void* pdata,
	unsigned int data_size)
{
	m_pdata = CORE_ALLOC(data_size);
	memcpy(m_pdata, pdata, data_size);
	m_data_size = data_size;

 //   GLenum eTextureFormat = 0;
 //   GLenum eTextureType = 0;
//	GLenum eTextureInternalFormat = 0;	// often this is the same as textureFormat, but not for BGRA8888 on the iPhone, for instance
//	unsigned int nBitCount = 0; //number of bits per 

	m_nWidth = width;
	m_nHeight = height;
	m_nLevels = levels;
    m_nFormat = format;
	
	Build(this->m_nWidth, this->m_nHeight, this->m_nLevels, this->m_nFormat, pdata, data_size);

	m_pTexSampler = m_pRender->CreateTextureSamper(this,ITextureSampler::TEX_2D);//,tex_mag,tex_min,u_wrap_mode,v_wrap_mode);
	
	return true;
}

bool CStaticTex::CreateFrom(fm_uint nTex)
{
	Assert(nTex != 0);

	m_nTexture = nTex;
    m_pTexSampler = m_pRender->CreateTextureSamperFromShaderTex(this,ITextureSampler::TEX_2D);
	return true;
}

bool CStaticTex::Restore()
{
	if(m_pTexBuilder)
	{
		m_pTexBuilder->Build(m_nTexture);
	}
	else
	{
		Build(this->m_nWidth, this->m_nHeight, this->m_nLevels, this->m_nFormat, this->m_pdata, this->m_data_size);
	}

//	m_pTexSampler->Restore();

	return true;
}

bool CStaticTex::Invalidate()
{
	if(m_nTexture)
	{
		::glDeleteTextures(1, &m_nTexture);
		m_nTexture = 0;
	}

	return true;
}