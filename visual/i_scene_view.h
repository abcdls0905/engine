//--------------------------------------------------------------------
// 文件名:		i_scene_view.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年6月24日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _I_SCENE_VIEW_H
#define _I_SCENE_VIEW_H

#include "../public/Macros.h"


#include "i_resource.h"
#include "i_context.h"


class ISceneView	
{
public:
	//视图类型
	enum SCENE_VIEW_TYPE
	{
		TYPE_NORMAL,
        TYPE_REFLECTION,
        TYPE_PROJECTION,
		TYPE_TEXTURE,
	};
public:
    // 渲染函数原型
	typedef void (*render_func_t)(void* pdata);
public:
	// 渲染器参数
	virtual void SetContext(IRenderContext* value) = 0;
	virtual IRenderContext* GetContext() const = 0;

	virtual void SetBackColor(const FmVec4 backcolor ) = 0;

	// 获取当前背景颜色
	virtual const FmVec4& GetBackColor()const = 0;

	// 获取视图类型
	virtual ISceneView::SCENE_VIEW_TYPE GetSceneViewType() = 0;

	// 释放
	virtual void Release() = 0;
	// 清除所有批次
	virtual void ClearBatchs() = 0;
    // 新增EarlyZ批次 渲染回调 渲染函数 到屏幕空间距离  bRestoreRenderState是否需要进行状态恢复
	virtual void AddEarlyZBatch(render_func_t only_z_pass_func, void* pdata,float distance,const char* ident = NULL,render_func_t only_color_pass_func = NULL,bool bRestoreRenderState = true) = 0;
	// 增加实心批次
	virtual void AddSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 新加地表渲染批次
	virtual void AddGroundBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
    // 增加带Alpha测试一类的实体批次渲染
	virtual void AddSolidAlphaTestBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 新增绘制草体批次
	virtual void AddGrassBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 增加水体批次
	virtual void AddWaterBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 新增绘制所有实体批次后的一个批次内容
	virtual void AddAfterSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 增加半透明提前批次
	virtual void AddPriorBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 增加半透明批次
	virtual void AddBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 增加天空批次
	virtual void AddSkyBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 增加后处理批次
	virtual void AddPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 添加屏幕叠加渲染批次
	virtual void AddTopmostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 增加全屏阴影生成批次
	virtual void AddShadowPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// 渲染场景
	virtual void DrawScene() = 0;

	// 渲染器相关参数
	virtual void SetColorRT(IColorRT* pRT) = 0;
	virtual void SetColorTex(IColorTex* pTex) = 0;
    // 收集深度信息
	virtual void SetDepthRT(IDepthRT* pRT) = 0;
	virtual void SetDepthTex(IDepthTex* pTex) = 0;
    // 收集模板信息
	virtual void SetStencilTex(IStencilTex* pTex) = 0;
	virtual void SetDepthStencilTex(IDepthStencilTex* pTex) = 0;
    
	//渲染辅助参数
	virtual void SetUnderWater(bool value) = 0;
	virtual void SetSkyVisible(bool value) = 0;
	virtual void SetBlendVisible(bool value) = 0;
};

#endif
/*
这里注解一下关于批次的公共批次(以下底层全局状态批次管理状态,对于批次内部的公共状态下面代码有状态情况，不需要批次内部再重复设置，如果批次内部需要特殊处理，那么在改批次渲染结束后需要恢复到批次组的全局状态）

void CSceneView::BeginOnlyZPassBatch()
{
	m_pRenderStateOp->EnableColorWrite(false);	
	m_pRenderStateOp->EnableDepthWrite(true);
}

void CSceneView::BeginOnlyColorPassBatch()
{
	m_pRenderStateOp->EnableColorWrite(true);
	m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->SetDepthTestFunc( IRenderStateOp::DEPTH_TEST_EQUAL);
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

void CSceneView::BeginGroundBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false); 
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

void CSceneView::BeginSolidAlphaTestBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// 关闭阿法混合
	m_pRenderStateOp->EnableBlend(false);
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

void CSceneView::BeginWaterBatch()
{
	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);

}

void CSceneView::BeginPriorBlendBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);

	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
}


void CSceneView::BeginBlendBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);

	// 设置批次相关状态
	m_pRenderStateOp->EnableCullFace(true);
}

void CSceneView::BeginSkyBatch()
{
	m_pRenderStateOp->EnableDepthWrite(false);
}


void CSceneView::BeginPostBatch()
{
	m_pRenderStateOp->EnableDepthTest(false);
}

void CSceneView::BeginTopmostBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->EnableDepthTest(false);
}  


*/