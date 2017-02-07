//--------------------------------------------------------------------
// 文件名:		scene_view_draw.cpp
// 内  容:		
// 说  明:		做scene_view的具体渲染逻辑
// 创建日期:	2013年11月1日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "scene_view.h"
#include <stdlib.h>


#include "../public/core_log.h"
#include "context.h"
#include "../public/portable.h"
#include "render.h"

#define performance_time Port_GetPerformanceTime

// 开始准备渲染之前操作
void CSceneView::DrawSceneBefore()
{
    // 先存档原先的FBO环境
	m_pOld_FrameRT = m_pRender->GetUsedFrameRT();
}

// 完成渲染
void CSceneView::DrawSceneAfter()
{
    // 恢复原先的FBO环境
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
     当开启全屏后处理
	 或者开启全屏折射
	 或者开启水的折射效果的时候需要单独处理
	 */
	//是否需要先渲染到一张零食的RT上然后后面进行全盘填充一下
	// 是否有后处理，是否有水面折射
	IFrameRT* pTempFrameRT = NULL;
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	// 是否需要渲染到一张TempRT
	if(m_pContext->GetEnableRealizeTempColorRT())
	{
		// 是否需要收集深度模板
		if(m_pContext->GetEnableRealizeTempDepthStencilRT())
		{
			pTempFrameRT = FindAndCreateFrameRT(m_pContext->GetRealizeTempColorRT(), m_pContext->GetRealizeTempDepthStencilRT());
		}
		else
		{
			// 是否需要收集深度图
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

		// 是否需要收集深度图
		if(m_pContext->GetEnableClearColorOptimize())
		{
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
		else
		{
			// 清空一下当前的FBO内容
			pRenderDrawOp->SetClearColor(m_backcolor);
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
	}
	else
	{
		// 是否需要收集深度图
		if(m_pContext->GetEnableClearColorOptimize())
		{
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
		else
		{
			// 清空一下当前的FBO内容
			pRenderDrawOp->SetClearColor(m_backcolor);
			pRenderDrawOp->SetClearDepth(1.0F);
			pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
		}
	}

}

// 直接将场景渲染到正常模型上面
void CSceneView::DrawNormalScene()
{
    //先进性EarlyZ批次剔除
	DrawEarlyZBatchs();

	//绘制实体模型
	DrawSolidBatchs();

	//绘制带阿法测试的批次
	DrawSolidAlphaTestBatchs();

	//绘制植被批次
	DrawGrassBatchs();

	//绘制地表批次
	DrawGroundBatchs();

	//渲染所有需要在实体绘制完之后一个批次处理
	DrawAfterSolidBatchs();

	//绘制天空批次
	DrawSkyBatchs();

	//绘制水体效果
	DrawWaterBatchs();	

	//半透明提前批次
	DrawPriorBlendBatchs();

	//绘制半透明批次
	DrawBlendBatchs();

	//绘制屏幕叠加批次
	DrawTopmostBatchs();

	//处理后处理批次
	DrawPostBatchs();

	DrawSceneShadowPostBatchs();
}

void CSceneView::DrawNormalSceneAfter()
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	if(m_pContext->GetEnableRealizeTempColorRT())
	{
		
		//判断是否为depthtex 如果是depthtex可以考虑数据
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

	// 设置开始收集FBO渲染相关
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

	// 是否需要收集深度图
	if(m_pContext->GetEnableClearColorOptimize())
	{
		pRenderDrawOp->SetClearDepth(1.0F);
		pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_DEPTH_BUFFER);
	}
	else
	{
		// 清空一下当前的FBO内容
		pRenderDrawOp->SetClearColor(m_backcolor);
		pRenderDrawOp->SetClearDepth(1.0F);
		pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
	}
}
// 绘制反射场景
void CSceneView::DrawReflectionScene()
{
	//先进性EarlyZ批次剔除
	DrawEarlyZBatchs();

	//绘制实体模型
	DrawSolidBatchs();

	//绘制带阿法测试的批次
	DrawSolidAlphaTestBatchs();

	//绘制植被批次
	//	DrawGrassBatchs();

	//绘制地表批次
	//	DrawGroundBatchs();

	//绘制天空批次
	DrawSkyBatchs();
	
	//半透明提前批次
	DrawPriorBlendBatchs();

	//绘制半透明批次
	DrawBlendBatchs();
}


void CSceneView::DrawReflectionSceneAfter()
{
	IColorRT* pColorRT = m_pColorRT;

	IDepthRT* pDepthRT = m_pDepthRT;

	IDepthTex* pDepthTex = m_pDepthTex;
//	IStencilTex* pStencilTex = m_pStencilTex;

	// 设置开始收集FBO渲染相关
	IFrameRT* pFrameRT = NULL;

	// 如果绑定的是Tex则可以考虑丢弃深度信息
	if(pDepthTex)
	{
		const GLenum attachments[] = { GL_DEPTH_ATTACHMENT };
		//glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
		m_pRender->GetDeviceCaps()->DiscardFramebufferEXT(1, attachments);
	}
}

// 绘制投影场景
void CSceneView::DrawProjectionSceneBefore()
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	m_pTempFrameRT = m_pRender->GetUsedFrameRT();

	// 设置开始收集FBO渲染相关
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

	// 清空一下当前的FBO内容
	pRenderDrawOp->SetClearColor(m_backcolor);
	pRenderDrawOp->SetClearDepth(1.0F);
	pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
}

void CSceneView::DrawProjectionScene()
{
	//绘制实体模型
	DrawSolidBatchs();

	//绘制带阿法测试的批次
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
	m_pTempFrameRT = NULL;
}

// 渲染全屏阴影后处理相关
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

		// 设置开始收集FBO渲染相关
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

		// 是否需要收集深度图
		FmVec4 init_color = FmVec4(1.0F, 1.0F, 1.0F, 1.0F);
		pRenderDrawOp->SetClearColor(init_color);
		pRenderDrawOp->SetClearStencil(0);
		pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_STENCIL_BUFFER);

		m_pRenderStateOp->EnableDepthTest(false);
		m_pRenderStateOp->EnableDepthWrite(false);

		// 画阴影生成批次
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