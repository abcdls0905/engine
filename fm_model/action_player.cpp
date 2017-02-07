//--------------------------------------------------------------------
// 文件名:		action_player.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月18日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_player.h"
#include "skeleton.h"
#include "action_set.h"
#include "action_data.h"
#include "res_manager.h"
#include "location.h"
#include "math_3d.h"
#include "../visual/i_render.h"
#include "../visual/vis_utils.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

extern IRender* g_pRender;

// 获得当前时间秒数
static inline float get_current_seconds()
{
	return (float)g_pRender->GetCore()->GetTotalSeconds();
}

// 获得节点矩阵
static void get_node_tm(skt_pose_t* pose, skt_node_t* node, 
	FmMat4* mtxTM)
{
	*mtxTM = pose->pCurrentNodeTMList[node->nIndex];
}

// 获得当前帧
static int get_current_frame(skeleton_t* pInst, 
	blend_node_t* pBlendNode, float speed)
{
	int nFrameCount = get_skeleton_frame_count(pInst);

	if (nFrameCount > 0)
	{
		// 必须是当前时间对应的帧数，即使骨骼位置还未到达
		float fCurSktTime = get_current_seconds() - pBlendNode->fBlendTime;
		unsigned int nFrame;
		float fFactor;

		math_get_frame_lerp(nFrameCount, speed * pInst->fInternalFPS, 
			fCurSktTime, pBlendNode->bLoop, &nFrame, &fFactor);
		nFrame += 3;
		return nFrame;
	}
	else
	{
		return pInst->nEndFrame - pInst->nStartFrame;
	}
}

// 初始化骨骼修正信息
static void init_change_info(skt_pose_t* pose)
{
	skeleton_t* pInst = pose->pSkeleton;

	if (pose->pChangeNodeTMList)
	{
		CORE_FREE(pose->pChangeNodeTMList, 
			sizeof(FmMat4) * pInst->nTotalNodeCount);
	}

	pose->pChangeNodeTMList = (FmMat4*)CORE_ALLOC(
		sizeof(FmMat4) * pInst->nTotalNodeCount);

	for (unsigned int i = 0; i < pInst->nTotalNodeCount; ++i)
	{
		FmMatrixIdentity(&pose->pChangeNodeTMList[i]);
	}
}

// 初始化骨骼为RAGDOLL信息
static void init_ragdoll_info(skt_pose_t* pose)
{
	skeleton_t* pInst = pose->pSkeleton;

	if (pose->pCurrentNodeUserResetList)
	{
		CORE_FREE(pose->pCurrentNodeUserResetList, 
			sizeof(bool) * pInst->nTotalNodeCount);
	}

	pose->pCurrentNodeUserResetList = (bool*)CORE_ALLOC(
		sizeof(bool) * pInst->nTotalNodeCount);

    memset(pose->pCurrentNodeUserResetList, 0, 
        sizeof(sizeof(bool) * pInst->nTotalNodeCount));
}

static skt_pose_t* create_skeleton_pose()
{
	skt_pose_t* pose = (skt_pose_t*)CORE_ALLOC(sizeof(skt_pose_t));

	memset(pose, 0, sizeof(skt_pose_t));
	FmMatrixIdentity(&pose->mtxCurrentTM);
	pose->fScale = 1.0F;
	pose->pSkeleton = (skeleton_t*)CORE_ALLOC(sizeof(skeleton_t));
	memset(pose->pSkeleton, 0, sizeof(skeleton_t));

	return pose;
}

static void release_skeleton_pose(skt_pose_t* pose)
{
	if (pose->pCurrentNodeTMList)
	{
		CORE_FREE(pose->pCurrentNodeTMList, 
			sizeof(FmMat4) * pose->pSkeleton->nTotalNodeCount);
	}

	if (pose->pChangeNodeTMList)
	{
		CORE_FREE(pose->pChangeNodeTMList, 
			sizeof(FmMat4) * pose->pSkeleton->nTotalNodeCount);
	}

    if (pose->pCurrentNodeUserResetList)
    {
        CORE_FREE(pose->pCurrentNodeUserResetList, 
            sizeof(bool) * pose->pSkeleton->nTotalNodeCount);
    }

	if (pose->pSkeleton != NULL)
	{
		unload_skeleton(pose->pSkeleton);
	}

	CORE_FREE(pose, sizeof(skt_pose_t));
}

// CActionPlayer

CActionPlayer* CActionPlayer::NewInstance(CActionSet* pActionSet)
{
	CActionPlayer* p = (CActionPlayer*)CORE_ALLOC(
		sizeof(CActionPlayer));
	
	new (p) CActionPlayer(pActionSet);

	return p;
}

CActionPlayer::CActionPlayer(CActionSet* pActionSet)
{
	Assert(pActionSet != NULL);
	
	m_pActionSet = pActionSet;
	m_bAsyncLoad = true;
	m_bCreated = false;
	m_fSpeed = 1.0F;
	m_pTriggerProc = NULL;
	m_pUserPointer = NULL;
	m_fLoopNotCriticalBlendTimer = 0.0F;
	m_nSequenceId = 0;
	m_bOptimizeBySequenceId = true;
	//m_nKChainCnt = 0;
	//m_pKList = NULL;
	//m_pKContext = NULL;
	m_pParentAction = NULL;
	m_pChildAction = NULL;
	m_pCurrentPose = create_skeleton_pose();
	FmMatrixIdentity(&m_mtxWorld);
	//FmMatrixIdentity(&m_mtxWorldPrev);
	m_pBlendNodeList = array_create(sizeof(blend_node_t));
    m_pNotCriticalBlendNodeList = array_create(sizeof(not_critical_t));
    m_bUserBoneEnable = false;
}

CActionPlayer::~CActionPlayer()
{
	if (m_pBlendNodeList != NULL)
	{
		ClearBlendAction();
		array_release(m_pBlendNodeList);
		m_pBlendNodeList = NULL;
	}

	if (m_pCurrentPose != NULL)
	{
		release_skeleton_pose(m_pCurrentPose);
		m_pCurrentPose = NULL;
	}

	if (m_pNotCriticalBlendNodeList != NULL)
	{
		array_release(m_pNotCriticalBlendNodeList);
		m_pNotCriticalBlendNodeList = NULL;
	}

	//if (m_pKContext)
	//{
	//	delete m_pKContext;
	//	m_pKContext = NULL;
	//}

	//if (m_pKList)
	//{
	//	delete[] m_pKList;
	//	m_pKList = NULL;
	//}

	m_pActionSet->Release();
}

void CActionPlayer::Release()
{
	CORE_DELETE(this);
}

bool CActionPlayer::IsReady()
{
	if (!m_pActionSet->IsReady())
	{
		return false;
	}

	return true;
}

bool CActionPlayer::IsLoadComplete()
{
	if (!m_pActionSet->IsLoadComplete())
	{
		return false;
	}

	if (!m_bCreated)
	{
		Create();
	}
	
	return true;
}

bool CActionPlayer::Create()
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	if (NULL == pActionSet)
	{
		return false;
	}
	
	skeleton_t* pMainSkeleton = pActionSet->pMainSkeleton->GetSkeletonData();

	Assert(pMainSkeleton != NULL);

	// 初始动作混合结果的数据结构
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;
	
	if (m_pCurrentPose->pCurrentNodeTMList)
	{
		CORE_FREE(m_pCurrentPose->pCurrentNodeTMList, 
			sizeof(FmMat4) * pInst->nTotalNodeCount);
		m_pCurrentPose->pCurrentNodeTMList = NULL;
	}

	copy_skeleton_structure(pInst, pMainSkeleton);

	//if (m_pCurrentPose->pCurrentNodeTMList)
	//{
	//	delete[] m_pCurrentPose->pCurrentNodeTMList;
	//	m_pCurrentPose->pCurrentNodeTMList = NULL;
	//}

	if (pMainSkeleton->nTotalNodeCount != 0)
	{
		m_pCurrentPose->pCurrentNodeTMList = (FmMat4*)CORE_ALLOC(
			sizeof(FmMat4) * pMainSkeleton->nTotalNodeCount);
		memset(m_pCurrentPose->pCurrentNodeTMList, 0, 
			sizeof(FmMat4) * pMainSkeleton->nTotalNodeCount);
	}

	// 初始化主骨架位置信息
	FmMat4 mtxCurrentTM;

	FmMatrixIdentity(&mtxCurrentTM);

	for (int i = 0; i < (int)pInst->nRootNodeCount; ++i)
	{
		init_skeleton_pose_inverse_matrix(&pInst->RootNodes[i], &mtxCurrentTM);
	}

	// 设置初始动作
	update_skeleton(m_pCurrentPose->pSkeleton, 
		m_pCurrentPose->pCurrentNodeTMList, 
		m_pCurrentPose->pChangeNodeTMList, 
		m_pCurrentPose->pParentPose); 

	//IntializeKineticsChain();

	// 必须在播放缺省动作之前设置创建完成 
	m_bCreated = true;

	if (!m_strDefaultAction.empty())
	{
		// 开始播放缺省动作
		int action_index = this->GetActionIndex(m_strDefaultAction.c_str());

		if (action_index >= 0)
		{
			this->BlendAction(action_index, true, false, true, 1.0F, "", "");
		}
	}

	return true;
}

void CActionPlayer::SetDefaultAction(const char* value)
{
	m_strDefaultAction = value;
}

const char* CActionPlayer::GetDefaultAction()
{
	return m_strDefaultAction.c_str();
}

bool CActionPlayer::SetSpeed(float fSpeed)
{
	if (fSpeed <= 0.0F)
	{
		//FXLOG_ERROR("[error 030023] FXACTION_SetSpeed:Can't set the action's FPS = 0.0\n");
		return false;
	}

	if (FloatEqual(m_fSpeed, fSpeed))
	{
		return true;
	}
	
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	int uBlendCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < uBlendCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction < 0)
		{
			continue;
		}

		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, pBlendNode->nAction);
		skeleton_t* pSkt = pActionNode->pSkeleton->GetSkeletonData();

		if (NULL == pSkt)
		{
			continue;
		}

		// 使用双精度浮点数防止误差
		double internal_fps = pSkt->fInternalFPS;
		double old_fps = m_fSpeed * pBlendNode->fSpeed * internal_fps;
		double new_fps = fSpeed * pBlendNode->fSpeed * internal_fps;
		double frames = old_fps * pBlendNode->fCurrentTime;
		double cur_time = double(pBlendNode->fBlendTime) 
			+ double(pBlendNode->fCurrentTime);

		pBlendNode->fCurrentTime = float(frames / new_fps);
		pBlendNode->fBlendTime = float(cur_time - pBlendNode->fCurrentTime);

		if (!FloatEqual(pBlendNode->fUnblendTime, 0.0F))
		{
			frames = old_fps * (cur_time - pBlendNode->fUnblendTime);
			pBlendNode->fUnblendTime = float(cur_time - frames / new_fps);
		}
	}

	m_fSpeed = fSpeed;

	return true;
}

float CActionPlayer::GetSpeed()
{
	return m_fSpeed;
}

bool CActionPlayer::SetActionSpeed(int action_index, float fSpeed)
{
	if (fSpeed <= 0.0F)
	{
		//FXLOG_ERROR("[error 030023] FXACTION_SetSpeed:Can't set the action's FPS = 0.0\n");
		return false;
	}
	
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
		
	pActionNode->fSpeed = fSpeed;

	return true;
}

float CActionPlayer::GetActionSpeed(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->fSpeed;
}

void CActionPlayer::SetWorldMatrix(const FmMat4* mtxWorld)
{
	//if (memcmp(&m_mtxWorldPrev, &m_mtxWorld, sizeof(FmMat4)) != 0)
	//{
	//	m_mtxWorldPrev = m_mtxWorld;
	//}

	m_mtxWorld = *mtxWorld;
}

void CActionPlayer::GetWorldMatrix(FmMat4* mtxWorld)
{
	*mtxWorld = m_mtxWorld;
}

int CActionPlayer::GetActionCount()
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	if (NULL == pActionSet)
	{
		return 0;
	}

	return array_get_size(pActionSet->pActionNodeList);
}

int CActionPlayer::GetActionIndex(const char* pszName)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	if (NULL == pActionSet)
	{
		return -1;
	}

	int action_index = m_pActionSet->GetActionIndex(pszName);

	if (action_index >= 0)
	{
		return action_index;
	}
	
	return -1;
}

const char* CActionPlayer::GetActionName(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->pszActionName;
}

bool CActionPlayer::SetActionName(int action_index, const char* pszName)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	// 修改索引
	m_pActionSet->RemoveActionIndex(pActionNode->pszActionName, 
		action_index);
	m_pActionSet->AddActionIndex(pszName, action_index);

	set_action_node_string(pActionNode, pszName, pActionNode->pszSkeletonName, 
		pActionNode->pszActionFileName);

	return true;
}

bool CActionPlayer::SetActionEnterTime(int action_index, float fTime)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->fEnterTime = fTime;

	return true;
}

float CActionPlayer::GetActionEnterTime(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->fEnterTime;
}

bool CActionPlayer::SetActionLeaveTime(int action_index, float fTime)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->fLeaveTime = fTime;

	return true;
}

float CActionPlayer::GetActionLeaveTime(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->fLeaveTime;
}

bool CActionPlayer::SetActionBaseSpeed(int action_index, float fBaseSpeed)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->fBaseSpeed = fBaseSpeed;

	return true;
}

float CActionPlayer::GetActionBaseSpeed(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->fBaseSpeed;
}

bool CActionPlayer::SetActionTranslate(int action_index, int nTranslate)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}
	
	pActionNode->nTranslate = nTranslate;
	
	return true;
}

int CActionPlayer::GetActionTranslate(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}
	
	return pActionNode->nTranslate;
}

bool CActionPlayer::SetActionStartFrame(int action_index, int frame)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}

	pActionNode->nStartFrame = frame;

	return true;
}

int CActionPlayer::GetActionStartFarme(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return 0;
	}

	return pActionNode->nStartFrame;
}

bool CActionPlayer::SetActionEndFrame(int action_index, int frame)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}

	pActionNode->nEndFrame = frame;

	return true;
}

int CActionPlayer::GetActionEndFarme(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return 0;
	}

	return pActionNode->nEndFrame;
}

bool CActionPlayer::SetActionFootstep(int action_index, int footstep_num,
	int* footsteps)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}

	if (pActionNode->pFootsteps)
	{
		CORE_FREE(pActionNode->pFootsteps, 
			sizeof(int) * pActionNode->nFootstepNum);
	}

	if (footstep_num > 0)
	{
		pActionNode->nFootstepNum = footstep_num;
		pActionNode->pFootsteps = (int*)CORE_ALLOC(sizeof(int) * footstep_num);
		memcpy(pActionNode->pFootsteps, footsteps, footstep_num * sizeof(int));
	}
	else
	{
		pActionNode->nFootstepNum = 0;
		pActionNode->pFootsteps = NULL;
	}

	return true;
}

int CActionPlayer::GetActionFootstepCount(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return 0;
	}

	return pActionNode->nFootstepNum;
}

int CActionPlayer::GetActionFootstepFrame(int action_index, int step_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return 0;
	}

	Assert(size_t(step_index) < size_t(pActionNode->nFootstepNum));

	return pActionNode->pFootsteps[step_index];
}

float CActionPlayer::GetActionScale(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
		
	return pActionNode->fScale;
}

const char* CActionPlayer::GetActionSkeletonName(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->pszSkeletonName;
}

skeleton_t* CActionPlayer::GetActionSkeleton(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	if (NULL == pSkeleton)
	{
		return NULL;
	}
		
	skeleton_t* pSkt = pSkeleton->GetSkeletonData();
		
	if (NULL == pSkt)
	{
		return NULL;
	}
	
	return pSkt;
}

bool CActionPlayer::InnerLoadAction(int action_index, bool update_last_use)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (pActionNode->pSkeleton != NULL)
	{
		// 已经加载
		return true;
	}

	CResManager* pManager = m_pActionSet->GetManager();

	pActionNode->pSkeleton = pManager->CreateSkeleton(
		pActionNode->pszSkeletonName, m_bAsyncLoad);

	if (NULL == pActionNode->pSkeleton)
	{
		return false;
	}

	if (update_last_use)
	{
		m_pActionSet->UpdateActionLastUse(action_index);
	}
	
	return true;
}

bool CActionPlayer::LoadAction(int action_index)
{
	return InnerLoadAction(action_index, true);
//	action_set_t* pActionSet = m_pActionSet->GetActionSet();
//
//	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));
//
//	action_node_t* pActionNode = (action_node_t*)array_get(
//		pActionSet->pActionNodeList, action_index);
//
//	if (pActionNode->pSkeleton != NULL)
//	{
//		// 已经加载
//		return true;
//	}
//
//	CResManager* pManager = m_pActionSet->GetManager();
//
//	pActionNode->pSkeleton = pManager->CreateSkeleton(
//		pActionNode->pszSkeletonName, m_bAsyncLoad);
//
//	if (NULL == pActionNode->pSkeleton)
//	{
//		return false;
//	}
//
//	return true;
}

bool CActionPlayer::GetActionLoaded(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	if (NULL == pSkeleton)
	{
		return false;
	}

	return pSkeleton->IsReady();
}

bool CActionPlayer::GetActionLoadFinish(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	if (NULL == pSkeleton)
	{
		return false;
	}

	return pSkeleton->IsLoadComplete();
}

void CActionPlayer::RegistActionTriggerProc(ACTION_TRIGGER_PROC proc)
{
	m_pTriggerProc = proc;
}

void CActionPlayer::UnregistActionTriggerProc()
{
	m_pTriggerProc = NULL;
}

void CActionPlayer::SetUserPointer(void* pUserPointer)
{
	m_pUserPointer = pUserPointer;
}

void* CActionPlayer::GetUserPointer()
{
	return m_pUserPointer;
}

void CActionPlayer::GetActionTriggerProc(ACTION_TRIGGER_PROC* pProc)
{
	*pProc = m_pTriggerProc;
}

int CActionPlayer::GetActionTriggerCount(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode->pTriggerList)
	{
		return 0;
	}

	return array_get_size(pActionNode->pTriggerList);
}

const char* CActionPlayer::GetActionTriggerName(int action_index, 
	int nTrigger)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode->pTriggerList)
	{
		return "";
	}
	
	action_trigger_t* pTrigger = (action_trigger_t*)array_get(
		pActionNode->pTriggerList, nTrigger);
		
	return pTrigger->pszName;
}

int CActionPlayer::GetActionTriggerFrame(int action_index, int nTrigger)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode->pTriggerList)
	{
		return 0;
	}

	action_trigger_t* pTrigger = (action_trigger_t*)array_get(
		pActionNode->pTriggerList, nTrigger);
		
	return pTrigger->nFrame;
}

int CActionPlayer::AddActionTrigger(int action_index, 
	const char* pszTriggerName, int nFrame, const char* pszString)
{
	Assert(pszTriggerName != NULL);

	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode->pTriggerList)
	{
		pActionNode->pTriggerList = array_create(sizeof(action_trigger_t));
	}

	int nTriggerCnt = array_get_size(pActionNode->pTriggerList);

	for (int i = 0; i < nTriggerCnt; ++i)
	{
		action_trigger_t* pTrigger = (action_trigger_t*)array_get(
			pActionNode->pTriggerList, i);

		Assert(pTrigger != NULL);

		if (pTrigger->pszName)
		{
			if ((pTrigger->nFrame == nFrame) 
				&& (strcmp(pTrigger->pszName, pszTriggerName) == 0))
			{
				return -1;
			}
		}
	}

	action_trigger_t trigger;

	memset(&trigger, 0, sizeof(trigger));
	set_trigger_name_string(&trigger, pszTriggerName, pszString);
	trigger.nFrame = nFrame;

	array_push_back(pActionNode->pTriggerList, &trigger);

	return array_get_size(pActionNode->pTriggerList) - 1;
}

bool CActionPlayer::DeleteActionTrigger(int action_index, 
	const char* pszTriggerName, int nFrame)
{
	Assert(pszTriggerName != NULL);

	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}

	if (NULL == pActionNode->pTriggerList)
	{
		return false;
	}

	int uSize = array_get_size(pActionNode->pTriggerList);

	for (int i = 0; i < uSize; ++i)
	{
		action_trigger_t* pTrigger = (action_trigger_t*)array_get(
			pActionNode->pTriggerList, i);

		Assert(pTrigger != NULL);

		if (pTrigger->pszName)
		{
			if ((pTrigger->nFrame == nFrame) 
				&& (strcmp(pTrigger->pszName, pszTriggerName) == 0))
			{
				if (pTrigger->pszName)
				{
					CORE_FREE(pTrigger->pszName, pTrigger->nNameSize);
					pTrigger->pszName = NULL;
				}

				array_remove(pActionNode->pTriggerList, i);
				return true;
			}
		}
	}

	return false;
}

bool CActionPlayer::SetActionTriggerString(int action_index, 
	const char* pszTriggerName, int nFrame, const char* pszString)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return false;
	}

	if (NULL == pActionNode->pTriggerList)
	{
		return false;
	}

	int uSize = array_get_size(pActionNode->pTriggerList);

	for (int i = 0; i < uSize; ++i)
	{
		action_trigger_t* pTrigger = (action_trigger_t*)array_get(
			pActionNode->pTriggerList, i);

		Assert(pTrigger != NULL);

		if (pTrigger->pszName)
		{
			if ((pTrigger->nFrame == nFrame) 
				&& (strcmp(pTrigger->pszName, pszTriggerName) == 0))
			{
				if (pszString)
				{
					set_trigger_name_string(pTrigger, pTrigger->pszName,
						pszString);
				}
				else
				{
					set_trigger_name_string(pTrigger, pTrigger->pszName, "");
				}

				return true;
			}
		}
	}

	return false;
}

const char* CActionPlayer::GetActionTriggerString(int action_index, 
	const char* pszTriggerName, int nFrame)
{
	Assert(pszTriggerName != NULL);

	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		return "";
	}

	if (NULL == pActionNode->pTriggerList)
	{
		return "";
	}

	int uSize = array_get_size(pActionNode->pTriggerList);

	for (int i = 0; i < uSize; ++i)
	{
		action_trigger_t* pTrigger = (action_trigger_t*)array_get(
			pActionNode->pTriggerList, i);

		Assert(pTrigger != NULL);

		if (pTrigger->pszName)
		{
			if ((pTrigger->nFrame == nFrame) 
				&& (strcmp(pTrigger->pszName, pszTriggerName) == 0))
			{
				return pTrigger->pszString;
			}
		}
	}

	return "";
}

bool CActionPlayer::GetActionStartTriggerNotify(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->bEnableStartTriggerNotify;
}

bool CActionPlayer::SetActionStartTriggerNotify(int action_index, 
	bool bStartTrigger)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->bEnableStartTriggerNotify = bStartTrigger;
	
	return true;
}

bool CActionPlayer::GetActionEndTriggerNotify(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->bEnableEndTriggerNotify;
}

bool CActionPlayer::SetActionEndTriggerNotify(int action_index, bool bEndTrigger)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->bEnableEndTriggerNotify = bEndTrigger;
	
	return true;
}

bool CActionPlayer::GetActionLoopTriggerNotify(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->bEnableLoopTriggerNotify;
}

bool CActionPlayer::SetActionLoopTriggerNotify(int action_index, 
	bool bLoopTrigger)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->bEnableLoopTriggerNotify = bLoopTrigger;
	
	return true;
}

bool CActionPlayer::GetActionBreakTriggerNotify(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	return pActionNode->bEnableBreakTriggerNotify;
}

bool CActionPlayer::SetActionBreakTriggerNotify(int action_index, 
	bool bBreakTrigger)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	pActionNode->bEnableBreakTriggerNotify = bBreakTrigger;
	
	return true;
}

int CActionPlayer::GetBlendActionCount()
{
	return array_get_size(m_pBlendNodeList);
}

const char* CActionPlayer::GetBlendActionName(int uBlendedAction)
{
	Assert(size_t(uBlendedAction) < array_get_size(m_pBlendNodeList));

	blend_node_t* pBlendNode = (blend_node_t*)array_get(
		m_pBlendNodeList, uBlendedAction);

	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(pBlendNode->nAction) 
		< array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, pBlendNode->nAction);

	return pActionNode->pszActionName;
}

bool CActionPlayer::BlendAction(int action_index, bool bLoop, 
	bool bAutoDelete, bool bCritical, float fMaxBlendWeight,
	const char* bone_mask, const char* bone_name)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode->pSkeleton)
	{
		// 加载这个动作
		if (!this->InnerLoadAction(action_index, false))
		{
			return false;
		}
	}

	// If the action is not critical
	if (!bCritical)
	{
		// 移出回收队列
		m_pActionSet->RemoveActionLastUse(action_index);

		// Build new node
		not_critical_t not_critical;

		not_critical.nAction = action_index;
		not_critical.bAutoDelete = bAutoDelete;
		not_critical.bLoop = bLoop;
		not_critical.fMaxBlendWeight = fMaxBlendWeight;
		not_critical.szBoneMask[0] = bone_mask[0];
		not_critical.szBoneMask[1] = 0;
		Assert(strlen(bone_name) < sizeof(not_critical.szBoneName));
		CopyString(not_critical.szBoneName, sizeof(not_critical.szBoneName),
			bone_name);

		array_push_back(m_pNotCriticalBlendNodeList, &not_critical);

		//FXLOG_DEVELOPER("BlendAction %s Loop = %d AutoDelete = %d "
		//	"Critical = %d CurrentActionBlended = %d\n", 
		//	this->GetActionName(action_index), bLoop, bAutoDelete, bCritical, 
		//	array_get_size(m_pBlendNodeList));
		return true;
	}

	// If the action is unblending
	int uBlendCnt = array_get_size(m_pBlendNodeList);

	for (int k = 0; k < uBlendCnt; ++k)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, k);

		if (!pBlendNode->bUnblend)
		{
			continue;
		}

		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		if (k < (uBlendCnt - 1))
		{
			// 将要混合的动作移到混合动作列表的顶部
			blend_node_t old_blend_node = *pBlendNode;

			for (int i = k; i < uBlendCnt - 1; ++i)
			{
				blend_node_t* pNowBlendNode = 
					(blend_node_t*)array_get(m_pBlendNodeList, i);
				blend_node_t* pNextBlendNode = 
					(blend_node_t*)array_get(m_pBlendNodeList, i + 1);

				*pNowBlendNode = *pNextBlendNode;
			}

			blend_node_t* pTopBlendNode = (blend_node_t*)array_get(
				m_pBlendNodeList, uBlendCnt - 1);
				
			*pTopBlendNode = old_blend_node;
			pBlendNode = pTopBlendNode;
		}

		// Restore action.
		float fCurrentTime = get_current_seconds();
		float fBlendWeight;

		if (pActionNode->fLeaveTime > 0.001F)
		{
			fBlendWeight = 1.0F - (fCurrentTime - pBlendNode->fUnblendTime) 
				/ pActionNode->fLeaveTime;
		}
		else
		{
			fBlendWeight = 0.0F;
		}

		pBlendNode->fBlendTime = fCurrentTime 
			- fabs(fBlendWeight * pActionNode->fEnterTime);

		Assert(pBlendNode->fBlendTime < 999999.0F);

		pBlendNode->bLoop = bLoop;
		pBlendNode->bPause = false;
		pBlendNode->bAutoDelete = bAutoDelete;
		pBlendNode->fUnblendTime = 0.0F;
		pBlendNode->bUnblend = false;
		pBlendNode->fMaxBlendWeight = fMaxBlendWeight;
		pBlendNode->nFramePrev = 0;

		if (bAutoDelete)
		{
			int uBlendNodeCnt = array_get_size(m_pBlendNodeList);
			blend_node_t* pBlendNodeLast = (blend_node_t*)array_get(
				m_pBlendNodeList, uBlendNodeCnt - 1);
			blend_node_t bnodeTemp = *pBlendNode;

			*pBlendNode = *pBlendNodeLast;
			*pBlendNodeLast = bnodeTemp;
		}

		// Trigger notify.
		if ((m_pTriggerProc != NULL) 
			&& pActionNode->bEnableStartTriggerNotify)
		{
			m_pTriggerProc(m_pUserPointer, action_index, 
				"TRIGGER_ACTION_START", 0);
		}

		//FXLOG_DEVELOPER("ResumeAction %s Loop = %d AutoDelete = %d "
		//	"Critical = %d CurrentActionBlended = %d\n", 
		//	pActionNode->pszActionName, bLoop, bAutoDelete, bCritical, 
		//	array_get_size(m_pBlendNodeList));

		// First blend.
		//this->Blend();
		return true;
	}

	if (this->IsActionBlended(action_index))
	{
		return false;
	}

	int* pBoneDisable = NULL;
	
	if (bone_mask[0] != 0)
	{
		skeleton_t* pInst = m_pCurrentPose->pSkeleton;
		skt_node_t* pNode = get_skeleton_bone_node_from_name(pInst, bone_name);

		if (pNode)
		{
			size_t bone_num = pInst->nTotalBoneNodeCount;

			pBoneDisable = (int*)CORE_ALLOC(sizeof(int) * bone_num);
			memset(pBoneDisable, 0, sizeof(int) * bone_num);
			
			if (bone_mask[0] == 'o') // "open"
			{
				open_skeleton_bone_node(pInst, pNode, pBoneDisable);
			}
			else if (bone_mask[0] == 'c') // "close"
			{
				close_skeleton_bone_node(pInst, pNode, pBoneDisable);
			}
		}
	}

	// 移出回收队列
	m_pActionSet->RemoveActionLastUse(action_index);

	// Build new node.
	blend_node_t blend_node;

	blend_node.nAction = action_index;
	blend_node.bAutoDelete = bAutoDelete;
	blend_node.fBlendTime = get_current_seconds();
	blend_node.bLoop = bLoop;
	blend_node.bPause = false;
	blend_node.fMaxBlendWeight = fMaxBlendWeight;
	blend_node.fCurrentWeight = 0.0F;
	blend_node.bUnblend = false;
	blend_node.fUnblendTime = 0.0F;
	blend_node.nFramePrev = 0;
	blend_node.fEnterTime = -1.0F;
	blend_node.fLeaveTime = -1.0F;
	blend_node.fCurrentTime = 0.0F;
	blend_node.fLoopTriggerTimer = 0.0F;
	blend_node.fSpeed = pActionNode->fSpeed;
	blend_node.pBoneDisable = pBoneDisable;

	//FXLOG_DEVELOPER("BlendAction %s Loop = %d AutoDelete = %d Critical = %d "
	//	"CurrentActionBlended = %d\n", this->GetActionName(action_index), 
	//	bLoop, bAutoDelete, bCritical, array_get_size(m_pBlendNodeList));

	// Trigger notify.
	if ((m_pTriggerProc != NULL) && pActionNode->bEnableStartTriggerNotify)
	{
		m_pTriggerProc(m_pUserPointer, blend_node.nAction, 
			"TRIGGER_ACTION_START", 0);
	}

	// Push new node to blend node list.
	array_push_back(m_pBlendNodeList, &blend_node);

	// First blend.
	//this->Blend();

	return true;
}

bool CActionPlayer::InsertAction(int nAimAction, int nSrcAction, 
	bool bLoop, float fMaxBlendWeight, const char* bone_mask, 
	const char* bone_name)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(nAimAction) < array_get_size(pActionSet->pActionNodeList));
	Assert(size_t(nSrcAction) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, nSrcAction);
		
	if (NULL == pActionNode)
	{
		return false;
	}

	if (NULL == pActionNode->pSkeleton)
	{
		// 加载此动作
		if (!this->InnerLoadAction(nSrcAction, false))
		{
			return false;
		}
	}

	int uBlendCnt = array_get_size(m_pBlendNodeList);
	int uBlendNode = -1;

	for (int i = 0; i < uBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == nAimAction)
		{
			uBlendNode = i;
		}
		else if (pBlendNode->nAction == nSrcAction)
		{
			return false;
		}
	}

	if (uBlendNode < 0)
	{
		return false;
	}

	// Push new node to blend node list
	blend_node_t new_blend_node;
	
	array_push_back(m_pBlendNodeList, &new_blend_node);

	for (int y = uBlendCnt; y > uBlendNode; y--)
	{
		blend_node_t* pBlendNode1 = (blend_node_t*)array_get(
			m_pBlendNodeList, y);
		blend_node_t* pBlendNode2 = (blend_node_t*)array_get(
			m_pBlendNodeList, y - 1);
			
		memcpy(pBlendNode1, pBlendNode2, sizeof(blend_node_t));
	}

	int* pBoneDisable = NULL;

	if (bone_mask[0] != 0)
	{
		skeleton_t* pInst = m_pCurrentPose->pSkeleton;
		skt_node_t* pNode = get_skeleton_bone_node_from_name(pInst, bone_name);

		if (pNode)
		{
			size_t bone_num = pInst->nTotalBoneNodeCount;

			pBoneDisable = (int*)CORE_ALLOC(sizeof(int) * bone_num);
			memset(pBoneDisable, 0, sizeof(int) * bone_num);

			if (bone_mask[0] == 'o') // "open"
			{
				open_skeleton_bone_node(pInst, pNode, pBoneDisable);
			}
			else if (bone_mask[0] == 'c') // "close"
			{
				close_skeleton_bone_node(pInst, pNode, pBoneDisable);
			}
		}
	}

	// 移出回收队列
	m_pActionSet->RemoveActionLastUse(nSrcAction);

	//
	blend_node_t* pBlendNode = (blend_node_t*)array_get(
		m_pBlendNodeList, uBlendNode);

	pBlendNode->nAction = nSrcAction;
	pBlendNode->bAutoDelete = false;
	pBlendNode->fBlendTime = get_current_seconds();
	pBlendNode->bLoop = bLoop;
	pBlendNode->bPause = false;
	pBlendNode->fMaxBlendWeight = fMaxBlendWeight;
	pBlendNode->bUnblend = false;
	pBlendNode->fUnblendTime = 0.0F;
	pBlendNode->nFramePrev = 0;
	pBlendNode->fSpeed = pActionNode->fSpeed;
	pBlendNode->pBoneDisable = pBoneDisable;

	// Trigger notify.
	if ((m_pTriggerProc != NULL) && pActionNode->bEnableStartTriggerNotify)
	{
		m_pTriggerProc(m_pUserPointer, nSrcAction, 
			"TRIGGER_ACTION_START", 0);
	}

	return true;
}

bool CActionPlayer::UnblendAction(int action_index)
{
	if (this->IsActionUnblending(action_index))
	{
		return false;
	}
	
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	bool bRet = false;
	int uBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < uBlendCnt; ++i)
	{
		// Find the action
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		// Unblend action
		pBlendNode->bUnblend = true;
		pBlendNode->fUnblendTime = get_current_seconds();

		// Trigger notify
		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, pBlendNode->nAction);
		skeleton_t* pSkt = pActionNode->pSkeleton->GetSkeletonData();	

		if (NULL == pSkt)
		{
			return false;
		}
		
		float fSpeed = m_fSpeed * pBlendNode->fSpeed;
		int nCurrentFrame = get_current_frame(pSkt, pBlendNode, fSpeed);
		int nFrameCount = get_skeleton_frame_count(pSkt);
		float fInternalFPS = pSkt->fInternalFPS;

		nFrameCount -= int(pActionNode->fLeaveTime * fSpeed * fInternalFPS);

		if ((nCurrentFrame < nFrameCount) && (nCurrentFrame >= 4))
		{
			if ((m_pTriggerProc != NULL) 
				&& pActionNode->bEnableBreakTriggerNotify)
			{
				m_pTriggerProc(m_pUserPointer, action_index,
					"TRIGGER_ACTION_BREAK",	nCurrentFrame);
				//FXLOG_DEVELOPER("Name = %s, String = %s, CurrentFrame = %d, "
				//	"FireFrame = %d, PrevFrame = %d\n", "TRIGGER_ACTION_BREAK", 
				//	"NULL", nCurrentFrame, 0, 0);
			}
		}
		else
		{
			if ((m_pTriggerProc != NULL) 
				&& pActionNode->bEnableEndTriggerNotify)
			{
				m_pTriggerProc(m_pUserPointer, action_index,
					"TRIGGER_ACTION_END", 0);
			}
		}

		//FXLOG_DEVELOPER("UnblendAction %s\n", this->GetActionName(action_index));
		bRet = true;
		break;
	}

	if (!bRet)
	{
		int uNotCriticalCnt = array_get_size(m_pNotCriticalBlendNodeList);

		for (int i = 0; i < uNotCriticalCnt; ++i)
		{
			// Find the action
			not_critical_t* pNotCritical = (not_critical_t*)array_get(
				m_pNotCriticalBlendNodeList, i);

			if (pNotCritical->nAction != action_index)
			{
				continue;
			}

			array_remove(m_pNotCriticalBlendNodeList, i);
			bRet = true;
			break;
		}
	}

	if (bRet)
	{
		// 添加到回收队列
		m_pActionSet->UpdateActionLastUse(action_index);
	}

	return bRet;
}

bool CActionPlayer::IsActionBlended(int action_index)
{
	int nBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			return true;
		}
	}

	return false;
}

bool CActionPlayer::IsActionUnblending(int action_index)
{
	int nBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (!pBlendNode->bUnblend)
		{
			continue;
		}

		if (pBlendNode->nAction == action_index)
		{
			return true;
		}
	}

	return false;
}

bool CActionPlayer::SetBlendActionEnterTime(int action_index, float fTime)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));
	
	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	int nCount = array_get_size(m_pBlendNodeList);
	
	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			pBlendNode->fEnterTime = fTime;
			return true;
		}
	}

	return false;
}

float CActionPlayer::GetBlendActionEnterTime(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);
	
	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		if (pBlendNode->fEnterTime < 0.0F)
		{
			action_node_t* pActionNode = (action_node_t*)array_get(
				pActionSet->pActionNodeList, action_index);

			return pActionNode->fEnterTime;
		}

		return pBlendNode->fEnterTime;
	}

	return 0.0F;
}

bool CActionPlayer::SetBlendActionLeaveTime(int action_index, float fTime)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);
	
	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			pBlendNode->fLeaveTime = fTime;
			return true;
		}
	}

	return false;
}

float CActionPlayer::GetBlendActionLeaveTime(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);
	
	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		if (pBlendNode->fLeaveTime < 0.0F)
		{
			action_node_t* pActionNode = (action_node_t*)array_get(
				pActionSet->pActionNodeList, action_index);

			return pActionNode->fLeaveTime;
		}

		return pBlendNode->fLeaveTime;
	}

	return 0.0F;
}

bool CActionPlayer::SetBlendActionSpeed(int action_index, float fSpeed)
{
	if (fSpeed <= 0.0F)
	{
		//FXLOG_ERROR("[error 030023] FXACTION_SetSpeed:Can't set the action's FPS = 0.0\n");
		return false;
	}

	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int uBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < uBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		if (FloatEqual(pBlendNode->fSpeed, fSpeed))
		{
			return true;
		}

		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, action_index);
		skeleton_t* pSkt = pActionNode->pSkeleton->GetSkeletonData();

		if (NULL == pSkt)
		{
			continue;
		}

		double internal_fps = pSkt->fInternalFPS;
		double old_fps = m_fSpeed * pBlendNode->fSpeed * internal_fps;
		double new_fps = m_fSpeed * fSpeed * internal_fps;
		double frames = old_fps * pBlendNode->fCurrentTime;
		double cur_time = double(pBlendNode->fBlendTime) 
			+ double(pBlendNode->fCurrentTime);

		pBlendNode->fCurrentTime = float(frames / new_fps);
		pBlendNode->fBlendTime = float(cur_time - pBlendNode->fCurrentTime);

		if (!FloatEqual(pBlendNode->fUnblendTime, 0.0F))
		{
			frames = old_fps * (cur_time - pBlendNode->fUnblendTime);

			pBlendNode->fUnblendTime = float(cur_time - frames / new_fps);
		}

		pBlendNode->fSpeed = fSpeed;
	}

	return true;
}

float CActionPlayer::GetBlendActionSpeed(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			return pBlendNode->fSpeed;
		}
	}

	return 1.0F;
}

bool CActionPlayer::SetBlendActionWeight(int action_index, float fWeight)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			pBlendNode->fMaxBlendWeight = fWeight;
			return true;
		}
	}

	return false;
}

float CActionPlayer::GetBlendActionWeight(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			return pBlendNode->fMaxBlendWeight;
		}
	}

	return 0.0F;
}

bool CActionPlayer::SetBlendActionPause(int action_index, bool value)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			pBlendNode->bPause = value;
			return true;
		}
	}

	return false;
}

bool CActionPlayer::GetBlendActionPause(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			return pBlendNode->bPause;
		}
	}

	return false;
}

bool CActionPlayer::GetBlendActionLoop(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	int nCount = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			return pBlendNode->bLoop;
		}
	}

	return false;
}

bool CActionPlayer::ClearBlendAction()
{
	int nBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->pBoneDisable)
		{
			CORE_FREE(pBlendNode->pBoneDisable, 
				sizeof(int) * m_pCurrentPose->pSkeleton->nTotalBoneNodeCount);
		}

		if (pBlendNode->nAction > 0)
		{
			// 添加到回收队列
			m_pActionSet->UpdateActionLastUse(pBlendNode->nAction);
		}
	}

	array_clean(m_pBlendNodeList);
	return true;
}

void CActionPlayer::CleanupBlendList()
{
	int nBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < nBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction < 0)
		{
			if (pBlendNode->pBoneDisable)
			{
				CORE_FREE(pBlendNode->pBoneDisable, 
					sizeof(int) * m_pCurrentPose->pSkeleton->nTotalBoneNodeCount);
			}
			
			array_remove(m_pBlendNodeList, i);
			--nBlendCnt;
			--i;
		}
	}
}

#define FLOAT_EQUAL(v1, v2) \
	((v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON)))

enum ACTION_LOAD_STATE_ENUM
{
	LOADSTATE_LOADING, 
	LOADSTATE_SUCCEED, 
	LOADSTATE_FAILED, 
	LOADSTATE_BONE_NOT_MATCH_WITH_MAINSKELETON, 
	LOADSTATE_BONE_TREE_IS_DIFFERENT, 
};

// 检查动作是否加载成功
static int check_action_loaded(action_node_t* pActionNode, 
	skeleton_t* pMainSkeleton)
{
	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	if (NULL == pSkeleton)
	{
		return LOADSTATE_FAILED;
	}

	if (pActionNode->bLoaded)
	{
		skeleton_t* pSkt = pSkeleton->GetSkeletonData();

		if (NULL == pSkt)
		{
			return LOADSTATE_FAILED;
		}

		return (pSkt->nTotalBoneNodeCount == pMainSkeleton->nTotalBoneNodeCount) 
			? LOADSTATE_SUCCEED : LOADSTATE_BONE_NOT_MATCH_WITH_MAINSKELETON;
	}

	if (!pSkeleton->IsLoadComplete())
	{
		return LOADSTATE_LOADING;
	}

	skeleton_t* pSkt = pSkeleton->GetSkeletonData();

	if (pSkt != NULL)
	{
		if (pSkt->nTotalBoneNodeCount != pMainSkeleton->nTotalBoneNodeCount)
		{
			//FXLOG_ERROR("[error 030032] FXSAMODEL_LoadSkeletonAniModel "
			//	"Can't initialize action [%s] with main action [%s], "
			//	"the node count is not match!\n", 
			//	pActionNode->pszSkeletonName, pMainSkeleton->pszName);
			return LOADSTATE_BONE_NOT_MATCH_WITH_MAINSKELETON;
		}

#ifdef MODELADV_RESCHECK_OPEN
		if (!compare_skeleton_bone_tree(pSkt, pMainSkeleton))
		{
			//FXLOG_ERROR("[error 030042] FXSAMODEL_LoadSkeletonAniModel [%s] "
			//	"bone tree is different!\n", pActionNode->pszSkeletonName);
			return LOADSTATE_BONE_TREE_IS_DIFFERENT;
		}
#endif

		// 计算根骨骼Y轴方向的比例，作为根骨骼位移缩放比
		location_t location1 = pSkt->pBoneNodeList[0]->pLocation[0];
		location_t location2 = pMainSkeleton->pBoneNodeList[0]->pLocation[0];

		if (!FLOAT_EQUAL(location1.position.y, 0.0F) 
			&& !FLOAT_EQUAL(location2.position.y, 0.0F))
		{
			pActionNode->fScale = location2.position.y / location1.position.y;
		}
	}

	pActionNode->bLoaded = true;

	if (NULL == pSkt)
	{
		return LOADSTATE_FAILED;
	}

	return (pSkt->nTotalBoneNodeCount == pMainSkeleton->nTotalBoneNodeCount) 
		? LOADSTATE_SUCCEED : LOADSTATE_BONE_NOT_MATCH_WITH_MAINSKELETON;
}

bool CActionPlayer::Blend()
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	if (NULL == pActionSet)
	{
		return false;
	}

	if (!m_bCreated)
	{
		Create();
	}

	skeleton_t* pMainSkeleton = pActionSet->pMainSkeleton->GetSkeletonData();

	if (NULL == pMainSkeleton)
	{
		return false;
	}

	int nBlendCnt = array_get_size(m_pBlendNodeList);

	if (0 == nBlendCnt)
	{
		int nNotCriticalBlendCnt = array_get_size(m_pNotCriticalBlendNodeList);

		if (nNotCriticalBlendCnt > 0)
		{
			// If there is no action blended now, 
			// and there is some action in the not critical action list.
			// blend it.
			return BlendNotCriticalAction();
		}

		return false;
	}

	// Generate weight table.
	float fCurrentTime = get_current_seconds();
	int DelTable[100];
	int UnblendTable[100];
	int uDelTableCnt = 0;
	int uUnblendTable = 0;

	// Scan all actions blended.
	for (int x = nBlendCnt - 1; x >= 0; --x)
	{
		// Get parameters.
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, x);

	    // Is need to auto delete?
        bool bNeedAutoDelete = (x != nBlendCnt - 1) && pBlendNode->bAutoDelete;

		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, pBlendNode->nAction);
		int load_state = check_action_loaded(pActionNode, pMainSkeleton);

		if (load_state != LOADSTATE_SUCCEED)
		{
			if (load_state == LOADSTATE_LOADING)
			{
				// 动作还未成功加载
				pBlendNode->fBlendTime = fCurrentTime;
				continue;
			}
			else
			{
				Assert(uDelTableCnt < 100);

				DelTable[uDelTableCnt++] = x;
				continue;
			}
		}

		//if (!check_action_loaded(pActionNode, pMainSkeleton))
		//{
		//	// 动作还未成功加载
		//	pBlendNode->fBlendTime = fCurrentTime;
		//	continue;
		//}

		if (pBlendNode->bPause)
		{
			// 动作暂停
			float new_blend_time = fCurrentTime - pBlendNode->fCurrentTime;
			
			if (!FloatEqual(pBlendNode->fUnblendTime, 0.0F))
			{
				pBlendNode->fUnblendTime += 
					new_blend_time - pBlendNode->fBlendTime;
			}

			pBlendNode->fBlendTime = new_blend_time;
			continue;
		}

		bool bLoopedAction = pBlendNode->bLoop;
		skeleton_t* pSkt = pActionNode->pSkeleton->GetSkeletonData();
		int nFrameCount = get_skeleton_frame_count(pSkt);
		float fSpeed = m_fSpeed * pBlendNode->fSpeed;
		float fInternalFPS = pSkt->fInternalFPS;
		float fTotalTime = nFrameCount / (fSpeed * fInternalFPS);
		float fEnterTime = pActionNode->fEnterTime;
		
		if (pBlendNode->fEnterTime >= 0.0F)
		{
			fEnterTime = pBlendNode->fEnterTime;
		}
		
		float fLeaveTime = pActionNode->fLeaveTime;
		
		if (pBlendNode->fLeaveTime >= 0.0F)
		{
			fLeaveTime = pBlendNode->fLeaveTime;
		}

		float fBlendTime = pBlendNode->fBlendTime;
		float fUnblendTime = pBlendNode->fUnblendTime;
		bool bUnblend = pBlendNode->bUnblend;

		// 防止动作融合时源动作LeaveTime过小
		//if (pLastActionNode != NULL)
		//{
		//	fLeaveTime = pLastActionNode->fEnterTime;
		//}
		
		//pLastActionNode = pActionNode;

		float fCurSktTime = fCurrentTime - fBlendTime;
		float fCurFrameTime = math_get_remain(fCurSktTime, fTotalTime);
		float fCurSktTimeUnblend = fCurrentTime - fUnblendTime;
		float fCurFrameTimeUnblend = math_get_remain(fCurSktTimeUnblend, 
			fTotalTime);

		pBlendNode->fCurrentTime = fCurSktTime;

		if (!bLoopedAction)
		{
			if (fCurSktTime > fTotalTime)
			{
				fCurFrameTime = fTotalTime;
			}
		}

		// Calculate blend factor between current frame and next frame.
		float fFactor = 1.0F;

		if (bUnblend)
		{
            // 动作淡出
			if ((fUnblendTime - fBlendTime < 0.001F) || (fLeaveTime < 0.001F))
			{
                // 无需淡出
				fFactor = 0.0F;
			}
			else
			{
				if (!bLoopedAction && (fCurSktTime >= fTotalTime))
				{
                    // 淡出完成
					fFactor = 0.0F;
				}
				else
				{
					fFactor = 1.0F - math_saturate(
                        fCurSktTimeUnblend / fLeaveTime);

                    if ((fUnblendTime - fBlendTime) < fEnterTime)
                    {
                        // 淡入未完就开始淡出
                        float t = (fUnblendTime - fBlendTime) / fEnterTime;
                        t = (sinf(t * FLOAT_PI - FLOAT_PI * 0.5F) + 1.0F) * 0.5F;
                        fFactor *= t;
                    }
				}
			}
		}
		else if (fCurSktTime < fEnterTime)
		{
			if (fEnterTime < 0.001F)
			{
				fFactor = 1.0F;
			}
			else
			{
				fFactor = fCurSktTime / fEnterTime;
			}
		}
		else
		{
			if (bLoopedAction)
			{
				fFactor = 1.0F;
			}
			else
			{
				if (fCurSktTime >= fTotalTime)
				{
					fFactor = 0.0F;
				}
				else if (fCurSktTime > (fTotalTime - fLeaveTime))
				{
					if (fLeaveTime < 0.001F)
					{
						fFactor = 0.0F;
					}
					else
					{
						fFactor = (fTotalTime - fCurSktTime) / fLeaveTime;
					}
				}
				else
				{
					fFactor = 1.0F;
				}
			}
		}

		// 将线性插值转换为正弦曲线插值
		//fFactor = sin(fFactor * FLOAT_PI * 0.5F);
		fFactor = (sinf(fFactor * FLOAT_PI - FLOAT_PI * 0.5F) + 1.0F) * 0.5F;
		pBlendNode->fCurrentWeight = fFactor * pBlendNode->fMaxBlendWeight;

		// unblend action.
		if (bNeedAutoDelete)
		{
			Assert(uUnblendTable < 100);
			
			UnblendTable[uUnblendTable++] = pBlendNode->nAction;
		}
		else if (!bLoopedAction)
		{
			// If the action is not a loop action and the time is need to unblend.
			if ((fCurrentTime - fBlendTime) > (fTotalTime - fLeaveTime))
			{
				Assert(uUnblendTable < 100);
				
				UnblendTable[uUnblendTable++] = pBlendNode->nAction;
			}
		}

		// delete action.
		if (bUnblend)
		{
			// If the action is unblending and the frame is out of it's total frame count.
			//if ((fCurrentTime - fUnblendTime) 
			//	> min(fLeaveTime, fUnblendTime - fBlendTime))
			if ((fCurrentTime - fUnblendTime) 
				>= min(fLeaveTime, fUnblendTime - fBlendTime))
			{
				Assert(uDelTableCnt < 100);
				
				DelTable[uDelTableCnt++] = x;
			}
		}
		else if (!bLoopedAction)
		{
			// If the action is not a loop action and the time if out of it's total time.
			if ((fCurrentTime - fBlendTime) > fTotalTime)
			{
				Assert(uDelTableCnt < 100);
				
				DelTable[uDelTableCnt++] = x;
			}
		}

		// Calculate current frame.
		unsigned int nFrame;
		float fTempFactor;

		if (nFrameCount > 0)
		{
			math_get_frame_lerp(nFrameCount, fSpeed * fInternalFPS, 
				fCurSktTime, pBlendNode->bLoop, &nFrame, &fTempFactor);
			nFrame += 3;
		}
		else
		{
			nFrame = pSkt->nEndFrame - pSkt->nStartFrame;
			fTempFactor = 0.0F;
		}

		// If the action is a loop action and looped, trigger notify.
		if (bLoopedAction)
		{
			float fTempTriggerTime = fCurSktTime / fTotalTime;

			fTempTriggerTime = fTempTriggerTime - (int)fTempTriggerTime;

			if (fTempTriggerTime < pBlendNode->fLoopTriggerTimer)
			{
				if ((m_pTriggerProc != NULL) 
					&& pActionNode->bEnableLoopTriggerNotify)
				{
					m_pTriggerProc(m_pUserPointer, pBlendNode->nAction,
						"TRIGGER_ACTION_LOOP", nFrame);

					//FXLOG_DEVELOPER("Name = %s, String = %s, "
					//	"CurrentFrame = %d, FireFrame = %d, PrevFrame = %d\n", 
					//	"TRIGGER_ACTION_LOOP", "NULL", nFrame, 0, 0);
				}
			}

			pBlendNode->fLoopTriggerTimer = fTempTriggerTime;
		}

		// If the action has user-setting-trigger, find out it, and trigger notify.
		if (!bUnblend && (m_pTriggerProc != NULL))
		{
			// Scan all trigger.
			//int nTriggerCnt = array_get_size(pActionNode->pTriggerList);
			int nTriggerCnt;
			
			if (pActionNode->pTriggerList)
			{
				nTriggerCnt = array_get_size(pActionNode->pTriggerList);
			}
			else
			{
				nTriggerCnt = 0;
			}

			for (int t = 0; t < nTriggerCnt; t++)
			{
				action_trigger_t* pTrigger = (action_trigger_t*)array_get(
					pActionNode->pTriggerList, t);

				// If the trigger is invalid.
				if (NULL == pTrigger->pszName)
				{
					continue;
				}

				bool bNeedFire = false;

				// Judgement the trigger is need to fire or not.
				if (pBlendNode->nFramePrev < pTrigger->nFrame)
				{
					if ((pTrigger->nFrame <= int(nFrame)) 
						|| (int(nFrame) < pBlendNode->nFramePrev))
					{
						bNeedFire = true;
					}
				}

				if ((pBlendNode->nFramePrev > int(nFrame)) 
					&& (int(nFrame) >= pTrigger->nFrame))
				{
					bNeedFire = bLoopedAction;
				}

				if (bNeedFire)
				{
					m_pTriggerProc(m_pUserPointer, pBlendNode->nAction,
						pTrigger->pszName, pTrigger->nFrame);
					//FXLOG_DEVELOPER("Name = %s, String = %s, "
					//	"CurrentFrame = %d, FireFrame = %d, PrevFrame = %d\n", 
					//	pTrigger->pszName, pTrigger->pszString, nFrame, 
					//	pTrigger->nFrame, pBlendNode->nFramePrev);
				}
			}

			if (bLoopedAction || (pBlendNode->nFramePrev <= int(nFrame)))
			{
				pBlendNode->nFramePrev = nFrame;
			}
			else
			{
				pBlendNode->nFramePrev = 0;
			}
		}
	}

	// Blend action
	blend_node_t* pBlendNode1 = (blend_node_t*)array_get(
		m_pBlendNodeList, 0);
		
	Assert(pBlendNode1 != NULL);

	action_node_t* pActionNode1 = (action_node_t*)array_get(
		pActionSet->pActionNodeList, pBlendNode1->nAction);
		
	//if (!check_action_loaded(pActionNode1, pMainSkeleton))
	//if (check_action_loaded(pActionNode1, pMainSkeleton) != LOADSTATE_SUCCEED)
	//{
	//	// 动作还未成功加载
	//	return false;
	//}

	if (check_action_loaded(pActionNode1, pMainSkeleton) == LOADSTATE_SUCCEED)
	{
		skeleton_t* pSkt = pActionNode1->pSkeleton->GetSkeletonData();

		// First blend, because the first action will not been action blend, so copy it only.
		copy_skeleton_action(m_pCurrentPose->pSkeleton, pSkt, 
			pBlendNode1->bLoop, m_fSpeed * pBlendNode1->fSpeed, 
			pBlendNode1->fCurrentTime, pMainSkeleton, pActionNode1->nTranslate, 
			pActionNode1->fScale, pBlendNode1->pBoneDisable);
	}

	// Others, the other must been blend or not copy, because it is multi action blend.
	for (int i = 1; i < nBlendCnt; ++i)
	{
		blend_node_t* pBlendNode2 = (blend_node_t*)array_get(
			m_pBlendNodeList, i);
		action_node_t* pActionNode2 = (action_node_t*)array_get(
			pActionSet->pActionNodeList, pBlendNode2->nAction);

		//if (check_action_loaded(pActionNode2, pMainSkeleton) 
		//	!= LOADSTATE_SUCCEED)
		//{
		//	// 动作还未成功加载
		//	continue;
		//}

		int load_state = check_action_loaded(pActionNode2, pMainSkeleton);

		if (load_state != LOADSTATE_SUCCEED)
		{
			if (load_state == LOADSTATE_LOADING)
			{
				// 动作还未成功加载
				continue;
			}
		}
		else
		{
			skeleton_t* pSkt2 = pActionNode2->pSkeleton->GetSkeletonData();

			action_blend(m_pCurrentPose->pSkeleton, pSkt2, 
				pBlendNode2->fCurrentWeight, pBlendNode2->bLoop, 
				pBlendNode2->fCurrentTime, m_fSpeed * pBlendNode2->fSpeed, 
				pMainSkeleton, pActionNode2->nTranslate, pActionNode2->fScale,
				pBlendNode2->pBoneDisable);
		}
	}

	if (m_pParentAction)
	{
		m_pCurrentPose->pParentPose = m_pParentAction->GetCurrentPose();
	}
	else
	{
		m_pCurrentPose->pParentPose = NULL;
	}

    if (!m_bUserBoneEnable)
    {
        update_skeleton(m_pCurrentPose->pSkeleton, 
            m_pCurrentPose->pCurrentNodeTMList, 
            m_pCurrentPose->pChangeNodeTMList, 
            m_pCurrentPose->pParentPose); 
    }

	// Clear up the invalid actions which are unblended and blend out ok.
	for (int i = 0; i < uDelTableCnt; ++i)
	{
		int uBlendNodeIndex = DelTable[i];
		blend_node_t* pBlendNodeTemp = (blend_node_t*)array_get(
			m_pBlendNodeList, uBlendNodeIndex);

		Assert(pBlendNodeTemp != NULL);

		if (pBlendNodeTemp->nAction >= 0)
		{
			// 添加到回收队列
			m_pActionSet->UpdateActionLastUse(pBlendNodeTemp->nAction);
		}

		pBlendNodeTemp->nAction = -1;
		pBlendNodeTemp->fBlendTime = 0.0F;
		pBlendNodeTemp->fUnblendTime = 0.0F;
		pBlendNodeTemp->bUnblend = false;
		pBlendNodeTemp->bAutoDelete = false;
		pBlendNodeTemp->fMaxBlendWeight = 1.0F;
		pBlendNodeTemp->bLoop = false;
		pBlendNodeTemp->bPause = false;
	}

	// If any action is invalid, delete it.
	if (uDelTableCnt != 0)
	{
		this->CleanupBlendList();
	}

	// If any action is need to auto delete, unblend it.
	for (int i = 0; i < uUnblendTable; ++i)
	{
		this->UnblendAction(UnblendTable[i]);
	}

	// If the not-critical-action exist, and the condition matched, blend it.
	nBlendCnt = array_get_size(m_pBlendNodeList);

	if (nBlendCnt > 0)
	{
		blend_node_t* pBlendNodeLast = (blend_node_t*)array_get(
			m_pBlendNodeList, nBlendCnt - 1);

		if (pBlendNodeLast->bLoop)
		{
			// If the last action which blended last is a loop action, 
			// and it is looped ok,
			// the not critical action will been blended.
			action_node_t* pActionNodeLast = (action_node_t*)array_get(
				pActionSet->pActionNodeList, pBlendNodeLast->nAction);
				
			//if (check_action_loaded(pActionNodeLast, pMainSkeleton))
			if (check_action_loaded(pActionNodeLast, pMainSkeleton) 
				== LOADSTATE_SUCCEED)
			{
				skeleton_t* pSktLast = 
					pActionNodeLast->pSkeleton->GetSkeletonData();
				float fSpeed = m_fSpeed * pBlendNodeLast->fSpeed;
				float fInternalFPS = pSktLast->fInternalFPS;
				float fFrameCnt = (float)get_skeleton_frame_count(pSktLast);
				float fTotalTime = fFrameCnt / (fSpeed * fInternalFPS);
				float fTimeTemp = (fCurrentTime - pBlendNodeLast->fBlendTime) 
					/ fTotalTime;

				fTimeTemp = fTimeTemp - (int)fTimeTemp;

				if (fTimeTemp < m_fLoopNotCriticalBlendTimer)
				{
					BlendNotCriticalAction();
				}

				m_fLoopNotCriticalBlendTimer = fTimeTemp;
			}
		}
		else
		{
			// if the last action which blended last is not a loop action,
			// and it is blend completed, the not critical action will be blended.
			m_fLoopNotCriticalBlendTimer = 0.0F;

			if (pBlendNodeLast->bUnblend)
			{
				BlendNotCriticalAction();
			}
		}
	}
	else
	{
		// If there is no action blended now, 
		// and there is some action in the not critical action list.
		// blend it.
		BlendNotCriticalAction();
	}

	// 增加帧序号
	++m_nSequenceId;

	return true;
}

bool CActionPlayer::BlendNotCriticalAction()
{
	int nNotCriticalCnt = array_get_size(m_pNotCriticalBlendNodeList);

	for (int i = 0; i < nNotCriticalCnt; ++i)
	{
		not_critical_t* pNotCritical = (not_critical_t*)array_get(
			m_pNotCriticalBlendNodeList, i);

		Assert(pNotCritical != NULL);
		Assert(pNotCritical->nAction >= 0);

		not_critical_t data;

		memcpy(&data, pNotCritical, sizeof(not_critical_t));

		array_remove(m_pNotCriticalBlendNodeList, i);
		
		this->BlendAction(data.nAction, data.bLoop, data.bAutoDelete, true, 
			data.fMaxBlendWeight, data.szBoneMask, data.szBoneName);

		return true;
	}

	return false;
}

skt_pose_t* CActionPlayer::CreateSkeletonPose()
{
	return create_skeleton_pose();
}

void CActionPlayer::ReleaseSkeletonPose(skt_pose_t* pose)
{
	release_skeleton_pose(pose);
}

void CActionPlayer::CopySkeletonPose(skt_pose_t* dst, skt_pose_t* src)
{
	skeleton_t* pDstInst = dst->pSkeleton;
	skeleton_t* pSrcInst = src->pSkeleton;
	
	if (dst->pCurrentNodeTMList)
	{
		CORE_FREE(dst->pCurrentNodeTMList, 
			sizeof(FmMat4) * pDstInst->nTotalNodeCount);
		dst->pCurrentNodeTMList = NULL;
	}

	// 拷贝动作混合结果的数据结构
	copy_skeleton_structure(pDstInst, pSrcInst);

	// 拷贝矩阵位置
	//if (pDstRef->pCurrentNodeTMList)
	//{
	//	delete[] pDstRef->pCurrentNodeTMList;
	//	pDstRef->pCurrentNodeTMList = NULL;
	//}

	unsigned int node_num = pSrcInst->nTotalNodeCount;

	if (node_num > 0)
	{
		dst->pCurrentNodeTMList = (FmMat4*)CORE_ALLOC(
			sizeof(FmMat4) * node_num);
		memcpy(dst->pCurrentNodeTMList, src->pCurrentNodeTMList,
			sizeof(FmMat4) * node_num);

		skt_node_t** src_node_list = pSrcInst->pNodeList;
		skt_node_t** dst_node_list = pDstInst->pNodeList;
		
		for (unsigned int i = 0; i < node_num; ++i)
		{
			dst_node_list[i]->mtxInitInverse = 
				src_node_list[i]->mtxInitInverse;
			dst_node_list[i]->mtxOriginal = src_node_list[i]->mtxOriginal;
			dst_node_list[i]->mtxCurrent = src_node_list[i]->mtxCurrent;
		}
	}
}

void CActionPlayer::SetSkeletonPose(skt_pose_t* pose)
{
	Assert(pose != NULL);
	
	m_pCurrentPose = pose;
}

skt_pose_t* CActionPlayer::GetSkeletonPose()
{
	return m_pCurrentPose;
}

void CActionPlayer::SetParentAction(void* parent)
{
	m_pParentAction = (CActionPlayer*)parent;

	if (m_pCurrentPose)
	{
		skeleton_t* pInst = m_pCurrentPose->pSkeleton;
		int node_num = pInst->nTotalNodeCount;
		skt_node_t** node_list = pInst->pNodeList;
		
		for (int k = 0; k < node_num; ++k)
		{
			skt_node_t* node = node_list[k];

			// 清除缓冲
			node->bGetSuper = false;
			node->pSuperNode = NULL;
		}
	}
}

void* CActionPlayer::GetParentAction()
{
	return m_pParentAction;
}

void CActionPlayer::SetChildAction(void* child)
{
	m_pChildAction = (CActionPlayer*)child;
}

void* CActionPlayer::GetChildAction()
{
	return m_pChildAction;
}

float CActionPlayer::GetCurrentFrame(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	int nCount = array_get_size(m_pBlendNodeList);
	
	for (int i = 0; i < nCount; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);

		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, action_index);
		CSkeleton* pSkeleton = pActionNode->pSkeleton;

		if (NULL == pSkeleton)
		{
			return -1;
		}

		skeleton_t* pSkt = pSkeleton->GetSkeletonData();

		if (NULL == pSkt)
		{
			return -1;
		}

		int nFrameCount = get_skeleton_frame_count(pSkt);
		unsigned int nFrame;
		float fFactor;

		if (nFrameCount > 0)
		{
			float fInternalFPS = pSkt->fInternalFPS;
			float fSpeed = m_fSpeed * pBlendNode->fSpeed;
			// 必须是当前时间对应的帧数，即使骨骼位置还未到达
			float fCurSktTime = get_current_seconds() - pBlendNode->fBlendTime;

			math_get_frame_lerp(nFrameCount, fSpeed * fInternalFPS, 
				fCurSktTime, pBlendNode->bLoop, &nFrame, &fFactor);
			nFrame += 3;
		}
		else
		{
			nFrame = pSkt->nEndFrame - pSkt->nStartFrame;
			fFactor = 0.0F;
		}

		return float(nFrame) + fFactor;
	}

	return -1;
}

bool CActionPlayer::SetCurrentFrame(int action_index, float fFrame)
{
	if (fFrame < 3)
	{
		return false;
	}

	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	int uBlendCnt = array_get_size(m_pBlendNodeList);

	for (int i = 0; i < uBlendCnt; ++i)
	{
		blend_node_t* pBlendNode = (blend_node_t*)array_get(
			m_pBlendNodeList, i);
		
		if (pBlendNode->nAction != action_index)
		{
			continue;
		}

		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, action_index);
		CSkeleton* pSkeleton = pActionNode->pSkeleton;

		if (NULL == pSkeleton)
		{
			return false;
		}
			
		skeleton_t* pSkt = pSkeleton->GetSkeletonData();

		if (NULL == pSkt)
		{
			return false;
		}

		float fInternalFPS = pSkt->fInternalFPS;
		float fFPS = m_fSpeed * pBlendNode->fSpeed * fInternalFPS;
		
		if (fFPS <= 0.0F)
		{
			fFPS = fInternalFPS;
		}

		if (int(fFrame - 3) >= get_skeleton_frame_count(pSkt))
		{
			return false;
		}

		float fOffsetTime = (fFrame - 3) / fFPS;

		pBlendNode->fBlendTime = get_current_seconds() - fOffsetTime;
		pBlendNode->fCurrentTime = fOffsetTime;
		return true;
	}

	return false;
}

int CActionPlayer::GetFrameCount(int action_index)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	if (NULL == pSkeleton)
	{
		// 只有动作加载后才能取得帧数
		this->InnerLoadAction(action_index, true);

		if (NULL == pActionNode->pSkeleton)
		{
			return -1;
		}

		pSkeleton = pActionNode->pSkeleton;
	}

	skeleton_t* pSkt = pSkeleton->GetSkeletonData();
		
	if (NULL == pSkt)
	{
		return -1;
	}
	
	return get_skeleton_frame_count(pSkt);
}

float CActionPlayer::GetInternalFPS(int action_index)
{
	skeleton_t* pInst = this->GetActionSkeleton(action_index);

	if (NULL == pInst)
	{
		return 1.0F;
	}

	return pInst->fInternalFPS;
}

int CActionPlayer::AddAction(const char* pszName, const char* pszFileName, 
	const char* pszActionFileName)
{
	Assert(pszName != NULL);
	Assert(pszFileName != NULL);
	Assert(pszActionFileName != NULL);

	int index = this->GetActionIndex(pszName);

	if (index >= 0)
	{
		return index;
	}

	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	action_node_t anode;

	memset(&anode, 0, sizeof(anode));
	//anode.pszActionName = FXAllocString(pszName);
	//anode.pszSkeletonName = FXAllocString(pszFileName);
	anode.fSpeed = 1.0F;
	anode.fScale = 1.0F;
	anode.nHashNext = -1;

	if ((strlen(pszActionFileName) != 0) 
		&& (strstr(pActionSet->pszName, pszActionFileName) == NULL))
	{
		set_action_node_string(&anode, pszName, pszFileName, pszActionFileName);
	}
	else
	{
		set_action_node_string(&anode, pszName, pszFileName, "");
	}

	index = array_get_size(pActionSet->pActionNodeList);
	
	array_push_back(pActionSet->pActionNodeList, &anode);

	// 增加动作名索引
	m_pActionSet->AddActionIndex(pszName, index);

	return index;
}

bool CActionPlayer::DeleteAction(const char* pszName)
{
	Assert(pszName != NULL);

	int action_index = this->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	// 移除动作名索引
	m_pActionSet->RemoveActionIndex(pszName, action_index);

	if (pActionNode->pSkeleton)
	{
		pActionNode->pSkeleton->Release();
		pActionNode->pSkeleton = NULL;
	}

	//if (pActionNode->pszActionFileName)
	//{
	//	delete[] pActionNode->pszActionFileName;
	//	pActionNode->pszActionFileName = NULL;
	//}

	//if (pActionNode->pszActionName)
	//{
	//	delete[] pActionNode->pszActionName;
	//	pActionNode->pszActionName = FXAllocString("");
	//}

	if (pActionNode->pTriggerList)
	{
		array_release(pActionNode->pTriggerList);
		pActionNode->pTriggerList = NULL;
	}

	set_action_node_string(pActionNode, "", "", "");

	// 移除动作名索引
	//m_pActionSet->RemoveActionIndex(pszName, action_index);

	return true;
}

bool CActionPlayer::SetKineticsCoefficient(int kinetics_index, 
	int blender_index, float fBaseValue, float fValue)
{
	return false;
//	if (kinetics_index >= m_nKChainCnt)
//	{
//		return false;
//	}
//
//	Kinetics& kinetics = m_pKList[kinetics_index];
//
//	if (kinetics.GetNodeCount() == 0)
//	{
//		return false;
//	}
//
//	return kinetics.SetCoefficient(blender_index, fBaseValue, fValue) == true;
}

bool CActionPlayer::GetKineticsCoefficient(int kinetics_index, 
	int blender_index, float* fBaseValue, float* fValue)
{
	return false;
//	if (kinetics_index >= m_nKChainCnt)
//	{
//		return false;
//	}
//
//	Kinetics& kinetics = m_pKList[kinetics_index];
//
//	if (kinetics.GetNodeCount() == 0)
//	{
//		return false;
//	}
//
//	return kinetics.GetCoefficient(blender_index, fBaseValue, fValue) == true;
}

void CActionPlayer::GetRootBoneOriginPosition(FmVec3* vPos)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	*vPos = pActionSet->vRootPoint;
}

bool CActionPlayer::SetBonePositionAppend(const char* pszName, 
	const FmVec3& vPos)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	m_pCurrentPose->pChangeNodeTMList[node->nIndex]._41 = vPos.x;
	m_pCurrentPose->pChangeNodeTMList[node->nIndex]._42 = vPos.y;
	m_pCurrentPose->pChangeNodeTMList[node->nIndex]._43 = vPos.z;

	return true;
}

bool CActionPlayer::GetBonePositionAppend(const char* pszName, 
	FmVec3* vPos)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	vPos->x = m_pCurrentPose->pChangeNodeTMList[node->nIndex]._41;
	vPos->y = m_pCurrentPose->pChangeNodeTMList[node->nIndex]._42;
	vPos->z = m_pCurrentPose->pChangeNodeTMList[node->nIndex]._43;

	return true;
}

bool CActionPlayer::SetBoneAngleAppend(const char* pszName, 
	const FmVec3& vAngle)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_SetMatrixAngle(&m_pCurrentPose->pChangeNodeTMList[node->nIndex], 
		vAngle.x, vAngle.y, vAngle.z);

	return true;
}

bool CActionPlayer::GetBoneAngleAppend(const char* pszName, 
	FmVec3* vAngle)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_GetMatrixAngle(vAngle, 
		&m_pCurrentPose->pChangeNodeTMList[node->nIndex]);

	return true;
}

bool CActionPlayer::SetBoneAngleAppendNoScale(const char* pszName, 
	const FmVec3& vAngle)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	// 防止矩阵缩放值的误差累积
	VisUtil_SetMatrixAngleNoScale(
		&m_pCurrentPose->pChangeNodeTMList[node->nIndex], 
		vAngle.x, vAngle.y, vAngle.z);

	return true;
}

bool CActionPlayer::SetBoneRotateAppend(const char* pszName, 
	const FmQuat& rotate)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_SetMatrixRotate(&m_pCurrentPose->pChangeNodeTMList[node->nIndex], 
		&rotate);

	return true;
}

bool CActionPlayer::GetBoneRotateAppend(const char* pszName, 
	FmQuat* rotate)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_GetMatrixRotate(rotate, 
		&m_pCurrentPose->pChangeNodeTMList[node->nIndex]);

	return true;
}

bool CActionPlayer::SetBoneRotateAppendNoScale(const char* pszName, 
	const FmQuat& rotate)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_SetMatrixRotateNoScale(
		&m_pCurrentPose->pChangeNodeTMList[node->nIndex], &rotate);

	return true;
}

bool CActionPlayer::GetBoneRotateAppendNoScale(const char* pszName, 
	FmQuat* rotate)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_GetMatrixRotateNoScale(rotate, 
		&m_pCurrentPose->pChangeNodeTMList[node->nIndex]);

	return true;
}

bool CActionPlayer::SetBoneScaleAppend(const char* pszName, 
	const FmVec3& vScale)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_SetMatrixScale(&m_pCurrentPose->pChangeNodeTMList[node->nIndex], 
		vScale.x, vScale.y, vScale.z);

	return true;
}

bool CActionPlayer::GetBoneScaleAppend(const char* pszName, 
	FmVec3* vScale)
{
	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	skt_node_t* node = get_skeleton_bone_node_from_name(
		m_pCurrentPose->pSkeleton, pszName);

	if (NULL == node)
	{
		return false;
	}

	VisUtil_GetMatrixScale(vScale, 
		&m_pCurrentPose->pChangeNodeTMList[node->nIndex]);

	return true;
}

int CActionPlayer::GetNodeCount()
{
	return m_pCurrentPose->pSkeleton->nTotalNodeCount;
}

bool CActionPlayer::GetCurrentNodeTMFromID(int nNodeID, FmMat4* mtxTM)
{
	Assert(size_t(nNodeID) < m_pCurrentPose->pSkeleton->nTotalNodeCount);

	if (NULL == m_pCurrentPose->pCurrentNodeTMList)
	{
		return false;
	}
	
	*mtxTM = m_pCurrentPose->pCurrentNodeTMList[nNodeID];

	return true;
}

// 获得指定节点的当前矩阵
bool CActionPlayer::SetCurrentNodeTMFromID(int nNodeID, const FmMat4& mtxTM)
{
    Assert(size_t(nNodeID) < m_pCurrentPose->pSkeleton->nTotalNodeCount);

    if (NULL == m_pCurrentPose->pCurrentNodeTMList)
    {
        return false;
    }

    m_pCurrentPose->pCurrentNodeTMList[nNodeID] = mtxTM;

    if (NULL == m_pCurrentPose->pCurrentNodeUserResetList)
    {
        init_ragdoll_info(m_pCurrentPose);
    }
    m_pCurrentPose->pCurrentNodeUserResetList[nNodeID] = true;

    return true;
}

bool CActionPlayer::GetChangeNodeTMFromID(int nNodeID, FmMat4* mtxTM)
{
	Assert(size_t(nNodeID) < m_pCurrentPose->pSkeleton->nTotalNodeCount);

	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	*mtxTM = m_pCurrentPose->pChangeNodeTMList[nNodeID];

	return true;
}

bool CActionPlayer::SetChangeNodeTMFromID(int nNodeID, 
	const FmMat4& mtxTM)
{
	Assert(size_t(nNodeID) < m_pCurrentPose->pSkeleton->nTotalNodeCount);

	if (NULL == m_pCurrentPose->pChangeNodeTMList)
	{
		if (!m_bCreated)
		{
			return false;
		} 

		init_change_info(m_pCurrentPose);
	}

	m_pCurrentPose->pChangeNodeTMList[nNodeID] = mtxTM;

	return true;
}

const char* CActionPlayer::GetNodeNameFromID(int nNodeID)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;

	Assert(size_t(nNodeID) < pInst->nTotalNodeCount);

	skt_node_t* pNode = pInst->pNodeList[nNodeID];

	return pNode->pszName;
}

FXNODE_TYPE CActionPlayer::GetNodeTypeFromID(int nNodeID)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;
	
	Assert(size_t(nNodeID) < pInst->nTotalNodeCount);

	skt_node_t* pNode = pInst->pNodeList[nNodeID];

	return pNode->nType;
}

int CActionPlayer::GetNodeIDFromName(const char* pszName)
{
	skt_node_t* node = get_skeleton_node_from_name(m_pCurrentPose->pSkeleton, 
		pszName);

	if (NULL == node)
	{
		return -1;
	}

	return node->nIndex;
}

bool CActionPlayer::GetNodePositionFromID(int nNodeID, 
	FmVec3* position)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;

	Assert(size_t(nNodeID) < pInst->nTotalNodeCount);

	*position = pInst->pNodeList[nNodeID]->pLocation[0].position;
	
	return true;
}

bool CActionPlayer::GetNodeRotateFromID(int nNodeID, 
	FmQuat* rotate)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;

	Assert(size_t(nNodeID) < pInst->nTotalNodeCount);

	*rotate = pInst->pNodeList[nNodeID]->pLocation[0].rotate;

	return true;
}

bool CActionPlayer::GetNodeScaleFromID(int nNodeID, FmVec3* scale)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;

	Assert(size_t(nNodeID) < pInst->nTotalNodeCount);

	*scale = pInst->pNodeList[nNodeID]->pLocation[0].scale;

	return true;
}

bool CActionPlayer::GetRootNodeTMNoTransform(int action_index, int nFrame, 
	FmMat4* mtxTM)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();

	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));

	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);
	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	if (NULL == pSkeleton)
	{
		return false;
	}

	skeleton_t* pSkt = pSkeleton->GetSkeletonData();

	if (NULL == pSkt)
	{
		return false;
	}

	get_root_node_tm_no_transform(pSkt, nFrame, pActionNode->nTranslate, 
		mtxTM);

	return true;
}

bool CActionPlayer::GetCurrentRootNodeTMNoTransform(int action_index, 
	FmMat4* mtxTM)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	Assert(size_t(action_index) < array_get_size(pActionSet->pActionNodeList));
	
	blend_node_t* pBlendNode = NULL;
	action_node_t* pActionNode = NULL;
	int uBlendCnt = array_get_size(m_pBlendNodeList);
	
	for (int i = 0; i < uBlendCnt; ++i)
	{
		pBlendNode = (blend_node_t*)array_get(m_pBlendNodeList, i);

		if (pBlendNode->nAction == action_index)
		{
			pActionNode = (action_node_t*)array_get(
				pActionSet->pActionNodeList, action_index);
			break;
		}
	}

	if (NULL == pActionNode)
	{
		return false;
	}

	CSkeleton* pSkeleton = pActionNode->pSkeleton;

	Assert(pSkeleton != NULL);

	skeleton_t* pSkt = pSkeleton->GetSkeletonData();

	if (NULL == pSkt)
	{
		return false;
	}

	float fCurSktTime = get_current_seconds() - pBlendNode->fBlendTime;
	float fSpeed = m_fSpeed * pBlendNode->fSpeed;
	
	get_current_root_node_tm_no_transform(pSkt, fCurSktTime, fSpeed, 
		pBlendNode->bLoop, pActionNode->nTranslate, mtxTM);
		
	return true;
}

void CActionPlayer::GetCurrentRootNodeTM(FmMat4* mtxTM)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;

	for (unsigned int i = 0; i < pInst->nRootNodeCount; ++i)
	{
		if (FXNODE_TYPE_BONE == pInst->RootNodes[i].nType)
		{
			get_node_tm(m_pCurrentPose, &pInst->RootNodes[i], mtxTM);
			return;
		}
	}

	FmMatrixIdentity(mtxTM);
}

int CActionPlayer::GetBoneNodeCount()
{
	return m_pCurrentPose->pSkeleton->nTotalBoneNodeCount;
}

const char* CActionPlayer::GetBoneNameFromID(int nNodeID)
{
	skeleton_t* pInst = m_pCurrentPose->pSkeleton;

	Assert(size_t(nNodeID) < size_t(pInst->nTotalBoneNodeCount));

	skt_node_t* pBone = pInst->pBoneNodeList[nNodeID];

	return pBone->pszName;
}

bool CActionPlayer::SaveToFile(const char* file_name)
{
	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	
	return save_action_set(pActionSet, file_name);
}

bool CActionPlayer::SaveActionToFile(const char* action_name)
{
	Assert(action_name != NULL);

	int action_index = this->GetActionIndex(action_name);

	if (action_index < 0)
	{
		//FXLOG_ERROR("[error 030021] SaveActionToFile: Can't find action: [%s]\n", 
		//	action_name);
		return false;
	}

	action_set_t* pActionSet = m_pActionSet->GetActionSet();
	action_node_t* pActionNode = (action_node_t*)array_get(
		pActionSet->pActionNodeList, action_index);

	if (NULL == pActionNode)
	{
		//FXLOG_ERROR("[error 030021] SaveActionToFile: Can't find action: [%s] "
		//	"in actionset [%s]\n", 
		//	action_name, pActionSet->pszName);	
		return false;
	}

	char fname[256];

	//if (NULL == pActionNode->pszActionFileName)
	if (0 == pActionNode->pszActionFileName[0])
	{
		CopyString(fname, sizeof(fname), pActionSet->pszName); 
	}
	else
	{
		SafeSprintf(fname, sizeof(fname), "%s%s", 
			g_pRender->GetPathPrefix(pActionNode->pszActionFileName),
			pActionNode->pszActionFileName);
	}

	IIniFile* pIni = g_pCore->CreateIniFile(fname);

	if (!pIni->LoadFromFile())
	{
		pIni->Release();
		//FXLOG_ERROR("[error 030012] FXACTION_SaveAction:Can't load the action, "
		//	"the action file is not exist! %s\n", 
		//	fname);
		return false;
	}

	save_action_node(pIni, pActionSet, pActionNode);

	bool succeed = pIni->SaveToFile();

	pIni->Release();

	return succeed;
}

static void dump_bone_node(FILE* fp, skt_node_t* node, int level)
{
	for (int k = 0; k < level; ++k)
	{
		core_file::fprintf(fp, "  ");
	}

	Assert(node->pszName != NULL);

	if (node->nType == FXNODE_TYPE_BONE)
	{
		core_file::fprintf(fp, "bone_name: %s", node->pszName);
	}
	else if (node->nType == FXNODE_TYPE_HELPER)
	{
		core_file::fprintf(fp, "helper_name: %s", node->pszName);
	}
	else
	{
		core_file::fprintf(fp, "node_name: %s", node->pszName);
	}

	if (node->pParent)
	{
		core_file::fprintf(fp, ", parent_name: %s", node->pParent->pszName);
	}
	else
	{
		core_file::fprintf(fp, ", parent_name: <NULL>");
	}

	core_file::fprintf(fp, "\r\n");

	unsigned int child_node_num = node->nChildNodeCount;

	for (unsigned int i = 0; i < child_node_num; ++i)
	{
		dump_bone_node(fp, &node->ChildNodes[i], level + 1);
	}
}

bool CActionPlayer::DumpBoneNodeTree(const char* file_name)
{
	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	skeleton_t* pInst = m_pCurrentPose->pSkeleton;
	unsigned int root_node_num = pInst->nRootNodeCount;

	for (unsigned int i = 0; i < root_node_num; ++i)
	{
		dump_bone_node(fp, &pInst->RootNodes[i], 0);
	}

	core_file::fclose(fp);

	return true;
}


void * CActionPlayer::GetBonePtr(const char* pszName)
{
	return get_skeleton_bone_node_from_name(m_pCurrentPose->pSkeleton, pszName);
}

const char* CActionPlayer::GetBoneChildsName(void* ptr, int i)
{
	skt_node_t* node = (skt_node_t*)ptr;
	return node->ChildNodes[i].pszName;
}

size_t CActionPlayer::GetBondChildsCount(void* ptr)
{
	skt_node_t* node = (skt_node_t*)ptr;
	return node->nChildNodeCount;
}

// 开启RAGDOLL
void CActionPlayer::EnableUserBone(bool enable)
{
    m_bUserBoneEnable = enable;
    if (m_bUserBoneEnable)
    {
        if (NULL == m_pCurrentPose->pCurrentNodeUserResetList)
        {
            init_ragdoll_info(m_pCurrentPose);
        }
        else
        {
            memset(m_pCurrentPose->pCurrentNodeUserResetList, 0, 
                sizeof(bool)*m_pCurrentPose->pSkeleton->nTotalNodeCount);
        }
    }
}

void CActionPlayer::UserBoneUpdate()
{
    if (m_bUserBoneEnable)
    {
        update_ragdoll_skeleton(m_pCurrentPose->pSkeleton, 
            m_pCurrentPose->pCurrentNodeTMList, 
            m_pCurrentPose->pCurrentNodeUserResetList, 
            m_pCurrentPose->pParentPose); 

        memset(m_pCurrentPose->pCurrentNodeUserResetList, 0, 
            sizeof(bool)*m_pCurrentPose->pSkeleton->nTotalNodeCount);
    }
}