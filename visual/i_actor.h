//--------------------------------------------------------------------
// 文件名:		i_actor.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年2月28日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_ACTOR_H
#define _VISUAL_I_ACTOR_H

#include "i_vis_base.h"

// 组合模型接口

class IActor: public IVisBase
{
public:
	// 通过配置文件加载组合模型
	virtual bool CreateFromIni(const char* pFileName) = 0;
	virtual bool CreateFromIniEx(const char* pFileName, const char* append_path) = 0;
	// 设置动作文件
	virtual bool SetActionEx(const char* pActionName, const char* pszDefaultAction, const char* pszAppendPath, bool bAsyncLoad) = 0;
	// 动作总数
	virtual int GetActionCount() = 0;
	// 根据动作名获得动作索引
	virtual int GetActionIndex(const char* pszName) = 0;

	// 根据动作索引获得动作名
	virtual const char* GetActionName(int action_index) = 0;
	// 查找动作是否存在
	virtual int FindAction(const char* pszName) = 0;

	// 取得辅助点节点列表
	virtual void GetHelperNodeList(const IVarList& args, IVarList& result) = 0;

	// 获得动作基准速度
	virtual float GetActionBaseSpeed(const char* pszName) = 0;
	// 获得当前融合的动作列表
	virtual void GetActionBlendList(const IVarList& args, IVarList& result) = 0;
	// 获得动作的帧数
	virtual int GetActionFrame(const char* pszName) const  = 0;
	// 获得动作的每秒帧数
	virtual float GetActionFPS(const char* pszName) const  = 0;
	// 获得动作的持续时间
	virtual float GetActionSeconds(const char* pszName) const = 0;
	// 设置当前的动作帧(浮点数)
	virtual float GetCurrentFrameFloat(const char* pszName) const = 0;
	// 获取当前的动作帧(浮点数)
	virtual  bool SetCurrentFrameFloat(const char* pszName, float fFrame) = 0;
	// 设置当前的动作帧(整数)
	virtual bool SetCurrentFrame(const char* pszName, int nFrame) = 0;
	// 获取当前的动作帧(整数)
	virtual int GetCurrentFrame(const char* pszName) const = 0;
	// 设置指定的动作是否为位移动作
	virtual bool SetActionTranslate(const char* pszName, int nTranslate) = 0;
	// 获取指定的动作是否为位移动作
	virtual int GetActionTranslate(const char* pszName) = 0;
	// 获取在位移模式下，当前非位移根骨骼的相对位置
	virtual void GetTranslatePosition(const IVarList& args, IVarList& result)= 0;
	// 添加动作
	virtual int AddAction(const char* pszName, const char* pszFileName, 
		const char* pszActionFileName)= 0;
	// 删除动作
	virtual bool DeleteAction(const char* pszName)= 0;
	// 加载动作骨骼数据
	virtual bool LoadAction(int action_index)= 0;
	// 获得动作骨骼数据是否加载成功
	virtual bool GetActionLoaded(int action_index) = 0;

	// 融合指定动作
	virtual void BlendAction(const IVarList& args, IVarList& result) = 0;
	// 取消指定融合动作
	virtual bool UnblendAction(const char* pszName) = 0;
    // 动作是否已融合
	virtual bool IsActionBlended(const char* pszName) = 0;
	// 动作是否正在取消融合
	virtual bool IsActionUnblending(const char* pszName) = 0;
	// 获得当前在融合的动作数量
	virtual int GetBlendActionCount() = 0;
	// 获得指定索引的已融合动作名
	virtual const char* GetBlendActionName(int uBlendedAction) = 0;
	// 获取已融合动作是否是循环动作
	virtual bool GetBlendActionLoop(const char* pszName) = 0;
	// 清除所有混合动作
	virtual bool ClearBlendAction() = 0;
	
	// 取得节点的当前大小
	virtual FmVec3 GetNodeScale(const char* pNodeName) = 0;
	virtual void GetNodeScale(const IVarList& args, IVarList& result) = 0;

	// 判断节点是否存在
	virtual bool NodeIsExist(const char* pNodeName) = 0;
	// 取得节点的当前位置
	virtual bool GetNodePosition(const char* pNodeName, FmVec3& vPos) = 0;
	// 取得节点的当前方向
	virtual bool GetNodeAngle(const char* pNodeName, FmVec3& vAngle) = 0;

    // 取得节点的世界矩阵
    virtual bool GetNodeWorldMatrix(const char* pNodeName, FmMat4& mat) = 0;
    // 设置节点的世界矩阵(需要启动EnableUserBone才能有效)
    virtual  bool SetNodeWorldMatrix(const char* pNodeName, const FmMat4& mat) = 0;

	// 删除蒙皮
	virtual bool DeleteSkin(const char* pModelName) = 0;
	// 添加蒙皮
	virtual bool AddSkin(const char* pszModelName, const char* pszModelFile) = 0;
	virtual bool AddSkinEx(const char* pszModelName, const char* pszModelFile, 
		const char* pszMaterialFile) = 0;

	// 添加特效模型
	virtual bool AddEffectModel(const char* em_file, const char* append_path) = 0;

	// 连接对象到点上
	virtual void LinkToPoint(const IVarList& args, IVarList& result) = 0;
	virtual bool LinkToPoint(const char* name, const char* pointname, const PERSISTID& id, bool bEsseObj = true) = 0;
	// 从点上删除连接对象
	virtual bool UnLink(const char* name, bool bDelete) = 0;
	// 取得连接对象ID
	virtual PERSISTID GetLinkObject(const char* name) = 0;
	// 相对连接位置
	virtual bool SetLinkPosition(const char* name, float x, float y, float z) = 0;
	// 相对连接方向
	virtual bool SetLinkAngle(const char* name, float x, float y, float z) = 0;
	// 相对连接大小
	virtual bool SetLinkScale(const char* name, float x, float y, float z) = 0;
	// 保持连接对象方向是否保持绑定方向
	virtual bool SetLinkAngleKeep(const char* name, bool value) = 0;
	// 动作播放速度
	virtual bool SetActionSpeed(const char* pszName, float fSpeed) = 0;
	virtual float GetActionSpeed(const char* pszName) = 0;
	// 获得所有节点的可见对象列表
	virtual void GetVisualList(const IVarList& args, IVarList& result) = 0;
	virtual void GetLinkObjList(const IVarList& args, IVarList& result) = 0;

	// 设置已融合动作的融入时间
	virtual bool SetBlendActionEnterTime(const char* pszName, float fTime) = 0;
	// 获取已融合动作的融入时间
	virtual float GetBlendActionEnterTime(const char* pszName) = 0;
	// 设置已融合动作的融出时间
	virtual bool SetBlendActionLeaveTime(const char* pszName, float fTime) = 0;
	// 获取已融合动作的融出时间
	virtual float GetBlendActionLeaveTime(const char* pszName) = 0;
	// 设置已融合动作的播放速度
	virtual bool SetBlendActionSpeed(const char* pszName, float fSpeed) = 0;
	// 获取已融合动作的播放速度
	virtual float GetBlendActionSpeed(const char* pszName) = 0;;
	// 设置已融合动作的权值
	virtual bool SetBlendActionWeight(const char* pszName, float fWeight) = 0;
	// 获取已融合动作的播放速度
	virtual float GetBlendActionWeight(const char* pszName) = 0;;
	// 设置已融合动作是否暂停
	virtual bool SetBlendActionPause(const char* pszName, bool bPause) = 0;
	// 获取已融合动作是否暂停
	virtual bool GetBlendActionPause(const char* pszName) = 0;

	// 骨骼相对位移信息
	virtual bool SetBonePosition(const char* pszName, float x, float y, float z) = 0;
	virtual void GetBonePosition(const IVarList& args, IVarList& result) = 0;
	// 骨骼相对旋转信息
	virtual bool SetBoneAngle(const char* pszName, float x, float y, float z) = 0;
	virtual bool SetBoneAngleNoScale(const char* pszName, float x, float y, float z) = 0;
	virtual void GetBoneAngle(const IVarList& args, IVarList& result) = 0;
	// 骨骼相对缩放信息
	virtual bool SetBoneScale(const char* pszName, float x, float y, float z) = 0;
	virtual void GetBoneScale(const IVarList& args, IVarList& result) = 0;

	virtual bool SetKinetics(const char* pName, float fBaseValue, float fValue) = 0;
	virtual void GetKinetics(const IVarList& args, IVarList& result) = 0;

	// 添加子动作集
	virtual bool AddChildAction(const PERSISTID& id) = 0;
	// 添加父动作集
	virtual bool AddParentAction(const PERSISTID& id) = 0;
	// 判断子动作集是否存在
	virtual bool ChildActionExist() = 0;
	// 判断父动作集是否存在
	virtual bool ParentActionExist() = 0;

	// 设置自动朝向目标对象
	virtual void SetAimObject(PERSISTID id) = 0;
	virtual PERSISTID GetAimObject() const = 0;

	// 足部是否贴合地面
	virtual void SetFootAnchor(bool value) = 0;
	virtual bool GetFootAnchor() const = 0;

	// 地面对象
	virtual void SetFootFloorID(const PERSISTID& value) = 0;
	virtual PERSISTID GetFootFloorID() const = 0;

	// 导出骨骼结构到文本文件
	virtual bool DumpBoneNodeTree(const char* file_name) = 0;

	// 显示辅助点
	virtual void SetShowHelper(bool value) = 0;
	virtual bool GetShowHelper() const = 0;

	// 显示骨骼
	virtual void SetShowBone(bool value) = 0;
	virtual bool GetShowBone() const = 0;

    // 获取当前模型是否存在主模（裸模）
    virtual bool ExistMainModel() = 0;
    // 指定当前模型仅对主模（裸模）进行碰撞检测
    virtual void SetTraceMainModel(bool value) = 0;
    // 获取当前模型是否仅对主模（裸模）进行碰撞检测
    virtual bool GetTraceMainModel() = 0;

    // 用户在外部重设置骨骼,如开启RAGDOLL
    virtual void EnableUserBone(bool enable) = 0;
    // 重设骨骼后,需要由外部调用骨骼更新
    virtual void UserBoneUpdate() = 0;

    // 速度
    virtual void SetSpeed(float fSpeed) = 0;
    virtual float GetSpeed() = 0;

	// 角色残影效果
	virtual void SetBrokenShadow(bool value) = 0;
	virtual bool GetBrokenShadow() = 0;
	
	// 角色残影效果对象
	virtual PERSISTID GetBrokenShadowID() const = 0;

	// 节点残影效果
	virtual bool SetNodeBrokenShadow(const char* pName, bool value) = 0;
	virtual bool GetNodeBrokenShadow(const char* pName) = 0;

	// 节点残影效果对象
	virtual PERSISTID GetNodeBrokenShadowID(const char* pName) const = 0;
};

#endif // _VISUAL_I_ACTOR_H
