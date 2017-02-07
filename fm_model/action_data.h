//--------------------------------------------------------------------
// 文件名:		action_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ACTION_DATA_H
#define _ACTION_DATA_H

#include "../public/macros.h"
#include "../visual/i_model_system.h"
#include "array.h"

class IIniFile;
class CSkeleton;

// 动作触发数据
struct action_trigger_t
{
	unsigned int nNameSize;
	char* pszName;
	int	nFrame;
	char* pszString;
};

// 动作数据
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

// 动作集数据
struct action_set_t
{
	char* pszName;
	char* pszMainSkeletonName;
	array_t* pActionNodeList; // element is action_node_t
	int* pHashBuckets; // 哈希链表	
	unsigned int nBucketSize;
	CSkeleton* pMainSkeleton;
	FmVec3 vRootPoint;
	action_node_t* pLastUseHead;
	action_node_t* pLastUseTail;
};

// 动作混合数据
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
	// 屏蔽某些骨骼节点的标志数组
	int* pBoneDisable;
};

// 低优先级动作混合数据
struct not_critical_t
{
	int	nAction;
	bool bAutoDelete;
	bool bLoop;
	float fMaxBlendWeight;
	char szBoneMask[2];
	char szBoneName[32];
};

// 设置动作触发参数
void set_trigger_name_string(action_trigger_t* pTrigger, const char* pszName,
	const char* pszString);
// 设置动作节点参数
void set_action_node_string(action_node_t* pActionNode, 
	const char* pszActionName, const char* pszSkeletonName,
	const char* pszActionFileName);
// 加载动作集
action_set_t* load_action_set(const char* pszActionSetName, 
	const char* pszAppendPath);
// 卸载动作集数据
bool unload_action_set(action_set_t* pActionSet);
// 保存动作信息到配置文件
bool save_action_node(IIniFile* pFile, action_set_t* pActionSet,
	action_node_t* pActionNode);
// 保存动作集配置文件
bool save_action_set(action_set_t* pActionSet, const char* pszName);

#endif // _ACTION_DATA_H
