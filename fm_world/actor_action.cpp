//--------------------------------------------------------------------
// 文件名:		actor_action.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年12月3日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "actor.h"
#include "action_control.h"
#include "../visual/i_action_player.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

static bool init_rotation_180(FmMat4* pOut)
{
	FmMatrixRotationY(pOut, 3.1415926f);
	return true;
}

static FmMat4 s_mtxRotation180;
static bool s_bInitRotation180 = init_rotation_180(&s_mtxRotation180);

// 骨骼动画相关函数

bool Actor::OnActionEvent(void* pUserPointer, unsigned int nActionIndex, 
	const char* pszTriggerName, unsigned int nFrame)
{
	//Assert(pszActionName != NULL);
	Assert(pszTriggerName != NULL);

	Actor* pSelf = (Actor*)pUserPointer;

	Assert(pSelf != NULL);

	// 取得动作回调事件接口对象指针
	IActorCallee* pCallee = pSelf->m_pCallee;

	if (NULL == pCallee)
	{
		return false;
	}

	IActionPlayer* pActionPlayer = pSelf->m_pActionPlayer;

	if (NULL == pActionPlayer)
	{
		return false;
	}

	const char* pszActionName = pActionPlayer->GetActionName(nActionIndex);
	const char* pInfo = pActionPlayer->GetActionTriggerString(nActionIndex, 
		pszTriggerName, nFrame);

	// 触发动作事件回调
	pCallee->OnActionEvent(pSelf, pszActionName, pszTriggerName,
		nFrame, pInfo);

	return true;
}

void Actor::SetCalleeID(const PERSISTID& id)
{
	SAFE_RELEASE(m_pCallee);

	IActorCallee* pCallee = (IActorCallee*)GetCore()->GetEntity(id);

	if (NULL == pCallee)
	{
		return;
	}

	if (!pCallee->GetEntInfo()->IsKindOf("IActorCallee"))
	{
		return;
	}

	m_pCallee = pCallee;
}

PERSISTID Actor::GetCalleeID()
{
	if (NULL == m_pCallee)
	{
		return PERSISTID();
	}

	return m_pCallee->GetID();
}

void Actor::SetActionControlID(const PERSISTID& value)
{
	SAFE_RELEASE(m_pActionControl);

	ActionControl* p = (ActionControl*)GetCore()->GetEntity(value);

	if (NULL == p)
	{
		return;
	}

	if (!p->GetEntInfo()->IsKindOf("ActionControl"))
	{
		return;
	}

	p->SetActionPlayer(m_pActionPlayer);
	m_pActionControl = p;
}

PERSISTID Actor::GetActionControlID() const
{
	if (NULL == m_pActionControl)
	{
		return PERSISTID();
	}

	return m_pActionControl->GetID();
}

bool Actor::SetActionSpeed(const char* pszName, float fSpeed)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}
	
	return m_pActionPlayer->SetActionSpeed(action_index, fSpeed);
}

float Actor::GetActionSpeed(const char* pszName)
{
	if (NULL == m_pActionPlayer)
	{
		return 1.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 1.0F;
	}

	return m_pActionPlayer->GetActionSpeed(action_index);
}

bool Actor::SetAction(const char* pActionName)
{
	return SetActionEx(pActionName, m_DefaultAction.c_str(), "", true);
}

bool Actor::SetActionEx(const char* pActionName, 
	const char* pszDefaultAction, const char* pszAppendPath, bool bAsyncLoad)
{
	Assert(pActionName != NULL);
	Assert(pszDefaultAction != NULL);
	Assert(pszAppendPath != NULL);

    // 不允许重复设置,临时用于测试
    if (m_pActionPlayer != NULL)
    {
		CORE_TRACE("(Actor::SetActionEx)m_pActionPlayer not null");
		CORE_TRACE(pActionName);
        return true;
    }

	SAFE_RELEASE(m_pActionPlayer);

	// means user want to delete current action set
	if (0 == pActionName[0])
	{
		UpdateRoot();
		return true;
	}

	m_DefaultAction = pszDefaultAction;
	m_pActionPlayer = m_pModelSystem->CreateActionPlayer(pActionName, 
		pszAppendPath, pszDefaultAction, (void*)OnActionEvent, this, bAsyncLoad);

	if (NULL == m_pActionPlayer)
	{
		UpdateRoot();
		return false;
	}

	UpdateRoot();

	m_ActionFile = pActionName;

	//if (NULL == m_pCallee)
	//{
	//	m_pCallee = (IActorCallee*)GetCore()->CreateEntity("ActorCallee");
	//}

	IVisBase* pVisBase = m_pRootNode->GetVisBase();

	if (pVisBase)
	{
		m_pRootNode->UpdateMatrix(pVisBase);
	}

	return true;
}

const char* Actor::GetActionFile() const
{
	return m_ActionFile.c_str();
}

void Actor::GetActionList(const IVarList& args, IVarList& result) const
{
	result.Clear();

	if (m_pActionPlayer)
	{
		int action_num = m_pActionPlayer->GetActionCount();

		for (int i = 0; i < action_num; ++i)
		{
			const char* action_name = m_pActionPlayer->GetActionName(i);

			result.AddString(action_name);
		}
	}
}

int Actor::GetActionCount()
{
	if (NULL == m_pActionPlayer)
	{
		return 0;
	}

	return m_pActionPlayer->GetActionCount();
}

int Actor::GetActionIndex(const char* pszName)
{
	Assert(NULL != pszName);

	if (NULL == m_pActionPlayer)
	{
		return -1;
	}

	return m_pActionPlayer->GetActionIndex(pszName);
}

const char* Actor::GetActionName(int action_index)
{
	if (action_index < 0)
	{
		return "";
	}

	if (NULL == m_pActionPlayer)
	{
		return "";
	}

	return m_pActionPlayer->GetActionName(action_index);
}

const char* Actor::GetSkeletonFile(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return "";
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return "";
	}

	return m_pActionPlayer->GetActionSkeletonName(action_index);
}

bool Actor::SetActionEnterTime(const char* pszName, float fTime)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionEnterTime(action_index, fTime);
}

float Actor::GetActionEnterTime(const char* pszName)
{
	Assert(pszName != NULL);
	
	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetActionEnterTime(action_index);
}

bool Actor::SetActionLeaveTime(const char* pszName, float fTime)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionLeaveTime(action_index, fTime);
}

float Actor::GetActionLeaveTime(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetActionLeaveTime(action_index);
}

bool Actor::SetActionBaseSpeed(const char* pszName, float fBaseSpeed)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionBaseSpeed(action_index, fBaseSpeed);
}

float Actor::GetActionBaseSpeed(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetActionBaseSpeed(action_index);
}

float Actor::GetActionScale(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 1.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 1.0F;
	}

	return m_pActionPlayer->GetActionScale(action_index);
}

int Actor::GetActionFrame(const char* pszName) const
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return -1;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return -1;
	}

	return m_pActionPlayer->GetFrameCount(action_index);
}

float Actor::GetActionFPS(const char* pszName) const
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetInternalFPS(action_index);
}

float Actor::GetActionSeconds(const char* pszName) const
{
	float fps = GetActionFPS(pszName);

	if (fps == 0.0F)
	{
		return 0.0F;
	}

	return float(GetActionFrame(pszName)) / fps;
}

float Actor::GetCurrentFrameFloat(const char* pszName) const
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return -1.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return -1.0F;
	}

	return m_pActionPlayer->GetCurrentFrame(action_index);
}

bool Actor::SetCurrentFrameFloat(const char* pszName, float fFrame)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetCurrentFrame(action_index, fFrame);
}

bool Actor::SetCurrentFrame(const char* pszName, int nFrame)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetCurrentFrame(action_index, float(nFrame));
}

int Actor::GetCurrentFrame(const char* pszName) const
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return -1;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return -1;
	}

	return (int)m_pActionPlayer->GetCurrentFrame(action_index);
}

bool Actor::SetActionTranslate(const char* pszName, int nTranslate)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionTranslate(action_index, nTranslate);
}

int Actor::GetActionTranslate(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0;
	}

	return m_pActionPlayer->GetActionTranslate(action_index);
}

void Actor::GetTranslatePosition(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (VTYPE_STRING != args.GetType(0))
	{
		return;
	}

	const char* action_name = args.StringVal(0);

	if (m_pActionPlayer)
	{
		int action_index = m_pActionPlayer->GetActionIndex(action_name);

		if (action_index < 0)
		{
			result.AddFloat(0.0F);
			result.AddFloat(0.0F);
			result.AddFloat(0.0F);
			return;
		}

		FmMat4 mtxTM;

		if (args.GetCount() == 2)
		{
			int frame = args.IntVal(1);

			if (m_pActionPlayer->GetRootNodeTMNoTransform(action_index, 
				frame, &mtxTM))
			{
				//FmMat4 mtxTemp;

#ifdef IMODEL_ADV_NODERADIAN180
				//FmMatrixRotationY(&mtxTemp, D3DXToRadian(180.0F));
				FmMatrixMultiply(&mtxTM, &mtxTM, &s_mtxRotation180);
//#else
//				D3DXMatrixIdentity(&mtxTemp);
#endif // IMODEL_ADV_NODERADIAN180

				//mtxTM *= mtxTemp;

				//Assert(float3_is_valid(mtxTM._41, mtxTM._42, mtxTM._43));

				result.AddFloat(mtxTM._41);
				result.AddFloat(mtxTM._42);
				result.AddFloat(mtxTM._43);
				return;
			}
		}
		
		if (m_pActionPlayer->GetCurrentRootNodeTMNoTransform(action_index,
			&mtxTM))
		{
			//FmMat4 mtxTemp;

#ifdef IMODEL_ADV_NODERADIAN180
			//FmMatrixRotationY(&mtxTemp, D3DXToRadian(180.0F));
			FmMatrixMultiply(&mtxTM, &mtxTM, &s_mtxRotation180);
//#else
//			D3DXMatrixIdentity(&mtxTemp);
#endif // IMODEL_ADV_NODERADIAN180

			//mtxTM *= mtxTemp;

			//Assert(float3_is_valid(mtxTM._41, mtxTM._42, mtxTM._43));

			result.AddFloat(mtxTM._41);
			result.AddFloat(mtxTM._42);
			result.AddFloat(mtxTM._43);
			return;
		}
	}

	result.AddFloat(0.0F);
	result.AddFloat(0.0F);
	result.AddFloat(0.0F);
}

bool Actor::SetCurrentPose(skt_pose_t* pose)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	skt_pose_t* current_pose = m_pActionPlayer->GetSkeletonPose();

	if (NULL == current_pose)
	{
		return false;
	}

	m_pActionPlayer->CopySkeletonPose(current_pose, pose);

	return true;
}

skt_pose_t* Actor::GetCopyPose()
{
	if (NULL == m_pActionPlayer)
	{
		return NULL;
	}

	skt_pose_t* current_pose = m_pActionPlayer->GetSkeletonPose();

	if (NULL == current_pose)
	{
		return NULL;
	}
	
	skt_pose_t* pose = m_pActionPlayer->CreateSkeletonPose();

	m_pActionPlayer->CopySkeletonPose(pose, current_pose);

	return pose;
}

void Actor::DeletePose(skt_pose_t* pose)
{
	if (NULL == m_pActionPlayer)
	{
		return;
	}

	m_pActionPlayer->ReleaseSkeletonPose(pose);
}

int Actor::AddAction(const char* pszName, const char* pszFileName, 
	const char* pszActionFileName)
{
	if (NULL == m_pActionPlayer)
	{
		return -1;
	}

	return m_pActionPlayer->AddAction(pszName, pszFileName, pszActionFileName);
}

bool Actor::DeleteAction(const char* pszName)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->DeleteAction(pszName);
}

bool Actor::LoadAction(int action_index)
{
	if (action_index < 0)
	{
		return false;
	}

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->LoadAction(action_index);
}

bool Actor::GetActionLoaded(int action_index)
{
	if (action_index < 0)
	{
		return false;
	}

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->GetActionLoaded(action_index);
}

void Actor::GetActionBlendList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (m_pActionPlayer)
	{
		int blend_num = m_pActionPlayer->GetBlendActionCount();

		for (int i = 0; i < blend_num; ++i)
		{
			const char* name = m_pActionPlayer->GetBlendActionName(i);

			result.AddString(name);
		}
	}
}

int Actor::GetBlendActionCount()
{
	if (NULL == m_pActionPlayer)
	{
		return 0;
	}

	return m_pActionPlayer->GetBlendActionCount();
}

const char* Actor::GetBlendActionName(int uBlendedAction)
{
	if (uBlendedAction < 0)
	{
		return "";
	}

	if (NULL == m_pActionPlayer)
	{
		return "";
	}

	return m_pActionPlayer->GetBlendActionName(uBlendedAction);
}

void Actor::BlendAction(const IVarList& args, IVarList& result)
{
	if (NULL == m_pActionPlayer)
	{
		result.AddBool(false);
		return;
	}
	
	int arg_num = (int)args.GetCount();

	if ((arg_num != 3) && (arg_num != 5) && (arg_num != 7))
	{
		result.AddBool(false);
		return;
	}

	if ((VTYPE_STRING != args.GetType(0)) 
		|| (VTYPE_BOOL != args.GetType(1))
		|| (VTYPE_BOOL != args.GetType(2)))
	{
		result.AddBool(false);
		return;
	}

	bool bCritical = true;
	float fMaxBlendWeight = 1.0F;
	const char* bone_mask = "";
	const char* bone_name = "";

	if (5 == arg_num)
	{
		if ((VTYPE_BOOL != args.GetType(3)) 
			|| ((VTYPE_FLOAT != args.GetType(4)) 
			&& (VTYPE_DOUBLE != args.GetType(4))))
		{
			result.AddBool(false);
			return;
		}

		bCritical = args.BoolVal(3);
		fMaxBlendWeight = args.FloatVal(4);
	}
	else if (7 == arg_num)
	{
		bCritical = args.BoolVal(3);
		fMaxBlendWeight = args.FloatVal(4);
		bone_mask = args.StringVal(5);
		bone_name = args.StringVal(6);
	}

	int action_index = m_pActionPlayer->GetActionIndex(args.StringVal(0));

	if (action_index < 0)
	{
		CORE_TRACE("(Actor::BlendAction)not find action"); 
		CORE_TRACE(args.StringVal(0));
		result.AddBool(false);
		return;
	}

	if (m_pActionPlayer->BlendAction(action_index, args.BoolVal(1), 
		args.BoolVal(2), bCritical, fMaxBlendWeight, bone_mask, bone_name))
	{
		result.AddBool(true);
		return;
	}
	
	result.AddBool(false);
}

void Actor::InsertAction(const IVarList& args, IVarList& result)
{
	if (NULL == m_pActionPlayer)
	{
		result.AddBool(false);
		return;
	}

	int arg_num = (int)args.GetCount();

	if ((arg_num != 3) && (arg_num != 4) && (arg_num != 6))
	{
		result.AddBool(false);
		return;
	}

	if ((VTYPE_STRING != args.GetType(0)) 
		|| (VTYPE_STRING != args.GetType(1))
		|| (VTYPE_BOOL != args.GetType(2)))
	{
		result.AddBool(false);
		return;
	}

	float fMaxBlendWeight = 1.0F;
	const char* bone_mask = "";
	const char* bone_name = "";

	if (4 == arg_num)
	{
		if ((VTYPE_FLOAT != args.GetType(3)) 
			&& (VTYPE_DOUBLE != args.GetType(3)))
		{
			result.AddBool(false);
			return;
		}

		fMaxBlendWeight = args.FloatVal(3);
	}
	else if (6 == arg_num)
	{
		fMaxBlendWeight = args.FloatVal(3);
		bone_mask = args.StringVal(4);
		bone_name = args.StringVal(5);
	}

	int aim_action = m_pActionPlayer->GetActionIndex(args.StringVal(0));
	int src_action = m_pActionPlayer->GetActionIndex(args.StringVal(1));

	if ((aim_action < 0) || (src_action < 0))
	{
		result.AddBool(false);
		return;
	}
	
	if (m_pActionPlayer->InsertAction(aim_action, src_action, 
		args.BoolVal(2), fMaxBlendWeight, bone_mask, bone_name))
	{
		result.AddBool(true);
		return;
	}
	
	result.AddBool(false);
}

bool Actor::UnblendAction(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->UnblendAction(action_index);
}

bool Actor::IsActionBlended(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->IsActionBlended(action_index);
}

bool Actor::IsActionUnblending(const char* pszName)
{
	Assert(pszName != NULL);
	
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}
	
	return m_pActionPlayer->IsActionUnblending(action_index);
}

bool Actor::SetBlendActionEnterTime(const char* pszName, float fTime)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetBlendActionEnterTime(action_index, fTime);
}

float Actor::GetBlendActionEnterTime(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetBlendActionEnterTime(action_index);
}

bool Actor::SetBlendActionLeaveTime(const char* pszName, float fTime)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetBlendActionLeaveTime(action_index, fTime);
}

float Actor::GetBlendActionLeaveTime(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetBlendActionLeaveTime(action_index);
}

bool Actor::SetBlendActionSpeed(const char* pszName, float fSpeed)
{
	Assert(pszName != NULL);
	
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetBlendActionSpeed(action_index, fSpeed);
}

float Actor::GetBlendActionSpeed(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 1.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 1.0F;
	}

	return m_pActionPlayer->GetBlendActionSpeed(action_index);
}

bool Actor::SetBlendActionWeight(const char* pszName, float fWeight)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetBlendActionWeight(action_index, fWeight);
}

float Actor::GetBlendActionWeight(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 1.0F;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return 1.0F;
	}

	return m_pActionPlayer->GetBlendActionWeight(action_index);
}

bool Actor::SetBlendActionPause(const char* pszName, bool bPause)
{
	Assert(pszName != NULL);
	
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetBlendActionPause(action_index, bPause);
}

bool Actor::GetBlendActionPause(const char* pszName)
{
	Assert(pszName != NULL);
	
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->GetBlendActionPause(action_index);
}

bool Actor::GetBlendActionLoop(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->GetBlendActionLoop(action_index);
}

bool Actor::ClearBlendAction()
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->ClearBlendAction();
}

int Actor::FindAction(const char* pszName)
{
	Assert(pszName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return -1;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return -1;
	}

	if (m_pActionPlayer->GetFrameCount(action_index) < 0)
	{
		return 0;
	}

	return 1;
}

void Actor::GetBoneNodeList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (m_pActionPlayer)
	{
		int bone_num = m_pActionPlayer->GetBoneNodeCount();

		for (int i = 0; i < bone_num; ++i)
		{
			const char* bone_name = m_pActionPlayer->GetBoneNameFromID(i);

			result.AddString(bone_name);
		}
	}
}

void Actor::GetActionNodeList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (m_pActionPlayer)
	{
		int node_num = m_pActionPlayer->GetNodeCount();

		for (int i = 0; i < node_num; ++i)
		{
			const char* node_name = m_pActionPlayer->GetNodeNameFromID(i);

			result.AddString(node_name);
		}
	}
}

bool Actor::SetDefaultAction(const char* pszDefaultAction)
{
	Assert(pszDefaultAction != NULL);

	m_DefaultAction = pszDefaultAction;

	return true;
}

const char*	Actor::GetDefaultAction()
{
	return m_DefaultAction.c_str();
}

bool Actor::SaveActionFile(const char* pszFile)
{
	Assert(pszFile != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->SaveToFile(pszFile);
}

bool Actor::SaveAction(const char* pszActionName)
{
	Assert(pszActionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->SaveActionToFile(pszActionName);
}

bool Actor::AddActionTrigger(const char* pszName, const char* pszTriggerName, 
	int nFrame, const char* pszString)
{
	Assert(pszName != NULL);
	Assert(pszTriggerName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}

	int trigger_index = m_pActionPlayer->AddActionTrigger(action_index, 
		pszTriggerName,	nFrame, pszString);

	if (trigger_index < 0)
	{
		return false;
	}

	return true;
}

bool Actor::DeleteActionTrigger(const char* pszName, 
	const char* pszTriggerName, int nFrame)
{
	Assert(pszName != NULL);
	Assert(pszTriggerName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(pszName);

	if (action_index < 0)
	{
		return false;
	}
		 
	return m_pActionPlayer->DeleteActionTrigger(action_index, 
		pszTriggerName, nFrame);
}

void Actor::GetActionTriggerList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (NULL == m_pActionPlayer)
	{
		return;
	}

	const char* sActionName = args.StringVal(0);
	int action_index = m_pActionPlayer->GetActionIndex(sActionName);

	if (action_index < 0)
	{
		return;
	}

	int trigger_num = m_pActionPlayer->GetActionTriggerCount(action_index);

	for (int i = 0; i < trigger_num; ++i)
	{
		const char* trigger_name = m_pActionPlayer->GetActionTriggerName(
			action_index, i);

		result.AddString(trigger_name);
	}
}

int Actor::GetActionTriggerFrame(const char* actionName, 
	const char* triggerName)
{
	Assert(actionName != NULL);
	Assert(triggerName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return 0;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return 0;
	}

	int trigger_num = m_pActionPlayer->GetActionTriggerCount(action_index);

	for (int i = 0; i < trigger_num; ++i)
	{
		const char* trigger_name = m_pActionPlayer->GetActionTriggerName(
			action_index, i);

		if (strcmp(trigger_name, triggerName) == 0)
		{
			return m_pActionPlayer->GetActionTriggerFrame(action_index, i);
		}
	}

	return 0;
}

const char* Actor::GetActionTriggerString(const char* actionName, 
	const char* triggerName)
{
	Assert(actionName != NULL);
	Assert(triggerName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return "";
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return "";
	}

	int frame = this->GetActionTriggerFrame(actionName, triggerName);

	return m_pActionPlayer->GetActionTriggerString(action_index,
		triggerName, frame);
}

bool Actor::SetActionStartTrigger(const char* actionName, bool bStartTrigger)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionStartTriggerNotify(action_index, 
		bStartTrigger);
}

bool Actor::GetActionStartTrigger(const char* actionName)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->GetActionStartTriggerNotify(action_index);
}

bool Actor::SetActionEndTrigger(const char* actionName, bool bEndTrigger)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionEndTriggerNotify(action_index, 
		bEndTrigger);
}

bool Actor::GetActionEndTrigger(const char* actionName)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->GetActionEndTriggerNotify(action_index);
}

bool Actor::SetActionLoopTrigger(const char* actionName, bool bLoop)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionLoopTriggerNotify(action_index, bLoop);
}

bool Actor::GetActionLoopTrigger(const char* actionName)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->GetActionLoopTriggerNotify(action_index);
}

bool Actor::SetActionBreakTrigger(const char* actionName, bool bBreakTrigger)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->SetActionBreakTriggerNotify(action_index, 
		bBreakTrigger);
}

bool Actor::GetActionBreakTrigger(const char* actionName)
{
	Assert(actionName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int action_index = m_pActionPlayer->GetActionIndex(actionName);

	if (action_index < 0)
	{
		return false;
	}

	return m_pActionPlayer->GetActionBreakTriggerNotify(action_index);
}

bool Actor::SetBonePosition(const char* pszName, float x, float y, float z)
{
	Assert(pszName != NULL);
	//Assert(float3_is_valid(x, y, z));

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->SetBonePositionAppend(pszName, 
		FmVec3(x, y, z));
}

void Actor::GetBonePosition(const IVarList& args, IVarList& result)
{
	result.Clear();
	
	if (args.GetCount() != 1)
	{
		return;
	}

	const char* pszName = args.StringVal(0);

	if (pszName == NULL)
	{
		return;
	}

	if (NULL == m_pActionPlayer)
	{
		return;
	}

	FmVec3 pos;

	if (m_pActionPlayer->GetBonePositionAppend(pszName, &pos))
	{
		//Assert(v3_is_valid(pos));

		result.AddFloat(pos.x);
		result.AddFloat(pos.y);
		result.AddFloat(pos.z);
	}
}

bool Actor::SetBoneAngle(const char* pszName, float x, float y, float z)
{
	Assert(pszName != NULL);
	//Assert(float3_is_valid(x, y, z));

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->SetBoneAngleAppend(pszName, FmVec3(x, y, z));
}

bool Actor::SetBoneAngleNoScale(const char* pszName, float x, float y, 
	float z)
{
	Assert(pszName != NULL);
	//Assert(float3_is_valid(x, y, z));

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->SetBoneAngleAppendNoScale(pszName, 
		FmVec3(x, y, z));
}

void Actor::GetBoneAngle(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (args.GetCount() != 1)
	{
		return;
	}

	const char* pszName = args.StringVal(0);

	if (pszName == NULL)
	{
		return;
	}

	if (NULL == m_pActionPlayer)
	{
		return;
	}

	FmVec3 angle;

	if (m_pActionPlayer->GetBoneAngleAppend(pszName, &angle))
	{
		//Assert(v3_is_valid(angle));

		result.AddFloat(angle.x);
		result.AddFloat(angle.y);
		result.AddFloat(angle.z);
	}
}

bool Actor::SetBoneScale(const char* pszName, float x, float y, float z)
{
	Assert(pszName != NULL);
	//Assert(float3_is_valid(x, y, z));

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->SetBoneScaleAppend(pszName, FmVec3(x, y, z));
}

void Actor::GetBoneScale(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (args.GetCount() != 1)
	{
		return;
	}

	const char* pszName = args.StringVal(0);

	if (pszName == NULL)
	{
		return;
	}

	if (NULL == m_pActionPlayer)
	{
		return;
	}

	FmVec3 scale;

	if (m_pActionPlayer->GetBoneScaleAppend(pszName, &scale))
	{
		//Assert(v3_is_valid(scale));

		result.AddFloat(scale.x);
		result.AddFloat(scale.y);
		result.AddFloat(scale.z);
	}
}

bool Actor::SetKinetics(const char* pName, float fBaseValue, float fValue)
{
	Assert(pName != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int nIndex = -1;

	if (stricmp(pName, "hardness") == 0)
	{
		nIndex = 1;
	}
	else if (stricmp(pName, "gravity") == 0)
	{
		nIndex = 2;
	}
	else
	{
		return false;
	}

	return m_pActionPlayer->SetKineticsCoefficient(0, nIndex, fBaseValue, 
		fValue);
}

void Actor::GetKinetics(const IVarList& args, IVarList& result)
{
	result.Clear();
	
	if (NULL == m_pActionPlayer)
	{
		result.AddBool(false);
		return;
	}

	const char* pName = args.StringVal(0);
	int nIndex = -1;

	if (stricmp(pName, "hardness") == 0)
	{
		nIndex = 1;
	}
	else if (stricmp(pName, "gravity") == 0)
	{
		nIndex = 2;
	}
	else
	{
		result.AddBool(false);
		return;
	}

	float fBaseValue, fValue;

	if (!m_pActionPlayer->GetKineticsCoefficient(0, nIndex, &fBaseValue, 
		&fValue))
	{
		result.AddBool(false);
		return;
	}

	result.AddBool(true);
	result.AddFloat(fBaseValue);
	result.AddFloat(fValue);
}

bool Actor::AddChildAction(const PERSISTID& id)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	Actor* pChildActor = (Actor*)GetCore()->GetEntity(id);

	if (NULL == pChildActor)
	{
		m_pActionPlayer->SetChildAction(NULL);
	}
	else
	{
		m_pActionPlayer->SetChildAction(pChildActor->m_pActionPlayer);
	}

	return true;
}

bool Actor::AddParentAction(const PERSISTID& id)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	Actor* pParentActor = (Actor*)GetCore()->GetEntity(id);

	if (NULL == pParentActor)
	{
		m_pActionPlayer->SetParentAction(NULL);
	}
	else
	{
		m_pActionPlayer->SetParentAction(pParentActor->m_pActionPlayer);
	}

	return true;
}

bool Actor::ChildActionExist()
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->GetChildAction() != NULL;
}

bool Actor::ParentActionExist()
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->GetParentAction() != NULL;
}

bool Actor::DumpBoneNodeTree(const char* file_name)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->DumpBoneNodeTree(file_name);
}

void Actor::EnableUserBone(bool enable)
{
    if (m_pActionPlayer)
    {
        m_pActionPlayer->EnableUserBone(enable);
    }
}

void Actor::UserBoneUpdate()
{
    if (m_pActionPlayer)
    {
        m_pActionPlayer->UserBoneUpdate();
    }
}