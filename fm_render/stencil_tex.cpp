#include "stencil_tex.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

CStencilTex::CStencilTex(Render* pRender,unsigned int width, unsigned int height, TEX_FORMAT format,int type)
{
	m_nRenderTex = 0;
	m_pRender = pRender;
	m_nFormat = format;
	m_nWidth = width;
	m_nHeight = height;

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

CStencilTex::~CStencilTex()
{
	if(m_nRenderTex)
	{
		glDeleteRenderbuffers(1, &m_nRenderTex);
	}
}

// 释放
void CStencilTex::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

// 创建
bool CStencilTex::Create()
{
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
    //根据类型创建模板格式
	switch(m_nFormat)
	{
	case TEX_FORMAT_S_DEFAULT:
		internalformat = GL_STENCIL_INDEX8;//一定支持的格式
		break;
	case TEX_FORMAT_S1_UINT:
		if( !m_pRender->GetDeviceCaps()->IsStencil1Supported() )
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_S1_UINT!");
			return false;
		}

#ifdef RENDER_ES_3_0
		//internalformat = GL_STENCIL_INDEX;
		return false;
#else
	#ifdef FX_SYSTEM_IOS
		//	internalformat = GL_STENCIL_INDEX;
				return false;
	#else 
			internalformat = GL_STENCIL_INDEX1_OES;
	#endif
#endif
		break;
	case TEX_FORMAT_S4_UINT:
		if( !m_pRender->GetDeviceCaps()->IsStencil4Supported() )
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_S1_UINT!");
			return false;
		}

#ifdef RENDER_ES_3_0
		return false;
#else
	#ifdef FX_SYSTEM_IOS
			CORE_TRACE("IOS:[CStencilTex] Only Supported TEX_FORMAT_S_DEFAULT TEX_FORMAT_S1_UINT TEX_FORMAT_S8_UINT");
			return false;
	#else
			internalformat = GL_STENCIL_INDEX4_OES;
	#endif
#endif
		break;
	case TEX_FORMAT_S8_UINT:
		internalformat = GL_STENCIL_INDEX8;
		break;
	default:
		CORE_TRACE("[CStencilTex] Only Supported TEX_FORMAT_S_DEFAULT TEX_FORMAT_S1_UINT TEX_FORMAT_S4_UINT TEX_FORMAT_S8_UINT");
		return false;
		break;
	};

	//创建一张 模板
	glGenRenderbuffers(1, &m_nRenderTex);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nRenderTex);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_nWidth,m_nHeight);
	if(TestErr("CreateStencil Tex Failed!"))
	{
		return false;
	}

	return true;
}

// 销毁
void CStencilTex::Destroy()
{
	CORE_DELETE(this);
}

// 设备就绪时的处理
bool CStencilTex::Restore()
{
	return true;
}

// 设备丢失时的处理
bool CStencilTex::Invalidate()
{
	return true;
}

