//--------------------------------------------------------------------
// 文件名:		scene_view.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "scene_view.h"
#include <stdlib.h>


#include "../public/core_log.h"
#include "context.h"
#include "../public/portable.h"
#include "render.h"

#define performance_time Port_GetPerformanceTime

CSceneView* CSceneView::NewInstance(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent,Render* pRender)
{
	CSceneView* pView = CORE_NEW(CSceneView);
	if(pView)
	{
		pView->m_pstrIdent = pstrIdent;
		pView->m_nSceneViewType = scene_view_type;
	}
	if(pRender)
	{
		pView->Create(pRender);
	}

	return pView;
}

void CSceneView::Create(Render* pRender)
{
	m_pRender = pRender;
	m_pContext = pRender->GetContext();
	m_pRenderStateOp = pRender->GetRenderStateOp();

#ifdef PROCESS_LOST_DEVICE
	m_pRender->AddDeviceLostCallback(this, OnInvalidate, OnRestore);
#endif
}

CSceneView::CSceneView()
{
	m_pContext = NULL;
	m_nOrder = 0;
	m_pstrIdent = NULL;
	m_pRender = NULL;

	m_backcolor = FmVec4(0.0,0.0,0.0,0.0);

	m_pTempFrameRT = NULL;

	m_pColorRT = NULL;
	m_pDepthRT = NULL;

	m_pColorTex = NULL;
	m_pDepthTex = NULL;
	m_pStencilTex = NULL;
	m_pDepthStencilTex = NULL;

	m_bUnderWater = false;

	m_bSkyVisible = true;

	m_bBlendVisible = true;
}

CSceneView::~CSceneView()
{
	ClearAllFrameRT();
#ifdef PROCESS_LOST_DEVICE
	if(g_pCore && m_pRender)
	{
		m_pRender->RemoveDeviceLostCallback(this);
	}
#endif
}

void CSceneView::Release()
{
	CORE_DELETE(this);
}

void CSceneView::ClearBatchs()
{
	m_nOrder = 0;
	m_SolidBatchs.clear();
	m_SolidAlphaTestBatchs.clear();
	m_WaterBatchs.clear();
	m_BlendBatchs.clear();
	m_SkyBatchs.clear();
	m_PostBatchs.clear();
	m_TopmostBatchs.clear();
	m_EarlyZBatchs.clear();
	m_GroundBatchs.clear();
	m_GrassBatchs.clear();
	m_PriorBlendBatchs.clear();
	m_AfterSolidBatchs.clear();
	m_ShadowPostBatchs.clear();
}

// 新增EarlyZ批次 渲染回调 渲染函数 到屏幕距离 包围盒大小 
void CSceneView::AddEarlyZBatch(render_func_t only_z_pass_func, void* pdata,float distance,const char* ident,render_func_t only_color_pass_func,bool bRestoreRenderState)
{
	early_z_batch_t data;
	data.pOnlyZPassFunc = only_z_pass_func;
	data.pOnlyColorPassFunc = only_color_pass_func;
	data.fdistance = distance;
	data.pstrIdent = ident;
	data.pData = pdata;
	data.bRestoreRenderState = bRestoreRenderState;
	m_EarlyZBatchs.push_back(data);
}

void CSceneView::AddSolidBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_SolidBatchs.push_back(data);
}

// 新加地表渲染批次
void CSceneView::AddGroundBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_GroundBatchs.push_back(data);
}

// 增加带Alpha测试一类的实体批次渲染
void CSceneView::AddSolidAlphaTestBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_SolidAlphaTestBatchs.push_back(data);
}

// 新增绘制草体批次
void CSceneView::AddGrassBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_GrassBatchs.push_back(data);
}

// 新增绘制所有实体批次后的一个批次内容
void CSceneView::AddAfterSolidBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_AfterSolidBatchs.push_back(data);
}

// 增加水体批次
void CSceneView::AddWaterBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_WaterBatchs.push_back(data);
}

// 增加半透明提前批次
void CSceneView::AddPriorBlendBatch(render_func_t func, void* pdata, float distance,const char* ident,bool bRestoreRenderState)
{
	blend_batch_t data;
	data.pFunc = func;
	data.pData = pdata;
	data.fDistance = distance;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	// 在排序时保证顺序不乱
	data.nOrder = (int)m_PriorBlendBatchs.size();
	m_PriorBlendBatchs.push_back(data);

}

void CSceneView::AddBlendBatch(render_func_t func, void* pdata, float distance,const char* ident,bool bRestoreRenderState)
{
	blend_batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.fDistance = distance;
	data.pstrIdent = ident;
	data.nOrder = m_nOrder++;
	data.bRestoreRenderState = bRestoreRenderState;

	m_BlendBatchs.push_back(data);
}

void CSceneView::AddSkyBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_SkyBatchs.push_back(data);
}

void CSceneView::AddPostBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_PostBatchs.push_back(data);
}

void CSceneView::AddTopmostBatch(render_func_t func, void* pdata,const char* ident,bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_TopmostBatchs.push_back(data);
}

void CSceneView::AddShadowPostBatch(render_func_t func, void* pdata, const char* ident, bool bRestoreRenderState)
{
	batch_t data;

	data.pFunc = func;
	data.pData = pdata;
	data.pstrIdent = ident;
	data.bRestoreRenderState = bRestoreRenderState;

	m_ShadowPostBatchs.push_back(data);
}
int CSceneView::DistanceCompare(const void* elem1, const void* elem2)
{
	float d1 = ((blend_batch_t*)elem1)->fDistance;
	float d2 = ((blend_batch_t*)elem2)->fDistance;

	if (d1 < d2)
	{
		return 1;
	}

	if (d1 > d2)
	{
		return -1;
	}

	// 距离相同的物体要保证前后顺序
	int order1 = ((blend_batch_t*)elem1)->nOrder;
	int order2 = ((blend_batch_t*)elem2)->nOrder;

	if (order1 > order2)
	{
		return 1;
	}

	if (order1 < order2)
	{
		return -1;
	}

	return 0;
}

// EarlyZ比较 由小到大排序
int CSceneView::EarlyZCompare(const void* elem1, const void* elem2)
{
	float d1 = ((early_z_batch_t*)elem1)->fdistance;
	float d2 = ((early_z_batch_t*)elem2)->fdistance;

	if (d1 > d2)
	{
		return 1;
	}

	if (d1 < d2)
	{
		return -1;
	}

	return 0;
}

//根据CContext数据决定是否需要拷贝TempRT
void CSceneView::CopyTempRT()
{
	CopyTempColorRT();
	CopyTempDepthRT();
}

//同步
void CSceneView::CopyTempColorRT()
{
#ifdef PERFORMANCE_DEBUG
	double dCopyTempColorRT = performance_time();
#endif
	IColorRT* pColorCopyTempColorRT = m_pContext->GetRealizeCopyTempColorRT();
	if(m_pContext->GetEnableRealizeCopyTempColorRT() && pColorCopyTempColorRT)
	{
		m_pRender->CopyColorRT( m_pContext->GetRealizeTempColorRT() ,pColorCopyTempColorRT,"Copy ColorRT");
		/*
		glBindTexture(GL_TEXTURE_2D,pColorCopyTempColorRT->GetTexture());
		glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,pColorCopyTempColorRT->GetWidth(),pColorCopyTempColorRT->GetHeight());//拷贝的时候需要2的幂拷贝，而且拷贝的大小要求比原图要大才可以
		*/

		//m_pContext->SetDebugColorRT(pColorCopyTempColorRT);
		TestErr("glCopyTexSubImage2D Color");
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dCopyTempColorRTTime +=
		performance_time() - dCopyTempColorRT;
#endif
}

void CSceneView::CopyTempDepthRT()
{
#ifdef PERFORMANCE_DEBUG
	double dCopyTempDepthRT = performance_time();
#endif
	IColorRT* pDepthCopyTempRT = m_pContext->GetRealizeCopyTempDepth();
	if(m_pContext->GetEnableRealizeCopyTempDepth() && pDepthCopyTempRT)
	{
		/*
		glBindTexture(GL_TEXTURE_2D,pDepthCopyTempRT->GetTexture());
		glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,pDepthCopyTempRT->GetWidth(),pDepthCopyTempRT->GetHeight());
		//		m_pContext->SetDebugDepthRT(pDepthTempRT);
		TestErr("glCopyTexSubImage2D Depth");
		*/
		//判断当前绑定可采样的深度图类型
		if( m_pContext->GetEnableRealizeTempDepthStencilRT() && m_pContext->GetRealizeTempDepthStencilRT() )
		{
			m_pRender->CopyDepthStencilRT( m_pContext->GetRealizeTempDepthStencilRT(), pDepthCopyTempRT,"Copy DepthRT");

		}
		else
		{
			if(m_pContext->GetRealizeTempDepthRT() && m_pContext->GetEnableRealizTempDepthRT() )
				m_pRender->CopyDepthRT( m_pContext->GetRealizeTempDepthRT(), pDepthCopyTempRT,"Copy DepthRT");
		// m_pContext->SetDebugDepthRT( m_pContext->GetRealizeTempDepthRT() );
		}
		//m_pContext->SetDebugColorRT(pDepthCopyTempRT);
		//m_pContext->SetDebugDecodeColorRT(pDepthCopyTempRT);
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dCopyTempDepthRTTime +=
		performance_time() - dCopyTempDepthRT;
#endif
}

void CSceneView::DrawScene()
{ 
	TestErr("DrawSceneBefore");
	DrawSceneBefore();

	switch(m_nSceneViewType)
	{
	case ISceneView::TYPE_NORMAL:
		{
			DrawNormalSceneBefore();
			//正常绘制屏幕
			DrawNormalScene();
			DrawNormalSceneAfter();
		}
		break;
	case ISceneView::TYPE_REFLECTION:
		{
			DrawReflectionSceneBefore();
			DrawReflectionScene();
			DrawReflectionSceneAfter();
		}
		break;
	case ISceneView::TYPE_PROJECTION:
		{
			DrawProjectionSceneBefore();
			DrawProjectionScene();
			DrawProjectionSceneAfter();
		}
		break;
	}

	DrawSceneAfter();
	TestErr("DrawSceneAfter");
}

// 渲染器相关参数
void CSceneView::SetColorRT(IColorRT* pRT)
{
	m_pColorRT = pRT;
	m_pColorTex = NULL;
}

void CSceneView::SetDepthRT(IDepthRT* pRT)
{
	m_pDepthRT = pRT;
	m_pDepthTex = NULL;
	m_pDepthStencilTex = NULL;
}

//渲染辅助参数
void CSceneView::SetUnderWater(bool value)
{
	m_bUnderWater = value;
}

void CSceneView::SetSkyVisible(bool value)
{
	m_bSkyVisible = value;
}

void CSceneView::SetBlendVisible(bool value)
{
	m_bBlendVisible = value;
}

void CSceneView::SetColorTex(IColorTex* pTex)
{
	m_pColorTex = pTex;
	m_pColorRT = NULL;
}

void CSceneView::SetDepthTex(IDepthTex* pTex)
{
	m_pDepthTex = pTex;
	m_pDepthRT = NULL;
	m_pDepthStencilTex = NULL;
}

void CSceneView::SetStencilTex(IStencilTex* pTex)
{
	m_pStencilTex = pTex;
	m_pDepthStencilTex = NULL;
	m_pDepthStencilTex = NULL;
}

void CSceneView::SetDepthStencilTex(IDepthStencilTex* pTex)
{
	m_pDepthStencilTex = pTex;
	m_pStencilTex = NULL;
}


//渲染所有EarlyZ批次
void CSceneView::DrawEarlyZBatchs()
{
	// Early Z 渲染
#ifdef PERFORMANCE_DEBUG
	double dEarlyZBatchs = performance_time();
#endif

	size_t early_z_num = m_EarlyZBatchs.size();

	if( (early_z_num > 0) && (m_pContext->GetEnableEarlyZ()))
	{
		BeginEarlyZBatch();
		qsort(&m_EarlyZBatchs[0], early_z_num, sizeof(early_z_batch_t), 
			EarlyZCompare);
		BeginOnlyZPassBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < early_z_num; ++i)
		{
			early_z_batch_t* pBatch = &m_EarlyZBatchs[i];
			if(pBatch->pOnlyZPassFunc)
			{
				pBatch->pOnlyZPassFunc(pBatch->pData);
				TestErr(pBatch->pstrIdent);
				if(pBatch->bRestoreRenderState)
					m_pRender->GetRenderStateRecordOp()->Restore();
			}
		}
		EndOnlyZPassBatch();

		BeginOnlyColorPassBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < early_z_num; ++i)
		{
			early_z_batch_t* pBatch = &m_EarlyZBatchs[i];
			if(pBatch->pOnlyColorPassFunc)
			{
				pBatch->pOnlyColorPassFunc(pBatch->pData);
				TestErr(pBatch->pstrIdent);
				if(pBatch->bRestoreRenderState)
					m_pRender->GetRenderStateRecordOp()->Restore();
			}
		}
		EndOnlyColorPassBatch();

		EndEarlyZBatch();
	}
	else
	{
		early_z_num = 0;
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawEarlyZBatchsTime +=
		performance_time() - dEarlyZBatchs;

	m_pRender->GetPerformance()->nDrawEarlyZBatchsNum +=early_z_num;
#endif
}

//渲染所有实体批次
void CSceneView::DrawSolidBatchs()
{
	// 实体绘制批次
#ifdef PERFORMANCE_DEBUG
	double dSolidBatchs = performance_time();
#endif
	size_t solid_num = m_SolidBatchs.size();

	if (solid_num > 0)
	{
		BeginSolidBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < solid_num; ++i)
		{
			batch_t* pBatch = &m_SolidBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndSolidBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawSolidBatchsTime +=
		performance_time() - dSolidBatchs;

	m_pRender->GetPerformance()->nDrawSolidBatchsNum +=solid_num;
#endif

}

//渲染带Alpha测试一类的实体批次渲染
void CSceneView::DrawSolidAlphaTestBatchs()
{
	// 带阿法测试的绘制批次
#ifdef PERFORMANCE_DEBUG
	double dSolidAlphaTestBatchs = performance_time();
#endif
	size_t solidalphtest_num = m_SolidAlphaTestBatchs.size();

	if(solidalphtest_num>0)
	{
		BeginSolidAlphaTestBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < solidalphtest_num; ++i)
		{
			batch_t* pBatch = &m_SolidAlphaTestBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndSolidAlphaTestBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawSolidAlphaTestBatchsTime +=
		performance_time() - dSolidAlphaTestBatchs;

	m_pRender->GetPerformance()->nDrawSolidAlphaTestBatchsNum +=solidalphtest_num;
#endif 
}

//渲染草体
void CSceneView::DrawGrassBatchs()
{
	// 绘制草体批次
#ifdef PERFORMANCE_DEBUG
	double dGrassBatchs = performance_time();
#endif
	size_t grass_num = m_GrassBatchs.size();

	if(grass_num>0)
	{
		BeginGrassBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < grass_num; ++i)
		{
			batch_t* pBatch = &m_GrassBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndGrassBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawGrassBatchsTime +=
		performance_time() - dGrassBatchs;

	m_pRender->GetPerformance()->nDrawGrassBatchsNum +=grass_num;
#endif  
}

//渲染地表批次
void CSceneView::DrawGroundBatchs()
{
	// 地表绘制
#ifdef PERFORMANCE_DEBUG
	double dGroundBatchs = performance_time();
#endif
	size_t ground_num = m_GroundBatchs.size();

	if (ground_num > 0)
	{
		BeginGroundBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < ground_num; ++i)
		{
			batch_t* pBatch = &m_GroundBatchs[i];

			pBatch->pFunc(pBatch->pData);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
			TestErr(pBatch->pstrIdent);
		}
		EndGroundBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawGroundBatchsTime +=
		performance_time() - dGroundBatchs;

	m_pRender->GetPerformance()->nDrawGroundBatchsNum +=ground_num;
#endif 

}

//绘制所有实体批次后的一个批次
void CSceneView::DrawAfterSolidBatchs()
{
	// 地表绘制
#ifdef PERFORMANCE_DEBUG
	double dAfterSolidBatchs = performance_time();
#endif
	size_t aftersolid_num = m_AfterSolidBatchs.size();

	if (aftersolid_num > 0)
	{
		BeginAfterSolidBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < aftersolid_num; ++i)
		{
			batch_t* pBatch = &m_AfterSolidBatchs[i];

			pBatch->pFunc(pBatch->pData);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
			TestErr(pBatch->pstrIdent);
		}
		EndAfterSolidBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawAfterSolidBatchsTime +=
		performance_time() - dAfterSolidBatchs;

	m_pRender->GetPerformance()->nDrawAfterSolidBatchsNum +=aftersolid_num;
#endif 

}


//渲染水体
void CSceneView::DrawWaterBatchs()
{
	//	CopyTempRT();
	CopyTempDepthRT();
	// 水面绘制批次
#ifdef PERFORMANCE_DEBUG
	double dWaterBatchs = performance_time();
#endif	

	//渲染水面之前.考虑是否拷贝一张临时RT
	size_t water_num = m_WaterBatchs.size();
	if( water_num > 0)
	{
		//同步深度图
		//		CopyTempDepthRT();

		BeginWaterBatch();

		//如果有水面则开始同步TempRT
		CopyTempColorRT();

		m_pRender->GetRenderStateRecordOp()->Store();
		for(size_t i = 0; i < water_num; ++i)
		{
			batch_t* pBatch = &m_WaterBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}

		EndWaterBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawWaterBatchsTime +=
		performance_time() - dWaterBatchs;

	m_pRender->GetPerformance()->nDrawWaterBatchsNum +=water_num;
#endif 
}

//渲染半透明提前批次
void CSceneView::DrawPriorBlendBatchs()
{
#ifdef PERFORMANCE_DEBUG
	double dPriorBlendBatchs = performance_time();
#endif	
	size_t prior_blend_num = m_PriorBlendBatchs.size();
	if(prior_blend_num>0)
	{
		BeginPriorBlendBatch();
		m_pRender->GetRenderStateRecordOp()->Store();

		qsort(&m_PriorBlendBatchs[0], prior_blend_num, sizeof(blend_batch_t), 
			DistanceCompare);

		for (size_t i = 0; i < prior_blend_num; ++i)
		{
			blend_batch_t* pBatch = &m_PriorBlendBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndPriorBlendBatch();
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawPriorBlendBatchsTime +=
		performance_time() - dPriorBlendBatchs;

	m_pRender->GetPerformance()->nDrawPriorBlendBatchsNum += prior_blend_num;
#endif 
}

//渲染半透明批次
void CSceneView::DrawBlendBatchs()
{

#ifdef PERFORMANCE_DEBUG
	double dBlendBatchs = performance_time();
#endif	
	size_t blend_num = m_BlendBatchs.size();


	if (blend_num > 0)
	{
		BeginBlendBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		// 半透明物体排序
#ifndef _SORT_DISABLE
		qsort(&m_BlendBatchs[0], blend_num, sizeof(blend_batch_t), 
			DistanceCompare);
#endif
		for (size_t i = 0; i < blend_num; ++i)
		{
			blend_batch_t* pBatch = &m_BlendBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndBlendBatch();
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawBlendBatchsTime +=
		performance_time() - dBlendBatchs;

	m_pRender->GetPerformance()->nDrawBlendBatchsNum += blend_num;
#endif 
}

//渲染天空批次
void CSceneView::DrawSkyBatchs()
{
	// 天空绘制批次
	size_t sky_num = m_SkyBatchs.size();
#ifdef PERFORMANCE_DEBUG
	double dSkyBatchs = performance_time();
#endif	

	if (sky_num > 0)
	{
		BeginSkyBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < sky_num; ++i)
		{
			batch_t* pBatch = &m_SkyBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndSkyBatch();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawSkyBatchsTime +=
		performance_time() - dSkyBatchs;
	m_pRender->GetPerformance()->nDrawSkyBatchsNum +=sky_num;
#endif
}

//渲染屏幕叠加批次
void CSceneView::DrawTopmostBatchs()
{

#ifdef PERFORMANCE_DEBUG
	double dTopmostBatchs = performance_time();
#endif	
	// 画屏幕叠加物体
	size_t Topmost_num = m_TopmostBatchs.size();

	if( Topmost_num > 0)
	{
		BeginTopmostBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for(size_t i = 0; i < Topmost_num; ++i)
		{
			batch_t* pBatch = &m_TopmostBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndTopmostBatch();
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawTopmostBatchsTime +=
		performance_time() - dTopmostBatchs;

	m_pRender->GetPerformance()->nDrawTopmostBatchsNum += Topmost_num;
#endif  
}

// 渲染后处理相关
void CSceneView::DrawPostBatchs()
{
#ifdef PERFORMANCE_DEBUG
	double dPostBatchs = performance_time();
#endif	
	size_t post_num = m_PostBatchs.size();

	if (post_num > 0)
	{
		BeginPostBatch();
		m_pRender->GetRenderStateRecordOp()->Store();
		for (size_t i = 0; i < post_num; ++i)
		{
			batch_t* pBatch = &m_PostBatchs[i];

			pBatch->pFunc(pBatch->pData);
			TestErr(pBatch->pstrIdent);
			if(pBatch->bRestoreRenderState)
				m_pRender->GetRenderStateRecordOp()->Restore();
		}
		EndPostBatch();
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dDrawPostBatchsTime +=
		performance_time() - dPostBatchs;

	m_pRender->GetPerformance()->nDrawPostBatchsNum +=post_num;
#endif 
}

// Early pass
void CSceneView::BeginEarlyZBatch()
{
	//	m_pRenderStateOp->EnableColorWrite(false);	
}

void CSceneView::EndEarlyZBatch()
{
	//	m_pRenderStateOp->EnableColorWrite(true);	
}

void CSceneView::BeginOnlyZPassBatch()
{
	m_pRenderStateOp->EnableColorWrite(false);	
	m_pRenderStateOp->EnableDepthWrite(true);
}

void CSceneView::EndOnlyZPassBatch()
{
	m_pRenderStateOp->EnableColorWrite(true);	
}

void CSceneView::BeginOnlyColorPassBatch()
{
	m_pRenderStateOp->EnableColorWrite(true);
	m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->SetDepthTestFunc( IRenderStateOp::DEPTH_TEST_EQUAL);
}

void CSceneView::EndOnlyColorPassBatch()
{
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
}


//批次渲染的全局状态变换
void CSceneView::BeginSolidBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false);
}

void CSceneView::EndSolidBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndSolidBatch IsCullFaceEnable false"); 
	}

	// 检测状态
	if(!m_pRenderStateOp->IsEnableDepthWrite())
	{
		CORE_TRACE("Check! EndSolidBatch IsEnableDepthWrite false"); 
	}

	// 进行深度检测
	if( m_pRenderStateOp->GetDepthTestFunc() != IRenderStateOp::DEPTH_TEST_LEQUAL )
	{
		CORE_TRACE("Check! EndSolidBatch GetDepthTestFunc != DEPTH_TEST_LEQUAL"); 
	}

	// 进行阿法混合状态检测
	if( m_pRenderStateOp->IsEnableBlend() )
	{
		CORE_TRACE("Check! EndSolidBatch IsEnableBlend true"); 
	}
#endif
}

void CSceneView::BeginGroundBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false); 
}

void CSceneView::EndGroundBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndGroundBatch IsCullFaceEnable false"); 
	}

	// 检测状态
	if(!m_pRenderStateOp->IsEnableDepthWrite())
	{
		CORE_TRACE("Check! EndGroundBatch IsEnableDepthWrite false"); 
	}

	// 进行深度检测
	if( m_pRenderStateOp->GetDepthTestFunc() != IRenderStateOp::DEPTH_TEST_LEQUAL )
	{
		CORE_TRACE("Check! EndGroundBatch GetDepthTestFunc != DEPTH_TEST_LEQUAL"); 
	}

	// 进行阿法混合状态检测
	if( m_pRenderStateOp->IsEnableBlend() )
	{
		CORE_TRACE("Check! EndGroundBatch IsEnableBlend true"); 
	}
#endif
}


void CSceneView::BeginSolidAlphaTestBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false);
}

void CSceneView::EndSolidAlphaTestBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndSolidAlphaTestBatch IsCullFaceEnable false"); 
	}

	// 检测状态
	if(!m_pRenderStateOp->IsEnableDepthWrite())
	{
		CORE_TRACE("Check! EndSolidAlphaTestBatch IsEnableDepthWrite false"); 
	}

	// 进行深度检测
	if( m_pRenderStateOp->GetDepthTestFunc() != IRenderStateOp::DEPTH_TEST_LEQUAL )
	{
		CORE_TRACE("Check! EndSolidAlphaTestBatch GetDepthTestFunc != DEPTH_TEST_LEQUAL"); 
	}

	// 进行阿法混合状态检测
	if( m_pRenderStateOp->IsEnableBlend() )
	{
		CORE_TRACE("Check! EndSolidAlphaTestBatch IsEnableBlend true"); 
	}
#endif
}

void CSceneView::BeginGrassBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false);
}

void CSceneView::EndGrassBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndGrassBatch IsCullFaceEnable false"); 
	}

	// 检测状态
	if(!m_pRenderStateOp->IsEnableDepthWrite())
	{
		CORE_TRACE("Check! EndGrassBatch IsEnableDepthWrite false"); 
	}

	// 进行深度检测
	if( m_pRenderStateOp->GetDepthTestFunc() != IRenderStateOp::DEPTH_TEST_LEQUAL )
	{
		CORE_TRACE("Check! EndGrassBatch GetDepthTestFunc != DEPTH_TEST_LEQUAL"); 
	}

	// 进行阿法混合状态检测
	if( m_pRenderStateOp->IsEnableBlend() )
	{
		CORE_TRACE("Check! EndGrassBatch IsEnableBlend true"); 
	}
#endif
}

void CSceneView::BeginWaterBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);

}

void CSceneView::EndWaterBatch()
{
	m_pRenderStateOp->EnableBlend(false);

#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndWaterBatch IsCullFaceEnable false"); 
	}

	// 检测状态
	if(!m_pRenderStateOp->IsEnableDepthWrite())
	{
		CORE_TRACE("Check! EndWaterBatch IsEnableDepthWrite false"); 
	}

	// 进行深度检测
	if( m_pRenderStateOp->GetDepthTestFunc() != IRenderStateOp::DEPTH_TEST_LEQUAL )
	{
		CORE_TRACE("Check! EndWaterBatch GetDepthTestFunc != DEPTH_TEST_LEQUAL"); 
	}
#endif
}

void CSceneView::BeginAfterSolidBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false);
}
void CSceneView::EndAfterSolidBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndGrassBatch IsCullFaceEnable false"); 
	}

	// 检测状态
	if(!m_pRenderStateOp->IsEnableDepthWrite())
	{
		CORE_TRACE("Check! EndAfterSolidBatch IsEnableDepthWrite false"); 
	}

	// 进行深度检测
	if( m_pRenderStateOp->GetDepthTestFunc() != IRenderStateOp::DEPTH_TEST_LEQUAL )
	{
		CORE_TRACE("Check! EndGrassBatch GetDepthTestFunc != DEPTH_TEST_LEQUAL"); 
	}

	// 进行阿法混合状态检测
	if( m_pRenderStateOp->IsEnableBlend() )
	{
		CORE_TRACE("Check! EndGrassBatch IsEnableBlend true"); 
	}
#endif
}


void CSceneView::BeginPriorBlendBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);

	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
}

void CSceneView::EndPriorBlendBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndPriorBlendBatch IsCullFaceEnable false"); 
	}
#endif

	m_pRenderStateOp->EnableBlend(false);
	m_pRenderStateOp->EnableDepthWrite(true);
}


void CSceneView::BeginBlendBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);

	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
}

void CSceneView::EndBlendBatch()
{
#ifdef	_DEBUG
	//进行批次状态检测
	if(!m_pRenderStateOp->IsCullFaceEnable())
	{
		CORE_TRACE("Check! EndBlendBatch IsCullFaceEnable false"); 
	}
#endif

	m_pRenderStateOp->EnableBlend(false);
	m_pRenderStateOp->EnableDepthWrite(true);
}

void CSceneView::BeginSkyBatch()
{
	m_pRenderStateOp->EnableDepthWrite(false);
}

void CSceneView::EndSkyBatch()
{
	m_pRenderStateOp->EnableDepthWrite(true);
}

void CSceneView::BeginPostBatch()
{
	m_pRenderStateOp->EnableDepthTest(false);
}

void CSceneView::EndPostBatch()
{
	m_pRenderStateOp->EnableDepthTest(true);
}

void CSceneView::BeginTopmostBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->EnableDepthTest(false);
}  

void CSceneView::EndTopmostBatch()
{
	m_pRenderStateOp->EnableBlend(false);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->EnableDepthTest(true);
}


void CSceneView::TestErr(const char* ident)
{
#ifdef _DEBUG
	int err = glGetError();	
	if(err == GL_NO_ERROR)
		return;
	CORE_TRACE("Batch Err!");
	if(ident)
		CORE_TRACE(ident);
	switch(err)
	{
	case GL_INVALID_ENUM:
		CORE_TRACE("Err: GL_INVALID_ENUM ");
		break;
	case GL_INVALID_VALUE:
		CORE_TRACE("Err: GL_INVALID_VALUE ");
		break;
	case GL_INVALID_OPERATION:
		CORE_TRACE("Err: GL_INVALID_OPERATION ");
		break;
	case GL_OUT_OF_MEMORY:
		CORE_TRACE("Err: GL_OUT_OF_MEMORY ");
		break;
	default:
		CORE_TRACE("Err: UnKnown ");
		break;
	}
#else
	return;
#endif
}

bool CSceneView::OnInvalidate(void * context)
{
	CSceneView* pThis = (CSceneView*)context;
	pThis->m_pRenderStateOp = 0;
	pThis->m_pRender = 0;

	return true;
}

bool CSceneView::OnRestore(void * context)
{
	CSceneView* pThis = (CSceneView*)context;
	Render* pRender = (Render*)g_pCore->GetInterface("Render");
	pThis->m_pRenderStateOp = pRender->GetRenderStateOp();
	pThis->m_pRender = pRender;
	return true;
}

// 更具当前的收集的信息创建FrameRT FrameRT会自动管理和销毁
IFrameRT* CSceneView::FindAndCreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex)
{
	// 先建立KEY
	unsigned int color_key = pColorRT->GetTexture()<<CSceneView::FLAG_COLOR;
	unsigned int depth_key = pDepthRT->GetTexture()<<CSceneView::FLAG_DEPTH;

	unsigned int stencil_key = 0;
	if(pStencilTex)
		stencil_key = pStencilTex->GetRenderTex()<<CSceneView::FLAG_STENCIL;

	unsigned int type_key = (0)<<CSceneView::FLAG_TYPE;

	unsigned int key = color_key + depth_key + stencil_key + type_key;

	if(!m_FlagMapFrameRT.Exists(key))
	{
		IFrameRT* pFrameRT = m_pRender->CreateFrameRT(pColorRT,pDepthRT,pStencilTex);
		m_FlagMapFrameRT.Add(key,pFrameRT);
	}

	return m_FlagMapFrameRT.Find(key).GetData();
}

IFrameRT* CSceneView::FindAndCreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex)
{
	// 先建立KEY
	unsigned int color_key = pColorRT->GetTexture()<<CSceneView::FLAG_COLOR;
	unsigned int depth_key = pDepthTex->GetRenderTex()<<CSceneView::FLAG_DEPTH;

	unsigned int stencil_key = 0;
	if(pStencilTex)
		stencil_key = pStencilTex->GetRenderTex()<<CSceneView::FLAG_STENCIL;

	unsigned int type_key = (1<<CSceneView::FLAG_DEPTH_TYPE )<<CSceneView::FLAG_TYPE ;

	unsigned int key = color_key + depth_key + stencil_key + type_key;

	if(!m_FlagMapFrameRT.Exists(key))
	{
		IFrameRT* pFrameRT = m_pRender->CreateFrameRT(pColorRT,pDepthTex,pStencilTex);
		m_FlagMapFrameRT.Add(key,pFrameRT);
	}

	return m_FlagMapFrameRT.Find(key).GetData();
}


IFrameRT* CSceneView::FindAndCreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex)
{
	return NULL;
}

IFrameRT* CSceneView::FindAndCreateFrameRT(IColorRT* pColorTex,IDepthStencilTex* pDepthStencilTex)
{
	return NULL;
}

IFrameRT* CSceneView::FindAndCreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex)
{
	return NULL;
}

IFrameRT* CSceneView::FindAndCreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT)
{
	// 先建立KEY
	unsigned int color_key = pColorRT->GetTexture()<<CSceneView::FLAG_COLOR;
	unsigned int depth_key = pDepthStencilRT->GetTexture()<<CSceneView::FLAG_DEPTH;

	unsigned int stencil_key = 0;
	if(pDepthStencilRT)
		stencil_key = pDepthStencilRT->GetTexture()<<CSceneView::FLAG_STENCIL;

	unsigned int type_key = ((1 << CSceneView::FLAG_DEPTH_TYPE) << CSceneView::FLAG_TYPE) + ((1 << CSceneView::FLAG_STENCIL_TYPE) << CSceneView::FLAG_TYPE);

	unsigned int key = color_key + depth_key + stencil_key + type_key;

	if(!m_FlagMapFrameRT.Exists(key))
	{
		IFrameRT* pFrameRT = m_pRender->CreateFrameRT(pColorRT,pDepthStencilRT);
		m_FlagMapFrameRT.Add(key,pFrameRT);
	}

	return m_FlagMapFrameRT.Find(key).GetData();
}

//   TPodHashMap<unsigned int,IFrameRT*,TPodTraits<unsigned int>,TSceneViewAlloc> m_FlagMapFrameRT;

void CSceneView::ClearAllFrameRT()
{
#define framert 	TPodHashMap<unsigned int, IFrameRT*, TPodTraits<unsigned int>,TSceneViewAlloc>
	for(framert::iterator iter = m_FlagMapFrameRT.Begin(); iter != m_FlagMapFrameRT.End(); iter++ )
	{
		SAFE_RELEASE(iter.GetData());
	}   
#undef framert
}