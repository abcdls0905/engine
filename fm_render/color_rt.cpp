//--------------------------------------------------------------------
// 文件名:		color_rt.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "color_rt.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

CColorRT::CColorRT(Render* pRender, unsigned int width,
	unsigned int height, TEX_FORMAT format, bool auto_gen_mipmap, int type)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nType = type;
	m_bDeviceSize = false;
	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_bAutoGenMipmap = auto_gen_mipmap;
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

CColorRT::~CColorRT()
{
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}
}

void CColorRT::Destroy()
{
	CORE_DELETE(this);
}

void CColorRT::Release()
{
	SAFE_RELEASE(m_pTexSampler);
	m_pRender->ReleaseResource(m_nIndex);
}

bool CColorRT::Create()
{
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}
	
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
	case TEX_FORMAT_RGB888_UNORM:
        //检测一下设备是否支持
		if(!m_pRender->GetDeviceCaps()->IsRGB888_RGBA8888Supported())
		{
            CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_RGB888_UNORM!");
			return false;
		}

		internalformat = GL_RGB;
		format = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		break;
	case TEX_FORMAT_RGBA8888_UNORM:
        //检测一下设备是否支持
		if(!m_pRender->GetDeviceCaps()->IsRGBA8888Supported())
		{
            CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_RGBA8888_UNORM!");
			return false;
		}
		internalformat = GL_RGBA;
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case TEX_FORMAT_RGB_DEFAULT:
	case TEX_FORMAT_RGB565_UNORM:
		internalformat = GL_RGB;
		format = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case TEX_FORMAT_RGBA16161616F_FLOAT:
        //检测一下设备是否支持
		if(!m_pRender->GetDeviceCaps()->IsTextureFloat16ColorBufferSupported())
		{
            CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_RGBA16161616F_FLOAT!");
			return false;
		}
		internalformat = GL_RGBA;
		format = GL_RGBA;
#ifdef RENDER_ES_3_0
		type = GL_HALF_FLOAT;
#else
		type = GL_HALF_FLOAT_OES;
#endif
		break;
	case TEX_FORMAT_RGB161616_FLOAT:
        //检测一下设备是否支持
		if(!m_pRender->GetDeviceCaps()->IsTextureFloat16ColorBufferSupported())
		{
            CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_RGB161616_FLOAT!");
			return false;
		}
		internalformat = GL_RGB;
		format = GL_RGB;
#ifdef RENDER_ES_3_0
		type = GL_HALF_FLOAT;
#else
		type = GL_HALF_FLOAT_OES;
#endif
		break;

	case TEX_FORMAT_RGBA4444_UNORM:
		{
			internalformat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_SHORT_4_4_4_4;
			break;
		}
		break;
	case TEX_FORMAT_R16_FLOAT:
        //检测一下设备是否支持
		if(!m_pRender->GetDeviceCaps()->IsTextureFloat16ColorBufferSupported())
		{
            CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_R16_FLOAT!");
			return false;
		}

		internalformat = GL_LUMINANCE;
		format = GL_LUMINANCE;
#ifdef RENDER_ES_3_0
		type = GL_HALF_FLOAT;
#else
		type = GL_HALF_FLOAT_OES;
#endif
		break;
	case TEX_FORMAT_R32_FLOAT:
        //检测一下设备是否支持
		if(!m_pRender->GetDeviceCaps()->IsTextureFloat32ColorBufferSupported())
		{
            CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_R32_FLOAT!");
			return false;
		}
		internalformat = GL_LUMINANCE;
		format = GL_LUMINANCE;
		type = GL_FLOAT;
		break;
	default:
		CORE_TRACE("Only Supported TEX_FORMAT_RGB_DEFAULT TEX_FORMAT_RGBA8888_UNORM COLOR_RT TEX_FORMAT_RGB888_UNORM TEX_FORMAT_R16_FLOAT And TEX_FORMAT_RGB565_UNORM");
		return false;
		break;
	};

	glGenTextures(1,&m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
	// 设置采样器
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(TestErr("CColorRT::Create() false"))
	{
		return false;
	}

	m_pTexSampler = m_pRender->CreateTextureSamper(this,ITextureSampler::TEX_2D,ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR,
		ITextureSampler::TWM_CLAMP_TO_EDGE,ITextureSampler::TWM_CLAMP_TO_EDGE);
 
	return true;
}
