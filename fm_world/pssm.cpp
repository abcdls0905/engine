//--------------------------------------------------------------------
// 文件名:		pssm.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年1月28日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "pssm.h"

#include "../public/portable.h"

#include "../visual/i_render.h"
#include "../visual/i_scene_view.h"

/// \file pssm.cpp
/// \brief 平行切分阴影图实时阴影效果

/// entity: PSSM
/// \brief PSSM效果实体
/// \see 继承自IVisBase
DECLARE_ENTITY(PSSM, 0, IVisBase);

PSSM::PSSM()
{
	m_pRender = NULL;
	m_pObjects = NULL;
	m_pStencilShaderHandle = NULL;

	m_bVisible = true;
	m_bEnablePCF = false;
	m_bEnableVSM = false;
	m_bShadowMapReusable = false;

	m_fLogSplitWeight = 0.7F;
	m_fShadowExpandPercent = 0.1F;
	m_fShadowInten = 1.0F;

	m_nShadowMapCount = 1;
	m_nShadowMapIndex = -1;

	m_vShadowLightDir = FmVec3(0.0F, 0.0F, 0.0F);

	for (int i = 0; i < IRenderContext::MAX_SHADOWMAP_NUM; ++i)
	{
		m_vShadowOrthoMin[i] = FmVec2(0.0F, 0.0F);
		m_vShadowOrthoMax[i] = FmVec2(0.0F, 0.0F);
		m_vShadowOrthoCenter[i] = FmVec3(0.0F, 0.0F, 0.0F);

		m_fDepthSlope[i] = 0.0F;
		m_fDepthBias[i] = 0.0F;

		m_pShadowSceneView[i] = NULL;
	}
}

PSSM::~PSSM()
{
	for (size_t i = 0; i < IRenderContext::MAX_SHADOWMAP_NUM; ++i)
	{
		SAFE_RELEASE(m_pShadowSceneView[i]);
	}

	CORE_FREE(m_pStencilShaderHandle, sizeof(pssm_stencil_shaderhandle));
}

bool PSSM::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (m_pRender == NULL)
	{
		return false;
	}

	//m_pStencilShaderHandle = (pssm_stencil_shaderhandle*) CORE_ALLOC(sizeof(pssm_stencil_shaderhandle));

	//IVertexShader* pVS = m_pRender->LoadVertexShader("pssm.vsh", "main", "");
	//IPixelShader* pPS = m_pRender->LoadPixelShader("pssm.fsh", "main", "");

	//const char* vb[] = {"iPos"};
	//m_pStencilShaderHandle->pStencilShader = m_pRender->CreateShaderProgram(pVS, pPS, vb, 1);

	//IShaderParamOp* pParamOp = m_pStencilShaderHandle->pStencilShader->GetParamOp();
	//m_pStencilShaderHandle->hTexDepthMap = pParamOp->FindParamIdByName("tex_DepthMap");
	//m_pStencilShaderHandle->hTexShadowMap = pParamOp->FindParamIdByName("tex_ShadowMap");
	//m_pStencilShaderHandle->hMtxLightVP = pParamOp->FindParamIdByName("c_mtxLightVP");
	//m_pStencilShaderHandle->hDepthBiasAndSlope = pParamOp->FindParamIdByName("c_DepthBiasAndSlope");
	//m_pStencilShaderHandle->hPixelSize = pParamOp->FindParamIdByName("c_PixelSize");
	//m_pStencilShaderHandle->hShadowParam = pParamOp->FindParamIdByName("c_ShadowParam");
	//m_pStencilShaderHandle->hMtxProjInverse = pParamOp->FindParamIdByName("c_mtxProjInverse");
	//m_pStencilShaderHandle->hMtxWVP = pParamOp->FindParamIdByName("c_mtxWVP");

	//pVS = m_pRender->LoadVertexShader("pssm.vsh", "main", "#define MASK");
	//pPS = m_pRender->LoadPixelShader("pssm.fsh", "main", "#define MASK");

	//m_pStencilShaderHandle->pFilterShader = m_pRender->CreateShaderProgram(pVS, pPS, vb, 1);
	//pParamOp = m_pStencilShaderHandle->pFilterShader->GetParamOp();
	//m_pStencilShaderHandle->hShadowMaxZ = pParamOp->FindParamIdByName("c_fShadowMaxZ");

	return true;
}

bool PSSM::Shut()
{
	return true;
}

bool PSSM::Load()
{
	return true;
}

void PSSM::DrawShadowPostBatch(void* pData)
{
	PSSM* pthis = (PSSM*)pData;
	IRender* pRender = pthis->m_pRender;
	IRenderContext* pContext = pRender->GetContext();

	size_t ps_flags = 0;

	pssm_stencil_shaderhandle* pStencilShaderHandle = pthis->m_pStencilShaderHandle;
	IShaderProgram* pShader = pStencilShaderHandle->pFilterShader;

	pShader->UsedShader();
	// 不需要渲染的部分先填充模板
	IRenderStateOp* pRenderState = pRender->GetRenderStateOp();
	IShaderParamOp* pShaderParamOp = pShader->GetParamOp();

	pRenderState->EnableCullFace(false);
	pRenderState->EnableDepthWrite(false);
	pRenderState->EnableStencilTest(true);
	pRenderState->EnableDepthTest(true);
	pRenderState->EnableColorWrite(false);

	pRenderState->SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK, IRenderStateOp::STENCIL_TEST_EQUAL, 1, 0xFFFFFFFF);
	pRenderState->SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK,IRenderStateOp::STENCIL_FUNC_KEEP, IRenderStateOp::STENCIL_FUNC_KEEP, IRenderStateOp::STENCIL_FUNC_INCR);
	pShaderParamOp->SetParamValue(pStencilShaderHandle->hShadowMaxZ, pStencilShaderHandle->fShadowMaxZ, false);

	pRender->DrawAxisAlignedQuad();

	pRenderState->EnableColorWrite(true);
	pRenderState->EnableCullFace(true);
	// 只写入一个颜色通道z
	pRenderState->SetCullFaceModel(IRenderStateOp::CULL_BACK);
	pRenderState->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_GEQUAL);

	pShader = pStencilShaderHandle->pStencilShader;
	pShader->UsedShader();

	pShaderParamOp = pShader->GetParamOp();

	int shadow_map_num = pthis->m_nShadowMapCount;

	for (int i = 0; i < shadow_map_num; ++i)
	{
		pShaderParamOp->SetParamValue(pStencilShaderHandle->hMtxWVP, pStencilShaderHandle->matWVP, false);
		pShaderParamOp->SetParamValue(pStencilShaderHandle->hMtxProjInverse, pStencilShaderHandle->matProjInv, false);
		pShaderParamOp->SetParamValue(pStencilShaderHandle->hMtxLightVP, pStencilShaderHandle->matLightUp, false);
		pShaderParamOp->SetParamValue(pStencilShaderHandle->hDepthBiasAndSlope, pStencilShaderHandle->vDepthBiasAndSlope, false);
		pShaderParamOp->SetParamValue(pStencilShaderHandle->hPixelSize, pStencilShaderHandle->vPixelSize, false);
		pShaderParamOp->SetParamValue(pStencilShaderHandle->hMtxWVP, pStencilShaderHandle->matWVP, false);

		// 需要使用深度模板缓冲
		pShaderParamOp->SetTexture2D(pStencilShaderHandle->hTexDepthMap, pContext->GetRealizeTempDepthRT()->GetTexture());

		if (pthis->m_bEnablePCF)
		{
			Assert(pthis->m_bEnablePCF == true);
		}
		else
		{
			pShaderParamOp->SetTexture2D(pStencilShaderHandle->hTexShadowMap, pContext->GetShadowMapRT(i)->GetTexture());
		}

		pRender->DrawHalfCube();
	}

	// 恢复缺省的渲染状态
	pRenderState->EnableStencilTest(false);
	pRenderState->EnableDepthWrite(true);
	pRenderState->SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK, IRenderStateOp::STENCIL_TEST_ALWAYS, 0, 0xFFFFFFFF);
	pRenderState->SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK,IRenderStateOp::STENCIL_FUNC_KEEP,IRenderStateOp::STENCIL_FUNC_KEEP,IRenderStateOp::STENCIL_FUNC_KEEP);

	pRenderState->SetCullFaceModel(IRenderStateOp::CULL_BACK);
	pRenderState->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);
}

void PSSM::Realize()
{
	if (!m_bVisible)
	{
		return;
	}

	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();

	FmVec3 shadowParams = FmVec3(0.0f, 0.0f, pContext->GetShadowDistance());
	FmVec3 vDist;
	FmVec3TransformCoord(&vDist, &shadowParams, &camera.mtxProj);
	
	m_pStencilShaderHandle->fShadowMaxZ = vDist.z;

	FmMatrixTranspose(&m_pStencilShaderHandle->matProjInv, &camera.mtxProjInverse);

	m_pStencilShaderHandle->vShadowParam.x = m_fShadowInten;
	m_pStencilShaderHandle->vShadowParam.y = pContext->GetShadowDistance();
	m_pStencilShaderHandle->vShadowParam.z = camera.fNearZ;
	m_pStencilShaderHandle->vShadowParam.w = camera.fFarZ;
	
	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	m_pStencilShaderHandle->vPixelSize.x = 1.0F / view_width;
	m_pStencilShaderHandle->vPixelSize.y = 1.0F / view_height;

	FmVec3 refer_pos = pContext->GetReferPosition();
	int shadow_map_num = m_nShadowMapCount;

	for (int i = 0; i < shadow_map_num; ++i)
	{
		// 需要使用深度模板缓冲
		m_pStencilShaderHandle->vDepthBiasAndSlope.x = m_fDepthBias[i];
		m_pStencilShaderHandle->vDepthBiasAndSlope.y = m_fDepthSlope[i];

		// 采用相对坐标矩阵防止由于浮点数精度造成的阴影问题
		FmMat4 mtxShadowViewInverse;
		FmMat4 mtxShadowView;
		FmMat4 mtxShadowVP;
		FmMat4 mtxShadowVPI;
		FmMat4 mtxWVP;

		FmMat4Inverse(&mtxShadowViewInverse, NULL, pContext->GetShadowMapViewMatrix(i));
		mtxShadowViewInverse._41 -= refer_pos.x;
		mtxShadowViewInverse._42 -= refer_pos.y;
		mtxShadowViewInverse._43 -= refer_pos.z;

		FmMat4Inverse(&mtxShadowView, NULL, &mtxShadowViewInverse);
		FmMat4Multiply(&mtxShadowVP, &mtxShadowView, pContext->GetShadowMapProjMatrix(i));
		FmMat4Inverse(&mtxShadowVPI, NULL, &mtxShadowVP);
		FmMat4MultiplyTranspose(&m_pStencilShaderHandle->matWVP, &mtxShadowVPI, &camera.mtxViewProjRelative);

		FmMat4 mtxViewInverse = camera.mtxViewInverse;
		mtxViewInverse._41 -= refer_pos.x;
		mtxViewInverse._42 -= refer_pos.y;
		mtxViewInverse._43 -= refer_pos.z;

		FmMat4MultiplyTranspose(&m_pStencilShaderHandle->matLightUp, &mtxViewInverse, &mtxShadowVP);
	}

	m_pRender->GetSceneView()->AddShadowPostBatch(DrawShadowPostBatch, this);
}

void PSSM::GenerateShadowMap()
{
	if (m_pContext->GetDynamicShadowRT() == NULL)
		return;

#ifdef PERFORMANCE_DEBUG
	double dShadowMapRealizeTime = Port_GetPerformanceTime();
#endif

	// 设置环境上下文
	m_pRender->SetContext(m_pContext);

	FmMat4 old_view = m_pContext->GetCamera().mtxView;
	FmMat4 old_proj = m_pContext->GetCamera().mtxProj;

	float old_far_distance = m_pContext->GetFarClipDistance();
	float new_far_distance = m_pContext->GetShadowMapCameraDistance() * 2.0f;

	m_pContext->SetFarClipDistance(new_far_distance);

	unsigned int shadow_map_num = m_nShadowMapCount;

	for (unsigned int i = 0; i < shadow_map_num; ++i)
	{
		if (m_pContext->GetShadowMapNoFlush(i))
		{
			// 这一级阴影图不需要刷新
			continue;
		}

		m_pContext->SetShadowMapIndex(i);

		const FmMat4* mtxView = m_pContext->GetShadowMapViewMatrix(i);
		const FmMat4* mtxProj = m_pContext->GetShadowMapProjMatrix(i);

		m_pContext->SetViewMatrix(*mtxView);
		m_pContext->SetProjMatrix(*mtxProj);
		m_pContext->MakeHelperMatrix();
		m_pContext->MakeClipPlane();
		
		if (NULL == m_pShadowSceneView[i])
		{
			m_pShadowSceneView[i] = m_pRender->CreateSceneView(ISceneView::TYPE_PROJECTION,"PSSM Projection");
		}

		m_pRender->SetSceneView(m_pShadowSceneView[i]);
		m_pRender->AddSceneView(m_pShadowSceneView[i]);

		if (m_bEnablePCF)
		{
			Assert(m_bEnablePCF == false);
		}
		else
		{
			IColorRT* pShadowRT = m_pContext->GetDynamicShadowRT();

			m_pShadowSceneView[i]->SetColorRT(pShadowRT);
			//m_pShadowSceneView[i]->SetDepthTex(m_pContext->GetShadowMapDS(0));
      m_pShadowSceneView[i]->SetDepthRT(m_pContext->GetDepthRT());
		}

		object_t* p = m_pObjects;

		while (p)
		{
			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(p->ObjectId);

			if (pVisBase != NULL && pVisBase->GetCastShadow())
			{
				pVisBase->RealizeShadowMap();
			}

			p = p->pNext;
		}

		FmVec4 vClearColor = FmVec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_pShadowSceneView[i]->SetBackColor(vClearColor);
	}

	m_pContext->SetShadowMapIndex(-1);
	m_pContext->SetFarClipDistance(old_far_distance);
	m_pContext->SetViewMatrix(old_view);
	m_pContext->SetProjMatrix(old_proj);
	m_pContext->MakeHelperMatrix();
	m_pContext->MakeClipPlane();

	// 恢复环境上下文
	m_pRender->SetContext(NULL);
	m_pRender->SetSceneView(NULL);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dShadowMapRealizeTime +=
		Port_GetPerformanceTime() - dShadowMapRealizeTime;
#endif
}