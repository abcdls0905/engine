//--------------------------------------------------------------------
// �ļ���:		scene_view_draw.cpp
// ��  ��:		
// ˵  ��:		��scene_view�ľ�����Ⱦ�߼�
// ��������:	2013��11��1��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "scene_view.h"
#include <stdlib.h>


#include "../public/core_log.h"
#include "context.h"
#include "../public/portable.h"
#include "render.h"

#define performance_time Port_GetPerformanceTime

// ��ʼ׼����Ⱦ֮ǰ����
void CSceneView::DrawSceneBefore()
{
    // �ȴ浵ԭ�ȵ�FBO����
	m_pOld_FrameRT = m_pRender->GetUsedFrameRT();
}

// �����Ⱦ
void CSceneView::DrawSceneAfter()
{
    // �ָ�ԭ�ȵ�FBO����
    if(m_pOld_FrameRT)
	{
		m_pOld_FrameRT->UsedFrameRT();
		m_pRenderStateOp->SetViewPort(0, 0, this->GetContext()->GetViewWidth(), this->GetContext()->GetViewHeight());
	}
	else
	{
        m_pRender->UsedFrameRT(NULL);
		m_pRenderStateOp->SetViewPort(0, 0, m_pRender->GetDeviceWidth(),m_pRender->GetDeviceHeight());
	}
}

void CSceneView::DrawNormalSceneBefore()
{
     /*
     ������ȫ������
	 ���߿���ȫ������
	 ���߿���ˮ������Ч����ʱ����Ҫ��������
	 */
	//�Ƿ���Ҫ����Ⱦ��һ����ʳ��RT��Ȼ��������ȫ�����һ��
	// �Ƿ��к����Ƿ���ˮ������
	IFrameRT* pTempFrameRT = NULL;
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	// �Ƿ���Ҫ��Ⱦ��һ��TempRT
	if(m_pContext->GetEnableRealizeTempColorRT())
	{
		// �Ƿ���Ҫ�ռ����ģ��
		if(m_pContext->GetEnableRealizeTempDepthStencilRT())
		{
			pTempFrameRT = FindAndCreateFrameRT(m_pContext->GetRealizeTempColorRT(), m_pContext->GetRealizeTempDepthStencilRT());
		}
		else
		{
			// �Ƿ���Ҫ�ռ����ͼ
			if(m_pContext->GetEnableRealizTempDepthRT())
			{
				pTempFrameRT = FindAndCreateFrameRT(m_pContext->GetRealizeTempColorRT(), m_pContext->GetRealizeTempDepthRT(),NULL);
			}
			else if(m_pContext->GetRealizeTempDepthTex())
			{
				pTempFrameRT = FindAndCreateFrameRT(m_pContext->GetRealizeTempColorRT(), m_pContext->GetRealizeTempDepthTex(),NULL);
			}
			else
			{
				pTempFrameRT = NULL;
			}
		}
	}

	if(pTempFrameRT)
	{
		pTempFrameRT->UsedFrameRT();
		m_pRenderStateOp->SetViewPort(0, 0, m_pContext->GetRealizeTempColorRT()->GetWidth(), m_pContext->GetRealizeTempColorRT()->GetHeight());

		// �Ƿ���Ҫ�ռ����ͼ
		if(m_pContext->GetEnableClearColorOptimize())
		{
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
		else
		{
			// ���һ�µ�ǰ��FBO����
			pRenderDrawOp->SetClearColor(m_backcolor);
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
	}
	else
	{
		// �Ƿ���Ҫ�ռ����ͼ
		if(m_pContext->GetEnableClearColorOptimize())
		{
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
		else
		{
			// ���һ�µ�ǰ��FBO����
			pRenderDrawOp->SetClearColor(m_backcolor);
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
	}

}

// ֱ�ӽ�������Ⱦ������ģ������
void CSceneView::DrawNormalScene()
{
    //�Ƚ���EarlyZ�����޳�
	DrawEarlyZBatchs();

	//����ʵ��ģ��
	DrawSolidBatchs();

	//���ƴ��������Ե�����
	DrawSolidAlphaTestBatchs();

	//����ֲ������
	DrawGrassBatchs();

	//���Ƶر�����
	DrawGroundBatchs();

	//��Ⱦ������Ҫ��ʵ�������֮��һ�����δ���
	DrawAfterSolidBatchs();

	//�����������
	DrawSkyBatchs();

	//����ˮ��Ч��
	DrawWaterBatchs();	

	//��͸����ǰ����
	DrawPriorBlendBatchs();

	//���ư�͸������
	DrawBlendBatchs();

	//������Ļ��������
	DrawTopmostBatchs();

	//�����������
	DrawPostBatchs();

	DrawSceneShadowPostBatchs();
}

void CSceneView::DrawNormalSceneAfter()
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	if(m_pContext->GetEnableRealizeTempColorRT())
	{
		
		//�ж��Ƿ�Ϊdepthtex �����depthtex���Կ�������
		if(m_pContext->GetEnableRealizeTempDepthTex()&&!m_pContext->GetEnableRealizTempDepthRT())
		{
			const GLenum attachments[] = { GL_DEPTH_ATTACHMENT };
			//glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
			m_pRender->GetDeviceCaps()->DiscardFramebufferEXT(1, attachments);
		}


		m_pRender->UsedFrameRT(m_pOld_FrameRT);
		m_pRenderStateOp->SetViewPort(0, 0, m_pRender->GetDeviceWidth(),m_pRender->GetDeviceHeight());
		m_pRender->CopyColorRT(m_pContext->GetRealizeTempColorRT(),NULL,"Last Fill TempColorRT");
	}
}


void CSceneView::DrawReflectionSceneBefore()
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	IColorRT* pColorRT = m_pColorRT;

	IDepthRT* pDepthRT = m_pDepthRT;

	IDepthTex* pDepthTex = m_pDepthTex;
//	IStencilTex* pStencilTex = m_pStencilTex;

	// ���ÿ�ʼ�ռ�FBO��Ⱦ���
	IFrameRT* pFrameRT = NULL;

	if(pDepthRT)
	{
		pFrameRT = FindAndCreateFrameRT(pColorRT,pDepthRT,NULL);
	}
	else
	{
		pFrameRT = FindAndCreateFrameRT(pColorRT,pDepthTex,NULL);
	}

	pFrameRT->UsedFrameRT();

	m_pRenderStateOp->SetViewPort(0, 0, this->GetContext()->GetViewWidth(), this->GetContext()->GetViewHeight());

	// �Ƿ���Ҫ�ռ����ͼ
	if(m_pContext->GetEnableClearColorOptimize())
	{
		pRenderDrawOp->SetClearDepth(1.0F);
		pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_DEPTH_BUFFER);
	}
	else
	{
		// ���һ�µ�ǰ��FBO����
		pRenderDrawOp->SetClearColor(m_backcolor);
		pRenderDrawOp->SetClearDepth(1.0F);
		pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
	}
}
// ���Ʒ��䳡��
void CSceneView::DrawReflectionScene()
{
	//�Ƚ���EarlyZ�����޳�
	DrawEarlyZBatchs();

	//����ʵ��ģ��
	DrawSolidBatchs();

	//���ƴ��������Ե�����
	DrawSolidAlphaTestBatchs();

	//����ֲ������
	//	DrawGrassBatchs();

	//���Ƶر�����
	//	DrawGroundBatchs();

	//�����������
	DrawSkyBatchs();
	
	//��͸����ǰ����
	DrawPriorBlendBatchs();

	//���ư�͸������
	DrawBlendBatchs();
}


void CSceneView::DrawReflectionSceneAfter()
{
	IColorRT* pColorRT = m_pColorRT;

	IDepthRT* pDepthRT = m_pDepthRT;

	IDepthTex* pDepthTex = m_pDepthTex;
//	IStencilTex* pStencilTex = m_pStencilTex;

	// ���ÿ�ʼ�ռ�FBO��Ⱦ���
	IFrameRT* pFrameRT = NULL;

	// ����󶨵���Tex����Կ��Ƕ��������Ϣ
	if(pDepthTex)
	{
		const GLenum attachments[] = { GL_DEPTH_ATTACHMENT };
		//glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
		m_pRender->GetDeviceCaps()->DiscardFramebufferEXT(1, attachments);
	}
}

//ls test
// ����ͶӰ����
void CSceneView::DrawProjectionSceneBefore()
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	m_pTempFrameRT = m_pRender->GetUsedFrameRT();

	// ���ÿ�ʼ�ռ�FBO��Ⱦ���
	IFrameRT* pFrameRT = NULL;

	if (m_pDepthRT)
	{
		pFrameRT = FindAndCreateFrameRT(m_pColorRT, m_pDepthRT, NULL);
	}
	else
	{
		pFrameRT = FindAndCreateFrameRT(m_pColorRT, m_pDepthTex, NULL);
	}

	pFrameRT->UsedFrameRT();

	m_pRenderStateOp->SetViewPort(1, 1, m_pColorRT->GetWidth() - 2, m_pColorRT->GetHeight() - 2);

	// ���һ�µ�ǰ��FBO����
	pRenderDrawOp->SetClearColor(m_backcolor);
	pRenderDrawOp->SetClearDepth(1.0F);
	pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
}

void CSceneView::DrawProjectionScene()
{
	//����ʵ��ģ��
	DrawSolidBatchs();

	//���ƴ��������Ե�����
	DrawSolidAlphaTestBatchs();
}

void CSceneView::DrawProjectionSceneAfter()
{
	if(m_pTempFrameRT)
	{
		IColorRT* pColorRT = m_pTempFrameRT->GetColorRT();
		m_pRenderStateOp->SetViewPort(0, 0, pColorRT->GetWidth(), pColorRT->GetHeight());
	}
	else
	{
		m_pRenderStateOp->SetViewPort(0, 0, m_pRender->GetWinWidth(), m_pRender->GetWinHeight());
	}

	m_pRender->UsedFrameRT(m_pTempFrameRT);

  if (m_pDepthRT)
    m_pRender->GetContext()->SetDebugDepthRT(m_pDepthRT);

	m_pTempFrameRT = NULL;
}

// ��Ⱦȫ����Ӱ�������
void CSceneView::DrawSceneShadowPostBatchs()
{
	size_t numObjects = m_ShadowPostBatchs.size();

	if (numObjects > 0)
	{
		IRender* pRender = m_pRender;
		IRenderContext* pContext = m_pContext;
		IColorRT* pShadowRT = pContext->GetDynamicShadowRT();
		IDepthRT* pDephtRT = pContext->GetRealizeTempDepthRT();
		IStencilTex* pStencilTex = NULL;//pContext->GetRealizeTempStencilTex();

		// ���ÿ�ʼ�ռ�FBO��Ⱦ���
		m_pTempFrameRT = m_pRender->GetUsedFrameRT();
		IFrameRT* pFrameRT = NULL;

		if (pDephtRT)
		{
			pFrameRT = FindAndCreateFrameRT(pShadowRT, pDephtRT, pStencilTex);
		}
		else
		{
			IDepthTex* pDepthTex = pContext->GetRealizeTempDepthTex();
			pFrameRT = FindAndCreateFrameRT(pShadowRT, pDepthTex, pStencilTex);
		}

		pFrameRT->UsedFrameRT();

		m_pRenderStateOp->SetViewPort(0, 0, pShadowRT->GetWidth(), pShadowRT->GetHeight());

		IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

		// �Ƿ���Ҫ�ռ����ͼ
		FmVec4 init_color = FmVec4(1.0F, 1.0F, 1.0F, 1.0F);
		pRenderDrawOp->SetClearColor(init_color);
		pRenderDrawOp->SetClearStencil(0);
		pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_STENCIL_BUFFER);

		m_pRenderStateOp->EnableDepthTest(false);
		m_pRenderStateOp->EnableDepthWrite(false);

		// ����Ӱ��������
		size_t shadow_post_num = m_ShadowPostBatchs.size();

		for (size_t i = 0; i < shadow_post_num; ++i)
		{
			batch_t* pBatch = &m_ShadowPostBatchs[i];

			pBatch->pFunc(pBatch->pData);
		}

		m_pRender->UsedFrameRT(m_pTempFrameRT);

		m_pRenderStateOp->EnableDepthTest(true);
		m_pRenderStateOp->EnableDepthWrite(true);
	}
}