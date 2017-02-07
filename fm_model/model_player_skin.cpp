//--------------------------------------------------------------------
// 文件名:		model_player_skin.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "action_player.h"
#include "res_model.h"
#include "../visual/i_render.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"

extern IRender* g_pRender;

// 该宏开关打开后将会在log查看器中error中显示蒙皮权重有问题的顶点信息
// 用于调试顶点权重为0的bug
#ifndef DEBUG_SKIN_WEIGHT
#define DEBUG_SKIN_WEIGHT
#endif

bool reset_material_vertex_bone_mapping(node_material_t* mat, 
	model_t* inst, skt_pose_t* pose)
{
	if (mat->pWeightTable == NULL || pose == NULL)
	{
		return false;
	}

	skeleton_t* instsa = pose->pSkeleton;

	if (mat->pBoneList)
	{
		CORE_FREE(mat->pBoneList, sizeof(unsigned int) * MAX_BONE);
	}

	mat->nMappedBoneCount = 0;
	mat->pBoneList = (unsigned int*)CORE_ALLOC(sizeof(unsigned int) * MAX_BONE);

#ifdef DEBUG_SKIN_WEIGHT
	float fWeight = 0.0F;
#endif

	unsigned int* mat_bone_list = mat->pBoneList;
	unsigned int bone_node_num = instsa->nTotalBoneNodeCount;
	skt_node_t** bone_node_list = instsa->pBoneNodeList;
	unsigned int vertex_num = mat->nVertexCount;
	unsigned int total_weight_num = vertex_num * MAX_VERTEX_BONE_BLEND;
	node_weight_table_t* pWeightTable = mat->pWeightTable;

	for (unsigned int k = 0; k < total_weight_num; ++k)
	{
#ifdef DEBUG_SKIN_WEIGHT
		if (k % MAX_VERTEX_BONE_BLEND == 0)
		{
			if (k != 0)
			{
				if (fWeight > 1.001 || fWeight < 0.999)
				{
					//UniqueErrorLog("[error 030055]skin name is %s, value = %f\n", 
					//	inst->pszName, fWeight);
				}
			}

			fWeight = 0.0F;
		}
#endif

		node_weight_table_t* wnode = &pWeightTable[k];
		int bone_name_offset = wnode->nBoneNameOffset;

		if (bone_name_offset == -1)
		{
			wnode->nBoneID = -1;
			wnode->fWeight = 0.0F;
			continue;
		}

		unsigned int map_bone_num = mat->nMappedBoneCount;
		char* map_bone_name = &mat->pszBoneNameList[bone_name_offset];
		unsigned int hash = GetHashValueCase(map_bone_name);
		int nExistPos = -1;

		for (unsigned int b = 0; b < map_bone_num; b++)
		{
			skt_node_t* bone_node = bone_node_list[mat_bone_list[b]];
			
			if ((bone_node->nHash == hash)
				&& (strcmp(bone_node->pszName, map_bone_name) == 0))
			{
				nExistPos = b;
				break;
			}
		}

		if (nExistPos == -1)
		{
			if (mat->nMappedBoneCount == MAX_BONE)
			{
				//UniqueErrorLog("[error 030025] FXResetVAMatVertexBoneMapping:"
				//	"Current material's bone list is full, the max limit bone "
				//	"count is %d, skin is %s\n", MAX_BONE, inst->pszName);
				//FXLOG_ERROR("[error 030025] FXResetVAMatVertexBoneMapping:"
				//	"Current material's bone list is full, the max limit bone "
				//	"count is %d, skin is %s\n", MAX_BONE, inst->pszName);
				return false;
			}

			int nExistBonePos = -1;
			//char* pErrorBone = "";

			for (unsigned int b = 0; b < bone_node_num; b++)
			{
				// Here can't use strstr or strcmp function to compare the two strings 
				// for the postfix of the bone's name, etc !T(TRANSLATE BONE).
				char* pdst = bone_node_list[b]->pszName;
				//char* psrc = &mat->pszBoneNameList[wnode->nBoneNameOffset];
				char* psrc = map_bone_name;

				//pErrorBone = map_bone_name;

				bool bMatch = true;

				while (*psrc != 0 || *pdst != 0)
				{
					if (*psrc++ != *pdst++)
					{
						bMatch = false;
						break;
					}
				}

				if (bMatch)
				{
					nExistBonePos = b;
					break;
				}
			}

			if (nExistBonePos != -1)
			{
				wnode->nBoneID = mat->nMappedBoneCount;
				mat->pBoneList[mat->nMappedBoneCount] = nExistBonePos;
				mat->nMappedBoneCount++;
#ifdef DEBUG_SKIN_WEIGHT
				fWeight += wnode->fWeight;
#endif
			}
			else
			{
				//UniqueErrorLog("[error 030026] FXResetVAMatVertexBoneMapping:"
				//	"The bone[%s] vertex specified is not exist in the "
				//	"skeleton model [%s%s], mat name is [%s]!\n",
				//	map_bone_name, inst->pszTexBase, inst->pszName, mat->pszName);
				//FXLOG_ERROR("[error 030026] FXResetVAMatVertexBoneMapping:"
				//	"The bone[%s] vertex specified is not exist in the "
				//	"skeleton model [%s%s], mat name is [%s]!\n",
				//	map_bone_name, inst->pszTexBase, inst->pszName, mat->pszName);
				return false;
			}
		}
		else
		{
			wnode->nBoneID = nExistPos;
#ifdef DEBUG_SKIN_WEIGHT
			fWeight += wnode->fWeight;
#endif
		}
	}

	TAutoMem<skinned_blend_vertex_t, 256, TCoreAlloc> auto_buf(vertex_num);
	skinned_blend_vertex_t* pSkinVertex = auto_buf.GetBuffer();
	node_weight_table_t* pWeightNode = pWeightTable;

	for (unsigned int i = 0; i < vertex_num; ++i)
	{
		pSkinVertex->nIndex[0] = (short)pWeightNode[0].nBoneID;
		pSkinVertex->nIndex[1] = (short)pWeightNode[1].nBoneID;
		pSkinVertex->nIndex[2] = (short)pWeightNode[2].nBoneID;
		pSkinVertex->nIndex[3] = (short)pWeightNode[3].nBoneID;
		pSkinVertex->fWeight[0] = pWeightNode[0].fWeight;
		pSkinVertex->fWeight[1] = pWeightNode[1].fWeight;
		pSkinVertex->fWeight[2] = pWeightNode[2].fWeight;
		pSkinVertex->fWeight[3] = pWeightNode[3].fWeight;
		pSkinVertex++;
		pWeightNode += MAX_VERTEX_BONE_BLEND;
	}

	/*
	mat->pVBlendGPUVB = g_pRender->CreateStaticVB(auto_buf.GetBuffer(),
		vertex_num * mat->nSkinnedVertexBoneBlendVertexSize);*/


	//mat->pVBlendGPUVB = g_pRender->CreateStaticVB(
	//	mat->uVertexCnt * mat->uSkinnedVertexBoneBlendVertexSize, 0);

	/*
	暂时注掉，因为现在暂时不用多流
	if (NULL == mat->pVBlendGPUVB)
	{
		//UniqueErrorLog("[error 030027] FXResetVAMatVertexBoneMapping:Create "
		//	"Skinned vertex bone blend vertex buffer failed!, skin is %s\n", 
		//	inst->pszName);
		//FXLOG_ERROR("[error 030027] FXResetVAMatVertexBoneMapping:Create "
		//	"Skinned vertex bone blend vertex buffer failed!, skin is %s\n", 
		//	inst->pszName);
		return false;
	}*/

//	skinned_blend_vertex_t* pSkinVertex = 
//		(skinned_blend_vertex_t*)mat->pVBlendGPUVB->Lock();
//
//	if (NULL == pSkinVertex)
//	{
//		//UniqueErrorLog("[error 030028] FXResetVAMatVertexBoneMapping:Lock single "
//		//	"vertex buffer failed! skin is %s\n", inst->pszName);
//		//FXLOG_ERROR("[error 030028] FXResetVAMatVertexBoneMapping:Lock single "
//		//	"vertex buffer failed! skin is %s\n", inst->pszName);
//		return false;
//	}
//
//	node_weight_table_t* pWeightNode = pWeightTable;
//	
//	for (unsigned int x = 0; x < vertex_num; x++)
//	{
//		pSkinVertex->index[0] = (short)pWeightNode[0].nBoneID;
//		pSkinVertex->index[1] = (short)pWeightNode[1].nBoneID;
//		pSkinVertex->index[2] = (short)pWeightNode[2].nBoneID;
//		pSkinVertex->index[3] = (short)pWeightNode[3].nBoneID;
//		pSkinVertex->weight[0] = pWeightNode[0].fWeight;
//		pSkinVertex->weight[1] = pWeightNode[1].fWeight;
//		pSkinVertex->weight[2] = pWeightNode[2].fWeight;
//		pSkinVertex->weight[3] = pWeightNode[3].fWeight;
//		pSkinVertex++;
//		pWeightNode += MAX_VERTEX_BONE_BLEND;
//	}
//
//	mat->pVBlendGPUVB->Unlock();

	return true;
}

static bool reset_node_vertex_bone_mapping(model_node_t* pNode, 
	model_t* pModel, skt_pose_t* pose)
{
	Assert(pNode != NULL);
	Assert(pModel != NULL);

	for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
	{
		node_material_t* mat = &pNode->Materials[k];

		if (!reset_material_vertex_bone_mapping(mat, pModel, pose))
		{
			//UniqueErrorLog("[error 030029] FXResetVANodeVertexBoneMapping:"
			//	"Reset mat vertex bone mapping error, The Model File is "
			//	"[%s%s]\n", inst->pszTexBase, inst->pszName);
			//FXLOG_ERROR("[error 030029] FXResetVANodeVertexBoneMapping:"
			//	"Reset mat vertex bone mapping error, The Model File is "
			//	"[%s%s]\n", inst->pszTexBase, inst->pszName);
			return false;
		}
	}

	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		if (!reset_node_vertex_bone_mapping(&pNode->ChildNodes[i], pModel, 
			pose))
		{
			return false;
		}
	}

	return true;
}

static bool is_init_skeleton_mapping_table(model_node_t* pNode)
{
	for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
	{
		node_material_t* pMat = &pNode->Materials[k];

		/*
		暂时注掉，因为现在暂时不用多流
		if (pMat->pVBlendGPUVB == NULL)
		{
			return false;
		}*/

		//因为把多流去掉了，原来这里是用来检查pVBlendGPUVB是否为空来判断骨骼数据有没有填充好，现在改为检查pSingleGPUVB;
		if(pMat->pSingleGPUVB == NULL)
		{
			return false;
		}

	}

	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		bool bResult = is_init_skeleton_mapping_table(&pNode->ChildNodes[i]);

		if (!bResult)
		{
			return false;
		}
	}

	return true;
}

static void release_node_vertex_bone_mapping(model_node_t* pNode, 
	model_t* pModel)
{
	Assert(pNode != NULL);
	Assert(pModel != NULL);

	for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
	{
		node_material_t* mat = &pNode->Materials[k];

		SAFE_RELEASE(mat->pVBlendGPUVB);
		
		if (mat->pBoneList)
		{
			CORE_FREE(mat->pBoneList, sizeof(unsigned int) * MAX_BONE);
			mat->pBoneList = NULL;
		}

		mat->nMappedBoneCount = 0;
	}

	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		release_node_vertex_bone_mapping(&pNode->ChildNodes[i], pModel);
	}
}

bool CModelPlayer::InitSkeletonMappingTable()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::InitSkeletonMappingTable] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	if (IsInitSkeletonMappingTable())
	{
		return true;
	}

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	skt_pose_t* pose = m_pActionPlayer->GetCurrentPose();
	
	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		if (!reset_node_vertex_bone_mapping(&pModel->RootNodes[i], pModel, 
			pose))
		{
			return false;
		}
	}

	return true;
}

bool CModelPlayer::IsInitSkeletonMappingTable()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::IsInitSkeletonMappingTable] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		if (!is_init_skeleton_mapping_table(&pModel->RootNodes[i]))
		{
			return false;
		}
	}

	return true;
}

void CModelPlayer::ReleaseSkeletonMappingTable()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::ReleaseSkeletonMappingTable] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		release_node_vertex_bone_mapping(&pModel->RootNodes[i], pModel);
	}
}
