//--------------------------------------------------------------------
// �ļ���:		scene_view.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TSceneViewAlloc& src) {}
};

// ������ͼ

class IRenderContext;
class Render;

/*
���ڳ�����ͼ�����
CSceneView 
���ڳ��������վ�ڲ�ͬ�ĽǶȽ��в鿴

����һ�������µ���Ⱦ����

����˵Ĭ�ϵĴ��� 
MainSceneView

ˮ�淴��(���з����ʱ��)
SeaReflection

�ر���Ӱ(����ʵʱ��Ӱ��ʱ��)
PostShadow

CSceneView�൱��һ�������ռ�����
������
IRenderContext
����Ҫ��Ҫ��Ⱦ�Ķ������һ���߼��ϵ���Ⱦ����
���
CSceneView
�б�������Ը���IRenderContext ����Ҫ�����Ķ��������Ⱦ����

*/


class CSceneView:public ISceneView
{
	static bool OnInvalidate(void * context);
	static bool OnRestore(void * context);
	void Create(Render* pRender);
public:
	static CSceneView* NewInstance(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent,Render* pRender);

	// ��Ⱦ����ԭ��
	typedef void (*render_func_t)(void* pdata);

	enum FLAG_FRAME_RT
	{
		FLAG_COLOR = 0,//��ɫλ
        FLAG_DEPTH = 8,//���λ
		FLAG_STENCIL = 16,//ģ��λ
		FLAG_TYPE = 24,//�����չλ   
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
		render_func_t pOnlyZPassFunc;//֮ǰ��Ԥ��д���
		render_func_t pOnlyColorPassFunc;//֮��ر����ֻд����ɫ
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

	// ����Ƚ�
	static int DistanceCompare(const void* elem1, const void* elem2);

	// EarlyZ�Ƚ�
	static int EarlyZCompare(const void* elem1, const void* elem2);

public:
	CSceneView();
	~CSceneView();

	// ��Ⱦ������
	void SetContext(IRenderContext* value) { m_pContext = value; }
	IRenderContext* GetContext() const { return m_pContext; }

	// ���õ�ǰ��ȾFrameRT �������ɫ
	void SetFrameRT(IFrameRT* pFrameRT,FmVec4 backcolor)   { m_pFrameRT = pFrameRT; m_backcolor = backcolor; }
	// ��ȡ��ǰ�󶨵�FrameRT
	IFrameRT* GetFrameRT() const  { return  m_pFrameRT; }

	void SetBackColor(const FmVec4 backcolor ) { m_backcolor = backcolor; }

	// ��ȡ��ǰ������ɫ
	const FmVec4& GetBackColor()const { return m_backcolor ; }

	// ��ȡ��ͼ����
	ISceneView::SCENE_VIEW_TYPE GetSceneViewType(){ return m_nSceneViewType ; }

	// �ͷ�
	void Release();
	// �����������
	void ClearBatchs();
	// ����EarlyZ���� ��Ⱦ�ص� ��Ⱦ���� ���ĵ� ��Χ�� 
	void AddEarlyZBatch(render_func_t only_z_pass_func, void* pdata,float distance,const char* ident = NULL,render_func_t only_color_pass_func = NULL,bool bRestoreRenderState = true);
	// ����ʵ������
	void AddSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// �¼ӵر���Ⱦ����
	void AddGroundBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// ���Ӵ�Alpha����һ���ʵ��������Ⱦ
	void AddSolidAlphaTestBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// �������Ʋ�������
	void AddGrassBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
    // ������������ʵ�����κ��һ����������
    void AddAfterSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// ����ˮ������
	void AddWaterBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// ���Ӱ�͸����ǰ����
    void AddPriorBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true);
	// ���Ӱ�͸������
	void AddBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true);
	// �����������
	void AddSkyBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// ���Ӻ�������
	void AddPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// �����Ļ������Ⱦ����
	void AddTopmostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// ����ȫ����Ӱ��������
	void AddShadowPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true);
	// ��Ⱦ���� ����Ч��ͨ��glCopyXX�ķ������backbuffer�Ĵ˷���Ч�ʱȽϵ�
	void DrawScene();

	// ����Ϊ�ռ��ĸ���RT��Tex����Ϣ
	// �ռ���ɫ��Ϣ �������NULLδ�������ռ�
	void SetColorRT(IColorRT* pRT);
	void SetColorTex(IColorTex* pTex);
    // �ռ������Ϣ
	void SetDepthRT(IDepthRT* pRT);
	void SetDepthTex(IDepthTex* pTex);
    // �ռ�ģ����Ϣ
	void SetStencilTex(IStencilTex* pTex);
	void SetDepthStencilTex(IDepthStencilTex* pTex);

	//��Ⱦ��������
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

	// ��ʼ׼����Ⱦ֮ǰ����
	void DrawSceneBefore();

	// �����Ⱦ����ز���
	void DrawSceneAfter();

	// ֱ�ӽ�������Ⱦ������ģ������
    void DrawNormalSceneBefore();
	void DrawNormalScene();
	void DrawNormalSceneAfter();

	// ���Ʒ��䳡��
	void DrawReflectionSceneBefore();
	void DrawReflectionScene();
	void DrawReflectionSceneAfter();

	// ����ͶӰ����
	void DrawProjectionSceneBefore();
	void DrawProjectionScene();
	void DrawProjectionSceneAfter();

    // ���ߵ�ǰ���ռ�����Ϣ����FrameRT FrameRT���Զ����������
	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthRT,IStencilTex* pStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorTex,IDepthStencilTex* pDepthStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex);

	IFrameRT* FindAndCreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT);

private:
	//��Ⱦ����EarlyZ����
	void DrawEarlyZBatchs();

	//��Ⱦ����ʵ������
	void DrawSolidBatchs();

	//��Ⱦ��Alpha����һ���ʵ��������Ⱦ
	void DrawSolidAlphaTestBatchs();

	//��Ⱦ����
	void DrawGrassBatchs();

	//��Ⱦ�ر�����
	void DrawGroundBatchs();

	//��Ⱦˮ��
	void DrawWaterBatchs();

	//��������ʵ�����κ��һ������
	void DrawAfterSolidBatchs();

	//��Ⱦ��͸����ǰ����
	void DrawPriorBlendBatchs();

	//��Ⱦ��͸������
	void DrawBlendBatchs();

	//��Ⱦ�������
	void DrawSkyBatchs();

	//��Ⱦ��Ļ��������
	void DrawTopmostBatchs();

	// ��Ⱦ�������
	void DrawPostBatchs();

	// ��Ⱦȫ����Ӱ�������
	void DrawSceneShadowPostBatchs();

	// Early pass
	void BeginEarlyZBatch();
	void EndEarlyZBatch();

	void BeginOnlyZPassBatch();
	void EndOnlyZPassBatch();

	void BeginOnlyColorPassBatch();
	void EndOnlyColorPassBatch();

	//������Ⱦ��ȫ��״̬�任
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

	//����CContext���ݾ����Ƿ���Ҫ����TempRT ������Ϸ���
	void CopyTempRT();

	//ͬ��
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

	const char* m_pstrIdent;//���ڵ��Դ�����Ⱦ˳��
	ISceneView::SCENE_VIEW_TYPE m_nSceneViewType;
	Render* m_pRender;
	IRenderStateOp* m_pRenderStateOp;

    TPodHashMap<unsigned int,IFrameRT*,TPodTraits<unsigned int>,TSceneViewAlloc> m_FlagMapFrameRT;
};

#endif // _SCENE_VIEW_H
