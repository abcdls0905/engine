//--------------------------------------------------------------------
// �ļ���:		action_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��13��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTION_DATA_H
#define _ACTION_DATA_H

#include "../public/macros.h"
#include "../visual/i_model_system.h"
#include "array.h"

class IIniFile;
class CSkeleton;

// ������������
struct action_trigger_t
{
	unsigned int nNameSize;
	char* pszName;
	int	nFrame;
	char* pszString;
};

// ��������
struct action_node_t
{
	unsigned int nNameSize;
	char* pszActionName;
	char* pszSkeletonName;
	char* pszActionFileName;
	float fEnterTime;
	float fLeaveTime;
	CSkeleton* pSkeleton;
	array_t* pTriggerList; // element is actoin_trigger_t
	bool bLoaded;
    bool bPreload;
	bool bEnableStartTriggerNotify;
	bool bEnableEndTriggerNotify;
	bool bEnableLoopTriggerNotify;
	bool bEnableBreakTriggerNotify;
	float fSpeed;
	float fBaseSpeed;
	float fScale;
	int nTranslate;
	int nStartFrame;
	int nEndFrame;
	int nFootstepNum;
	int* pFootsteps;
	unsigned int nActionHash;
	int nHashNext;
	unsigned int nLastUseTick;
	action_node_t* pLastUsePrev;
	action_node_t* pLastUseNext;
};

// ����������
struct action_set_t
{
	char* pszName;
	char* pszMainSkeletonName;
	array_t* pActionNodeList; // element is action_node_t
	int* pHashBuckets; // ��ϣ����	
	unsigned int nBucketSize;
	CSkeleton* pMainSkeleton;
	FmVec3 vRootPoint;
	action_node_t* pLastUseHead;
	action_node_t* pLastUseTail;
};

// �����������
struct blend_node_t
{
	int	nAction;
	bool bUnblend;
	bool bAutoDelete;
	bool bLoop;
	bool bPause;
	float fSpeed;
	float fCurrentTime;
	float fBlendTime;
	float fUnblendTime;
	float fMaxBlendWeight;
	int	nFramePrev;
	float fCurrentWeight;
	float fEnterTime;
	float fLeaveTime;
	float fLoopTriggerTimer;
	// ����ĳЩ�����ڵ�ı�־����
	int* pBoneDisable;
};

// �����ȼ������������
struct not_critical_t
{
	int	nAction;
	bool bAutoDelete;
	bool bLoop;
	float fMaxBlendWeight;
	char szBoneMask[2];
	char szBoneName[32];
};

// ���ö�����������
void set_trigger_name_string(action_trigger_t* pTrigger, const char* pszName,
	const char* pszString);
// ���ö����ڵ����
void set_action_node_string(action_node_t* pActionNode, 
	const char* pszActionName, const char* pszSkeletonName,
	const char* pszActionFileName);
// ���ض�����
action_set_t* load_action_set(const char* pszActionSetName, 
	const char* pszAppendPath);
// ж�ض���������
bool unload_action_set(action_set_t* pActionSet);
// ���涯����Ϣ�������ļ�
bool save_action_node(IIniFile* pFile, action_set_t* pActionSet,
	action_node_t* pActionNode);
// ���涯���������ļ�
bool save_action_set(action_set_t* pActionSet, const char* pszName);

#endif // _ACTION_DATA_H
