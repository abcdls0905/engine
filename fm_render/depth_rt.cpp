//--------------------------------------------------------------------
// 文件名:		depth_rt.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "depth_rt.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

CDepthRT::CDepthRT(Render* pRender, unsigned int width,
	unsigned int height, TEX_FORMAT format, int type)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nType = type;
	m_bDeviceSize = false;
	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_nTexture = 0;
	m_pTexSampler = NULL;

	
	if (type == RT_TYPE_DEVICE)
	{
		int dev_width = m_pRender->GetDeviceWidth();
		int dev_height = m_pRender->GetDeviceHeight();

		if ((width == dev_width) && (height == dev_height))
		{
			m_bDeviceSize = true;
			m_dWidthRatio = 1.0;
			m_dHeightRatio = 1.0;
		}
		else
		{
			m_dWidthRatio = double(width) / double(dev_width);
			m_dHeightRatio = double(height) / double(dev_height);
		}
	}
	else
	{
		m_dWidthRatio = 0.0;
		m_dHeightRatio = 0.0;
	}
}

CDepthRT::~CDepthRT()
{
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}
}

void CDepthRT::Destroy()
{
	CORE_DELETE(this);
}

void CDepthRT::Release()
{
	SAFE_RELEASE(m_pTexSampler);
	m_pRender->ReleaseResource(m_nIndex);
}

bool CDepthRT::Create()
{
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}

	SAFE_RELEASE(m_pTexSampler);

	int width;
	int height;

	if (m_nType == RT_TYPE_DEVICE)
	{
		int dev_width = m_pRender->GetDeviceWidth();
		int dev_height = m_pRender->GetDeviceHeight();

		if (m_bDeviceSize)
		{
			// 使用当前设备的尺寸
			width = dev_width;
			height = dev_height;
		}
		else
		{
			// 保持与设备尺寸的比例
			width = int(double(dev_width) * m_dWidthRatio);
			height = int(double(dev_height) * m_dHeightRatio);
		}
	}
	else
	{
		width = m_nWidth;
		height = m_nHeight;
	}

	fm_int internalformat;
	GLenum format;
	GLenum type;

	switch(m_nFormat)
	{
	case TEX_FORMAT_D16_UINT:
		if(!m_pRender->GetDeviceCaps()->IsDepthTextureSupported())
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D16_UINT!");
			return false;
		}

		internalformat = GL_DEPTH_COMPONENT;
        format = GL_DEPTH_COMPONENT;
		type = GL_UNSIGNED_SHORT;
		break;
	case TEX_FORMAT_D24_UINT:
		if(!m_pRender->GetDeviceCaps()->IsDepthTextureSupported() || !m_pRender->GetDeviceCaps()->IsDepth24Supported())
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D24_UINT!");
			return false;
		}

		internalformat = GL_DEPTH_COMPONENT;
        format = GL_DEPTH_COMPONENT;
		type = GL_UNSIGNED_INT;
		break;
	case TEX_FORMAT_D32_UINT:
		if(!m_pRender->GetDeviceCaps()->IsDepthTextureSupported() || !m_pRender->GetDeviceCaps()->IsDepth32Supported())
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D32_UINT!");
			return false;
		}

		internalformat = GL_DEPTH_COMPONENT;
        format = GL_DEPTH_COMPONENT;
		type = GL_UNSIGNED_INT;
		break;
	case TEX_FORMAT_D_DEFAULT:

		if(!m_pRender->GetDeviceCaps()->IsDepthTextureSupported())
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D_DEFAULT!");
			return false;
		}

		internalformat = GL_DEPTH_COMPONENT;
        format = GL_DEPTH_COMPONENT;
		type = GL_UNSIGNED_SHORT;
		break;
	default:
		CORE_TRACE("Unknow DepthRT Format Only Supported TEX_FORMAT_D_DEFAULT TEX_FORMAT_D16_UINT TEX_FORMAT_D24_UINT TEX_FORMAT_D32_UINT");
		return false;
		break;
	};


	//深度模板分离的
	glGenTextures(1,&m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
	// Set the textures parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if(TestErr("CDepthRT::Create() false"))
	{
		return false;
	}

	m_pTexSampler = m_pRender->CreateTextureSamper(this,ITextureSampler::TEX_2D,ITextureSampler::TS_MAG_NEAREST,ITextureSampler::TS_MIN_NEAREST,
		ITextureSampler::TWM_CLAMP_TO_EDGE,ITextureSampler::TWM_CLAMP_TO_EDGE);

	return true;
}
