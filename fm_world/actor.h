//--------------------------------------------------------------------
// 文件名:		actor.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年12月3日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
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

// 动画组合物体

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

	// 加载数据是否成功
	virtual bool GetLoaded();
	//是否加载成功
	virtual bool GetLoadFinish();

	// 加载完成后
	bool AfterLoaded();

	virtual	bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
    virtual bool TraceSphere(const FmVec3& center, float radius);
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// 世界转换矩阵
	virtual void SetWorldMatrix(const FmMat4& mat);
	virtual void GetWorldMatrix(FmMat4& mat);

	// 全局中心点
	virtual FmVec3 GetCenter(); 
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize();
	// 模型半径
	virtual float GetRadius();

	// 动作回调对象
	void SetCalleeID(const PERSISTID& id);
	PERSISTID GetCalleeID();

	// 动作控制对象
	void SetActionControlID(const PERSISTID& value);
	PERSISTID GetActionControlID() const;

	// 动作播放速度
	bool SetActionSpeed(const char* pszName, float fSpeed);
	float GetActionSpeed(const char* pszName);

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;
	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;
	// 缩放比例
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// 是否显示包围盒
	void SetShowBoundBox(bool value);
	bool GetShowBoundBox() const;

	// 是否可见 
	virtual	void SetVisible(bool value);
	virtual	bool GetVisible() const;

	// 是否在水下
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable() const;

	// 是否直接返回碰撞到的节点对象
	void SetReturnTraceNode(bool value) { m_bReturnTraceNode = value; };
	bool GetReturnTraceNode() const { return m_bReturnTraceNode; };

	// 是否可以剪裁
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable() const;

	// 是否需要水面反射
	virtual void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	virtual bool GetWaterReflect() const { return m_bWaterReflect; }

	// 写入速度纹理标志
	virtual void SetWriteVelocity(int value);
	virtual int GetWriteVelocity() const;

	// 细节等级
	virtual void SetLevelOfDetail(float value);
	virtual float GetLevelOfDetail() const;

	// 是否异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// 骨骼动画是否需要LOD
	void SetAnimationLod(bool value) { m_bAnimationLod = value; }
	bool GetAnimationLod() const { return m_bAnimationLod; }

	// 颜色
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	// 附加颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 附加信息
	void SetExtraInfo(int value) { m_nExtraInfo = value; }
	int GetExtraInfo() const { return m_nExtraInfo; }

	// 速度
	void SetSpeed(float fSpeed);
	float GetSpeed();

	// 当前选择的节点
	void SetSelectNode(const char* name);
	const char* GetSelectNode();

	// 显示辅助点
	void SetShowHelper(bool value) { m_bShowHelper = value; }
	bool GetShowHelper() const { return m_bShowHelper; }

	// 显示摄像机
	void SetShowCamera(bool value) { m_bShowCamera = value; }
	bool GetShowCamera() const { return m_bShowCamera; }

	// 显示骨骼
	void SetShowBone(bool value) { m_bShowBone = value; }
	bool GetShowBone() const { return m_bShowBone; }

	// 获取当前模型是否存在主模（裸模）
	bool ExistMainModel();
	// 指定当前模型仅对主模（裸模）进行碰撞检测
	void SetTraceMainModel(bool value);
	// 获取当前模型是否仅对主模（裸模）进行碰撞检测
	bool GetTraceMainModel();

	// 角色残影效果
	void SetBrokenShadow(bool value);
	bool GetBrokenShadow();
	
	// 角色残影效果对象
	PERSISTID GetBrokenShadowID() const;

	// 节点残影效果
	bool SetNodeBrokenShadow(const char* pName, bool value);
	bool GetNodeBrokenShadow(const char* pName);

	// 节点残影效果对象
	PERSISTID GetNodeBrokenShadowID(const char* pName) const;

	// 获得组合体的配置文件名
	const char* GetConfigFile() const;

	// 更新根节点
	bool UpdateRoot();
	// 设置动作文件
	bool SetAction(const char* pActionName);
	bool SetActionEx(const char* pszAction, const char* pszDefaultAction, 
		const char* pszAppendPath, bool bAsyncLoad);
	// 获得动作文件名
	const char* GetActionFile() const;
	// 获得动作名列表
	void GetActionList(const IVarList& args, IVarList& result) const;
	// 动作总数
	int GetActionCount();
	// 根据动作名获得动作索引
	int GetActionIndex(const char* pszName);
	// 根据动作索引获得动作名
	const char* GetActionName(int action_index);
	// 获得指定动作的资源文件名
	const char* GetSkeletonFile(const char* pszName);
	// 动作融入时间
	bool SetActionEnterTime(const char* pszName, float fTime);
	float GetActionEnterTime(const char* pszName);
	// 动作融出时间
	bool SetActionLeaveTime(const char* pszName, float fTime);
	float GetActionLeaveTime(const char* pszName);
	// 动作基准速度
	bool SetActionBaseSpeed(const char* pszName, float fBaseSpeed);
	float GetActionBaseSpeed(const char* pszName);
	// 动作缩放系数
	float GetActionScale(const char* pszName);
	// 获得当前融合的动作列表
	void GetActionBlendList(const IVarList& args, IVarList& result);
	// 获得动作的帧数
	int GetActionFrame(const char* pszName) const;
	// 获得动作的每秒帧数
	float GetActionFPS(const char* pszName) const;
	// 获得动作的持续时间
	float GetActionSeconds(const char* pszName) const;
	// 设置当前的动作帧(浮点数)
	float GetCurrentFrameFloat(const char* pszName) const;
	// 获取当前的动作帧(浮点数)
	bool SetCurrentFrameFloat(const char* pszName, float fFrame);
	// 设置当前的动作帧(整数)
	bool SetCurrentFrame(const char* pszName, int nFrame);
	// 获取当前的动作帧(整数)
	int GetCurrentFrame(const char* pszName) const;
	// 设置指定的动作是否为位移动作
	bool SetActionTranslate(const char* pszName, int nTranslate);
	// 获取指定的动作是否为位移动作
	int GetActionTranslate(const char* pszName);
	// 获取在位移模式下，当前非位移根骨骼的相对位置
	void GetTranslatePosition(const IVarList& args, IVarList& result);
	// 设置当前动作信息
	bool SetCurrentPose(skt_pose_t* pose);
	// 取得当前动作的复本
	skt_pose_t* GetCopyPose();
	// 删除动作信息
	void DeletePose(skt_pose_t* pose);
	// 添加动作
	int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName);
	// 删除动作
	bool DeleteAction(const char* pszName);
	// 加载动作骨骼数据
	bool LoadAction(int action_index);
	// 获得动作骨骼数据是否加载成功
	bool GetActionLoaded(int action_index);

	// 获得当前在融合的动作数量
	int GetBlendActionCount();
	// 获得指定索引的已融合动作名
	const char* GetBlendActionName(int uBlendedAction);
	// 融合指定动作
	void BlendAction(const IVarList& args, IVarList& result);
	// 插入指定动作
	void InsertAction(const IVarList& args, IVarList& result);
	// 取消指定融合动作
	bool UnblendAction(const char* pszName);
	// 动作是否已融合
	bool IsActionBlended(const char* pszName);
	// 动作是否正在取消融合
	bool IsActionUnblending(const char* pszName);
	// 设置已融合动作的融入时间
	bool SetBlendActionEnterTime(const char* pszName, float fTime);
	// 获取已融合动作的融入时间
	float GetBlendActionEnterTime(const char* pszName);
	// 设置已融合动作的融出时间
	bool SetBlendActionLeaveTime(const char* pszName, float fTime);
	// 获取已融合动作的融出时间
	float GetBlendActionLeaveTime(const char* pszName);
	// 设置已融合动作的播放速度
	bool SetBlendActionSpeed(const char* pszName, float fSpeed);
	// 获取已融合动作的播放速度
	float GetBlendActionSpeed(const char* pszName);
	// 设置已融合动作的权值
	bool SetBlendActionWeight(const char* pszName, float fWeight);
	// 获取已融合动作的播放速度
	float GetBlendActionWeight(const char* pszName);
	// 设置已融合动作是否暂停
	bool SetBlendActionPause(const char* pszName, bool bPause);
	// 获取已融合动作是否暂停
	bool GetBlendActionPause(const char* pszName);
	// 获取已融合动作是否是循环动作
	bool GetBlendActionLoop(const char* pszName);
	// 清除所有混合动作
	bool ClearBlendAction();
	// 查找动作是否存在
	int FindAction(const char* pszName);

	// 取得骨骼节点列表
	void GetBoneNodeList(const IVarList& args, IVarList& result);
	// 取得辅助点节点列表
	void GetHelperNodeList(const IVarList& args, IVarList& result);
	// 取得动作相关节点列表
	void GetActionNodeList(const IVarList& args, IVarList& result);
	// 取得节点的当前的相对矩阵
	bool GetCurrentNodeMatrix(CActorNode* pNode, const char* pNodeName, 
		FmMat4* mtxTM);
    // 设置节点的当前的相对矩阵
    bool SetCurrentNodeMatrix(CActorNode* pNode, const char* pNodeName, 
        const FmMat4* mtxTM);
	// 取得节点的世界矩阵
	bool GetNodeWorldMatrix(CActorNode* pNode, const char* pNodeName, 
		FmMat4* mat);
	// 通过世界矩阵设置节点位置
	bool SetNodeWorldMatrix(CActorNode* pNode, const char* pNodeName, 
		const FmMat4* mat);

	// 取得节点的当前位置
	FmVec3 GetNodePosition(const char* pNodeName);
	bool GetNodePosition(const char* pNodeName, FmVec3& pos);
	void GetNodePosition(const IVarList& args, IVarList& result);
	// 取得节点的当前方向
	bool GetNodeAngle(const char* pNodeName, FmVec3& vAngle);
	void GetNodeAngle(const IVarList& args, IVarList& result);

	// 取得节点的当前大小
	FmVec3 GetNodeScale(const char* pNodeName);
	void GetNodeScale(const IVarList& args, IVarList& result);

	// 取得节点的世界矩阵
    bool GetNodeWorldMatrix(const char* pNodeName, FmMat4& mat);
	// 设置节点的世界矩阵
    bool SetNodeWorldMatrix(const char* pNodeName, const FmMat4& mat);

	// 判断节点是否存在
	bool NodeIsExist(const char* pNodeName);

	// 通过配置文件加载组合模型
	bool CreateFromIni(const char* pFileName);
	bool CreateFromIniEx(const char* pFileName, const char* append_path);

	// 设置默认动作
	bool SetDefaultAction(const char* pszDefaultAction);
	const char* GetDefaultAction();

	// 删除蒙皮
	bool DeleteSkin(const char* pModelName);
	// 添加蒙皮
	bool AddSkin(const char* pszModelName, const char* pszModelFile);
	bool AddSkinEx(const char* pszModelName, const char* pszModelFile, 
		const char* pszMaterialFile);

	// 添加特效模型
	bool AddEffectModel(const char* em_file, const char* append_path);

	// 添加动作事件回调触发器
	bool AddActionTrigger(const char* pszName, const char* pszTriggerName, 
		int nFrame, const char* pszString);
	// 保存动作文件
	bool SaveActionFile(const char* pszFile);
	// 保存指定动作的信息
	bool SaveAction(const char* pszActionName);
	// 删除动作事件回调触发器
	bool DeleteActionTrigger(const char* pszName, const char* pszTriggerName, 
		int nFrame);
	// 取得动作事件回调触发器列表
	void GetActionTriggerList(const IVarList& args, IVarList& result);
	// 取得动作事件回调触发器的触发桢
	int GetActionTriggerFrame(const char* actionName, const char* triggerName);
	// 取得动作事件回调触发器的附加信息
	const char* GetActionTriggerString(const char* actionName, 
		const char* triggerName);
	// 设置指定动作开始事件触发器是否开启
	bool SetActionStartTrigger(const char* actionName, bool bStartTrigger);
	// 获取指定动作开始事件触发器是否开启
	bool GetActionStartTrigger(const char* actionName);
	// 设置指定动作结束事件触发器是否开启
	bool SetActionEndTrigger(const char* actionName, bool bEndTrigger);
	// 获取指定动作结束事件触发器是否开启
	bool GetActionEndTrigger(const char* actionName);
	// 设置指定动作循环事件触发器是否开启
	bool SetActionLoopTrigger(const char* actionName, bool bLoop);
	// 获取指定动作循环事件触发器是否开启
	bool GetActionLoopTrigger(const char* actionName);
	// 设置指定动作被打断事件触发器是否开启
	bool SetActionBreakTrigger(const char* actionName, bool bBreakTrigger);
	// 获取指定动作被打断事件触发器是否开启
	bool GetActionBreakTrigger(const char* actionName);

	// 连接对象到点上
	void LinkToPoint(const IVarList& args, IVarList& result);
	bool LinkToPoint(const char* name, const char* pointname, const PERSISTID& id, bool bEsseObj = true);
	// 从点上删除连接对象
	bool UnLink(const char* name, bool bDelete);
	// 取得连接对象ID
	PERSISTID GetLinkObject(const char* name);

	// 相对连接位置
	bool SetLinkPosition(const char* name, float x, float y, float z);
	void GetLinkPosition(const IVarList& args, IVarList& result);

	// 相对连接方向
	bool SetLinkAngle(const char* name, float x, float y, float z);
	void GetLinkAngle(const IVarList& args, IVarList& result);

	// 相对连接大小
	bool SetLinkScale(const char* name, float x, float y, float z);
	void GetLinkScale(const IVarList& args, IVarList& result);

	// 保持连接对象方向是否保持绑定方向
	bool SetLinkAngleKeep(const char* name, bool value);
	bool GetLinkAngleKeep(const char* name) const;

	void GetLinkObjList(const IVarList& args, IVarList& result);

	// 获得所有节点的可见对象列表
	void GetVisualList(const IVarList& args, IVarList& result);

	// 设置子节点是否可见
	bool SetNodeVisible(const char* name, bool value);
	bool GetNodeVisible(const char* name);

	// 设置节点是否是参与碰撞和剪裁的实体
	bool SetNodeEsseObj(const char* name, bool value);
	bool GetNodeEsseObj(const char* name);

	// 设置不参与残影的节点列表
	void SetNoBrokenShadowList(const IVarList& args, IVarList& result);
	void GetNoBrokenShadowList(const IVarList& args, IVarList& result);

	// 描画残影
	void RealizeBrokenShadow();

	// 骨骼相对位移信息
	bool SetBonePosition(const char* pszName, float x, float y, float z);
	void GetBonePosition(const IVarList& args, IVarList& result);
	// 骨骼相对旋转信息
	bool SetBoneAngle(const char* pszName, float x, float y, float z);
	bool SetBoneAngleNoScale(const char* pszName, float x, float y, float z);
	void GetBoneAngle(const IVarList& args, IVarList& result);
	// 骨骼相对缩放信息
	bool SetBoneScale(const char* pszName, float x, float y, float z);
	void GetBoneScale(const IVarList& args, IVarList& result);

	bool SetKinetics(const char* pName, float fBaseValue, float fValue);
	void GetKinetics(const IVarList& args, IVarList& result);

	// 添加子动作集
	bool AddChildAction(const PERSISTID& id);
	// 添加父动作集
	bool AddParentAction(const PERSISTID& id);
	// 判断子动作集是否存在
	bool ChildActionExist();
	// 判断父动作集是否存在
	bool ParentActionExist();

	// 设置自动朝向目标对象
	void SetAimObject(PERSISTID id);
	PERSISTID GetAimObject() const;

	// 足部是否贴合地面
	void SetFootAnchor(bool value); 
	bool GetFootAnchor() const;

	// 地面对象
	void SetFootFloorID(const PERSISTID& value);
	PERSISTID GetFootFloorID() const;

	// 导出骨骼结构到文本文件
	bool DumpBoneNodeTree(const char* file_name);

    void EnableUserBone(bool enable);
    void UserBoneUpdate();

	// 需要投射影子
	virtual void SetCastShadow(bool value) { m_bCastShadow = value; }
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// 需要接受投射影子
	virtual void SetReceiveShadow(bool value) { m_bReceiveShadow = value; }
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

private:
	// 创建特效模型
	void CreateEffectModel(IIniFile* pIni, const char* append_path);
	
	// 更新包围盒
	void UpdateBoundingBox();

	// 脸朝向目标
	bool ResumeAimObjectAngle();
	bool ResumeAimObjectPitchAngle(float sub_pitch = 0.0F);
	bool ResumeAimObjectOrientAngle(float sub_orient = 0.0F);
	bool ResumeAimObjectHeadAngleZ(float sub_orient = 0.0F);
	bool UpdateAimObjectAngle();
	bool UpdateAimObjectPitchAngle();
	bool UpdateAimObjectOrientAngle();
	bool UpdateAimObjectHeadAngleZ();	// 为了让转动的时候头不歪

	// 清除足部贴地
	void ClearFootAnchor();
	// 足部贴地
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

	// 需要投射或接受阴影
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
