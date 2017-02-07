//--------------------------------------------------------------------
// 文件名:		model_player_param.cpp
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
//#include "../visual/sse.h"
#include "../visual/vis_utils.h"

void CModelPlayer::SetColor(unsigned int value)
{
	m_nColor = value;
}

unsigned int CModelPlayer::GetColor()
{
	return m_nColor;
}

void CModelPlayer::SetUnderWater(bool value)
{
	m_bUnderWater = value;
}

bool CModelPlayer::GetUnderWater()
{
	return m_bUnderWater;
}

unsigned int CModelPlayer::GetFrameCount()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetFrameCount] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0;
	}

	return pModel->nEndFrame - pModel->nStartFrame + 1;
}

float CModelPlayer::GetInternalFPS()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetInternalFPS] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0.0f;
	}

	return pModel->fInternalFPS;
}

void CModelPlayer::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

float CModelPlayer::GetSpeed()
{
	return m_fSpeed;
}

void CModelPlayer::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;
}

bool CModelPlayer::GetLoop()
{
	return m_bLoop;
}

void CModelPlayer::Play()
{
	m_nCurrentFrame = 0;
	m_nState = MODEL_STATE_PLAYING;
}

void CModelPlayer::Stop()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::Stop] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	m_nCurrentFrame = pModel->nEndFrame - pModel->nStartFrame;
	m_nState = MODEL_STATE_STOPPED;
}

void CModelPlayer::Pause()
{
	if (MODEL_STATE_PLAYING == m_nState)
	{
		m_nState = MODEL_STATE_PAUSE;
	}
}

void CModelPlayer::Resume()
{
	if (MODEL_STATE_PAUSE == m_nState)
	{
		m_nState = MODEL_STATE_PLAYING;
	}
}

void CModelPlayer::SetCurrentTimeFloat(double time)
{
	m_dCurrentTime = time;

	model_t* pModel = m_pResModel->GetModelData();
	
	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetCurrentTimeFloat] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	double dCurrentFrame = m_dCurrentTime * pModel->fInternalFPS;

	m_nCurrentFrame = (unsigned int)dCurrentFrame;
	m_fCurrentFrameOffset = (float)(dCurrentFrame - m_nCurrentFrame);
}

double CModelPlayer::GetCurrentTimeFloat()
{
	return m_dCurrentTime;
}

void CModelPlayer::SetCurrentFrame(unsigned int nFrame)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetCurrentFrame] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	m_nCurrentFrame = nFrame;
	m_fCurrentFrameOffset = 0.0F;
	m_dCurrentTime = nFrame / pModel->fInternalFPS;
}

unsigned int CModelPlayer::GetCurrentFrame()
{
	return m_nCurrentFrame;
}

void CModelPlayer::SetHide(bool bHide)
{
	m_bHide = bHide;
}

bool CModelPlayer::GetHide()
{
	return m_bHide;
}

void CModelPlayer::SetLive(bool value)
{
	m_bLive = value;
}

bool CModelPlayer::GetLive()
{
	return m_bLive;
}

void CModelPlayer::SetSelection(bool bSelected)
{
	m_bSelected = bSelected;
}

bool CModelPlayer::GetSelection()
{
	return m_bSelected;
}

void CModelPlayer::SetShowHelper(bool bShow)
{
	m_bShowHelper = bShow;
}

bool CModelPlayer::GetShowHelper()
{
	return m_bShowHelper;
}

unsigned int CModelPlayer::GetRootNodeCount()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetRootNodeCount] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0;
	}

	return pModel->nRootNodeCount;
}

void* CModelPlayer::GetRootNodeFromID(unsigned int index)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetRootNodeFromID] pModel == NULL model name is %s", m_pResModel->GetName());
		return NULL;
	}

	Assert(index < pModel->nRootNodeCount);

	if (index >= pModel->nRootNodeCount)
	{
		return NULL;
	}

	return &pModel->RootNodes[index];
}

unsigned int CModelPlayer::GetNodeCount()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetNodeCount] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0;
	}

	return pModel->nNodeCount;
}

void* CModelPlayer::GetNodeFromID(int iNodeID)
{
	Assert(iNodeID >= 0);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetNodeFromID] pModel == NULL model name is %s", m_pResModel->GetName());
		return NULL;
	}

	if ((unsigned int)iNodeID >= pModel->nNodeCount)
	{
		return NULL;
	}

	return pModel->pNodeList[iNodeID];
}

void* CModelPlayer::GetNodeFromName(const char* pszNodeName)
{
	Assert(pszNodeName != NULL);

	model_t* pModel = m_pResModel->GetModelData();
	
	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetNodeFromName] pModel == NULL model name is %s", m_pResModel->GetName());
		return NULL;
	}

	unsigned int node_num = pModel->nNodeCount;
	model_node_t** node_list = pModel->pNodeList;

	for (unsigned int i = 0; i < node_num; ++i)
	{
		model_node_t* pNode = node_list[i];

		if (strcmp(pNode->pszName, pszNodeName) == 0)
		{
			return pNode;
		}
	}

	return NULL;
}

int CModelPlayer::GetNodeID(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	return pNode->nIndex;
}

const char* CModelPlayer::GetNodeName(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	return pNode->pszName;
}

FXNODE_TYPE CModelPlayer::GetNodeType(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	return pNode->nType;
}

float CModelPlayer::GetCameraNodeFov(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	if (pNode->nType != FXNODE_TYPE_HELPER)
	{
		return -1.0F;
	}

	if (NULL == pNode->pCamera)
	{
		return -1.0F;
	}

	return pNode->pCamera->fFov;
}

float CModelPlayer::GetCameraNodeFarClip(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	if (pNode->nType != FXNODE_TYPE_HELPER)
	{
		return -1.0F;
	}

	if (NULL == pNode->pCamera)
	{
		return -1.0F;
	}

	return pNode->pCamera->fFarClip;
}

float CModelPlayer::GetCameraNodeNearClip(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	if (pNode->nType != FXNODE_TYPE_HELPER)
	{
		return -1.0F;
	}

	if (NULL == pNode->pCamera)
	{
		return -1.0F;
	}

	return pNode->pCamera->fNearClip;
}

bool CModelPlayer::IsAttachedNode(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	if (pNode->pszAttachedHelperParentName != NULL)
	{
		return true;
	}

	return false;
}

const char* CModelPlayer::GetAttachedParentName(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	return pNode->pszAttachedHelperParentName;
}

void CModelPlayer::GetCurrentNodeTM(void* node, FmMat4* mtxTM)
{
	Assert(mtxTM != NULL);
	Assert(node != NULL);

	if (m_nState == MODEL_STATE_INVALID)
	{
        //Assert(m_nState != MODEL_STATE_INVALID);
        FmMatrixIdentity(mtxTM); 
		return;
	}

	model_node_t* pNode = (model_node_t*)node;
	int id = pNode->nIndex;
	
	Assert(id >= 0);

	if (pNode->pszAttachedHelperParentName != NULL)
	{
		if (NULL == m_pActionPlayer)
		{
			FmMatrixIdentity(mtxTM);
		}
		else
		{
			const char* pszParentName = GetAttachedParentName(node);

			Assert(pszParentName != NULL);

			skt_pose_t* pose = m_pActionPlayer->GetCurrentPose();
			skt_node_t* pParentNode = get_skeleton_node_from_name(
				pose->pSkeleton, pszParentName);

			if (NULL == pParentNode)
			{
				//FXLOG_ERROR("[error 030034] Not find bone %s in file %s\n", 
				//	pszParentName, sa->pInstance->pszName);
				FmMatrixIdentity(mtxTM);
				return;
			}

			if (NULL == pose->pCurrentNodeTMList)
			{
				FmMatrixIdentity(mtxTM);
				return;
			}

			FmMat4 mtxNodeTM = pose->pCurrentNodeTMList[pParentNode->nIndex];

			//FmMat4 mtxNodeTM;
			//FXGetCurrentNodeTM(ref, pParentNode, &mtxNodeTM);
			//FXSAMODEL_GetCurrentNodeTM(sa, pParentNode, &mtxNodeTM);
			//model_node_t* pNode = (model_node_t*)node;

			Assert(pNode->pLocation != NULL);

			math_location_to_matrix(pNode->pLocation, mtxTM);
			FmMatrixMultiply(mtxTM, mtxTM, &mtxNodeTM);
		}
	}
	else
	{
		*mtxTM = m_mtxNodeCurrentTM[id];

		FmMat4 mtxTemp;

		FmMat4Inverse(&mtxTemp, NULL, &m_mtxWorldTM);
		FmMat4Multiply(mtxTM, mtxTM, &mtxTemp);
	}
}

void CModelPlayer::GetCurrentTM(FmMat4* mtxTM)
{
	Assert(mtxTM != NULL);

	*mtxTM = m_mtxCurrentTM;
}

bool CModelPlayer::ExistMainModel()
{
	model_t* pModel = m_pResModel->GetModelData();
	
	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::ExistMainModel] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	unsigned int node_num = pModel->nNodeCount;
	model_node_t** node_list = pModel->pNodeList;

	for (unsigned int i = 0; i < node_num; ++i)
	{
		model_node_t* pNode = node_list[i];

		if ((FXNODE_TYPE_SKIN == pNode->nType) 
			&& (pNode->nModelInfo & MODEL_MAINMODEL_FLAG))
		{
			return true;
		}
	}

	return false;
}

void CModelPlayer::SetTraceMainModel(bool value)
{
	m_bTraceMainModel = value;
}

bool CModelPlayer::GetTraceMainModel()
{
	return m_bTraceMainModel;
}

void CModelPlayer::SetShowNode(void* node, bool bShow)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	pNode->bHide = !bShow;
}

bool CModelPlayer::GetShowNode(void* node)
{
	Assert(node != NULL);

	model_node_t* pNode = (model_node_t*)node;

	return !pNode->bHide;
}

static bool set_material_cur_lod_level(node_material_t* pMat, 
	unsigned int lod_level)
{
	Assert(pMat != NULL);

	pMat->nCurLODLevel = lod_level;

	SAFE_RELEASE(pMat->pGPUIB);

	return true;
}

static bool set_node_cur_lod_level(model_node_t* pNode, unsigned int lod_level)
{
	Assert(pNode != NULL);

	for (unsigned int i = 0; i < pNode->nMaterialCount; ++i)
	{
		node_material_t* pMat = (node_material_t*)&pNode->Materials[i];

		set_material_cur_lod_level(pMat, lod_level);
	}

	for (unsigned int child = 0; child < pNode->nChildNodeCount; ++child)
	{
		set_node_cur_lod_level(&pNode->ChildNodes[child], lod_level);
	}

	return true;
}

void CModelPlayer::SetCurLODLevel(unsigned int lod_level)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetCurLODLevel] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	if (lod_level >= pModel->nLODCount)
	{
		return;
	}

	pModel->nCurLODLevel = lod_level;

	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		set_node_cur_lod_level(&pModel->RootNodes[i], lod_level);
	}
}

unsigned int CModelPlayer::GetCurLODLevel()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetCurLODLevel] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0;
	}

	return pModel->nCurLODLevel;
}

unsigned int CModelPlayer::GetLODLevels()
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetLODLevels] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0;
	}

	return pModel->nLODCount;
}

void CModelPlayer::SetSelectHelper(int nHelper)
{
	m_nSelectHelper = nHelper;
}

int CModelPlayer::GetSelectHelper()
{
	return m_nSelectHelper;
}

void* CModelPlayer::GetMaterialFromID(int iMatIndex)
{
	Assert(iMatIndex >= 0 && iMatIndex < (int)GetMaterialCount());

	model_t* pModel = m_pResModel->GetModelData();
	
	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetMaterialFromID] pModel == NULL model name is %s", m_pResModel->GetName());
		return NULL;
	}

	int count = 0;

	for (unsigned int i = 0; i < pModel->nNodeCount; ++i)
	{
		model_node_t* pNode = pModel->pNodeList[i];

		for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
		{
			if (count == iMatIndex)
			{
				return &pNode->Materials[k];
			}

			++count;
		}
	}

	return NULL;
}

void* CModelPlayer::GetMaterialFromName(const char* pszMatName)
{
	Assert(pszMatName != NULL);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetMaterialFromName] pModel == NULL model name is %s", m_pResModel->GetName());
		return NULL;
	}

	for (unsigned int i = 0; i < pModel->nNodeCount; ++i)
	{
		model_node_t* pNode = (model_node_t*)pModel->pNodeList[i];

		for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
		{
			node_material_t* pMat = (node_material_t*)&pNode->Materials[k];

			if (strcmp(pMat->pszName, pszMatName) == 0)
			{
				return pMat;
			}
		}
	}

	return NULL;
}

const char* CModelPlayer::GetMaterialName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->pszName;
}

unsigned int CModelPlayer::GetMaterialTextureCount(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	unsigned int ret = 0;

	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_BUMP_MAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_MAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_LEVEL_MAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_GLOSSINESS_MAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_REFLECTION_ENV_MAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO)
	{
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_SELF_ILLUMINATION_INFO)
	{
		ret++;
	}

	return ret;
}

const char* CModelPlayer::GetMaterialTextureName(void* mat, unsigned int index)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	unsigned int ret = 0;

	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.DiffuseMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_BUMP_MAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.BumpMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_MAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.SpecularMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_LEVEL_MAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.SpecularLevelMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_GLOSSINESS_MAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.GlossinessMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_REFLECTION_ENV_MAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.ReflectionMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.LightMap.pFileName;
		}
		ret++;
	}

	if (pMat->nMaterialInfo & MATERIAL_SELF_ILLUMINATION_INFO)
	{
		if (index == ret)
		{
			return pMat->MatInfo.EmissiveMap.pFileName;
		}
		ret++;
	}

	return "";
}

void* CModelPlayer::GetMaterialVertexBuffer(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	//return pMat->pSingleVB;
	return &(pMat->SingleVB);
}

void* CModelPlayer::GetMaterialIndexBuffer(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->pLODIB[pMat->nCurLODLevel];
}

unsigned int CModelPlayer::GetMaterialVertexSize(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->nSingleVertexSize;
}

unsigned int CModelPlayer::GetMaterialVertexCount(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->nVertexCount;
}

unsigned int CModelPlayer::GetMaterialIndexSize(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return sizeof(unsigned short);
}

unsigned int CModelPlayer::GetMaterialIndexCount(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->pLODIndexCount[pMat->nCurLODLevel];
}

//bool CModelPlayer::UpdateLightmap(void* mat)
//{
//	Assert(mat != NULL);
//
//	node_material_t* pMat = (node_material_t*)mat;
//
//	return false;
//}

int CModelPlayer::GetPositionOffset(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	model_node_t* pNode = (model_node_t*)pMat->pNode;

	if ((0 == (pNode->nModelInfo & MODEL_POSITION_INFO)) 
		&& (0 == (pNode->nModelInfo & MODEL_POSITION_ANI)))
	{
		return -1;
	}

	return 0;
}

int CModelPlayer::GetNormalOffset(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	model_node_t* pNode = (model_node_t*)pMat->pNode;

	if ((0 == (pNode->nModelInfo & MODEL_NORMAL_INFO)) 
		&& (0 == (pNode->nModelInfo & MODEL_NORMAL_ANI)))
	{
		return -1;
	}

	unsigned short uStream0Offset = 0;
	unsigned short uStream1Offset = 0;

	// Position 
	if (pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_POSITION_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Normal
	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		return uStream0Offset;
	}
	else
	{
		return uStream1Offset;
	}
}

int CModelPlayer::GetDiffuseOffset(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	model_node_t* pNode = (model_node_t*)pMat->pNode;

	if ((0 == (pNode->nModelInfo & MODEL_COLOR_INFO)) 
		&& (0 == (pNode->nModelInfo & MODEL_COLOR_ANI)))
	{
		return -1;
	}

	unsigned short uStream0Offset = 0;
	unsigned short uStream1Offset = 0;

	// Position 
	if (pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_POSITION_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Normal
	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_NORMAL_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Color
	if (pNode->nModelInfo & MODEL_COLOR_INFO)
	{
		return uStream0Offset;
	}
	else
	{
		return uStream1Offset;
	}
}

// 获得指定顶点缓冲的贴图的UV数据偏移
int CModelPlayer::GetUVOffset(void* mat)
{
	node_material_t* pMat = (node_material_t*)mat;
	model_node_t* pNode = (model_node_t*)pMat->pNode;

	// Diffuse texture
	if (!(pMat->nMaterialInfo & (MATERIAL_DIFFUSE_MAP_INFO | MATERIAL_BUMP_MAP_INFO
		| MATERIAL_SPECULAR_MAP_INFO | MATERIAL_SELF_ILLUMINATION_INFO)))
		return -1;

	unsigned short uStream0Offset = 0;
	unsigned short uStream1Offset = 0;

	// Position 
	if (pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_POSITION_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Normal
	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_NORMAL_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Color
	if (pNode->nModelInfo & MODEL_COLOR_INFO)
	{
		uStream0Offset += sizeof(unsigned long);
	}
	else if (pNode->nModelInfo & MODEL_COLOR_ANI)
	{
		uStream1Offset += sizeof(unsigned long);
	}

	// Illum
	if (pNode->nModelInfo & MODEL_ILLUM_INFO)
	{
		uStream0Offset += sizeof(float);
	}
	else if (pNode->nModelInfo & MODEL_ILLUM_ANI)
	{
		uStream1Offset += sizeof(float);
	}

	// Tangent
	if (pNode->nModelInfo & MODEL_TANGENT_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_TANGENT_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Binormal
	if (pNode->nModelInfo & MODEL_BINORMAL_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_BINORMAL_ANI)
	{
		uStream1Offset += sizeof(float) * 3;
	}

	// Diffuse texture
	if (pMat->nMaterialInfo & (MATERIAL_DIFFUSE_MAP_INFO | MATERIAL_BUMP_MAP_INFO
		| MATERIAL_SPECULAR_MAP_INFO | MATERIAL_SELF_ILLUMINATION_INFO))
	{
		return uStream0Offset;
	}
	else
	{
		return uStream1Offset;
	}
}

int CModelPlayer::GetLightmapUVOffset(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	model_node_t* pNode = (model_node_t*)pMat->pNode;

	if (0 == (pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO))
	{
		return -1;
	}

	if (FXNODE_TYPE_MESH != pNode->nType)
	{
		return -1;
	}

	unsigned short uStream0Offset = 0;

	// Position 
	if (pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}

	// Normal
	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}

	// Color
	if (pNode->nModelInfo & MODEL_COLOR_INFO)
	{
		uStream0Offset += sizeof(unsigned int);
	}

	// Illum
	if (pNode->nModelInfo & MODEL_ILLUM_INFO)
	{
	}

	// Tangent
	if (pNode->nModelInfo & MODEL_TANGENT_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}

	// Binormal
	if (pNode->nModelInfo & MODEL_BINORMAL_INFO)
	{
		uStream0Offset += sizeof(float) * 3;
	}

	// Texcoord
	if (pMat->nMaterialInfo & (MATERIAL_DIFFUSE_MAP_INFO | MATERIAL_BUMP_MAP_INFO
		| MATERIAL_SPECULAR_MAP_INFO | MATERIAL_SELF_ILLUMINATION_INFO))
	{
		uStream0Offset += sizeof(float) * 2;
	}

	// Lightmap Texcoord
	return uStream0Offset;
}

const char* CModelPlayer::GetTexDiffuseFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.DiffuseMap.pFileName;
}

const char* CModelPlayer::GetTexBumpFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.BumpMap.pFileName;
}

const char* CModelPlayer::GetTexSpecularmapFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.SpecularMap.pFileName;
}

const char* CModelPlayer::GetTexSpecularlevelmapFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.SpecularLevelMap.pFileName;
}

const char* CModelPlayer::GetTexGlossinessmapFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.GlossinessMap.pFileName;
}

const char* CModelPlayer::GetTexReflectionmapFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.ReflectionMap.pFileName;
}

const char* CModelPlayer::GetTexLightmapFileName(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;

	return pMat->MatInfo.LightMap.pFileName;
}

unsigned int CModelPlayer::GetMaterialAmbient(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	FmVec3 ambient_color(pMat->MatInfo.vAmbient * 255);
	unsigned int ambient = 0xFF000000 
		+ ((int(ambient_color.x) & 0xFF) << 16) 
		+ ((int(ambient_color.y) & 0xFF) << 8) 
		+ (int(ambient_color.z) & 0xFF);

	return ambient;
}

unsigned int CModelPlayer::GetMaterialDiffuse(void* mat)
{
	Assert(mat != NULL);

	node_material_t* pMat = (node_material_t*)mat;
	FmVec3 diffuse_color(pMat->MatInfo.vDiffuse * 255);

	unsigned int diffuse = 0xFF000000 
		+ ((int(diffuse_color.x) & 0xFF) << 16) 
		+ ((int(diffuse_color.y) & 0xFF) << 8) 
		+ (int(diffuse_color.z) & 0xFF);

	return diffuse;
}

const char* CModelPlayer::GetModelFileName()
{
	return m_pResModel->GetName();
}

unsigned int CModelPlayer::GetHelperCount()
{
	unsigned int nRet = 0;
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetHelperCount] pModel == NULL model name is %s", m_pResModel->GetName());
		return nRet;
	}

	for (unsigned int i = 0; i < pModel->nNodeCount; ++i)
	{
		if (FXNODE_TYPE_HELPER == pModel->pNodeList[i]->nType)
		{
			nRet++;
		}
	}

	return nRet;
}

const char* CModelPlayer::GetHelperIDFromIndex(unsigned int nIndex)
{
	unsigned int num = 0;
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetHelperIDFromIndex] pModel == NULL model name is %s", m_pResModel->GetName());
		return NULL;
	}

	for (unsigned int i = 0; i < pModel->nNodeCount; ++i)
	{
		if (FXNODE_TYPE_HELPER == pModel->pNodeList[i]->nType)
		{
			if (num == nIndex)
			{
				return pModel->pNodeList[i]->pszName;
			}

			num++;
		}
	}

	return NULL;
}

void CModelPlayer::SetEnableFallOff(bool value)
{
	m_bFallOff = value;
}

bool CModelPlayer::GetEnableFallOff()
{
	return m_bFallOff;
}

void CModelPlayer::SetEnablePointLighting(bool value)
{
	m_bPointLighting = value;
}

bool CModelPlayer::GetEnablePointLighting()
{
	return m_bPointLighting;
}
