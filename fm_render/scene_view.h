//--------------------------------------------------------------------
// 文件名:		scene_view.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SCENE_VIEW_H
#define _SCENE_VIEW_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "../visual/i_resource.h"
#include "../visual/i_scene_view.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"

class TSceneViewAlloc
{
public:
	TSceneViewAlloc() {}
	~TSceneViewAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TSceneViewAlloc& src) {}
};

// 场景视图

class IRenderContext;
class Render;

/*
关于场景视图的理解
CSceneView 
对于场景里面的站在不同的角度进行查看

包含一个场景下的渲染内容

比如说默认的创建 
MainSceneView

水面反射(具有反射的时候)
SeaReflection

地表阴影(具有实时阴影的时候)
PostShadow

CSceneView相当于一个批次收集器，
完整对
IRenderContext
中需要需要渲染的对象进行一个逻辑上的渲染控制
因此
CSceneView
中必须包含对各种IRenderContext 中需要包含的对象进行渲染补充

*/


class CSceneView:public ISceneView
{
	static bool OnInvalidate(void * context);
	static bool OnRestore(void * context);
	void Create(Render* pRender);
public:
	static CSceneView* NewInstance(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent,Render* pRender);

	// 渲染函数原型
	typedef void (*render_func_t)(void* pdata);

	enum FLAG_FRAME_RT
	{
		FLAG_COLOR = 0,//颜色位
        FLAG_DEPTH = 8,//深度位
		FLAG_STENCIL = 16,//模板位
		FLAG_TYPE = 24,//标记扩展位   
	};

	enum FLAG_FRAME_TYPE
	{
		FLAG_COLOR_TYPE = 0,
		FLAG_DEPTH_TYPE = 1,
		FLAG_STENCIL_TYPE = 2,
	};

private:
	// early z pass
	struct early_z_batch_t
	{
		render_func_t pOnlyZPassFunc;//之前先预填写深度
		render_func_t pOnlyColorPassFunc;//之后关闭深度只写入颜色
		void* pData;
		float fdistance;
		const char* pstrIdent;
	    bool bRestoreRenderState;
	};

	struct batch_t
	{
		render_func_t pFunc;
		void* pData;
		const char* pstrIdent;
		bool bRestoreRenderState;
	};

	struct blend_batch_t
	{
		render_func_t pFunc;
		void* pData;
		float fDistance;
		int nOrder;
		const char* pstrIdent;
		bool bRestoreRenderState;
	};

	// 距离比较
	static int DistanceCompare(const void* elem1, const void* elem2);

	// EarlyZ比较
	static int EarlyZCompare(const void* elem1, const void* elem2);

public:
	CSceneView();
	~CSceneView();

	// 渲染器参数
	void SetContext(IRenderContext* value) { m_pContext = value; }
	IRenderContext* GetContext() const { return m_pContext; }

	// 设置当前渲染FrameRT 背景填充色
	void SetFrameRT(IFrameRT* pFrameRT,FmVec4 backcolor)   { m_pFrameRT = pFrameRT; m_backcolor = backcolor; }
	// 获取当前绑定的FrameRT
	IFrameRT* GetFrameRT() const  { return  m_pFrameRT; }

	void SetBackColor(const FmVec4 backcolor ) { m_backcolor = backcolor; }

	// 获取当前背景颜色
	const FmVec4& GetBackColor()const { return m_backcolor ; }

	// 获取视图类型
	ISceneView::SCENE_VIEW_TYPE GetSceneViewType(){ return m_nSceneViewType ; }

	// 释放
	void Release();
	// 清除所有批次
	void ClearBatchs();
	// 新增EarlyZ批次 渲染回调 渲染函数 中心点 包围球 
	void AddEarlyZBatch(render_func_t only_z_pass_func, void* pdata,float distance,const char* ident = NULL,render_func_t only_color_pass_func = NULL,bool bRestoreRenderState = true);
	// 增加实心批次
	void AddSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 新加地表渲染批次
	void AddGroundBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加带Alpha测试一类的实体批次渲染
	void AddSolidAlphaTestBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 新增绘制草体批次
	void AddGrassBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
    // 新增绘制所有实体批次后的一个批次内容
    void AddAfterSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加水体批次
	void AddWaterBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加半透明提前批次
    void AddPriorBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加半透明批次
	void AddBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加天空批次
	void AddSkyBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加后处理批次
	void AddPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 添加屏幕叠加渲染批次
	void AddTopmostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 增加全屏阴影生成批次
	void AddShadowPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// 渲染场景 所有效果通过glCopyXX的方法完成backbuffer的此方法效率比较低
	void DrawScene();

	// 下面为收集的各种RT和Tex的信息
	// 收集颜色信息 如果设置NULL未不进行收集
	void SetColorRT(IColorRT* pRT);
	void SetColorTex(IColorTex* pTex);
    // 收集深度信息
	void SetDepthRT(IDepthRT* pRT);
	void SetDepthTex(IDepthTex* pTex);
    // 收集模板信息
	void SetStencilTex(IStencilTex* pTex);
	void SetDepthStencilTex(IDepthStencilTex* pTex);

	//渲染辅助参数
	void SetUnderWater(bool value);
	void SetSkyVisible(bool value);
	void SetBlendVisible(bool value);
private:
	IColorRT* m_pColorRT;
	IDepthRT* m_pDepthRT;
    IColorTex* m_pColorTex;
	IDepthTex* m_pDepthTex;
    IStencilTex*  m_pStencilTex;
    IDepthStencilTex* m_pDepthStencilTex;

	bool      m_bUnderWater;
	bool      m_bSkyVisible;
	bool      m_bBlendVisible;

private:

	void ClearAllFrameRT();

	// 开始准备渲染之前操作
	void DrawSceneBefore();

	// 完成渲染的相关操作
	void DrawSceneAfter();

	// 直接将场景渲染到正常模型上面
    void DrawNormalSceneBefore();
	void DrawNormalScene();
	void DrawNormalSceneAfter();

	// 绘制反射场景
	void DrawReflectionSceneBefore();
	void DrawReflectionScene();
	void DrawReflectionSceneAfter();

	// 绘制投影场景
	void DrawProjectionSceneBefore();
	void DrawProjectionScene();
	void DrawProjectionSceneAfter();

    // 更具当前的收集的信息创建FrameRT FrameRT会自动管理和销毁
	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthRT,IStencilTex* pStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorTex,IDepthStencilTex* pDepthStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT);

private:
	//渲染所有EarlyZ批次
	void DrawEarlyZBatchs();

	//渲染所有实体批次
	void DrawSolidBatchs();

	//渲染带Alpha测试一类的实体批次渲染
	void DrawSolidAlphaTestBatchs();

	//渲染草体
	void DrawGrassBatchs();

	//渲染地表批次
	void DrawGroundBatchs();

	//渲染水体
	void DrawWaterBatchs();

	//绘制所有实体批次后的一个批次
	void DrawAfterSolidBatchs();

	//渲染半透明提前批次
	void DrawPriorBlendBatchs();

	//渲染半透明批次
	void DrawBlendBatchs();

	//渲染天空批次
	void DrawSkyBatchs();

	//渲染屏幕叠加批次
	void DrawTopmostBatchs();

	// 渲染后处理相关
	void DrawPostBatchs();

	// 渲染全屏阴影后处理相关
	void DrawSceneShadowPostBatchs();

	// Early pass
	void BeginEarlyZBatch();
	void EndEarlyZBatch();

	void BeginOnlyZPassBatch();
	void EndOnlyZPassBatch();

	void BeginOnlyColorPassBatch();
	void EndOnlyColorPassBatch();

	//批次渲染的全局状态变换
	void BeginSolidBatch();
	void EndSolidBatch();

	void BeginGroundBatch();
	void EndGroundBatch();

	void BeginGrassBatch();
	void EndGrassBatch();

	void BeginSolidAlphaTestBatch();
	void EndSolidAlphaTestBatch();

	void BeginWaterBatch();
	void EndWaterBatch();

	void BeginAfterSolidBatch();
	void EndAfterSolidBatch();

	void BeginPriorBlendBatch();
	void EndPriorBlendBatch();

	void BeginBlendBatch();
	void EndBlendBatch();

	void BeginSkyBatch();
	void EndSkyBatch();

	void BeginPostBatch();
	void EndPostBatch();

	void BeginTopmostBatch();
	void EndTopmostBatch();

	void TestErr(const char* ident);

	//根据CContext数据决定是否需要拷贝TempRT 这个是老方法
	void CopyTempRT();

	//同步
	void CopyTempColorRT();
	void CopyTempDepthRT();
private:

	IRenderContext* m_pContext;
	IFrameRT* m_pFrameRT;

	IFrameRT* m_pOld_FrameRT;
	IFrameRT* m_pTempFrameRT;

	FmVec4    m_backcolor;
	int m_nOrder;
	TArrayPod<early_z_batch_t, 1,TSceneViewAlloc> m_EarlyZBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_SolidBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_GroundBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_SolidAlphaTestBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_GrassBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_WaterBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_AfterSolidBatchs;
	TArrayPod<blend_batch_t, 1, TSceneViewAlloc> m_PriorBlendBatchs;
	TArrayPod<blend_batch_t, 1, TSceneViewAlloc> m_BlendBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_TopmostBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_SkyBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_PostBatchs;
	TArrayPod<batch_t, 1, TSceneViewAlloc> m_ShadowPostBatchs;

	const char* m_pstrIdent;//用于调试处理渲染顺序
	ISceneView::SCENE_VIEW_TYPE m_nSceneViewType;
	Render* m_pRender;
	IRenderStateOp* m_pRenderStateOp;

    TPodHashMap<unsigned int,IFrameRT*,TPodTraits<unsigned int>,TSceneViewAlloc> m_FlagMapFrameRT;
};

#endif // _SCENE_VIEW_H
