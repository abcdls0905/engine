//--------------------------------------------------------------------
// 文件名:		model_player_trace.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "action_player.h"
#include "res_model.h"
#include "oct_tree.h"
#include "math_3d.h"
#include "../visual/i_context.h"
#include "../visual/i_render.h"
#include "../visual/vis_utils.h"

extern IRender* g_pRender;

static bool	s_bTraceAlpha = false;
static bool	s_bTraceHitted = false;

// 获得三角形的顶点坐标
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

// 获得三角形的顶点坐标
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

// 获得三角形的贴图坐标
bool get_triangle_tex_uv(model_node_t* pNode, node_material_t* pMat, 
	unsigned int tri_id, FmVec2* uv0, FmVec2* uv1, FmVec2* uv2)
{
	//unsigned char* vb = (unsigned char*)pMat->pSingleVB;
	vertex_data_t* vb = &(pMat->SingleVB);
	unsigned short* ib = pMat->pLODIB[pMat->nCurLODLevel];
	unsigned int index0 = ib[tri_id + 0];
	unsigned int index1 = ib[tri_id + 1];
	unsigned int index2 = ib[tri_id + 2];
	unsigned int offset = 0;
	unsigned int single_vertex_size = pMat->nSingleVertexSize;
	unsigned int vertex_count = pMat->nVertexCount;

	Assert(index0 < vertex_count);
	Assert(index1 < vertex_count);
	Assert(index2 < vertex_count);

	if (pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		offset += 12;
	}

	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		offset += 12;
	}

	if (pNode->nModelInfo & MODEL_COLOR_INFO)
	{
		offset += 4;
	}

	if (pNode->nModelInfo & MODEL_ILLUM_INFO)
	{
		offset += 4;
	}

	if (pNode->nModelInfo & MODEL_TANGENT_INFO)
	{
		offset += 12;
	}

	if (pNode->nModelInfo & MODEL_BINORMAL_INFO)
	{
		offset += 12;
	}

	Assert(offset < single_vertex_size);

	//*uv0 = *(FmVec2*)&vb[index0 * single_vertex_size + offset];
	//*uv1 = *(FmVec2*)&vb[index1 * single_vertex_size + offset];
	//*uv2 = *(FmVec2*)&vb[index2 * single_vertex_size + offset];
	*uv0 = *(FmVec2*)(get_vertex_by_index(vb, index0) + offset);
	*uv1 = *(FmVec2*)(get_vertex_by_index(vb, index1) + offset);
	*uv2 = *(FmVec2*)(get_vertex_by_index(vb, index2) + offset);

	return true;
}

// 判断三角形是否需要进行碰撞
inline static bool triangle_can_trace(const FmVec3* v0, 
	const FmVec3* v1, const FmVec3* v2, const FmVec3* vStart)
{
	if (!g_pRender->GetTraceOnlyFront())
	{
		return true;
	}

	// 三角形对应的平面
	FmPlane plane;

	FmPlaneFromPoints(&plane, v0, v1, v2);

	// 射线起点到面的距离
	float dist = FmPlaneDotCoord(&plane, vStart);

	if (dist < 0.0F)
	{
		// 不对反面进行碰撞
		return false;
	}

	return true;
}

// 判断三角形是否需要可拣选
inline static bool triangle_can_cull(const FmVec3* v0,
	const FmVec3* v1, const FmVec3* v2, const FmPlane* planes,
	size_t plane_num, const FmVec3* vCenter, float radius)
{
	if (plane_num > 0)
	{
		// 使用剪裁面
		FmVec3 bound_min = *v0;
		FmVec3 bound_max = *v0;

		FmVec3Minimize(&bound_min, &bound_min, v1);
		FmVec3Minimize(&bound_min, &bound_min, v2);
		FmVec3Maximize(&bound_max, &bound_max, v1);
		FmVec3Maximize(&bound_max, &bound_max, v2);

		FmVec3 tri_center = (bound_min + bound_max) * 0.5F;
		float sx = (bound_max.x - bound_min.x) * 0.5F;
		float sy = (bound_max.y - bound_min.y) * 0.5F;
		float sz = (bound_max.z - bound_min.z) * 0.5F;
		float radius_sq = sx * sx + sy * sy + sz * sz;
		bool bIn = true;

		for (size_t p = 0; p < plane_num; p++)
		{
			float dist = FmPlaneDotCoord(&planes[p], &tri_center);

			if (dist < 0.0F)
			{
				continue;
			}

			if ((dist * dist) > radius_sq)
			{
				bIn = false;
				break;
			}
		}

		if (!bIn)
		{
			return false;
		}
	}
	else
	{
		// 只用球体做碰撞检测
		if (!math_sphere_triangle_aabb_collision(*vCenter, radius, *v0, *v1, 
			*v2))
		{
			return false;
		}
	}

	return true;
}

// 设置当前使用的世界矩阵
void CModelPlayer::SetCurrentMatrix(model_node_t* pNode)
{
	// 计算当前世界矩阵
	if (pNode->nModelInfo & MODEL_MATRIX_ANI)
	{
		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::SetCurrentMatrix] pModel == NULL model name is %s", m_pResModel->GetName());
			return;
		}

		// 计算帧数
		unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;
		// 计算权重
		float weight = m_fCurrentFrameOffset;
		unsigned int src_frame = m_nCurrentFrame;
		unsigned int dst_frame;

		if (m_nCurrentFrame == (frame_num - 1))
		{
			if (m_bLoop)
			{
				dst_frame = 0;
			}
			else
			{
				dst_frame = m_nCurrentFrame;
			}
		}
		else
		{
			dst_frame = m_nCurrentFrame + 1;
		}

		location_t loc;
		FmMat4 mtxTM;

		// 插值
		math_location_lerp(&loc, &pNode->pLocation[src_frame], 
			&pNode->pLocation[dst_frame], weight);
		math_location_to_matrix(&loc, &mtxTM);
		FmMatrixMultiply(&m_mtxCurrentTM, &mtxTM, &m_mtxCurrentTM);	
	}
	else if (pNode->nModelInfo & MODEL_MATRIX_INFO)
	{
		FmMat4 mtxTM;

		math_location_to_matrix(&pNode->pLocation[0], &mtxTM);
		FmMatrixMultiply(&m_mtxCurrentTM, &mtxTM, &m_mtxCurrentTM);
	}
	else
	{
		FmMatrixIdentity(&m_mtxCurrentTM);
	}
}

// 设置碰撞信息
bool CModelPlayer::SetTraceInfo(trace_info_t* result, 
	model_node_t* pNode, node_material_t* pMat, int tri_id, float u, float v, 
	float t, const FmVec3& p0, const FmVec3& p1, 
	const FmVec3& p2)
{
	if (result->nHitMax > 1)
	{
		if (result->nHitCount >= result->nHitMax)
		{
			return false;
		}

		s_bTraceHitted = true;

		if (t < result->fDistance)
		{
			result->fDistance = t;
		}

		trace_info_t::hit_t* pHit = &result->Hits[result->nHitCount];

		pHit->fDistance = t;
		pHit->fTraceU = u;
		pHit->fTraceV = v;
		pHit->TraceID = PERSISTID();
		pHit->strMaterialName = pMat->pszName;
		pHit->nMaterialIndex = pMat->nIndexInModel;

		if (g_pRender->GetTraceTextureName())
		{
			pHit->strTexName = pMat->MatInfo.DiffuseMap.pFileName;

			ITexture* pTex = pMat->MatInfo.DiffuseMap.pTex;

			if (pTex)
			{
				get_triangle_tex_uv(pNode, pMat, tri_id, &pHit->TexUV[0], 
					&pHit->TexUV[1], &pHit->TexUV[2]);
			}
		}

		FmVec3TransformCoord(&pHit->Vertex[0], &p0, &m_mtxCurrentTM);
		FmVec3TransformCoord(&pHit->Vertex[1], &p1, &m_mtxCurrentTM);
		FmVec3TransformCoord(&pHit->Vertex[2], &p2, &m_mtxCurrentTM);

		pHit->VertexMark[0] = pHit->Vertex[0];
		pHit->VertexMark[1] = pHit->Vertex[1];
		pHit->VertexMark[2] = pHit->Vertex[2];

		FmMat4 mtxWorldInverse;

		FmMat4Inverse(&mtxWorldInverse, NULL, &m_mtxWorldTM);

		//FmVec3TransformCoordArray(&pHit->Vertex[0], sizeof(FmVec3), 
		//	&pHit->Vertex[0], sizeof(FmVec3), &mtxWorldInverse, 3);

		for(int i = 0; i < 3 ; i++ )
			FmVec3TransformCoord(&pHit->Vertex[i],&pHit->Vertex[i],&mtxWorldInverse);

		if (++result->nHitCount >= result->nHitMax)
		{
			return false;
		}
	}
	else if (t < result->fDistance)
	{
		s_bTraceHitted = true;
		result->fDistance = t;

		trace_info_t::hit_t* pHit = &result->Hits[0];

		pHit->fDistance = t;
		pHit->fTraceU = u;
		pHit->fTraceV = v;
		pHit->TraceID = PERSISTID();
		pHit->strMaterialName = pMat->pszName;
		pHit->nMaterialIndex = pMat->nIndexInModel;

		if (g_pRender->GetTraceTextureName())
		{
			pHit->strTexName = pMat->MatInfo.DiffuseMap.pFileName;

			ITexture* pTex = pMat->MatInfo.DiffuseMap.pTex;

			if (pTex)
			{
				get_triangle_tex_uv(pNode, pMat, tri_id, &pHit->TexUV[0], 
					&pHit->TexUV[1], &pHit->TexUV[2]);
			}
		}

		
		FmVec3TransformCoord(&pHit->Vertex[0], &p0, &m_mtxCurrentTM);
		FmVec3TransformCoord(&pHit->Vertex[1], &p1, &m_mtxCurrentTM);
		FmVec3TransformCoord(&pHit->Vertex[2], &p2, &m_mtxCurrentTM);
		pHit->VertexMark[0] = pHit->Vertex[0];
		pHit->VertexMark[1] = pHit->Vertex[1];
		pHit->VertexMark[2] = pHit->Vertex[2];
		FmMat4 mtxWorldInverse;

		FmMat4Inverse(&mtxWorldInverse, NULL, &m_mtxWorldTM);

		//FmVec3TransformCoordArray(&pHit->Vertex[0], sizeof(FmVec3), 
		//	&pHit->Vertex[0], sizeof(FmVec3), &mtxWorldInverse, 3);

		for(int i = 0; i < 3 ; i++ )
			FmVec3TransformCoord(&pHit->Vertex[i],&pHit->Vertex[i],&mtxWorldInverse);

		result->nHitCount = 1;
	}

	return true;
}

FmVec3* CModelPlayer::AllocCpuSkeletonAnimationVB( 
	node_material_t* pMat)
{
	//Assert(pMat->pSingleVB != NULL);
	Assert(pMat->nVertexCount != 0);
	Assert(pMat->nSingleVertexSize != 0);
	Assert(pMat->pMultiGPUVB == NULL);
	Assert(pMat->pWeightTable != NULL);

	unsigned int vertex_num = pMat->nVertexCount;

	if (vertex_num > 10000)
	{
		return NULL;
	}

	if (NULL == m_pCacheSkins)
	{
		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::AllocCpuSkeletonAnimationVB] pModel == NULL model name is %s", m_pResModel->GetName());
			return NULL;
		}

		unsigned int mat_num = pModel->nMaterialCount;
		m_pCacheSkins = (cache_skin_t*)CORE_ALLOC(
			sizeof(cache_skin_t) * mat_num);
		memset(m_pCacheSkins, 0, sizeof(cache_skin_t) * mat_num);
	}
	
	int sequence_id = m_pActionPlayer->GetSequenceId();
	cache_skin_t* pCacheSkin = &m_pCacheSkins[pMat->nIndexInModel];

	if (NULL == pCacheSkin->pSkinVectors)
	{
		pCacheSkin->pSkinVectors = (FmVec3*)CORE_ALLOC(
			sizeof(FmVec3) * vertex_num);
		pCacheSkin->nSkinVectorNum = vertex_num;
	}
	else if (pCacheSkin->nSkinSequenceId == sequence_id)
	{
		// 动画帧序号还是旧的，表明骨骼位置未发生改变
		return pCacheSkin->pSkinVectors;
	}
	
	skt_pose_t* pose = m_pActionPlayer->GetCurrentPose();
	skt_node_t** node_list = pose->pSkeleton->pBoneNodeList;
	unsigned int total_bone_node_cnt = pose->pSkeleton->nTotalBoneNodeCount;
	//const FmVec3* pOriginPos = (FmVec3*)pMat->pSingleVB;
	vertex_data_t* pVertexData = &(pMat->SingleVB);
	//unsigned int stride = pMat->uSingleVertexSize;
	FmVec3* skin = pCacheSkin->pSkinVectors;
	node_weight_table_t* w = pMat->pWeightTable;
	unsigned int* bone_list = pMat->pBoneList;
	FmVec3 pos0;
	FmVec3 pos1;
	//__declspec(align(16)) FmVec4 sse_pos0;
	//__declspec(align(16)) FmVec4 sse_pos1;

	// 为提高效率，最多只使用两个权值
	for (unsigned int i = 0; i < vertex_num; ++i)
	{
		FmVec3* pOriginPos = (FmVec3*)get_vertex_by_index(
			pVertexData, i);
		int bone0 = w[0].nBoneID;

		if ((bone0 != -1) && (bone_list[bone0] < total_bone_node_cnt))
		{
			FmVec3TransformCoord(&pos0, pOriginPos, 
				&node_list[bone_list[bone0]]->mtxOriginal);
			/*
			FmMat4* pMtx = &node_list[bone_list[bone0]]->mtxOriginal;
			FmVec4* pDst = &sse_pos0;

			__asm
			{
				mov eax, pMtx
				mov edx, pDst
				mov ecx, pOriginPos

				movups xmm4, xmmword ptr [eax]
				movups xmm5, xmmword ptr [eax+0x10]
				movups xmm6, xmmword ptr [eax+0x20]
				movups xmm7, xmmword ptr [eax+0x30]

				// process position
				xorps xmm2, xmm2
				movups xmm0, xmmword ptr [ecx]

				// row 1
				movaps xmm1, xmm0
				shufps xmm1, xmm1, 0x00
				mulps xmm1, xmm4
				addps xmm2, xmm1

				// row 2
				movaps xmm1, xmm0
				shufps xmm1, xmm1, 0x55
				mulps xmm1, xmm5
				addps xmm2, xmm1

				// row 3
				movaps xmm1, xmm0
				shufps xmm1, xmm1, 0xAA
				mulps xmm1, xmm6
				addps xmm2, xmm1

				// assume w is 1
				addps xmm2, xmm7

				// store
				movaps xmmword ptr [edx], xmm2
			}
*/
			int bone1 = w[1].nBoneID;

			if ((bone1 != -1) && (bone_list[bone1] < total_bone_node_cnt))
			{
				FmVec3TransformCoord(&pos1, pOriginPos, 
					&node_list[bone_list[bone1]]->mtxOriginal);
				/*
				pMtx = &node_list[bone_list[bone1]]->mtxOriginal;
				pDst = &sse_pos1;

				__asm
				{
					mov eax, pMtx
					mov edx, pDst
					mov ecx, pOriginPos

					movups xmm4, xmmword ptr [eax]
					movups xmm5, xmmword ptr [eax+0x10]
					movups xmm6, xmmword ptr [eax+0x20]
					movups xmm7, xmmword ptr [eax+0x30]

					// process position
					xorps xmm2, xmm2
					movups xmm0, xmmword ptr [ecx]

					// row 1
					movaps xmm1, xmm0
					shufps xmm1, xmm1, 0x00
					mulps xmm1, xmm4
					addps xmm2, xmm1

					// row 2
					movaps xmm1, xmm0
					shufps xmm1, xmm1, 0x55
					mulps xmm1, xmm5
					addps xmm2, xmm1

					// row 3
					movaps xmm1, xmm0
					shufps xmm1, xmm1, 0xAA
					mulps xmm1, xmm6
					addps xmm2, xmm1

					// assume w is 1
					addps xmm2, xmm7

					// store
					movaps xmmword ptr [edx], xmm2
				}*/

				float weight0 = w[0].fWeight;

			//	skin[i] = *(FmVec3*)&sse_pos0 * weight0 
			//		+ *(FmVec3*)&sse_pos1 * (1.0F - weight0);
				skin[i] = pos0 * weight0 + pos1 * (1.0F - weight0);
			}
			else
			{
				//skin[i] = *(FmVec3*)&sse_pos0;
				skin[i] = pos0;
			}
		}
		else
		{
			skin[i] = *pOriginPos;
		}
		
		w += 4;
		//pOriginPos = (const FmVec3*)((BYTE*)pOriginPos + stride);
	}

	// 更新动画帧序号
	pCacheSkin->nSkinSequenceId = sequence_id;

	return skin;
}

// 将射线变换到子坐标系中
static void transform_ray_to_child_coordinate(FmVec3* dst_start, 
	FmVec3* dst_direction, const FmVec3* src_start, 
	const FmVec3* src_direction, const FmMat4* mtxChild)
{
	FmMat4 mtxChildInverse;

	FmMat4Inverse(&mtxChildInverse, NULL, mtxChild);
	FmVec3TransformCoord(dst_start, src_start, &mtxChildInverse);
	FmVec3TransformNormal(dst_direction, src_direction, &mtxChildInverse);
}

// 将球变换到子坐标系中
static void transform_sphere_to_child_coordinate(FmVec3* dst_center, 
	float* dst_radius, const FmVec3* src_center, float src_radius, 
	const FmMat4* mtxChild)
{
	FmMat4 mtxChildInverse;

	FmMat4Inverse(&mtxChildInverse, NULL, mtxChild);

	FmVec3 temp = *src_center;

	temp.x += src_radius;

	FmVec3TransformCoord(dst_center, src_center, &mtxChildInverse);
	FmVec3TransformCoord(&temp, &temp, &mtxChildInverse);

	temp = temp - *dst_center;

	*dst_radius = VisUtil_Vec3Length(&temp);
}

// 将三角形变换到子坐标系中
static void transform_triangle_to_child_coordinate(FmVec3* vDest0, FmVec3* vDest1, FmVec3* vDest2,
                                                   const FmVec3* vSrc0, const FmVec3* vSrc1, const FmVec3* vSrc2,
                                                   const FmMat4* mtxChild)
{
    FmMat4 mtxChildInverse;

    FmMat4Inverse(&mtxChildInverse, NULL, mtxChild);
    FmVec3TransformCoord(vDest0, vSrc0, &mtxChildInverse);
    FmVec3TransformCoord(vDest1, vSrc1, &mtxChildInverse);
    FmVec3TransformCoord(vDest2, vSrc2, &mtxChildInverse);
}

bool get_texture_alpha(ITexture* pTex, float u, float v, 
	unsigned int* alpha)
{
	Assert(pTex != NULL);
	Assert(alpha != NULL);

	*alpha = g_pRender->GetTextureAlpha(pTex, u, v);

	return true;
}

// 是否是水模型材质
static inline bool get_water_model(node_material_t* pMat)
{
	return pMat->MatInfo.bSceneFog && (!pMat->MatInfo.bNoZWrite);
}

// 对顶点动画节点进行射线追踪
void CModelPlayer::NodeRayTrace(model_t* pModel, model_node_t* pNode, 
	const FmVec3* start, const FmVec3* direction, 
	trace_info_t* result)
{
	Assert(pModel != NULL);
	Assert(pNode != NULL);
	Assert(start != NULL);
	Assert(direction != NULL);

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	if (pNode->nType == FXNODE_TYPE_HELPER)
	{
		if (0 == pNode->nChildNodeCount)
		{
			return;
		}
		NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
		return;
	}

	if (pNode->nModelInfo & (MODEL_BB_FLAG | MODEL_BBX_FLAG | MODEL_BBY_FLAG))
	{
		CreateBillboard(pNode);
	}

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);

	// 屏蔽树叶
	//if (!g_bTraceTreeLeaf && (node->nModelInfo & MODEL_TREELEAF_FLAG))
	if ((pNode->nModelInfo & MODEL_TREELEAF_FLAG) 
		&& (!g_pRender->GetTraceTreeLeaf()))
	{
		NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
		return;
	}

	if (pNode->bHide || pNode->bWaterClip)
	{
		NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
		return;
	}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
//		if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		//if (!g_pRender->GetTraceHideModel())
        if (!m_bTraceMainModel)
		{
            // 跳过主模节点
			NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
			return;
		}
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
			NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
			return;
		}
	}

	if (pNode->nMaterialCount <= 0)
	{
		NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
		return;
	}

	for (unsigned int m = 0; m < pNode->nMaterialCount; ++m)
	{
		node_material_t* pMat = &pNode->Materials[m];

		/*
		// 是否仅Trace水模型
		if (g_pRender->GetTraceWaterModel())
		{
			if (!get_water_model(pMat))
			{
				goto DO_CHILD;
			}
		}
		else if (get_water_model(pMat))
		{
			// 水模型默认不Trace
			goto DO_CHILD;
		}

		// 是否对具有禁止站立属性标签的模型进行光线追踪
		if (!g_pRender->GetTraceFobidStandModel())
		{
			if (pMat->MatInfo.bForbidStandModel)
			{
				goto DO_CHILD;
			}
		}
		*/
	}

	// 如果该结点存在分割树，那么使用分割树进行更快的碰撞检测
	if (pNode->pOctTree)
	{
		FmVec3 vRayStart;
		FmVec3 vRayDirection;
		
		transform_ray_to_child_coordinate(&vRayStart, &vRayDirection, 
			start, direction, &m_mtxCurrentTM);

		if (!FmBoxBoundProbe(&pNode->vBoundMin, &pNode->vBoundMax, &vRayStart, 
			&vRayDirection))
		{
			NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
			return;
		}

		bool only_front = g_pRender->GetTraceOnlyFront();
		int alpha_ref;

		if (s_bTraceAlpha)
		{
			alpha_ref = g_pRender->GetTraceAlphaRef();
		}
		else
		{
			alpha_ref = 0;
		}

		if (NULL == result)
		{
			if (pNode->pOctTree->TraceHitted(this, vRayStart, vRayDirection, 
				alpha_ref, only_front))
			{
				s_bTraceHitted = true;
			}
		}
		else
		{
			if (pNode->pOctTree->TraceDetail(this, vRayStart, vRayDirection,
				alpha_ref, only_front, *result))
			{
				s_bTraceHitted = true;
			}
		}
	}
	else
	{
		int alpha_ref;

		if (s_bTraceAlpha)
		{
			alpha_ref = g_pRender->GetTraceAlphaRef();
		}
		else
		{
			alpha_ref = 0;
		}

		// 如果该结点不存在分割树，那么将遍历所有三角面来进行碰撞检测
		FmVec3 ray_start, ray_direction;

		transform_ray_to_child_coordinate(&ray_start, &ray_direction, 
			start, direction, &m_mtxCurrentTM);

		if (!FmBoxBoundProbe(&pNode->vBoundMin, &pNode->vBoundMax, &ray_start, 
			&ray_direction))
		{
			NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
			return;
		}

		// 将射线转换到三角形的空间
		transform_ray_to_child_coordinate(&ray_start, &ray_direction, 
			start, direction, &m_mtxCurrentTM);

		// 再进行三角面的碰撞检测，遍历当前结点各个材质所拥有的顶点数组
		for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
		{
			node_material_t* pMat = &pNode->Materials[i];

			Assert(pMat != NULL);

			// 是否仅Trace水模型
			//if (g_pRender->GetTraceWaterModel())
			//{
			//	if (!get_water_model(pMat))
			//	{
			//		continue;
			//	}
			//}
			//else if (get_water_model(pMat))
			//{
			//	// 水模型默认不Trace
			//	continue;
			//}

			// 是否对具有禁止站立属性标签的模型进行光线追踪
			//if (!g_pRender->GetTraceFobidStandModel())
			//{
			//	if (pMat->MatInfo.bForbidStandModel)
			//	{
			//		continue;
			//	}
			//}

			if (pNode->nModelInfo & MODEL_POSITION_INFO)
			{
				FmVec3 p0, p1, p2;
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
				
				for (unsigned int face = 0; face < index_num; face += 3)
				{
					//get_triangle_vertex(pMat, pMat->pSingleVB, 
					//	pMat->uSingleVertexSize, face, &p0, &p1, &p2);
					get_single_vertex(pMat, face, &p0, &p1, &p2);

					float u, v, d;

					if (VisUtil_IntersectTri(&p0, &p1, &p2,	&ray_start, 
						&ray_direction, &u, &v, &d))
					{
						if ((d < 0.0F) || (d > 1.0F))
						{
							continue;
						}

						if (!triangle_can_trace(&p0, &p1, &p2, &ray_start))
						{
							continue;
						}

						if (s_bTraceAlpha && pMat->MatInfo.bAlphaTest
							&& ((NULL == result) || (result->nHitMax > 1) 
							|| (d < result->fDistance)))
						{
							ITexture* pTex = pMat->MatInfo.DiffuseMap.pTex;

							if (pTex)
							{
								FmVec2 uv0;
								FmVec2 uv1;
								FmVec2 uv2;

								get_triangle_tex_uv(pNode, pMat, face, 
									&uv0, &uv1, &uv2);

								FmVec2 uv;

								uv = uv0 * (1.0F - u - v) + uv1 * u + uv2 * v;

								unsigned int alpha = 0;

								get_texture_alpha(pTex, uv.x, uv.y, &alpha);

								if ((int)alpha < alpha_ref)
								{
									continue;
								}
							}
						}

						if (NULL == result)
						{
							s_bTraceHitted = true;
							m_mtxCurrentTM = mtxOld;
							return;
						}

						if (!SetTraceInfo(result, pNode, pMat, face, u, 
							v, d, p0, p1, p2))
						{
							m_mtxCurrentTM = mtxOld;
							return;
						}
					}
				}
			}
			else if (pNode->nModelInfo & MODEL_POSITION_ANI)
			{
				//float* pVB = pMat->pMultiVB 
				//	+ (pMat->uVertexCnt * pMat->uMultiVertexSize 
				//	* ref->uCurrentFrame) / sizeof(float);
				float* pVB = pMat->pMultiVBs[m_nCurrentFrame]; 
				FmVec3 p0, p1, p2;
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];

				for (unsigned int face = 0; face < index_num; face += 3)
				{
					get_triangle_vertex(pMat, pVB, pMat->nMultiVertexSize, 
						face, &p0, &p1, &p2);

					float u, v, d;

					if (VisUtil_IntersectTri(&p0, &p1, &p2,	&ray_start, 
						&ray_direction,	&u, &v, &d))
					{
						if ((d < 0.0F) || (d > 1.0F))
						{
							continue;
						}

						if (!triangle_can_trace(&p0, &p1, &p2, &ray_start))
						{
							continue;
						}

						if (s_bTraceAlpha && pMat->MatInfo.bAlphaTest
							&& ((NULL == result) || (result->nHitMax > 1) 
							|| (d < result->fDistance)))
						{
							ITexture* pTex = pMat->MatInfo.DiffuseMap.pTex;

							if (pTex)
							{
								FmVec2 uv0;
								FmVec2 uv1;
								FmVec2 uv2;

								get_triangle_tex_uv(pNode, pMat, face, 
									&uv0, &uv1, &uv2);

								FmVec2 uv;

								uv = uv0 * (1.0F - u - v) + uv1 * u + uv2 * v;

								unsigned int alpha = 0;

								get_texture_alpha(pTex, uv.x, uv.y, &alpha);

								if ((int)alpha < alpha_ref)
								{
									continue;
								}
							}
						}

						if (NULL == result)
						{
							s_bTraceHitted = true;
							m_mtxCurrentTM = mtxOld;
							return;
						}

						if (!SetTraceInfo(result, pNode, pMat, face, 
							u, v, d, p0, p1, p2))
						{
							m_mtxCurrentTM = mtxOld;
							return;
						}
					}
				}
			}
		}
	}

	NodeRayTraceFunc_DoChild(pModel, pNode, start, direction, mtxOld, result);
	
/*
DO_CHILD:
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		model_node_t* child = &pNode->ChildNodes[i];

		Assert(child != NULL);

		if (child->nType == FXNODE_TYPE_MESH)
		{
			NodeRayTrace(pModel, child, start, direction, result);
		}
	}

	m_mtxCurrentTM = mtxOld;*/
}

bool CModelPlayer::NodeSphereTrace(model_t* pModel, model_node_t* pNode, 
	const FmVec3* position, float radius)
{
	Assert(pModel != NULL);
	Assert(pNode != NULL);
	Assert(position != NULL);

	bool result = false;

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	if (pNode->nModelInfo & (MODEL_BB_FLAG | MODEL_BBX_FLAG | MODEL_BBY_FLAG))
	{
		CreateBillboard(pNode);
	}

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);
	
	// 屏蔽树叶
	//if ((!g_bTraceTreeLeaf) && (node->nModelInfo & MODEL_TREELEAF_FLAG))
	if ((pNode->nModelInfo & MODEL_TREELEAF_FLAG) 
		&& (!g_pRender->GetTraceTreeLeaf()))
	{
		NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
		return result;
	}

	if (pNode->bHide || pNode->bWaterClip)
	{
		NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
		return result;
	}

	//if (ref->bTraceMainModel 
	//	&& (node->uModelInfo & MODEL_MAINMODEL_FLAG) == 0 
	//	&& (node->type == FXNODE_TYPE_SKIN))
	//{
	//	// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
	//	goto DO_CHILD;
	//}
	//else if (!g_pRender->GetTraceHideModel() 
	//	&& (node->uModelInfo & MODEL_MAINMODEL_FLAG))
	//{
	//	goto DO_CHILD;
	//}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		/*
		if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		//if (!g_pRender->GetTraceHideModel())
		{
			goto DO_CHILD;
		}
		*/
        if (!m_bTraceMainModel)
        {
            // 跳过主模节点
            NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
            return result;
        }
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
			NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
			return result;
		}
	}

	for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
	{
		node_material_t* pMat = &pNode->Materials[i];

		Assert(pMat != NULL);

		/*
		// 是否仅Trace水模型
		if (g_pRender->GetTraceWaterModel())
		{
			if (!get_water_model(pMat))
			{
				goto DO_CHILD;
			}
		}
		else if (get_water_model(pMat))
		{
			// 水模型默认不Trace
			goto DO_CHILD;
		}

		// 是否对具有禁止站立属性标签的模型进行光线追踪
		if (!g_pRender->GetTraceFobidStandModel())
		{
			if (pMat->MatInfo.bForbidStandModel)
			{
				goto DO_CHILD;
			}
		}
		*/
	}

	// 如果该结点存在分割树，那么使用分割树进行更快的碰撞检测
	//if (node->pSplitTree != NULL)
	if (pNode->pOctTree)
	{
		FmVec3 vSphereCenter;
		float fSphereRadius;
		
		// 将球心变换到三角面的空间
		transform_sphere_to_child_coordinate(&vSphereCenter, 
			&fSphereRadius, position, radius, &m_mtxCurrentTM);

		if (!math_intersect_bb_and_sphere(&pNode->vBoundMin, &pNode->vBoundMax, 
			&vSphereCenter, fSphereRadius))
		{
			NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
			return result;
		}

		if (pNode->pOctTree->TraceSphere(this, vSphereCenter, fSphereRadius))
		{
			result = true;
		}
	}
	else
	{
		// 如果该结点不存在分割树，那么将遍历所有三角面来进行碰撞检测
		FmVec3 vSphereCenter;
		float fSphereRadius;

		// 将球心变换到三角面的空间
		transform_sphere_to_child_coordinate(&vSphereCenter, 
			&fSphereRadius, position, radius, &m_mtxCurrentTM);

		if (!math_intersect_bb_and_sphere(&pNode->vBoundMin, &pNode->vBoundMax, 
			&vSphereCenter, fSphereRadius))
		{
			NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
			return result;
		}

		// 再进行三角面的碰撞检测，遍历当前结点各个材质所拥有的顶点数组
		for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
		{
			node_material_t* pMat = &pNode->Materials[i];

			Assert(pMat != NULL);

			// 是否仅Trace水模型
			//if (g_pRender->GetTraceWaterModel())
			//{
			//	if (!get_water_model(pMat))
			//	{
			//		continue;
			//	}
			//}
			//else if (get_water_model(pMat))
			//{
			//	// 水模型默认不Trace
			//	continue;
			//}

			// 是否对具有禁止站立属性标签的模型进行光线追踪
			//if (!g_pRender->GetTraceFobidStandModel())
			//{
			//	if (pMat->MatInfo.bForbidStandModel)
			//	{
			//		continue;
			//	}
			//}

			if (pNode->nModelInfo & MODEL_POSITION_INFO)
			{
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
				FmVec3 p0, p1, p2;

				for (unsigned int face = 0; face < index_num; face += 3)
				{
					//get_triangle_vertex(pMat, pMat->pSingleVB, 
					//	pMat->uSingleVertexSize, face, &p0, &p1, &p2);
					get_single_vertex(pMat, face, &p0, &p1, &p2);
					
					if (math_sphere_triangle_collision(vSphereCenter, 
						fSphereRadius, p0, p1, p2))
					{
						result = true;
						break;
					}
				}
			}
			else if (pNode->nModelInfo & MODEL_POSITION_ANI)
			{
				//float* pVB = pMat->pMultiVB + (pMat->uVertexCnt 
				//	* pMat->uMultiVertexSize * ref->uCurrentFrame) 
				//	/ sizeof(float);
				float* pVB = pMat->pMultiVBs[m_nCurrentFrame]; 
				FmVec3 p0, p1, p2;
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];

				for (unsigned int face = 0; face < index_num; face += 3)
				{
					get_triangle_vertex(pMat, pVB, pMat->nMultiVertexSize, 
						face, &p0, &p1, &p2);

					if (math_sphere_triangle_collision(vSphereCenter, 
						fSphereRadius, p0, p1, p2))
					{
						result = true;
						break;
					}
				}
			}

			if (result == true)
			{
				break;
			}
		}
	}

	NodeSphereTrace_DoChild(pModel, pNode, position, mtxOld, radius, result);
	return result;
}

// 对节点进行球体碰撞测试
bool CModelPlayer::NodeTriangleTrace(model_t* pModel, model_node_t* pNode, 
                       const FmVec3* v0, const FmVec3* v1, const FmVec3* v2)
{
    Assert(pModel != NULL);
	Assert(pNode != NULL);

	bool result = false;

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	if (pNode->nModelInfo & (MODEL_BB_FLAG | MODEL_BBX_FLAG | MODEL_BBY_FLAG))
	{
		CreateBillboard(pNode);
	}

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);
	
	// 屏蔽树叶
	//if ((!g_bTraceTreeLeaf) && (node->nModelInfo & MODEL_TREELEAF_FLAG))
	if ((pNode->nModelInfo & MODEL_TREELEAF_FLAG) 
		&& (!g_pRender->GetTraceTreeLeaf()))
	{
		NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
		return result;
	}

	if (pNode->bHide || pNode->bWaterClip)
	{
		NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
		return result;
	}

	//if (ref->bTraceMainModel 
	//	&& (node->uModelInfo & MODEL_MAINMODEL_FLAG) == 0 
	//	&& (node->type == FXNODE_TYPE_SKIN))
	//{
	//	// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
	//	goto DO_CHILD;
	//}
	//else if (!g_pRender->GetTraceHideModel() 
	//	&& (node->uModelInfo & MODEL_MAINMODEL_FLAG))
	//{
	//	goto DO_CHILD;
	//}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		/*
		if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		//if (!g_pRender->GetTraceHideModel())
		{
			goto DO_CHILD;
		}
		*/
        if (!m_bTraceMainModel)
        {
            // 跳过主模节点
            NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
            NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
            return result;
        }
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
			NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
			return result;
		}
	}

	for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
	{
		node_material_t* pMat = &pNode->Materials[i];

		Assert(pMat != NULL);

		/*
		// 是否仅Trace水模型
		if (g_pRender->GetTraceWaterModel())
		{
			if (!get_water_model(pMat))
			{
				goto DO_CHILD;
			}
		}
		else if (get_water_model(pMat))
		{
			// 水模型默认不Trace
			goto DO_CHILD;
		}

		// 是否对具有禁止站立属性标签的模型进行光线追踪
		if (!g_pRender->GetTraceFobidStandModel())
		{
			if (pMat->MatInfo.bForbidStandModel)
			{
				goto DO_CHILD;
			}
		}
		*/
	}

	// 如果该结点存在分割树，那么使用分割树进行更快的碰撞检测
	//if (node->pSplitTree != NULL)
	if (pNode->pOctTree)
	{
        FmVec3 vT0;
        FmVec3 vT1;
        FmVec3 vT2;
		
		// 将球心变换到三角面的空间
		transform_triangle_to_child_coordinate(&vT0, &vT1, &vT2,
			v0, v1, v2, &m_mtxCurrentTM);

		if (!math_intersect_bb_and_triangle(&pNode->vBoundMin, &pNode->vBoundMax, 
			    &vT0, &vT1, &vT2))
        {
            NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
			return result;
		}

		if (pNode->pOctTree->TraceTriangle(this, vT0, vT1, vT2))
		{
			result = true;
		}
	}
	else
	{
		// 如果该结点不存在分割树，那么将遍历所有三角面来进行碰撞检测
        FmVec3 vT0;
        FmVec3 vT1;
        FmVec3 vT2;

		// 将球心变换到三角面的空间
        transform_triangle_to_child_coordinate(&vT0, &vT1, &vT2,
            v0, v1, v2, &m_mtxCurrentTM);

		if (!math_intersect_bb_and_triangle(&pNode->vBoundMin, &pNode->vBoundMax, 
			    &vT0, &vT1, &vT2))
        {
            NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
			return result;
		}

		// 再进行三角面的碰撞检测，遍历当前结点各个材质所拥有的顶点数组
		for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
		{
			node_material_t* pMat = &pNode->Materials[i];

			Assert(pMat != NULL);

			// 是否仅Trace水模型
			//if (g_pRender->GetTraceWaterModel())
			//{
			//	if (!get_water_model(pMat))
			//	{
			//		continue;
			//	}
			//}
			//else if (get_water_model(pMat))
			//{
			//	// 水模型默认不Trace
			//	continue;
			//}

			// 是否对具有禁止站立属性标签的模型进行光线追踪
			//if (!g_pRender->GetTraceFobidStandModel())
			//{
			//	if (pMat->MatInfo.bForbidStandModel)
			//	{
			//		continue;
			//	}
			//}

			if (pNode->nModelInfo & MODEL_POSITION_INFO)
			{
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
				FmVec3 p0, p1, p2;

				for (unsigned int face = 0; face < index_num; face += 3)
				{
					//get_triangle_vertex(pMat, pMat->pSingleVB, 
					//	pMat->uSingleVertexSize, face, &p0, &p1, &p2);
					get_single_vertex(pMat, face, &p0, &p1, &p2);
					
					if (math_triangle_triangle_collision(vT0, vT1, vT2, p0, p1, p2))
					{
						result = true;
						break;
					}
				}
			}
			else if (pNode->nModelInfo & MODEL_POSITION_ANI)
			{
				//float* pVB = pMat->pMultiVB + (pMat->uVertexCnt 
				//	* pMat->uMultiVertexSize * ref->uCurrentFrame) 
				//	/ sizeof(float);
				float* pVB = pMat->pMultiVBs[m_nCurrentFrame]; 
				FmVec3 p0, p1, p2;
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];

				for (unsigned int face = 0; face < index_num; face += 3)
				{
					get_triangle_vertex(pMat, pVB, pMat->nMultiVertexSize, 
						face, &p0, &p1, &p2);

					if (math_triangle_triangle_collision(vT0, vT1, vT2, p0, p1, p2))
					{
						result = true;
						break;
					}
				}
			}

			if (result == true)
			{
				break;
			}
		}
	}

    NodeTriangleTrace_DoChild(pModel, pNode, mtxOld, v0, v1, v2, result);
	return result;
}

void CModelPlayer::NodeCull(model_t* pModel, model_node_t* pNode, 
	const FmPlane* planes, unsigned int plane_num, 
	const FmVec3* center, float radius, void* cull_func, 
	void* cull_context, const FmMat4* cull_matrix)
{
	Assert(pModel != NULL);
	Assert(pNode != NULL);

	// 如果该结点存在分割树，那么使用分割树进行更快的精选
	FmPlane planesTemp[6];

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	if (pNode->nType != FXNODE_TYPE_MESH)
	{
		NodeCull_DoChild(pModel, pNode, planesTemp, plane_num, center, radius, cull_func, cull_context, cull_matrix, mtxOld);
		return;
		//goto DO_CHILD;
	}

	// 屏蔽树叶
	//if (!g_bTraceTreeLeaf && (node->nModelInfo & MODEL_TREELEAF_FLAG))
	if ((pNode->nModelInfo & MODEL_TREELEAF_FLAG) 
		&& (!g_pRender->GetTraceTreeLeaf()))
	{
		NodeCull_DoChild(pModel, pNode, planesTemp, plane_num, center, radius, cull_func, cull_context, cull_matrix, mtxOld);
		return;
		//goto DO_CHILD;
	}

	if (pNode->bHide || pNode->bWaterClip)
	{
		NodeCull_DoChild(pModel, pNode, planesTemp, plane_num, center, radius, cull_func, cull_context, cull_matrix, mtxOld);
		return;
		//goto DO_CHILD;
	}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		NodeCull_DoChild(pModel, pNode, planesTemp, plane_num, center, radius, cull_func, cull_context, cull_matrix, mtxOld);
		return;
		//goto DO_CHILD;
	}

	for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
	{
		node_material_t* pMat = &pNode->Materials[i];

		/*
		// 是否仅Trace水模型
		if (g_pRender->GetTraceWaterModel())
		{
			if (!get_water_model(pMat))
			{
				goto DO_CHILD;
			}
		}
		else if (get_water_model(pMat))
		{
			// 水模型默认不Trace
			goto DO_CHILD;
		}
		*/

		/*
		// 是否对具有禁止站立属性标签的模型进行光线追踪
		if (!g_pRender->GetTraceFobidStandModel())
		{
			if (pMat->MatInfo.bForbidStandModel)
			{
				goto DO_CHILD;
			}
		}
		*/

		if (pMat->MatInfo.bBlend || 
			pMat->MatInfo.bBlendEnhance || 
			pMat->MatInfo.bAlphaTest || 
			pMat->MatInfo.bRefraction ||
			pMat->MatInfo.bApplique ||
			pMat->MatInfo.bSceneFog)
		{
			NodeCull_DoChild(pModel, pNode, planesTemp, plane_num, center, radius, cull_func, cull_context, cull_matrix, mtxOld);
			return;
			//goto DO_CHILD;
		}
	}

	if (pNode->nModelInfo & (MODEL_BB_FLAG | MODEL_BBX_FLAG | MODEL_BBY_FLAG))
	{
		CreateBillboard(pNode);
	}

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);

	//if (node->pSplitTree != NULL)
	if (pNode->pOctTree)
	{		
		// 将平面变换到结点空间
		Assert(plane_num <= 6);
		
		FmMat4 mtxCurrentInverse;
		FmMat4 mtxLocalInverse;
		
		FmMat4Inverse(&mtxCurrentInverse, NULL, &m_mtxCurrentTM);
		//FmMat4 mtxLocalInverse = mtxTM;
		FmMat4Transpose(&mtxLocalInverse, &m_mtxCurrentTM);
		
		for (unsigned int i = 0; i < plane_num; ++i)
		{
			FmPlaneTransform(&planesTemp[i], &planes[i], &mtxLocalInverse);
		}

		// 将球变换到结点空间
		FmVec3 centerTemp;

		FmVec3TransformCoord(&centerTemp, center, &mtxCurrentInverse);

		//location_t location;

		//FXMATH_MatrixToLocation(&mtxCurrentInverse, &location);
		FmVec3 scale;

		VisUtil_GetMatrixScale(&scale, &mtxCurrentInverse);

		float fRadiusTemp = radius * max(max(scale.x, scale.y), scale.z);

		// 包围盒剪裁
		//if (!math_intersect_bb_and_planes(&node->bbMin, &node->bbMax, 
		//	planesTemp, uPlaneCnt, &centerTemp, fRadiusTemp))
		//{
		//	goto DO_CHILD;
		//}

		pNode->pOctTree->Cull(this, planesTemp, plane_num, centerTemp, 
			fRadiusTemp, (CULL_FUNC)cull_func, cull_context, *cull_matrix);
	}
	else
	{	
		// 如果该结点不存在分割树，那么将遍历所有三角面来进行精选
		// 将平面变换到结点空间
		Assert(plane_num <= 6);

		FmMat4 mtxCurrentInverse;
		FmMat4 mtxLocalInverse;

		FmMat4Inverse(&mtxCurrentInverse, NULL, &m_mtxCurrentTM);
		FmMat4Transpose(&mtxLocalInverse, &m_mtxCurrentTM);
		//FmMat4 mtxLocalInverse = mtxTM;
		//FmMatrixTranspose(&mtxLocalInverse, &mtxLocalInverse);

		for (unsigned int i = 0; i < plane_num; ++i)
		{
			FmPlaneTransform(&planesTemp[i], &planes[i], &mtxLocalInverse);
		}

		// 将球变换到结点空间
		FmVec3 centerTemp;

		FmVec3TransformCoord(&centerTemp, center, &mtxCurrentInverse);

		//location_t location;

		//FXMATH_MatrixToLocation(&mtxCurrentInverse, &location);
		FmVec3 scale;

		VisUtil_GetMatrixScale(&scale, &mtxCurrentInverse);

		float fRadiusTemp = radius * max(max(scale.x, scale.y), scale.z);

		// 包围盒剪裁
		//if (!math_intersect_bb_and_planes(&node->bbMin, &node->bbMax, 
		//	planesTemp, uPlaneCnt, &centerTemp, fRadiusTemp))
		//{
		//	goto DO_CHILD;
		//}

		// 再进行三角面的精选，遍历当前结点各个材质所拥有的顶点数组
		for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
		{
			node_material_t* pMat = &pNode->Materials[i];

			Assert(pMat != NULL);

			// 是否仅Trace水模型
			//if (g_pRender->GetTraceWaterModel())
			//{
			//	if (!get_water_model(pMat))
			//	{
			//		continue;
			//	}
			//}
			//else if (get_water_model(pMat))
			//{
			//	// 水模型默认不Trace
			//	continue;
			//}

			// 是否对具有禁止站立属性标签的模型进行光线追踪
			//if (!g_pRender->GetTraceFobidStandModel())
			//{
			//	if (pMat->MatInfo.bForbidStandModel)
			//	{
			//		continue;
			//	}
			//}

			if (pNode->nModelInfo & MODEL_POSITION_INFO)
			{
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
				FmVec3 ptList[3];
				
				for (unsigned int face = 0; face < index_num; face += 3)
				{
					//get_triangle_vertex(pMat, pMat->pSingleVB, 
					//	pMat->uSingleVertexSize, face, &ptList[0], 
					//	&ptList[1],	&ptList[2]);
					get_single_vertex(pMat, face, &ptList[0], &ptList[1],
						&ptList[2]);

					if (triangle_can_cull(&ptList[0], &ptList[1], &ptList[2],
						planesTemp, plane_num, &centerTemp, fRadiusTemp))
					{
//						FmVec3TransformCoordArray(&ptList[0], 
//							sizeof(FmVec3), &ptList[0], 
//							sizeof(FmVec3), &m_mtxCurrentTM, 3);
						for(int j = 0; j < 3 ; j++ )
							FmVec3TransformCoord( &ptList[j] , &ptList[j] ,&m_mtxCurrentTM );


						FmMat4 mtxWorldInverse;

						FmMat4Inverse(&mtxWorldInverse, NULL, 
							&m_mtxWorldTM);

///						FmVec3TransformCoordArray(&ptList[0], 
//							sizeof(FmVec3), &ptList[0], 
//							sizeof(FmVec3), &mtxWorldInverse, 3);

						for(int j = 0; j < 3 ; j++ )
							FmVec3TransformCoord( &ptList[j] , &ptList[j], &mtxWorldInverse);


						CULL_FUNC func = (CULL_FUNC)cull_func;

						if (!func(cull_context, *cull_matrix, &ptList[0], 3))
						{
							return;
						}
					}
				}
			}
			else if (pNode->nModelInfo & MODEL_POSITION_ANI)
			{
				//float* pVB = pMat->pMultiVB + (pMat->uVertexCnt 
				//	* pMat->uMultiVertexSize * ref->uCurrentFrame) 
				//	/ sizeof(float);
				float* pVB = pMat->pMultiVBs[m_nCurrentFrame]; 
				unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
				FmVec3 ptList[3];

				for (unsigned int face = 0; face < index_num; face += 3)
				{
					get_triangle_vertex(pMat, pVB, pMat->nMultiVertexSize,
						face, &ptList[0], &ptList[1], &ptList[2]);

					if (triangle_can_cull(&ptList[0], &ptList[1], &ptList[2],
						planesTemp, plane_num, &centerTemp, fRadiusTemp))
					{
						/*
						FmVec3TransformCoordArray(&ptList[0], 
							sizeof(FmVec3), &ptList[0], 
							sizeof(FmVec3), &m_mtxCurrentTM, 3);
                        */

						for( int j = 0; i < 3 ; j++ )
							FmVec3TransformCoord( &ptList[j], &ptList[j],&m_mtxCurrentTM );


						FmMat4 mtxWorldInverse;

						FmMat4Inverse(&mtxWorldInverse, NULL, 
							&m_mtxWorldTM);
						/*
						FmVec3TransformCoordArray(&ptList[0], 
							sizeof(FmVec3), &ptList[0], 
							sizeof(FmVec3), &mtxWorldInverse, 3);
						*/
						for(int j = 0; j < 3; j++ )
							FmVec3TransformCoord(&ptList[j],&ptList[j],&mtxWorldInverse);

						CULL_FUNC func = (CULL_FUNC)cull_func;

						if (!func(cull_context, *cull_matrix, &ptList[0], 3))
						{
							return;
						}
					}
				}
			}
		}
	}
//DO_CHILD:
	
	NodeCull_DoChild(pModel, pNode, planesTemp, plane_num, center, radius, cull_func, cull_context, cull_matrix, mtxOld);

	/*
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		model_node_t* child = &pNode->ChildNodes[i];

		Assert(child != NULL);

		NodeCull(pModel, child, planesTemp, plane_num, center, 
			radius, cull_func, cull_context, cull_matrix);
	}

	m_mtxCurrentTM = mtxOld;*/
}

// 计算点到线的距离
static float calculate_point_to_line(const FmVec3* start, 
	const FmVec3* dir, const FmVec3* point)
{
	float bottom = FmVec3Length(dir);

	//if (FloatEqual(0.0F, bottom))
	if (bottom < FLT_EPSILON)
	{
		FmVec3 len = *point - *start;
		return FmVec3Length(&len);
	}

	//a = start; b = start + dir; c = point;
	FmVec3 ac = *start - *point;
	FmVec3 bc = *start + *dir - *point;
	float temp = FmVec3Length(&ac) * FmVec3Length(&bc) / bottom;

	FmVec3Normalize(&ac, &ac);
	FmVec3Normalize(&bc, &bc);

	return temp * sin(acos(ac.x * bc.x + ac.y * bc.y + ac.z * bc.z));
}

bool CModelPlayer::NodeRayTraceHelper(model_t* pModel, model_node_t* pNode, 
	const FmVec3* start, const FmVec3* direction, 
	void** result_node, float* result_distance)
{
	Assert(pModel != NULL);
	Assert(pNode != NULL);
	Assert(start != NULL);
	Assert(direction != NULL);
	Assert(result_node != NULL);
	Assert(result_distance != NULL);

	if (pNode->bHide || pNode->bWaterClip)
	{
		return false;
	}

	//if (ref->bTraceMainModel 
	//	&& (node->uModelInfo & MODEL_MAINMODEL_FLAG) == 0 
	//	&& (node->type == FXNODE_TYPE_SKIN))
	//{
	//	// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
	//	return bRet;
	//}
	//else if (!g_pRender->GetTraceHideModel() 
	//	&& (node->uModelInfo & MODEL_MAINMODEL_FLAG))
	//{
	//	return bRet;
	//}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		//if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		/*
		if (!g_pRender->GetTraceHideModel())
		{
			return false;
		}
		*/
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过此节点
			return false;
		}
	}

	bool bRet = false;

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);

	if (pNode->nType == FXNODE_TYPE_HELPER)
	{
		FmVec3 point = FmVec3(m_mtxCurrentTM._41, 
			m_mtxCurrentTM._42, m_mtxCurrentTM._43);
		float distance = calculate_point_to_line(start, direction, &point);

		if (distance < 0.3F)
		{
			*result_node = pNode;
			*result_distance = distance;
			//pick->model = this;
			//pick->node = node;
			//pick->pos = FmVec3(m_mtxCurrentTM._41, 
			//	m_mtxCurrentTM._42, m_mtxCurrentTM._43);
			//pick->dist = distance;
			//pick->tex = NULL;
			bRet = true;
		}
	}

	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		void* temp_node;
		float temp_dist;

		if (NodeRayTraceHelper(pModel, &pNode->ChildNodes[i], start, 
			direction, &temp_node, &temp_dist))
		{
			if (!bRet)
			{
				*result_node = temp_node;
				*result_distance = temp_dist;
				bRet = true;
			}
			else if (temp_dist < *result_distance)
			{
				*result_node = temp_node;
				*result_distance = temp_dist;
			}
		}
	}

	m_mtxCurrentTM = mtxOld;

	return bRet;
}

void CModelPlayer::NodeRayTraceSkin(model_t* pModel, model_node_t* pNode, 
	const FmVec3* start, const FmVec3* direction, 
	trace_info_t* result)
{
	if (pNode->bHide || pNode->bWaterClip)
	{
		return;
	}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		/*
		if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		//if (!g_pRender->GetTraceHideModel())
		{
			return;
		}
        */        
        if (!m_bTraceMainModel)
        {
            // 跳过主模节点
            return;
        }
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过非主模节点
			return;
		}
	}

	int alpha_ref;

	if (s_bTraceAlpha)
	{
		alpha_ref = g_pRender->GetTraceAlphaRef();
	}
	else
	{
		alpha_ref = 0;
	}

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);

	FmVec3 ray_start, ray_direction;

	transform_ray_to_child_coordinate(&ray_start, &ray_direction, 
		start, direction, &m_mtxCurrentTM);

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		node_material_t* pMat = &pNode->Materials[m];
		FmVec3* pPosList = AllocCpuSkeletonAnimationVB(pMat);

		if (NULL == pPosList)
		{
			// 顶点太多了
			continue;
		}

		unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
		unsigned short* ib = pMat->pLODIB[pMat->nCurLODLevel];

		for (unsigned int face = 0; face < index_num; face += 3)
		{
			unsigned short index0 = ib[face + 0];
			unsigned short index1 = ib[face + 1];
			unsigned short index2 = ib[face + 2];
			float u, v, d;

			if (VisUtil_IntersectTri(&pPosList[index0], &pPosList[index1],
				&pPosList[index2], &ray_start, &ray_direction, &u, &v, &d))
			{
				if ((d < 0.0F) || (d > 1.0F))
				{
					continue;
				}

				if (!triangle_can_trace(&pPosList[index0],
					&pPosList[index1], &pPosList[index2], &ray_start))
				{
					continue;
				}

				if (s_bTraceAlpha && pMat->MatInfo.bAlphaTest
					&& ((NULL == result) || (result->nHitMax > 1) 
					|| (d < result->fDistance)))
				{
					//DxTexture* pTex = FXGetTraceTexture(
					//	pMat->MatInfo.DiffuseMap.pTex);
					ITexture* pTex = pMat->MatInfo.DiffuseMap.pTex;

					if (pTex)
					{
						FmVec2 uv0;
						FmVec2 uv1;
						FmVec2 uv2;

						get_triangle_tex_uv(pNode, pMat, face, &uv0, &uv1, &uv2);

						FmVec2 uv;

						uv = uv0 * (1.0F - u - v) + uv1 * u + uv2 * v;

						unsigned int alpha = 0;

						get_texture_alpha(pTex, uv.x, uv.y, &alpha);

						if ((int)alpha < alpha_ref)
						{
							continue;
						}
					}
				}

				if (NULL == result)
				{
					s_bTraceHitted = true;
					m_mtxCurrentTM = mtxOld;
					return;
				}

				if (!SetTraceInfo(result, pNode, pMat, face, u, v, d, 
					pPosList[index0], pPosList[index1], pPosList[index2]))
				{
					m_mtxCurrentTM = mtxOld;
					return;
				}
			}
		}
	}

	m_mtxCurrentTM = mtxOld;
}

// 对骨骼动画节点进行球碰撞
bool CModelPlayer::NodeSphereTraceSkin(model_t* pModel, model_node_t* pNode, 
    const FmVec3* position, float radius)
{
    	if (pNode->bHide || pNode->bWaterClip)
	{
		return false;
	}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		/*
		if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		//if (!g_pRender->GetTraceHideModel())
		{
			return;
		}
        */    
        if (!m_bTraceMainModel)
        {
            // 跳过主模节点
            return false;
        }
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过非主模节点
			return false;
		}
	}

	int alpha_ref;

	if (s_bTraceAlpha)
	{
		alpha_ref = g_pRender->GetTraceAlphaRef();
	}
	else
	{
		alpha_ref = 0;
	}

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);

    FmVec3 vSphereCenter;
    float fSphereRadius;

    // 将球心变换到三角面的空间
    transform_sphere_to_child_coordinate(&vSphereCenter, 
        &fSphereRadius, position, radius, &m_mtxCurrentTM);

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		node_material_t* pMat = &pNode->Materials[m];
		FmVec3* pPosList = AllocCpuSkeletonAnimationVB(pMat);

		if (NULL == pPosList)
		{
			// 顶点太多了
			continue;
		}

		unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
		unsigned short* ib = pMat->pLODIB[pMat->nCurLODLevel];

		for (unsigned int face = 0; face < index_num; face += 3)
		{
			unsigned short index0 = ib[face + 0];
			unsigned short index1 = ib[face + 1];
			unsigned short index2 = ib[face + 2];

			if (math_sphere_triangle_collision(fSphereRadius, fSphereRadius,
                pPosList[index0], pPosList[index1], pPosList[index2]))
			{
				m_mtxCurrentTM = mtxOld;
				return true;
			}
		}
	}

	m_mtxCurrentTM = mtxOld;

    return false;
}

// 对骨骼动画节点进行三角形碰撞
bool CModelPlayer::NodeTriangleTraceSkin(model_t* pModel, model_node_t* pNode, 
    const FmVec3* v0, const FmVec3* v1, const FmVec3* v2)
{
	if (pNode->bHide || pNode->bWaterClip)
	{
		return false;
	}

	if (pNode->nModelInfo & MODEL_MAINMODEL_FLAG)
	{
		/*
		if (g_pRender->GetEditorMode() && !g_pRender->GetTraceHideModel())
		//if (!g_pRender->GetTraceHideModel())
		{
			return;
		}
        */    
        if (!m_bTraceMainModel)
        {
            // 跳过主模节点
            return false;
        }
	}
	else
	{
		if (m_bTraceMainModel)
		{
			// 如果设置只碰撞检测模型上的裸模，而当前节点不是裸模，则跳过非主模节点
			return false;
		}
	}

	int alpha_ref;

	if (s_bTraceAlpha)
	{
		alpha_ref = g_pRender->GetTraceAlphaRef();
	}
	else
	{
		alpha_ref = 0;
	}

	// 保存当前矩阵，用于恢复并渲染其兄弟结点，以及其父亲乃至祖系的各个结点
	FmMat4 mtxOld = m_mtxCurrentTM;

	// 计算当前世界矩阵
	SetCurrentMatrix(pNode);

	FmVec3 vT0, vT1, vT2;

	transform_triangle_to_child_coordinate(&vT0, &vT1, &vT2, 
        v0, v1, v2, &m_mtxCurrentTM);

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		node_material_t* pMat = &pNode->Materials[m];
		FmVec3* pPosList = AllocCpuSkeletonAnimationVB(pMat);

		if (NULL == pPosList)
		{
			// 顶点太多了
			continue;
		}

		unsigned int index_num = pMat->pLODIndexCount[pMat->nCurLODLevel];
		unsigned short* ib = pMat->pLODIB[pMat->nCurLODLevel];

		for (unsigned int face = 0; face < index_num; face += 3)
		{
			unsigned short index0 = ib[face + 0];
			unsigned short index1 = ib[face + 1];
			unsigned short index2 = ib[face + 2];

			if (math_triangle_triangle_collision(pPosList[index0], pPosList[index1], pPosList[index2], 
                    vT0, vT1, vT2))
			{
				m_mtxCurrentTM = mtxOld;
				return true;
			}
		}
	}

	m_mtxCurrentTM = mtxOld;

    return false;
}

// 根据距离判断是否仅碰撞检测包围盒
bool CModelPlayer::OnlyTraceBoundBoxByDistance( 
	const FmVec3* start, const FmVec3* direction, float dis, 
	trace_info_t* result)
{
//	if (dis <= 1.0F)
//	{
//		return false;
//	}

	IRenderContext* pContext = m_pContext;

	if (NULL == pContext)
	{
		return false;
	}

//	float radius = VisUtil_Vec3Length(&(ref->bbRootMax - ref->bbRootMin)) * 0.5F;
//
//	if (radius > 2.5F)
//	{
//		// 大模型不能仅通过包围盒进行碰撞检测
//		return false;
//	}

	FmVec3 pos(m_mtxCurrentTM._41, m_mtxCurrentTM._42, 
		m_mtxCurrentTM._43);
	FmVec3 v = pos - pContext->GetCamera().vPosition;
	float len = VisUtil_Vec3Length(&v);

	if (len > dis)
	{
		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::OnlyTraceBoundBoxByDistance] pModel == NULL model name is %s", m_pResModel->GetName());
			return false;
		}

		model_node_t* pNode = &pModel->RootNodes[0];

		if (pNode->nMaterialCount <= 0)
		{
			return false;
		}

		node_material_t* pMat = &pNode->Materials[0];
		FmMat4 mtxWorldInverse;

		FmMat4Inverse(&mtxWorldInverse, NULL, &m_mtxWorldTM);
		FmVec3TransformCoord(&pos, &pos, &mtxWorldInverse);

		if (NULL == result)
		{
			s_bTraceHitted = true;
			return true;
		}

		return SetTraceInfo(result, pNode, pMat, 0, 0, 0, 
			len / dis / VisUtil_Vec3Length(direction), pos, pos, pos);
	}

	return false;
}

bool CModelPlayer::InnerModelRayTrace(bool can_trace_box, 
	const FmVec3* start, const FmVec3* direction, 
	trace_info_t* result)
{
	Assert(start != NULL);
	Assert(direction != NULL);

	// 初始化助手变量
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::InnerModelRayTrace] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}
	
	if (can_trace_box)
	{
		/*
		// 只有设置的距离大于1时，才仅碰撞检测包围盒
		float trace_box_dist = g_pRender->GetTraceBoundBoxDistance();

		if (trace_box_dist > 1.0F)
		{
			// 判断射线是否与根包围盒相交
			FmVec3 ray_start;
			FmVec3 ray_dir;

			transform_ray_to_child_coordinate(&ray_start, &ray_dir, 
				start, direction, &m_mtxWorldTM);

			if (!FmBoxBoundProbe(&pModel->vRootMin, &pModel->vRootMax, 
				&ray_start, &ray_dir))
			{
				return false;
			}

			if (OnlyTraceBoundBoxByDistance(start, direction, 
				trace_box_dist, result))
			{
				return true;
			}
		}
		*/
	}

	// 初始化当前迭带变换矩阵
	m_mtxCurrentTM = m_mtxWorldTM;

	// 逐结点进行碰撞检测
	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		if (pModel->RootNodes[i].nType == FXNODE_TYPE_MESH)
		{
			NodeRayTrace(pModel, &pModel->RootNodes[i], start, direction, 
				result);
		}
		else if (pModel->RootNodes[i].nType == FXNODE_TYPE_SKIN)
		{
			if (m_pActionPlayer != NULL)
			{
				NodeRayTraceSkin(pModel, &pModel->RootNodes[i], start, 
					direction, result);
			}
			else
			{
				NodeRayTrace(pModel, &pModel->RootNodes[i], start, 
					direction, result);
			}
		}
		else if (pModel->RootNodes[i].nType == FXNODE_TYPE_HELPER)
		{
			if (NULL == m_pActionPlayer)
			{
				NodeRayTrace(pModel, &pModel->RootNodes[i], start, 
					direction, result);
			}
		}

		if (s_bTraceHitted && (NULL == result))
		{
			break;
		}
	}

	return s_bTraceHitted;
}

bool CModelPlayer::ModelRayTrace(bool can_trace_box,
	const FmVec3* start, const FmVec3* direction, 
	trace_info_t* result)
{
	s_bTraceHitted = false;
	
	InnerModelRayTrace(can_trace_box, start, direction, result);

	return s_bTraceHitted;
}

bool CModelPlayer::ModelRayTraceAlpha(bool can_trace_box, 
	const FmVec3* start, const FmVec3* direction, 
	trace_info_t* result)
{
	s_bTraceHitted = false;
	s_bTraceAlpha = true;

	InnerModelRayTrace(can_trace_box, start, direction, result);

	s_bTraceAlpha = false;

	return s_bTraceHitted;
}

bool CModelPlayer::ModelRayTraceFast( 
	const FmVec3* start, const FmVec3* direction) 
{
	s_bTraceHitted = false;

	InnerModelRayTrace(false, start, direction, NULL);

	return s_bTraceHitted;
}

bool CModelPlayer::ModelRayTraceFastAlpha( 
	const FmVec3* start, const FmVec3* direction) 
{
	s_bTraceHitted = false;
	s_bTraceAlpha = true;

	InnerModelRayTrace(false, start, direction, NULL);

	s_bTraceAlpha = false;

	return s_bTraceHitted;
}

bool CModelPlayer::ModelSphereTrace(const FmVec3* position, float radius)
{
	Assert(position != NULL);
	Assert(radius != NULL);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::ModelSphereTrace] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	// 初始化当前迭带变换矩阵
	m_mtxCurrentTM = m_mtxWorldTM;

	// 逐结点判断是否存在相交
	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
        if (pModel->RootNodes[i].nType == FXNODE_TYPE_MESH)
        {
            if (NodeSphereTrace(pModel, &pModel->RootNodes[i], position, radius))
            {
                return true;
            }
        }
        else if (pModel->RootNodes[i].nType == FXNODE_TYPE_SKIN)
        {
            if (m_pActionPlayer != NULL)
            {
                if (NodeSphereTraceSkin(pModel, &pModel->RootNodes[i], position, radius))
                {
                    return true;
                }
            }
            else
            {
                if (NodeSphereTrace(pModel, &pModel->RootNodes[i], position, radius))
                {
                    return true;
                }
            }
        }
	}

	return false;
}

bool CModelPlayer::ModelTriangleTrace(const FmVec3* v0, const FmVec3* v1, const FmVec3* v2)
{
    model_t* pModel = m_pResModel->GetModelData();

    if (pModel == NULL)
    {
        CORE_TRACE_EX("WARNING: [CModelPlayer::ModelTriangleTrace] pModel == NULL model name is %s", m_pResModel->GetName());
        return false;
    }

    // 初始化当前迭带变换矩阵
    m_mtxCurrentTM = m_mtxWorldTM;

    // 逐结点判断是否存在相交
    for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
    {
        if (pModel->RootNodes[i].nType == FXNODE_TYPE_MESH)
        {
            if (NodeTriangleTrace(pModel, &pModel->RootNodes[i], v0, v1, v2))
            {
                return true;
            }
        }
        else if (pModel->RootNodes[i].nType == FXNODE_TYPE_SKIN)
        {
            if (m_pActionPlayer != NULL)
            {
                if (NodeTriangleTraceSkin(pModel, &pModel->RootNodes[i], v0, v1, v2))
                {
                    return true;
                }
            }
            else
            {
                if (NodeTriangleTrace(pModel, &pModel->RootNodes[i], v0, v1, v2))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void* CModelPlayer::RayTraceHelper(const FmVec3* start, 
	const FmVec3* direction)
{
	Assert(start != NULL);
	Assert(direction != NULL);

	if (m_bHide)
	{
		return NULL;
	}

	if (!m_bShowHelper)
	{
		return NULL;
	}

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::RayTraceHelper] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	void* pResult = NULL;
	float fDistance = 1e8;

	// 初始化当前迭带变换矩阵
	m_mtxCurrentTM = m_mtxWorldTM;

	// 逐结点进行碰撞检测
	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		void* temp_node;
		float temp_dist;

		if (NodeRayTraceHelper(pModel, &pModel->RootNodes[i], start, 
			direction, &temp_node, &temp_dist))
		{
			if (NULL == pResult)
			{
				pResult = temp_node;
				fDistance = temp_dist;
			}
			else if (temp_dist < fDistance)
			{
				pResult = temp_node;
				fDistance = temp_dist;
			}
		}
	}

	return pResult;
}

bool CModelPlayer::ModelCull(const FmPlane* planes, 
	unsigned int plane_num, const FmVec3* center, float fRadius, 
	void* cull_func, void* cull_context, const FmMat4* cull_matrix)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::ModelCull] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	m_mtxCurrentTM = m_mtxWorldTM;

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		model_node_t* child = &pModel->RootNodes[i];

		NodeCull(pModel, child, planes, plane_num, center, 
			fRadius, cull_func, cull_context, cull_matrix); 
	}

	return true;
}
