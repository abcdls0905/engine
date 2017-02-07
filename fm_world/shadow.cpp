//--------------------------------------------------------------------
// 文件名:		shadow.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "shadow.h"
#include "shadow_manager.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"

/// \file shadow.cpp
/// \brief 影子

/// entity: Shadow
/// \brief 影子实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Shadow, 0, IVisBase);

/// property: object Bind
/// \brief 绑定的对象
DECLARE_PROPERTY(PERSISTID, Shadow, Bind, GetBindID, SetBindID);
/// property: int TextureSize
/// \brief 阴影贴图尺寸
//DECLARE_PROPERTY(int, Shadow, TextureSize, GetTextureSize, SetTextureSize);
/// property: bool Visible
DECLARE_PROPERTY(bool, Shadow, Visible, GetVisible, SetVisible);
/// property: float LightDispersion
/// \brief 阴影模糊度
DECLARE_PROPERTY(float, Shadow, LightDispersion, GetLightDispersion, 
	SetLightDispersion);
/// property: string ShadowTopColor
/// \brief 阴影底部颜色
DECLARE_PROPERTY(result_string, Shadow, ShadowTopColor, GetShadowTopColorString, 
	SetShadowTopColorString);
/// property: string ShadowBottomColor
/// \brief 阴影底部颜色
DECLARE_PROPERTY(result_string, Shadow, ShadowBottomColor, 
	GetShadowBottomColorString, SetShadowBottomColorString);
/// property: bool UseSimpleShadow
/// \brief 是否使用简易阴影
DECLARE_PROPERTY(bool, Shadow, UseSimpleShadow, GetUseSimpleShadow, 
	SetUseSimpleShadow);

#define VBUFFER_SIZE 4096

// Shadow

void BuildViewConeMesh(const FmMat4& mtxCull, FmVec3* pOut)
{
	FmVec3 vecWorld[8];
	FmVec3 vecScreen[8];

	vecScreen[0].x = -1.0F; vecScreen[0].y = +1.0F; vecScreen[0].z = 0.0F;
	vecScreen[1].x = -1.0F; vecScreen[1].y = +1.0F; vecScreen[1].z = 1.0F;
	vecScreen[2].x = -1.0F; vecScreen[2].y = -1.0F; vecScreen[2].z = 0.0F;
	vecScreen[3].x = -1.0F; vecScreen[3].y = -1.0F; vecScreen[3].z = 1.0F;
	vecScreen[4].x = +1.0F; vecScreen[4].y = -1.0F; vecScreen[4].z = 0.0F;
	vecScreen[5].x = +1.0F; vecScreen[5].y = -1.0F; vecScreen[5].z = 1.0F;
	vecScreen[6].x = +1.0F; vecScreen[6].y = +1.0F; vecScreen[6].z = 0.0F;
	vecScreen[7].x = +1.0F; vecScreen[7].y = +1.0F; vecScreen[7].z = 1.0F;

    for(int i = 0; i < 8; i ++ )
	{
		FmVec3Unproject(&vecWorld[i],&vecScreen[i],&mtxCull,NULL,NULL);
	}

	// left
	pOut[0] = vecWorld[2];
	pOut[1] = vecWorld[1];
	pOut[2] = vecWorld[0];
	pOut[3] = vecWorld[2];
	pOut[4] = vecWorld[3];
	pOut[5] = vecWorld[1];
	// top
	pOut[6] = vecWorld[0];
	pOut[7] = vecWorld[1];
	pOut[8] = vecWorld[7];
	pOut[9] = vecWorld[0];
	pOut[10] = vecWorld[7];
	pOut[11] = vecWorld[6];
	// right
	pOut[12] = vecWorld[6];
	pOut[13] = vecWorld[7];
	pOut[14] = vecWorld[4];
	pOut[15] = vecWorld[4];
	pOut[16] = vecWorld[7];
	pOut[17] = vecWorld[5];
	// bottom
	pOut[18] = vecWorld[2];
	pOut[19] = vecWorld[4];
	pOut[20] = vecWorld[3];
	pOut[21] = vecWorld[4];
	pOut[22] = vecWorld[5];
	pOut[23] = vecWorld[3];
	// front
	pOut[24] = vecWorld[0];
	pOut[25] = vecWorld[6];
	pOut[26] = vecWorld[2];
	pOut[27] = vecWorld[2];
	pOut[28] = vecWorld[6];
	pOut[29] = vecWorld[4];
	// Back
	pOut[30] = vecWorld[3];
	pOut[31] = vecWorld[5];
	pOut[32] = vecWorld[7];
	pOut[33] = vecWorld[3];
	pOut[34] = vecWorld[7];
	pOut[35] = vecWorld[1];
}

Shadow::Shadow()
{
	m_pShadowManager = NULL;
	m_vPosition = FmVec3(0.0F, 0.0F, 0.0F);
	m_pVertices = NULL;
	m_nVertexNum = 0;
	m_bVisible = true;
	m_pShadowTex = NULL;
	m_pVBuffer = NULL;
	m_pShadowContext = NULL;
	m_bUseSimpleShadow = true;
	m_bSimpleShadow = true;
	m_nBottomColor = VisUtil_ColorToVector4(0xFF343447);
	m_nTopColor = VisUtil_ColorToVector4(0xFFA5A5C6);
	m_fLightDispersion = 0.008F;
	m_bShadowVisible = false;
	m_bInitShadowTex = false;
	m_pVBBufferData = NULL;
}

Shadow::~Shadow()
{
	SAFE_RELEASE(m_pShadowContext);
	SAFE_RELEASE(m_pShadowTex);
	SAFE_RELEASE(m_pVBuffer);
	if(m_pVBBufferData)
	{
		delete [] m_pVBBufferData;
		m_pVBBufferData = NULL;
	}
}

bool Shadow::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}
	
	return true;
}

bool Shadow::Shut()
{
	return true;
}

bool Shadow::Load()
{
	return true;
}

bool Shadow::AddPoly(void* pContext, const FmMat4& mtxModel, 
	const FmVec3* pVertexList, size_t nVertexCount)
{
	Assert(pContext != NULL);
	Assert(nVertexCount == 3);
	Assert(pVertexList != NULL);

	Shadow* pthis = (Shadow*)pContext;
	
	size_t tot_verts = pthis->m_nVertexNum;

	if (tot_verts >= VBUFFER_SIZE - 3)
	{
		return false;
	}

	FmVec3* pVertex = pthis->m_pVertices + tot_verts;

	for(int i = 0; i < 3;i++ )
	{
		FmVec3TransformCoord(&pVertex[i],&pVertexList[i],&mtxModel);
	}

	if (pthis->m_bSimpleShadow)
	{
		// 在使用简易阴影的情况下防止在近乎垂直面上投影
		FmVec3 vVector1 = pVertex[1] - pVertex[0];
		FmVec3 vVector2 = pVertex[2] - pVertex[0];
		FmVec3 vNormal;

		FmVec3Cross(&vNormal, &vVector1, &vVector2);
		FmVec3Normalize(&vNormal, &vNormal);

		if (vNormal.y < 0.2F)
		{
			// 面的倾斜度太大
			return true;
		}
	}

	pthis->m_nVertexNum = tot_verts + 3;

	return true;
}

void Shadow::Update(float seconds)
{
	m_bShadowVisible = false;

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_BindID);

	if (pVisBase)
	{
		// 同步位置
		m_vPosition = pVisBase->GetPosition();
	}
}

void Shadow::Realize(float offset_seconds)
{
	if (!m_bVisible)
	{
		return;
	}

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_BindID);

	if (NULL == pVisBase || !pVisBase->GetVisible())
	{
		return;
	}

	if (NULL == m_pShadowManager)
	{
		return;
	}

	// 决定是否使用简易阴影
	m_bSimpleShadow = m_pShadowManager->GetUseSimpleShadow() 
		|| m_bUseSimpleShadow;

	if (m_bSimpleShadow)
	{
	    m_pShadowShader = m_pShadowManager->GetSimpleShadowShader();
	}
	else
	{
        m_pShadowShader = m_pShadowManager->GetShadowShader();
	}

	if (NULL == m_pVBuffer)
	{
		m_pVBuffer = m_pRender->CreateDynamicVB(
			sizeof(FmVec3) * VBUFFER_SIZE);

		m_pVBBufferData = new FmVec3[VBUFFER_SIZE];

		if (NULL == m_pVBuffer)
		{
			CORE_TRACE("(Shadow::Prepare)create vertex buffer failed");
			return;
		}
	}

	if (!m_bSimpleShadow)
	{
		if (NULL == m_pShadowTex)
		{
			if (m_bInitShadowTex)
			{
				return;
			}

			m_bInitShadowTex = true;

			int texture_size = m_pShadowManager->GetTextureSize();

			m_pShadowTex = m_pRender->CreateColorRT(texture_size, 
				texture_size, TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_FIXED);

			if (NULL == m_pShadowTex)
			{
				CORE_TRACE("(Shadow::Prepare)create shadow texture failed");
				return;
			}
		}

		IDepthRT* pShadowDepth = m_pShadowManager->GetShadowDepth();

		if (NULL == pShadowDepth)
		{
			return;
		}
	}

	IRenderContext* pContext = m_pRender->GetContext();
	// 阳光的方向
	FmVec3 sun_dir = pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);

	FmVec3Normalize(&sun_dir, &sun_dir);

	if (sun_dir.y < 0.1F)
	{
		// 太阳位置太低
		return;
	}

	// 投影物体的位置和半径
	FmVec3 posObj = pVisBase->GetCenter();
	float fRadiusObj = pVisBase->GetRadius();

	if (fRadiusObj < 0.01F)
	{
		return;
	}

	// 影子的半径
	//float shadow_radius = 0.5F;
	float shadow_radius = 0.75F;
	// Build shadow view parameters.
	FmVec3 posShadowView = posObj;
	FmVec3 posShadowViewLookAt = posObj;
	FmVec3 vecShadowViewUp;

	if (!m_bSimpleShadow)
	{
		FmVec2 vec2_sun_dir = FmVec2(sun_dir.x, sun_dir.z);
		float len = FmVec2Length(&vec2_sun_dir);

		shadow_radius += len / sun_dir.y * fRadiusObj;

		posShadowView += sun_dir;
		//posShadowViewLookAt.y += fRadiusObj;// * 0.7F;
		//posShadowView.y += fRadiusObj;

		if (abs(sun_dir.x) + abs(sun_dir.z) < 0.1)
		{
			vecShadowViewUp = FmVec3(1.0F, 0.0F, 0.0F);
		}
		else
		{
			vecShadowViewUp = FmVec3(0.0F, 1.0F, 0.0F);
		}
	}
	else
	{
		//posShadowView.y += fRadiusObj;
		posShadowView.y += 0.1F;
		vecShadowViewUp = FmVec3(1.0F, 0.0F, 0.0F);
	}

	// If the shadow is not in view frustum, not realize shadow.
	FmVec3 dirShadowView = posShadowViewLookAt - posShadowView;

	FmVec3Normalize(&dirShadowView, &dirShadowView);

	bool old_trace_enable = pVisBase->GetTraceEnable();

	// 不要检测影子绑定对象的碰撞
	pVisBase->SetTraceEnable(false);

	float fIntersectResult = pContext->LayerTrace("shadow", posShadowView, 
		posShadowView + dirShadowView * 10.0F);

	pVisBase->SetTraceEnable(old_trace_enable);

	// 投影的中心点
	FmVec3 posIntersect = posShadowView 
		+ (dirShadowView * 10.0F) * fIntersectResult;
	const FmMat4& mtxCamView = pContext->GetCamera().mtxView;
	const FmMat4& mtxCamProj = pContext->GetCamera().mtxDxProj;
	FmPlane camViewCone[6];

	this->FindPlanes(camViewCone, mtxCamView, mtxCamProj);

	for (int i = 0; i < 6; ++i)
	{
		float fDistanceToPlane = FmPlaneDotCoord(&camViewCone[i], 
			&posIntersect);

		if (fDistanceToPlane > 5.0F)
		{
			return;
		}
	}

	// Draw shadow to texture.
	FmMat4 mtxShadowView, mtxShadowProj;

	FmMat4LookAtLH(&mtxShadowView, &posShadowView, &posShadowViewLookAt, 
		&vecShadowViewUp);

	
	FmMat4OrthoLH(&mtxShadowProj, fRadiusObj * 2.0F, 
		fRadiusObj * 2.0F, 0.0F, 100.0F,0);

	if (!m_bSimpleShadow)
	{
		DrawModel(pVisBase, m_pShadowTex, m_pShadowManager->GetShadowDepth(),
			mtxShadowView, mtxShadowProj);
	}

	// Cull scene and draw shadow.
	FmMat4Multiply(&m_mtxTexTransform, &mtxShadowView, &mtxShadowProj);

	m_nVertexNum = 0;
	m_pVertices = (FmVec3*)LockDynamicVB();

	if (NULL == m_pVertices)
	{
		return;
	}

	if (m_bSimpleShadow)
	{
		// 只用一个包围球做拣选
		pContext->LayerCull("shadow", NULL, 0, posIntersect, 
			shadow_radius, AddPoly, this);
	}
	else
	{
		FmPlane shadowViewCone[6];

		this->FindPlanes(shadowViewCone, mtxShadowView, mtxShadowProj);

		pContext->LayerCull("shadow", shadowViewCone, 6, posIntersect, 
			shadow_radius, AddPoly, this);
	}

	UnlockDynamicVB();

	m_pShaderHandle = m_pShadowManager->GetSimpleShadowShaderHandle();

	m_bShadowVisible = true;

	if (m_bShadowVisible && m_nVertexNum >= 3)
	{
		DrawShadowByShader(m_pShadowTex, m_pVBuffer);
	}

	/*
	//调试输出使用
	{
		FmVec3 posList[36];

		BuildViewConeMesh(m_mtxTexTransform,posList);

		struct view_cone_mesh_vertex_t
		{
			FmVec3 pos;
            unsigned int color;
		};

		view_cone_mesh_vertex_t vb[36];

		for(unsigned int x = 0 ; x < 36 ; x++ )
		{
			vb[x].pos = posList[x];
		}

		vb[0].color = vb[1].color = vb[2].color = 0xFFFF0000;
		vb[3].color = vb[4].color = vb[5].color = 0xFFFF0000;
		vb[6].color = vb[7].color = vb[8].color = 0xFFFFFF00;
		vb[9].color = vb[10].color = vb[11].color = 0xFFFFFF00;
		vb[12].color = vb[13].color = vb[14].color = 0xFF00FF00;
		vb[15].color = vb[16].color = vb[17].color = 0xFF00FF00;
		vb[18].color = vb[19].color = vb[20].color = 0xFF0000FF;
		vb[21].color = vb[22].color = vb[23].color = 0xFF0000FF;
		vb[24].color = vb[25].color = vb[26].color = 0xFF00FFFF;
		vb[27].color = vb[28].color = vb[29].color = 0xFF00FFFF;
		vb[30].color = vb[31].color = vb[32].color = 0xFFFF00FF;
		vb[33].color = vb[34].color = vb[35].color = 0xFFFF00FF;

		BuildViewConeMesh( m_mtxTexTransform , posList);

		FmMat4 mtxWorld;
		FmMat4Identity(&mtxWorld);
		m_pRender->DrawDesignLine( mtxWorld ,PRIMITIVE_TRIANGLE,12,vb,sizeof(view_cone_mesh_vertex_t));
	}
	*/
}

FmVec3* Shadow::LockDynamicVB()
{
	return m_pVBBufferData;
}

void Shadow::UnlockDynamicVB()
{
	m_pRender->UpdateDynamicVB(m_pVBuffer,0,m_pVBBufferData,m_pVBuffer->GetSize());
}

void Shadow::RenderShadow(void* pdata)
{
	Shadow* pthis = (Shadow*)pdata;
	ShadowManager::shadow_shaderhandle_t* pShaderHandle = pthis->m_pShaderHandle;
	pShaderHandle->pShader->UsedShader();
	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(pShaderHandle->c_BottomColorHandle,pthis->m_shadow_cb_object.bottom_color);
	pShaderOp->SetParamValue(pShaderHandle->c_fLightDispersionHandle,pthis->m_shadow_cb_object.light_dispersion);
	pShaderOp->SetParamValue(pShaderHandle->c_mtxLightMVPHandle,pthis->m_shadow_cb_object.mtxLightMVP);
	pShaderOp->SetParamValue(pShaderHandle->c_mtxViewProjHandle,pthis->m_shadow_cb_object.mtxWVP);
	pShaderOp->SetParamValue(pShaderHandle->c_TopColorHandle,pthis->m_shadow_cb_object.top_color);

	//获取渲染器状态操作
	IRenderStateOp* pRenderStateOp = pShaderHandle->pShader->GetRenderStateOp();

	IRenderDrawOp*  pRenderDrawOp = pShaderHandle->pShader->GetRenderDrawOp();

	pRenderDrawOp->SetVB( pthis->m_shadow_cb_object.pVBuffer->GetBuffer() );
	pRenderDrawOp->EnableVertexAttribArray(0,true);
	pRenderDrawOp->SetVertexAttribPointer(0,3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(FmVec3),(void*) (0));

	pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ZERO,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_COLOR);

	//fm_sizei vert_count =  pthis->m_shadow_cb_object.pVBuffer->GetSize()/sizeof(FmVec3);

	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLES, 0,(fm_sizei)pthis->m_nVertexNum);

}


bool Shadow::DrawModel(IVisBase* pVisBase, IColorRT* pShadowTex, 
	IDepthRT* pShadowDepth, const FmMat4& view, const FmMat4& proj)
{
	Assert(pVisBase != NULL);
	
	if (NULL == m_pShadowContext)
	{
		m_pShadowContext = m_pRender->CreateContext();
	}

	IRenderContext* pOldContext = m_pRender->GetContext();

	m_pRender->SetContext(m_pShadowContext);
   
	int texture_size = 0;//m_pShadowManager->GetTextureSize();

	m_pShadowContext->SetViewSize(texture_size, texture_size);
	m_pShadowContext->SetViewMatrix(view);
	m_pShadowContext->SetProjMatrix(proj);
	m_pShadowContext->MakeHelperMatrix();
	m_pShadowContext->MakeClipPlane();

//	m_pRender->BeginProjectionSceneBatchs(pShadowTex, pShadowDepth, 
//		0xFFFFFFFF, NULL, NULL);

//	pVisBase->RealizeShadowMap(0.0F);

//	m_pShadowContext->UpdateProjectSceneCB(m_pShadowCB, 
//		pOldContext->GetCamera().vPosition);
//	m_pRender->SetSceneBatchConstBuffer(m_pShadowCB);
//	m_pRender->EndSceneBatchs();

	m_pRender->SetContext(pOldContext);

	return true;
}

void Shadow::DrawShadowByShader(IColorRT* pShadowTex, IDynamicVB* pVBuffer)
{
	IRenderContext* pContext = m_pRender->GetContext();

	//计算距离
	FmMat4 mtxView = pContext->GetCamera().mtxView;

	float camera_z = mtxView._13 * m_vPosition.x + mtxView._23 * m_vPosition.y + mtxView._33 * m_vPosition.z + mtxView._43;
	m_pRender->GetSceneView()->AddBlendBatch( RenderShadow,this,camera_z,"shadow");

	FmMat4 mtxWVP;
	FmMat4 mtxLightMVP;
	FmMat4Transpose(&mtxWVP,&pContext->GetCamera().mtxViewProj);
    FmMat4Transpose(&mtxLightMVP,&m_mtxTexTransform);

	float light_dispersion = m_fLightDispersion;
    FmVec4 top_color = m_nTopColor;
	FmVec4 bottom_color = m_nBottomColor;

	m_shadow_cb_object.mtxWVP = mtxWVP;
	m_shadow_cb_object.bottom_color = bottom_color;
	m_shadow_cb_object.light_dispersion = light_dispersion;
	m_shadow_cb_object.mtxLightMVP = mtxLightMVP;
	m_shadow_cb_object.top_color = top_color;

	m_shadow_cb_object.pVBuffer = pVBuffer;
}

void Shadow::FindPlanes(FmPlane* planes, const FmMat4& view, 
	const FmMat4& proj)
{
	FmVec3 vecWorld[8];
	FmVec3 vecScreen[8];

	vecScreen[0].x = -1.0F; vecScreen[0].y = +1.0F; vecScreen[0].z = 0.0F;
	vecScreen[1].x = -1.0F; vecScreen[1].y = +1.0F; vecScreen[1].z = 1.0F;
	vecScreen[2].x = -1.0F; vecScreen[2].y = -1.0F; vecScreen[2].z = 0.0F;
	vecScreen[3].x = -1.0F; vecScreen[3].y = -1.0F; vecScreen[3].z = 1.0F;
	vecScreen[4].x = +1.0F; vecScreen[4].y = -1.0F; vecScreen[4].z = 0.0F;
	vecScreen[5].x = +1.0F; vecScreen[5].y = -1.0F; vecScreen[5].z = 1.0F;
	vecScreen[6].x = +1.0F; vecScreen[6].y = +1.0F; vecScreen[6].z = 0.0F;
	vecScreen[7].x = +1.0F; vecScreen[7].y = +1.0F; vecScreen[7].z = 1.0F;

	FmMat4 mtxTransform;
	FmMat4Multiply(&mtxTransform,&view, &proj);

	FmMat4Inverse(&mtxTransform,NULL, &mtxTransform);

	for(int i = 0; i < 8; i++ )
	{
		FmVec3TransformCoord(&vecWorld[i],&vecScreen[i],&mtxTransform);
	}

	/*
	for(int i = 0; i < 8; i++ )
	{
		FmVec3Unproject(&vecWorld[i],&vecScreen[i],&proj, &view, NULL);
	}
	*/

	FmPlaneFromPoints(&planes[0], &vecWorld[0], &vecWorld[1], &vecWorld[6]);
	FmPlaneFromPoints(&planes[1], &vecWorld[6], &vecWorld[7], &vecWorld[4]);
	FmPlaneFromPoints(&planes[2], &vecWorld[4], &vecWorld[5], &vecWorld[2]);
	FmPlaneFromPoints(&planes[3], &vecWorld[0], &vecWorld[2], &vecWorld[3]);
	FmPlaneFromPoints(&planes[4], &vecWorld[0], &vecWorld[6], &vecWorld[4]);
	FmPlaneFromPoints(&planes[5], &vecWorld[1], &vecWorld[3], &vecWorld[5]);

	for (int i = 0; i < 6; ++i)
	{
		FmPlaneNormalize(&planes[i], &planes[i]);
	}
}

void Shadow::SetBindID(const PERSISTID& value)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(value);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}
	
	m_BindID = value;
	// 同步位置
	m_vPosition = pVisBase->GetPosition();
}

PERSISTID Shadow::GetBindID() const
{
	return m_BindID;
}

void Shadow::SetLightDispersion(float value)
{
	m_fLightDispersion = value;
}

float Shadow::GetLightDispersion() const
{
	return m_fLightDispersion;
}

void Shadow::SetShadowTopColorString(const char* value)
{
	m_nTopColor = VisUtil_ColorToVector4(VisUtil_ColorToInt(value));
}

result_string Shadow::GetShadowTopColorString() const
{
	return VisUtil_IntToColor(VisUtil_Vector4ToColor(m_nTopColor));
}

void Shadow::SetShadowBottomColorString(const char* value)
{
	m_nBottomColor = VisUtil_ColorToVector4(VisUtil_ColorToInt(value));
}

result_string Shadow::GetShadowBottomColorString() const
{
	return VisUtil_IntToColor(VisUtil_Vector4ToColor(m_nBottomColor));
}

void Shadow::SetUseSimpleShadow(bool value)
{
	m_bUseSimpleShadow = value;
}

bool Shadow::GetUseSimpleShadow() const
{
	return m_bUseSimpleShadow;
}

bool Shadow::SetPosition(float x, float y, float z)
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;

	return true;
}

FmVec3 Shadow::GetPosition() const
{ 
	return m_vPosition; 
}

void Shadow::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Shadow::GetVisible() const
{
	return m_bVisible;
}

bool Shadow::IsViewClip()
{
	IRenderContext* pContext = m_pRender->GetContext();
	
	if (pContext->IsFarClip(m_vPosition, GetRadius()))
	{
		return true;
	}
	
	return pContext->IsViewClip(m_vPosition, GetRadius());
}
