//--------------------------------------------------------------------
// 文件名:		model_player_routine.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "action_player.h"
#include "res_model.h"
#include "math_3d.h"
#include "../public/portable.h"
#include "../visual/i_render.h"
//#include "../visual/i_dx_context.h"
#include "../visual/i_texture.h"
//#include "../visual/dx_scene_batch.h"
#include "../visual/vis_utils.h"
//#include "../visual/sse.h"
#include "../public/inlines.h"
//#include "../utils/system_utils.h"

extern IRender* g_pRender;

// 获得合适的ALPHA测试参考值
int get_current_alpha_ref(float alpha_ref)
{
	if ((alpha_ref > 0.001F) && (alpha_ref <= 1.0F))
	{
		return int(alpha_ref * 255.0F);
	}

	return g_pRender->GetRenderAlphaRef();
}

// 创建骨骼矩阵
FmVec4* CModelPlayer::CreateBoneMatrices(node_material_t* pMat)
{
	if (NULL == pMat->pBoneList)
	{
		return NULL;
	}

	if (NULL == m_pActionPlayer)
	{
		return NULL;
	}

	if (NULL == m_pCacheBones)
	{
		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::CreateBoneMatrices] pModel == NULL model name is %s", m_pResModel->GetName());
			return NULL;
		}

		unsigned int mat_num = pModel->nMaterialCount;
		
		m_pCacheBones = (cache_bone_t*)CORE_ALLOC(
			sizeof(cache_bone_t) * mat_num);
		memset(m_pCacheBones, 0, sizeof(cache_bone_t) * mat_num);
	}

	int sequence_id = m_pActionPlayer->GetSequenceId();
	bool optimize_by_sequence_id = m_pActionPlayer->GetOptimizeBySequenceId();
	skt_pose_t* pose = m_pActionPlayer->GetCurrentPose();
	skeleton_t* pInst = pose->pSkeleton;
	skt_node_t** bone_node_list = pInst->pBoneNodeList;

	if (NULL == bone_node_list)
	{
		return NULL;
	}

	unsigned int mat_index = pMat->nIndexInModel;
	cache_bone_t* pCacheBone = &m_pCacheBones[mat_index];
	unsigned int mapped_bone_num = pMat->nMappedBoneCount;

	if (NULL == pCacheBone->pBoneMatrices)
	{
		pCacheBone->pBoneMatrices = (FmVec4*)CORE_ALLOC(
			sizeof(FmVec4) * mapped_bone_num * 3);
		pCacheBone->nBoneMatrixNum = mapped_bone_num;
	}
	else if (optimize_by_sequence_id 
		&& pCacheBone->nBoneSequenceId == sequence_id)
	{
		return pCacheBone->pBoneMatrices;
	}

	unsigned int* bone_indices = pMat->pBoneList;
	FmVec4* bone_matrices = pCacheBone->pBoneMatrices;

	for (unsigned int i = 0; i < mapped_bone_num; ++i)
	{
		if (bone_indices[i] >= pInst->nTotalBoneNodeCount)
		{
			return NULL;
		}

		memcpy(&bone_matrices[i * 3], 
			&bone_node_list[bone_indices[i]]->mtxCurrent, 
			sizeof(FmVec4) * 3);
	}
	
	pCacheBone->nBoneSequenceId = sequence_id;
	
	return bone_matrices;
}

void CModelPlayer::CreateBillboard(model_node_t* pNode)
{
	// 如果是Billboard，我们对它进行附加的旋转操作
	if (NULL == m_pContext)
	{
		return;
	}

	/*
	if (node->uModelInfo & MODEL_BB_FLAG)
	{
		FmMat4 mtxViewInverse = m_pContext->GetCamera().mtxViewInverse;

		mtxViewInverse._41 = mtxViewInverse._42 = mtxViewInverse._43 = 0.0F;

		FmMat4 mtxTempTM;

		FmMatrixIdentity(&mtxTempTM);

		mtxTempTM._41 = m_mtxCurrentTM._41;
		mtxTempTM._42 = m_mtxCurrentTM._42;
		mtxTempTM._43 = m_mtxCurrentTM._43;

		location_t location;

		FXMATH_MatrixToLocation(&m_mtxCurrentTM, &location);

		FmMat4 mtxScale;

		FmMatrixScaling(&mtxScale, location.sx, location.sy, location.sz);

		FmMatrixMultiply(&m_mtxCurrentTM, &mtxViewInverse, &mtxScale);
		FmMatrixMultiply(&m_mtxCurrentTM, &m_mtxCurrentTM, &mtxTempTM);
	}
	*/

	if (pNode->nModelInfo & MODEL_BB_FLAG)
	{
		//const dx_camera_t& camera = g_pRender->GetMainCamera();

		//FmVec3 posView = FmVec3(camera.mtxViewInverse._41, 
		//	camera.mtxViewInverse._42, camera.mtxViewInverse._43);
		//const FmMat4& mtxViewInverse = 
		//	m_pContext->GetCamera().mtxViewInverse;

		//FmVec3 posView = FmVec3(mtxViewInverse._41, 
		//	mtxViewInverse._42, mtxViewInverse._43);

		FmVec3 posView = m_pContext->GetCamera().vPosition;
		FmMat4 mtxTemp;

		math_location_to_matrix(&pNode->pLocation[0], &mtxTemp);
		FmMatrixMultiply(&mtxTemp, &mtxTemp, &m_mtxCurrentTM);

		FmVec3 posLookAt = FmVec3(mtxTemp._41, mtxTemp._42, 
			mtxTemp._43);
		FmVec3 vecUp;
		float distance_x = posView.x - posLookAt.x;
		float distance_z = posView.z - posLookAt.z;

		if (distance_x < 0.0F)
		{
			distance_x = -distance_x;
		}

		if (distance_z < 0.0F)
		{
			distance_z = -distance_z;
		}

		if ((distance_x < 0.001F) && (distance_z < 0.001F))
		{
			vecUp = FmVec3(0.0F, 0.0F, 1.0F);
		}
		else
		{
			vecUp = FmVec3(0.0F, 1.0F, 0.0F);
		}

		FmMat4 mtxView;

		FmMat4LookAtLH(&mtxView, &posView, &posLookAt, &vecUp);
		FmMat4Inverse(&mtxView, NULL, &mtxView);
		{
			mtxView._41 = mtxView._42 = mtxView._43 = 0.0F;

			FmMat4 mtxTempTM;

			FmMatrixIdentity(&mtxTempTM);
			mtxTempTM._41 = m_mtxCurrentTM._41;
			mtxTempTM._42 = m_mtxCurrentTM._42;
			mtxTempTM._43 = m_mtxCurrentTM._43;

			//location_t location;

			//FXMATH_MatrixToLocation(&m_mtxCurrentTM, &location);
            FmVec3 scale;
            FmVec3 scaleView;
			FmMat4 mtxScale;

            VisUtil_GetMatrixScale(&scale, &m_mtxCurrentTM);
            VisUtil_GetMatrixScale(&scaleView, &mtxView);
            FmMatrixScaling(&mtxScale, scale.x/scaleView.x, scale.y/scaleView.y, scale.z/scaleView.z);
			FmMatrixMultiply(&m_mtxCurrentTM, &mtxView, &mtxScale);
			FmMatrixMultiply(&m_mtxCurrentTM, &m_mtxCurrentTM, 
				&mtxTempTM);
		}
	}

	// 如果是BillboardY，我们对它进行附加的旋转操作
	if (pNode->nModelInfo & MODEL_BBY_FLAG)
	{
		FmMat4 mtxViewInverse = m_pContext->GetCamera().mtxViewInverse;

		mtxViewInverse._41 = mtxViewInverse._42 = mtxViewInverse._43 = 0.0F;

		FmVec3 pos = FmVec3(0.0F, 0.0F, 1.0F);

		FmVec3TransformCoord(&pos, &pos, &mtxViewInverse);
		pos.y = 0.0F;
		FmVec3Normalize(&pos, &pos);

		FmMat4 mtxView;
		FmVec3 eye (0.0F, 0.0F, 0.0F);
		FmVec3 up (0.0F, 1.0F, 0.0F);
		FmMat4LookAtLH(&mtxView, &eye, &pos, 
			&up);
		FmMat4Inverse(&mtxView, NULL, &mtxView);

		FmMat4 mtxTempTM;

		FmMatrixIdentity(&mtxTempTM);
		mtxTempTM._41 = m_mtxCurrentTM._41;
		mtxTempTM._42 = m_mtxCurrentTM._42;
		mtxTempTM._43 = m_mtxCurrentTM._43;

		//location_t location;

		//FXMATH_MatrixToLocation(&m_mtxCurrentTM, &location);
        FmVec3 scale;
        FmVec3 scaleView;
		FmMat4 mtxScale;

        VisUtil_GetMatrixScale(&scale, &m_mtxCurrentTM);
        VisUtil_GetMatrixScale(&scaleView, &mtxView);
		FmMatrixScaling(&mtxScale, scale.x/scaleView.x, scale.y/scaleView.y, scale.z/scaleView.z);
		FmMatrixMultiply(&m_mtxCurrentTM, &mtxView, &mtxScale);
		FmMatrixMultiply(&m_mtxCurrentTM, &m_mtxCurrentTM, 
			&mtxTempTM);
	}

	// 如果是BillboardX，我们对它进行附加的旋转操作
	if (pNode->nModelInfo & MODEL_BBX_FLAG)
	{
		FmMat4 mtxViewInverse = m_pContext->GetCamera().mtxViewInverse;

		mtxViewInverse._41 = mtxViewInverse._42 = mtxViewInverse._43 = 0.0F;

		FmVec3 pos = FmVec3(0.0F, 0.0F, 1.0F);

		FmVec3TransformCoord(&pos, &pos, &mtxViewInverse);
		pos.x = 0.0F;
		pos.z = sqrt(1.0F - pos.y * pos.y);//VisUtil_SSE_Sqrt(1.0F - pos.y * pos.y);
		FmVec3Normalize(&pos, &pos);

		FmMat4 mtxView;

		FmVec3 eye(0.0F, 0.0F, 0.0F);
		FmVec3 up(0.0F, 1.0F, 0.0F);
		FmMat4LookAtLH(&mtxView, &eye, &pos, 
			&up);
		FmMat4Inverse(&mtxView, NULL, &mtxView);

		FmMat4 mtxTempTM;

		FmMatrixIdentity(&mtxTempTM);
		mtxTempTM._41 = m_mtxCurrentTM._41;
		mtxTempTM._42 = m_mtxCurrentTM._42;
		mtxTempTM._43 = m_mtxCurrentTM._43;

		//location_t location;

		//FXMATH_MatrixToLocation(&m_mtxCurrentTM, &location);
        FmVec3 scale;
        FmVec3 scaleView;
		FmMat4 mtxScale;

        VisUtil_GetMatrixScale(&scale, &m_mtxCurrentTM);
        VisUtil_GetMatrixScale(&scaleView, &mtxView);
        FmMatrixScaling(&mtxScale, scale.x/scaleView.x, scale.y/scaleView.y, scale.z/scaleView.z);
		FmMatrixMultiply(&m_mtxCurrentTM, &mtxView, &mtxScale);
		FmMatrixMultiply(&m_mtxCurrentTM, &m_mtxCurrentTM, 
			&mtxTempTM);
	}
}

static int element_sort_proc(const void* pE1, const void* pE2)
{
	Assert(pE1 != NULL);
	Assert(pE2 != NULL);
	/*

	const D3D11_INPUT_ELEMENT_DESC* e1 = (const D3D11_INPUT_ELEMENT_DESC*)pE1;
	const D3D11_INPUT_ELEMENT_DESC* e2 = (const D3D11_INPUT_ELEMENT_DESC*)pE2;

	if (e1->InputSlot < e2->InputSlot)
	{
		return -1;
	}
	else if (e1->InputSlot == e2->InputSlot)
	{
		if (e1->AlignedByteOffset < e2->AlignedByteOffset)
		{
			return -1;
		}
		else if (e1->AlignedByteOffset > e2->AlignedByteOffset)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 1;
	}
	*/
	return 1;
}

//inline void VertexAttribPointer(int indx, fm_int size, GLenum type, GLboolean normalized, fm_sizei stride, const void* ptr)
//{
//	glEnableVertexAttribArray((fm_uint)indx);
//	glVertexAttribPointer((fm_uint)indx, size, type, normalized, stride, ptr);
//}

void CModelPlayer::SetAniVertexData(int ani_offset, int stride, node_material_t* pMat, int vf1, int vf2)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetAniVertexData] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;
	float weight = m_fCurrentFrameOffset;

	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	//前祯
	//VertexAttribPointer(vf1, 3, GL_FLOAT, GL_FALSE, stride, (fm_void*)(ani_offset + m_nCurrentFrame * pMat->nMultiVertexSize));
	pRenderDrawOp->EnableVertexAttribArray( vf1 );
	pRenderDrawOp->SetVertexAttribPointer( vf1, 3,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)(ani_offset + m_nCurrentFrame * pMat->nMultiVertexSize));

	//后祯
	if (m_nCurrentFrame == (frame_num - 1))
	{
		if (m_bLoop)
		{
			//VertexAttribPointer(vf2, 3, GL_FLOAT, GL_FALSE, stride, (fm_void*)ani_offset);
			pRenderDrawOp->EnableVertexAttribArray(vf2);
			pRenderDrawOp->SetVertexAttribPointer(vf2, 3,IRenderDrawOp::VERTEX_DATA_FLOAT, stride, (fm_void*)ani_offset);
		}
		else
		{
			//VertexAttribPointer(vf2, 3, GL_FLOAT, GL_FALSE, stride, (fm_void*)(ani_offset + m_nCurrentFrame*pMat->nMultiVertexSize));
			pRenderDrawOp->EnableVertexAttribArray(vf2);
            pRenderDrawOp->SetVertexAttribPointer(vf2,3,IRenderDrawOp::VERTEX_DATA_FLOAT, stride, (fm_void*)(ani_offset + m_nCurrentFrame*pMat->nMultiVertexSize));
		}
	}
	else
	{
		//VertexAttribPointer(vf2, 3, GL_FLOAT, GL_FALSE, stride, (fm_void*)(ani_offset + (m_nCurrentFrame + 1)*pMat->nMultiVertexSize));
		pRenderDrawOp->EnableVertexAttribArray(vf2);
        pRenderDrawOp->SetVertexAttribPointer(vf2,3,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)(ani_offset + (m_nCurrentFrame + 1)*pMat->nMultiVertexSize));
	}
}

bool CModelPlayer::CreateVDecl(node_material_t* pMat, model_node_t* pNode, 
	bool lod)
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	//vertex_t* pvertex = (vertex_t*)pMat->SingleVB.pVertices;
	int offset = 0;
	int ani_offset = pMat->nSingleVertexSize;
	unsigned int vertex_define = 0;

	ModelVF& vf = pMat->vf;

	int stride = pMat->SingleVB.nStride;
	//POS
	if(pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		int POS = vf.GetVF(ModelVF::VERTEX);
		pRenderDrawOp->EnableVertexAttribArray(POS);
		pRenderDrawOp->SetVertexAttribPointer(POS,3,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
		offset += sizeof(float)*3;
	}
	else if(pNode->nModelInfo & MODEL_POSITION_ANI)
	{
		SetAniVertexData(ani_offset, stride, pMat, vf.GetVF(ModelVF::VERTEX), vf.GetVF(ModelVF::VERTEX_ANI));
		ani_offset += sizeof(float)*3;
	}

	//NORAML
	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		int NORMAL = vf.GetVF(ModelVF::NORMAL);
		pRenderDrawOp->EnableVertexAttribArray(NORMAL);
		pRenderDrawOp->SetVertexAttribPointer(NORMAL,3,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
		offset += sizeof(float)*3;
	}
	else if(pNode->nModelInfo & MODEL_NORMAL_ANI)
	{
		SetAniVertexData(ani_offset, stride, pMat, vf.GetVF(ModelVF::NORMAL), vf.GetVF(ModelVF::NORMAL_ANI));
		ani_offset += sizeof(float)*3;
	}
	
	//COLOR
	// Color
	if (pNode->nModelInfo & MODEL_COLOR_INFO)
	{
		int COLOR = vf.GetVF(ModelVF::DIFFUSE);
		pRenderDrawOp->EnableVertexAttribArray(COLOR);
		pRenderDrawOp->SetVertexAttribPointer(COLOR,4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE,stride, (fm_void*)offset);
		offset += sizeof(unsigned int);
		vertex_define |= (0x1 << VERTEX_COLOR_SHIFT);
	}
	else if(pNode->nModelInfo & MODEL_COLOR_ANI)
	{
		int COLOR = vf.GetVF(ModelVF::DIFFUSE_ANI);
		pRenderDrawOp->EnableVertexAttribArray(COLOR);
		pRenderDrawOp->SetVertexAttribPointer(COLOR,4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE,stride, (fm_void*)ani_offset);
		ani_offset += sizeof(float)*3;
		vertex_define |= (0x1 << VERTEX_COLOR_SHIFT);
	}

	// Illum
	if (pNode->nModelInfo & MODEL_ILLUM_INFO)
	{
		int ILLUM = vf.GetVF(ModelVF::ILLUM);
		pRenderDrawOp->EnableVertexAttribArray(ILLUM);
		pRenderDrawOp->SetVertexAttribPointer(ILLUM,1,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
		offset += sizeof(float);
	}
	else if(pNode->nModelInfo & MODEL_ILLUM_ANI)
	{
		int ILLUM = vf.GetVF(ModelVF::ILLUM_ANI);
		pRenderDrawOp->EnableVertexAttribArray(ILLUM);
        pRenderDrawOp->SetVertexAttribPointer(ILLUM,1,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)ani_offset);
		ani_offset += sizeof(float);
	}

	// Tangent
	if (pNode->nModelInfo & MODEL_TANGENT_INFO)
	{
		int TANGENT = vf.GetVF(ModelVF::TANGENT);
		pRenderDrawOp->EnableVertexAttribArray(TANGENT);
		pRenderDrawOp->SetVertexAttribPointer(TANGENT,3,IRenderDrawOp::VERTEX_DATA_FLOAT, stride, (fm_void*)offset);
		offset += sizeof(float) * 3;
		vertex_define |= (0x1 << VERTEX_TANGENT_SHIFT);
	}
	else if(pNode->nModelInfo & MODEL_TANGENT_ANI)
	{
		int TANGENT = vf.GetVF(ModelVF::TANGENT_ANI);
		pRenderDrawOp->EnableVertexAttribArray(TANGENT);
		pRenderDrawOp->SetVertexAttribPointer(TANGENT,3,IRenderDrawOp::VERTEX_DATA_FLOAT, stride, (fm_void*)ani_offset);
		ani_offset += sizeof(float) * 3;
		vertex_define |= (0x1 << VERTEX_TANGENT_SHIFT);
	}

	// Binormal
	if (pNode->nModelInfo & MODEL_BINORMAL_INFO)
	{
		int BINORMAL = vf.GetVF(ModelVF::BINORMAL);
		pRenderDrawOp->EnableVertexAttribArray(BINORMAL);
		pRenderDrawOp->SetVertexAttribPointer(BINORMAL,3,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
		offset += sizeof(float)*3;
		vertex_define |= (0x1 << VERTEX_BINORMAL_SHIFT);
	}
	else if(pNode->nModelInfo & MODEL_BINORMAL_ANI)
	{
		int BINORMAL = vf.GetVF(ModelVF::BINORMAL_ANI);
		pRenderDrawOp->EnableVertexAttribArray(BINORMAL);
		pRenderDrawOp->SetVertexAttribPointer(BINORMAL,3,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)ani_offset);
		ani_offset += sizeof(float)*3;
		vertex_define |= (0x1 << VERTEX_BINORMAL_SHIFT);
	}

	// Diffuse texture
	if (pMat->nMaterialInfo & (MATERIAL_DIFFUSE_MAP_INFO | MATERIAL_BUMP_MAP_INFO
		| MATERIAL_SPECULAR_MAP_INFO | MATERIAL_SELF_ILLUMINATION_INFO))
	{
		int UV = vf.GetVF(ModelVF::TEXCOORD);
		if (pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO)
		{
			pRenderDrawOp->EnableVertexAttribArray(UV);
			pRenderDrawOp->SetVertexAttribPointer(UV,2,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
			offset += sizeof(float)*2;

			vertex_define |= (0x1 << VERTEX_TEXTURE0_SHIFT);

			UV = vf.GetVF(ModelVF::TEXCOORD2);
			pRenderDrawOp->EnableVertexAttribArray(UV);
			pRenderDrawOp->SetVertexAttribPointer(UV,2,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
			offset += sizeof(float)*2;

			vertex_define |= (0x1 << VERTEX_TEXTURE1_SHIFT);
		}
		else
		{
			pRenderDrawOp->EnableVertexAttribArray(UV);
			pRenderDrawOp->SetVertexAttribPointer(UV,2,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
			offset += sizeof(float)*2;
			vertex_define |= (0x1 << VERTEX_TEXTURE0_SHIFT);
		}

		if(pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO)
		{
			UV = vf.GetVF(ModelVF::TEXCOORD_FILTER);
			pRenderDrawOp->EnableVertexAttribArray(UV);
			pRenderDrawOp->SetVertexAttribPointer(UV,2,IRenderDrawOp::VERTEX_DATA_FLOAT, stride, (fm_void*)offset);
			offset += sizeof(float)*2;
		}
	}

	//BONEID && WEIGHT
	if ((pNode->nType != FXNODE_TYPE_MESH) && !lod)
	{
		int BONE = vf.GetVF(ModelVF::BONE);
		pRenderDrawOp->EnableVertexAttribArray(BONE);
		pRenderDrawOp->SetVertexAttribPointer(BONE,4,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
		offset += sizeof(float)*4;
		int WEIGHT = vf.GetVF(ModelVF::WEIGHT);
		pRenderDrawOp->EnableVertexAttribArray(WEIGHT);
		pRenderDrawOp->SetVertexAttribPointer(WEIGHT,4,IRenderDrawOp::VERTEX_DATA_FLOAT,stride, (fm_void*)offset);
		offset += sizeof(float)*4;
#ifndef CPU_COMPUTE_BONE
		vertex_define |= (0x1 << VERTEX_SKELETON_SHIFT);
#endif
	}
	
	if ((pNode->nModelInfo & MODEL_POSITION_ANI)
		&& (pNode->nModelInfo & MODEL_NORMAL_ANI))
	{
		vertex_define |= (0x1 << VERTEX_ANIMATION_SHIFT);
	}

	pMat->nVertexDefine = vertex_define;

	return true;
}

inline void modify_bone_data(vertex_data_t& pData, const node_weight_table_t* weight_table)
{
	int dst_offset = pData.nStride - sizeof(int)*4 - sizeof(float)*4;
	unsigned char* dst_ptr = pData.pVertices;
	int offset = 0;

	for(unsigned int i = 0; i < pData.nCount; i++)
	{
		dst_ptr += dst_offset;

		float fBoneId1 = (float)max(weight_table[offset].nBoneID, 0);
		float fBoneId2 = (float)max(weight_table[offset + 1].nBoneID, 0);
		float fBoneId3 = (float)max(weight_table[offset + 2].nBoneID, 0);
		float fBoneId4 = (float)max(weight_table[offset + 3].nBoneID, 0);
	
		//BONEID
		memcpy(dst_ptr, &fBoneId1, sizeof(float));
		dst_ptr += sizeof(int);
		memcpy(dst_ptr, &fBoneId2, sizeof(float));
		dst_ptr += sizeof(int);
		memcpy(dst_ptr, &fBoneId2, sizeof(float));
		dst_ptr += sizeof(int);
		memcpy(dst_ptr, &fBoneId4, sizeof(float));
		dst_ptr += sizeof(int);
		
		//WEIGHT
		memcpy(dst_ptr, &weight_table[offset].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		memcpy(dst_ptr, &weight_table[offset+1].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		memcpy(dst_ptr, &weight_table[offset+2].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		memcpy(dst_ptr, &weight_table[offset+3].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		
		offset += 4;
	}
}

inline void modify_multi_data(vertex_data_t& pData, float** pMultiVBs, unsigned int multi_size, unsigned int frame_num)
{

	int new_stride = pData.nStride + multi_size*frame_num;
	unsigned char* pVertices = (unsigned char*)CORE_ALLOC(new_stride * pData.nCount);
	unsigned char* pSrcData = pData.pVertices;
	unsigned char* pDstData = pVertices;
	
	for(unsigned int i = 0; i < pData.nCount; i++)
	{
		memcpy(pDstData, pSrcData, pData.nStride);

		int offset = 0;
		for(unsigned int j = 0; j < frame_num; j++)
		{
			unsigned char* pSrcMultiData = (unsigned char*)pMultiVBs[j];
			memcpy(pDstData + pData.nStride + offset, &pSrcMultiData[multi_size * i], multi_size);
			offset += multi_size;
		}
		pSrcData += pData.nStride;
		pDstData += new_stride;
	}
	
	CORE_FREE(pData.pVertices, pData.nCount * pData.nStride);
	pData.pVertices = pVertices;
	pData.nStride = new_stride;
}

bool CModelPlayer::CreateVBIB(node_material_t* pMat, model_node_t* pNode)
{
	
	// Initialize single vertex buffer.
	//if ((pMat->pSingleGPUVB == NULL) && pMat->pSingleVB != NULL)
	if ((pMat->pSingleGPUVB == NULL) && (pMat->SingleVB.nCount > 0))
	{
		if(pNode->nType == FXNODE_TYPE_SKIN)
		{
			modify_bone_data(pMat->SingleVB, pMat->pWeightTable);
		}
		else if(pMat->pMultiVBs != NULL)
		{
			model_t* pInst = m_pResModel->GetModelData();

			if (pInst == NULL)
			{
				CORE_TRACE_EX("WARNING: [CModelPlayer::CreateVBIB] pInst == NULL model name is %s", m_pResModel->GetName());
				return false;
			}

			unsigned int frame_num = pInst->nEndFrame - pInst->nStartFrame + 1;
			modify_multi_data(pMat->SingleVB, pMat->pMultiVBs, pMat->nMultiVertexSize, frame_num);			
		}
		
		unsigned int vb_size = pMat->nVertexCount * pMat->SingleVB.nStride;

		pMat->pSingleGPUVB = g_pRender->CreateStaticVB(pMat->SingleVB.pVertices, vb_size);
		//pMat->pSingleGPUVB = g_pRender->CreateStaticVB(vb_size, 0);
		
		if (NULL == pMat->pSingleGPUVB)
		{
			//FXLOG_ERROR("[error 030043] CreateVBIB:Create single vb failed!\n");
			return false;
		}

//		unsigned char* pTemp = (unsigned char*)pMat->pSingleGPUVB->Lock();
//
//		if (NULL == pTemp)
//		{
//			//FXLOG_ERROR("[error 030045] CreateVBIB:Lock single vb failed!\n");
//			return false;
//		}
//
//		for (size_t block = 0; block < DX_VERTEX_BLOCK_NUM; ++block)
//		{
//			size_t block_size = get_vertex_block_size(&(pMat->SingleVB), block);
//
//			if (block_size == 0)
//			{
//				break;
//			}
//
//			size_t data_size = pMat->uSingleVertexSize * block_size;
//
//			memcpy(pTemp, pMat->SingleVB.pBlocks[block], data_size);
//			pTemp += data_size;		
//		}
//		//memcpy(pTemp, pMat->pSingleVB, vb_size);
//
//		pMat->pSingleGPUVB->Unlock();
	}

	// Initialize multi vertex buffer
	//if (pMat->pMultiGPUVB == NULL && pMat->pMultiVB != NULL)
	if (pMat->pMultiGPUVB == NULL && pMat->pMultiVBs != NULL)
	{
		model_t* pInst = m_pResModel->GetModelData();

		unsigned int frame_num = pInst->nEndFrame - pInst->nStartFrame + 1;
		unsigned int multi_size = pMat->nVertexCount * pMat->nMultiVertexSize;
		unsigned int vb_size = multi_size * frame_num;
		
		//pMat->pMultiGPUVB = g_pRender->CreateStaticVB(vb_size, 0);
		//pMat->pMultiGPUVB = g_pRender->CreateDynamicVB(vb_size);

		if (NULL == pMat->pMultiGPUVB)
		{
			//FXLOG_ERROR("[error 030044] CreateVBIB:Create multi vb failed!\n");
		//	return false;
		}

		//unsigned char* pTemp = (unsigned char*)pMat->pMultiGPUVB->Lock();
		/*
		unsigned char* pTemp = (unsigned char*)g_pRender->LockDynamicVB(
			pMat->pMultiGPUVB);

		if (NULL == pTemp)
		{
			//FXLOG_ERROR("[error 030045] CreateVBIB:Lock single vb failed!\n");
			return false;
		}

		for (unsigned int k = 0; k < frame_num; ++k)
		{
			memcpy(pTemp, pMat->pMultiVBs[k], multi_size);
			pTemp += multi_size;
		}
		//memcpy(pTemp, pMat->pMultiVB, vb_size);

		//pMat->pMultiGPUVB->Unlock();
		g_pRender->UnlockDynamicVB(pMat->pMultiGPUVB);
		*/
	}

	// Initialize index buffer
	if ((pMat->pGPUIB == NULL) && (pMat->pLODIB[pMat->nCurLODLevel] != NULL))
	{
		unsigned int ib_size = pMat->pLODIndexCount[pMat->nCurLODLevel] 
			* sizeof(unsigned short);

		pMat->pGPUIB = g_pRender->CreateStaticIB(pMat->pLODIB[pMat->nCurLODLevel], ib_size);
		//pMat->pGPUIB = g_pRender->CreateStaticIB(ib_size);
		
		if (NULL == pMat->pGPUIB)
		{
			//FXLOG_ERROR("[error 030046] CreateVBIB:Create ib failed!\n");
			return false;
		}

		//void* pTemp = pMat->pGPUIB->Lock();

		//if (NULL == pTemp)
		//{
		//	//FXLOG_ERROR("[error 030047] CreateVBIB:Lock ib failed!\n");
		//	return false;
		//}

		//memcpy(pTemp, pMat->pLODIB[pMat->uCurLODLevel], ib_size);

		//pMat->pGPUIB->Unlock();
	}

	return true;
}

void CModelPlayer::CreateTextureMatrix(node_material_t* pMat,  
	FmMat4* mtxTex)
{
	model_t* pInst = m_pResModel->GetModelData();

	if (pInst == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::CreateTextureMatrix] pInst == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	unsigned int frame_num = pInst->nEndFrame - pInst->nStartFrame + 1;
	float weight = m_fCurrentFrameOffset;

	if (pMat->bNoLerpT)
	{
		uv_param_t param_src = pMat->pFrameDiffuseTM[m_nCurrentFrame];

		math_create_texture_ani_matrix(mtxTex, 
			param_src.fOffsetU,	param_src.fOffsetV,
			param_src.fRotateU,	param_src.fRotateV,	param_src.fRotateW,	
			param_src.fTilingU,	param_src.fTilingV);
		//FmMatrixTranspose(mtxTex, mtxTex);
	}
	else
	{
		uv_param_t param_src = pMat->pFrameDiffuseTM[m_nCurrentFrame];
		uv_param_t param_dst;

		if (m_nCurrentFrame == (frame_num - 1))
		{
			if (m_bLoop)
			{
				param_dst = pMat->pFrameDiffuseTM[0];
			}
			else
			{
				param_dst = pMat->pFrameDiffuseTM[m_nCurrentFrame];
			}
		}
		else
		{
			param_dst = pMat->pFrameDiffuseTM[m_nCurrentFrame + 1];
		}

		param_src.fOffsetU += (param_dst.fOffsetU - param_src.fOffsetU) * weight;
		param_src.fOffsetV += (param_dst.fOffsetV - param_src.fOffsetV) * weight;
		param_src.fRotateU += (param_dst.fRotateU - param_src.fRotateU) * weight;
		param_src.fRotateV += (param_dst.fRotateV - param_src.fRotateV) * weight;
		param_src.fRotateW += (param_dst.fRotateW - param_src.fRotateW) * weight;
		param_src.fTilingU += (param_dst.fTilingU - param_src.fTilingU) * weight;
		param_src.fTilingV += (param_dst.fTilingV - param_src.fTilingV) * weight;

		math_create_texture_ani_matrix(mtxTex, 
			param_src.fOffsetU,	param_src.fOffsetV,
			param_src.fRotateU,	param_src.fRotateV,	param_src.fRotateW,
			param_src.fTilingU,	param_src.fTilingV);
		//FmMatrixTranspose(mtxTex, mtxTex);
	}
}

/*

// 设置批次的几何体信息
bool CModelPlayer::SetBatchGeometry(DxSceneBatch* pBatch, 
	node_material_t* pMat, model_node_t* pNode)
{
	// 设置顶点定义
	pBatch->SetInputLayout(pMat->pInputLayout);

	if (pMat->pMultiGPUVB != NULL)
	{
		// 设置顶点动画缓冲
		model_t* pModel = m_pResModel->GetModelData();
		unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;
		float weight = m_fCurrentFrameOffset;
		unsigned int vb_size = pMat->nVertexCount * pMat->nMultiVertexSize;

		pBatch->SetVertexBuffer(1, pMat->pMultiGPUVB, 
			pMat->nMultiVertexSize, m_nCurrentFrame * vb_size);

		if (m_nCurrentFrame == (frame_num - 1))
		{
			if (m_bLoop)
			{
				pBatch->SetVertexBuffer(2, pMat->pMultiGPUVB, 
					pMat->nMultiVertexSize, 0);
			}
			else
			{
				pBatch->SetVertexBuffer(2, pMat->pMultiGPUVB, 
					pMat->nMultiVertexSize, m_nCurrentFrame * vb_size);
			}
		}
		else
		{
			pBatch->SetVertexBuffer(2, pMat->pMultiGPUVB, 
				pMat->nMultiVertexSize, (m_nCurrentFrame + 1) * vb_size);
		}
	}
	
	if (pMat->pSingleGPUVB != NULL)
	{
		pBatch->SetVertexBuffer(0, pMat->pSingleGPUVB, 
			pMat->nSingleVertexSize, 0);
	}
	//else
	//{
	//	//pBatch->SetCancel("Single vertex buffer is NULL");
	//	return false;
	//}

	// 设置骨骼动画顶点缓冲
	if (pNode->nType == FXNODE_TYPE_SKIN)
	{
		if (pMat->pVBlendGPUVB != NULL)
		{
			pBatch->SetVertexBuffer(3, pMat->pVBlendGPUVB,  
				pMat->nSkinnedVertexBoneBlendVertexSize, 0);
		}
		//else
		//{
		//	//pBatch->SetCancel("Skin mesh vertex is NULL");
		//	return false;
		//}
	}

	// 几何体信息 
	pBatch->SetIndexBuffer(pMat->pGPUIB, 0);
	pBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, pMat->nVertexCount, 
		pMat->pLODIndexCount[pMat->nCurLODLevel]);

	return true;
}
*/

/*
bool CModelPlayer::SetBatchOcclusionQuery(DxSceneBatch* pBatch, 
	node_material_t* pMat, model_node_t* pNode)
{
	IDxContext* pContext = g_pRender->GetContext();
	FmMat4 mtxInverse;
	FmVec3 vPos;
	FmVec3 vMin;
	FmVec3 vMax;

	FmXMatrixInverse(&mtxInverse, NULL, &m_mtxCurrentTM);
	FmVec3TransformCoord(&vPos, &pContext->GetCamera().vPosition, 
		&mtxInverse);
	GetNodeLocationBoundingBox(vMin, vMax, pNode->vBoundMin, 
		pNode->vBoundMax);

	if (VisUtil_BoxContainPoint(vMin, vMax, vPos))
	{
		// 摄像机在物体包围盒中时遮挡查询可能会不正确
		return false;
	}

	const FmVec3& refer_pos = pContext->GetReferPosition();
	// 摄像机在物体包围盒中时遮挡查询可能会不正确
	FmMat4 mtxWorld = m_mtxCurrentTM;
	char uid[64];

	mtxWorld._41 -= refer_pos.x;
	mtxWorld._42 -= refer_pos.y;
	mtxWorld._43 -= refer_pos.z;
	FmVec3TransformCoord(&vMin, &vMin, &mtxWorld);
	FmVec3TransformCoord(&vMax, &vMax, &mtxWorld);
	SafeSprintf(uid, sizeof(uid), "mda_%d_%d", m_nRefId, pMat->nIndexInModel);

	pBatch->SetBoundBox(vMin, vMax);
	pBatch->SetUniqueId(uid);

	return true;
}
*/

// 获得当前的材质透明度
float CModelPlayer::GetCurrentOpacity(node_material_t* pMat, 
	model_node_t* pNode, unsigned int frame_num, float weight)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	float opacity = 1.0F;
	
	if (pMat->nMaterialInfo & MATERIAL_OPACITY_ANI)
	{
		float opacity_src = pMat->pFrameOpacity[m_nCurrentFrame];
		float opacity_dst;

		if (m_nCurrentFrame == frame_num - 1)
		{
			if (m_bLoop)
			{
				opacity_dst = pMat->pFrameOpacity[0];
			}
			else
			{
				opacity_dst = pMat->pFrameOpacity[m_nCurrentFrame];
			}
		}
		else
		{
			opacity_dst = pMat->pFrameOpacity[m_nCurrentFrame + 1];
		}

		if (pMatInfo->bDisappear)
		{
			IRenderContext* pContext = g_pRender->GetContext();
			FmVec3 cam_pos = pContext->GetCamera().vPosition;
			FmVec3 delta;

			delta.x = (pNode->vBoundMin.x + pNode->vBoundMax.x) * 0.5F;
			delta.y = (pNode->vBoundMin.y + pNode->vBoundMax.y) * 0.5F;
			delta.z = (pNode->vBoundMin.z + pNode->vBoundMax.z) * 0.5F;
			delta.x += m_mtxCurrentTM._41;
			delta.y += m_mtxCurrentTM._42;
			delta.z += m_mtxCurrentTM._43;
			delta.x -= cam_pos.x;
			delta.y -= cam_pos.y;
			delta.z -= cam_pos.z;

			float fDistance = VisUtil_Vec3Length(&delta);
			float fOpacityWeight = (1.0F - fDistance / 100.0F);

			opacity_src *= fOpacityWeight;
		}

		opacity = opacity_src + (opacity_dst - opacity_src) * weight;
	}
	else
	{
		opacity = pMatInfo->fOpacity;

		if (pMatInfo->bDisappear)
		{
			IRenderContext* pContext = g_pRender->GetContext();
			FmVec3 cam_pos = pContext->GetCamera().vPosition;
			FmVec3 delta;

			delta.x = (pNode->vBoundMin.x + pNode->vBoundMax.x) * 0.5F;
			delta.y = (pNode->vBoundMin.y + pNode->vBoundMax.y) * 0.5F;
			delta.z = (pNode->vBoundMin.z + pNode->vBoundMax.z) * 0.5F;
			delta.x += m_mtxCurrentTM._41;
			delta.y += m_mtxCurrentTM._42;
			delta.z += m_mtxCurrentTM._43;
			delta.x -= cam_pos.x;
			delta.y -= cam_pos.y;
			delta.z -= cam_pos.z;

			float fDistance = VisUtil_Vec3Length(&delta);
			float fOpacityWeight = (1.0F - fDistance / 100.0F);

			if (fOpacityWeight < 0.0F)
			{
				fOpacityWeight = 0.0F;
			}
			else if (fOpacityWeight > 1.0F)
			{
				fOpacityWeight = 1.0F;
			}

			opacity *= fOpacityWeight;
		}
	}

	return opacity;
}

bool CModelPlayer::CheckMaterialData(node_material_t* pMat, int offset, 
	int stride)
{
	model_t* pInst = m_pResModel->GetModelData();

	if (pInst == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::CheckMaterialData] pInst == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	unsigned int frame_num = pInst->nEndFrame - pInst->nStartFrame + 1;
	// 数据不会超过4个float，就不要动态申请内存了
	float pBuffer[4] = { 0 };
	unsigned int count = stride / sizeof(float);
	unsigned char* pv = NULL;

	if (0 == pMat->nMultiVertexSize)
	{
		for (unsigned int i = 0; i < pMat->nVertexCount; ++i)
		{
			pv = (unsigned char*)
				(get_vertex_by_index(&pMat->SingleVB, i) + offset);
			memcpy(pBuffer, pv, stride);

			for (unsigned int j = 0; j < count; ++j)
			{
				if (Port_IsNaN(pBuffer[j]))
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (unsigned int k = 0; k < frame_num; ++k)
		{
			for (unsigned int i = 0; i < pMat->nVertexCount; ++i)
			{
				pv = (unsigned char*)pMat->pMultiVBs[k] 
					+ pMat->nMultiVertexSize * i + offset;
				memcpy(pBuffer, pv, stride);
				
				for (unsigned int j = 0; j < count; ++j)
				{
					if (Port_IsNaN(pBuffer[j]))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

// 检查材质是否存在无效数据
void CModelPlayer::CheckMaterialAllData(node_material_t* pMat)
{
	// 检查顶点
	{
		int offset = GetPositionOffset(pMat);

		if (offset >= 0)
		{
			int pos_stride = sizeof(float) * 3;

			if (!CheckMaterialData(pMat, offset, pos_stride))
			{
				//FXLOG_ERROR("[error vertex data]:The position data is invalid,"
				//	"must to modify! ModelName = %s, MatName = %s\n", 
				//	m_pszResourceFileName, pMat->pszName);
			}
		}
		else
		{
			//FXLOG_ERROR("[error vertex data]:The position data is not exist, "
			//	"must to modify! ModelName = %s, MatName = %s\n", 
			//	m_pszResourceFileName, pMat->pszName);
		}
	}

	// 检查法线
	{
		int offset = GetNormalOffset(pMat);

		if (offset > 0)
		{
			int nor_stride = sizeof(float) * 3;

			if (!CheckMaterialData(pMat, offset, nor_stride))
			{
				//FXLOG_ERROR("[error vertex data]:The normal data is invalid, "
				//	"must to modify! ModelName = %s, MatName = %s\n", 
				//	m_pszResourceFileName, pMat->pszName);
			}
		}
	}
}

//extern IShaderProgram* g_pShader;

void CModelPlayer::SetShaderConstValue(IShaderProgram* pShader, const MatInfo* info)
{
	IRenderContext* pContext = g_pRender->GetContext();
	const camera_t& camera = info->camera; //pContext->GetCamera();
	const FmVec3& refer_pos = info->refer_pos; //pContext->GetReferPosition();
	//const camera_t& camera = pContext->GetCamera();
	//const FmVec3& refer_pos = pContext->GetReferPosition();
	node_material_t* pMat = info->pMat;
	model_node_t* pNode = info->pNode;
	material_info_t* pMatInfo = info->pMatInfo;
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetShaderConstValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}
	
	//CB_WORLD
	FmMat4 mtxWorld = info->mtxCurrentTM;//m_mtxCurrentTM;
	mtxWorld._41 -= (float)refer_pos.x;
	mtxWorld._42 -= (float)refer_pos.y;
	mtxWorld._43 -= (float)refer_pos.z;
	//FmMat4Transpose(&mtxWorld, &mtxWorld);
	ShaderManager::Inst().SetShaderValueMat4(c_mtxWorld, mtxWorld);

	FmMat4 mtxWVP;
	FmMat4Multiply(&mtxWVP, &mtxWorld, &camera.mtxViewProjRelative);
	ShaderManager::Inst().SetShaderValueMat4(c_mtxWVP, mtxWVP);
	
	if(pMatInfo->bNoLight)
	{
		float light_ambient[4] = { 1.0F, 1.0F, 1.0F, 1.0F };
		float light_diffuse[4] = { 0.0F, 0.0F, 0.0F, 1.0F };
				
		ShaderManager::Inst().SetShaderValue4f(c_LightAmbient, light_ambient);
		ShaderManager::Inst().SetShaderValue4f(c_LightDiffuse, light_diffuse);
	}
	else
	{
		FmVec4 light_ambient = pContext->GetVector4(IRenderContext::V_LIGHT_AMBIENT);
		FmVec4 light_diffuse = pContext->GetVector4(IRenderContext::V_LIGHT_DIFFUSE);
		light_ambient.w = pContext->GetFloat(IRenderContext::F_REFLECT_FACTOR);
		ShaderManager::Inst().SetShaderValue4f(c_LightAmbient, light_ambient);
    //ShaderManager::Inst().SetShaderValue4f(c_LightDiffuse, light_diffuse);
    ShaderManager::Inst().SetShaderValue4f(c_LightDiffuse, FmVec4(0.5, 0.5, 0.5, 1));
	}

	if (pContext->GetEnableDynamicShadow())
	{
		FmMat4 viewRelative;

		FmMat4Inverse(&viewRelative, NULL, pContext->GetShadowMapViewMatrix(0));
		viewRelative._41 -= refer_pos.x;
		viewRelative._42 -= refer_pos.y;
		viewRelative._43 -= refer_pos.z;

		FmMat4Inverse(&viewRelative, NULL, &viewRelative);

		FmMat4MultiplyTranspose(&mtxWVP, &viewRelative, pContext->GetShadowMapProjMatrix(0));
		ShaderManager::Inst().SetShaderValueMat4(c_mtxShadowViewProj, mtxWVP);
	}

	bool blend_quality = pMatInfo->bBlendQuality;

	float alpha_ref;
	if(pMatInfo->fAlphaRef < 0.001f && blend_quality)
	{
		alpha_ref = 0.9f;
	}
	else
	{
		alpha_ref = get_current_alpha_ref(pMatInfo->fAlphaRef) / 255.0F;
	}
	
	unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;
	float frame_time = (m_fCurrentFrameOffset + (float)m_nCurrentFrame) 
		/ (float)(frame_num - 1);
	
	//CB_BUMP_SCALE
	ShaderManager::Inst().SetShaderValue1f(c_fBumpScale, pMat->MatInfo.fBumpScale);
	ShaderManager::Inst().SetShaderValue1f(c_fReflectScale, pMat->MatInfo.fReflectFactor);
	ShaderManager::Inst().SetShaderValue1f(c_fSaturationScale, pMat->MatInfo.fSaturationScale);
	ShaderManager::Inst().SetShaderValue1f(c_fGlowAlpha, 1.0f);

	ShaderManager::Inst().SetShaderValue1f(c_fBlendWeight, m_fCurrentFrameOffset);
	ShaderManager::Inst().SetShaderValue1f(c_fAlphaRefValue, alpha_ref);
	ShaderManager::Inst().SetShaderValue1f(c_fTime, frame_time);
	ShaderManager::Inst().SetShaderValue1f(c_fGlowSize, pMatInfo->fGlowSize);
	

	//设置骨骼矩阵
	const FmVec3 & pos = g_pRender->GetContext()->GetReferPosition();
	if(info->bone_matrices)
		ShaderManager::Inst().SetShaderValue4fv(c_BoneList, pMat->nMappedBoneCount * 3, (float*)info->bone_matrices);
	
//	g_pRender->UpdateDynamicVB((IDynamicVB*), pMat->pSingleGPUVB,
	

	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_UV_ANI)
	{
		FmMat4 mtxDiffuseTM;

		CreateTextureMatrix(pMat, &mtxDiffuseTM);
		ShaderManager::Inst().SetShaderValueMat4(c_mtxDiffuseTM, mtxDiffuseTM);
	}

	// 自发光材质参数
	if (pMat->nMaterialInfo & MATERIAL_EMISSIVE_ANI)
	{
		FmVec4 emissive_src = FmVec4(
			pMat->pFrameEmissive[m_nCurrentFrame].x,
			pMat->pFrameEmissive[m_nCurrentFrame].y,
			pMat->pFrameEmissive[m_nCurrentFrame].z, 1.0F);				
		FmVec4 emissive_dst;

		if (m_nCurrentFrame == frame_num - 1)
		{
			if (m_bLoop)
			{
				emissive_dst = FmVec4(
					pMat->pFrameEmissive[0].x,
					pMat->pFrameEmissive[0].y, 
					pMat->pFrameEmissive[0].z, 1.0F);
			}
			else
			{
				emissive_dst = FmVec4(
					pMat->pFrameEmissive[m_nCurrentFrame].x,
					pMat->pFrameEmissive[m_nCurrentFrame].y,
					pMat->pFrameEmissive[m_nCurrentFrame].z, 1.0F);
			}
		}
		else
		{
			emissive_dst = FmVec4(
				pMat->pFrameEmissive[m_nCurrentFrame + 1].x,
				pMat->pFrameEmissive[m_nCurrentFrame + 1].y,
				pMat->pFrameEmissive[m_nCurrentFrame + 1].z, 1.0F);
		}

		emissive_src += (emissive_dst - emissive_src) * m_fCurrentFrameOffset;

		ShaderManager::Inst().SetShaderValue4f(c_MaterialEmissive, emissive_src);
	}
	else
	{
		FmVec4 value(pMatInfo->vEmissive.x, pMatInfo->vEmissive.y, 
			pMatInfo->vEmissive.z, 1.0F);

		ShaderManager::Inst().SetShaderValue4f(c_MaterialEmissive, value);
	}

	// 轮廓光参数
	if (pMatInfo->bRimLight)
	{
		ShaderManager::Inst().SetShaderValue4f(c_FallOffParam, 
			pMatInfo->vRimColor.x * pMatInfo->vEmissive.x * pMatInfo->fRimIntensity,
			pMatInfo->vRimColor.y * pMatInfo->vEmissive.y * pMatInfo->fRimIntensity, 
			pMatInfo->vRimColor.z * pMatInfo->vEmissive.z * pMatInfo->fRimIntensity, 
			pMatInfo->fRimPower);
	}
	else if(m_bFallOff && pNode->nType == FXNODE_TYPE_SKIN)
	{
		// 轮廓光全局开关
		if (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_FALLOFF) != 0)
		{
			// 轮廓光参数
			unsigned int falloff_color = pContext->GetColor(
				IRenderContext::C_ROLE_FALLOFF_COLOR);
			float falloff_inten = pContext->GetFloat(
				IRenderContext::F_ROLE_FALLOFF_INTENSITY);
			float falloff_power = pContext->GetFloat(
				IRenderContext::F_ROLE_FALLOFF_POWER);
			FmVec4 falloff_param;

			{
				falloff_param = VisUtil_GetColor2(falloff_color, 
					falloff_inten);
			}

			falloff_param.w = falloff_power;

			ShaderManager::Inst().SetShaderValue4f(c_FallOffParam, 
			falloff_param.x,
			falloff_param.y, 
			falloff_param.z, 
			falloff_param.w);
		}
	}
	
	// 次表面参数
	if (pMatInfo->bSkinEffect)
	{
		float value[4] = { pMatInfo->fSubSurfaceScale,
			pMatInfo->fSubSurfaceFactor, 0.0F, 0.0F };

		ShaderManager::Inst().SetShaderValue4f(c_SubSurfaceParam, value);
	}


	// 光泽度材质参数
	float gloss = pMatInfo->fGlossiness;

	if (pMat->nMaterialInfo & MATERIAL_GLOSS_ANI)
	{
		float gloss_src = pMat->pFrameGlossiness[m_nCurrentFrame];
		float gloss_dst;

		if (m_nCurrentFrame == frame_num - 1)
		{
			if (m_bLoop)
			{
				gloss_dst = pMat->pFrameGlossiness[0];
			}
			else
			{
				gloss_dst = pMat->pFrameGlossiness[m_nCurrentFrame];
			}
		}
		else
		{
			gloss_dst = pMat->pFrameGlossiness[m_nCurrentFrame + 1];
		}

		gloss = gloss_src + (gloss_dst - gloss_src) * m_fCurrentFrameOffset;
	}

//	bool gamma_correct = g_pRender->GetContext()->GetEnableGammaCorrect();

	// 高光反射材质参数
	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_ANI)
	{
		FmVec4 specular_src = FmVec4(
			pMat->pFrameSpecular[m_nCurrentFrame].x,
			pMat->pFrameSpecular[m_nCurrentFrame].y,
			pMat->pFrameSpecular[m_nCurrentFrame].z, 1.0F);				
		FmVec4 specular_dst;

		if (m_nCurrentFrame == frame_num - 1)
		{
			if (m_bLoop)
			{
				specular_dst = FmVec4(pMat->pFrameSpecular[0].x,
					pMat->pFrameSpecular[0].y, pMat->pFrameSpecular[0].z, 1.0F);
			}
			else
			{
				specular_dst = FmVec4(
					pMat->pFrameSpecular[m_nCurrentFrame].x,
					pMat->pFrameSpecular[m_nCurrentFrame].y,
					pMat->pFrameSpecular[m_nCurrentFrame].z, 1.0F);
			}
		}
		else
		{
			specular_dst = FmVec4(
				pMat->pFrameSpecular[m_nCurrentFrame + 1].x,
				pMat->pFrameSpecular[m_nCurrentFrame + 1].y,
				pMat->pFrameSpecular[m_nCurrentFrame + 1].z, 1.0F);
		}

		specular_src += (specular_dst - specular_src) * m_fCurrentFrameOffset;
		specular_src.w = gloss;

//		if (gamma_correct)
		{
		//	specular_src = VisUtil_GetGammaColor4(specular_src.x,
		//		specular_src.y, specular_src.z, specular_src.w);
		}

		//VisUtil_SetShaderConst(pConst, CB_MATERIAL_SPECULAR, &specular_src, 1);
		ShaderManager::Inst().SetShaderValue3f(c_MaterialSpecular, specular_src.x, specular_src.y, specular_src.z);
		ShaderManager::Inst().SetShaderValue1f(c_fSpecularPower, specular_src.w);
		//c_fSpecularPower
	}
	//else if (pMatInfo->bSpecularEnable)
	else
	{
		FmVec4 value(pMatInfo->vSpecular.x, pMatInfo->vSpecular.y, 
			pMatInfo->vSpecular.z, gloss);

//		if (gamma_correct)
		{
		//	value = VisUtil_GetGammaColor4(value.x, value.y, value.z, value.w);
		}
		ShaderManager::Inst().SetShaderValue4f(c_MaterialSpecular, value);
		ShaderManager::Inst().SetShaderValue1f(c_fSpecularPower, value.w);
		//VisUtil_SetShaderConst(pConst, CB_MATERIAL_SPECULAR, &value, 1);
	}

	// 漫反射材质参数
	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_ANI)
	{
		FmVec4 diffuse_src = FmVec4(
			pMat->pFrameDiffuse[m_nCurrentFrame].x,
			pMat->pFrameDiffuse[m_nCurrentFrame].y,
			pMat->pFrameDiffuse[m_nCurrentFrame].z, 1.0F);				
		FmVec4 diffuse_dst;

		if (m_nCurrentFrame == frame_num - 1)
		{
			if (m_bLoop)
			{
				diffuse_dst = FmVec4(pMat->pFrameDiffuse[0].x,
					pMat->pFrameDiffuse[0].y, pMat->pFrameDiffuse[0].z, 1.0F);
			}
			else
			{
				diffuse_dst = FmVec4(
					pMat->pFrameDiffuse[m_nCurrentFrame].x,
					pMat->pFrameDiffuse[m_nCurrentFrame].y,
					pMat->pFrameDiffuse[m_nCurrentFrame].z, 1.0F);
			}
		}
		else
		{
			diffuse_dst = FmVec4(
				pMat->pFrameDiffuse[m_nCurrentFrame + 1].x,
				pMat->pFrameDiffuse[m_nCurrentFrame + 1].y,
				pMat->pFrameDiffuse[m_nCurrentFrame + 1].z, 1.0F);
		}

		diffuse_src += (diffuse_dst - diffuse_src) * m_fCurrentFrameOffset;
		ShaderManager::Inst().SetShaderValue4f(c_MaterialDiffuse, diffuse_src);
	}
	else
	{
		FmVec4 value(pMatInfo->vDiffuse.x, pMatInfo->vDiffuse.y, pMatInfo->vDiffuse.z, 1.0F);
		ShaderManager::Inst().SetShaderValue4f(c_MaterialDiffuse, value);
	}

	FmVec4 append_color = VisUtil_ColorToVector4(m_nColor);
	if (pMatInfo->bOpacityEnable)
	{
		float model_opacity = GetCurrentOpacity(pMat, pNode, frame_num, 
			m_fCurrentFrameOffset);

		// 模型附加色透明通道与材质透明信息叠加，否则附加色的透明通道信息将丢失
		append_color.w *= model_opacity;
	}

	ShaderManager::Inst().SetShaderValue4f(c_AppendColor, append_color);

	// 环境反射材质参数
	if (pMat->nMaterialInfo & MATERIAL_AMBIENT_ANI)
	{
		FmVec4 ambient_src = FmVec4(
			pMat->pFrameAmbient[m_nCurrentFrame].x,
			pMat->pFrameAmbient[m_nCurrentFrame].y,
			pMat->pFrameAmbient[m_nCurrentFrame].z, 1.0F);				
		FmVec4 ambient_dst;

		if (m_nCurrentFrame == frame_num - 1)
		{
			if (m_bLoop)
			{
				ambient_dst = FmVec4(pMat->pFrameAmbient[0].x,
					pMat->pFrameAmbient[0].y, pMat->pFrameAmbient[0].z, 1.0F);
			}
			else
			{
				ambient_dst = FmVec4(
					pMat->pFrameAmbient[m_nCurrentFrame].x,
					pMat->pFrameAmbient[m_nCurrentFrame].y,
					pMat->pFrameAmbient[m_nCurrentFrame].z, 1.0F);
			}
		}
		else
		{
			ambient_dst = FmVec4(
				pMat->pFrameAmbient[m_nCurrentFrame + 1].x,
				pMat->pFrameAmbient[m_nCurrentFrame + 1].y,
				pMat->pFrameAmbient[m_nCurrentFrame + 1].z, 1.0F);
		}

		ambient_src += (ambient_dst - ambient_src) * m_fCurrentFrameOffset;

		ShaderManager::Inst().SetShaderValue4f(c_MaterialAmbient, ambient_src);

		FmVec4 valueex(pMatInfo->vAmbientEx.x, pMatInfo->vAmbientEx.y, 
			pMatInfo->vAmbientEx.z, 1.0F);

		ShaderManager::Inst().SetShaderValue4f(c_MaterialAmbientEx, valueex);
	}
	else
	{
		FmVec4 value(pMatInfo->vAmbient.x, pMatInfo->vAmbient.y, 
			pMatInfo->vAmbient.z, 1.0F);

		ShaderManager::Inst().SetShaderValue4f(c_MaterialAmbient, value);

		FmVec4 valueex(pMatInfo->vAmbientEx.x, pMatInfo->vAmbientEx.y, 
			pMatInfo->vAmbientEx.z, 1.0F);

		ShaderManager::Inst().SetShaderValue4f(c_MaterialAmbientEx, valueex);
	}

	//==========================================================================
	//scene相关
	//==========================================================================
	const FmVec3& camera_pos = camera.vPosition;

	FmVec4 light_dir = pContext->GetVector4(IRenderContext::V_LIGHT_DIRECTION);
	FmMat4 mtxViewInverse;
	FmMat4Inverse(&mtxViewInverse, NULL, &camera.mtxViewRelative);
	

	// 摄像机位置
	/*
	FmVec4 view_pos(camera_pos.x - refer_pos.x, 
		camera_pos.y - refer_pos.y, camera_pos.z - refer_pos.z, 1.0F);
		*/
	FmVec4 view_pos(camera_pos.x - refer_pos.x, camera_pos.y - refer_pos.y, camera_pos.z - refer_pos.z, 1.0f);

	
	FmVec4 clip_plane = pContext->GetVector4(IRenderContext::V_CLIP_PLANE);
	
	
	
	//fog
	FmVec4 fog_color = pContext->GetVector4(IRenderContext::V_FOG_CURRENT);
	float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
	float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END);
	float fog_height = pContext->GetFloat(IRenderContext::F_FOG_HEIGHT);
	float fog_density = pContext->GetFloat(IRenderContext::F_FOG_DENSITY);
	FmVec4 fog_param(fog_start, 1.0F / (fog_end - fog_start), fog_height, 0.0F);
	FmVec4 fog_exp_param = pContext->GetVector4(IRenderContext::V_FOG_EXP_CURRENT);	

	float heightfog_start = pContext->GetFloat(IRenderContext::F_HEIGHT_FOG_START);
	float heightfog_end = pContext->GetFloat(IRenderContext::F_HEIGHT_FOG_END);
	FmVec4 height_fog_param = FmVec4(heightfog_start, 1.0f / (heightfog_end - heightfog_start), 0.0f, 0.0f);

	FmVec4 height_fog_color = pContext->GetVector4(IRenderContext::V_HEIGHTFOG_CURRENT);
	
	ShaderManager::Inst().SetShaderValue3f(c_vLightDir, light_dir.x, light_dir.y, light_dir.z);
	ShaderManager::Inst().SetShaderValueMat4(c_mtxViewInverse, mtxViewInverse);

	//SetShaderValue4f(pShader, "c_LightAmbient", 0.5,0.5,0.5,0.5);
	ShaderManager::Inst().SetShaderValue4f(c_vViewPos, view_pos);
	ShaderManager::Inst().SetShaderValue4f(c_vReferPos, refer_pos.x, refer_pos.y, refer_pos.z, 0.0);
	ShaderManager::Inst().SetShaderValue4f(c_ClipPlane, clip_plane.x, clip_plane.y, clip_plane.z, clip_plane.w);
	ShaderManager::Inst().SetShaderValue4f(c_FogColor, fog_color);
	ShaderManager::Inst().SetShaderValue4f(c_FogParam, fog_param);
	ShaderManager::Inst().SetShaderValue4f(c_FogExpParam, fog_exp_param);
	ShaderManager::Inst().SetShaderValue4f(c_HeightFogParam, height_fog_param);
	ShaderManager::Inst().SetShaderValue4f(c_HeightFogColor, height_fog_color); 

	//点光源
	// 是否开启静态光照下的人物点光源照明
	bool point_light =
		 (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_LIGHT) != 0)
		&& (pNode->nType == FXNODE_TYPE_SKIN);

	if(point_light)
	{
		unsigned int role_light_color = pContext->GetColor(
			IRenderContext::C_ROLE_LIGHT_COLOR);

		float role_light_inten = pContext->GetFloat(
				IRenderContext::F_ROLE_LIGHT_INTENSITY);

		FmVec3 role_light_pos = pContext->GetVector(
				IRenderContext::V_ROLE_LIGHT_POSITION);

		FmVec4 light_color = VisUtil_GetColor2(role_light_color, 
					role_light_inten);

		float role_light_range = pContext->GetFloat(
			IRenderContext::F_ROLE_LIGHT_RANGE);

		float light_position[4] = { 
			role_light_pos.x - camera.vPosition.x, 
			role_light_pos.y - camera.vPosition.y, 
			role_light_pos.z - camera.vPosition.z, 
			role_light_range };

	

		ShaderManager::Inst().SetShaderValue4f(c_PointLightDiffuse, 
			light_color);
		ShaderManager::Inst().SetShaderValue4f(c_PointLightPosition, 
			light_position);
	}

	//设置遮罩相关

	if (info->pMat->nMaterialInfoEx && MATERIAL_FILTER_MAP_INFO && info->pMatInfo->bFilterEnable)
	{
		filter_param filter_param = GetCurrentFilterParam(info, frame_num, m_fCurrentFrameOffset);

		ShaderManager::Inst().SetShaderValue4f(c_FilterParam, (float*)&filter_param);
	//	pBatch->SetPixelShaderConstantF(CP_FILTERPARAM, (float*)(&filter_param), 1);
		
		ITexture* pTexFilterMap = pMatInfo->FilterMap.pTex;

		if (pTexFilterMap)
		{
			SetTexture(tex_FilterMap, pTexFilterMap->GetCanUseShaderTex()->GetTexture());
		}

		// 遮罩UV动画 
		if (pMat->nMaterialInfoEx & MATERIAL_FILTER_UV_ANI)
		{
			FmMat4 mtxTexTM;

			CreateFilterTextureMatrix(pMat, &mtxTexTM);
			ShaderManager::Inst().SetShaderValueMat4(c_FilterMatrix, (float*)&mtxTexTM);
		//	pBatch->SetVertexShaderConstantF(CV_FILTER_MATRIX, 
		//		(FLOAT*)&mtxTexTM, 4);
		}
	}
	
	if(m_bKeepOut)
	{
		SetTexture(tex_KeepoutMap, m_KeepOutTexture->GetCanUseShaderTex()->GetTexture());
	}
	m_ShaderUserConstValue.UploadGPU();
}

#ifdef CPU_COMPUTE_BONE
void CModelPlayer::CpuComputeBone(node_material_t* pMat, const FmVec4* bone_matrices)
{
	if(NULL == pMat->pWeightTable)
		return;
	
	if(pMat->SingleVB.pDynamicVertices == 0)
	{
		pMat->SingleVB.pDynamicVertices = (unsigned char*)malloc(pMat->SingleVB.nStride* pMat->SingleVB.nCount);
	}
	memcpy(pMat->SingleVB.pDynamicVertices, pMat->SingleVB.pVertices, pMat->SingleVB.nCount* pMat->SingleVB.nStride);
	char* p = (char*)pMat->SingleVB.pDynamicVertices;
	for(int i = 0; i < pMat->SingleVB.nCount; i++)
	{
		FmVec3 pos;
		GetVertexPosition(pMat->nIndexInNode, i, pos);
		FmVec3* _pos = (FmVec3*)p;
		(*_pos) = pos;
		p += pMat->SingleVB.nStride;
	}
	g_pRender->UpdateDynamicVB((IDynamicVB*)pMat->pSingleGPUVB, 0, pMat->SingleVB.pDynamicVertices, pMat->SingleVB.nCount* pMat->SingleVB.nStride);
}
#endif