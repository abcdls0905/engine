//--------------------------------------------------------------------
// �ļ���:		actor.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��12��3��
// ������:		�ݾ���
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTOR_H
#define _ACTOR_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../visual/i_actor.h"
#include "../visual/i_actor_callee.h"
#include "../visual/i_model_system.h"
#include "../visual/i_model_player.h"
#include "actor_node.h"
#include "../visual/i_render.h"

// �����������

class IIniFile;
class ActionControl;

class Actor: public IActor
{
	IRender* m_pRender;
public:
	Actor();
	virtual ~Actor();

	static bool OnActionEvent(void* pUserPointer,
		unsigned int nActionIndex, const char* pszTriggerName, 
		unsigned int nFrame);

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();
	virtual void RealizeShadowMap();

	// ���������Ƿ�ɹ�
	virtual bool GetLoaded();
	//�Ƿ���سɹ�
	virtual bool GetLoadFinish();

	// ������ɺ�
	bool AfterLoaded();

	virtual	bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
    virtual bool TraceSphere(const FmVec3& center, float radius);
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// ����ת������
	virtual void SetWorldMatrix(const FmMat4& mat);
	virtual void GetWorldMatrix(FmMat4& mat);

	// ȫ�����ĵ�
	virtual FmVec3 GetCenter(); 
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	// ģ�Ͱ뾶
	virtual float GetRadius();

	// �����ص�����
	void SetCalleeID(const PERSISTID& id);
	PERSISTID GetCalleeID();

	// �������ƶ���
	void SetActionControlID(const PERSISTID& value);
	PERSISTID GetActionControlID() const;

	// ���������ٶ�
	bool SetActionSpeed(const char* pszName, float fSpeed);
	float GetActionSpeed(const char* pszName);

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;
	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;
	// ���ű���
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value);
	bool GetShowBoundBox() const;

	// �Ƿ�ɼ� 
	virtual	void SetVisible(bool value);
	virtual	bool GetVisible() const;

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable() const;

	// �Ƿ�ֱ�ӷ�����ײ���Ľڵ����
	void SetReturnTraceNode(bool value) { m_bReturnTraceNode = value; };
	bool GetReturnTraceNode() const { return m_bReturnTraceNode; };

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable() const;

	// �Ƿ���Ҫˮ�淴��
	virtual void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	virtual bool GetWaterReflect() const { return m_bWaterReflect; }

	// д���ٶ������־
	virtual void SetWriteVelocity(int value);
	virtual int GetWriteVelocity() const;

	// ϸ�ڵȼ�
	virtual void SetLevelOfDetail(float value);
	virtual float GetLevelOfDetail() const;

	// �Ƿ��첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// ���������Ƿ���ҪLOD
	void SetAnimationLod(bool value) { m_bAnimationLod = value; }
	bool GetAnimationLod() const { return m_bAnimationLod; }

	// ��ɫ
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	// ������ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// ������Ϣ
	void SetExtraInfo(int value) { m_nExtraInfo = value; }
	int GetExtraInfo() const { return m_nExtraInfo; }

	// �ٶ�
	void SetSpeed(float fSpeed);
	float GetSpeed();

	// ��ǰѡ��Ľڵ�
	void SetSelectNode(const char* name);
	const char* GetSelectNode();

	// ��ʾ������
	void SetShowHelper(bool value) { m_bShowHelper = value; }
	bool GetShowHelper() const { return m_bShowHelper; }

	// ��ʾ�����
	void SetShowCamera(bool value) { m_bShowCamera = value; }
	bool GetShowCamera() const { return m_bShowCamera; }

	// ��ʾ����
	void SetShowBone(bool value) { m_bShowBone = value; }
	bool GetShowBone() const { return m_bShowBone; }

	// ��ȡ��ǰģ���Ƿ������ģ����ģ��
	bool ExistMainModel();
	// ָ����ǰģ�ͽ�����ģ����ģ��������ײ���
	void SetTraceMainModel(bool value);
	// ��ȡ��ǰģ���Ƿ������ģ����ģ��������ײ���
	bool GetTraceMainModel();

	// ��ɫ��ӰЧ��
	void SetBrokenShadow(bool value);
	bool GetBrokenShadow();
	
	// ��ɫ��ӰЧ������
	PERSISTID GetBrokenShadowID() const;

	// �ڵ��ӰЧ��
	bool SetNodeBrokenShadow(const char* pName, bool value);
	bool GetNodeBrokenShadow(const char* pName);

	// �ڵ��ӰЧ������
	PERSISTID GetNodeBrokenShadowID(const char* pName) const;

	// ��������������ļ���
	const char* GetConfigFile() const;

	// ���¸��ڵ�
	bool UpdateRoot();
	// ���ö����ļ�
	bool SetAction(const char* pActionName);
	bool SetActionEx(const char* pszAction, const char* pszDefaultAction, 
		const char* pszAppendPath, bool bAsyncLoad);
	// ��ö����ļ���
	const char* GetActionFile() const;
	// ��ö������б�
	void GetActionList(const IVarList& args, IVarList& result) const;
	// ��������
	int GetActionCount();
	// ���ݶ�������ö�������
	int GetActionIndex(const char* pszName);
	// ���ݶ���������ö�����
	const char* GetActionName(int action_index);
	// ���ָ����������Դ�ļ���
	const char* GetSkeletonFile(const char* pszName);
	// ��������ʱ��
	bool SetActionEnterTime(const char* pszName, float fTime);
	float GetActionEnterTime(const char* pszName);
	// �����ڳ�ʱ��
	bool SetActionLeaveTime(const char* pszName, float fTime);
	float GetActionLeaveTime(const char* pszName);
	// ������׼�ٶ�
	bool SetActionBaseSpeed(const char* pszName, float fBaseSpeed);
	float GetActionBaseSpeed(const char* pszName);
	// ��������ϵ��
	float GetActionScale(const char* pszName);
	// ��õ�ǰ�ںϵĶ����б�
	void GetActionBlendList(const IVarList& args, IVarList& result);
	// ��ö�����֡��
	int GetActionFrame(const char* pszName) const;
	// ��ö�����ÿ��֡��
	float GetActionFPS(const char* pszName) const;
	// ��ö����ĳ���ʱ��
	float GetActionSeconds(const char* pszName) const;
	// ���õ�ǰ�Ķ���֡(������)
	float GetCurrentFrameFloat(const char* pszName) const;
	// ��ȡ��ǰ�Ķ���֡(������)
	bool SetCurrentFrameFloat(const char* pszName, float fFrame);
	// ���õ�ǰ�Ķ���֡(����)
	bool SetCurrentFrame(const char* pszName, int nFrame);
	// ��ȡ��ǰ�Ķ���֡(����)
	int GetCurrentFrame(const char* pszName) const;
	// ����ָ���Ķ����Ƿ�Ϊλ�ƶ���
	bool SetActionTranslate(const char* pszName, int nTranslate);
	// ��ȡָ���Ķ����Ƿ�Ϊλ�ƶ���
	int GetActionTranslate(const char* pszName);
	// ��ȡ��λ��ģʽ�£���ǰ��λ�Ƹ����������λ��
	void GetTranslatePosition(const IVarList& args, IVarList& result);
	// ���õ�ǰ������Ϣ
	bool SetCurrentPose(skt_pose_t* pose);
	// ȡ�õ�ǰ�����ĸ���
	skt_pose_t* GetCopyPose();
	// ɾ��������Ϣ
	void DeletePose(skt_pose_t* pose);
	// ��Ӷ���
	int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName);
	// ɾ������
	bool DeleteAction(const char* pszName);
	// ���ض�����������
	bool LoadAction(int action_index);
	// ��ö������������Ƿ���سɹ�
	bool GetActionLoaded(int action_index);

	// ��õ�ǰ���ںϵĶ�������
	int GetBlendActionCount();
	// ���ָ�����������ں϶�����
	const char* GetBlendActionName(int uBlendedAction);
	// �ں�ָ������
	void BlendAction(const IVarList& args, IVarList& result);
	// ����ָ������
	void InsertAction(const IVarList& args, IVarList& result);
	// ȡ��ָ���ں϶���
	bool UnblendAction(const char* pszName);
	// �����Ƿ����ں�
	bool IsActionBlended(const char* pszName);
	// �����Ƿ�����ȡ���ں�
	bool IsActionUnblending(const char* pszName);
	// �������ں϶���������ʱ��
	bool SetBlendActionEnterTime(const char* pszName, float fTime);
	// ��ȡ���ں϶���������ʱ��
	float GetBlendActionEnterTime(const char* pszName);
	// �������ں϶������ڳ�ʱ��
	bool SetBlendActionLeaveTime(const char* pszName, float fTime);
	// ��ȡ���ں϶������ڳ�ʱ��
	float GetBlendActionLeaveTime(const char* pszName);
	// �������ں϶����Ĳ����ٶ�
	bool SetBlendActionSpeed(const char* pszName, float fSpeed);
	// ��ȡ���ں϶����Ĳ����ٶ�
	float GetBlendActionSpeed(const char* pszName);
	// �������ں϶�����Ȩֵ
	bool SetBlendActionWeight(const char* pszName, float fWeight);
	// ��ȡ���ں϶����Ĳ����ٶ�
	float GetBlendActionWeight(const char* pszName);
	// �������ں϶����Ƿ���ͣ
	bool SetBlendActionPause(const char* pszName, bool bPause);
	// ��ȡ���ں϶����Ƿ���ͣ
	bool GetBlendActionPause(const char* pszName);
	// ��ȡ���ں϶����Ƿ���ѭ������
	bool GetBlendActionLoop(const char* pszName);
	// ������л�϶���
	bool ClearBlendAction();
	// ���Ҷ����Ƿ����
	int FindAction(const char* pszName);

	// ȡ�ù����ڵ��б�
	void GetBoneNodeList(const IVarList& args, IVarList& result);
	// ȡ�ø�����ڵ��б�
	void GetHelperNodeList(const IVarList& args, IVarList& result);
	// ȡ�ö�����ؽڵ��б�
	void GetActionNodeList(const IVarList& args, IVarList& result);
	// ȡ�ýڵ�ĵ�ǰ����Ծ���
	bool GetCurrentNodeMatrix(CActorNode* pNode, const char* pNodeName, 
		FmMat4* mtxTM);
    // ���ýڵ�ĵ�ǰ����Ծ���
    bool SetCurrentNodeMatrix(CActorNode* pNode, const char* pNodeName, 
        const FmMat4* mtxTM);
	// ȡ�ýڵ���������
	bool GetNodeWorldMatrix(CActorNode* pNode, const char* pNodeName, 
		FmMat4* mat);
	// ͨ������������ýڵ�λ��
	bool SetNodeWorldMatrix(CActorNode* pNode, const char* pNodeName, 
		const FmMat4* mat);

	// ȡ�ýڵ�ĵ�ǰλ��
	FmVec3 GetNodePosition(const char* pNodeName);
	bool GetNodePosition(const char* pNodeName, FmVec3& pos);
	void GetNodePosition(const IVarList& args, IVarList& result);
	// ȡ�ýڵ�ĵ�ǰ����
	bool GetNodeAngle(const char* pNodeName, FmVec3& vAngle);
	void GetNodeAngle(const IVarList& args, IVarList& result);

	// ȡ�ýڵ�ĵ�ǰ��С
	FmVec3 GetNodeScale(const char* pNodeName);
	void GetNodeScale(const IVarList& args, IVarList& result);

	// ȡ�ýڵ���������
    bool GetNodeWorldMatrix(const char* pNodeName, FmMat4& mat);
	// ���ýڵ���������
    bool SetNodeWorldMatrix(const char* pNodeName, const FmMat4& mat);

	// �жϽڵ��Ƿ����
	bool NodeIsExist(const char* pNodeName);

	// ͨ�������ļ��������ģ��
	bool CreateFromIni(const char* pFileName);
	bool CreateFromIniEx(const char* pFileName, const char* append_path);

	// ����Ĭ�϶���
	bool SetDefaultAction(const char* pszDefaultAction);
	const char* GetDefaultAction();

	// ɾ����Ƥ
	bool DeleteSkin(const char* pModelName);
	// �����Ƥ
	bool AddSkin(const char* pszModelName, const char* pszModelFile);
	bool AddSkinEx(const char* pszModelName, const char* pszModelFile, 
		const char* pszMaterialFile);

	// �����Чģ��
	bool AddEffectModel(const char* em_file, const char* append_path);

	// ��Ӷ����¼��ص�������
	bool AddActionTrigger(const char* pszName, const char* pszTriggerName, 
		int nFrame, const char* pszString);
	// ���涯���ļ�
	bool SaveActionFile(const char* pszFile);
	// ����ָ����������Ϣ
	bool SaveAction(const char* pszActionName);
	// ɾ�������¼��ص�������
	bool DeleteActionTrigger(const char* pszName, const char* pszTriggerName, 
		int nFrame);
	// ȡ�ö����¼��ص��������б�
	void GetActionTriggerList(const IVarList& args, IVarList& result);
	// ȡ�ö����¼��ص��������Ĵ�����
	int GetActionTriggerFrame(const char* actionName, const char* triggerName);
	// ȡ�ö����¼��ص��������ĸ�����Ϣ
	const char* GetActionTriggerString(const char* actionName, 
		const char* triggerName);
	// ����ָ��������ʼ�¼��������Ƿ���
	bool SetActionStartTrigger(const char* actionName, bool bStartTrigger);
	// ��ȡָ��������ʼ�¼��������Ƿ���
	bool GetActionStartTrigger(const char* actionName);
	// ����ָ�����������¼��������Ƿ���
	bool SetActionEndTrigger(const char* actionName, bool bEndTrigger);
	// ��ȡָ�����������¼��������Ƿ���
	bool GetActionEndTrigger(const char* actionName);
	// ����ָ������ѭ���¼��������Ƿ���
	bool SetActionLoopTrigger(const char* actionName, bool bLoop);
	// ��ȡָ������ѭ���¼��������Ƿ���
	bool GetActionLoopTrigger(const char* actionName);
	// ����ָ������������¼��������Ƿ���
	bool SetActionBreakTrigger(const char* actionName, bool bBreakTrigger);
	// ��ȡָ������������¼��������Ƿ���
	bool GetActionBreakTrigger(const char* actionName);

	// ���Ӷ��󵽵���
	void LinkToPoint(const IVarList& args, IVarList& result);
	bool LinkToPoint(const char* name, const char* pointname, const PERSISTID& id, bool bEsseObj = true);
	// �ӵ���ɾ�����Ӷ���
	bool UnLink(const char* name, bool bDelete);
	// ȡ�����Ӷ���ID
	PERSISTID GetLinkObject(const char* name);

	// �������λ��
	bool SetLinkPosition(const char* name, float x, float y, float z);
	void GetLinkPosition(const IVarList& args, IVarList& result);

	// ������ӷ���
	bool SetLinkAngle(const char* name, float x, float y, float z);
	void GetLinkAngle(const IVarList& args, IVarList& result);

	// ������Ӵ�С
	bool SetLinkScale(const char* name, float x, float y, float z);
	void GetLinkScale(const IVarList& args, IVarList& result);

	// �������Ӷ������Ƿ񱣳ְ󶨷���
	bool SetLinkAngleKeep(const char* name, bool value);
	bool GetLinkAngleKeep(const char* name) const;

	void GetLinkObjList(const IVarList& args, IVarList& result);

	// ������нڵ�Ŀɼ������б�
	void GetVisualList(const IVarList& args, IVarList& result);

	// �����ӽڵ��Ƿ�ɼ�
	bool SetNodeVisible(const char* name, bool value);
	bool GetNodeVisible(const char* name);

	// ���ýڵ��Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
	bool SetNodeEsseObj(const char* name, bool value);
	bool GetNodeEsseObj(const char* name);

	// ���ò������Ӱ�Ľڵ��б�
	void SetNoBrokenShadowList(const IVarList& args, IVarList& result);
	void GetNoBrokenShadowList(const IVarList& args, IVarList& result);

	// �軭��Ӱ
	void RealizeBrokenShadow();

	// �������λ����Ϣ
	bool SetBonePosition(const char* pszName, float x, float y, float z);
	void GetBonePosition(const IVarList& args, IVarList& result);
	// ���������ת��Ϣ
	bool SetBoneAngle(const char* pszName, float x, float y, float z);
	bool SetBoneAngleNoScale(const char* pszName, float x, float y, float z);
	void GetBoneAngle(const IVarList& args, IVarList& result);
	// �������������Ϣ
	bool SetBoneScale(const char* pszName, float x, float y, float z);
	void GetBoneScale(const IVarList& args, IVarList& result);

	bool SetKinetics(const char* pName, float fBaseValue, float fValue);
	void GetKinetics(const IVarList& args, IVarList& result);

	// ����Ӷ�����
	bool AddChildAction(const PERSISTID& id);
	// ��Ӹ�������
	bool AddParentAction(const PERSISTID& id);
	// �ж��Ӷ������Ƿ����
	bool ChildActionExist();
	// �жϸ��������Ƿ����
	bool ParentActionExist();

	// �����Զ�����Ŀ�����
	void SetAimObject(PERSISTID id);
	PERSISTID GetAimObject() const;

	// �㲿�Ƿ����ϵ���
	void SetFootAnchor(bool value); 
	bool GetFootAnchor() const;

	// �������
	void SetFootFloorID(const PERSISTID& value);
	PERSISTID GetFootFloorID() const;

	// ���������ṹ���ı��ļ�
	bool DumpBoneNodeTree(const char* file_name);

    void EnableUserBone(bool enable);
    void UserBoneUpdate();

	// ��ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value) { m_bCastShadow = value; }
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// ��Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value) { m_bReceiveShadow = value; }
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

private:
	// ������Чģ��
	void CreateEffectModel(IIniFile* pIni, const char* append_path);
	
	// ���°�Χ��
	void UpdateBoundingBox();

	// ������Ŀ��
	bool ResumeAimObjectAngle();
	bool ResumeAimObjectPitchAngle(float sub_pitch = 0.0F);
	bool ResumeAimObjectOrientAngle(float sub_orient = 0.0F);
	bool ResumeAimObjectHeadAngleZ(float sub_orient = 0.0F);
	bool UpdateAimObjectAngle();
	bool UpdateAimObjectPitchAngle();
	bool UpdateAimObjectOrientAngle();
	bool UpdateAimObjectHeadAngleZ();	// Ϊ����ת����ʱ��ͷ����

	// ����㲿����
	void ClearFootAnchor();
	// �㲿����
	void UpdateFootAnchor(float seconds);

	void ClearNoBrokenShadowList()
	{
		for (size_t i = 0; i < m_NoBrokenShadowList.size(); ++i)
		{
			char* pName = m_NoBrokenShadowList[i];

			CORE_FREE(pName, strlen(pName) + 1);
		}

		m_NoBrokenShadowList.clear();
	}

private:
	IModelSystem* m_pModelSystem;
	IActionPlayer* m_pActionPlayer;
	ActionControl* m_pActionControl;
	FmMat4 m_mtxWorld;
	FmVec3 m_vBoundMin;
	FmVec3 m_vBoundMax;
	FmVec3 m_vCenter;
	float m_fRadius;
	core_string m_ConfigFile;
	core_string m_ActionFile;
	core_string m_DefaultAction;
	CActorNode* m_pRootNode;

	bool m_bShowBoundBox;
	bool m_bShowBone;
	bool m_bShowHelper;
	bool m_bShowCamera;
	bool m_bVisible;
	bool m_bUnderWater;
	bool m_bTraceEnable;
	bool m_bReturnTraceNode;
	bool m_bCullEnable;
	bool m_bWaterReflect;
	int m_nWriteVelocity;
	bool m_bAsyncLoad;
	bool m_bAnimationLod;

	// ��ҪͶ��������Ӱ
	bool m_bCastShadow;
	bool m_bReceiveShadow;
	
	unsigned int m_nColor;
	int m_nExtraInfo;
	float m_fAlphaValue;
	float m_fLodValue;
	unsigned int m_nBlendTick;

	IActorCallee* m_pCallee;
	core_string m_SelectNode;
	PERSISTID m_BrokenShadow;
	PERSISTID m_AimObject;
	bool m_bAutoAimObjAngle;
	bool m_bUpdatedBoundingBox;
	bool m_bNeedUpdateNode;
	bool m_bFootAnchor;
	float m_fFootAnchorFactor;
	PERSISTID m_FootFloorID;

	TArrayPod<char*, 1, TCoreAlloc> m_NoBrokenShadowList;
};

#endif // _ACTOR_H
