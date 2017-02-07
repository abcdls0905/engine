//--------------------------------------------------------------------
// �ļ���:		i_actor.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��2��28��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_ACTOR_H
#define _VISUAL_I_ACTOR_H

#include "i_vis_base.h"

// ���ģ�ͽӿ�

class IActor: public IVisBase
{
public:
	// ͨ�������ļ��������ģ��
	virtual bool CreateFromIni(const char* pFileName) = 0;
	virtual bool CreateFromIniEx(const char* pFileName, const char* append_path) = 0;
	// ���ö����ļ�
	virtual bool SetActionEx(const char* pActionName, const char* pszDefaultAction, const char* pszAppendPath, bool bAsyncLoad) = 0;
	// ��������
	virtual int GetActionCount() = 0;
	// ���ݶ�������ö�������
	virtual int GetActionIndex(const char* pszName) = 0;

	// ���ݶ���������ö�����
	virtual const char* GetActionName(int action_index) = 0;
	// ���Ҷ����Ƿ����
	virtual int FindAction(const char* pszName) = 0;

	// ȡ�ø�����ڵ��б�
	virtual void GetHelperNodeList(const IVarList& args, IVarList& result) = 0;

	// ��ö�����׼�ٶ�
	virtual float GetActionBaseSpeed(const char* pszName) = 0;
	// ��õ�ǰ�ںϵĶ����б�
	virtual void GetActionBlendList(const IVarList& args, IVarList& result) = 0;
	// ��ö�����֡��
	virtual int GetActionFrame(const char* pszName) const  = 0;
	// ��ö�����ÿ��֡��
	virtual float GetActionFPS(const char* pszName) const  = 0;
	// ��ö����ĳ���ʱ��
	virtual float GetActionSeconds(const char* pszName) const = 0;
	// ���õ�ǰ�Ķ���֡(������)
	virtual float GetCurrentFrameFloat(const char* pszName) const = 0;
	// ��ȡ��ǰ�Ķ���֡(������)
	virtual  bool SetCurrentFrameFloat(const char* pszName, float fFrame) = 0;
	// ���õ�ǰ�Ķ���֡(����)
	virtual bool SetCurrentFrame(const char* pszName, int nFrame) = 0;
	// ��ȡ��ǰ�Ķ���֡(����)
	virtual int GetCurrentFrame(const char* pszName) const = 0;
	// ����ָ���Ķ����Ƿ�Ϊλ�ƶ���
	virtual bool SetActionTranslate(const char* pszName, int nTranslate) = 0;
	// ��ȡָ���Ķ����Ƿ�Ϊλ�ƶ���
	virtual int GetActionTranslate(const char* pszName) = 0;
	// ��ȡ��λ��ģʽ�£���ǰ��λ�Ƹ����������λ��
	virtual void GetTranslatePosition(const IVarList& args, IVarList& result)= 0;
	// ��Ӷ���
	virtual int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName)= 0;
	// ɾ������
	virtual bool DeleteAction(const char* pszName)= 0;
	// ���ض�����������
	virtual bool LoadAction(int action_index)= 0;
	// ��ö������������Ƿ���سɹ�
	virtual bool GetActionLoaded(int action_index) = 0;

	// �ں�ָ������
	virtual void BlendAction(const IVarList& args, IVarList& result) = 0;
	// ȡ��ָ���ں϶���
	virtual bool UnblendAction(const char* pszName) = 0;
    // �����Ƿ����ں�
	virtual bool IsActionBlended(const char* pszName) = 0;
	// �����Ƿ�����ȡ���ں�
	virtual bool IsActionUnblending(const char* pszName) = 0;
	// ��õ�ǰ���ںϵĶ�������
	virtual int GetBlendActionCount() = 0;
	// ���ָ�����������ں϶�����
	virtual const char* GetBlendActionName(int uBlendedAction) = 0;
	// ��ȡ���ں϶����Ƿ���ѭ������
	virtual bool GetBlendActionLoop(const char* pszName) = 0;
	// ������л�϶���
	virtual bool ClearBlendAction() = 0;
	
	// ȡ�ýڵ�ĵ�ǰ��С
	virtual FmVec3 GetNodeScale(const char* pNodeName) = 0;
	virtual void GetNodeScale(const IVarList& args, IVarList& result) = 0;

	// �жϽڵ��Ƿ����
	virtual bool NodeIsExist(const char* pNodeName) = 0;
	// ȡ�ýڵ�ĵ�ǰλ��
	virtual bool GetNodePosition(const char* pNodeName, FmVec3& vPos) = 0;
	// ȡ�ýڵ�ĵ�ǰ����
	virtual bool GetNodeAngle(const char* pNodeName, FmVec3& vAngle) = 0;

    // ȡ�ýڵ���������
    virtual bool GetNodeWorldMatrix(const char* pNodeName, FmMat4& mat) = 0;
    // ���ýڵ���������(��Ҫ����EnableUserBone������Ч)
    virtual  bool SetNodeWorldMatrix(const char* pNodeName, const FmMat4& mat) = 0;

	// ɾ����Ƥ
	virtual bool DeleteSkin(const char* pModelName) = 0;
	// �����Ƥ
	virtual bool AddSkin(const char* pszModelName, const char* pszModelFile) = 0;
	virtual bool AddSkinEx(const char* pszModelName, const char* pszModelFile, 
		const char* pszMaterialFile) = 0;

	// �����Чģ��
	virtual bool AddEffectModel(const char* em_file, const char* append_path) = 0;

	// ���Ӷ��󵽵���
	virtual void LinkToPoint(const IVarList& args, IVarList& result) = 0;
	virtual bool LinkToPoint(const char* name, const char* pointname, const PERSISTID& id, bool bEsseObj = true) = 0;
	// �ӵ���ɾ�����Ӷ���
	virtual bool UnLink(const char* name, bool bDelete) = 0;
	// ȡ�����Ӷ���ID
	virtual PERSISTID GetLinkObject(const char* name) = 0;
	// �������λ��
	virtual bool SetLinkPosition(const char* name, float x, float y, float z) = 0;
	// ������ӷ���
	virtual bool SetLinkAngle(const char* name, float x, float y, float z) = 0;
	// ������Ӵ�С
	virtual bool SetLinkScale(const char* name, float x, float y, float z) = 0;
	// �������Ӷ������Ƿ񱣳ְ󶨷���
	virtual bool SetLinkAngleKeep(const char* name, bool value) = 0;
	// ���������ٶ�
	virtual bool SetActionSpeed(const char* pszName, float fSpeed) = 0;
	virtual float GetActionSpeed(const char* pszName) = 0;
	// ������нڵ�Ŀɼ������б�
	virtual void GetVisualList(const IVarList& args, IVarList& result) = 0;
	virtual void GetLinkObjList(const IVarList& args, IVarList& result) = 0;

	// �������ں϶���������ʱ��
	virtual bool SetBlendActionEnterTime(const char* pszName, float fTime) = 0;
	// ��ȡ���ں϶���������ʱ��
	virtual float GetBlendActionEnterTime(const char* pszName) = 0;
	// �������ں϶������ڳ�ʱ��
	virtual bool SetBlendActionLeaveTime(const char* pszName, float fTime) = 0;
	// ��ȡ���ں϶������ڳ�ʱ��
	virtual float GetBlendActionLeaveTime(const char* pszName) = 0;
	// �������ں϶����Ĳ����ٶ�
	virtual bool SetBlendActionSpeed(const char* pszName, float fSpeed) = 0;
	// ��ȡ���ں϶����Ĳ����ٶ�
	virtual float GetBlendActionSpeed(const char* pszName) = 0;;
	// �������ں϶�����Ȩֵ
	virtual bool SetBlendActionWeight(const char* pszName, float fWeight) = 0;
	// ��ȡ���ں϶����Ĳ����ٶ�
	virtual float GetBlendActionWeight(const char* pszName) = 0;;
	// �������ں϶����Ƿ���ͣ
	virtual bool SetBlendActionPause(const char* pszName, bool bPause) = 0;
	// ��ȡ���ں϶����Ƿ���ͣ
	virtual bool GetBlendActionPause(const char* pszName) = 0;

	// �������λ����Ϣ
	virtual bool SetBonePosition(const char* pszName, float x, float y, float z) = 0;
	virtual void GetBonePosition(const IVarList& args, IVarList& result) = 0;
	// ���������ת��Ϣ
	virtual bool SetBoneAngle(const char* pszName, float x, float y, float z) = 0;
	virtual bool SetBoneAngleNoScale(const char* pszName, float x, float y, float z) = 0;
	virtual void GetBoneAngle(const IVarList& args, IVarList& result) = 0;
	// �������������Ϣ
	virtual bool SetBoneScale(const char* pszName, float x, float y, float z) = 0;
	virtual void GetBoneScale(const IVarList& args, IVarList& result) = 0;

	virtual bool SetKinetics(const char* pName, float fBaseValue, float fValue) = 0;
	virtual void GetKinetics(const IVarList& args, IVarList& result) = 0;

	// ����Ӷ�����
	virtual bool AddChildAction(const PERSISTID& id) = 0;
	// ��Ӹ�������
	virtual bool AddParentAction(const PERSISTID& id) = 0;
	// �ж��Ӷ������Ƿ����
	virtual bool ChildActionExist() = 0;
	// �жϸ��������Ƿ����
	virtual bool ParentActionExist() = 0;

	// �����Զ�����Ŀ�����
	virtual void SetAimObject(PERSISTID id) = 0;
	virtual PERSISTID GetAimObject() const = 0;

	// �㲿�Ƿ����ϵ���
	virtual void SetFootAnchor(bool value) = 0;
	virtual bool GetFootAnchor() const = 0;

	// �������
	virtual void SetFootFloorID(const PERSISTID& value) = 0;
	virtual PERSISTID GetFootFloorID() const = 0;

	// ���������ṹ���ı��ļ�
	virtual bool DumpBoneNodeTree(const char* file_name) = 0;

	// ��ʾ������
	virtual void SetShowHelper(bool value) = 0;
	virtual bool GetShowHelper() const = 0;

	// ��ʾ����
	virtual void SetShowBone(bool value) = 0;
	virtual bool GetShowBone() const = 0;

    // ��ȡ��ǰģ���Ƿ������ģ����ģ��
    virtual bool ExistMainModel() = 0;
    // ָ����ǰģ�ͽ�����ģ����ģ��������ײ���
    virtual void SetTraceMainModel(bool value) = 0;
    // ��ȡ��ǰģ���Ƿ������ģ����ģ��������ײ���
    virtual bool GetTraceMainModel() = 0;

    // �û����ⲿ�����ù���,�翪��RAGDOLL
    virtual void EnableUserBone(bool enable) = 0;
    // ���������,��Ҫ���ⲿ���ù�������
    virtual void UserBoneUpdate() = 0;

    // �ٶ�
    virtual void SetSpeed(float fSpeed) = 0;
    virtual float GetSpeed() = 0;

	// ��ɫ��ӰЧ��
	virtual void SetBrokenShadow(bool value) = 0;
	virtual bool GetBrokenShadow() = 0;
	
	// ��ɫ��ӰЧ������
	virtual PERSISTID GetBrokenShadowID() const = 0;

	// �ڵ��ӰЧ��
	virtual bool SetNodeBrokenShadow(const char* pName, bool value) = 0;
	virtual bool GetNodeBrokenShadow(const char* pName) = 0;

	// �ڵ��ӰЧ������
	virtual PERSISTID GetNodeBrokenShadowID(const char* pName) const = 0;
};

#endif // _VISUAL_I_ACTOR_H
