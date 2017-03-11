
#include "model_player.h"
#include "action_player.h"
#include "res_model.h"
#include "math_3d.h"
#include "aux_tool.h"
#include "../visual/i_render.h"
//#include "../visual/i_dx_context.h"
#include "../visual/i_texture.h"
//#include "../visual/dx_scene_batch.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../visual/i_scene_view.h"

extern IRender* g_pRender;
extern bool g_bCheckModelData;
extern bool g_bShowNodeBoundingBox;
/*
extern IDxDepthState* g_pAppliqueMaskDepthState;
extern IDxDepthState* g_pAppliqueDrawDepthState;
extern IDxDepthState* g_pAppliqueClearDepthState;
extern IDxDepthState* g_pZWriteEnableDepthState;
extern IDxDepthState* g_pGlowMaskDepthState;
extern IDxDepthState* g_pGlowDrawDepthState;
extern IDxDepthState* g_pVolumeFogDepthState;
extern IDxRasterState* g_pCullNoneRasterState;
extern IDxRasterState* g_pCullFrontRasterState;
extern IDxBlendState* g_pNormalBlendState;
extern IDxBlendState* g_pBrightBlendState;
extern IDxBlendState* g_pEnhanceBlendState;
extern IDxBlendState* g_pDefaultBlendState;
extern IDxBlendState* g_pInverseBlendState;
extern IDxBlendState* g_pFactorBlendState;
extern IDxBlendState* g_pColorDisableBlendState;
*/

extern void SetShaderValue1i(IShaderProgram* pShader, const char* name, int value);
extern int get_current_alpha_ref(float alpha_ref);

// ��������εĶ�������
static inline void get_single_vertex(node_material_t* pMat, 
	unsigned int tri_id, FmVec3* v0, FmVec3* v1, FmVec3* v2)
{
	vertex_data_t* vb = &(pMat->SingleVB);
	unsigned short* ib = pMat->pLODIB[pMat->nCurLODLevel];
	unsigned int index0 = ib[tri_id + 0];
	unsigned int index1 = ib[tri_id + 1];
	unsigned int index2 = ib[tri_id + 2];

	*v0 = *(FmVec3*)get_vertex_by_index(vb, index0);
	*v1 = *(FmVec3*)get_vertex_by_index(vb, index1);
	*v2 = *(FmVec3*)get_vertex_by_index(vb, index2);
}

// ��������εĶ�������
static inline void get_triangle_vertex(node_material_t* pMat, float* pVB, 
	unsigned int vertex_stride, unsigned int tri_id, FmVec3* v0, 
	FmVec3* v1, FmVec3* v2)
{
	unsigned char* vb = (unsigned char*)pVB;
	unsigned short* ib = pMat->pLODIB[pMat->nCurLODLevel];
	unsigned int index0 = ib[tri_id + 0];
	unsigned int index1 = ib[tri_id + 1];
	unsigned int index2 = ib[tri_id + 2];

	*v0 = *(FmVec3*)&vb[index0 * vertex_stride];
	*v1 = *(FmVec3*)&vb[index1 * vertex_stride];
	*v2 = *(FmVec3*)&vb[index2 * vertex_stride];
}

void CModelPlayer::GetNodeLocationBoundingBox( 
	FmVec3& bbDstMin, FmVec3& bbDstMax, 
	const FmVec3& bbSrcMin, const FmVec3& bbSrcMax)
{
	bbDstMin = bbSrcMin;
	bbDstMax = bbSrcMax;

	if (m_pActionPlayer)
	{
		if (NULL == m_pActionPlayer->GetParentAction())
		{
			return;
		}

		skt_pose_t* pose = m_pActionPlayer->GetSkeletonPose();
		FmVec3 pos;
		FmMat4 mtxRoot;

		Assert(pose != NULL);
		m_pActionPlayer->GetRootBoneOriginPosition(&pos);
		get_current_root_node_tm(pose, &mtxRoot);
		bbDstMin.x += mtxRoot._41 - pos.x;
		bbDstMin.y += mtxRoot._42 - pos.y;
		bbDstMin.z += mtxRoot._43 - pos.z;
		bbDstMax.x += mtxRoot._41 - pos.x;
		bbDstMax.y += mtxRoot._42 - pos.y;
		bbDstMax.z += mtxRoot._43 - pos.z;
	}
}


/*
void CModelPlayer::CommitDepthBatch(node_material_t* pMat, 
	model_node_t* pNode, IDxDynamicCB* pCB)
{
	DxSceneBatch* pBatch = g_pRender->AddDepthBatch("ModelDepth");

	// ���ü�������Ϣ
	SetBatchGeometry(pBatch, pMat, pNode);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	material_info_t* pMatInfo = m_pCurMatInfo;

	if (pMatInfo->bAlphaTest || pMatInfo->bBlendQuality)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetPSTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
	}

	if (pMatInfo->bDoubleSide)
	{
		pBatch->SetRasterState(g_pCullNoneRasterState);
	}

	SelectDepthShader(pBatch, pNode, pMat);
}
*/

/*
void CModelPlayer::CommitGlowBatch(node_material_t* pMat, model_node_t* pNode,
	bool fog_linear, bool fog_exp, DxSceneBatch* pSolidBatch, 
	IDxDynamicCB* pCB)
{
	IDxContext* pContext = g_pRender->GetContext();
	material_info_t* pMatInfo = m_pCurMatInfo;
	DxSceneBatch* pMaskBatch = NULL;
	DxSceneBatch* pBatch = NULL;

	// �Ƿ���Ҫģ��
	bool need_blur = pMatInfo->bGlowBlur && pContext->GetEnableRealizeGlow();

	if (need_blur)
	{
		// ��䷺����ɫ
		pBatch = g_pRender->AddGlowMaskBatch("ModelGlowMask");

		// ���ð�Χ�����ھ����������䷶Χ
		FmVec3 vMin;
		FmVec3 vMax;

		GetNodeLocationBoundingBox(vMin, vMax, pNode->vBoundMin, 
			pNode->vBoundMax);
		// ���ð�Χ�����ھ����������䷶Χ
		pBatch->SetBoundBox(vMin, vMax);

		// �ٴλ���ʵģ
		DxSceneBatch* pColorBatch = g_pRender->AddGlowColorBatch(
			"ModelGlowSolid");

		pColorBatch->Copy(pSolidBatch);
	}
	else
	{
		if (!pMatInfo->bGlowEntire)
		{
			pMaskBatch = g_pRender->AddBlendStencilBatch("ModelGlowMask");
		}

		pBatch = g_pRender->AddBlendBatch("ModelGlowColor");
		
		const FmMat4& mtxView = pContext->GetCamera().mtxView;
		float camera_z = mtxView._13 * m_mtxCurrentTM._41 
			+ mtxView._23 * m_mtxCurrentTM._42 
			+ mtxView._33 * m_mtxCurrentTM._43 + mtxView._43;

		pBatch->SetCameraZ(camera_z);
		
		if (pMatInfo->bGlowEnhance)
		{
			pBatch->SetBlendState(g_pEnhanceBlendState);
		}
		else
		{
			pBatch->SetBlendState(g_pNormalBlendState);
		}
	}
	
	// ���ü�������Ϣ
	SetBatchGeometry(pBatch, pMat, pNode);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	if (pMatInfo->bAlphaTest || pMatInfo->bGlowDiffuse)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetPSTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
	}

	if (pMatInfo->bGlowEmissiveMap)
	{
		ITexture* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;

		if (pTexEmissiveMap != NULL)
		{
			pBatch->SetPSTexture(1, pTexEmissiveMap->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(1, NULL);
		}
	}

	if (pMatInfo->bDoubleSide)
	{
		pBatch->SetRasterState(g_pCullNoneRasterState);
	}

	bool gamma = pContext->GetEnableGammaCorrect();

	SelectGlowShader(pBatch, pNode, pMat, fog_linear, fog_exp, gamma);

	if (pMaskBatch)
	{
		pMaskBatch->Copy(pBatch);
		pMaskBatch->SetDepthState(g_pGlowMaskDepthState);
		pMaskBatch->SetBlendState(g_pColorDisableBlendState);
		
		pBatch->SetRasterState(g_pCullFrontRasterState);
		pBatch->SetDepthState(g_pGlowDrawDepthState);
	}
}
*/

/*
// �ύ��Ⱦ�ٶ�����
void CModelPlayer::CommitVelocityBatch(node_material_t* pMat, 
	model_node_t* pNode, IDxDynamicCB* pCB)
{
	DxSceneBatch* pBatch = g_pRender->AddVelocityBatch("VelocityModel");

	// ���ü�������Ϣ
	if (!SetBatchGeometry(pBatch, pMat, pNode))
	{
		//return;
	}

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	material_info_t* pMatInfo = m_pCurMatInfo;
	IDxContext* pContext = g_pRender->GetContext();
	const dx_camera_t& camera = pContext->GetCamera();
	const FmVec3& refer_pos = pContext->GetReferPosition();

//	FmMat4 mtxWVP;
//
//	FmMatrixMultiplyTranspose(&mtxWVP, &mtxWorld, 
//		&camera.mtxViewProjRelative);
//
//	pBatch->SetVSConst(0, &mtxWVP, 4);

	FmMat4 mtxWorld = m_mtxCurrentTM;
	FmMat4 mtxWorldInverse;
	FmMat4 mtxLocal;
	
	mtxWorld._41 -= refer_pos.x;
	mtxWorld._42 -= refer_pos.y;
	mtxWorld._43 -= refer_pos.z;
	FmXMatrixInverse(&mtxWorldInverse, NULL, &m_mtxWorldTM);
	FmMatrixMultiply(&mtxLocal, &mtxWorld, &mtxWorldInverse);

	FmMat4 mtxPrevWorld = m_mtxPrevWorldTM;
	FmMat4 mtxPrevView;
	FmMat4 mtxPrevViewInverse;
	FmMat4 mtxPrevViewProj;
	FmMat4 mtxPrevWVP;

	mtxPrevWorld._41 -= refer_pos.x;
	mtxPrevWorld._42 -= refer_pos.y;
	mtxPrevWorld._43 -= refer_pos.z;
	FmMatrixMultiply(&mtxPrevWorld, &mtxLocal, &mtxPrevWorld);
	FmXMatrixInverse(&mtxPrevViewInverse, NULL, &camera.mtxView);
	mtxPrevViewInverse._41 -= refer_pos.x;
	mtxPrevViewInverse._42 -= refer_pos.y;
	mtxPrevViewInverse._43 -= refer_pos.z;
	FmXMatrixInverse(&mtxPrevView, NULL, &mtxPrevViewInverse);
	FmMatrixMultiply(&mtxPrevViewProj, &mtxPrevView, &camera.mtxProj);
	FmMatrixMultiplyTranspose(&mtxPrevWVP, &mtxPrevWorld, &mtxPrevViewProj);

//	pBatch->SetVSConst(4, &mtxPrevWVP, 4);

//	if (pMat->pMultiGPUVB != NULL)
//	{
//		float blend_weight[4] = { m_fCurrentFrameOffset, 0.0F, 0.0F, 0.0F };
//
//		pBatch->SetVSConst(8, blend_weight, 1);
//	}

	if (pMatInfo->bAlphaTest)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetPSTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
		
//		float ref_value = get_current_alpha_ref(pMatInfo->fAlphaRef) / 255.0F;
//		float alpha_ref[4] = { ref_value, 0.0F, 0.0F, 0.0F };
//
//		pBatch->SetPSConst(0, alpha_ref, 1);
	}

//	if (node->nType == FXNODE_TYPE_SKIN)
//	{
//		FmVec4* bone_matrices = CreateBoneMatrices(pMat);
//
//		if (NULL == bone_matrices)
//		{
//			// ����δ��ʼ��
//			//pBatch->SetCancel("CreateBoneMatrices failed");
//			return;
//		}
//
//		pBatch->SetVSConst(CV_BONE_LIST, bone_matrices, 
//			pMat->nMappedBoneCount * 3);
//	}

	if (!SelectVelocityShader(pBatch, pNode, pMat)) 
	{
		//pBatch->SetCancel("SelectVelocityShader failed!");
		//return;
	}

	if (pMatInfo->bDoubleSide)
	{
		//pBatch->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pBatch->SetRasterState(g_pCullNoneRasterState);
	}

	// ������һ֡��λ�þ���
	m_mtxPrevWorldTM = m_mtxWorldTM;
}
*/

void CModelPlayer::DrawMaterialGBuffer(node_material_t* pMat, 
	model_node_t* pNode, const FmVec4* bone_matrices)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();


	/*
	DxSceneBatch* pBatch;

	if ((pNode->nType == FXNODE_TYPE_SKIN) || m_bLive)
	{
		// ������ʵ������
		pBatch = g_pRender->AddLiveSolidBatch("ModelGBuffer");
	}
	else
	{
		pBatch = g_pRender->AddSolidBatch("ModelGBuffer");
	}

	// ���ü�������Ϣ 
	SetBatchGeometry(pBatch, pMat, pNode);

	IDxDynamicCB* pCB = GetObjectCB(pMat, bone_matrices);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	bool use_tessellation = pMatInfo->bTessellationEnable 
		&& (pContext->GetInt(IDxContext::I_USE_TESSELLATION) != 0);

	if (use_tessellation)
	{
		pBatch->SetType(DxSceneBatch::TYPE_TESSELLATION);
		pBatch->SetHSConstBuffer(0, pCB);
		pBatch->SetDSConstBuffer(0, pCB);
		
		// λ����ͼ
		ITexture* pTexDisplaceMap = pMatInfo->DisplacementMap.pTex;

		if (pTexDisplaceMap)
		{
			pBatch->SetDSTexture(0, pTexDisplaceMap->GetShaderTex());
		}
		else
		{
			// ʹ�÷���ͼ��ALPHAͨ����Ϊ�߶�ͼ
			ITexture* pTexBumpMap = pMatInfo->BumpMap.pTex;

			if (pTexBumpMap)
			{
				pBatch->SetDSTexture(0, pTexBumpMap->GetShaderTex());
			}
		}
	}

	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		// ��������ͼ
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetDiffuseTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
	}

	if (pMatInfo->bBumpMapEnable)
	{
		// ������ͼ
		ITexture* pTexBumpMap = pMatInfo->BumpMap.pTex;

		if (pTexBumpMap)
		{
			pBatch->SetPSTexture(1, pTexBumpMap->GetShaderTex());
		}
	}

	if (pMatInfo->bSpecularEnable)
	{
		// �߹���ͼ
		ITexture* pTexSpecularMap = pMatInfo->SpecularMap.pTex;

		if (pTexSpecularMap)
		{
			pBatch->SetPSTexture(2, pTexSpecularMap->GetShaderTex());
		}
		else
		{
			// �߹�������ͼ 
			ITexture* pTexSpecularLevelMap = 
				pMatInfo->SpecularLevelMap.pTex;

			if (pTexSpecularLevelMap != NULL)
			{
				pBatch->SetPSTexture(2, 
					pTexSpecularLevelMap->GetShaderTex());
			}
		}
	}

//	if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
//	{
//		// ����������ͼ
//		if (NULL == pMatInfo->ReflectionMap.pTex)
//		{
//			// ����ʹ��ȫ�ֻ�����ͼ
//			ITexture* pGlobalEnvTex = 
//				pContext->GetGlobalEnvironmentCubeTex();
//	
//			if (pGlobalEnvTex)
//			{
//				pBatch->SetFlags(DxSceneBatch::FLAG_REFLECTION);
//			}
//		}
//	}

	if (pMatInfo->bDoubleSide)
	{
		// ˫��ģ��
		pBatch->SetRasterState(g_pCullNoneRasterState);
	}

	bool gamma = pContext->GetEnableGammaCorrect();

	SelectGBufferShader(pBatch, pNode, pMat, gamma, use_tessellation);

	if (pContext->GetEnableOcclusionQuery())
	{
		SetBatchOcclusionQuery(pBatch, pMat, pNode);
	}
	*/
}

void CModelPlayer::DrawMaterialEarlyZ(const MatInfo* info)
{
	node_material_t* pMat = info->pMat;
	model_node_t* pNode = info->pNode;
	const FmVec4* bone_matrices = info->bone_matrices;

	IRenderDrawOp* pDrawOp = g_pRender->GetRenderDrawOp();

	pDrawOp->SetVB( pMat->pSingleGPUVB->GetBuffer());
	pDrawOp->SetIB( pMat->pGPUIB->GetBuffer());

	if (!CreateVDecl(pMat, pNode, false))
	{
		pDrawOp->SetVB( 0 );
		pDrawOp->SetIB( 0 );
		return;
	}

	material_info_t* pMatInfo = info->pMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();
		
	// ʵʱ��Ӱ
	bool dynamic_shadow = false;
	
	// ���������Ƿ���
	bool reflect_enable = false;

	// ���򻷾���
	bool sphere_ambient = pMatInfo->bSphereAmbient;

	if (!sphere_ambient)
	{
		if (pNode->nType == FXNODE_TYPE_SKIN)
		{
			if (pContext->GetInt(IRenderContext::I_SKIN_SPHERE_AMBIENT) != 0)
			{
				// ������������ģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
		else if (pNode->nType == FXNODE_TYPE_MESH)
		{
			// �ر���Ҷ�İ�����AO
			if (((pNode->nModelInfo & MODEL_TREELEAF_FLAG) == 0)
				&& (pContext->GetInt(IRenderContext::I_MODEL_SPHERE_AMBIENT) != 0))
			{
				// ������ͨģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
	}

	
	// �Ƿ�����̬�����µ�������Դ����
	bool point_light = (!dynamic_shadow)
		&& (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_LIGHT) != 0)
		&& (pNode->nType == FXNODE_TYPE_SKIN);
	
	// ָ����Ч
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
	
	// ������Ч
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;

	// �߶���Ч
	bool height_fog = pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0;

	bool camera_light = (pNode->nType == FXNODE_TYPE_SKIN) 
		&& (pContext->GetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT) != 0);
	
	bool gamma = false;
	bool use_clip_plane = pContext->GetInt(IRenderContext::I_USE_CLIP_PLANE) != 0;

	bool prelight = false;
	bool fix_shadow = false;

	bool dynamic_lighting = true;;
	ITexture* pTexLight;
	float light_map_param[4] = { 1.0F, 1.0F, 0.0F, 0.0F };
	UseLightMap(pMat, dynamic_lighting, pTexLight, light_map_param);
	bool filter = pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO  && info->pMatInfo->bFilterEnable;

	IShaderProgram* pShader = SelectShader(info, pNode, pMat, dynamic_lighting, dynamic_shadow, fog_linear, 
		fog_exp, height_fog, prelight, reflect_enable, fix_shadow, sphere_ambient, 
		point_light, 255, false, camera_light, false, gamma, filter, use_clip_plane, true); 

	pShader->UsedShader();
	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();
	IRenderDrawOp* pRenderDrawOp = pShader->GetRenderDrawOp();

	// ������Ⱦ״̬
	if (pMatInfo->bDoubleSide)
		pRenderStateOp->EnableCullFace(false);
	else
		pRenderStateOp->EnableCullFace(true);

	SetModelTexture(info, pShader, pMat, pTexLight);

	SetShaderConstValue(pShader, info);

	if(pTexLight)
	{
		ShaderManager::Inst().SetShaderValue4f(c_LightMapParam, light_map_param);
		ShaderManager::Inst().SetShaderValue3f(c_vLightMapColorScale, GetMaterialLigthMapColorRangeScale(pMat->nIndexInModel) * 2.0f);//ͳһ���ϰ汾����ͼ����ɫ��Χ���ż��㷽ʽ
	}
	
	int ib_count = pMat->pLODIndexCount[pMat->nCurLODLevel];
	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,ib_count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[0]);

	// �ظ���Ⱦ״̬
	if (pMatInfo->bDoubleSide)
		pRenderStateOp->EnableCullFace(true);
}

//model draw�ӿ�
void CModelPlayer::DrawMaterialSolid(const MatInfo* info, bool onlyzpass, bool onlycolor)
{
	node_material_t* pMat = info->pMat;
	model_node_t* pNode = info->pNode;
	const FmVec4* bone_matrices = info->bone_matrices;

	IRenderDrawOp* pDrawOp = g_pRender->GetRenderDrawOp();

	//glBindBuffer(GL_ARRAY_BUFFER, pMat->pSingleGPUVB->GetBuffer());
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMat->pGPUIB->GetBuffer());
	pDrawOp->SetVB( pMat->pSingleGPUVB->GetBuffer());
	pDrawOp->SetIB( pMat->pGPUIB->GetBuffer());

	if (!CreateVDecl(pMat, pNode, false))
	{
		/*
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		*/
		pDrawOp->SetVB( 0 );
		pDrawOp->SetIB( 0 );
		return;
	}

	material_info_t* pMatInfo = info->pMatInfo;
		//m_pCurMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();
		
	// ʵʱ��Ӱ
	bool dynamic_shadow = (pContext->GetEnableDynamicShadow() && GetReceiveShadow());

	//if (dynamic_shadow)
	//{
	//	// ʵʱ��Ӱͼ
	//	pBatch->SetPSTexture(3, pContext->GetDynamicShadowRT());
	//}

	// ���������Ƿ���
	bool reflect_enable = false;

	if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
	{
		// ����������ͼ
		ITexture* pTexReflectionMap = pMatInfo->ReflectionMap.pTex;

		if (pTexReflectionMap)
		{
//			pBatch->SetPSTexture(4, pTexReflectionMap->GetShaderTex());
//			reflect_enable = true;
		}
		else
		{
			// ����ʹ��ȫ�ֻ�����ͼ
			/*
			ITexture* pGlobalEnvTex = 
				pContext->GetGlobalEnvironmentCubeTex();

			if (pGlobalEnvTex)
			{
				// Ҳ��Ҫ��٤��У��
				pBatch->SetPSTexture(4, pGlobalEnvTex->GetShaderTex());
				reflect_enable = true;
			}*/
		}
	}

	if (pMatInfo->bEmissiveEnable || pMatInfo->bSkinEffect)
	{
		// �Է�����ͼ��α�����ͼ
		ITexture* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;

		if (pTexEmissiveMap)
		{
//			pBatch->SetPSTexture(6, pTexEmissiveMap->GetShaderTex());
		}
	}

	// �ӳٹ���

	/*
	bool prelight = pContext->GetEnableLightPrepass();

	if (prelight)
	{
		pBatch->SetPSTexture(7, pContext->GetLightRT());
	}*/


	// �������������ʱ����Ӱ��Ե����
	/*
	bool fix_shadow = dynamic_shadow && pContext->GetEnableFixShadow()
		&& (g_pRender->GetMultiSampleType() > 1);

	if (fix_shadow)
	{
		pBatch->SetPSTexture(8, pContext->GetDepthRT());
	}

	// ����ʵ��ģ�Ͳ�������λЧ��
	if (pMatInfo->bPhaseMonitor)
	{
		if (pContext->GetEnableRealizePhaseRT())
		{
			pBatch->SetPSTexture(9, pContext->GetPhaseRT());
		}
	}
	*/

	// ���򻷾���
	bool sphere_ambient = pMatInfo->bSphereAmbient;

	if (!sphere_ambient)
	{
		if (pNode->nType == FXNODE_TYPE_SKIN)
		{
			if (pContext->GetInt(IRenderContext::I_SKIN_SPHERE_AMBIENT) != 0)
			{
				// ������������ģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
		else if (pNode->nType == FXNODE_TYPE_MESH)
		{
			// �ر���Ҷ�İ�����AO
			if (((pNode->nModelInfo & MODEL_TREELEAF_FLAG) == 0)
				&& (pContext->GetInt(IRenderContext::I_MODEL_SPHERE_AMBIENT) != 0))
			{
				// ������ͨģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
	}


	
	// �Ƿ�����̬�����µ�������Դ����
	bool point_light = (!dynamic_shadow)
		&& (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_LIGHT) != 0)
		&& (pNode->nType == FXNODE_TYPE_SKIN);

	// ָ����Ч
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;

	// ������Ч
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;

	// �߶���Ч
	bool height_fog = pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0;

	bool camera_light = (pNode->nType == FXNODE_TYPE_SKIN) 
		&& (pContext->GetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT) != 0);

	//bool gamma = pContext->GetEnableGammaCorrect();
	bool gamma = false;
	bool use_clip_plane = pContext->GetInt(IRenderContext::I_USE_CLIP_PLANE) != 0;

	bool prelight = false;
	bool fix_shadow = false;

	bool dynamic_lighting = true;;
	ITexture* pTexLight;
	float light_map_param[4] = { 1.0F, 1.0F, 0.0F, 0.0F };
	UseLightMap(pMat, dynamic_lighting, pTexLight, light_map_param);
	bool filter = pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO  && info->pMatInfo->bFilterEnable;

	IShaderProgram* pShader = SelectShader(info, pNode, pMat, dynamic_lighting, dynamic_shadow, fog_linear, 
		fog_exp, height_fog, prelight, reflect_enable, fix_shadow, sphere_ambient, 
		point_light, 255, false, camera_light, false, gamma, filter, use_clip_plane, onlyzpass, onlycolor);

	//glUseProgram(pShader->GetProgram());
	pShader->UsedShader();
	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();
	IRenderDrawOp* pRenderDrawOp = pShader->GetRenderDrawOp();

	// ������Ⱦ״̬
	if (pMatInfo->bDoubleSide)
	{
		//glDisable(GL_CULL_FACE);
		pRenderStateOp->EnableCullFace(false);
	//		pBatch->SetRasterState(g_pCullNoneRasterState);
	}
	else
	{
		pRenderStateOp->EnableCullFace(true);
	}

	SetModelTexture(info, pShader, pMat, pTexLight);
	SetShaderConstValue(pShader, info);

	if(pTexLight)
	{
		ShaderManager::Inst().SetShaderValue4f(c_LightMapParam, light_map_param);
		ShaderManager::Inst().SetShaderValue3f(c_vLightMapColorScale, GetMaterialLigthMapColorRangeScale(pMat->nIndexInModel) * 2.0f);//ͳһ���ϰ汾����ͼ����ɫ��Χ���ż��㷽ʽ
	}

	int ib_count = pMat->pLODIndexCount[pMat->nCurLODLevel];

	if(info->pMatInfo->bBlendQuality)
		//������Ⱦ��������ϣ�����д������ʹ�ñ༭����Ĳ�������
		pRenderStateOp->EnableDepthWrite(true);
	else
		pRenderStateOp->EnableDepthWrite(!pMatInfo->bNoZWrite);
	
	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,ib_count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[0]);


	if(pMatInfo->bDoubleSide)
	{
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		pRenderStateOp->EnableCullFace(true);
  //      glCullFace(GL_FRONT);
	}
	
	/*
	if (pContext->GetEnableOcclusionQuery())
	{
		SetBatchOcclusionQuery(pBatch, pMat, pNode);
	}

	// ģ�ͷ��⣬�и���ɫ��͸��ʱ����ʾ
	if (pMatInfo->bGlowEnable)
	{
		CommitGlowBatch(pMat, pNode, fog_linear, fog_exp, pBatch, pCB);
	}*/
}

void CModelPlayer::DrawMaterialKeepOut(const MatInfo* info)
{
	node_material_t* pMat = info->pMat;
	model_node_t* pNode = info->pNode;
	const FmVec4* bone_matrices = info->bone_matrices;

	IRenderDrawOp* pDrawOp = g_pRender->GetRenderDrawOp();
	pDrawOp->SetVB( pMat->pSingleGPUVB->GetBuffer());
	pDrawOp->SetIB( pMat->pGPUIB->GetBuffer());

	if (!CreateVDecl(pMat, pNode, false))
	{
		pDrawOp->SetIB( 0 );
		pDrawOp->SetVB( 0 );
		return;
	}

	material_info_t* pMatInfo = info->pMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();
		
	// ʵʱ��Ӱ
	bool dynamic_shadow = false;

	// ���������Ƿ���
	bool reflect_enable = false;

	// ���򻷾���
	bool sphere_ambient = false;

	// �Ƿ�����̬�����µ�������Դ����
	bool point_light = (!dynamic_shadow)
		&& (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_LIGHT) != 0)
		&& (pNode->nType == FXNODE_TYPE_SKIN);
	// ָ����Ч
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;

	// ������Ч
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;

	// �߶���Ч
	bool Height_fog = pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0;

	bool camera_light = (pNode->nType == FXNODE_TYPE_SKIN) 
		&& (pContext->GetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT) != 0);
	//bool gamma = pContext->GetEnableGammaCorrect();
	bool gamma = false;
	bool use_clip_plane = pContext->GetInt(IRenderContext::I_USE_CLIP_PLANE) != 0;

	bool dynamic_lighting = true;
	ITexture* pTexLight = NULL;
	float light_map_param[4] = { 1.0F, 1.0F, 0.0F, 0.0F };
	UseLightMap(pMat, dynamic_lighting, pTexLight, light_map_param);

	bool prelight = false;
	bool fix_shadow = false;
	bool filter = pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO  && info->pMatInfo->bFilterEnable;

	IShaderProgram* pShader = SelectShader(info, pNode, pMat, dynamic_lighting, dynamic_shadow, fog_linear, 
		fog_exp, Height_fog, prelight, reflect_enable, fix_shadow, sphere_ambient, 
		point_light, 255, false, camera_light, true, gamma, filter, use_clip_plane, false, false, true);

	pShader->UsedShader();

	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();
	IRenderDrawOp*  pRenderDrawOp = pShader->GetRenderDrawOp();

	SetModelTexture(info, pShader, pMat, pTexLight);
	SetShaderConstValue(pShader, info);
	
	if(pTexLight)
	{
		ShaderManager::Inst().SetShaderValue4f(c_LightMapParam, light_map_param);
		ShaderManager::Inst().SetShaderValue3f(c_vLightMapColorScale, GetMaterialLigthMapColorRangeScale(pMat->nIndexInModel) * 2.0f);//ͳһ���ϰ汾����ͼ����ɫ��Χ���ż��㷽ʽ
	}

	int ib_count = pMat->pLODIndexCount[pMat->nCurLODLevel];

	bool old_depth_write = pRenderStateOp->IsEnableDepthWrite();

	pRenderStateOp->EnableDepthWrite(false);

	bool old_depth_test = pRenderStateOp->IsEnableDepthTest();
	pRenderStateOp->EnableDepthTest(true);

	pRenderStateOp->EnableBlend(true);
	pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_SRC_ALPHA , IRenderStateOp::BLEND_COLOR_ONE);
	pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_SRC_COLOR , IRenderStateOp::BLEND_COLOR_ONE);
	pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_DST_COLOR , IRenderStateOp::BLEND_COLOR_ONE);

	IRenderStateOp::DEPTH_TEST_FUNC testfunc = pRenderStateOp->GetDepthTestFunc();
	pRenderStateOp->SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_GREATER);

	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,ib_count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[0]);

	pRenderStateOp->SetDepthTestFunc(testfunc);
	pRenderStateOp->EnableDepthTest(old_depth_test);
	pRenderStateOp->EnableDepthWrite(old_depth_write);
}

void CModelPlayer::DrawMaterialBlend(const MatInfo* info)
{
	node_material_t* pMat = info->pMat;
	model_node_t* pNode = info->pNode;
	const FmVec4* bone_matrices = info->bone_matrices;

	IRenderDrawOp* pDrawOp = g_pRender->GetRenderDrawOp();
	//glBindBuffer(GL_ARRAY_BUFFER, pMat->pSingleGPUVB->GetBuffer());
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMat->pGPUIB->GetBuffer());
	pDrawOp->SetVB( pMat->pSingleGPUVB->GetBuffer());
	pDrawOp->SetIB( pMat->pGPUIB->GetBuffer());
	
	if (!CreateVDecl(pMat, pNode, false))
	{
		/*
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		*/
		pDrawOp->SetIB( 0 );
		pDrawOp->SetVB( 0 );
		return;
	}

	material_info_t* pMatInfo = info->pMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();
		
	// ʵʱ��Ӱ
	bool dynamic_shadow = (pContext->GetEnableDynamicShadow() && GetReceiveShadow());
	/*
	bool dynamic_shadow = pContext->GetEnableDynamicShadow()
		&& pContext->GetEnableRealizeDepth();

	if (dynamic_shadow)
	{
		// ʵʱ��Ӱͼ
		pBatch->SetPSTexture(3, pContext->GetDynamicShadowRT());
	}
	*/

	// ���������Ƿ���
	bool reflect_enable = false;

	if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
	{
		// ����������ͼ
		ITexture* pTexReflectionMap = pMatInfo->ReflectionMap.pTex;

		if (pTexReflectionMap)
		{
			reflect_enable = true;
		}
		else
		{
			// ����ʹ��ȫ�ֻ�����ͼ
			/*
			ITexture* pGlobalEnvTex = 
				pContext->GetGlobalEnvironmentCubeTex();

			if (pGlobalEnvTex)
			{
				// Ҳ��Ҫ��٤��У��
				pBatch->SetPSTexture(4, pGlobalEnvTex->GetShaderTex());
				reflect_enable = true;
			}*/
		}
	}

	// ���򻷾���
	bool sphere_ambient = pMatInfo->bSphereAmbient;

	if (!sphere_ambient)
	{
		if (pNode->nType == FXNODE_TYPE_SKIN)
		{
			if (pContext->GetInt(IRenderContext::I_SKIN_SPHERE_AMBIENT) != 0)
			{
				// ������������ģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
		else if (pNode->nType == FXNODE_TYPE_MESH)
		{
			// �ر���Ҷ�İ�����AO
			if (((pNode->nModelInfo & MODEL_TREELEAF_FLAG) == 0)
				&& (pContext->GetInt(IRenderContext::I_MODEL_SPHERE_AMBIENT) != 0))
			{
				// ������ͨģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
	}
	
	// �Ƿ�����̬�����µ�������Դ����
	bool point_light = (!dynamic_shadow)
		&& (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_LIGHT) != 0)
		&& (pNode->nType == FXNODE_TYPE_SKIN);

	// ָ����Ч
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;

	// ������Ч
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;

	// �߶���Ч
	bool height_fog = pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0;

	bool camera_light = (pNode->nType == FXNODE_TYPE_SKIN) 
		&& (pContext->GetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT) != 0);
	//bool gamma = pContext->GetEnableGammaCorrect();
	bool gamma = false;
	bool use_clip_plane = pContext->GetInt(IRenderContext::I_USE_CLIP_PLANE) != 0;

	bool dynamic_lighting = true;
	ITexture* pTexLight = NULL;
	float light_map_param[4] = { 1.0F, 1.0F, 0.0F, 0.0F };
	UseLightMap(pMat, dynamic_lighting, pTexLight, light_map_param);

	bool prelight = false;
	bool fix_shadow = false;
	bool filter = pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO  && info->pMatInfo->bFilterEnable;

	IShaderProgram* pShader = SelectShader(info, pNode, pMat, dynamic_lighting, dynamic_shadow, fog_linear, 
		fog_exp, height_fog, prelight, reflect_enable, fix_shadow, sphere_ambient, 
		point_light, 255, false, camera_light, true, gamma, filter, use_clip_plane);

	pShader->UsedShader();

	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();
	IRenderDrawOp*  pRenderDrawOp = pShader->GetRenderDrawOp();

		// ������Ⱦ״̬
	if (pMatInfo->bDoubleSide)
	{
		pRenderStateOp->EnableCullFace(false);
	}
	else
	{
		pRenderStateOp->EnableCullFace(true);
	}

	SetModelTexture(info, pShader, pMat, pTexLight);
	SetShaderConstValue(pShader, info);
	
	if(pTexLight)
	{
		ShaderManager::Inst().SetShaderValue4f(c_LightMapParam, light_map_param);
		ShaderManager::Inst().SetShaderValue3f(c_vLightMapColorScale, GetMaterialLigthMapColorRangeScale(pMat->nIndexInModel) * 2.0f);//ͳһ���ϰ汾����ͼ����ɫ��Χ���ż��㷽ʽ
	}

	int ib_count = pMat->pLODIndexCount[pMat->nCurLODLevel];

	// ģ�͸�����ɫ
	unsigned int model_alpha = (info->nColor >> 24) & 0xFF;

	if(pMatInfo->bBlendQuality)
	{
		pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_SRC_ALPHA , IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA);
	}
	else if	(pMat->MatInfo.bBlend)
	{
		pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_SRC_ALPHA , IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA );
	}
	else if (pMatInfo->bBlendEnhance)
	{
		pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_SRC_ALPHA , IRenderStateOp::BLEND_COLOR_ONE );
	}
	else if (pMatInfo->bOpacityEnable)
	{
		pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_SRC_ALPHA , IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA);
	}
	else if (model_alpha != 255)
	{
		FmVec4 color(model_alpha/255.0f, model_alpha/255.0f, model_alpha/255.0f, model_alpha/255.0f);

        pRenderStateOp->SetBlendColor( color);
		pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_CONSTANT_COLOR, IRenderStateOp::BLEND_COLOR_ONE_MINUS_CONSTANT_COLOR );
		pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_CONSTANT_ALPHA, IRenderStateOp::BLEND_COLOR_ONE_MINUS_CONSTANT_ALPHA );
       
	}
	
	bool old_depth_write = pRenderStateOp->IsEnableDepthWrite();

	if(info->pMatInfo->bBlendQuality)
		//������Ⱦ��������ϣ�����д������ʹ�ñ༭����Ĳ�������
		pRenderStateOp->EnableDepthWrite(false);
	else
		pRenderStateOp->EnableDepthWrite(!pMatInfo->bNoZWrite);

	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,ib_count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[0]);

	//�ظ�˫����ʵ���Ⱦ״̬
	if (pMatInfo->bDoubleSide)
	{
		pRenderStateOp->EnableCullFace(true);
	}

	if(pMatInfo->bNoZWrite)
	{
		pRenderStateOp->EnableDepthWrite(old_depth_write);
	}
}

void CModelPlayer::DrawMaterialAlphaTest(const MatInfo* info)
{
	DrawMaterialSolid(info);
}

void CModelPlayer::DrawMaterialWater(node_material_t* pMat, 
	model_node_t* pNode, const FmVec4* bone_matrices)
{
	/*
	if (!g_pRender->GetShowWaterModel())
	{
		return;
	}

	material_info_t* pMatInfo = m_pCurMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();
	/*
	DxSceneBatch* pBatch = g_pRender->AddWaterBatch("ModelWater");

	// ���ü�������Ϣ 
	SetBatchGeometry(pBatch, pMat, pNode);

	IDxDynamicCB* pCB = GetObjectCB(pMat, bone_matrices);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	// ��������ͼ
	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetPSTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
	}

	if (pMatInfo->bBumpMapEnable)
	{
		// ������ͼ
		ITexture* pTexBumpMap = pMatInfo->BumpMap.pTex;

		if (pTexBumpMap)
		{
			pBatch->SetPSTexture(1, pTexBumpMap->GetShaderTex());
		}
	}

	if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
	{
		// ����������ͼ
		ITexture* pTexReflectionMap = pMatInfo->ReflectionMap.pTex;

		if (pTexReflectionMap)
		{
			pBatch->SetPSTexture(4, pTexReflectionMap->GetShaderTex());
		}
		else
		{
			// ����ʹ��ȫ�ֻ�����ͼ
			ITexture* pGlobalEnvTex = 
				pContext->GetGlobalEnvironmentCubeTex();

			if (pGlobalEnvTex)
			{
				// Ҳ��Ҫ��٤��У��
				pBatch->SetPSTexture(4, pGlobalEnvTex->GetShaderTex());
			}
		}
	}

	if (pMatInfo->bSpecularEnable)
	{
		// �߹���ͼ
		ITexture* pTexSpecularMap = pMatInfo->SpecularMap.pTex;

		if (pTexSpecularMap)
		{
			pBatch->SetPSTexture(5, pTexSpecularMap->GetShaderTex());
		}
		else
		{
			// �߹�������ͼ 
			ITexture* pTexSpecularLevelMap = pMatInfo->SpecularLevelMap.pTex;

			if (pTexSpecularLevelMap)
			{
				pBatch->SetPSTexture(5, pTexSpecularLevelMap->GetShaderTex());
			}
		}
	}

	if (pMatInfo->bEmissiveEnable)
	{
		// �Է�����ͼ
		ITexture* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;

		if (pTexEmissiveMap)
		{
			pBatch->SetPSTexture(6, pTexEmissiveMap->GetShaderTex());
		}
	}

	pBatch->SetPSTexture(3, pContext->GetRealizeTempRT());
	//if (pContext->GetEnableGammaCorrect() && !pContext->GetEnableHDR())
	//{
	//	// ��HDR״̬����Ҫ٤��У��
	//	pBatch->SetPSTexture(3, pContext->GetRealizeTempRT());
	//}
	//else
	//{
	//	pBatch->SetPSTexture(3, pContext->GetRealizeTempRT());
	//}

	bool depth_map = pContext->GetEnableRealizeDepth();
	bool fog_exp = pContext->GetInt(IDxContext::I_FOG_EXP) != 0;
	bool fog_linear = pContext->GetInt(IDxContext::I_FOG_LINEAR) != 0;
	bool refraction = pContext->GetEnableRealizeTempRT();
	bool gamma = pContext->GetEnableGammaCorrect();

	if (!refraction)
	{
		pBatch->SetBlendState(g_pDefaultBlendState);
	}
	else
	{
		pBatch->SetRasterState(g_pCullNoneRasterState);
	}

	SelectWaterSurfaceShader(pBatch, pNode, pMat, fog_linear, fog_exp, 
		depth_map, refraction, gamma);
	*/
}

void CModelPlayer::DrawMaterialWaterClip(node_material_t* pMat, 
	model_node_t* pNode, const FmVec4* bone_matrices)
{
	/*
	DxSceneBatch* pBatch = g_pRender->AddWaterClipBatch("ModelWaterClip");

	// ���ü�������Ϣ 
	SetBatchGeometry(pBatch, pMat, pNode);

	IDxDynamicCB* pCB = GetObjectCB(pMat, bone_matrices);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);
	pBatch->SetRasterState(g_pCullNoneRasterState);

	// ����Ҫ����һ��SHADER
	SelectDepthShader(pBatch, pNode, pMat);
	*/
}

void CModelPlayer::DrawMaterialApplique(node_material_t* pMat, 
	model_node_t* pNode, const FmVec4* bone_matrices)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	IRenderContext* pContext = g_pRender->GetContext();

	/*
	// ģ������
	DxSceneBatch* pBatch = g_pRender->AddNoLiveStencilBatch("ModelApplique");

	// ���ü�������Ϣ 
	SetBatchGeometry(pBatch, pMat, pNode);

	IDxDynamicCB* pCB = GetObjectCB(pMat, bone_matrices);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	// ��������ͼ
	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetDiffuseTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
	}

	// ָ����Ч
	bool fog_exp = pContext->GetInt(IDxContext::I_FOG_EXP) != 0;
	// ������Ч
	bool fog_linear = pContext->GetInt(IDxContext::I_FOG_LINEAR) != 0;

	// ���ģ���������
	DxSceneBatch* pBatchStencil= g_pRender->AddNoLiveStencilBatch(
		"ModelAppliqueColor");

	pBatchStencil->Copy(pBatch);

	// pBatch��pBatchStencil����λ��
	DxSceneBatch* pTempBatch = pBatch;
	
	pBatch = pBatchStencil;
	pBatchStencil = pTempBatch;

	SelectDepthShader(pBatchStencil, pNode, pMat);

	// ������ͼ��д��ȵ�����ģʽ(ͶӰ)
	if (pMatInfo->bNoZWrite && (pMatInfo->DiffuseMap.pFileName != NULL))
	{
		pBatchStencil->SetRasterState(g_pCullNoneRasterState);
		pBatchStencil->SetDepthState(g_pAppliqueMaskDepthState);
		pBatchStencil->SetBlendState(g_pColorDisableBlendState);
		pBatch->SetRasterState(g_pCullFrontRasterState);
		pBatch->SetDepthState(g_pAppliqueDrawDepthState);

		if (pMatInfo->bBlend) //Alpha���
		{
			pBatch->SetBlendState(g_pNormalBlendState);
		}
		else if (pMatInfo->bBlendEnhance) //��������
		{
			pBatch->SetBlendState(g_pEnhanceBlendState);
		}
		else //��Ƭ����
		{
			pBatch->SetBlendState(g_pInverseBlendState);
		}

		bool shadow = false;
		bool prelight = false;
		bool use_intz = false;
		bool multiply = !(m_pCurMatInfo->bBlend 
			|| m_pCurMatInfo->bBlendEnhance);

		if (pContext->GetEnableRealizeDepth())
		{
			pBatch->SetPSTexture(1, pContext->GetDepthRT());

			if (!multiply)
			{
				if (pContext->GetEnableDynamicShadow())
				{
					pBatch->SetPSTexture(2, pContext->GetDynamicShadowRT());
					shadow = true;
				}

				if (pContext->GetEnableLightPrepass())
				{
					pBatch->SetPSTexture(3, pContext->GetLightRT());
					prelight = true;
				}
			}
		}

		pBatch->SetPSTexture(4, pContext->GetNormalRT());

		bool use_stereo = g_pRender->GetStereoIsEnabled();

		SelectAppliqueShader(pBatch, pNode, pMat, shadow, prelight, 
			fog_linear, fog_exp, use_intz, multiply, use_stereo);
		
		if (use_intz)
		{
			// ������Ҫ�������ģ�建��
			DxSceneBatch* pClearBatch = g_pRender->AddNoLiveStencilBatch(
				"ModelAppliqueClear");

			pClearBatch->Copy(pBatchStencil);
			pClearBatch->SetRasterState(g_pCullFrontRasterState);
			pClearBatch->SetDepthState(g_pAppliqueClearDepthState);
			pClearBatch->SetBlendState(g_pColorDisableBlendState);
		}

		return;
	}

	pBatchStencil->SetRasterState(g_pCullNoneRasterState);
	pBatchStencil->SetDepthState(g_pAppliqueMaskDepthState);
	pBatchStencil->SetBlendState(g_pColorDisableBlendState);
	pBatch->SetRasterState(g_pCullFrontRasterState);
	pBatch->SetDepthState(g_pAppliqueDrawDepthState);

	// ʵʱ��Ӱ
	bool dynamic_shadow = pContext->GetEnableDynamicShadow()
		&& pContext->GetEnableRealizeDepth();

	if (dynamic_shadow)
	{
		// ʵʱ��Ӱͼ
		pBatch->SetPSTexture(3, pContext->GetDynamicShadowRT());
	}

	if (pMatInfo->bBumpMapEnable)
	{
		// ������ͼ
		ITexture* pTexBumpMap = pMatInfo->BumpMap.pTex;

		if (pTexBumpMap)
		{
			pBatch->SetPSTexture(1, pTexBumpMap->GetShaderTex());
		}
	}

	// ���������Ƿ���
	bool reflect_enable = false;

	if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
	{
		// ����������ͼ
		ITexture* pTexReflectionMap = pMatInfo->ReflectionMap.pTex;

		if (pTexReflectionMap)
		{
			pBatch->SetPSTexture(4, pTexReflectionMap->GetShaderTex());
			reflect_enable = true;
		}
		else
		{
			// ����ʹ��ȫ�ֻ�����ͼ
			ITexture* pGlobalEnvTex = 
				pContext->GetGlobalEnvironmentCubeTex();

			if (pGlobalEnvTex)
			{
				// Ҳ��Ҫ��٤��У��
				pBatch->SetPSTexture(4, pGlobalEnvTex->GetShaderTex());
				reflect_enable = true;
			}
		}
	}

	if (pMatInfo->bSpecularEnable)
	{
		// �߹���ͼ
		ITexture* pTexSpecularMap = pMatInfo->SpecularMap.pTex;

		if (pTexSpecularMap)
		{
			pBatch->SetPSTexture(5, pTexSpecularMap->GetShaderTex());
		}
		else
		{
			// �߹�������ͼ 
			ITexture* pTexSpecularLevelMap = pMatInfo->SpecularLevelMap.pTex;

			if (pTexSpecularLevelMap)
			{
				pBatch->SetPSTexture(5, pTexSpecularLevelMap->GetShaderTex());
			}
		}
	}

	if (pMatInfo->bEmissiveEnable)
	{
		// �Է�����ͼ��α�����ͼ
		ITexture* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;

		if (pTexEmissiveMap)
		{
			pBatch->SetPSTexture(6, pTexEmissiveMap->GetShaderTex());
		}
	}

	// �ӳٹ���
	bool prelight = pContext->GetEnableLightPrepass();

	if (prelight)
	{
		pBatch->SetPSTexture(7, pContext->GetLightRT());
	}

	// �������������ʱ����Ӱ��Ե����
	bool fix_shadow = dynamic_shadow && pContext->GetEnableFixShadow()
		&& (g_pRender->GetMultiSampleType() > 1);

	if (fix_shadow)
	{
		pBatch->SetPSTexture(8, pContext->GetDepthRT());
	}

	// ���򻷾���
	bool sphere_ambient = pMatInfo->bSphereAmbient;

	if (!sphere_ambient)
	{
		if (pNode->nType == FXNODE_TYPE_SKIN)
		{
			if (pContext->GetInt(IDxContext::I_SKIN_SPHERE_AMBIENT) != 0)
			{
				// ������������ģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
		else if (pNode->nType == FXNODE_TYPE_MESH)
		{
			// �ر���Ҷ�İ�����AO
			if (((pNode->nModelInfo & MODEL_TREELEAF_FLAG) == 0)
				&& (pContext->GetInt(IDxContext::I_MODEL_SPHERE_AMBIENT) != 0))
			{
				// ������ͨģ�͵İ��򻷾���
				sphere_ambient = true;
			}
		}
	}

	// ģ�͸�����ɫ
	unsigned int model_alpha = (m_nColor >> 24) & 0xFF;

	if (pMatInfo->bBlend)
	{
		pBatch->SetBlendState(g_pNormalBlendState);
	}
	else if (pMatInfo->bBlendEnhance)
	{
		pBatch->SetBlendState(g_pEnhanceBlendState);
	}
	else if (pMatInfo->bOpacityEnable)
	{
		pBatch->SetBlendState(g_pNormalBlendState);
	}
	else if (model_alpha != 255)
	{
		// ���ջ��ϵ������͸�����
		FmVec4 blend_factor(model_alpha / 255.0F, model_alpha / 255.0F, 
			model_alpha / 255.0F, 1.0F);

		pBatch->SetBlendState(g_pFactorBlendState);
		pBatch->SetBlendFactor(blend_factor);
	}
	
	bool gamma = pContext->GetEnableGammaCorrect();

	SelectShader(pBatch, pNode, pMat, true, dynamic_shadow, fog_linear, 
		fog_exp, prelight, reflect_enable, fix_shadow, sphere_ambient, 
		false, model_alpha, false, false, false, gamma, false);

	*/
}

void CModelPlayer::DrawMaterialVolumeFog(node_material_t* pMat, 
	model_node_t* pNode, const FmVec4* bone_matrices)
{
	IRenderContext* pContext = g_pRender->GetContext();

	/*
	if (!pContext->GetEnableRealizeDepth() 
		|| !pContext->GetEnableRealizeVolumeFog())
	{
		return;
	}

	material_info_t* pMatInfo = m_pCurMatInfo;

	/*
	// ����
	DxSceneBatch* pBatchFront;
	// ����
	DxSceneBatch* pBatchBack;
	DxSceneBatch* pBatch;

	if (m_pCurMatInfo->bNoZWrite)
	{
		pBatchFront = g_pRender->AddBlendBatch("VolumeFogFront");
		pBatchBack = g_pRender->AddBlendBatch("VolumeFogBack");
		pBatch = g_pRender->AddBlendBatch("ModelVolumeFog");
	}
	else
	{
		pBatchFront = g_pRender->AddUnderWaterBatch("VolumeFogFront");
		pBatchBack = g_pRender->AddUnderWaterBatch("VolumeFogBack");
		pBatch = g_pRender->AddUnderWaterBatch("ModelVolumeFog");
	}

	// ���ü�������Ϣ 
	SetBatchGeometry(pBatch, pMat, pNode);

	IDxDynamicCB* pCB = GetObjectCB(pMat, bone_matrices);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	// ����ʹ�ð�ֱ��ʵ����ͼ
	IDxColorRT* depth_tex = pContext->GetHalfDepthRT();

	if (NULL == depth_tex)
	{
		depth_tex = pContext->GetDepthRT();
	}

	pBatchFront->Copy(pBatch);
	pBatchFront->SetBlendState(g_pBrightBlendState);
	pBatchFront->SetPSTexture(0, depth_tex);
	pBatchFront->SetRenderTarget(pContext->GetVolumeFogRT());
	pBatchFront->SetType(DxSceneBatch::TYPE_POST_PROCESS);
	pBatchFront->SetFlags(DxSceneBatch::FLAG_CLEAR_COLOR);

	pBatchBack->Copy(pBatch);
	pBatchBack->SetRasterState(g_pCullFrontRasterState);
	pBatchBack->SetBlendState(g_pBrightBlendState);
	pBatchBack->SetPSTexture(0, depth_tex);
	pBatchBack->SetRenderTarget(pContext->GetVolumeFogRT());
	pBatchBack->SetType(DxSceneBatch::TYPE_POST_PROCESS);

	pBatch->SetRasterState(g_pCullFrontRasterState);
	pBatch->SetDepthState(g_pVolumeFogDepthState);
	pBatch->SetBlendState(g_pNormalBlendState);
	pBatch->SetPSTexture(0, pContext->GetVolumeFogRT());
	pBatch->SetFlags(DxSceneBatch::FLAG_CLEAR_STENCIL);

	bool fog_linear = pContext->GetInt(IDxContext::I_FOG_LINEAR) != 0;
	bool fog_exp = pContext->GetInt(IDxContext::I_FOG_EXP) != 0;

	SelectVolumeFogDepthShader(pBatchFront, pNode, pMat, false);
	SelectVolumeFogDepthShader(pBatchBack, pNode, pMat, true);
	SelectVolumeFogColorShader(pBatch, pNode, pMat, fog_linear, fog_exp);

	*/
}

void CModelPlayer::DrawMaterialRefraction(node_material_t* pMat, 
	model_node_t* pNode, const FmVec4* bone_matrices)
{
	IRenderContext* pContext = g_pRender->GetContext();
	
	/*
	if (!pContext->GetEnableRealizeRefraction())
	{
		return;
	}
	
	material_info_t* pMatInfo = m_pCurMatInfo;

	/*
	// ��ĻŤ�������䣩
	DxSceneBatch* pBatch = g_pRender->AddRefractionBatch("ModelRefraction");

	// ���ü�������Ϣ 
	SetBatchGeometry(pBatch, pMat, pNode);

	IDxDynamicCB* pCB = GetObjectCB(pMat, bone_matrices);

	pBatch->SetVSConstBuffer(0, pCB);
	pBatch->SetPSConstBuffer(0, pCB);

	// ��������ͼ
	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			pBatch->SetPSTexture(0, pTexDiffuse->GetShaderTex());
		}
		else
		{
			pBatch->SetPSTexture(0, NULL);
		}
	}

	pBatch->SetBlendState(g_pNormalBlendState);

	// �Ƿ�˫��
	if (m_pCurMatInfo->bDoubleSide)
	{
		pBatch->SetRasterState(g_pCullNoneRasterState);
	}

	SelectRefractionShader(pBatch, pNode, pMat);
	*/
}


void CModelPlayer::DrawMaterial(node_material_t* pMat, model_node_t* pNode, MatInfo* matinfo)
{
	MatInfo& _matinfo = *m_MatInfoPools.Alloc(); //
	//matinfo[pMat->nIndexInModel];
	pMat->vf.Parse(pMat->nMaterialInfo, pMat->nMaterialInfoEx, pNode->nModelInfo, pNode->nType);
	_matinfo.pMat = pMat;
	_matinfo.pNode = pNode;
	_matinfo.pThis = this;
	IRenderContext* pContext = g_pRender->GetContext();
	_matinfo.camera = pContext->GetCamera();
	_matinfo.refer_pos = pContext->GetReferPosition();
	_matinfo.pMatInfo = m_pCurMatInfo;
	_matinfo.nColor = m_nColor;

	
	Assert(pMat != NULL);
	Assert(pNode != NULL);

	

	if (!CreateVBIB(pMat, pNode))
	{
		//error
		return;
	}

	FmVec4* bone_matrices;

	if (pNode->nType == FXNODE_TYPE_SKIN)
	{
		bone_matrices = CreateBoneMatrices(pMat);


		if (NULL == bone_matrices)
		{
			// ����δ��ʼ��
			return;
		}
		
	}
	else
	{
		bone_matrices = NULL;
	}

	_matinfo.bone_matrices = bone_matrices;
	_matinfo.mtxCurrentTM = m_mtxCurrentTM;

	

	#ifdef CPU_COMPUTE_BONE
		if (pNode->nType == FXNODE_TYPE_SKIN)
		{
			CpuComputeBone(pMat, bone_matrices);
		}
	#endif

	material_info_t* pMatInfo = m_pCurMatInfo;

	if (pMatInfo->bApplique)
	{
		if (pMatInfo->bDisappear)
		{
			// ��̬����
			//DrawMaterialBlend(pMat, pNode, bone_matrices);
			AddDrawMaterialBlendBatch(pMat, pNode, bone_matrices, _matinfo);
		}
		else
		{
			// ģ������
			DrawMaterialApplique(pMat, pNode, bone_matrices);
		}
	}
	else if (pNode->nModelInfo & MODEL_WATERCLIP_FLAG)
	{
		// ˮ���޳�
		DrawMaterialWaterClip(pMat, pNode, bone_matrices);
	}
	else if (pMatInfo->bRefraction)
	{
		// ��ĻŤ�������䣩
		DrawMaterialRefraction(pMat, pNode, bone_matrices);
	}
	else if (pMatInfo->bSceneFog)
	{
		if (pMatInfo->bNoZWrite)
		{
			// ˮ��
			DrawMaterialWater(pMat, pNode, bone_matrices);
		}
		else
		{
			// �����
			DrawMaterialVolumeFog(pMat, pNode, bone_matrices);
		}
	}
	else
	{
		// �Ƿ�͸��
		bool is_blend = ((_matinfo.nColor & 0xFF000000) != 0xFF000000) 
			|| pMatInfo->bBlend || pMatInfo->bBlendEnhance 
			|| pMatInfo->bOpacityEnable;

		//is_blend = false;
		if (is_blend)
		{
			// ����ϵ�����
			AddDrawMaterialBlendBatch(pMat, pNode, bone_matrices, _matinfo);
		}
		else
		{
			IRenderContext* pContext = g_pRender->GetContext();


			if (pMatInfo->bBlendQuality)
			{
				// ������Ⱦ

				IRenderContext* pContext = g_pRender->GetContext();
				const FmMat4& mtxView = pContext->GetCamera().mtxView;
				float camera_z = mtxView._13 * m_mtxCurrentTM._41 
					+ mtxView._23 * m_mtxCurrentTM._42 
					+ mtxView._33 * m_mtxCurrentTM._43 + mtxView._43;

				AddDrawMaterialAlphaTestBatch(pMat, pNode, bone_matrices, _matinfo);
			
				MatInfo* __matinfo = m_MatInfoPools.Alloc();
				memcpy(__matinfo, &_matinfo, sizeof(MatInfo));
				g_pRender->GetSceneView()->AddPriorBlendBatch(DrawMaterialBlendBatch, __matinfo, camera_z, "CModelPlayer::AddDrawMaterialBledndBatch");
			}
			else
			{		
				if(m_bKeepOut)
				{
					AddDrawMaterialKeepOut(pMat, pNode, bone_matrices, _matinfo);
				}
				else
				{
					// ʵ������
					if(pMat->MatInfo.bAlphaTest)
						AddDrawMaterialAlphaTestBatch(pMat, pNode, bone_matrices, _matinfo);
					else
						AddDrawMaterialSolidBatch(pMat, pNode, bone_matrices, _matinfo);
				}
			}
		}
	}
}

void CModelPlayer::DrawNode(model_node_t* pNode, bool bCull, MatInfo* matinfo)
{
	Assert(pNode != NULL);

	FmMat4 mtxOldWorld = m_mtxCurrentTM;
	int id;

	if (!pNode->bNeedDraw)
	{
		goto DO_CHILD;
	}

	id = pNode->nIndex;

	Assert(id >= 0);

	m_mtxCurrentTM = m_mtxNodeCurrentTM[id];

	// ����Χ��
	if (g_bShowNodeBoundingBox && bCull)
	{
		FmVec3 bbMin = pNode->vBoundMin;
		FmVec3 bbMax = pNode->vBoundMax;

		if (m_pActionPlayer)
		{
			skt_pose_t* pose = m_pActionPlayer->GetSkeletonPose();
			FmVec3 pos;
			FmMat4 mtxRoot;
			
			Assert(pose != NULL);
			m_pActionPlayer->GetRootBoneOriginPosition(&pos);
			get_current_root_node_tm(pose, &mtxRoot);
			bbMin.x += mtxRoot._41 - pos.x;
			bbMin.y += mtxRoot._42 - pos.y;
			bbMin.z += mtxRoot._43 - pos.z;
			bbMax.x += mtxRoot._41 - pos.x;
			bbMax.y += mtxRoot._42 - pos.y;
			bbMax.z += mtxRoot._43 - pos.z;
			math_rotate_bounding_box(&bbMin, &bbMax, &bbMin, &bbMax, 
				&m_mtxCurrentTM);
		}

		bbMin.x += m_mtxCurrentTM._41;
		bbMin.y += m_mtxCurrentTM._42;
		bbMin.z += m_mtxCurrentTM._43;
		bbMax.x += m_mtxCurrentTM._41;
		bbMax.y += m_mtxCurrentTM._42;
		bbMax.z += m_mtxCurrentTM._43;

		aux_draw_bound_box(&bbMin, &bbMax, 0xFFFF00FF, 0xFFFF00FF, 0.2F); 
		aux_flush();
	}

	if (m_bShowHelper)
	{
		if (pNode->nType == FXNODE_TYPE_HELPER)
		{
			FmVec3 point_position = FmVec3(m_mtxCurrentTM._41, 
				m_mtxCurrentTM._42, m_mtxCurrentTM._43);

			if (id != m_nSelectHelper)
			{
				aux_draw_point(&point_position, 0xFF00FF00, 0.3F, false);
			}
			else
			{
				aux_draw_point(&point_position, 0xFFFF0000, 0.3F, false);
			}
		}
	}

	
	if (((pNode->nModelInfo & MODEL_MAINMODEL_FLAG) == 0) 
		|| g_pRender->GetShowHideModel())
	{
		for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
		{
			node_material_t* pMat = &pNode->Materials[m];

			m_pCurMatInfo = &pMat->MatInfo;
			
			// �Ƿ�������ָ���Ĳ��ʲ���
			if (m_nCustomMaterialNum > 0)
			{
				// ���в��Һ͵�ǰ������ƥ��Ĳ���
				for (unsigned int k = 0; k < m_nCustomMaterialNum; ++k)
				{
					// ֱ�ӱȽ�ָ��
					if (m_pCustomMaterials[k].pMaterialName 
						== pMat->pszName)
					{
						// ʹ������ָ���Ĳ��ʲ���������Ⱦ
						m_pCurMatInfo = &m_pCustomMaterials[k].MatInfo;
					}
				}
			}

			DrawMaterial(pMat, pNode, matinfo);
		}
	}
	

DO_CHILD:
	for (unsigned int child = 0; child < pNode->nChildNodeCount; child++)
	{
		DrawNode(&pNode->ChildNodes[child], bCull, matinfo);
	}

	m_mtxCurrentTM = mtxOldWorld;
}

void CModelPlayer::DrawNodeNormal(model_t* pModel, model_node_t* pNode)
{
	Assert(pModel != NULL);
	Assert(pNode != NULL);

	// ���浱ǰ�������ڻָ�����Ⱦ���ֵܽ�㣬�Լ��丸��������ϵ�ĸ������
	FmMat4 mtxOld = m_mtxCurrentTM;

	if (pNode->nType == FXNODE_TYPE_HELPER)
	{
		if (0 == pNode->nChildNodeCount)
		{
			return;
		}

		DrawNodeNormal_DoChild(pModel, pNode, mtxOld);
		return;
		//goto DO_CHILD;
	}

	if (pNode->bHide || pNode->bWaterClip)
	{
		DrawNodeNormal_DoChild(pModel, pNode, mtxOld);
		return;
		//goto DO_CHILD;
	}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		DrawNodeNormal_DoChild(pModel, pNode, mtxOld);
		return;
		//goto DO_CHILD;
	}

	if (!pNode->bNeedDraw)
	{
		DrawNodeNormal_DoChild(pModel, pNode, mtxOld);
		return;
		//goto DO_CHILD;
	}

	int id = pNode->nIndex;

	Assert(id >= 0);

	m_mtxCurrentTM = m_mtxNodeCurrentTM[id];

	// ������ǰ������������ӵ�еĶ�������
	for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
	{
		node_material_t* pMat = &pNode->Materials[i];

		Assert(pMat != NULL);

		if (pNode->nModelInfo & MODEL_POSITION_INFO)
		{
			FmVec3 p0, p1, p2;
			FmVec3 v0, v1;

			unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];

			for (unsigned int face = 0; face < index_num; face += 3)
			{
				get_single_vertex(pMat, face, &p0, &p1, &p2);

				// ת����������
				FmMat4 mat = m_mtxCurrentTM;

				FmVec3TransformCoord(&p0, &p0, &mat);
				FmVec3TransformCoord(&p1, &p1, &mat);
				FmVec3TransformCoord(&p2, &p2, &mat);

				FmVec3 normal;

				FmVec3 len1 = p1 - p0;
				FmVec3 len2 = p2 - p0;
				FmVec3Cross(&normal, &len1, &len2);
				FmVec3Normalize(&normal, &normal);

				// ȡ���ĵ��޷�ȷ���������ʲôλ���ϣ���λ����������ʶ��
				//v0 = (p0 + p1 + p2) / 3.0F;
				v0 = p0;
//				v1 = v0 + normal * g_pRender->GetShowNormalLength();

				if (normal.y < 0.0F)
				{
					aux_draw_line3(&v0, &v1, 0xFFFF0000, 0xFFFF0000);
				}
				else
				{
					aux_draw_line3(&v0, &v1, 0xFF00FF00, 0xFF00FF00);
				}
			}
		}
		else if (pNode->nModelInfo & MODEL_POSITION_ANI)
		{
			float* pVB = pMat->pMultiVBs[m_nCurrentFrame]; 
			FmVec3 p0, p1, p2;
			FmVec3 v0, v1;

			unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];

			for (unsigned int face = 0; face < index_num; face += 3)
			{
				get_triangle_vertex(pMat, pVB, pMat->nMultiVertexSize, 
					face, &p0, &p1, &p2);

				// ת����������
				FmMat4 mat = m_mtxCurrentTM;

				FmVec3TransformCoord(&p0, &p0, &mat);
				FmVec3TransformCoord(&p1, &p1, &mat);
				FmVec3TransformCoord(&p2, &p2, &mat);

				FmVec3 normal;
				FmVec3 len1 = p1 - p0;
				FmVec3 len2 = p2 - p0;

				FmVec3Cross(&normal, &len1, &len2);
				FmVec3Normalize(&normal, &normal);

				// ȡ���ĵ��޷�ȷ���������ʲôλ���ϣ���λ����������ʶ��
				//v0 = (p0 + p1 + p2) / 3.0F;
				v0 = p0;
//				v1 = v0 + normal * g_pRender->GetShowNormalLength();

				if (normal.y < 0.0F)
				{
					aux_draw_line3(&v0, &v1, 0xFFFF0000, 0xFFFF0000);
				}
				else
				{
					aux_draw_line3(&v0, &v1, 0xFF00FF00, 0xFF00FF00);
				}
			}
		}
	}

	DrawNodeNormal_DoChild(pModel, pNode, mtxOld);
	/*
DO_CHILD:
	// Child node
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		model_node_t* child = &pNode->ChildNodes[i];

		Assert(child != NULL);

		if (child->nType == FXNODE_TYPE_MESH)
		{
			DrawNodeNormal(pModel, child);
		}
	}

	m_mtxCurrentTM = mtxOld;*/
}

void CModelPlayer::DrawModel(bool bCull, MatInfo* MatList)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::DrawModel] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	if (!m_bNeedDraw)
	{
		return;
	}

	m_mtxCurrentTM = m_mtxWorldTM;

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		DrawNode(&pModel->RootNodes[i], bCull, MatList);
	}

	// ��ʾ��Χ��
	if (m_bSelected)
	{
//		FmVec3 bbMin, bbMax;
//
//		FXVAMODEL_GetCurrentBoundingBox(vaModel, &bbMin, &bbMax);
//		aux_draw_bound_box(&bbMin, &bbMax, 0xFF00FFFF, 0xFF00FFFF, 0.0F);

		/*
		if (g_pRender->GetShowNormalLength() > 0.0F)
		{
			for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
			{
				DrawNodeNormal(pModel, &pModel->RootNodes[i]);
			}

			aux_flush();
		}
		*/
	}
}

void CModelPlayer::DrawMaterialShadowMap(const MatInfo* info)
{
	const MatInfo* pRenderInfo = info;
	CModelPlayer* pthis = pRenderInfo->pThis;
	IRender* pRender = g_pRender;
	IRenderContext* pContext = g_pRender->GetContext();
	node_material_t* pMat = pRenderInfo->pMat;
	material_info_t* pMatInfo = pRenderInfo->pMatInfo;
	model_node_t* pNode = pMat->pNode;

	IShaderProgram* pShader = SelectShadowMapShader(info, pMat, pNode); 

	if (pShader == NULL)
	{
		return;
	}

	pShader->UsedShader();

	IRenderDrawOp* pRenderDrawOp = g_pRender->GetRenderDrawOp();

	pRenderDrawOp->SetVB( pMat->pSingleGPUVB->GetBuffer());
	pRenderDrawOp->SetIB( pMat->pGPUIB->GetBuffer());

	if (!CreateVDecl(pMat, pNode, false))
	{
		/*
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		*/
		pRenderDrawOp->SetVB(0);
		pRenderDrawOp->SetIB(0);
		return;
	}

	// �����ο���ͼ
	if (pMatInfo->bAlphaTest)
	{
		ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;

		if (pTexDiffuse)
		{
			SetTexture(tex_Diffuse, pTexDiffuse->GetCanUseShaderTex()->GetTexture());
		}
		else
		{
			SetTexture(tex_Diffuse, NULL);
		}

		float alpha_ref = get_current_alpha_ref(pMatInfo->fAlphaRef) / 255.0F;
		ShaderManager::Inst().SetShaderValue1f(c_fAlphaRefValue, alpha_ref * 0.5f);
	}

	const camera_t& camera = pRenderInfo->camera;
	const FmVec3& refer_pos = pRenderInfo->refer_pos;
	FmMat4 mtxWorld = pRenderInfo->mtxCurrentTM;
	mtxWorld._41 -= (float)refer_pos.x;
	mtxWorld._42 -= (float)refer_pos.y;
	mtxWorld._43 -= (float)refer_pos.z;

	FmMat4 mtxWVP;
	FmMat4Multiply(&mtxWVP, &mtxWorld, &camera.mtxViewProjRelative);
	ShaderManager::Inst().SetShaderValueMat4(c_mtxWVP, mtxWVP);

	if(pRenderInfo->bone_matrices)
		ShaderManager::Inst().SetShaderValue4fv(c_BoneList, pMat->nMappedBoneCount * 3, (float*)pRenderInfo->bone_matrices);

	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();

	int ib_count = pMat->pLODIndexCount[pMat->nCurLODLevel];
	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES, ib_count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT, &((fm_short*)0)[0]);

	pRenderDrawOp->SetIB( 0 );
	pRenderDrawOp->SetVB( 0 );
}

void CModelPlayer::DrawMaterialShadowMapBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->DrawMaterialShadowMap(param);
}

void CModelPlayer::AddDrawMaterialShadowMapBatch(node_material_t* pMat, material_info_t* pMatInfo, model_node_t* pNode)
{
	Assert(pMat != NULL);
	Assert(pMatInfo != NULL);
	Assert(pNode != NULL);

	// ��Ҫʱ�������㶨��
	if (!CreateVBIB(pMat, pNode))
	{
		//error
		return;
	}

	FmVec4* bone_matrices = NULL;

	if (pNode->nType == FXNODE_TYPE_SKIN)
	{
		bone_matrices = CreateBoneMatrices(pMat);

		if (NULL == bone_matrices)
		{
			// ����δ��ʼ��
			return;
		}
	}

	pMat->vf.Parse(pMat->nMaterialInfo, pMat->nMaterialInfoEx, pNode->nModelInfo, pNode->nType);

	MatInfo& _matinfo = *m_MatInfoPools.Alloc();
	_matinfo.pMat = pMat;
	_matinfo.pNode = pNode;
	_matinfo.pThis = this;

	IRenderContext* pContext = g_pRender->GetContext();
	_matinfo.camera = pContext->GetCamera();
	_matinfo.refer_pos = pContext->GetReferPosition();
	_matinfo.pMatInfo = m_pCurMatInfo;

	_matinfo.bone_matrices = bone_matrices;
	_matinfo.mtxCurrentTM = m_mtxCurrentTM;

	g_pRender->GetSceneView()->AddSolidBatch(DrawMaterialShadowMapBatch, &_matinfo);
}

void CModelPlayer::DrawNodeShadowMap(model_node_t* pNode, bool bCull)
{
	Assert(pNode != NULL);

	FmMat4 mtxOldWorld = m_mtxCurrentTM;

	unsigned int m,k;
	material_info_t* pMatInfo;
	node_material_t* pMat;
	if (pNode->bNeedDraw)
	{
		int id = pNode->nIndex;

		Assert(id >= 0);

		m_mtxCurrentTM = m_mtxNodeCurrentTM[id];

		if (((!pNode->bHide) && (!pNode->bWaterClip)) &&
			(pNode->nModelInfo & MODEL_MAINMODEL_FLAG) == 0)
		{
			for (m = 0; m < pNode->nMaterialCount; m++)
			{
				pMat = &pNode->Materials[m];

				m_pCurMatInfo = &pMat->MatInfo;

				// �Ƿ�������ָ���Ĳ��ʲ���
				if (m_nCustomMaterialNum > 0)
				{
					// ���в��Һ͵�ǰ������ƥ��Ĳ���
					for (k = 0; k < m_nCustomMaterialNum; ++k)
					{
						// ֱ�ӱȽ�ָ��
						if (m_pCustomMaterials[k].pMaterialName == pMat->pszName)
						{
							// ʹ������ָ���Ĳ��ʲ���������Ⱦ
							m_pCurMatInfo = &m_pCustomMaterials[k].MatInfo;
						}
					}
				}

				pMatInfo = m_pCurMatInfo;

				// �����������岻����Ӱͼ
				if (pMatInfo->bBlend || pMatInfo->bBlendEnhance 
					|| pMatInfo->bOpacityEnable || pMatInfo->bNoZWrite 
					|| pMatInfo->bRefraction || pMatInfo->bApplique 
					|| pMatInfo->bSceneFog)
				{
					continue;
				}

				AddDrawMaterialShadowMapBatch(pMat, pMatInfo, pNode);
			}
		}
	}

	for (unsigned int child = 0; child < pNode->nChildNodeCount; child++)
	{
		DrawNodeShadowMap(&pNode->ChildNodes[child], bCull);
	}

	m_mtxCurrentTM = mtxOldWorld;
}

void CModelPlayer::DrawModelShadowMap(bool bCull)
{
	if (!m_bNeedDraw)
	{
		return;
	}

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::DrawModelShadowMap] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	m_mtxCurrentTM = m_mtxWorldTM;

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		DrawNodeShadowMap(&pModel->RootNodes[i], bCull);
	}
}

void CModelPlayer::DrawMaterialSolidColorPassBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialSolid(param, false, true);
}

void CModelPlayer::DrawMaterialSolidOnlyZPassBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialSolid(param, true, false);
}

void CModelPlayer::DrawMaterialSolidBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialSolid(param);
}

void CModelPlayer::DrawMaterialBlendBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialBlend(param);
}

void CModelPlayer::DrawMaterialAlphaTestBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialAlphaTest(param);
}

void CModelPlayer::DrawMaterialEarlyZBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialEarlyZ(param);
}

void CModelPlayer::DrawMaterialKeepOutBatch(void* pdata)
{
	MatInfo* param = (MatInfo*)pdata;
	param->pThis->m_MatInfoPools.RemoveOne();
	param->pThis->DrawMaterialKeepOut(param);
}

void CModelPlayer::AddDrawMaterialSolidBatch(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info)
{
	info.mtxCurrentTM = this->m_mtxCurrentTM;

	//��������Early Z�Ż�
	FmVec3 vecTemp = FmVec3(info.mtxCurrentTM._41,info.mtxCurrentTM._42,info.mtxCurrentTM._43) - info.camera.vPosition;
	float d_len = vecTemp.length();
	FmVec3 vecR = pNode->vBoundMax - pNode->vBoundMin;
	float r_len = vecR.length() / 2.0f;//�뾶

//	m_MatInfoMan.AddRef(&info);
 	if(r_len > 3.0 && this->m_pContext->GetEnableEarlyZ() && d_len < 15)
	{
		g_pRender->GetSceneView()->AddEarlyZBatch(DrawMaterialSolidOnlyZPassBatch, &info,fabsf(d_len-r_len), "CModelPlayer::AddDrawMaterialSolidBatch EarlyZ",DrawMaterialSolidColorPassBatch);
	}
 	else
	{
 		g_pRender->GetSceneView()->AddSolidBatch(DrawMaterialSolidBatch, &info, "CModelPlayer::AddDrawMaterialSolidBatch");
	}
}

void CModelPlayer::AddDrawMaterialAlphaTestBatch(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info)
{
	info.mtxCurrentTM = m_mtxCurrentTM;

	//��������Early Z�Ż�
	FmVec3 vecTemp = FmVec3(info.mtxCurrentTM._41,info.mtxCurrentTM._42,info.mtxCurrentTM._43) - info.camera.vPosition;
	float d_len = vecTemp.length();
	FmVec3 vecR = pNode->vBoundMax - pNode->vBoundMin;
	float r_len = vecR.length() / 2.0f;//�뾶

//	m_MatInfoMan.AddRef(&info);
 	if(r_len > 3.0 && m_pContext->GetEnableEarlyZ() && d_len < 30.0f)
	{
		g_pRender->GetSceneView()->AddEarlyZBatch(DrawMaterialSolidOnlyZPassBatch, &info,fabsf(d_len-r_len), "CModelPlayer::AddDrawMaterialAlphaTestBatch EarlyZ", DrawMaterialSolidColorPassBatch);
	}
 	else
	{
 		g_pRender->GetSceneView()->AddSolidAlphaTestBatch(DrawMaterialAlphaTestBatch, &info, "CModelPlayer::AddDrawMaterialAlphaTestBatch");
	}
	
}

void CModelPlayer::AddDrawMaterialBlendBatch(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info)
{
	IRenderContext* pContext = g_pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	float camera_z = mtxView._13 * m_mtxCurrentTM._41 
		+ mtxView._23 * m_mtxCurrentTM._42 
		+ mtxView._33 * m_mtxCurrentTM._43 + mtxView._43;

	info.mtxCurrentTM = m_mtxCurrentTM;

//	m_MatInfoMan.AddRef(&info);
	g_pRender->GetSceneView()->AddBlendBatch(DrawMaterialBlendBatch, &info, camera_z, "CModelPlayer::AddDrawMaterialBlendBatch");
}

void CModelPlayer::AddDrawMaterialKeepOut(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info)
{
	IRenderContext* pContext = g_pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	float camera_z = mtxView._13 * m_mtxCurrentTM._41 
		+ mtxView._23 * m_mtxCurrentTM._42 
		+ mtxView._33 * m_mtxCurrentTM._43 + mtxView._43;

	info.mtxCurrentTM = m_mtxCurrentTM;
//	m_MatInfoMan.AddRef(&info);
	g_pRender->GetSceneView()->AddGroundBatch(DrawMaterialKeepOutBatch, &info, "CModelPlayer::AddDrawMaterialKeepOut");

//	m_MatInfoMan.AddRef(&info);
	g_pRender->GetSceneView()->AddAfterSolidBatch(DrawMaterialSolidBatch, &info, "CModelPlayer::AddDrawMaterialKeepOut");
}