//--------------------------------------------------------------------
// �ļ���:		i_scene_view.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��6��24��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef _I_SCENE_VIEW_H
#define _I_SCENE_VIEW_H

#include "../public/Macros.h"


#include "i_resource.h"
#include "i_context.h"


class ISceneView	
{
public:
	//��ͼ����
	enum SCENE_VIEW_TYPE
	{
		TYPE_NORMAL,
        TYPE_REFLECTION,
        TYPE_PROJECTION,
		TYPE_TEXTURE,
	};
public:
    // ��Ⱦ����ԭ��
	typedef void (*render_func_t)(void* pdata);
public:
	// ��Ⱦ������
	virtual void SetContext(IRenderContext* value) = 0;
	virtual IRenderContext* GetContext() const = 0;

	virtual void SetBackColor(const FmVec4 backcolor ) = 0;

	// ��ȡ��ǰ������ɫ
	virtual const FmVec4& GetBackColor()const = 0;

	// ��ȡ��ͼ����
	virtual ISceneView::SCENE_VIEW_TYPE GetSceneViewType() = 0;

	// �ͷ�
	virtual void Release() = 0;
	// �����������
	virtual void ClearBatchs() = 0;
    // ����EarlyZ���� ��Ⱦ�ص� ��Ⱦ���� ����Ļ�ռ����  bRestoreRenderState�Ƿ���Ҫ����״̬�ָ�
	virtual void AddEarlyZBatch(render_func_t only_z_pass_func, void* pdata,float distance,const char* ident = NULL,render_func_t only_color_pass_func = NULL,bool bRestoreRenderState = true) = 0;
	// ����ʵ������
	virtual void AddSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// �¼ӵر���Ⱦ����
	virtual void AddGroundBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
    // ���Ӵ�Alpha����һ���ʵ��������Ⱦ
	virtual void AddSolidAlphaTestBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// �������Ʋ�������
	virtual void AddGrassBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ����ˮ������
	virtual void AddWaterBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ������������ʵ�����κ��һ����������
	virtual void AddAfterSolidBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ���Ӱ�͸����ǰ����
	virtual void AddPriorBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ���Ӱ�͸������
	virtual void AddBlendBatch(render_func_t func, void* pdata, float distance,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// �����������
	virtual void AddSkyBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ���Ӻ�������
	virtual void AddPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// �����Ļ������Ⱦ����
	virtual void AddTopmostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ����ȫ����Ӱ��������
	virtual void AddShadowPostBatch(render_func_t func, void* pdata,const char* ident = NULL,bool bRestoreRenderState = true) = 0;
	// ��Ⱦ����
	virtual void DrawScene() = 0;

	// ��Ⱦ����ز���
	virtual void SetColorRT(IColorRT* pRT) = 0;
	virtual void SetColorTex(IColorTex* pTex) = 0;
    // �ռ������Ϣ
	virtual void SetDepthRT(IDepthRT* pRT) = 0;
	virtual void SetDepthTex(IDepthTex* pTex) = 0;
    // �ռ�ģ����Ϣ
	virtual void SetStencilTex(IStencilTex* pTex) = 0;
	virtual void SetDepthStencilTex(IDepthStencilTex* pTex) = 0;
    
	//��Ⱦ��������
	virtual void SetUnderWater(bool value) = 0;
	virtual void SetSkyVisible(bool value) = 0;
	virtual void SetBlendVisible(bool value) = 0;
};

#endif
/*
����ע��һ�¹������εĹ�������(���µײ�ȫ��״̬���ι���״̬,���������ڲ��Ĺ���״̬���������״̬���������Ҫ�����ڲ����ظ����ã���������ڲ���Ҫ���⴦����ô�ڸ�������Ⱦ��������Ҫ�ָ����������ȫ��״̬��

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

//������Ⱦ��ȫ��״̬�任
void CSceneView::BeginSolidBatch()
{
	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// �رհ������
	m_pRenderStateOp->EnableBlend(false);
}

void CSceneView::BeginGroundBatch()
{
	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// �رհ������
	m_pRenderStateOp->EnableBlend(false); 
}

void CSceneView::BeginGroundBatch()
{
	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// �رհ������
	m_pRenderStateOp->EnableBlend(false); 
}

void CSceneView::BeginSolidAlphaTestBatch()
{
	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// �رհ������
	m_pRenderStateOp->EnableBlend(false);
}

void CSceneView::BeginGrassBatch()
{
	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
	// �رհ������
	m_pRenderStateOp->EnableBlend(false);
}

void CSceneView::BeginWaterBatch()
{
	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
	m_pRenderStateOp->EnableDepthWrite(true);
	m_pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);

}

void CSceneView::BeginPriorBlendBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);

	// �����������״̬
	m_pRenderStateOp->EnableCullFace(true);
}


void CSceneView::BeginBlendBatch()
{
	m_pRenderStateOp->EnableBlend(true);
	m_pRenderStateOp->EnableDepthWrite(false);

	// �����������״̬
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