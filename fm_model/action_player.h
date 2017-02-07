//--------------------------------------------------------------------
// �ļ���:		action_player.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTION_PLAYER_H
#define _ACTION_PLAYER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_action_player.h"
#include "skeleton_data.h"
#include "array.h"

// ������������

class IRender;
class CActionSet;

class CActionPlayer: public IActionPlayer
{
public:
	static CActionPlayer* NewInstance(CActionSet* pActionSet);

public:
	CActionPlayer(CActionSet* pActionSet);
	virtual ~CActionPlayer();

	// �ͷ�
	virtual void Release();
	// �Ƿ����
	virtual bool IsReady();
	// �Ƿ���ؽ������ɹ���ʧ�ܣ�
	virtual bool IsLoadComplete();

	// ȱʡ����
	virtual void SetDefaultAction(const char* value);
	virtual const char* GetDefaultAction();

	// ���岥���ٶ�
	virtual bool SetSpeed(float fSpeed);
	virtual float GetSpeed();
	
	// �����Ĳ����ٶ�
	virtual bool SetActionSpeed(int action_index, float fSpeed);
	virtual float GetActionSpeed(int action_index);

	// ��ǰλ�þ���
	virtual void SetWorldMatrix(const FmMat4* mtxWorld);
	virtual void GetWorldMatrix(FmMat4* mtxWorld);
	
	// ��������
	virtual int GetActionCount();
	// ���ݶ�������ö�������
	virtual int GetActionIndex(const char* pszName);
	// ���ݶ���������ö�����
	virtual const char* GetActionName(int action_index);
	// ���ö�����
	virtual bool SetActionName(int action_index, const char* pszName);
	// ����������ʱ��
	virtual bool SetActionEnterTime(int action_index, float fTime);
	virtual float GetActionEnterTime(int action_index);
	// �������ڳ�ʱ��
	virtual bool SetActionLeaveTime(int action_index, float fTime);
	virtual float GetActionLeaveTime(int action_index);
	// �����Ļ�׼�ƶ��ٶ�
	virtual bool SetActionBaseSpeed(int action_index, float fBaseSpeed);
	virtual float GetActionBaseSpeed(int action_index);
	// �����Ƿ��λ��
	virtual bool SetActionTranslate(int action_index, int nTranslate);
	virtual int GetActionTranslate(int action_index);
	// �����Ĳ�����ʼ֡
	virtual bool SetActionStartFrame(int action_index, int frame);
	virtual int GetActionStartFarme(int action_index);
	// �����Ĳ��Ž���֡
	virtual bool SetActionEndFrame(int action_index, int frame);
	virtual int GetActionEndFarme(int action_index);
	// ���ö����ĽŲ�֡��Ϣ
	virtual bool SetActionFootstep(int action_index, int footstep_num,
		int* footsteps);
	// ��ö����Ų�֡��Ϣ����
	virtual int GetActionFootstepCount(int action_index);
	// ���ָ�������ĽŲ�֡��Ϣ
	virtual int GetActionFootstepFrame(int action_index, int step_index);
	// ��ö�������ϵ��
	virtual float GetActionScale(int action_index);
	// ��ö����Ĺ��������ļ���
	virtual const char* GetActionSkeletonName(int action_index);

	// ���ض�����������
	virtual bool LoadAction(int action_index);
	// ��ö������������Ƿ���سɹ�
	virtual bool GetActionLoaded(int action_index);
	// ��ö������������Ƿ���ɼ��أ��ɹ���ʧ�ܣ�
	virtual bool GetActionLoadFinish(int action_index);
	
	// ע�ᶯ���ص�
	virtual void RegistActionTriggerProc(ACTION_TRIGGER_PROC proc);
	// ע�������ص�
	virtual void UnregistActionTriggerProc();
	// ���ö����ص�������ָ��
	virtual void SetUserPointer(void* pUserPointer);
	// ��ö����ص�������ָ��
	virtual void* GetUserPointer();
	// ��ö����ص�����ָ��
	virtual void GetActionTriggerProc(ACTION_TRIGGER_PROC* pProc);
	// ����Զ��嶯���ص�����
	virtual int GetActionTriggerCount(int action_index);
	// ����Զ��崥����
	virtual const char* GetActionTriggerName(int action_index, int nTrigger);
	// ����Զ��崥������֡��
	virtual int GetActionTriggerFrame(int action_index, int nTrigger);
	// ����Զ��嶯������
	virtual int AddActionTrigger(int action_index, const char* pszTriggerName, 
		int nFrame, const char* pszString);
	// ɾ���Զ��嶯������
	virtual bool DeleteActionTrigger(int action_index, 
		const char* pszTriggerName, int nFrame);
	// �����Զ��崥���ַ���
	virtual bool SetActionTriggerString(int action_index, 
		const char* pszTriggerName, int nFrame, const char* pszString);
	// ����Զ��崥���ַ���
	virtual const char* GetActionTriggerString(int action_index, 
		const char* pszTriggerName, int nFrame);
	// ������ʼ����֪ͨ	
	virtual bool GetActionStartTriggerNotify(int action_index);
	virtual bool SetActionStartTriggerNotify(int action_index, 
		bool bStartTrigger);
	// ������������֪ͨ	
	virtual bool GetActionEndTriggerNotify(int action_index);
	virtual bool SetActionEndTriggerNotify(int action_index, 
		bool bEndTrigger);
	// ����ѭ������֪ͨ	
	virtual bool GetActionLoopTriggerNotify(int action_index);
	virtual bool SetActionLoopTriggerNotify(int action_index, 
		bool bLoopTrigger);
	// �����жϴ���֪ͨ	
	virtual bool GetActionBreakTriggerNotify(int action_index);
	virtual bool SetActionBreakTriggerNotify(int action_index, 
		bool bBreakTrigger);
	
	// ��õ�ǰ���ںϵĶ�������
	virtual int GetBlendActionCount();
	// ���ָ�����������ں϶�����
	virtual const char* GetBlendActionName(int uBlendedAction);
	// ���붯��
	virtual bool BlendAction(int action_index, bool bLoop, bool bAutoDelete, 
		bool bCritical, float fMaxBlendWeight, const char* bone_mask, 
		const char* bone_name);
	// ���붯��
	virtual bool InsertAction(int nAimAction, int nSrcAction, bool bLoop, 
		float fMaxBlendWeight, const char* bone_mask, const char* bone_name);
	// �ڳ�����
	virtual bool UnblendAction(int action_index);
	// ָ�������Ƿ��Ѿ�����
	virtual bool IsActionBlended(int action_index);
	// ָ�������Ƿ������ڳ�
	virtual bool IsActionUnblending(int action_index);
	// ���û�϶�������ʱ��
	virtual bool SetBlendActionEnterTime(int action_index, float fTime);
	// ��û�϶�������ʱ��
	virtual float GetBlendActionEnterTime(int action_index);
	// ���û�϶����ڳ�ʱ��
	virtual bool SetBlendActionLeaveTime(int action_index, float fTime);
	// ��û�϶����ڳ�ʱ��
	virtual float GetBlendActionLeaveTime(int action_index);
	// ���û�϶��������ٶ�
	virtual bool SetBlendActionSpeed(int action_index, float fSpeed);
	// ��û�϶��������ٶ�
	virtual float GetBlendActionSpeed(int action_index);
	// ���û�϶���Ȩֵ
	virtual bool SetBlendActionWeight(int action_index, float fWeight);
	// ��û�϶���Ȩֵ
	virtual float GetBlendActionWeight(int action_index);
	// ���ö����Ƿ���ͣ
	virtual bool SetBlendActionPause(int action_index, bool value);
	// ��ö����Ƿ���ͣ
	virtual bool GetBlendActionPause(int action_index);
	// ��ȡ���ں϶����Ƿ���ѭ������
	virtual bool GetBlendActionLoop(int action_index);
	// ������ж���
	virtual bool ClearBlendAction();
	// ִ�ж������
	virtual bool Blend();

	// �����Ǽ�λ������
	virtual skt_pose_t* CreateSkeletonPose();
	// ɾ���Ǽ�λ������
	virtual void ReleaseSkeletonPose(skt_pose_t* pose);
	// �����Ǽ�λ������
	virtual void CopySkeletonPose(skt_pose_t* dst, skt_pose_t* src);
	// ���õ�ǰ�Ǽ�λ������
	virtual void SetSkeletonPose(skt_pose_t* pose);
	// ��õ�ǰ�Ǽ�λ������
	virtual skt_pose_t* GetSkeletonPose();
	
	// ���ø�������
	virtual void SetParentAction(void* parent);
	// ��ø�������
	virtual void* GetParentAction();
	// �����Ӷ�����
	virtual void SetChildAction(void* child);
	// ����Ӷ�����
	virtual void* GetChildAction();
	
	// ��ö����ĵ�ǰ֡
	virtual float GetCurrentFrame(int action_index);
	// ���ö����ĵ�ǰ֡
	virtual bool SetCurrentFrame(int action_index, float fFrame);
	// ��ö���֡��
	virtual int GetFrameCount(int action_index);
	// ��ö�������ÿ��֡��
	virtual float GetInternalFPS(int action_index);
	
	// ���Ӷ���
	virtual int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName);
	// ɾ������
	virtual bool DeleteAction(const char* pszName);
	
	// ���ö���ѧ�������ֵ
	virtual bool SetKineticsCoefficient(int kinetics_index, int blender_index, 
		float fBaseValue, float fValue);
	// ��ö���ѧ�������ֵ
	virtual bool GetKineticsCoefficient(int kinetics_index, int blender_index, 
		float* fBaseValue, float* fValue);
	
	// ��ø������ĳ�ʼλ��
	virtual void GetRootBoneOriginPosition(FmVec3* vPos);
	
	virtual void * GetBonePtr(const char* pszName);

	virtual const char* GetBoneChildsName(void* ptr, int i);

	virtual size_t GetBondChildsCount(void* ptr);

	// �����ĸ���λ��
	virtual bool SetBonePositionAppend(const char* pszName, 
		const FmVec3& vPos);
	virtual bool GetBonePositionAppend(const char* pszName, FmVec3* vPos);
	// �����ĸ��ӽǶ�
	virtual bool SetBoneAngleAppend(const char* pszName, 
		const FmVec3& vAngle);
	virtual bool GetBoneAngleAppend(const char* pszName, FmVec3* vAngle);
	// ���ù����ĸ��ӽǶȣ������ţ�
	virtual bool SetBoneAngleAppendNoScale(const char* pszName, 
		const FmVec3& vAngle);
	// �����ĸ�����ת
	virtual bool SetBoneRotateAppend(const char* pszName, 
		const FmQuat& rotate);
	virtual bool GetBoneRotateAppend(const char* pszName, 
		FmQuat* rotate);
	// �����ĸ�����ת�������ţ�
	virtual bool SetBoneRotateAppendNoScale(const char* pszName, 
		const FmQuat& rotate);
	virtual bool GetBoneRotateAppendNoScale(const char* pszName, 
		FmQuat* rotate);
	// �����ĸ�������
	virtual bool SetBoneScaleAppend(const char* pszName, 
		const FmVec3& vScale);
	virtual bool GetBoneScaleAppend(const char* pszName, FmVec3* vScale);
	
	// ��ýڵ�����
	virtual int GetNodeCount();
	// ���ָ���ڵ�ĵ�ǰ������������
	virtual bool GetCurrentNodeTMFromID(int nNodeID, FmMat4* mtxTM);
    // ���ָ���ڵ�ĵ�ǰ����
    virtual bool SetCurrentNodeTMFromID(int nNodeID, const FmMat4& mtxTM);
	// ���ָ���ڵ����������
	virtual bool GetChangeNodeTMFromID(int nNodeID, FmMat4* mtxTM);
	// ����ָ���ڵ����������
	virtual bool SetChangeNodeTMFromID(int nNodeID, const FmMat4& mtxTM);
	// ���ָ�������Ľڵ���
	virtual const char* GetNodeNameFromID(int nNodeID);
	// ���ָ�������Ľڵ�����
	virtual FXNODE_TYPE GetNodeTypeFromID(int nNodeID);
	// ���ָ�����ֵĽڵ�����
	virtual int GetNodeIDFromName(const char* pszName);
	// ��ýڵ㵱ǰλ�ã�δ������
	virtual bool GetNodePositionFromID(int nNodeID, FmVec3* position);
	// ��ýڵ㵱ǰ��ת��δ������
	virtual bool GetNodeRotateFromID(int nNodeID, FmQuat* rotate);
	// ��ýڵ㵱ǰ���ţ�δ������
	virtual bool GetNodeScaleFromID(int nNodeID, FmVec3* scale);
	// ���ָ������ָ��֡����λ�Ƹ��ڵ�λ��
	virtual bool GetRootNodeTMNoTransform(int action_index, int nFrame,
		FmMat4* mtxTM);
	// ���ָ����������λ�Ƹ��ڵ�λ��
	virtual bool GetCurrentRootNodeTMNoTransform(int action_index, 
		FmMat4* mtxTM);
	// ��õ�ǰ���ڵ�ľ���
	virtual void GetCurrentRootNodeTM(FmMat4* mtxTM);
	// ��ù�������
	virtual int GetBoneNodeCount();
	// ���ָ�������Ĺ�����
	virtual const char* GetBoneNameFromID(int nNodeID);
	
	// ���涯�������ļ�
	virtual bool SaveToFile(const char* file_name);
	// ���涯�����ݵ��ļ�
	virtual bool SaveActionToFile(const char* action_name);
	
	// ����������Ϣ
	virtual bool DumpBoneNodeTree(const char* file_name);

	// �����Ƿ��첽����
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }

	// ��õ�ǰ�Ǽ�λ������
	skt_pose_t* GetCurrentPose() { return m_pCurrentPose; }

	// ��ö���֡���
	int GetSequenceId() const { return m_nSequenceId; }

	// �Ƿ�ͨ��֡����Ż�
	virtual void SetOptimizeBySequenceId(bool value) { m_bOptimizeBySequenceId = value; }
	virtual bool GetOptimizeBySequenceId() { return m_bOptimizeBySequenceId; }

	// ����
	bool Create();

    // ����RAGDOLL
    void EnableUserBone(bool enable);
    void UserBoneUpdate();

private:
	CActionPlayer();
	CActionPlayer(const CActionPlayer&);
	CActionPlayer& operator=(const CActionPlayer&);

	// ���ض���
	bool InnerLoadAction(int action_index, bool update_last_use);
	// ��ö����Ĺ�����������
	skeleton_t* GetActionSkeleton(int action_index);
	// ��ʼ��ϵ����ȼ�����
	bool BlendNotCriticalAction();
	// �����϶����б�
	void CleanupBlendList();

private:
	CActionSet* m_pActionSet;
	core_string m_strDefaultAction;
	bool m_bAsyncLoad;
	bool m_bCreated;
	float m_fSpeed;
	array_t* m_pBlendNodeList; // element is blend_node_t
	array_t* m_pNotCriticalBlendNodeList; // element is not_critical_t
	float m_fLoopNotCriticalBlendTimer;
	skt_pose_t* m_pCurrentPose;
	ACTION_TRIGGER_PROC m_pTriggerProc;
	void* m_pUserPointer;
	int m_nSequenceId;
	bool m_bOptimizeBySequenceId;
    FmMat4 m_mtxWorld;
    bool m_bUserBoneEnable; // ���ⲿ�Թ�������������

	CActionPlayer* m_pParentAction;
	CActionPlayer* m_pChildAction;
};

#endif // _ACTION_PLAYER_H
