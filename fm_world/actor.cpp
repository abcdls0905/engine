//--------------------------------------------------------------------
// �ļ���:		actor.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��12��3��
// ������:		�ݾ���
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "actor.h"
#include "model.h"
#include "skin.h"
#include "effect_model.h"
#include "broken_shadow.h"
#include "action_control.h"
#include "../visual/i_action_player.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/var_list.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/core_file.h"
#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime
/// \file actor.cpp
/// \brief �����������

/// entity: IActorCallee
/// \brief ���嶯���¼��ص�ʵ�����
DECLARE_ENTITY(IActorCallee, 0, IEntity);

/// entity: Actor
/// \brief �����������ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(Actor,0, IVisBase);

/// readonly: const char* ConfigFile
/// \brief �����������ļ�
DECLARE_PROPERTY_GET(const char*, Actor, ConfigFile, GetConfigFile);
/// readonly: const char* ActionFile
/// \brief �������ļ�
DECLARE_PROPERTY_GET(const char*, Actor, ActionFile, GetActionFile);
/// readonly: object BrokenShadowID
/// \brief ��̬��ӰЧ��ID
DECLARE_PROPERTY_GET(PERSISTID, Actor, BrokenShadowID, GetBrokenShadowID);

/// property: bool AsyncLoad
/// \brief �Ƿ��첽����
DECLARE_PROPERTY(bool, Actor, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool AnimationLod
/// \brief ���������Ƿ���ҪLOD
DECLARE_PROPERTY(bool, Actor, AnimationLod, GetAnimationLod, SetAnimationLod);
/// property: bool Visible
/// \brief �Ƿ�ɼ�
DECLARE_PROPERTY(bool, Actor, Visible, GetVisible, SetVisible);
/// property: bool TraceEanble
/// \brief �Ƿ�����ײ
DECLARE_PROPERTY(bool, Actor, TraceEnable, GetTraceEnable, SetTraceEnable);
/// property: bool ReturnTraceNode
/// \brief �Ƿ�ֱ�ӷ�����ײ���Ľڵ����
DECLARE_PROPERTY(bool, Actor, ReturnTraceNode, GetReturnTraceNode, 
	SetReturnTraceNode);
/// property: bool CullEanble
/// \brief �Ƿ����
DECLARE_PROPERTY(bool, Actor, CullEnable, GetCullEnable, SetCullEnable);
/// property: bool WaterReflect
/// \brief �Ƿ���Ҫˮ�淴��
DECLARE_PROPERTY(bool, Actor, WaterReflect, GetWaterReflect, SetWaterReflect);
/// property: int WriteVelocity
/// \brief д���ٶ������־��0-��д�룬1-д�룬2-д���ֵ��
DECLARE_PROPERTY(int, Actor, WriteVelocity, GetWriteVelocity, 
	SetWriteVelocity);
/// property: float LevelOfDetail
/// \brief ϸ�ڵȼ�
DECLARE_PROPERTY(float, Actor, LevelOfDetail, GetLevelOfDetail, 
	SetLevelOfDetail);
/// property: bool ShowBoundBox
/// \brief �Ƿ���ʾ��Χ��
DECLARE_PROPERTY(bool, Actor, ShowBoundBox, GetShowBoundBox, SetShowBoundBox);
/// property: string Color
/// \brief ��ɫ
DECLARE_PROPERTY(result_string, Actor, Color, GetColorString, SetColorString);
/// property: int ExtraInfo
/// \brief ������Ϣ
DECLARE_PROPERTY(int, Actor, ExtraInfo, GetExtraInfo, SetExtraInfo);
/// property: float Speed
/// \brief ���������ٶ�
DECLARE_PROPERTY(float, Actor, Speed, GetSpeed, SetSpeed);
/// property: string DefaultAction
/// \brief Ĭ�϶���
DECLARE_PROPERTY(const char*, Actor, DefaultAction, GetDefaultAction, 
	SetDefaultAction);
/// property: string SelectNode
/// \brief ��ʾѡ���
DECLARE_PROPERTY(const char*, Actor, SelectNode, GetSelectNode, 
	SetSelectNode);
/// property: bool ShowBone
/// \brief ��ʾ������
DECLARE_PROPERTY(bool, Actor, ShowBone, GetShowBone, SetShowBone);
/// property: bool ShowHelper
/// \brief ��ʾ������
DECLARE_PROPERTY(bool, Actor, ShowHelper, GetShowHelper, SetShowHelper);
/// property: bool ShowCamera
/// \brief ��ʾ�����
DECLARE_PROPERTY(bool, Actor, ShowCamera, GetShowCamera, SetShowCamera);
/// property: bool BrokenShadow
/// \brief ��ӰЧ��
DECLARE_PROPERTY(bool, Actor, BrokenShadow, GetBrokenShadow, SetBrokenShadow);

/// property: bool TraceMainModel
/// \brief ��ǰģ���Ƿ������ģ����ģ��������ײ���
DECLARE_PROPERTY(bool, Actor, TraceMainModel, GetTraceMainModel, 
	SetTraceMainModel);

/// property: object Callee
/// \brief �����ص�����
DECLARE_PROPERTY(PERSISTID, Actor, Callee, GetCalleeID, SetCalleeID);
/// property: object ActionControl
/// \brief �������ƶ���
DECLARE_PROPERTY(PERSISTID, Actor, ActionControl, GetActionControlID, 
	SetActionControlID);

/// property: object AimObject
/// \brief �Զ�����Ŀ�����
DECLARE_PROPERTY(PERSISTID, Actor, AimObject, GetAimObject, SetAimObject);
/// property: bool FootAnchor
/// \brief �㲿�Ƿ����ϵ���
DECLARE_PROPERTY(bool, Actor, FootAnchor, GetFootAnchor, SetFootAnchor);
/// property: object FootFloor
/// \brief ���µĵ������
DECLARE_PROPERTY(PERSISTID, Actor, FootFloor, GetFootFloorID, SetFootFloorID);

/*
/// method: bool ExistMainModel()
/// \brief ��ȡ��ǰģ���Ƿ������ģ����ģ��
DECLARE_METHOD_0(bool, Actor, ExistMainModel);

/// method: bool AfterLoaded()
/// \brief ������ɺ�
DECLARE_METHOD_0(bool, Actor, AfterLoaded);

/// method: bool SetNodeVisible(const char* name, bool value)
/// \brief �����ӽڵ��Ƿ�ɼ�
/// \param name �ڵ���
/// \param value �Ƿ�ɼ�
/// \return �����Ƿ�ɹ�
DECLARE_METHOD_2(bool, Actor, SetNodeVisible, const char*, bool);
/// method: bool GetNodeVisible(const char* name)
/// \brief ��ȡ�ӽڵ��Ƿ�ɼ�
/// \param name �ڵ���
/// \return �Ƿ�ɼ�
DECLARE_METHOD_1(bool, Actor, GetNodeVisible, const char*);

/// method: bool SetNodeEsseObj(const char* name, bool value)
/// \brief ���ýڵ��Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
/// \param name �ڵ���
/// \param value �Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
/// \return �����Ƿ�ɹ�
DECLARE_METHOD_2(bool, Actor, SetNodeEsseObj, const char*, bool);
/// method: bool GetNodeEsseObj(const char* name)
/// \brief ���ýڵ��Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
/// \param name �ڵ���
/// \return �Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
DECLARE_METHOD_1(bool, Actor, GetNodeEsseObj, const char*);

/// method: table SetNoBrokenShadowList()
/// \brief ���ò������Ӱ�Ľڵ��б�
DECLARE_METHOD_T(Actor, SetNoBrokenShadowList);
/// method: table SetNoBrokenShadowList()
/// \brief ��ȡ�������Ӱ�Ľڵ��б�
DECLARE_METHOD_T(Actor, GetNoBrokenShadowList);

/// method: bool SetAction(string pFileName)
/// \brief ͨ�������ļ����ö�������
/// \brief pFileName �����ļ�
/// \return ���óɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(bool, Actor, SetAction, const char*);
/// method: bool SetActionEx(string pFileName, string pszDefaultAction
///		string pszAppendPath, bool bAsyncLoad)
/// \brief ͨ�������ļ����ö�������
/// \param pFileName �����ļ�
/// \param pszDefaultAction Ĭ�϶�����
/// \param pszAppendPath ����·��
/// \param bAsyncLoad �Ƿ��첽����
/// \return ���óɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_4(bool, Actor, SetActionEx, const char*, const char*, 
	const char*, bool);
/// method: table GetActionList()
/// \brief ��ö����б�
DECLARE_METHOD_T(Actor, GetActionList);
/// method: int GetActionCount()
/// \brief ��������
DECLARE_METHOD_0(int, Actor, GetActionCount);
/// method: int GetActionIndex(string pszName)
/// \brief ���ݶ�������ö�������
DECLARE_METHOD_1(int, Actor, GetActionIndex, const char*);
/// method: int GetActionName(int action_index)
/// \brief ���ݶ���������ö�����
DECLARE_METHOD_1(const char*, Actor, GetActionName, int);
/// method: table GetSkeletonFile(string pszName)
/// \brief ���ָ����������Դ�ļ�
/// \param pszName ������
/// \return ������Դ�ļ���
DECLARE_METHOD_1(const char*, Actor, GetSkeletonFile, const char*);
/// method: table GetActionBlendList()
/// \brief ������ں϶����б�
DECLARE_METHOD_T(Actor, GetActionBlendList);

/// method: bool SetActionEnterTime(string pActionName, float fTime)
/// \brief ���ö�������ʱ�ȴ�ʱ��
/// \param pActionName ������
/// \param fTime ʱ��
/// \return ���óɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, SetActionEnterTime, const char*, float);
/// method: float GetActionEnterTime(string pActionName)
/// \brief ȡ�ö�������ʱ�ȴ�ʱ��
/// \param pActionName ������
/// \return ���ض�������ʱ�ȴ�ʱ��
DECLARE_METHOD_1(float, Actor, GetActionEnterTime, const char*);
/// method: bool SetActionLeaveTime(string pActionName, float fTime)
/// \brief ����ɾ������ʱ�ĵȴ�ʱ��
/// \param pActionName ������
/// \param fTime ʱ��
/// \return ���óɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, SetActionLeaveTime, const char*, float);
/// method: float GetActionLeaveTime(string pActionName)
/// \brief ȡ��ɾ������ʱ�ĵȴ�ʱ��
/// \param pActionName ������
/// \return ����ɾ������ʱ�ĵȴ�ʱ��
DECLARE_METHOD_1(float, Actor, GetActionLeaveTime, const char*);
/// method: bool SetActionBaseSpeed(string pszName, float fBaseSpeed)
/// \brief ����ָ���Ķ����Ļ�׼�ٶ�
/// \param pszName ������
/// \param fBaseSpeed ��׼�ٶ�
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, SetActionBaseSpeed, const char*, float);
/// method: float GetActionBaseSpeed(string pszName)
/// \brief ��ȡָ���Ķ����Ļ�׼�ٶ�
/// \param pszName ������
/// \return ���ػ�׼�ٶ�
DECLARE_METHOD_1(float, Actor, GetActionBaseSpeed, const char*);
/// method: float GetActionScale(string pszName)
/// \brief ��ȡָ���Ķ���������ϵ��
/// \param pszName ������
/// \return ��������ϵ��
DECLARE_METHOD_1(float, Actor, GetActionScale, const char*);
/// method: int GetActionFrame(const char* pActionName)
/// \brief ȡ��ָ������������
/// \param pActionName ������
/// \return ���ظö�����������
DECLARE_METHOD_1(int, Actor, GetActionFrame, const char*);
/// method: float GetActionFPS(string action_name)
/// \brief ȡ��ָ��������ÿ��֡��
/// \param action_name ������
DECLARE_METHOD_1(float, Actor, GetActionFPS, const char*);
/// method: float GetActionSeconds(string action_name)
/// \brief ȡ��ָ�������ĳ���ʱ��
/// \param action_name ������
DECLARE_METHOD_1(float, Actor, GetActionSeconds, const char*);

/// method: bool SetCurrentFrameFloat(const char* pActionName, float fFrame)
/// \brief ȡ��ָ�������ĵ�ǰ֡
/// \param pActionName ������
/// \param fFrame ֡��
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_2(bool, Actor, SetCurrentFrameFloat, const char*, float);
/// method: float GetCurrentFrameFloat(const char* pActionName)
/// \brief ȡ��ָ�������ĵ�ǰ֡
/// \param pActionName ������
/// \return ���ظö����ĵ�ǰ֡
DECLARE_METHOD_1(float, Actor, GetCurrentFrameFloat, const char*);
/// method: bool SetCurrentFrame(const char* pActionName, int nFrame)
/// \brief ȡ��ָ�������ĵ�ǰ֡
/// \param pActionName ������
/// \param nFrame ֡��
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_2(bool, Actor, SetCurrentFrame, const char*, int);
/// method: int GetCurrentFrame(const char* pActionName)
/// \brief ȡ��ָ�������ĵ�ǰ֡
/// \param pActionName ������
/// \return ���ظö����ĵ�ǰ֡
DECLARE_METHOD_1(int, Actor, GetCurrentFrame, const char*);

/// method: int GetBlendActionCount()
/// \brief ��õ�ǰ���ںϵĶ�������
DECLARE_METHOD_0(int, Actor, GetBlendActionCount);
/// method: int GetBlendActionName(int uBlendedAction)
/// \brief ���ָ�����������ں϶�����
DECLARE_METHOD_1(const char*, Actor, GetBlendActionName, int);
/// method: bool BlendAction(string pszName, bool bLoop, bool bAutoDelete, 
///		bool bCritical, float fMaxBlendWeight, string bone_mask, string bone_name)
/// \brief �ں�ָ������
/// \param pszName ������
/// \param bLoop �ظ����ű�־λ
/// \param bAutoDelete �Զ�ж�ر�־λ
/// \param bCritical �Ƿ񲻵ȴ�ǰһ��������[��ѡ����,Ĭ��Ϊtrue]
/// \param fMaxBlendWeight �ں�Ȩ��[��ѡ����,Ĭ��Ϊ1.0F]
/// \param bone_mask ֻ���λ�ֻ����ָ���ڵ�Ĺ�������[��ѡ����,"open"��"close",Ĭ��Ϊ""]
/// \param bone_name �����ڵ�����[��ѡ����,Ĭ��Ϊ""]
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_N(Actor, BlendAction);
/// method: bool InsertAction(string pszAimName, string pszSrcName, bool bLoop, 
///		float fMaxBlendWeight, string bone_mask, string bone_name)
/// \brief �ڶ�����ΪpszAimName�Ķ���ǰ���붯����ΪpszSrcName�Ķ���
/// \param pszAimName ��׼������
/// \param pszSrcName Ҫ����Ķ�����
/// \param bLoop �ظ����ű�־λ
/// \param fMaxBlendWeight �ں�Ȩ��[��ѡ����,Ĭ��Ϊ1.0F]
/// \param bone_mask ֻ���λ�ֻ����ָ���ڵ�Ĺ�������[��ѡ����,"open"��"close",Ĭ��Ϊ""]
/// \param bone_name �����ڵ�����[��ѡ����,Ĭ��Ϊ""]
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_N(Actor, InsertAction);
/// method: bool IsActionBlended(string pszName)
/// \brief �����Ƿ��Ѿ��ں�
/// \param pszName ������
/// \return �Ѿ��ںϷ���true,δ�ںϷ���false
DECLARE_METHOD_1(bool, Actor, IsActionBlended, const char*);
/// method: bool IsActionUnblending(string pszName)
/// \brief �����Ƿ�����ȡ���ں���
/// \param pszName ������
/// \return �����ںϷ���true,���򷵻�false
DECLARE_METHOD_1(bool, Actor, IsActionUnblending, const char*);
/// method: bool UnblendAction(string pszName)
/// \brief ȡ��ָ���ں϶���
/// \param pszName ������
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(bool, Actor, UnblendAction, const char*);
/// method: bool SetBlendActionEnterTime(string pszName, float fTime)
/// \brief �������ں϶�������ʱ�ȴ�ʱ��
/// \param pszName ������
/// \param fTime ʱ��
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, SetBlendActionEnterTime, const char*, float);
/// method: float GetBlendActionEnterTime(string pszName)
/// \brief ȡ�����ں϶�������ʱ�ȴ�ʱ��
/// \param pszName ������
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(float, Actor, GetBlendActionEnterTime, const char*);
/// method: bool SetBlendActionEnterTime(string pszName, float fTime)
/// \brief �������ں϶����ڳ�ʱ�ȴ�ʱ��
/// \param pszName ������
/// \param fTime ʱ��
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, SetBlendActionLeaveTime, const char*, float);
/// method: float GetBlendActionEnterTime(string pszName)
/// \brief ȡ�����ں϶����ڳ�ʱ�ȴ�ʱ��
/// \param pszName ������
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(float, Actor, GetBlendActionLeaveTime, const char*);
/// method: bool SetBlendActionSpeed(string pszName, float fSpeed)
/// \brief �������ں϶����Ĳ����ٶ�
/// \param pszName ������
/// \param fSpeed �����ٶ�
DECLARE_METHOD_2(bool, Actor, SetBlendActionSpeed, const char*, float);
/// method: float GetBlendActionSpeed(string pszName)
/// \brief ȡ�����ں϶����Ĳ����ٶ�
/// \param pszName ������
DECLARE_METHOD_1(float, Actor, GetBlendActionSpeed, const char*);
/// method: bool SetBlendActionWeight(string pszName, float fWeight)
/// \brief �������ں϶����Ļ��Ȩֵ
/// \param pszName ������
/// \param fWeight ���Ȩֵ
DECLARE_METHOD_2(bool, Actor, SetBlendActionWeight, const char*, float);
/// method: float GetBlendActionWeight(string pszName)
/// \brief ȡ�����ں϶����Ļ��Ȩֵ
/// \param pszName ������
DECLARE_METHOD_1(float, Actor, GetBlendActionWeight, const char*);
/// method: bool SetBlendActionPause(string pszName, bool bPause)
/// \brief �������ں϶����Ƿ���ͣ
/// \param pszName ������
/// \param bPause �Ƿ���ͣ
DECLARE_METHOD_2(bool, Actor, SetBlendActionPause, const char*, bool);
/// method: bool GetBlendActionPause(string pszName)
/// \brief ȡ�����ں϶����Ƿ���ͣ
/// \param pszName ������
DECLARE_METHOD_1(bool, Actor, GetBlendActionPause, const char*);
/// method: bool GetBlendActionLoop(string pszName)
/// \brief ��ȡ���ں϶����Ƿ���ѭ������
/// \param pszName ������
DECLARE_METHOD_1(bool, Actor, GetBlendActionLoop, const char*);
/// method: bool ClearBlendAction()
/// \brief ����������ں϶���
DECLARE_METHOD_0(bool, Actor, ClearBlendAction);
/// method: int FindAction(string pszName)
/// \brief ���Ҷ����Ƿ����
/// \param pszName ������
/// \return �ɹ�����0��1(0��ʾ����δ�������,1��ʾ�����������),ʧ�ܷ���-1
DECLARE_METHOD_1(int, Actor, FindAction, const char*);
/// method: bool SetActionTranslate(string pszName, int nTranslate)
/// \brief ����ָ���Ķ����Ƿ�Ϊλ�ƶ���
/// \param pszName ������
/// \param nTranslate λ�ƶ�����־
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, SetActionTranslate, const char*, int);
/// method: int GetActionTranslate(string pszName)
/// \brief ��ȡָ���Ķ����Ƿ�Ϊλ�ƶ���
/// \param pszName ������
/// \return ��λ�ƶ�������1��2,���򷵻�0
DECLARE_METHOD_1(int, Actor, GetActionTranslate, const char*);
/// method: [x, y, z] GetTranslatePosition(string pszName, [int nFrame])
/// \brief ��ȡ��λ��ģʽ�£���ǰ��λ�Ƹ����������λ��
/// \param pszName ������
/// \param nFrame ָ��֡����ѡ������
DECLARE_METHOD_N(Actor, GetTranslatePosition);
/// method: bool SetActionSpeed(string pszName, float fSpeed)
/// \brief ����ָ���Ķ����Ĳ����ٶ�
///		[���������Ϊ���ַ���ʱ��ͬSetSpeed(fSpeed)]
/// \param pszName ������
/// \param fSpeed ������
/// \return �ǳɹ�����true,���򷵻�false
DECLARE_METHOD_2(bool, Actor, SetActionSpeed, const char*, float);
/// method: bool GetActionSpeed(string pszName)
///		[���������Ϊ���ַ���ʱ��ͬGetSpeed()]
/// \brief ��ȡָ���Ķ����Ĳ����ٶ�
/// \param pszName ������
/// \return ָ�������Ĳ����ٶ�
DECLARE_METHOD_1(bool, Actor, GetActionSpeed, const char*);
/// method: int AddAction(string pszName, string pszFileName, 
///		string pszActionFileName)
/// \brief ��Ӷ���
/// \param pszName ������
/// \param pszFileName �����ļ���
/// \param pszActionFileName ���ڶ�������
/// \return ����������ֵ
DECLARE_METHOD_3(int, Actor, AddAction, const char*, const char*, 
	const char*);
/// method: bool DeleteAction(string pszName)
/// \brief ɾ������
/// \param pszName ������
/// \return �Ƿ�ɾ���ɹ�
DECLARE_METHOD_1(bool, Actor, DeleteAction, const char*);
/// method: bool LoadAction(int action_index)
/// \brief ���ض�����������
/// \param action_index ��������
/// \return �Ƿ���سɹ�
DECLARE_METHOD_1(bool, Actor, LoadAction, int);
/// method: bool GetActionLoaded(int action_index)
/// \brief ��ö������������Ƿ���سɹ����
/// \param action_index ��������
/// \return �Ƿ���سɹ����
DECLARE_METHOD_1(bool, Actor, GetActionLoaded, int);

/// method: bool CreateFromIni(string pFileName)
/// \brief �������ģ��
/// \param pFileName ���ģ�������ļ�����
/// \return ���سɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(bool, Actor, CreateFromIni, const char*);
/// method: bool CreateFromIniEx(string pFileName, const char* append_path)
/// \brief �������ģ��
/// \param pFileName ���ģ�������ļ�����
/// \param append_path ��չ·����
/// \return ���سɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, CreateFromIniEx, const char*, const char*);

/// method: table GetActionNodeList()
/// \brief ��ö�����ؽڵ��б�
DECLARE_METHOD_T(Actor, GetActionNodeList);
/// method: table GetHelperNodeList()
/// \brief ��ø������б�
DECLARE_METHOD_T(Actor, GetHelperNodeList);
/// method: table GetBoneNodeList()
/// \brief ��ù����ڵ��б�
DECLARE_METHOD_T(Actor, GetBoneNodeList);
/// method: [x, y, z] GetNodePosition(string pNodeName)
/// \brief ȡ�ý�����������
/// \param pNodeName �����
DECLARE_METHOD_N(Actor, GetNodePosition);
/// method: [x, y, z] GetNodeAngle(string pNodeName)
/// \brief ȡ�ý������������ϵ�ķ���
/// \param pNodeName �����
DECLARE_METHOD_N(Actor, GetNodeAngle);
/// method: [x, y, z] GetNodeScale(string pNodeName)
/// \brief ȡ�ý������������ϵ�����ű���
/// \param pNodeName �����
DECLARE_METHOD_N(Actor, GetNodeScale);
/// method: bool NodeIsExist(string pNodeName)
/// \brief �жϽڵ��Ƿ����
/// \param pNodeName �ڵ�����
/// \return �ڵ���ڷ���true,��֮����false
DECLARE_METHOD_1(bool, Actor, NodeIsExist, const char*);

/// method: bool AddChildAction(PERSISTID id)
/// \brief ����Ӷ�����
/// \param id �ڵ�����
/// \return ��ӳɹ�����true,��֮����false
DECLARE_METHOD_1(bool, Actor, AddChildAction, PERSISTID);
/// method: bool AddParentAction(PERSISTID id)
/// \brief bool ��Ӹ�������
/// \param id �ڵ�����
/// \return ��ӳɹ�����true,��֮����false
DECLARE_METHOD_1(bool, Actor, AddParentAction, PERSISTID);
/// method: bool ChildActionExist()
/// \brief bool �Ƿ�����Ӷ�����
/// \return ���ڷ���true,��֮����false
DECLARE_METHOD_0(bool, Actor, ChildActionExist);
/// method: bool ParentActionExist()
/// \brief bool �Ƿ���ڸ�������
/// \return ��ӳɹ�����true,��֮����false
DECLARE_METHOD_0(bool, Actor, ParentActionExist);

/// method: bool AddSkin(string pszModelName, string pszModelFile)
/// \brief �����Ƥ
/// \param pszModelName ��Ƥ��
/// \param pszModelFile ��Ƥ�ļ���
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, AddSkin, const char*, const char*);
/// method: bool AddSkinEx(string pszModelName, string pszModelFile, 
///		string pszMaterialFile)
/// \brief �����Ƥ
/// \param pszModelName ��Ƥ��
/// \param pszModelFile ��Ƥ�ļ���
/// \param pszMaterialFile ��Ƥ�����ļ�
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_3(bool, Actor, AddSkinEx, const char*, const char*, 
	const char*);
/// method: bool AddEffectModel(string em_file, string append_path)
/// \brief �����Чģ��
/// \param em_file ��Чģ����
/// \param append_path ����·��
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, AddEffectModel, const char*, const char*);
/// method: bool DeleteSkin(string pModelName)
/// \brief ɾ����Ƥ
/// \param pModelName ��Ƥ��
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(bool, Actor, DeleteSkin, const char*);

/// method: bool SaveActionFile(string pszFile)
/// \brief ���涯���ļ�
/// \param pszFile �����ļ���
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(bool, Actor, SaveActionFile, const char*);
/// method: bool SaveAction(string pszActionName)
/// \brief ����ָ����������Ϣ
/// \param pszActionName ������
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_1(bool, Actor, SaveAction, const char*);
/// method: bool AddActionTrigger(string pszName, 
///		string pszTriggerName, int nFrame, string pszString)
/// \brief ��Ӷ����ص��¼�
/// \param pszName ������
/// \param pszTriggerName �¼���
/// \param nFrame �¼��ص������
/// \param pszString �����ص��¼�������Ϣ
/// \return �ɹ����ض����ص�������,ʧ��-1
DECLARE_METHOD_4(bool, Actor, AddActionTrigger, const char*, const char*, 
	int, const char*);
/// method: bool DeleteActionTrigger(string pszName, 
///		string pszTriggerName, int nFrame)
/// \brief ɾ�������ص��¼�
/// \param pszName ������
/// \param pszTriggerName �¼���
/// \param nFrame �¼��ص������
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_3(bool, Actor, DeleteActionTrigger, 
	const char*, const char*, int);
/// method: table GetActionTriggerList()
/// \brief ���ָ�������Ĵ����¼��б�
DECLARE_METHOD_T(Actor, GetActionTriggerList);
/// method: int GetActionTriggerFrame(const char* actionName, 
///		const char* triggerName);
/// \brief ȡ�ö����ص��¼���������
/// \param actionName ������
/// \param triggerName �¼���
/// \return �ɹ�����true,ʧ�ܷ���false
/// \return ���ض����ص��¼���������,ʧ��ʱ����0
DECLARE_METHOD_2(int, Actor, GetActionTriggerFrame, const char*, const char*);
/// method: const char* GetActionTriggerString(const char* actionName, 
///		const char* triggerName);
/// \brief ȡ�ö����ص��¼�������Ϣ
/// \param actionName ������
/// \param triggerName �¼���
/// \return ���ض����ص��¼�������Ϣ,ʧ��ʱ���ؿ��ַ���
DECLARE_METHOD_2(const char*, Actor, GetActionTriggerString, const char*, 
	const char*);
/// method: bool SetActionStartTrigger(const char* actionName, 
///		bool bStartTrigger)
/// \brief ����ָ��������ʼ�¼��������Ƿ���
/// \param actionName ������
/// \param bStartTrigger true��ʾ�򿪴�����,false��ʾ�رմ�����
DECLARE_METHOD_2(bool, Actor, SetActionStartTrigger, const char*, bool);
/// method: bool GetActionStartTrigger(const char* actionName)
/// \brief ��ȡָ��������ʼ�¼��������Ƿ���
/// \param actionName ������
DECLARE_METHOD_1(bool, Actor, GetActionStartTrigger, const char*);
/// method: bool SetActionEndTrigger(const char* actionName, 
///										bool bStartTrigger)
/// \brief ����ָ�����������¼��������Ƿ���
/// \param actionName ������
/// \param bStartTrigger true��ʾ�򿪴�����,false��ʾ�رմ�����
DECLARE_METHOD_2(bool, Actor, SetActionEndTrigger, const char*, bool);
/// method: bool GetActionEndTrigger(const char* actionName)
/// \brief ��ȡָ�����������¼��������Ƿ���
/// \param actionName ������
DECLARE_METHOD_1(bool, Actor, GetActionEndTrigger, const char*);
/// method: bool SetActionLoopTrigger(const char* actionName, 
///		bool bStartTrigger)
/// \brief ����ָ������ѭ���¼��������Ƿ���
/// \param actionName ������
/// \param bStartTrigger true��ʾ�򿪴�����,false��ʾ�رմ�����
DECLARE_METHOD_2(bool, Actor, SetActionLoopTrigger, const char*, bool);
/// method: bool GetActionLoopTrigger(const char* actionName)
/// \brief ��ȡָ������ѭ���¼��������Ƿ���
/// \param actionName ������
DECLARE_METHOD_1(bool, Actor, GetActionLoopTrigger, const char*);
/// method: bool SetActionBreakTrigger(const char* actionName, 
///		bool bStartTrigger)
/// \brief ����ָ������������¼��������Ƿ���
/// \param actionName ������
/// \param bStartTrigger true��ʾ�򿪴�����,false��ʾ�رմ�����
DECLARE_METHOD_2(bool, Actor, SetActionBreakTrigger, const char*, bool);
/// method: bool GetActionBreakTrigger(const char* actionName)
/// \brief ��ȡָ������������¼��������Ƿ���
/// \param actionName ������
DECLARE_METHOD_1(bool, Actor, GetActionBreakTrigger, const char*);

/// method: bool LinkToPoint(string name, string point, 
///		const PERSISTID& id, bool bEsseObj)
/// \brief ����Чģ�Ͱ󶨵����ģ��ָ������
/// \param name �󶨹�ϵ��
/// \param point �󶨵���
/// \param id �󶨶���ID
/// \param [��ѡ����,Ĭ��Ϊtrue]bEsseObj �Ƿ���Ϊ��Чʵ�壬��������ײ�ͼ���
DECLARE_METHOD_N(Actor, LinkToPoint);
/// method: bool UnLink(string name, bool bDelete)
/// \brief ȡ��ģ�Ͱ󶨵Ĺ�ϵ
/// \param name �󶨹�ϵ��
/// \param bDelete �Ƿ�ɾ��ģ��
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_2(bool, Actor, UnLink, const char*, bool);
/// method: bool SetLinkPosition(string name, float x, float y, float z)
/// \brief ��������λ��
/// \param name ���ӹ�ϵ��
/// \param x X����
/// \param y Y����
/// \param z Z����
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_4(bool, Actor, SetLinkPosition, const char*, float, float, 
	float);
/// method: bool SetLinkAngle(string name, float x, float y, float z)
/// \brief �������ӷ���
/// \param name ���ӹ�ϵ��
/// \param x X�᷽��
/// \param y Y�᷽��
/// \param z Z�᷽��
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_4(bool, Actor, SetLinkAngle, const char*, float, float, float);
/// method: bool SetLinkScale(string name, float x, float y, float z)
/// \brief �����������ű���
/// \param name ���ӹ�ϵ��
/// \param x X�����ű���
/// \param y Y�����ű���
/// \param z Z�����ű���
/// \return �ɹ�����true,ʧ�ܷ���false
DECLARE_METHOD_4(bool, Actor, SetLinkScale, const char*, float, float, float);
/// method: [x, y, z] GetLinkPosition(string pLinkName)
/// \brief ȡ�����ӵ�λ�ñ仯��Ϣ
/// \param pLinkName ���ӹ�ϵ��
DECLARE_METHOD_N(Actor, GetLinkPosition);
/// method: [x, y, z] GetLinkAngle(string pLinkName)
/// \brief ȡ�����ӵ㷽��仯��Ϣ
/// \param pLinkName ���ӹ�ϵ��
DECLARE_METHOD_N(Actor, GetLinkAngle);
/// method: [x, y, z] GetLinkScale(string pLinkName)
/// \brief ȡ�����ӵ����ű仯��Ϣ
/// \param pLinkName ���ӹ�ϵ��
DECLARE_METHOD_N(Actor, GetLinkScale);
/// method:PERSISTID GetLinkObject(string name)
/// \brief ȡ�����ӵ����Ӷ���ID
/// \param name ���ӹ�ϵ��
/// \return ����ģ��ID
DECLARE_METHOD_1(PERSISTID, Actor, GetLinkObject, const char*);
/// method:PERSISTID SetLinkAngleKeep(string name, bool value)
/// \brief �����Ƿ񱣳����ӵ㷽�򱣳ֲ���
/// \param name ���ӹ�ϵ��
/// \param value �����Ƿ񱣳ַ��򲻱�
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_2(bool, Actor, SetLinkAngleKeep, const char*, bool);
/// method:PERSISTID GetLinkAngleKeep(string name)
/// \brief ȡ���Ƿ񱣳����ӵ㷽�򱣳ֲ���
/// \param name ���ӹ�ϵ��
/// \return �����Ƿ񱣳����ӵ㷽�򲻱�
DECLARE_METHOD_1(bool, Actor, GetLinkAngleKeep, const char*);
/// method: table GetLinkObjList()
/// \brief ������Ӷ���󶨹�ϵ���б�
DECLARE_METHOD_T(Actor, GetLinkObjList);

/// method: table GetVisualList()
/// \brief ������нڵ�Ŀɼ������б�
DECLARE_METHOD_T(Actor, GetVisualList);

/// method:bool SetNodeBrokenShadow(string name, bool value)
/// \brief ���ýڵ����Ӷ����Ӱ�Ƿ��
/// \param name ���ӹ�ϵ��
/// \param value true��ʾ�򿪲�Ӱ��false��ʾ�رղ�Ӱ
/// \return ���������Ƿ�ɹ�
DECLARE_METHOD_2(bool, Actor, SetNodeBrokenShadow, const char*, bool);
/// \brief ȡ�ýڵ����Ӷ����Ӱ�Ƿ��
/// method:bool GetNodeBrokenShadow(string name)
/// \param name ���ӹ�ϵ��
/// \return ��Ӱ�Ƿ��
DECLARE_METHOD_1(bool, Actor, GetNodeBrokenShadow, const char*);
/// \brief ȡ�ýڵ����Ӷ����Ӱ��ID
/// method:PERSISTID GetNodeBrokenShadowID(string name)
/// \param name ���ӹ�ϵ��
/// \return ���ز�ӰID
DECLARE_METHOD_1(PERSISTID, Actor, GetNodeBrokenShadowID, const char*);

/// \brief ���ù���λ����Ϣ
/// method:bool SetBonePosition(string name, float x, float y, float z)
/// \param name ������
/// \param x, y, z λ����Ϣ
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_4(bool, Actor, SetBonePosition, const char*, float, float, 
	float);
/// method: [x, y, z] GetBonePosition(string name)
/// \brief ȡ�ù���λ����Ϣ
/// \param name ������
/// \param x, y, z����λ����Ϣ
DECLARE_METHOD_N(Actor, GetBonePosition);
/// \brief ���ù�����ת��Ϣ
/// method:bool SetBoneAngle(string name, float x, float y, float z)
/// \param name ������
/// \param x, y, z ��ת��Ϣ
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_4(bool, Actor, SetBoneAngle, const char*, float, float, float);
/// \brief ���ù�����ת��Ϣ�������ţ�
/// method:bool SetBoneAngleNoScale(string name, float x, float y, float z)
/// \param name ������
/// \param x, y, z ��ת��Ϣ
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_4(bool, Actor, SetBoneAngleNoScale, const char*, float, float, 
	float);
/// method: [x, y, z] GetBoneAngle(string name)
/// \brief ȡ�ù�����ת��Ϣ
/// \param name ������
/// \param x, y, z������ת��Ϣ
DECLARE_METHOD_N(Actor, GetBoneAngle);
/// \brief ���ù���������Ϣ
/// method:bool SetBoneScale(string name, float x, float y, float z)
/// \param name ������
/// \param x, y, z ������Ϣ
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_4(bool, Actor, SetBoneScale, const char*, float, float, float);
/// method: [x, y, z] GetBoneScale(string name)
/// \brief ȡ�ù���������Ϣ
/// \param name ������
/// \param x, y, z����������Ϣ
DECLARE_METHOD_N(Actor, GetBoneScale);

/// method:bool SetKinetics(string name, float fBaseValue, float fValue)
/// \brief ����Ʈ��ϵ��
/// \param name ϵ����
/// \param fBaseValue ��ʼϵ��
/// \param fValue ϵ��
/// \return �����Ƿ����óɹ�
DECLARE_METHOD_3(bool, Actor, SetKinetics, const char*, float, float);
/// method: [res, fBaseValue, fValue] GetKinetics(string name)
/// \brief ȡ��Ʈ��ϵ��
/// \param name ϵ����
/// \param res �Ƿ�ȡ�óɹ�
/// \param fBaseValue ��ʼϵ��
/// \param fValue ϵ��
DECLARE_METHOD_N(Actor, GetKinetics);

/// method: bool DumpBoneNodeTree(string file_name)
/// \brief ���������ṹ���ı��ļ�
/// \param file_name �ı��ļ���
DECLARE_METHOD_1(bool, Actor, DumpBoneNodeTree, const char*);*/

// ������Ŷ���
const unsigned int VISUAL_EXTRA_INFO_RANDOM_PLAY = 0x00000004;

static bool init_rotation_180(FmMat4* pOut)
{
	FmMatrixRotationY(pOut, 3.1415926F);
	return true;
}

static FmMat4 s_mtxRotation180;
static bool s_bInitRotation180 = init_rotation_180(&s_mtxRotation180);

// Actor

Actor::Actor()
{
	m_pModelSystem = NULL;
	m_pActionPlayer = NULL;
	m_pActionControl = NULL;
	m_vBoundMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoundMax = FmVec3(0.0F, 0.0F, 0.0F);
	m_vCenter = FmVec3(0.0F, 0.0F, 0.0F);
	m_fRadius = 0.0F;
	m_pRootNode = NULL;
	m_bShowBoundBox = false;
	m_bShowBone = false;
	m_bShowHelper = false;
	m_bShowCamera = false;
	m_bVisible = true;
	m_bUnderWater = false;
	m_bTraceEnable = true;
	m_bReturnTraceNode = false;
	m_bCullEnable = true;
	m_bWaterReflect = false;
	m_nWriteVelocity = 0;
	m_bAsyncLoad = false;
	m_bAnimationLod = true;
	m_nColor = 0xFFFFFFFF;
	m_nExtraInfo = 0;
	m_fAlphaValue = 1.0F;
	m_fLodValue = 1.0F;
	m_nBlendTick = 0;
	m_pCallee = NULL;
	m_bAutoAimObjAngle = false;
	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = false;
	m_bFootAnchor = false;
	m_bCastShadow = false;
	m_bReceiveShadow = false;
	m_fFootAnchorFactor = 0.0F;
	FmMatrixIdentity(&m_mtxWorld);
}

Actor::~Actor()
{
	if (m_pRootNode)
	{
		CORE_DELETE(m_pRootNode);
	}

	SAFE_RELEASE(m_pActionPlayer);
	ClearNoBrokenShadowList();
}

bool Actor::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
		
	if (NULL == m_pRender)
		return false;
	
	m_pModelSystem = (IModelSystem*)GetCore()->GetInterface("ModelSystem");

	if (NULL == m_pModelSystem)
	{
		return false;
	}

	m_pRootNode = CORE_NEW(CActorNode);
	m_pRootNode->SetActor(this);

	return true;
}

bool Actor::Shut()
{
	m_pRootNode->ReleaseAll();

	BrokenShadow* pBrokenShadow = (BrokenShadow*)GetCore()->GetEntity(
		m_BrokenShadow);

	SAFE_RELEASE(pBrokenShadow);
	SAFE_RELEASE(m_pModelSystem);
	SAFE_RELEASE(m_pActionPlayer);
	SAFE_RELEASE(m_pActionControl);
	SAFE_RELEASE(m_pCallee);
	
	return true;
}

void Actor::Update(float seconds)
{
	if (m_pActionControl)
	{
		m_pActionControl->Update(seconds);
	}
	
	m_bUpdatedBoundingBox = false;

	// �Զ�����Ŀ��
	if (m_bAutoAimObjAngle)
	{
		UpdateAimObjectAngle();
	}

	if (m_bFootAnchor)
	{
		UpdateFootAnchor(seconds);
	}

	if (m_pActionPlayer)
	{
#ifdef PERFORMANCE_DEBUG
		double dSkeletonBlend = performance_time();
#endif

		if (m_bAnimationLod && (m_fLodValue < 0.99F))
		{
			// ���͹����������ŵ�Ƶ��
			float fps = 5.0F + 25.0F * m_fLodValue;
			unsigned int frame_ticks = (unsigned int)(1000.0F / fps);
			unsigned int tick = Port_GetTickCount();

			if ((tick - m_nBlendTick) >= frame_ticks)
			{
				m_pActionPlayer->Blend();
				m_nBlendTick = tick;
			}
		}
		else
		{
			m_pActionPlayer->Blend();
		}

#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dSkeletonBlendTime +=
			performance_time() - dSkeletonBlend;
#endif
	}

	m_pRootNode->Update(seconds);

	if (!m_BrokenShadow.IsNull())
	{
		IVisBase* pBrokenShadow = (IVisBase*)GetCore()->GetEntity(
			m_BrokenShadow);

		if (pBrokenShadow != NULL)
		{
			// ���²�Ӱ
			pBrokenShadow->Update(seconds);

			if (((BrokenShadow*)pBrokenShadow)->GetComplete())
			{
				pBrokenShadow->Release();
				m_BrokenShadow = PERSISTID();
			}
		}
	}
}

void Actor::Realize()
{
	Assert(m_pModelSystem != NULL);

	if (!m_bVisible)
	{
		return;
	}

	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsLoadComplete())
		{
			return;
		}	
	}

#ifdef PERFORMANCE_DEBUG
	double dRealizeActor = performance_time();
#endif

	m_pRootNode->Realize();

	//if ((GetKeyState('T') & 0x8000) && (GetAsyncKeyState(VK_CONTROL) < 0))
	//{
	//	if (m_pActionPlayer != NULL)
	//	{
	//		LPFXACTION action = get_action_ref(m_pActionPlayer);

	//		if (action != NULL)
	//		{
	//			m_pModelSystem->RealizeActionBlendedList(action);
	//		}
	//	}
	//}

	if (!m_BrokenShadow.IsNull())
	{
		IVisBase* pMotionBlur = (IVisBase*)GetCore()->GetEntity(
			m_BrokenShadow);

		if (pMotionBlur)
		{
			// �軭��Ӱ
			pMotionBlur->Realize();
		}
	}

	if (m_bShowBoundBox)
	{
		if (!m_bUpdatedBoundingBox)
		{
			UpdateBoundingBox();
		}
		
		m_pModelSystem->DrawBoundingBox(&m_vBoundMin, &m_vBoundMax, 
			0xFF00FF00, 0xFF00FF00, 0.0F);
	}

	// ��ʾ��������ʾ������
	if ((m_bShowBone || m_bShowHelper || m_bShowCamera) 
		&& (m_pActionPlayer != NULL))
	{
		int node_num = m_pActionPlayer->GetNodeCount();

		for (int i = 0; i < node_num; ++i)
		{
			const char* pNodeName = m_pActionPlayer->GetNodeNameFromID(i);

			if (NULL == pNodeName)
			{
				continue;
			}

			int node_type = m_pActionPlayer->GetNodeTypeFromID(i);

			if (m_bShowHelper && (node_type == FXNODE_TYPE_HELPER))
			{
				FmVec3 pos = GetNodePosition(pNodeName);
				m_pModelSystem->DrawPoint(&pos, 
					0xFF0000FF, 0.1F, false);
			}
			else if (m_bShowBone && (node_type == FXNODE_TYPE_BONE))
			{
				FmVec3 pos = GetNodePosition(pNodeName);
				m_pModelSystem->DrawPoint(&pos, 
					0xFF00FF00, 0.1F, false);
			}
		}
	}

	// ��ʾѡ���
	if (!m_SelectNode.empty())
	{
		FmVec3 pos = GetNodePosition(m_SelectNode.c_str());
		m_pModelSystem->DrawPoint(&pos, 
			0xFFFFFF00, 0.1F, false);
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dActorRealizeTime +=
		performance_time() - dRealizeActor;
#endif
}

void Actor::RealizeShadowMap()
{
	if (!m_bVisible)
	{
		return;
	}

	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsLoadComplete())
		{
			return;
		}	
	}

	if (((m_nColor >> 24) & 0xFF) < 255)
	{
		return;
	}

	m_pRootNode->RealizeShadowMap();
}

bool Actor::GetLoaded()
{
	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsReady())
		{
			return false;
		}
	}

	return m_pRootNode->GetLoaded();
}

bool Actor::GetLoadFinish()
{
	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsLoadComplete())
		{
			return false;
		}
	}

	if (!m_pRootNode->GetLoadFinish())
	{
		return false;
	}

	m_pRootNode->UpdateNode();

	return true;
}

// ����0��ָ��ֵ֮��������
static inline float rand_float(float r)
{
	static const float s_rand_max = float(RAND_MAX_WIN);

	return r * (((float)rand()) / s_rand_max);
}

bool Actor::AfterLoaded()
{
	if (m_nExtraInfo & VISUAL_EXTRA_INFO_RANDOM_PLAY)
	{
		if (!m_DefaultAction.empty())
		{
			int frame_count = GetActionFrame(m_DefaultAction.c_str());

			if (frame_count > 0)
			{
				SetCurrentFrameFloat(m_DefaultAction.c_str(), 
					rand_float((float)frame_count));
			}
		}
	}

	return true;
}

bool Actor::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	if (!m_bVisible || !m_bTraceEnable)
	{
		return false;
	}

	int old_hit_count = result.nHitCount;
	
	if (m_pRootNode->TraceDetail(src, dst, result))
	{
		if (m_bReturnTraceNode)
		{
			return true;
		}

		if (result.nHitMax > 1)
		{
			for (int t = old_hit_count; t < result.nHitCount; ++t)
			{
				result.Hits[t].TraceID = this->GetID();
			}
		}
		else
		{
			result.Hits[0].TraceID = this->GetID();
		}

		return true;
	}

	return false;
}

bool Actor::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	if (!m_bVisible || !m_bTraceEnable)
	{
		return false;
	}

	return m_pRootNode->TraceHitted(src, dst);
}

bool Actor::TraceSphere(const FmVec3& center, float radius)
{
	if (!m_bVisible || !m_bTraceEnable)
	{
		return false;
	}

	return m_pRootNode->TraceSphere(center, radius);
}

bool Actor::TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2)
{
    if (!m_bVisible || !m_bTraceEnable)
    {
        return false;
    }

    return m_pRootNode->TraceTriangle(v0, v1, v2);
}

bool Actor::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	if (!m_bVisible || !m_bCullEnable)
	{
		return false;
	}

	return m_pRootNode->Cull(planes, plane_num, center, radius, cull_func, 
		context);
}

FmVec3 Actor::GetCenter()
{
	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}
	
	return m_vCenter;
}

FmVec3 Actor::GetBoxSize()
{
	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return m_vBoundMax - m_vBoundMin;
}

float Actor::GetRadius()
{
	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}
	
	return m_fRadius;
}

void Actor::UpdateBoundingBox()
{
	// ������ɫģ��Ϊ������ɾ�updateboundingbox�����³��������
	if (!GetLoadFinish())
	{
		return;
	}

	FmVec3 v_min(1e8, 1e8, 1e8);
	FmVec3 v_max(-1e8, -1e8, -1e8);

	if (m_bNeedUpdateNode)
	{
		m_pRootNode->UpdateNode();
		m_bNeedUpdateNode = false;
	}

	if (m_pRootNode->GetBoundingBox(v_min, v_max))
	{
		m_vBoundMin = v_min;
		m_vBoundMax = v_max;
	}
	else
	{
		m_vBoundMin.x = m_mtxWorld._41;
		m_vBoundMin.y = m_mtxWorld._42;
		m_vBoundMin.z = m_mtxWorld._43;
		m_vBoundMax = m_vBoundMin;
	}

	m_vCenter = (m_vBoundMax + m_vBoundMin) * 0.5F;
	FmVec3 len = m_vBoundMax - m_vBoundMin;
	m_fRadius = VisUtil_Vec3Length(&len) * 0.5F;
	m_bUpdatedBoundingBox = true;
}

bool Actor::SetPosition(float x, float y, float z)
{
	Assert(m_pModelSystem != NULL);
	//Assert(float3_is_valid(x, y, z));

	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;

	return true;
}

FmVec3 Actor::GetPosition() const
{
	return FmVec3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
}

bool Actor::SetAngle(float x, float y, float z)
{
	//Assert(float3_is_valid(x, y, z));

	VisUtil_SetMatrixAngle(&m_mtxWorld, x, y, z);

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;

	return true;
}

FmVec3 Actor::GetAngle() const
{
	FmVec3 angle;

	VisUtil_GetMatrixAngle(&angle, &m_mtxWorld);

	return angle;
}

bool Actor::SetScale(float x, float y, float z)
{
	//Assert(float3_is_valid(x, y, z));

	VisUtil_SetMatrixScale(&m_mtxWorld, x, y, z);

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;

	return true;
}

FmVec3 Actor::GetScale() const
{
	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, &m_mtxWorld);

	return scale;
}

void Actor::SetShowBoundBox(bool value)
{
	m_bShowBoundBox = value;
}

bool Actor::GetShowBoundBox() const
{
	return m_bShowBoundBox;
}

void Actor::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Actor::GetVisible() const
{
	return m_bVisible;
}

void Actor::SetUnderWater(bool value)
{
	if (value == m_bUnderWater)
	{
		// ��ֵû�仯
		return;
	}
	
	m_bUnderWater = value;

	if (m_pRootNode)
	{
		m_pRootNode->SetUnderWater(value);
	}
}

bool Actor::GetUnderWater() const
{
	return m_bUnderWater;
}

void Actor::SetTraceEnable(bool value)
{
	m_bTraceEnable = value;
}

bool Actor::GetTraceEnable() const
{
	return m_bTraceEnable;
}

void Actor::SetCullEnable(bool value)
{
	m_bCullEnable = value;
}

bool Actor::GetCullEnable() const
{
	return m_bCullEnable;
}

void Actor::SetWriteVelocity(int value)
{
	m_nWriteVelocity = value;

	if (m_pRootNode)
	{
		m_pRootNode->SetWriteVelocity(value);
	}
}

int Actor::GetWriteVelocity() const
{
	return m_nWriteVelocity;
}

void Actor::SetLevelOfDetail(float value)
{
	if (int(value * 100) == int(m_fLodValue * 100))
	{
		// ��ֵû���Ա仯��ʱ���Ż�����
		return;
	}
	
	m_fLodValue = value;

	if (m_pRootNode)
	{
		m_pRootNode->SetLevelOfDetail(value);
	}
}

float Actor::GetLevelOfDetail() const
{
	return m_fLodValue;
}

void Actor::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;

	if (m_pActionPlayer)
	{
		m_pActionPlayer->SetAsyncLoad(value);
	}
}

bool Actor::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

void Actor::SetColor(unsigned int value)
{
	if (value == m_nColor)
	{
		// û�仯
		return;
	}
	
	m_nColor = value;

	if (m_pRootNode)
	{
		m_pRootNode->SetColor(value);
	}
}

unsigned int Actor::GetColor() const
{
	return m_nColor;
}

void Actor::SetColorString(const char* value)
{
	Assert(value != NULL);

	m_nColor = VisUtil_ColorToInt(value);

	if (m_pRootNode)
	{
		m_pRootNode->SetColor(m_nColor);
	}
}

result_string Actor::GetColorString() const
{
	return VisUtil_IntToColor(m_nColor);
}

void Actor::SetSpeed(float fSpeed)
{
	if (m_pActionPlayer)
	{
		m_pActionPlayer->SetSpeed(fSpeed);
	}

	CVarList nodeList;

	m_pRootNode->GetVisualNodeList(nodeList);

	for (size_t i = 0; i < nodeList.GetCount(); ++i)
	{
		CActorNode* pNode = (CActorNode*)nodeList.PointerVal(i);
		IVisBase* pVisBase = (IVisBase*)pNode->GetVisBase();

		if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
		{	
			((EffectModel*)pVisBase)->SetSpeed(fSpeed);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
		{
			((Model*)pVisBase)->SetSpeed(fSpeed);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
		{
			((Actor*)pVisBase)->SetSpeed(fSpeed);
		}
	}
}

float Actor::GetSpeed()
{
	if (NULL == m_pActionPlayer)
	{
		return 0.0F;
	}

	return m_pActionPlayer->GetSpeed();
}

void Actor::SetSelectNode(const char* name)
{
	Assert(name != NULL);

	m_SelectNode = name;
}

const char*	Actor::GetSelectNode()
{
	return m_SelectNode.c_str();
}

bool Actor::ExistMainModel()
{
	return m_pRootNode->ExistMainModel();
}

void Actor::SetTraceMainModel(bool value)
{
	m_pRootNode->SetTraceMainModel(value);
}

bool Actor::GetTraceMainModel()
{
	return m_pRootNode->GetTraceMainModel();
}

void Actor::SetBrokenShadow(bool value)
{
	if (value)
	{
		BrokenShadow* pBrokenShadow = (BrokenShadow*)GetCore()->GetEntity(
			m_BrokenShadow);

		if (NULL == pBrokenShadow)
		{
			pBrokenShadow = (BrokenShadow*)GetCore()->CreateEntityArgs(
				"BrokenShadow", CVarList());

			if (NULL == pBrokenShadow)
			{
				CORE_TRACE("(Actor::SetBrokenShadow)create broken shadow failed");
				return;
			}

			pBrokenShadow->SetContext(m_pContext);
			pBrokenShadow->SetObject(this->GetID());

			m_BrokenShadow = pBrokenShadow->GetID();
		}
	}
	else
	{
		BrokenShadow* pBrokenShadow = (BrokenShadow*)GetCore()->GetEntity(
			m_BrokenShadow);

		SAFE_RELEASE(pBrokenShadow);

		m_BrokenShadow = PERSISTID();
	}
}

bool Actor::GetBrokenShadow()
{
	if (m_BrokenShadow.IsNull())
	{
		return false;
	}

	return true;
}

PERSISTID Actor::GetBrokenShadowID() const
{
	return m_BrokenShadow;
}

bool Actor::SetNodeBrokenShadow(const char* pName, bool value)
{
	Assert(pName != NULL);

	CActorNode* pNode = m_pRootNode->GetNode(pName);

	if (NULL == pNode)
	{
		return false;
	}

	if (value)
	{
		PERSISTID id = pNode->GetBrokenShadowID();
		BrokenShadow* pBrokenShadow = (BrokenShadow*)GetCore()->GetEntity(id);

		if (pBrokenShadow)
		{
			return true;
		}

		pBrokenShadow = (BrokenShadow*)GetCore()->CreateEntityArgs(
			"BrokenShadow", CVarList());

		if (NULL == pBrokenShadow)
		{
			CORE_TRACE("(Actor::SetNodeBrokenShadow)create broken shadow failed");
			return false;
		}

		pBrokenShadow->SetContext(m_pContext);
		pBrokenShadow->SetObject(pNode->GetVisualID());

		pNode->SetBrokenShadowID(pBrokenShadow->GetID());
	}
	else
	{
		PERSISTID id = pNode->GetBrokenShadowID();
		BrokenShadow* pBrokenShadow = (BrokenShadow*)GetCore()->GetEntity(id);

		SAFE_RELEASE(pBrokenShadow);

		pNode->SetBrokenShadowID(PERSISTID());
	}

	return true;
}

bool Actor::GetNodeBrokenShadow(const char* pName)
{
	Assert(pName != NULL);

	CActorNode* pNode = m_pRootNode->GetNode(pName);

	if (NULL == pNode)
	{
		return false;
	}

	return !pNode->GetBrokenShadowID().IsNull();
}

PERSISTID Actor::GetNodeBrokenShadowID(const char* pName) const
{
	Assert(pName != NULL);

	CActorNode* pNode = m_pRootNode->GetNode(pName);

	if (NULL == pNode)
	{
		return PERSISTID();
	}

	return pNode->GetBrokenShadowID();
}

void Actor::RealizeBrokenShadow()
{
	if (m_pActionPlayer)
	{
		bool old_optimize = m_pActionPlayer->GetOptimizeBySequenceId();

		m_pActionPlayer->SetOptimizeBySequenceId(false);
		m_pRootNode->Realize();
		m_pActionPlayer->SetOptimizeBySequenceId(old_optimize);
	}
	else
	{
		m_pRootNode->Realize();
	}
}

const char* Actor::GetConfigFile() const
{
	return m_ConfigFile.c_str();
}

bool Actor::UpdateRoot()
{
	m_pRootNode->SetActionPlayer(m_pActionPlayer);

	return true;
}

void Actor::GetHelperNodeList(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (m_pActionPlayer)
	{
		int node_num = m_pActionPlayer->GetNodeCount();

		for (int i = 0; i < node_num; ++i)
		{
			if (m_pActionPlayer->GetNodeTypeFromID(i) == FXNODE_TYPE_HELPER)
			{
				const char* node_name = m_pActionPlayer->GetNodeNameFromID(i);

				result.AddString(node_name);
			}
		}
	}

	//ȡ�ð�ģ���ϵĸ�����
	CVarList nodeList;

	m_pRootNode->GetVisualNodeList(nodeList);

	size_t nCount = nodeList.GetCount();

	for (size_t i = 0; i < nCount; ++i)
	{
		CActorNode* pNode = (CActorNode*)nodeList.PointerVal(i);
		const char* nodeName = pNode->GetName();
		IVisBase* pVisBase = pNode->GetVisBase();

		CVarList args;
		CVarList helperList;

		if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
		{
			EffectModel* pModel = (EffectModel*)pVisBase;
			pModel->GetHelperNameList(args, helperList);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
		{
			Model* pModel = (Model*)pVisBase;
			pModel->GetHelperNameList(args, helperList);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
		{
			Actor* pModel = (Actor*)pVisBase;
			pModel->GetHelperNodeList(args, helperList);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
		{
			Skin* pSkin = (Skin*)pVisBase;
			pSkin->GetHelperNodeList(args, helperList);
		}

		char helper_node_name[128];

		for (size_t j = 0; j < helperList.GetCount(); ++j)
		{
			const char* helperName = helperList.StringVal(j);

			SafeSprintf(helper_node_name, sizeof(helper_node_name),
				"%s::%s", nodeName, helperName);

			result.AddString(helper_node_name);
		}
	}
}

// �ֽ�ڵ����͵���
static inline const char* parse_point_name(const char* str, char* node_name,
	size_t node_name_size)
{
	const char* split = NULL;
	const char* next = strstr(str, "::");

	while (next)
	{
		split = next;
		next = strstr(next + 2, "::");
	}

	if (NULL == split)
	{
		node_name[0] = 0;
		return str;
	}

	CopyString(node_name, node_name_size, str);

	size_t k = split - str;

	if (k < node_name_size)
	{
		node_name[k] = 0;
	}

	return split + 2;
}

bool Actor::GetCurrentNodeMatrix(CActorNode* pNode, const char* pNodeName, 
	FmMat4* mtxTM)
{
	Assert(pNodeName != NULL);

	if (0 == pNodeName[0])
	{
		FmMatrixIdentity(mtxTM);
		return true;
	}

	if (m_pActionPlayer)
	{
		int node_id = m_pActionPlayer->GetNodeIDFromName(pNodeName);

		if (node_id >= 0)
		{
			if (m_pActionPlayer->GetCurrentNodeTMFromID(node_id, mtxTM))
			{
#ifdef IMODEL_ADV_NODERADIAN180
				FmMatrixMultiply(mtxTM, mtxTM, &s_mtxRotation180);
#endif
				return true;
			}
		}
	}

	if (NULL == pNode)
	{
		// ���������͸��ڵ���
		char node_name[128];

		pNodeName = parse_point_name(pNodeName, node_name, sizeof(node_name));

		if (0 == node_name[0])
		{
			return false;
		}

		pNode = m_pRootNode->GetNode(node_name);

		if (NULL == pNode)
		{
			return false;
		}
	}

	// ȡ�ð�ģ���ϵĸ�����
	IVisBase* pVisBase = pNode->GetVisBase();

	if (NULL == pVisBase)
	{
		return false;
	}

	if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
	{
		Skin* pSkin = (Skin*)pVisBase;

		if (!pSkin->GetCurrentNodeMatrix(pNodeName, mtxTM))
		{
			return false;
		}

#ifdef IMODEL_ADV_NODERADIAN180
		FmMatrixMultiply(mtxTM, mtxTM, &s_mtxRotation180);
#endif // IMODEL_ADV_NODERADIAN180
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
	{
		Model* pModel = (Model*)pVisBase;

		return pModel->GetCurrentNodeMatrix(pNodeName, mtxTM);
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
	{
		EffectModel* pEffectModel = (EffectModel*)pVisBase;

		return pEffectModel->GetCurrentNodeMatrix(pNodeName, mtxTM);
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		Actor* pActorModel = (Actor*)pVisBase;

		return pActorModel->GetCurrentNodeMatrix(NULL, pNodeName, mtxTM);
	}
	else
	{
		return false;
	}

	return true;
}

// ���ýڵ�ĵ�ǰ����Ծ���
bool Actor::SetCurrentNodeMatrix(CActorNode* pNode, const char* pNodeName, 
    const FmMat4* mtxTM)
{
    Assert(pNodeName != NULL);
    if (0 == pNodeName[0])
    {
        return false;
    }

    if (m_pActionPlayer)
    {
        int node_id = m_pActionPlayer->GetNodeIDFromName(pNodeName);

        if (node_id >= 0)
        {
#ifdef IMODEL_ADV_NODERADIAN180
            FmMat4 mtxTmp;
            FmMatrixMultiply(&mtxTmp, mtxTM, &s_mtxRotation180);
            if (m_pActionPlayer->SetCurrentNodeTMFromID(node_id, mtxTmp))
            {
                return true;
            }
#else
            if (m_pActionPlayer->SetCurrentNodeTMFromID(node_id, *mtxTM))
            {
                return true;
            }
#endif
        }
    }

    if (NULL == pNode)
    {
        // ���������͸��ڵ���
        char node_name[128];

        pNodeName = parse_point_name(pNodeName, node_name, sizeof(node_name));

        if (0 == node_name[0])
        {
            return false;
        }

        pNode = m_pRootNode->GetNode(node_name);

        if (NULL == pNode)
        {
            return false;
        }
    }

    // ȡ�ð�ģ���ϵĸ�����
    IVisBase* pVisBase = pNode->GetVisBase();

    if (NULL == pVisBase)
    {
        return false;
    }

    if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
    {
        Skin* pSkin = (Skin*)pVisBase;

#ifdef IMODEL_ADV_NODERADIAN180
        FmMat4 mtxTmp;
        FmMatrixMultiply(&mtxTmp, mtxTM, &s_mtxRotation180);
#endif // IMODEL_ADV_NODERADIAN180

        return false;
    }
    else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
    {
        Model* pModel = (Model*)pVisBase;

        return false;
    }
    else if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
    {
        EffectModel* pEffectModel = (EffectModel*)pVisBase;

        return false;
    }
    else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
    {
        Actor* pActorModel = (Actor*)pVisBase;

        return pActorModel->SetCurrentNodeMatrix(NULL, pNodeName, mtxTM);
    }
    else
    {
        return false;
    }

    return true;
}

bool Actor::NodeIsExist(const char* pNodeName)
{
	Assert(pNodeName != NULL);

	if (0 == pNodeName[0])
	{
		return true;
	}

	if (m_pActionPlayer)
	{
		int nodeID = m_pActionPlayer->GetNodeIDFromName(pNodeName);

		if (nodeID >= 0)
		{
			return true;
		}
	}

	// ���������͸��ڵ���
	char node_name[128];
	const char* point_name = parse_point_name(pNodeName, node_name, 
		sizeof(node_name));

	if (0 == node_name[0])
	{
		return false;
	}

	CActorNode* pNode = m_pRootNode->GetNode(node_name);

	if (NULL == pNode)
	{
		return false;
	}

	// ȡ�ð�ģ���ϵĸ�����
	IVisBase* pVisBase = pNode->GetVisBase();

	if (NULL == pVisBase)
	{
		return false;
	}

	if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
	{
		EffectModel* pEffectModel = (EffectModel*)pVisBase;
		int id = pEffectModel->GetNodeFromName(point_name);

		if (id >= 0)
		{
			return true;
		}
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
	{
		Model* pModel = (Model*)pVisBase;
		int id = pModel->GetNodeFromName(point_name);

		if (id >= 0)
		{
			return true;
		}
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
	{
		Skin* pSkin = (Skin*)pVisBase;
		int id = pSkin->GetNodeFromName(point_name);

		if (id >= 0)
		{
			return true;
		}
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		Actor* pActorModel = (Actor*)pVisBase;

		return pActorModel->NodeIsExist(point_name);
	}

	return false;
}

bool Actor::GetNodeWorldMatrix(CActorNode* pNode, const char* pNodeName, 
	FmMat4* mat)
{
	Assert(pNodeName != NULL);

	FmMat4 mtxNode;

	if (!GetCurrentNodeMatrix(pNode, pNodeName, &mtxNode))
	{
		return false;
	}

	if (NULL == pNode)
	{
		char node_name[128];
		
		const char* point_name = parse_point_name(pNodeName, node_name, 
			sizeof(node_name));

		//CActorNode* node = m_pRootNode->GetNode(node_name);
		pNode = m_pRootNode->GetNode(node_name);
	}

	if (pNode)
	{
		FmMat4 mtxVisual;

		pNode->GetWorldMatrix(mtxVisual);

		IVisBase* pVisBase = pNode->GetVisBase();

		if (NULL == pVisBase)
		{
			FmMatrixMultiply(mat, &mtxNode, &mtxVisual);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
		{
#ifdef IMODEL_ADV_NODERADIAN180
			FmMatrixMultiply(mat, &s_mtxRotation180, &mtxNode);
			FmMatrixMultiply(mat, mat, &mtxVisual);
#else
			FmMatrixMultiply(mat, &mtxNode, &mtxVisual);
#endif
		}
		else
		{
			FmMatrixMultiply(mat, &mtxNode, &mtxVisual);
		}
	}
	else
	{
		FmMatrixMultiply(mat, &mtxNode, &m_mtxWorld);
	}

	//Assert(mtx_is_valid(*mat));

	return true;
}

// ͨ������������ýڵ�λ��
bool Actor::SetNodeWorldMatrix(CActorNode* pNode, const char* pNodeName, 
    const FmMat4* mat)
{
    Assert(pNodeName != NULL);

    if (NULL == pNode)
    {
        char node_name[128];

        const char* point_name = parse_point_name(pNodeName, node_name, 
            sizeof(node_name));

        //CActorNode* node = m_pRootNode->GetNode(node_name);
        pNode = m_pRootNode->GetNode(node_name);
    }

    FmMat4 mtxLocal;
    FmMat4 mtxInverse;

    if (pNode)
    {
        FmMat4 mtxVisual;

        pNode->GetWorldMatrix(mtxVisual);

        IVisBase* pVisBase = pNode->GetVisBase();

        if (NULL == pVisBase)
        {
            FmMat4Inverse(&mtxInverse, NULL, &mtxVisual);
            FmMatrixMultiply(&mtxLocal, mat, &mtxInverse);
        }
        else if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
        {
#ifdef IMODEL_ADV_NODERADIAN180
            FmMat4Inverse(&mtxInverse, NULL, &mtxVisual);
            FmMatrixMultiply(&mtxLocal, mat, &mtxInverse);
            FmMatrixMultiply(&mtxLocal, &s_mtxRotation180, &mtxLocal);
#else
            FmMat4Inverse(&mtxInverse, NULL, &mtxVisual);
            FmMatrixMultiply(&mtxLocal, mat, &mtxInverse);
#endif
        }
        else
        {
            FmMat4Inverse(&mtxInverse, NULL, &mtxVisual);
            FmMatrixMultiply(&mtxLocal, mat, &mtxInverse);
        }
    }
    else
    {
        FmMat4Inverse(&mtxInverse, NULL, &m_mtxWorld);
        FmMatrixMultiply(&mtxLocal, mat, &mtxInverse);
    }

    return SetCurrentNodeMatrix(pNode, pNodeName, &mtxLocal);
}

FmVec3 Actor::GetNodePosition(const char* pNodeName)
{
	FmMat4 mtxNode;

	if (!GetNodeWorldMatrix(NULL, pNodeName, &mtxNode))
	{
		return FmVec3(m_mtxWorld._41, m_mtxWorld._42, 
			m_mtxWorld._43);
	}

	return FmVec3(mtxNode._41, mtxNode._42, mtxNode._43);
}

bool Actor::GetNodePosition(const char* pNodeName, FmVec3& pos)
{
	FmMat4 mtxNode;

	if (!GetNodeWorldMatrix(NULL, pNodeName, &mtxNode))
	{
		return false;
	}

	pos = FmVec3(mtxNode._41, mtxNode._42, mtxNode._43);

	return true;
}

void Actor::GetNodePosition(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (1 != args.GetCount() || VTYPE_STRING != args.GetType(0))
	{
		return;
	}

	const char* pNodeName= args.StringVal(0);
	FmVec3 pos = GetNodePosition(pNodeName);

	result.AddFloat(pos.x);
	result.AddFloat(pos.y);
	result.AddFloat(pos.z);
}

bool Actor::GetNodeAngle(const char* pNodeName, FmVec3& vAngle)
{
	Assert(m_pModelSystem != NULL);

	FmMat4 mtxNode;

	if (!GetNodeWorldMatrix(NULL, pNodeName, &mtxNode))
	{
		return false;
	}

	VisUtil_GetMatrixAngle(&vAngle, &mtxNode);

	return true;
}

void Actor::GetNodeAngle(const IVarList& args, IVarList& result)
{
	result.Clear();

	if ((1 != args.GetCount()) || (VTYPE_STRING != args.GetType(0)))
	{
		return;
	}

	const char* pNodeName= args.StringVal(0);
	FmVec3 angle;
	
	if (!GetNodeAngle(pNodeName, angle))
	{
		return;
	}

	result.AddFloat(angle.x);
	result.AddFloat(angle.y);
	result.AddFloat(angle.z);
}

FmVec3 Actor::GetNodeScale(const char* pNodeName)
{
	Assert(m_pModelSystem != NULL);

	FmMat4 mtxNode;

	if (!GetNodeWorldMatrix(NULL, pNodeName, &mtxNode))
	{
		return FmVec3(1.0F, 1.0F, 1.0F);
	}

	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, &mtxNode);

	return scale;
}

void Actor::GetNodeScale(const IVarList& args, IVarList& result)
{
	result.Clear();

	if ((1 != args.GetCount()) || (VTYPE_STRING != args.GetType(0)))
	{
		return;
	}

	const char* pNodeName= args.StringVal(0);
	FmVec3 scale = GetNodeScale(pNodeName);

	result.AddFloat(scale.x);
	result.AddFloat(scale.y);
	result.AddFloat(scale.z);
}

// ȡ�ýڵ���������
bool Actor::GetNodeWorldMatrix(const char* pNodeName, FmMat4& mat)
{
     return GetNodeWorldMatrix(NULL, pNodeName, &mat);
}

// ���ýڵ���������
bool Actor::SetNodeWorldMatrix(const char* pNodeName, const FmMat4& mat)
{
    return SetNodeWorldMatrix(NULL, pNodeName, &mat);
}

static inline bool is_model_key(const char* value)
{
	size_t len = strlen(value);

	if (len < 6)
	{
		return false;
	}

	if (stricmp(value + (len - 5), ".xmod"))
	{
		return false;
	}

	return true;
}

bool Actor::CreateFromIni(const char* pFileName)
{
	return CreateFromIniEx(pFileName,"");
}

bool Actor::CreateFromIniEx(const char* pFileName, const char* append_path)
{
	Assert(m_pModelSystem != NULL);
	Assert(pFileName != NULL);

	m_ConfigFile = pFileName;

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(pFileName), pFileName);

	/*
	CReadIni local_ini;
	CReadIni* pIni = (CReadIni*)m_pRender->GetCacheIni(fname);

	if (NULL == pIni)
	{
		pIni = &local_ini;
		pIni->SetFileName(fname);

		if (!pIni->LoadFromFile())
		{
			CORE_TRACE("(Actor::CreateFromIniEx)load ini failed");
			CORE_TRACE(pFileName);
			return false;
		}
	}
	*/
	//IIniFile* pIni = m_pRender->GetCacheIni(fname);
	//bool cached = (pIni != NULL);

	//if (NULL == pIni)
	//{
		IIniFile* pIni = GetCore()->CreateIniFile(fname);

		if (!pIni->LoadFromFile())
		{
			pIni->Release();
			CORE_TRACE("(Actor::CreateFromIniEx)load ini failed");
			CORE_TRACE(pFileName);
			return false;
		}
	//}

	size_t uSectionCount = pIni->GetSectionCount();

	if (uSectionCount != 1)
	{
		//if (!cached)
		//{
			pIni->Release();
		//}

		return false;
	}

	const char* sect = pIni->GetSectionByIndex(0);
	size_t uItemCount = pIni->GetSectionItemCount(0);

	if (0 == uItemCount)
	{
		//if (!cached)
		//{
			pIni->Release();
		//}

		return false;
	}

	const char* action = pIni->ReadString(sect, "Action", "");

	if (action[0] != 0)
	{
		m_DefaultAction = pIni->ReadString(sect, "DefaultAction", "");

		char pszAction[256];

		SafeSprintf(pszAction, sizeof(pszAction), "%s%s", append_path, action);
		SetActionEx(pszAction, m_DefaultAction.c_str(), append_path, 
			m_bAsyncLoad);

		for (size_t i = 0; i < uItemCount; ++i)
		{
			const char* pValue = pIni->GetSectionItemValue(0, i);

			if (!is_model_key(pValue))
			{
				continue;
			}

			const char* pItem = pIni->GetSectionItemKey(0, i);

			char pModelFile[256];
			char pMaterialFile[256] = { 0 };

			SafeSprintf(pModelFile, sizeof(pModelFile), "%s%s", append_path,
				pValue);

			char strItem[64];

			SafeSprintf(strItem, sizeof(strItem), "@%s", pItem);

			if (pIni->FindItem(sect, strItem))
			{
				SafeSprintf(pMaterialFile, sizeof(pMaterialFile), "%s%s", 
					append_path, pIni->ReadString(sect, strItem, ""));
			}

			AddSkinEx(pItem, pModelFile, pMaterialFile);
		}
	}
	else
	{
		// û�ж����ļ��ģ���Ϊ��Чģ�ʹ���
		CreateEffectModel(pIni, append_path);
	}

	const char* em_file = pIni->ReadString(sect, "EffectModel", "");

	//if (!cached)
	//{
	//	pIni->Release();
	//}

	if (em_file[0] != 0)
	{
		SafeSprintf(fname, sizeof(fname), "%s%s%s", 
			m_pRender->GetPathPrefix(em_file), append_path, em_file);

		//IIniFile* pEmIni = m_pRender->GetCacheIni(fname);
		//bool em_cached = (pEmIni != NULL);

		//if (NULL == pEmIni)
		//{
			IIniFile* pEmIni = GetCore()->CreateIniFile(fname);

			if (!pEmIni->LoadFromFile())
			{
				//if (!cached)
				//{
					pIni->Release();
				//}

				pEmIni->Release();
				CORE_TRACE("(Actor::CreateFromIniEx)load ini failed");
				CORE_TRACE(em_file);
				return false;
			}
		//}

		CreateEffectModel(pEmIni, append_path);

		//if (!em_cached)
		//{
			pEmIni->Release();
		//}
	}

	//if (!cached)
	//{
		pIni->Release();
	//}

	return true;
}

bool Actor::AddEffectModel(const char* em_file, const char* append_path)
{
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s%s", 
		m_pRender->GetPathPrefix(em_file), append_path, em_file);

	//IIniFile* pIni = m_pRender->GetCacheIni(fname);
	//bool cached = (pIni != NULL);

	//if (NULL == pIni)
	//{
		IIniFile* pIni = GetCore()->CreateIniFile(fname);

		if (!pIni->LoadFromFile())
		{
			pIni->Release();
			CORE_TRACE("(Actor::AddEffectModel)load ini failed");
			CORE_TRACE(em_file);
			return false;
		}
	//}

	CreateEffectModel(pIni, append_path);

	//if (!cached)
	//{
		pIni->Release();
	//}

	return true;
}

void Actor::CreateEffectModel(IIniFile* pIni, const char* append_path)
{
	size_t section_num = pIni->GetSectionCount();

	if (0 == section_num)
	{
		return;
	}

	for (size_t i = 0; i < section_num; ++i)
	{
		const char* sect = pIni->GetSectionByIndex(i);
		const char* pszType = pIni->ReadString(sect, "Type", "");
		const char* pszFile = pIni->ReadString(sect, "File", "");
		const char* pszName = pIni->ReadString(sect, "Name", "");
		const char* pszLinkPoint = pIni->ReadString(sect, "LinkPoint", "");

		if (StringEmpty(pszFile) || StringEmpty(pszName))
		{
			if (pIni->FindItem(sect, "Model"))
			{
				pszFile = pIni->GetFileName();
				pszName = sect;
			}
			else
			{
				continue;
			}
		}

		PERSISTID modelID;
		EffectModel* pEffectModel = NULL;
		char file_name[256];

		SafeSprintf(file_name, sizeof(file_name), "%s%s", append_path, pszFile);

		if (0 == strcmp(pszType, "Actor"))
		{
			Actor* pActor = (Actor*)GetCore()->CreateEntityArgs("Actor", 
				CVarList() );

			Assert(pActor != NULL);

			pActor->SetContext(m_pContext);
			pActor->CreateFromIniEx(file_name, append_path);

			modelID = pActor->GetID();

			if (pIni->FindItem(sect, "IsChildAction"))
			{
				// ����Ӷ�����
				AddChildAction(pActor->GetID());
				pActor->AddParentAction(GetID());
			}
		}
		else
		{
			pEffectModel = (EffectModel*)GetCore()->CreateEntityArgs(
				"EffectModel", CVarList());

			pEffectModel->SetContext(m_pContext);
			
			if (!pEffectModel->CreateFromIniEx(file_name, pszName, 
				m_bAsyncLoad, append_path))
			{
				SAFE_RELEASE(pEffectModel);
				continue;
			}

            float x = m_mtxWorld._41 + 5.0f*m_mtxWorld._11;
            float y = m_mtxWorld._42 + 5.0f*m_mtxWorld._12;
            float z = m_mtxWorld._43 + 5.0f*m_mtxWorld._13;
            pEffectModel->SetTarget(x, y, z);

			//if (pEffectModel->GetModelID().IsNull())
			//{
			//	SAFE_RELEASE(pEffectModel);
			//	continue;
			//}

			modelID = pEffectModel->GetID();
		}

		if (modelID.IsNull())
		{
			continue;
		}

		CVarList args, result;
		
		args.AddString(sect);
		args.AddString(pszLinkPoint);
		args.AddObject(modelID);
		
		const char* sEsseObj = pIni->ReadString(sect, "EsseObj", "true");
		
		if (0 == strcmp(sEsseObj, "false"))
		{
			args.AddBool(false);
		}

		// ���ӵ����
		LinkToPoint(args, result);

		CActorNode* pNode = m_pRootNode->GetNode(sect);

		if (NULL == pNode)
		{
			continue;
		}

		float pos_x = pIni->ReadFloat(sect, "offset_x", 0.0F);
		float pos_y = pIni->ReadFloat(sect, "offset_y", 0.0F);
		float pos_z = pIni->ReadFloat(sect, "offset_z", 0.0F);

		pNode->SetLocalPosition(pos_x, pos_y, pos_z);

		if (pIni->FindItem(sect, "IsChildAction"))
		{
			pNode->SetLocalAngle(0.0F, FLOAT_PI, 0.0F);
		}
		else
		{
			float angle_x = pIni->ReadFloat(sect, "angle_x", 0.0F);
			float angle_y = pIni->ReadFloat(sect, "angle_y", 0.0F);
			float angle_z = pIni->ReadFloat(sect, "angle_z", 0.0F);

			pNode->SetLocalAngle(angle_x, angle_y, angle_z);
		}
		
		float scale_x = pIni->ReadFloat(sect, "scale_x", 1.0F);
		float scale_y = pIni->ReadFloat(sect, "scale_y", 1.0F);
		float scale_z = pIni->ReadFloat(sect, "scale_z", 1.0F);

		pNode->SetLocalScale(scale_x, scale_y, scale_z);

		if (pIni->FindItem(sect, "KeepAngle"))
		{
			pNode->SetAngleKeep(true);
		}

		if (NULL != pEffectModel)
		{
			float fLifeTime = pIni->ReadFloat(sect, "LifeTime", -1.0F);

			if (fLifeTime >= 0.0F)
			{
				pEffectModel->SetLifeTime(fLifeTime);
			}

			const char* sLoop = pIni->ReadString(sect, "Loop", "true");

			if (0 == strcmp(sLoop, "false"))
			{
				pEffectModel->SetLoop(false);
			}

			const char* sIsOneOff = pIni->ReadString(sect, "IsOneOff", 
				"true");

			if (0 == strcmp(sIsOneOff, "false"))
			{
				pEffectModel->SetIsOneOff(false);
			}

			const char* sTraceEnable = pIni->ReadString(sect, 
				"TraceEnable", "false");

			if (0 == strcmp(sTraceEnable, "true"))
			{
				pEffectModel->SetTraceEnable(true);
			}

			const char* sCullEnable = pIni->ReadString(sect, "CullEnable", 
				"true");

			if (0 == strcmp(sCullEnable, "false"))
			{
				pEffectModel->SetCullEnable(false);
			}

			const char* sTraceBoxEnable = pIni->ReadString(sect, 
				"TraceBoxEnable", "true");

			if (0 == strcmp(sTraceBoxEnable, "false"))
			{
				pEffectModel->SetTraceBoxEnable(false);
			}

			const char* sShowParticle = pIni->ReadString(sect, 
				"ShowParticle", "true");

			if (0 == strcmp(sShowParticle, "false"))
			{
				pEffectModel->SetShowParticle(false);
			}
		}
	}
}

bool Actor::DeleteSkin(const char* pModelName)
{
	Assert(m_pModelSystem != NULL);
	Assert(pModelName != NULL);
	
	CActorNode* pNode = m_pRootNode->GetNode(pModelName);

	if (pNode)
	{
		IVisBase* pVisBase = pNode->GetVisBase();
		
		pNode->SetVisualID(PERSISTID());

		if (pVisBase)
		{
			pVisBase->Release();
		}
	}

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;

	return true;
}

bool Actor::AddSkin(const char* pszModelName, const char* pszModelFile)
{
	return AddSkinEx(pszModelName, pszModelFile, "");
}

bool Actor::AddSkinEx(const char* pszModelName, const char* pszModelFile, 
	const char* pszMaterialFile)
{
	Assert(m_pModelSystem != NULL);
	Assert(pszModelName != NULL);
	Assert(pszModelFile != NULL);
	Assert(pszMaterialFile != NULL);

	if (0 == pszModelName[0])
	{
		return false;
	}

	Skin* pSkin = (Skin*)GetCore()->CreateEntityArgs("Skin", 
		CVarList());

	if (NULL == pSkin)
	{
		CORE_TRACE("(Actor::AddSkin)create skin failed");
		CORE_TRACE(pszModelFile);
		return false;
	}

	pSkin->SetContext(m_pContext);
	pSkin->SetAsyncLoad(m_bAsyncLoad);
	pSkin->SetMaterialFile(pszMaterialFile);

	if (m_pActionPlayer)
	{
		if (!pSkin->AddModel(m_pActionPlayer, pszModelFile))
		{
			pSkin->Release();
			CORE_TRACE("(Actor::AddSkin)add model failed");
			CORE_TRACE(pszModelFile);
			return false;
		}
	}
	else
	{
		CORE_TRACE("(Actor::AddSkin) m_pActionPlayer is null");
		CORE_TRACE(pszModelFile);
	}

	CActorNode* pNode = m_pRootNode->GetNode(pszModelName);

	if (pNode)
	{
		IVisBase* pVisBase = pNode->GetVisBase();

		if (pVisBase)
		{
			pVisBase->Release();
		}

		pNode->SetVisualID(pSkin->GetID());
		pNode->SetParentColor(m_nColor);
		pNode->SetWriteVelocity(m_nWriteVelocity);

		m_bUpdatedBoundingBox = false;
		m_bNeedUpdateNode = true;
		return true;
	}

	pNode = m_pRootNode->AddChild(pszModelName, "", pSkin->GetID());

	if (NULL == pNode)
	{
		pSkin->Release();
		CORE_TRACE("(Actor::AddSkin)add skin failed");
		CORE_TRACE(pszModelFile);
		return false;
	}

	pNode->SetParentColor(m_nColor);
	pNode->SetWriteVelocity(m_nWriteVelocity);

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;

	return true;
}

void Actor::LinkToPoint(const IVarList& args, IVarList& result)
{
	result.Clear();

	if (args.GetCount() < 3)
	{
		result.AddBool(false);
		return;
	}

	if (VTYPE_STRING != args.GetType(0) || VTYPE_STRING != args.GetType(1) 
		|| VTYPE_OBJECT != args.GetType(2))
	{
		result.AddBool(false);
		return;
	}

	const char* name = args.StringVal(0);

	if (0 == name[0])
	{
		// ���ֲ���Ϊ��
		result.AddBool(false);
		return;
	}

	PERSISTID id = args.ObjectVal(2);

	if (id.IsNull())
	{
		result.AddBool(false);
		return;
	}

	// ȡ��Ҫ��ӽڵ�ĸ��ڵ�
	CActorNode* pPointNode = m_pRootNode;
	// ���������͸��ڵ���
	core_string pointName = args.StringVal(1);
	core_string nodeName = "";
	int pos = (int)pointName.find("::");

	while (pos >= 0)
	{
		if (nodeName.empty())
		{
			nodeName += pointName.substr(0, pos);
		}
		else
		{
			nodeName += "::" + pointName.substr(0, pos);
		}

		CActorNode* pTempNode = pPointNode->GetNode(nodeName.c_str());

		if (NULL == pTempNode)
		{
			pTempNode = pPointNode->AddChild(nodeName.c_str(), "", 
				PERSISTID());

			if (NULL == pTempNode)
			{
				CORE_TRACE("(Actor::LinkToPoint)create child failed");
				result.AddBool(false);
				return;
			}
		}

		pPointNode = pTempNode;
		pointName = pointName.substr(pos + 2, pointName.length() - pos - 2);
		pos = (int)pointName.find("::");
	}

	if (nodeName.empty())
	{
		pPointNode = m_pRootNode->GetNode(pointName.c_str());

		if (NULL == pPointNode)
		{
			pPointNode = m_pRootNode->AddChild(pointName.c_str(), "", 
				PERSISTID());

			if (NULL == pPointNode)
			{
				CORE_TRACE("(Actor::LinkToPoint)create child failed");
				result.AddBool(false);
				return;
			}
		}
	}

	// ��ӽڵ�
	CActorNode* pNode = m_pRootNode->GetNode(name);

	if (pNode)
	{
		CActorNode* pOldParentNode = pNode->GetParentNode();

		if (pOldParentNode != NULL)
		{
			pOldParentNode->DeleteChild(name);
		}
	}

	// ���ڵ��Ѿ�ȷ�����˴����õĵ����Ѿ�ȥ��ǰ׺�Ľڵ���
	pNode = pPointNode->AddChild(name, pointName.c_str(), id);

	if (NULL == pNode)
	{
		CORE_TRACE("(Actor::LinkToPoint)add child failed");
		result.AddBool(false);
		return;
	}
	
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (pVisBase)
	{
		// �����ǰ�ĸ���ɫ
		pNode->SetColor(pVisBase->GetColor());
	}

	pNode->SetParentColor(m_nColor);
	pNode->SetWriteVelocity(m_nWriteVelocity);

	if (4 == args.GetCount())
	{
		if (!args.BoolVal(3))
		{
			pNode->SetIsEsseObj(false);
			result.AddBool(true);
			return;
		}
	}

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;
	result.AddBool(true);
}

/// \param name �󶨹�ϵ��
/// \param point �󶨵���
/// \param id �󶨶���ID
/// \param [��ѡ����,Ĭ��Ϊtrue]bEsseObj �Ƿ���Ϊ��Чʵ�壬��������ײ�ͼ���
bool Actor::LinkToPoint(const char* name, const char* pointname, const PERSISTID& id, bool bEsseObj)
{
	if (0 == name[0])
	{
		// ���ֲ���Ϊ��
		return false;
	}

	if (id.IsNull())
	{
		return false;
	}

	// ȡ��Ҫ��ӽڵ�ĸ��ڵ�
	CActorNode* pPointNode = m_pRootNode;
	// ���������͸��ڵ���
	core_string pointName = pointname;
	core_string nodeName = "";
	int pos = (int)pointName.find("::");

	while (pos >= 0)
	{
		if (nodeName.empty())
		{
			nodeName += pointName.substr(0, pos);
		}
		else
		{
			nodeName += "::" + pointName.substr(0, pos);
		}

		CActorNode* pTempNode = pPointNode->GetNode(nodeName.c_str());

		if (NULL == pTempNode)
		{
			pTempNode = pPointNode->AddChild(nodeName.c_str(), "", 
				PERSISTID());

			if (NULL == pTempNode)
			{
				CORE_TRACE("(Actor::LinkToPoint)create child failed");
				return false;
			}
		}

		pPointNode = pTempNode;
		pointName = pointName.substr(pos + 2, pointName.length() - pos - 2);
		pos = (int)pointName.find("::");
	}

	if (nodeName.empty())
	{
		pPointNode = m_pRootNode->GetNode(pointName.c_str());

		if (NULL == pPointNode)
		{
			pPointNode = m_pRootNode->AddChild(pointName.c_str(), "", 
				PERSISTID());

			if (NULL == pPointNode)
			{
				CORE_TRACE("(Actor::LinkToPoint)create child failed");
				return false;
			}
		}
	}

	// ��ӽڵ�
	CActorNode* pNode = m_pRootNode->GetNode(name);

	if (pNode)
	{
		CActorNode* pOldParentNode = pNode->GetParentNode();

		if (pOldParentNode != NULL)
		{
			pOldParentNode->DeleteChild(name);
		}
	}

	// ���ڵ��Ѿ�ȷ�����˴����õĵ����Ѿ�ȥ��ǰ׺�Ľڵ���
	pNode = pPointNode->AddChild(name, pointName.c_str(), id);

	if (NULL == pNode)
	{
		CORE_TRACE("(Actor::LinkToPoint)add child failed");
		return false;
	}
	
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (pVisBase)
	{
		// �����ǰ�ĸ���ɫ
		pNode->SetColor(pVisBase->GetColor());
	}

	pNode->SetParentColor(m_nColor);
	pNode->SetWriteVelocity(m_nWriteVelocity);

	
	if (!bEsseObj)
	{
		pNode->SetIsEsseObj(false);
		return true;
	}
	
	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;
	return true;
}

bool Actor::UnLink(const char* name, bool bDelete)
{
	Assert(name != NULL);

	CActorNode* node = m_pRootNode->GetNode(name);

	if (node)
	{
		if (bDelete)
		{
			CActorNode* pParentNode = node->GetParentNode();

			if (pParentNode)
			{
				if (!pParentNode->DeleteChild(name))
				{
					return false;
				}

				m_bUpdatedBoundingBox = false;
				m_bNeedUpdateNode = true;
			}
		}
		else
		{
			node->SetVisualID(PERSISTID());
			node->ClearChild();
		}
	}

	return true;
}

bool Actor::SetLinkPosition(const char* name, float x, float y, float z)
{
	Assert(name != NULL);
	//Assert(float3_is_valid(x, y, z));

	if (m_pRootNode)
	{
		CActorNode* pNode = m_pRootNode->GetNode(name);

		if (pNode)
		{
			return pNode->SetLocalPosition(x, y, z);
		}
	}

	return false;
}

bool Actor::SetLinkAngle(const char* name, float x, float y, float z)
{
	Assert(name != NULL);
	//Assert(float3_is_valid(x, y, z));
		
	if (m_pRootNode)
	{
		CActorNode* pNode = m_pRootNode->GetNode(name);

		if (pNode)
		{
			return pNode->SetLocalAngle(x, y, z);
		}
	}

	return false;
}

bool Actor::SetLinkScale(const char* name, float x, float y, float z)
{
	Assert(name != NULL);
	//Assert(float3_is_valid(x, y, z));

	if (m_pRootNode)
	{
		CActorNode* pNode = m_pRootNode->GetNode(name);

		if (pNode)
		{
			return pNode->SetLocalScale(x, y, z);
		}
	}

	return false;
}

PERSISTID Actor::GetLinkObject(const char* name)
{
	Assert(name != NULL);

	if (0 == strcmp(name, ""))
	{
		return PERSISTID();
	}

	if (m_pRootNode)
	{
		CActorNode* pNode = m_pRootNode->GetNode(name);

		if (pNode)
		{
			IVisBase* pIVisBase = pNode->GetVisBase();

			if (pIVisBase)
			{
				return pIVisBase->GetID();
			}
		}
	}

	return PERSISTID();
}

void Actor::SetWorldMatrix(const FmMat4& mat)
{
	//Assert(mtx_is_valid(mat));

	m_mtxWorld = mat;

	m_bUpdatedBoundingBox = false;
	m_bNeedUpdateNode = true;
}

void Actor::GetWorldMatrix(FmMat4& mat)
{
	mat = m_mtxWorld;
}

void Actor::GetLinkPosition(const IVarList& args, IVarList& result)
{
	const char* pLinkName= args.StringVal(0);

	if (pLinkName)
	{
		CActorNode* pNode = m_pRootNode->GetNode(pLinkName);

		if (pNode)
		{
			FmVec3 vLinkPos = pNode->GetLocalPosition();

			//Assert(v3_is_valid(vLinkPos));

			result.AddFloat(vLinkPos.x);
			result.AddFloat(vLinkPos.y);
			result.AddFloat(vLinkPos.z);
		}
	}
}

void Actor::GetLinkAngle(const IVarList& args, IVarList& result)
{
	const char* pLinkName= args.StringVal(0);

	if (pLinkName)
	{
		CActorNode* pNode = m_pRootNode->GetNode(pLinkName);

		if (pNode)
		{
			FmVec3 vLinkAngle = pNode->GetLocalAngle();

			//Assert(v3_is_valid(vLinkAngle));

			result.AddFloat(vLinkAngle.x);
			result.AddFloat(vLinkAngle.y);
			result.AddFloat(vLinkAngle.z);
		}
	}
}

void Actor::GetLinkScale(const IVarList& args, IVarList& result)
{
	const char* pLinkName = args.StringVal(0);

	if (pLinkName)
	{
		CActorNode* pNode = m_pRootNode->GetNode(pLinkName);

		if (pNode)
		{
			FmVec3 vLinkScale = pNode->GetLocalScale();

			//Assert(v3_is_valid(vLinkScale));

			result.AddFloat(vLinkScale.x);
			result.AddFloat(vLinkScale.y);
			result.AddFloat(vLinkScale.z);
		}
	}
}

bool Actor::SetLinkAngleKeep(const char* name, bool value)
{
	CActorNode* pNode = m_pRootNode->GetNode(name);

	if (pNode)
	{
		pNode->SetAngleKeep(value);
		return true;
	}

	return false;
}

bool Actor::GetLinkAngleKeep(const char* name) const
{
	CActorNode* pNode = m_pRootNode->GetNode(name);

	if (pNode)
	{
		return pNode->GetAngleKeep();
	}

	return false;
}

void Actor::GetLinkObjList(const IVarList& args, IVarList& result)
{
	result.Clear();

	CVarList nodeList;

	m_pRootNode->GetVisualNodeList(nodeList);

	size_t nCount = nodeList.GetCount();

	for (size_t i = 0; i < nCount; ++i)
	{
		CActorNode* pNode = (CActorNode*)nodeList.PointerVal(i);
		IVisBase* pVisBase = (IVisBase*)pNode->GetVisBase();

		if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
		{
			continue;
		}

		result.AddString(pNode->GetName());
	}
}

void Actor::GetVisualList(const IVarList& args, IVarList& result)
{
	m_pRootNode->GetVisualList(result);
}

bool Actor::SetNodeVisible(const char* name, bool value)
{
	m_pRootNode->SetNodeVisible(name, value);

	return true;
}

bool Actor::GetNodeVisible(const char* name)
{
	return m_pRootNode->GetNodeVisible(name);
}

bool Actor::SetNodeEsseObj(const char* name, bool value)
{
	m_pRootNode->SetEsseObj(name, value);

	return true;
}

bool Actor::GetNodeEsseObj(const char* name)
{
	return m_pRootNode->GetEsseObj(name);
}

void Actor::SetNoBrokenShadowList(const IVarList& args, IVarList& result)
{
	ClearNoBrokenShadowList();

	size_t count = args.GetCount();

	for (size_t i = 0; i < count; ++i)
	{
		const char* name = args.StringVal(i);
		size_t name_size = strlen(name) + 1;
		char* new_value = (char*)CORE_ALLOC(name_size);

		memcpy(new_value, name, name_size);
		m_NoBrokenShadowList.push_back(new_value);
	}
}

void Actor::GetNoBrokenShadowList(const IVarList& args, IVarList& result)
{
	size_t count = m_NoBrokenShadowList.size();

	for (size_t i = 0; i < count; ++i)
	{
		result.AddString(m_NoBrokenShadowList[i]);
	}
}

