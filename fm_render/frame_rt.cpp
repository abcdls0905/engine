//--------------------------------------------------------------------
// �ļ���:		frame.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��4��2��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------


#include "frame_rt.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

CFrameRT::CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = pColorRT;
	m_pDepthRT = pDepthRT;

	m_pStencilTex = pStencilTex;
	m_nFrame = 0;

	m_nBindColorState = IFrameRT::BIND_RT;
	m_nBindDepthState = IFrameRT::BIND_RT;

	if(pStencilTex)
	{
		m_nBindStencilState = IFrameRT::BIND_TEX;
	}
	else
	{
		m_nBindStencilState = IFrameRT::BIND_NONE;
	}

	m_bIsBindDepthStencil = false;
}

CFrameRT::CFrameRT(Render* pRender, IColorTex* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = 0;
	m_pDepthRT = pDepthRT;

	m_pColorTex = pColorRT;
	m_pDepthTex = NULL;
	m_pStencilTex = pStencilTex;
	m_nFrame = 0;

	m_nBindColorState = IFrameRT::BIND_TEX;
	m_nBindDepthState = IFrameRT::BIND_RT;

	if(pStencilTex)
	{
		m_nBindStencilState = IFrameRT::BIND_TEX;
	}
	else
	{
		m_nBindStencilState = IFrameRT::BIND_NONE;
	}
	m_bIsBindDepthStencil = false;
}


// ��������ǰ���ϵķ�ʽ������
CFrameRT::CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = pColorRT;
	m_pDepthRT = NULL;

	m_nFrame = 0;

	m_pDepthTex = pDepthTex;

	m_pStencilTex = pStencilTex;

	m_nBindColorState = IFrameRT::BIND_RT;
	m_nBindDepthState = IFrameRT::BIND_TEX;

	if(pStencilTex)
	{
		m_nBindStencilState = IFrameRT::BIND_TEX;
	}
	else
	{
		m_nBindStencilState = IFrameRT::BIND_NONE;
	}

	m_bIsBindDepthStencil = false;
}

CFrameRT::CFrameRT(Render* pRender, IColorTex* pColorRT,IDepthStencilTex* pDepthStencilTex)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = 0;
	m_pDepthRT = NULL;

	m_nFrame = 0;

	m_pColorTex = pColorRT;
	m_pDepthTex = NULL;
	m_pStencilTex = NULL;
	m_pDepthStencilTex = pDepthStencilTex;

	m_nBindColorState = IFrameRT::BIND_TEX;
	m_nBindDepthState = IFrameRT::BIND_TEX;
	m_nBindStencilState = IFrameRT::BIND_TEX;

	m_bIsBindDepthStencil = false;
}

// 
CFrameRT::CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthStencilTex* pDepthStencilTex)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = pColorRT;
	m_pDepthRT = NULL;

	m_nFrame = 0;

	m_pDepthTex = NULL;
	m_pStencilTex = NULL;
	m_pDepthStencilTex = pDepthStencilTex;
	m_bIsBindDepthStencil = true;

	m_nBindColorState = IFrameRT::BIND_RT;
	m_nBindDepthState = IFrameRT::BIND_TEX;
	m_nBindStencilState = IFrameRT::BIND_TEX;
}

CFrameRT::CFrameRT(Render* pRender, IColorRT* pColorRT)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = pColorRT;
	m_pDepthRT = NULL;

	m_nFrame = 0;

	m_pDepthTex = NULL;
	m_pStencilTex = NULL;
	m_pDepthStencilTex = NULL;
	m_bIsBindDepthStencil = false;

	m_nBindColorState = IFrameRT::BIND_RT;
	m_nBindDepthState = IFrameRT::BIND_NONE;
	m_nBindStencilState = IFrameRT::BIND_NONE;
}

CFrameRT::CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_pColorRT = pColorRT;
	m_pDepthRT = NULL;

	m_nFrame = 0;

	m_pDepthTex = NULL;
	m_pStencilTex = NULL;
	m_pDepthStencilTex = NULL;

	m_pDepthStencilRT = pDepthStencilRT;
	m_bIsBindDepthStencil = true;

	m_nBindColorState = IFrameRT::BIND_RT;
	m_nBindDepthState = IFrameRT::BIND_RT;
	m_nBindStencilState = IFrameRT::BIND_RT;
}


CFrameRT::~CFrameRT()
{
	if(m_nFrame)
	{
		glDeleteFramebuffers(1, &m_nFrame);
	}
}

// ��ȡIColorRT
IColorRT* CFrameRT::GetColorRT()
{
	return m_pColorRT;
}

// ��ȡIDepthRT
IDepthRT* CFrameRT::GetDepthRT()
{
	return m_pDepthRT;
}

// ��ȡ�󶨵�IColorTex
IColorTex* CFrameRT::GetColorTex()
{
	return m_pColorTex;
}

// ��ȡ�󶨵�DepthTex
IDepthTex* CFrameRT::GetDepthTex()
{
	return m_pDepthTex;
}

// ��ȡ�󶨵�ģ��
IStencilTex* CFrameRT::GetStencilTex()
{
	return m_pStencilTex;
}


// ����IColorRT
bool CFrameRT::SetColorRT(IColorRT* pColorRT) 
{
	if( m_pColorRT == pColorRT)
		return true;
	return inSetColorRT(pColorRT);
}

// ����IDepthRT
bool CFrameRT::SetDepthRT(IDepthRT* pDepthRT)
{
	if( m_pDepthRT == pDepthRT )
		return true;
    return inSetDepthRT(pDepthRT);
}

bool CFrameRT::SetColorTex(IColorTex* pColorTex)
{
	if( m_pColorTex == pColorTex )
		return true;
	return inSetColorTex(pColorTex);
}

bool CFrameRT::SetDepthTex(IDepthTex* pDepthTex)
{
	if( m_pDepthTex == pDepthTex )
		return true;

	return inSetDepthTex(pDepthTex);
}

bool CFrameRT::SetStencilTex(IStencilTex* pStencil)
{
	if( m_pStencilTex == pStencil )
		return true;

	return inSetStencilTex(pStencil);
}

bool CFrameRT::SetDepthStencilTex(IDepthStencilTex* pDepthStencilTex)
{
	if( m_pDepthStencilTex == pDepthStencilTex )
		return true;

	return inSetDepthStencilTex(pDepthStencilTex);
}

bool CFrameRT::SetDepthStencilRT(IDepthStencilRT* pDepthStencilRT)
{
	if( m_pDepthStencilRT == pDepthStencilRT )
		return true;

   return inSetDepthStencilRT(pDepthStencilRT);
}

// ��ȡ�󶨵����ģ����
IDepthStencilTex* CFrameRT::GetDepthStencilTex()
{
	return m_pDepthStencilTex;
}

// ��ȡ�󶨵����ģ��
IDepthStencilRT* CFrameRT::GetDepthStencilRT()
{
	return m_pDepthStencilRT;
}

// ����IColorRT
bool CFrameRT::inSetColorRT(IColorRT* pColorRT) 
{
	//֮ǰ�Ѿ����ù���

	fm_int nfbo;
	// ����ɵ�fbo
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFrame);
	m_pColorRT = pColorRT;
	// ��ʼ��framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pColorRT->GetTexture(), 0);
	if(TestFrameErr("SetColorRT CFrameRT Failed! glFramebufferTexture2D GL_COLOR_ATTACHMENT0"))
	{
		glBindFramebuffer(GL_RENDERBUFFER, nfbo);
		return false;
	}

	m_nBindColorState = IFrameRT::BIND_RT;
	return true;
}

// ����IDepthRT
bool CFrameRT::inSetDepthRT(IDepthRT* pDepthRT)
{
	fm_int nfbo;
	// ����ɵ�fbo
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFrame);
	m_pDepthRT = pDepthRT;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_pDepthRT->GetTexture(), 0);
	if(TestFrameErr("SetDepthRT CFrameRT Failed! glFramebufferTexture2D GL_DEPTH_ATTACHMENT"))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
		return false;
	}

	m_nBindDepthState = IFrameRT::BIND_RT;
	m_bIsBindDepthStencil = false;
	return true;
}

bool CFrameRT::inSetColorTex(IColorTex* pColorTex)
{
	fm_int nrbo;
	// ����ɵ�rbo
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &nrbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_pColorTex->GetRenderTex());
	m_pColorTex = pColorTex;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_pColorTex->GetRenderTex());
	if(TestErr("SetColorTex glFramebufferRenderbuffer GL_COLOR_ATTACHMENT0"))
	{
		glBindRenderbuffer(GL_RENDERBUFFER, nrbo);
		return false;
	}
	m_nBindColorState = IFrameRT::BIND_TEX;
	return true;
}

bool CFrameRT::inSetDepthTex(IDepthTex* pDepthTex)
{
	fm_int nrbo;
	// ����ɵ�rbo
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &nrbo);
  //glBindRenderbuffer(GL_RENDERBUFFER, m_pColorTex->GetRenderTex());
  glBindRenderbuffer(GL_RENDERBUFFER, pDepthTex->GetRenderTex());
	m_pDepthTex = pDepthTex;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pDepthTex->GetRenderTex());
	if(TestErr("SetDepthTex glFramebufferRenderbuffer GL_DEPTH_ATTACHMENT"))
	{
		glBindRenderbuffer(GL_RENDERBUFFER, nrbo);
		return false;
	}
	m_nBindDepthState = IFrameRT::BIND_TEX;
	m_bIsBindDepthStencil = false;
	return true;
}

bool CFrameRT::inSetStencilTex(IStencilTex* pStencil)
{
	fm_int nrbo;
	// ����ɵ�rbo
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &nrbo);
    glBindRenderbuffer(GL_RENDERBUFFER, pStencil->GetRenderTex());
	m_pStencilTex = pStencil;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pStencil->GetRenderTex());
	if(TestErr("SetStencilTex glFramebufferRenderbuffer GL_STENCIL_ATTACHMENT"))
	{
		glBindRenderbuffer(GL_RENDERBUFFER, nrbo);
		return false;
	}
	m_nBindStencilState = IFrameRT::BIND_TEX;
	m_bIsBindDepthStencil = false;
	return true;
}

bool CFrameRT::inSetDepthStencilTex(IDepthStencilTex* pDepthStencilTex)
{
	fm_int nrbo;
	// ����ɵ�rbo
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &nrbo);
    glBindRenderbuffer(GL_RENDERBUFFER, pDepthStencilTex->GetRenderTex());
	m_pDepthStencilTex = pDepthStencilTex;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pDepthStencilTex->GetRenderTex());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pDepthStencilTex->GetRenderTex());
	if(TestErr("SetStencilTex glFramebufferRenderbuffer GL_STENCIL_ATTACHMENT"))
	{
		glBindRenderbuffer(GL_RENDERBUFFER, nrbo);
		return false;
	}
    m_bIsBindDepthStencil = true;
	return true;
}

bool CFrameRT::inSetDepthStencilRT(IDepthStencilRT* pDepthStencilRT)
{
	fm_int nfbo;
	// ����ɵ�fbo
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFrame);
	m_pDepthStencilRT = pDepthStencilRT;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pDepthStencilRT->GetTexture(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, pDepthStencilRT->GetTexture(), 0);
	if(TestFrameErr("SetDepthStencilRT CFrameRT Failed! glFramebufferTexture2D GL_DEPTH_ATTACHMENT"))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
		return false;
	}

	m_nBindDepthState = IFrameRT::BIND_RT;
	m_nBindStencilState = IFrameRT::BIND_RT;
	m_bIsBindDepthStencil = true;
	return true;
}

// ��ѯ��Frame��ǰ�İ󶨵�״̬
IFrameRT::BIND_STATE CFrameRT::GetColorState()
{
	return m_nBindColorState;
}

IFrameRT::BIND_STATE CFrameRT::GetDepthState()
{
	return m_nBindDepthState;
}

IFrameRT::BIND_STATE CFrameRT::GetStencilState()
{
	return m_nBindStencilState;
}

// �Ƿ����DepthStencilTex
bool CFrameRT::IsBindDepthStencil()
{
	return m_bIsBindDepthStencil;
}


void CFrameRT::Destroy()
{
	CORE_DELETE(this);
}

void CFrameRT::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

 

bool CFrameRT::Create()
{
	// �Ƿ���Ҫcolor
	if(m_nBindColorState == IFrameRT::BIND_NONE)
	{ 
        CORE_TRACE("Create FrameBuffer Failed!  No Color");
		return false;
	}
	fm_int nfbo;
	// ����ɵ�fbo
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nfbo);
	// ����һ��framebuffer
	glGenFramebuffers(1, &m_nFrame);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFrame);


	unsigned int color_width = 0;
	unsigned int color_heigth = 0;
	// �Ȱ�color
	if( m_nBindColorState == IFrameRT::BIND_RT )
	{
		if(!inSetColorRT( m_pColorRT ))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
			return false;
		}
		color_width = m_pColorRT->GetWidth();
		color_heigth = m_pColorRT->GetHeight();
	}
	else if( m_nBindColorState == IFrameRT::BIND_TEX )
	{
		if(!inSetColorTex( m_pColorTex) )
		{
			glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
			return false;
		}
		color_width = m_pColorTex->GetWidth();
		color_heigth = m_pColorTex->GetHeight();
	}
	else
	{
		return false;
	}

	// �Ƿ�����ģ������
	if(m_bIsBindDepthStencil)
	{
		if( m_nBindDepthState == IFrameRT::BIND_RT )
		{
			if(  (color_width != m_pDepthStencilRT->GetWidth() ) ||
				(color_heigth != m_pDepthStencilRT->GetHeight() ))
			{
				CORE_TRACE_EX("Create FrameRT Failed! ColorPoint Bind %d X %d,But DepthPoint Bind %d X %d",color_width,color_heigth,m_pDepthStencilRT->GetWidth(),m_pDepthStencilRT->GetHeight());
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}


			if(!inSetDepthStencilRT( m_pDepthStencilRT ) )
			{
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
		}
		else if( m_nBindDepthState == IFrameRT::BIND_TEX )
		{

			if(  (color_width != m_pDepthStencilTex->GetWidth() ) ||
				(color_heigth != m_pDepthStencilTex->GetHeight() ))
			{
				CORE_TRACE_EX("Create FrameRT Failed! ColorPoint Bind %d X %d,But DepthPoint Bind %d X %d",color_width,color_heigth,m_pDepthStencilTex->GetWidth(),m_pDepthStencilTex->GetHeight());
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
			if( !inSetDepthStencilTex( m_pDepthStencilTex ) )
			{
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		// �����
		if( m_nBindDepthState == IFrameRT::BIND_RT )
		{

			if(  (color_width != m_pDepthRT->GetWidth() ) ||
				(color_heigth != m_pDepthRT->GetHeight() ))
			{
				CORE_TRACE_EX("Create FrameRT Failed! ColorPoint Bind %d X %d,But DepthPoint Bind %d X %d",color_width,color_heigth,m_pDepthRT->GetWidth(),m_pDepthRT->GetHeight());
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
			if(!inSetDepthRT( m_pDepthRT) )
			{
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
		}
		else if(m_nBindDepthState == IFrameRT::BIND_TEX)
		{
			if(  (color_width != m_pDepthTex->GetWidth() ) ||
				(color_heigth != m_pDepthTex->GetHeight() ))
			{
				CORE_TRACE_EX("Create FrameRT Failed! ColorPoint Bind %d X %d,But DepthPoint Bind %d X %d",color_width,color_heigth,m_pDepthTex->GetWidth(),m_pDepthTex->GetHeight());
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
			if(!inSetDepthTex( m_pDepthTex) )
			{
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
		}
		else
		{
		//	return false; //֧�ֲ�Ҫ��ȵĴ���FBO
		}

		// ��ģ��
		if( m_nBindStencilState == IFrameRT::BIND_TEX)
		{
			if(!inSetStencilTex(m_pStencilTex) )
			{
				glBindFramebuffer(GL_FRAMEBUFFER, nfbo);
				return false;
			}
		}
	}
 
	glBindFramebuffer(GL_FRAMEBUFFER, nfbo);

	return true;
}
// ʹ�õ�ǰFBO��Ϊ��Ⱦ����
void CFrameRT::UsedFrameRT()
{
	m_pRender->UsedFrameRT(this);
	/*
	#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderFrameRTSumNum++;
	#endif

	//	if( !m_pRender->GetUsedFrameRT() || (this->GetFrame() != m_pRender->GetUsedFrameRT()->GetFrame() ) )
	//	{
	m_pRender->UsedFrameRT(this);
	//glBindFramebuffer(GL_FRAMEBUFFER,this->GetFrame());
	#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderFrameRTUsedNum++;
	#endif
	//	}
	*/
}
