//--------------------------------------------------------------------
// 文件名:		i_action_player.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_ACTION_PLAYER_H
#define _VISUAL_I_ACTION_PLAYER_H

#include "../public/macros.h"
//#include "dx_header.h"
#include "i_model_system.h"

// 动作播放器接口

struct skt_pose_t;

class IActionPlayer
{
public:
	virtual ~IActionPlayer() = 0;

	// 释放
	virtual void Release() = 0;
	// 是否就绪
	virtual bool IsReady() = 0;
	// 是否加载结束（成功或失败）
	virtual bool IsLoadComplete() = 0;

	// 缺省动作
	virtual void SetDefaultAction(const char* value) = 0;
	virtual const char* GetDefaultAction() = 0;

	// 整体播放速度
	virtual bool SetSpeed(float fSpeed) = 0;
	virtual float GetSpeed() = 0;

	// 动作的播放速度
	virtual bool SetActionSpeed(int action_index, float fSpeed) = 0;
	virtual float GetActionSpeed(int action_index) = 0;

	// 当前位置矩阵
	virtual void SetWorldMatrix(const FmMat4* mtxWorld) = 0;
	virtual void GetWorldMatrix(FmMat4* mtxWorld) = 0;

	// 动作总数
	virtual int GetActionCount() = 0;
	// 根据动作名获得动作索引
	virtual int GetActionIndex(const char* pszName) = 0;
	// 根据动作索引获得动作名
	virtual const char* GetActionName(int action_index) = 0;
	// 设置动作名
	virtual bool SetActionName(int action_index, const char* pszName) = 0;
	// 动作的融入时间
	virtual bool SetActionEnterTime(int action_index, float fTime) = 0;
	virtual float GetActionEnterTime(int action_index) = 0;
	// 动作的融出时间
	virtual bool SetActionLeaveTime(int action_index, float fTime) = 0;
	virtual float GetActionLeaveTime(int action_index) = 0;
	// 动作的基准移动速度
	virtual bool SetActionBaseSpeed(int action_index, float fBaseSpeed) = 0;
	virtual float GetActionBaseSpeed(int action_index) = 0;
	// 动作是否带位移
	virtual bool SetActionTranslate(int action_index, int nTranslate) = 0;
	virtual int GetActionTranslate(int action_index) = 0;
	// 动作的播放起始帧
	virtual bool SetActionStartFrame(int action_index, int frame) = 0;
	virtual int GetActionStartFarme(int action_index) = 0;
	// 动作的播放结束帧
	virtual bool SetActionEndFrame(int action_index, int frame) = 0;
	virtual int GetActionEndFarme(int action_index) = 0;
	// 设置动作的脚步帧信息
	virtual bool SetActionFootstep(int action_index, int footstep_num,
		int* footsteps) = 0;
	// 获得动作脚步帧信息数量
	virtual int GetActionFootstepCount(int action_index) = 0;
	// 获得指定索引的脚步帧信息
	virtual int GetActionFootstepFrame(int action_index, int step_index) = 0;
	// 获得动作缩放系数
	virtual float GetActionScale(int action_index) = 0;
	// 获得动作的骨骼动画文件名
	virtual const char* GetActionSkeletonName(int action_index) = 0;

	// 加载动作骨骼数据
	virtual bool LoadAction(int action_index) = 0;
	// 获得动作骨骼数据是否加载成功
	virtual bool GetActionLoaded(int action_index) = 0;
	// 获得动作骨骼数据是否完成加载（成功或失败）
	virtual bool GetActionLoadFinish(int action_index) = 0;

	// 注册动作回调
	virtual void RegistActionTriggerProc(ACTION_TRIGGER_PROC proc) = 0;
	// 注销动作回调
	virtual void UnregistActionTriggerProc() = 0;
	// 设置动作回调上下文指针
	virtual void SetUserPointer(void* pUserPointer) = 0;
	// 获得动作回调上下文指针
	virtual void* GetUserPointer() = 0;
	// 获得动作回调函数指针
	virtual void GetActionTriggerProc(ACTION_TRIGGER_PROC* pProc) = 0;
	// 获得自定义动作回调数量
	virtual int GetActionTriggerCount(int action_index) = 0;
	// 获得自定义触发名
	virtual const char* GetActionTriggerName(int action_index, int nTrigger) = 0;
	// 获得自定义触发所在帧数
	virtual int GetActionTriggerFrame(int action_index, int nTrigger) = 0;
	// 添加自定义动作触发
	virtual int AddActionTrigger(int action_index, const char* pszTriggerName, 
		int nFrame, const char* pszString) = 0;
	// 删除自定义动作触发
	virtual bool DeleteActionTrigger(int action_index, 
		const char* pszTriggerName, int nFrame) = 0;
	// 设置自定义触发字符串
	virtual bool SetActionTriggerString(int action_index, 
		const char* pszTriggerName, int nFrame, const char* pszString) = 0;
	// 获得自定义触发字符串
	virtual const char* GetActionTriggerString(int action_index, 
		const char* pszTriggerName, int nFrame) = 0;
	// 动作开始触发通知	
	virtual bool GetActionStartTriggerNotify(int action_index) = 0;
	virtual bool SetActionStartTriggerNotify(int action_index, 
		bool bStartTrigger) = 0;
	// 动作结束触发通知	
	virtual bool GetActionEndTriggerNotify(int action_index) = 0;
	virtual bool SetActionEndTriggerNotify(int action_index, 
		bool bEndTrigger) = 0;
	// 动作循环触发通知	
	virtual bool GetActionLoopTriggerNotify(int action_index) = 0;
	virtual bool SetActionLoopTriggerNotify(int action_index, 
		bool bLoopTrigger) = 0;
	// 动作中断触发通知	
	virtual bool GetActionBreakTriggerNotify(int action_index) = 0;
	virtual bool SetActionBreakTriggerNotify(int action_index, 
		bool bBreakTrigger) = 0;

	// 获得当前在融合的动作数量
	virtual int GetBlendActionCount() = 0;
	// 获得指定索引的已融合动作名
	virtual const char* GetBlendActionName(int uBlendedAction) = 0;
	// 融入动作
	virtual bool BlendAction(int action_index, bool bLoop, bool bAutoDelete, 
		bool bCritical, float fMaxBlendWeight, const char* bone_mask, 
		const char* bone_name) = 0;
	// 插入动作
	virtual bool InsertAction(int nAimAction, int nSrcAction, bool bLoop, 
		float fMaxBlendWeight, const char* bone_mask, const char* bone_name) = 0;
	// 融出动作
	virtual bool UnblendAction(int action_index) = 0;
	// 指定动作是否已经融入
	virtual bool IsActionBlended(int action_index) = 0;
	// 指定动作是否正在融出
	virtual bool IsActionUnblending(int action_index) = 0;
	// 设置混合动作融入时间
	virtual bool SetBlendActionEnterTime(int action_index, float fTime) = 0;
	// 获得混合动作融入时间
	virtual float GetBlendActionEnterTime(int action_index) = 0;
	// 设置混合动作融出时间
	virtual bool SetBlendActionLeaveTime(int action_index, float fTime) = 0;
	// 获得混合动作融出时间
	virtual float GetBlendActionLeaveTime(int action_index) = 0;
	// 设置混合动作播放速度
	virtual bool SetBlendActionSpeed(int action_index, float fSpeed) = 0;
	// 获得混合动作播放速度
	virtual float GetBlendActionSpeed(int action_index) = 0;
	// 设置混合动作权值
	virtual bool SetBlendActionWeight(int action_index, float fWeight) = 0;
	// 获得混合动作权值
	virtual float GetBlendActionWeight(int action_index) = 0;
	// 设置动作是否暂停
	virtual bool SetBlendActionPause(int action_index, bool value) = 0;
	// 获得动作是否暂停
	virtual bool GetBlendActionPause(int action_index) = 0;
	// 获取已融合动作是否是循环动作
	virtual bool GetBlendActionLoop(int action_index) = 0;
	// 清除所有动作
	virtual bool ClearBlendAction() = 0;
	// 执行动作混合
	virtual bool Blend() = 0;

	// 创建骨架位置数据
	virtual skt_pose_t* CreateSkeletonPose() = 0;
	// 删除骨架位置数据
	virtual void ReleaseSkeletonPose(skt_pose_t* pose) = 0;
	// 拷贝骨架位置数据
	virtual void CopySkeletonPose(skt_pose_t* dst, skt_pose_t* src) = 0;
	// 设置当前骨架位置数据
	virtual void SetSkeletonPose(skt_pose_t* pose) = 0;
	// 获得当前骨架位置数据
	virtual skt_pose_t* GetSkeletonPose() = 0;

	// 设置父动作集
	virtual void SetParentAction(void* parent) = 0;
	// 获得父动作集
	virtual void* GetParentAction() = 0;
	// 设置子动作集
	virtual void SetChildAction(void* child) = 0;
	// 获得子动作集
	virtual void* GetChildAction() = 0;

	// 获得动作的当前帧
	virtual float GetCurrentFrame(int action_index) = 0;
	// 设置动作的当前帧
	virtual bool SetCurrentFrame(int action_index, float fFrame) = 0;
	// 获得动作帧数
	virtual int GetFrameCount(int action_index) = 0;
	// 获得动作数据每秒帧数
	virtual float GetInternalFPS(int action_index) = 0;

	// 增加动作
	virtual int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName) = 0;
	// 删除动作
	virtual bool DeleteAction(const char* pszName) = 0;

	// 设置动力学计算参数值
	virtual bool SetKineticsCoefficient(int kinetics_index, int blender_index, 
		float fBaseValue, float fValue) = 0;
	// 获得动力学计算参数值
	virtual bool GetKineticsCoefficient(int kinetics_index, int blender_index, 
		float* fBaseValue, float* fValue) = 0;

	// 获得根骨骼的初始位置
	virtual void GetRootBoneOriginPosition(FmVec3* vPos) = 0;

	// 骨骼的附加位置
	virtual bool SetBonePositionAppend(const char* pszName, 
		const FmVec3& vPos) = 0;
	virtual bool GetBonePositionAppend(const char* pszName, FmVec3* vPos) = 0;
	// 骨骼的附加角度
	virtual bool SetBoneAngleAppend(const char* pszName, 
		const FmVec3& vAngle) = 0;
	virtual bool GetBoneAngleAppend(const char* pszName, FmVec3* vAngle) = 0;
	// 设置骨骼的附加角度（无缩放）
	virtual bool SetBoneAngleAppendNoScale(const char* pszName, 
		const FmVec3& vAngle) = 0;
	// 骨骼的附加旋转
	virtual bool SetBoneRotateAppend(const char* pszName, 
		const FmQuat& rotate) = 0;
	virtual bool GetBoneRotateAppend(const char* pszName, 
		FmQuat* rotate) = 0;
	// 骨骼的附加旋转（无缩放）
	virtual bool SetBoneRotateAppendNoScale(const char* pszName, 
		const FmQuat& rotate) = 0;
	virtual bool GetBoneRotateAppendNoScale(const char* pszName, 
		FmQuat* rotate) = 0;
	// 骨骼的附加缩放
	virtual bool SetBoneScaleAppend(const char* pszName, 
		const FmVec3& vScale) = 0;
	virtual bool GetBoneScaleAppend(const char* pszName, FmVec3* vScale) = 0;

	// 获得节点数量
	virtual int GetNodeCount() = 0;
	// 获得指定节点的当前矩阵（已修正）
	virtual bool GetCurrentNodeTMFromID(int nNodeID, FmMat4* mtxTM) = 0;
	// 获得指定节点的当前矩阵
	virtual bool SetCurrentNodeTMFromID(int nNodeID, const FmMat4& mtxTM) = 0;
	// 获得指定节点的修正矩阵
	virtual bool GetChangeNodeTMFromID(int nNodeID, FmMat4* mtxTM) = 0;
	// 设置指定节点的修正矩阵
	virtual bool SetChangeNodeTMFromID(int nNodeID, const FmMat4& mtxTM) = 0;
	// 获得指定索引的节点名
	virtual const char* GetNodeNameFromID(int nNodeID) = 0;
	// 获得指定索引的节点类型
	virtual FXNODE_TYPE GetNodeTypeFromID(int nNodeID) = 0;
	// 获得指定名字的节点索引
	virtual int GetNodeIDFromName(const char* pszName) = 0;
	// 获得节点当前位置（未修正）
	virtual bool GetNodePositionFromID(int nNodeID, FmVec3* position) = 0;
	// 获得节点当前旋转（未修正）
	virtual bool GetNodeRotateFromID(int nNodeID, FmQuat* rotate) = 0;
	// 获得节点当前缩放（未修正）
	virtual bool GetNodeScaleFromID(int nNodeID, FmVec3* scale) = 0;
	// 获得指定动作指定帧的无位移根节点位置
	virtual bool GetRootNodeTMNoTransform(int action_index, int nFrame,
		FmMat4* mtxTM) = 0;
	// 获得指定动作的无位移根节点位置
	virtual bool GetCurrentRootNodeTMNoTransform(int action_index, 
		FmMat4* mtxTM) = 0;
	// 获得当前根节点的矩阵
	virtual void GetCurrentRootNodeTM(FmMat4* mtxTM) = 0;
	// 获得骨骼数量
	virtual int GetBoneNodeCount() = 0;
	// 获得指定索引的骨骼名
	virtual const char* GetBoneNameFromID(int nNodeID) = 0;

	// 保存动作集到文件
	virtual bool SaveToFile(const char* file_name) = 0;
	// 保存动作数据到文件
	virtual bool SaveActionToFile(const char* action_name) = 0;

	// 导出骨骼信息
	virtual bool DumpBoneNodeTree(const char* file_name) = 0;

	// 设置是否异步加载
	virtual void SetAsyncLoad(bool value) = 0;

	// 是否通过帧序号优化
	virtual void SetOptimizeBySequenceId(bool value) = 0;
	virtual bool GetOptimizeBySequenceId() = 0;

	virtual void * GetBonePtr(const char* pszName) = 0;

	virtual const char* GetBoneChildsName(void* ptr, int i) = 0;

	virtual size_t GetBondChildsCount(void* ptr) = 0;

    // 用户在外部重设置骨骼,如开启RAGDOLL
    virtual void EnableUserBone(bool enable) = 0;
    // 重设骨骼后,需要由外部调用骨骼更新
    virtual void UserBoneUpdate() = 0;
};

inline IActionPlayer::~IActionPlayer() {}

#endif // _VISUAL_I_ACTION_PLAYER_H

