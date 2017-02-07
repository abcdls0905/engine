
//--------------------------------------------------------------------
// 文件名:		color_tex.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月6日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "depth_tex.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"


CDepthTex::CDepthTex(Render* pRender, unsigned int width, unsigned int height, TEX_FORMAT format, int type)
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

CDepthTex::~CDepthTex()
{
	if(m_nRenderTex)
	{
		glDeleteRenderbuffers(1, &m_nRenderTex);
	}
}

// 释放
void CDepthTex::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

// 创建
bool CDepthTex::Create()
{
	fm_int internalformat;
	//根据类型创建模板格式
	switch(m_nFormat)
	{
	case TEX_FORMAT_D_DEFAULT:
#ifdef __ANDROID__
		//Tergra专属优化方案
		if(m_pRender->GetDeviceCaps()->IsTegraGPU())
		{
			internalformat = GL_DEPTH_COMPONENT16;
		}
		else
		{
	#ifdef RENDER_ES_3_0
				internalformat = GL_DEPTH_COMPONENT16;
	#else
				internalformat = GL_DEPTH_COMPONENT16_NONLINEAR_NV;
	#endif
		}
#else
		internalformat = GL_DEPTH_COMPONENT16;
#endif
		break;
	case TEX_FORMAT_D16_UINT:
		internalformat = GL_DEPTH_COMPONENT16;
		break;
	case TEX_FORMAT_D24_UINT:
 		if(!m_pRender->GetDeviceCaps()->IsDepth24Supported())
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D24_UINT!");
			return false;
		}       
#ifdef RENDER_ES_3_0
        internalformat = GL_DEPTH_COMPONENT24;
#else
        internalformat = GL_DEPTH_COMPONENT24_OES;
#endif
		break;
	case TEX_FORMAT_D32_UINT:
		if(!m_pRender->GetDeviceCaps()->IsDepth32Supported())
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D32_UINT!");
			return false;
		}
#ifdef RENDER_ES_3_0
		//internalformat = GL_DEPTH_COMPONENT32;
		return false;
#else
        internalformat = GL_DEPTH_COMPONENT32_OES;
#endif
		break;
	default:
		CORE_TRACE("[DEPTH_TEX] Only Supported TEX_FORMAT_D_DEFAULT TEX_FORMAT_D16_UINT TEX_FORMAT_D32_UINT");
		return false;
		break;
	};

	glGenRenderbuffers(1, &m_nRenderTex);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nRenderTex);

	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_nWidth,m_nHeight);

	if(TestErr("CreateDepth Tex Failed!"))
	{
		return false;
	}

	return true;
}

void CDepthTex::Destroy()
{
	CORE_DELETE(this);
}