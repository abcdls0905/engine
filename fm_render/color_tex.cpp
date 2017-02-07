//--------------------------------------------------------------------
// 文件名:		color_tex.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月6日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "color_tex.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

CColorTex::CColorTex(Render* pRender, unsigned int width,
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
	m_nRenderTex = 0;
	
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

CColorTex::~CColorTex()
{
	if (m_nRenderTex)
	{
		glDeleteRenderbuffers(1, &m_nRenderTex);
	}
}

void CColorTex::Destroy()
{
	CORE_DELETE(this);
}

void CColorTex::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CColorTex::Create()
{
	if (m_nRenderTex)
	{
		glDeleteRenderbuffers(1, &m_nRenderTex);
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

	switch(m_nFormat)
	{
	case TEX_FORMAT_RGB_DEFAULT:
		internalformat = GL_RGB565;
		break;
	case TEX_FORMAT_RGBA_DEFAULT:
		internalformat = GL_RGB5_A1;
		break;
	case TEX_FORMAT_RGB565_UNORM:
		internalformat = GL_RGB565;
		break;
	case TEX_FORMAT_RGBA8888_UNORM:
#ifdef RENDER_ES_3_0
		internalformat = GL_RGBA8;
#else
		internalformat = GL_RGBA8_OES;
#endif
		break;
	case TEX_FORMAT_RGB888_UNORM:
#ifdef RENDER_ES_3_0
		internalformat = GL_RGB;
#else
		internalformat = GL_RGB8_OES;
#endif
		break;
	default:
		CORE_TRACE("[CColorTex] Only Supported TEX_FORMAT_RGB_DEFAULT TEX_FORMAT_RGBA_DEFAULT TEX_FORMAT_RGBA8888_UNORM TEX_FORMAT_RGB888_UNORM TEX_FORMAT_RGB565_UNORM");
		return false;
		break;
	};

	glGenRenderbuffers(1,&m_nRenderTex);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nRenderTex);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_nWidth,m_nHeight);
	if(TestErr("CreateColor Tex Failed!"))
	{
		return false;
	}

	return true;
}
