//--------------------------------------------------------------------
// 文件名:		model_player_update.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "res_model.h"
#include "math_3d.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"

void CModelPlayer::UpdateModelNode(model_node_t* pNode, model_t* pModel, 
	bool bCull)
{
	int id;
	Assert(pNode != NULL);

	FmMat4 mtxOldWorld = m_mtxCurrentTM;

	pNode->bNeedDraw = false;

	// Calculate model transform matrix.
	if (pNode->nModelInfo & (MODEL_BB_FLAG | MODEL_BBX_FLAG | MODEL_BBY_FLAG))
	{
		CreateBillboard(pNode);
	}

	if (pNode->nModelInfo & MODEL_MATRIX_INFO)
	{
		FmMat4 mtxTM;

		math_location_to_matrix(&pNode->pLocation[0], &mtxTM);
		FmMatrixMultiply(&m_mtxCurrentTM, &mtxTM, &m_mtxCurrentTM);
	}
	else if (pNode->nModelInfo & MODEL_MATRIX_ANI)
	{
		unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;
		float weight = m_fCurrentFrameOffset;
		//FmMat4 mtxScaleSrc, mtxRotateSrc, mtxTranslateSrc;
		location_t src;
		location_t dst;

		src = pNode->pLocation[m_nCurrentFrame];

		if (m_nCurrentFrame == (frame_num - 1))
		{
			if (m_bLoop)
			{
				dst = pNode->pLocation[0];
			}
			else
			{
				dst = pNode->pLocation[m_nCurrentFrame];
			}
		}
		else
		{
			dst = pNode->pLocation[m_nCurrentFrame + 1];
		}

		if ((src.scale.x < 0.01F) || (dst.scale.x < 0.01F) ||
			(src.scale.y < 0.01F) || (dst.scale.y < 0.01F) ||
			(src.scale.z < 0.01F) || (dst.scale.z < 0.01F))
		{	
			// 当缩放比例小于0.01F的时候，则对node不进行描画
			goto DO_CHILD;
		}

		math_location_lerp(&src, &src, &dst, weight);

		FmMat4 mtxTM;

		math_location_to_matrix(&src, &mtxTM);
		FmMatrixMultiply(&m_mtxCurrentTM, &mtxTM, &m_mtxCurrentTM);
	}

	// Record the transform matrix.
	id = pNode->nIndex;

	Assert(id >= 0);

	m_mtxNodeCurrentTM[id] = m_mtxCurrentTM;

	if (pNode->bHide)
	{
		goto DO_CHILD;
	}

	/*
	if (bCull)
	{
		FmVec3 bbTempMin;
		FmVec3 bbTempMax;

		// Calculate node bounding box.
		FmVec3TransformCoord(&bbTempMin, &node->bbMin, 
			&ref->mtxCurrentTM);
		FmVec3TransformCoord(&bbTempMax, &node->bbMax, 
			&ref->mtxCurrentTM);

		// View cone cull node, if it is not exist in view cone, try it's childs.
		if (!math_intersect_bb_and_planes(&bbTempMin, &bbTempMax, 
			(FmVec4*)s_viewcone, 6))
		{
			goto DO_CHILD;
		}
	}
	*/

	pNode->bNeedDraw = true;
	
	
//	pNode->m_NodeList[pNode->nIndex].mtxCurrentTM = m_mtxCurrentTM;

DO_CHILD:
	for (unsigned int child = 0; child < pNode->nChildNodeCount; child++)
	{
		UpdateModelNode(&pNode->ChildNodes[child], pModel, bCull);
	}

	m_mtxCurrentTM = mtxOldWorld;
}

void CModelPlayer::UpdateAniTime(double dTime)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::UpdateAniTime] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;

	if ((m_nState == MODEL_STATE_PLAYING) && (frame_num > 1) 
		&& (m_fSpeed > 0))
	{
		double dCurrentFrame = m_dCurrentTime * pModel->fInternalFPS;
		
		m_nCurrentFrame = (unsigned int)dCurrentFrame;
		m_fCurrentFrameOffset = (float)(dCurrentFrame - m_nCurrentFrame);

		if (m_nCurrentFrame >= (frame_num - 1))
		{
			if (m_bLoop)
			{
				if (m_pActionPlayer != NULL)
				{
					m_nCurrentFrame = 3;
					m_dCurrentTime = 3.0F / pModel->fInternalFPS;
				}
				else
				{
					m_nCurrentFrame = 0;
					m_dCurrentTime = 0.0F;
				}
			}
			else
			{
				m_nCurrentFrame = frame_num - 1;
				m_nState = MODEL_STATE_STOPPED;
			}
		}
		else
		{
			m_dCurrentTime += (dTime - m_dLastTime) * m_fSpeed;
		}
	}

	m_dLastTime = dTime;
}

void CModelPlayer::UpdateModel(bool bCull)
{
	if (0 == m_nState)
	{
		return;
	}
	
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::UpdateModel] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}
	//Assert(pModel != NULL);

	//m_bNeedDraw = false;

	//if (!pModel->bLoaded || m_bHide)
	//{
	//	return;
	//}

	m_bNeedDraw = pModel->bLoaded && (!m_bHide);

	/*
	// View cone cull.
	if (bCull)
	{
		FmVec3 bbRootMin = ref->bbRootMin;
		FmVec3 bbRootMax = ref->bbRootMax;

		bbRootMin.x += ref->mtxWorldTM._41;
		bbRootMin.y += ref->mtxWorldTM._42;
		bbRootMin.z += ref->mtxWorldTM._43;
		bbRootMax.x += ref->mtxWorldTM._41;
		bbRootMax.y += ref->mtxWorldTM._42;
		bbRootMax.z += ref->mtxWorldTM._43;

		if (!math_intersect_bb_and_planes(&bbRootMin, &bbRootMax, 
			(FmVec4*)s_viewcone, 6))
		{
			return;
		}
	}
	*/

	m_mtxCurrentTM = m_mtxWorldTM;
	//m_bNeedDraw = true;

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		UpdateModelNode(&pModel->RootNodes[i], pModel, bCull);
	}
}
