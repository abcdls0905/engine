//--------------------------------------------------------------------
// �ļ���:		i_action_player.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_ACTION_PLAYER_H
#define _VISUAL_I_ACTION_PLAYER_H

#include "../public/macros.h"
//#include "dx_header.h"
#include "i_model_system.h"

// �����������ӿ�

struct skt_pose_t;

class IActionPlayer
{
public:
	virtual ~IActionPlayer() = 0;

	// �ͷ�
	virtual void Release() = 0;
	// �Ƿ����
	virtual bool IsReady() = 0;
	// �Ƿ���ؽ������ɹ���ʧ�ܣ�
	virtual bool IsLoadComplete() = 0;

	// ȱʡ����
	virtual void SetDefaultAction(const char* value) = 0;
	virtual const char* GetDefaultAction() = 0;

	// ���岥���ٶ�
	virtual bool SetSpeed(float fSpeed) = 0;
	virtual float GetSpeed() = 0;

	// �����Ĳ����ٶ�
	virtual bool SetActionSpeed(int action_index, float fSpeed) = 0;
	virtual float GetActionSpeed(int action_index) = 0;

	// ��ǰλ�þ���
	virtual void SetWorldMatrix(const FmMat4* mtxWorld) = 0;
	virtual void GetWorldMatrix(FmMat4* mtxWorld) = 0;

	// ��������
	virtual int GetActionCount() = 0;
	// ���ݶ�������ö�������
	virtual int GetActionIndex(const char* pszName) = 0;
	// ���ݶ���������ö�����
	virtual const char* GetActionName(int action_index) = 0;
	// ���ö�����
	virtual bool SetActionName(int action_index, const char* pszName) = 0;
	// ����������ʱ��
	virtual bool SetActionEnterTime(int action_index, float fTime) = 0;
	virtual float GetActionEnterTime(int action_index) = 0;
	// �������ڳ�ʱ��
	virtual bool SetActionLeaveTime(int action_index, float fTime) = 0;
	virtual float GetActionLeaveTime(int action_index) = 0;
	// �����Ļ�׼�ƶ��ٶ�
	virtual bool SetActionBaseSpeed(int action_index, float fBaseSpeed) = 0;
	virtual float GetActionBaseSpeed(int action_index) = 0;
	// �����Ƿ��λ��
	virtual bool SetActionTranslate(int action_index, int nTranslate) = 0;
	virtual int GetActionTranslate(int action_index) = 0;
	// �����Ĳ�����ʼ֡
	virtual bool SetActionStartFrame(int action_index, int frame) = 0;
	virtual int GetActionStartFarme(int action_index) = 0;
	// �����Ĳ��Ž���֡
	virtual bool SetActionEndFrame(int action_index, int frame) = 0;
	virtual int GetActionEndFarme(int action_index) = 0;
	// ���ö����ĽŲ�֡��Ϣ
	virtual bool SetActionFootstep(int action_index, int footstep_num,
		int* footsteps) = 0;
	// ��ö����Ų�֡��Ϣ����
	virtual int GetActionFootstepCount(int action_index) = 0;
	// ���ָ�������ĽŲ�֡��Ϣ
	virtual int GetActionFootstepFrame(int action_index, int step_index) = 0;
	// ��ö�������ϵ��
	virtual float GetActionScale(int action_index) = 0;
	// ��ö����Ĺ��������ļ���
	virtual const char* GetActionSkeletonName(int action_index) = 0;

	// ���ض�����������
	virtual bool LoadAction(int action_index) = 0;
	// ��ö������������Ƿ���سɹ�
	virtual bool GetActionLoaded(int action_index) = 0;
	// ��ö������������Ƿ���ɼ��أ��ɹ���ʧ�ܣ�
	virtual bool GetActionLoadFinish(int action_index) = 0;

	// ע�ᶯ���ص�
	virtual void RegistActionTriggerProc(ACTION_TRIGGER_PROC proc) = 0;
	// ע�������ص�
	virtual void UnregistActionTriggerProc() = 0;
	// ���ö����ص�������ָ��
	virtual void SetUserPointer(void* pUserPointer) = 0;
	// ��ö����ص�������ָ��
	virtual void* GetUserPointer() = 0;
	// ��ö����ص�����ָ��
	virtual void GetActionTriggerProc(ACTION_TRIGGER_PROC* pProc) = 0;
	// ����Զ��嶯���ص�����
	virtual int GetActionTriggerCount(int action_index) = 0;
	// ����Զ��崥����
	virtual const char* GetActionTriggerName(int action_index, int nTrigger) = 0;
	// ����Զ��崥������֡��
	virtual int GetActionTriggerFrame(int action_index, int nTrigger) = 0;
	// ����Զ��嶯������
	virtual int AddActionTrigger(int action_index, const char* pszTriggerName, 
		int nFrame, const char* pszString) = 0;
	// ɾ���Զ��嶯������
	virtual bool DeleteActionTrigger(int action_index, 
		const char* pszTriggerName, int nFrame) = 0;
	// �����Զ��崥���ַ���
	virtual bool SetActionTriggerString(int action_index, 
		const char* pszTriggerName, int nFrame, const char* pszString) = 0;
	// ����Զ��崥���ַ���
	virtual const char* GetActionTriggerString(int action_index, 
		const char* pszTriggerName, int nFrame) = 0;
	// ������ʼ����֪ͨ	
	virtual bool GetActionStartTriggerNotify(int action_index) = 0;
	virtual bool SetActionStartTriggerNotify(int action_index, 
		bool bStartTrigger) = 0;
	// ������������֪ͨ	
	virtual bool GetActionEndTriggerNotify(int action_index) = 0;
	virtual bool SetActionEndTriggerNotify(int action_index, 
		bool bEndTrigger) = 0;
	// ����ѭ������֪ͨ	
	virtual bool GetActionLoopTriggerNotify(int action_index) = 0;
	virtual bool SetActionLoopTriggerNotify(int action_index, 
		bool bLoopTrigger) = 0;
	// �����жϴ���֪ͨ	
	virtual bool GetActionBreakTriggerNotify(int action_index) = 0;
	virtual bool SetActionBreakTriggerNotify(int action_index, 
		bool bBreakTrigger) = 0;

	// ��õ�ǰ���ںϵĶ�������
	virtual int GetBlendActionCount() = 0;
	// ���ָ�����������ں϶�����
	virtual const char* GetBlendActionName(int uBlendedAction) = 0;
	// ���붯��
	virtual bool BlendAction(int action_index, bool bLoop, bool bAutoDelete, 
		bool bCritical, float fMaxBlendWeight, const char* bone_mask, 
		const char* bone_name) = 0;
	// ���붯��
	virtual bool InsertAction(int nAimAction, int nSrcAction, bool bLoop, 
		float fMaxBlendWeight, const char* bone_mask, const char* bone_name) = 0;
	// �ڳ�����
	virtual bool UnblendAction(int action_index) = 0;
	// ָ�������Ƿ��Ѿ�����
	virtual bool IsActionBlended(int action_index) = 0;
	// ָ�������Ƿ������ڳ�
	virtual bool IsActionUnblending(int action_index) = 0;
	// ���û�϶�������ʱ��
	virtual bool SetBlendActionEnterTime(int action_index, float fTime) = 0;
	// ��û�϶�������ʱ��
	virtual float GetBlendActionEnterTime(int action_index) = 0;
	// ���û�϶����ڳ�ʱ��
	virtual bool SetBlendActionLeaveTime(int action_index, float fTime) = 0;
	// ��û�϶����ڳ�ʱ��
	virtual float GetBlendActionLeaveTime(int action_index) = 0;
	// ���û�϶��������ٶ�
	virtual bool SetBlendActionSpeed(int action_index, float fSpeed) = 0;
	// ��û�϶��������ٶ�
	virtual float GetBlendActionSpeed(int action_index) = 0;
	// ���û�϶���Ȩֵ
	virtual bool SetBlendActionWeight(int action_index, float fWeight) = 0;
	// ��û�϶���Ȩֵ
	virtual float GetBlendActionWeight(int action_index) = 0;
	// ���ö����Ƿ���ͣ
	virtual bool SetBlendActionPause(int action_index, bool value) = 0;
	// ��ö����Ƿ���ͣ
	virtual bool GetBlendActionPause(int action_index) = 0;
	// ��ȡ���ں϶����Ƿ���ѭ������
	virtual bool GetBlendActionLoop(int action_index) = 0;
	// ������ж���
	virtual bool ClearBlendAction() = 0;
	// ִ�ж������
	virtual bool Blend() = 0;

	// �����Ǽ�λ������
	virtual skt_pose_t* CreateSkeletonPose() = 0;
	// ɾ���Ǽ�λ������
	virtual void ReleaseSkeletonPose(skt_pose_t* pose) = 0;
	// �����Ǽ�λ������
	virtual void CopySkeletonPose(skt_pose_t* dst, skt_pose_t* src) = 0;
	// ���õ�ǰ�Ǽ�λ������
	virtual void SetSkeletonPose(skt_pose_t* pose) = 0;
	// ��õ�ǰ�Ǽ�λ������
	virtual skt_pose_t* GetSkeletonPose() = 0;

	// ���ø�������
	virtual void SetParentAction(void* parent) = 0;
	// ��ø�������
	virtual void* GetParentAction() = 0;
	// �����Ӷ�����
	virtual void SetChildAction(void* child) = 0;
	// ����Ӷ�����
	virtual void* GetChildAction() = 0;

	// ��ö����ĵ�ǰ֡
	virtual float GetCurrentFrame(int action_index) = 0;
	// ���ö����ĵ�ǰ֡
	virtual bool SetCurrentFrame(int action_index, float fFrame) = 0;
	// ��ö���֡��
	virtual int GetFrameCount(int action_index) = 0;
	// ��ö�������ÿ��֡��
	virtual float GetInternalFPS(int action_index) = 0;

	// ���Ӷ���
	virtual int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName) = 0;
	// ɾ������
	virtual bool DeleteAction(const char* pszName) = 0;

	// ���ö���ѧ�������ֵ
	virtual bool SetKineticsCoefficient(int kinetics_index, int blender_index, 
		float fBaseValue, float fValue) = 0;
	// ��ö���ѧ�������ֵ
	virtual bool GetKineticsCoefficient(int kinetics_index, int blender_index, 
		float* fBaseValue, float* fValue) = 0;

	// ��ø������ĳ�ʼλ��
	virtual void GetRootBoneOriginPosition(FmVec3* vPos) = 0;

	// �����ĸ���λ��
	virtual bool SetBonePositionAppend(const char* pszName, 
		const FmVec3& vPos) = 0;
	virtual bool GetBonePositionAppend(const char* pszName, FmVec3* vPos) = 0;
	// �����ĸ��ӽǶ�
	virtual bool SetBoneAngleAppend(const char* pszName, 
		const FmVec3& vAngle) = 0;
	virtual bool GetBoneAngleAppend(const char* pszName, FmVec3* vAngle) = 0;
	// ���ù����ĸ��ӽǶȣ������ţ�
	virtual bool SetBoneAngleAppendNoScale(const char* pszName, 
		const FmVec3& vAngle) = 0;
	// �����ĸ�����ת
	virtual bool SetBoneRotateAppend(const char* pszName, 
		const FmQuat& rotate) = 0;
	virtual bool GetBoneRotateAppend(const char* pszName, 
		FmQuat* rotate) = 0;
	// �����ĸ�����ת�������ţ�
	virtual bool SetBoneRotateAppendNoScale(const char* pszName, 
		const FmQuat& rotate) = 0;
	virtual bool GetBoneRotateAppendNoScale(const char* pszName, 
		FmQuat* rotate) = 0;
	// �����ĸ�������
	virtual bool SetBoneScaleAppend(const char* pszName, 
		const FmVec3& vScale) = 0;
	virtual bool GetBoneScaleAppend(const char* pszName, FmVec3* vScale) = 0;

	// ��ýڵ�����
	virtual int GetNodeCount() = 0;
	// ���ָ���ڵ�ĵ�ǰ������������
	virtual bool GetCurrentNodeTMFromID(int nNodeID, FmMat4* mtxTM) = 0;
	// ���ָ���ڵ�ĵ�ǰ����
	virtual bool SetCurrentNodeTMFromID(int nNodeID, const FmMat4& mtxTM) = 0;
	// ���ָ���ڵ����������
	virtual bool GetChangeNodeTMFromID(int nNodeID, FmMat4* mtxTM) = 0;
	// ����ָ���ڵ����������
	virtual bool SetChangeNodeTMFromID(int nNodeID, const FmMat4& mtxTM) = 0;
	// ���ָ�������Ľڵ���
	virtual const char* GetNodeNameFromID(int nNodeID) = 0;
	// ���ָ�������Ľڵ�����
	virtual FXNODE_TYPE GetNodeTypeFromID(int nNodeID) = 0;
	// ���ָ�����ֵĽڵ�����
	virtual int GetNodeIDFromName(const char* pszName) = 0;
	// ��ýڵ㵱ǰλ�ã�δ������
	virtual bool GetNodePositionFromID(int nNodeID, FmVec3* position) = 0;
	// ��ýڵ㵱ǰ��ת��δ������
	virtual bool GetNodeRotateFromID(int nNodeID, FmQuat* rotate) = 0;
	// ��ýڵ㵱ǰ���ţ�δ������
	virtual bool GetNodeScaleFromID(int nNodeID, FmVec3* scale) = 0;
	// ���ָ������ָ��֡����λ�Ƹ��ڵ�λ��
	virtual bool GetRootNodeTMNoTransform(int action_index, int nFrame,
		FmMat4* mtxTM) = 0;
	// ���ָ����������λ�Ƹ��ڵ�λ��
	virtual bool GetCurrentRootNodeTMNoTransform(int action_index, 
		FmMat4* mtxTM) = 0;
	// ��õ�ǰ���ڵ�ľ���
	virtual void GetCurrentRootNodeTM(FmMat4* mtxTM) = 0;
	// ��ù�������
	virtual int GetBoneNodeCount() = 0;
	// ���ָ�������Ĺ�����
	virtual const char* GetBoneNameFromID(int nNodeID) = 0;

	// ���涯�������ļ�
	virtual bool SaveToFile(const char* file_name) = 0;
	// ���涯�����ݵ��ļ�
	virtual bool SaveActionToFile(const char* action_name) = 0;

	// ����������Ϣ
	virtual bool DumpBoneNodeTree(const char* file_name) = 0;

	// �����Ƿ��첽����
	virtual void SetAsyncLoad(bool value) = 0;

	// �Ƿ�ͨ��֡����Ż�
	virtual void SetOptimizeBySequenceId(bool value) = 0;
	virtual bool GetOptimizeBySequenceId() = 0;

	virtual void * GetBonePtr(const char* pszName) = 0;

	virtual const char* GetBoneChildsName(void* ptr, int i) = 0;

	virtual size_t GetBondChildsCount(void* ptr) = 0;

    // �û����ⲿ�����ù���,�翪��RAGDOLL
    virtual void EnableUserBone(bool enable) = 0;
    // ���������,��Ҫ���ⲿ���ù�������
    virtual void UserBoneUpdate() = 0;
};

inline IActionPlayer::~IActionPlayer() {}

#endif // _VISUAL_I_ACTION_PLAYER_H

