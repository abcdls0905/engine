
//--------------------------------------------------------------------
// �ļ���:		depthstencil_tex.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��6��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "depthstencil_tex.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"


CDepthStencilTex::CDepthStencilTex(Render* pRender, unsigned int width, unsigned int height, TEX_FORMAT format, int type)
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

CDepthStencilTex::~CDepthStencilTex()
{
	if(m_nRenderTex)
	{
		glDeleteRenderbuffers(1, &m_nRenderTex);
	}
}

void CDepthStencilTex::Destroy()
{
	CORE_DELETE(this);
}

// �ͷ�
void CDepthStencilTex::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

// ����
bool CDepthStencilTex::Create()
{
	fm_int internalformat;
    //�������ʹ���ģ���ʽ
	switch(m_nFormat)
	{
	case TEX_FORMAT_D24S8_UINT:

		if(! m_pRender->GetDeviceCaps()->IsDepth24Stencil8Supported() )
		{
			CORE_TRACE("[Device Warning] Cann't support TEX_FORMAT_D24S8_UINT!");
			return false;
		}
#ifdef RENDER_ES_3_0
		internalformat = GL_DEPTH24_STENCIL8;
#else
		internalformat = GL_DEPTH24_STENCIL8_OES;
#endif
		break;
	default:
		CORE_TRACE("[CDepthStencilTex] Only Supported TEX_FORMAT_D24S8_UINT");
		return false;
		break;
	};

	//����һ�� ģ��
	glGenRenderbuffers(1, &m_nRenderTex);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nRenderTex);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_nWidth,m_nHeight);
	if(TestErr("CreateStencil Tex Failed!"))
	{
		return false;
	}

	return true;
}