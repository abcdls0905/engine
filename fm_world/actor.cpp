//--------------------------------------------------------------------
// 文件名:		actor.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年12月3日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
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
/// \brief 动画组合物体

/// entity: IActorCallee
/// \brief 定义动作事件回调实体基类
DECLARE_ENTITY(IActorCallee, 0, IEntity);

/// entity: Actor
/// \brief 动画组合物体实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Actor,0, IVisBase);

/// readonly: const char* ConfigFile
/// \brief 组合体的配置文件
DECLARE_PROPERTY_GET(const char*, Actor, ConfigFile, GetConfigFile);
/// readonly: const char* ActionFile
/// \brief 动作集文件
DECLARE_PROPERTY_GET(const char*, Actor, ActionFile, GetActionFile);
/// readonly: object BrokenShadowID
/// \brief 动态残影效果ID
DECLARE_PROPERTY_GET(PERSISTID, Actor, BrokenShadowID, GetBrokenShadowID);

/// property: bool AsyncLoad
/// \brief 是否异步加载
DECLARE_PROPERTY(bool, Actor, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool AnimationLod
/// \brief 骨骼动画是否需要LOD
DECLARE_PROPERTY(bool, Actor, AnimationLod, GetAnimationLod, SetAnimationLod);
/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, Actor, Visible, GetVisible, SetVisible);
/// property: bool TraceEanble
/// \brief 是否检测碰撞
DECLARE_PROPERTY(bool, Actor, TraceEnable, GetTraceEnable, SetTraceEnable);
/// property: bool ReturnTraceNode
/// \brief 是否直接返回碰撞到的节点对象
DECLARE_PROPERTY(bool, Actor, ReturnTraceNode, GetReturnTraceNode, 
	SetReturnTraceNode);
/// property: bool CullEanble
/// \brief 是否剪裁
DECLARE_PROPERTY(bool, Actor, CullEnable, GetCullEnable, SetCullEnable);
/// property: bool WaterReflect
/// \brief 是否需要水面反射
DECLARE_PROPERTY(bool, Actor, WaterReflect, GetWaterReflect, SetWaterReflect);
/// property: int WriteVelocity
/// \brief 写入速度纹理标志（0-不写入，1-写入，2-写入空值）
DECLARE_PROPERTY(int, Actor, WriteVelocity, GetWriteVelocity, 
	SetWriteVelocity);
/// property: float LevelOfDetail
/// \brief 细节等级
DECLARE_PROPERTY(float, Actor, LevelOfDetail, GetLevelOfDetail, 
	SetLevelOfDetail);
/// property: bool ShowBoundBox
/// \brief 是否显示包围盒
DECLARE_PROPERTY(bool, Actor, ShowBoundBox, GetShowBoundBox, SetShowBoundBox);
/// property: string Color
/// \brief 颜色
DECLARE_PROPERTY(result_string, Actor, Color, GetColorString, SetColorString);
/// property: int ExtraInfo
/// \brief 附加信息
DECLARE_PROPERTY(int, Actor, ExtraInfo, GetExtraInfo, SetExtraInfo);
/// property: float Speed
/// \brief 动作播放速度
DECLARE_PROPERTY(float, Actor, Speed, GetSpeed, SetSpeed);
/// property: string DefaultAction
/// \brief 默认动作
DECLARE_PROPERTY(const char*, Actor, DefaultAction, GetDefaultAction, 
	SetDefaultAction);
/// property: string SelectNode
/// \brief 显示选择点
DECLARE_PROPERTY(const char*, Actor, SelectNode, GetSelectNode, 
	SetSelectNode);
/// property: bool ShowBone
/// \brief 显示骨骼点
DECLARE_PROPERTY(bool, Actor, ShowBone, GetShowBone, SetShowBone);
/// property: bool ShowHelper
/// \brief 显示辅助点
DECLARE_PROPERTY(bool, Actor, ShowHelper, GetShowHelper, SetShowHelper);
/// property: bool ShowCamera
/// \brief 显示摄像机
DECLARE_PROPERTY(bool, Actor, ShowCamera, GetShowCamera, SetShowCamera);
/// property: bool BrokenShadow
/// \brief 残影效果
DECLARE_PROPERTY(bool, Actor, BrokenShadow, GetBrokenShadow, SetBrokenShadow);

/// property: bool TraceMainModel
/// \brief 当前模型是否仅对主模（裸模）进行碰撞检测
DECLARE_PROPERTY(bool, Actor, TraceMainModel, GetTraceMainModel, 
	SetTraceMainModel);

/// property: object Callee
/// \brief 动作回调对象
DECLARE_PROPERTY(PERSISTID, Actor, Callee, GetCalleeID, SetCalleeID);
/// property: object ActionControl
/// \brief 动作控制对象
DECLARE_PROPERTY(PERSISTID, Actor, ActionControl, GetActionControlID, 
	SetActionControlID);

/// property: object AimObject
/// \brief 自动朝向目标对象
DECLARE_PROPERTY(PERSISTID, Actor, AimObject, GetAimObject, SetAimObject);
/// property: bool FootAnchor
/// \brief 足部是否贴合地面
DECLARE_PROPERTY(bool, Actor, FootAnchor, GetFootAnchor, SetFootAnchor);
/// property: object FootFloor
/// \brief 脚下的地面对象
DECLARE_PROPERTY(PERSISTID, Actor, FootFloor, GetFootFloorID, SetFootFloorID);

/*
/// method: bool ExistMainModel()
/// \brief 获取当前模型是否存在主模（裸模）
DECLARE_METHOD_0(bool, Actor, ExistMainModel);

/// method: bool AfterLoaded()
/// \brief 加载完成后
DECLARE_METHOD_0(bool, Actor, AfterLoaded);

/// method: bool SetNodeVisible(const char* name, bool value)
/// \brief 设置子节点是否可见
/// \param name 节点名
/// \param value 是否可见
/// \return 设置是否成功
DECLARE_METHOD_2(bool, Actor, SetNodeVisible, const char*, bool);
/// method: bool GetNodeVisible(const char* name)
/// \brief 获取子节点是否可见
/// \param name 节点名
/// \return 是否可见
DECLARE_METHOD_1(bool, Actor, GetNodeVisible, const char*);

/// method: bool SetNodeEsseObj(const char* name, bool value)
/// \brief 设置节点是否是参与碰撞和剪裁的实体
/// \param name 节点名
/// \param value 是否是参与碰撞和剪裁的实体
/// \return 设置是否成功
DECLARE_METHOD_2(bool, Actor, SetNodeEsseObj, const char*, bool);
/// method: bool GetNodeEsseObj(const char* name)
/// \brief 设置节点是否是参与碰撞和剪裁的实体
/// \param name 节点名
/// \return 是否是参与碰撞和剪裁的实体
DECLARE_METHOD_1(bool, Actor, GetNodeEsseObj, const char*);

/// method: table SetNoBrokenShadowList()
/// \brief 设置不参与残影的节点列表
DECLARE_METHOD_T(Actor, SetNoBrokenShadowList);
/// method: table SetNoBrokenShadowList()
/// \brief 获取不参与残影的节点列表
DECLARE_METHOD_T(Actor, GetNoBrokenShadowList);

/// method: bool SetAction(string pFileName)
/// \brief 通过配置文件设置动作集合
/// \brief pFileName 动作文件
/// \return 设置成功返回true,失败返回false
DECLARE_METHOD_1(bool, Actor, SetAction, const char*);
/// method: bool SetActionEx(string pFileName, string pszDefaultAction
///		string pszAppendPath, bool bAsyncLoad)
/// \brief 通过配置文件设置动作集合
/// \param pFileName 动作文件
/// \param pszDefaultAction 默认动作名
/// \param pszAppendPath 附加路径
/// \param bAsyncLoad 是否异步加载
/// \return 设置成功返回true,失败返回false
DECLARE_METHOD_4(bool, Actor, SetActionEx, const char*, const char*, 
	const char*, bool);
/// method: table GetActionList()
/// \brief 获得动作列表
DECLARE_METHOD_T(Actor, GetActionList);
/// method: int GetActionCount()
/// \brief 动作总数
DECLARE_METHOD_0(int, Actor, GetActionCount);
/// method: int GetActionIndex(string pszName)
/// \brief 根据动作名获得动作索引
DECLARE_METHOD_1(int, Actor, GetActionIndex, const char*);
/// method: int GetActionName(int action_index)
/// \brief 根据动作索引获得动作名
DECLARE_METHOD_1(const char*, Actor, GetActionName, int);
/// method: table GetSkeletonFile(string pszName)
/// \brief 获得指定动作的资源文件
/// \param pszName 动作名
/// \return 动作资源文件名
DECLARE_METHOD_1(const char*, Actor, GetSkeletonFile, const char*);
/// method: table GetActionBlendList()
/// \brief 获得已融合动作列表
DECLARE_METHOD_T(Actor, GetActionBlendList);

/// method: bool SetActionEnterTime(string pActionName, float fTime)
/// \brief 设置动作融入时等待时间
/// \param pActionName 动作名
/// \param fTime 时间
/// \return 设置成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, SetActionEnterTime, const char*, float);
/// method: float GetActionEnterTime(string pActionName)
/// \brief 取得动作融入时等待时间
/// \param pActionName 动作名
/// \return 返回动作融入时等待时间
DECLARE_METHOD_1(float, Actor, GetActionEnterTime, const char*);
/// method: bool SetActionLeaveTime(string pActionName, float fTime)
/// \brief 设置删除动作时的等待时间
/// \param pActionName 动作名
/// \param fTime 时间
/// \return 设置成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, SetActionLeaveTime, const char*, float);
/// method: float GetActionLeaveTime(string pActionName)
/// \brief 取得删除动作时的等待时间
/// \param pActionName 动作名
/// \return 返回删除动作时的等待时间
DECLARE_METHOD_1(float, Actor, GetActionLeaveTime, const char*);
/// method: bool SetActionBaseSpeed(string pszName, float fBaseSpeed)
/// \brief 设置指定的动作的基准速度
/// \param pszName 动作名
/// \param fBaseSpeed 基准速度
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, SetActionBaseSpeed, const char*, float);
/// method: float GetActionBaseSpeed(string pszName)
/// \brief 获取指定的动作的基准速度
/// \param pszName 动作名
/// \return 返回基准速度
DECLARE_METHOD_1(float, Actor, GetActionBaseSpeed, const char*);
/// method: float GetActionScale(string pszName)
/// \brief 获取指定的动作的缩放系数
/// \param pszName 动作名
/// \return 返回缩放系数
DECLARE_METHOD_1(float, Actor, GetActionScale, const char*);
/// method: int GetActionFrame(const char* pActionName)
/// \brief 取得指定动作的桢数
/// \param pActionName 动作名
/// \return 返回该动作的总桢数
DECLARE_METHOD_1(int, Actor, GetActionFrame, const char*);
/// method: float GetActionFPS(string action_name)
/// \brief 取得指定动作的每秒帧数
/// \param action_name 动作名
DECLARE_METHOD_1(float, Actor, GetActionFPS, const char*);
/// method: float GetActionSeconds(string action_name)
/// \brief 取得指定动作的持续时间
/// \param action_name 动作名
DECLARE_METHOD_1(float, Actor, GetActionSeconds, const char*);

/// method: bool SetCurrentFrameFloat(const char* pActionName, float fFrame)
/// \brief 取得指定动作的当前帧
/// \param pActionName 动作名
/// \param fFrame 帧数
/// \return 返回是否设置成功
DECLARE_METHOD_2(bool, Actor, SetCurrentFrameFloat, const char*, float);
/// method: float GetCurrentFrameFloat(const char* pActionName)
/// \brief 取得指定动作的当前帧
/// \param pActionName 动作名
/// \return 返回该动作的当前帧
DECLARE_METHOD_1(float, Actor, GetCurrentFrameFloat, const char*);
/// method: bool SetCurrentFrame(const char* pActionName, int nFrame)
/// \brief 取得指定动作的当前帧
/// \param pActionName 动作名
/// \param nFrame 帧数
/// \return 返回是否设置成功
DECLARE_METHOD_2(bool, Actor, SetCurrentFrame, const char*, int);
/// method: int GetCurrentFrame(const char* pActionName)
/// \brief 取得指定动作的当前帧
/// \param pActionName 动作名
/// \return 返回该动作的当前帧
DECLARE_METHOD_1(int, Actor, GetCurrentFrame, const char*);

/// method: int GetBlendActionCount()
/// \brief 获得当前在融合的动作数量
DECLARE_METHOD_0(int, Actor, GetBlendActionCount);
/// method: int GetBlendActionName(int uBlendedAction)
/// \brief 获得指定索引的已融合动作名
DECLARE_METHOD_1(const char*, Actor, GetBlendActionName, int);
/// method: bool BlendAction(string pszName, bool bLoop, bool bAutoDelete, 
///		bool bCritical, float fMaxBlendWeight, string bone_mask, string bone_name)
/// \brief 融合指定动作
/// \param pszName 动作名
/// \param bLoop 重复播放标志位
/// \param bAutoDelete 自动卸载标志位
/// \param bCritical 是否不等待前一动作结束[可选参数,默认为true]
/// \param fMaxBlendWeight 融合权重[可选参数,默认为1.0F]
/// \param bone_mask 只屏蔽或只允许指定节点的骨骼动画[可选参数,"open"或"close",默认为""]
/// \param bone_name 骨骼节点名称[可选参数,默认为""]
/// \return 成功返回true,失败返回false
DECLARE_METHOD_N(Actor, BlendAction);
/// method: bool InsertAction(string pszAimName, string pszSrcName, bool bLoop, 
///		float fMaxBlendWeight, string bone_mask, string bone_name)
/// \brief 在动作名为pszAimName的动作前插入动作名为pszSrcName的动作
/// \param pszAimName 基准动作名
/// \param pszSrcName 要插入的动作名
/// \param bLoop 重复播放标志位
/// \param fMaxBlendWeight 融合权重[可选参数,默认为1.0F]
/// \param bone_mask 只屏蔽或只允许指定节点的骨骼动画[可选参数,"open"或"close",默认为""]
/// \param bone_name 骨骼节点名称[可选参数,默认为""]
/// \return 成功返回true,失败返回false
DECLARE_METHOD_N(Actor, InsertAction);
/// method: bool IsActionBlended(string pszName)
/// \brief 动作是否已经融合
/// \param pszName 动作名
/// \return 已经融合返回true,未融合返回false
DECLARE_METHOD_1(bool, Actor, IsActionBlended, const char*);
/// method: bool IsActionUnblending(string pszName)
/// \brief 动作是否正在取消融合中
/// \param pszName 动作名
/// \return 结束融合返回true,否则返回false
DECLARE_METHOD_1(bool, Actor, IsActionUnblending, const char*);
/// method: bool UnblendAction(string pszName)
/// \brief 取消指定融合动作
/// \param pszName 动作名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_1(bool, Actor, UnblendAction, const char*);
/// method: bool SetBlendActionEnterTime(string pszName, float fTime)
/// \brief 设置已融合动作融入时等待时间
/// \param pszName 动作名
/// \param fTime 时间
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, SetBlendActionEnterTime, const char*, float);
/// method: float GetBlendActionEnterTime(string pszName)
/// \brief 取得已融合动作融入时等待时间
/// \param pszName 动作名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_1(float, Actor, GetBlendActionEnterTime, const char*);
/// method: bool SetBlendActionEnterTime(string pszName, float fTime)
/// \brief 设置已融合动作融出时等待时间
/// \param pszName 动作名
/// \param fTime 时间
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, SetBlendActionLeaveTime, const char*, float);
/// method: float GetBlendActionEnterTime(string pszName)
/// \brief 取得已融合动作融出时等待时间
/// \param pszName 动作名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_1(float, Actor, GetBlendActionLeaveTime, const char*);
/// method: bool SetBlendActionSpeed(string pszName, float fSpeed)
/// \brief 设置已融合动作的播放速度
/// \param pszName 动作名
/// \param fSpeed 播放速度
DECLARE_METHOD_2(bool, Actor, SetBlendActionSpeed, const char*, float);
/// method: float GetBlendActionSpeed(string pszName)
/// \brief 取得已融合动作的播放速度
/// \param pszName 动作名
DECLARE_METHOD_1(float, Actor, GetBlendActionSpeed, const char*);
/// method: bool SetBlendActionWeight(string pszName, float fWeight)
/// \brief 设置已融合动作的混合权值
/// \param pszName 动作名
/// \param fWeight 混合权值
DECLARE_METHOD_2(bool, Actor, SetBlendActionWeight, const char*, float);
/// method: float GetBlendActionWeight(string pszName)
/// \brief 取得已融合动作的混合权值
/// \param pszName 动作名
DECLARE_METHOD_1(float, Actor, GetBlendActionWeight, const char*);
/// method: bool SetBlendActionPause(string pszName, bool bPause)
/// \brief 设置已融合动作是否暂停
/// \param pszName 动作名
/// \param bPause 是否暂停
DECLARE_METHOD_2(bool, Actor, SetBlendActionPause, const char*, bool);
/// method: bool GetBlendActionPause(string pszName)
/// \brief 取得已融合动作是否暂停
/// \param pszName 动作名
DECLARE_METHOD_1(bool, Actor, GetBlendActionPause, const char*);
/// method: bool GetBlendActionLoop(string pszName)
/// \brief 获取已融合动作是否是循环动作
/// \param pszName 动作名
DECLARE_METHOD_1(bool, Actor, GetBlendActionLoop, const char*);
/// method: bool ClearBlendAction()
/// \brief 清除所有已融合动作
DECLARE_METHOD_0(bool, Actor, ClearBlendAction);
/// method: int FindAction(string pszName)
/// \brief 查找动作是否存在
/// \param pszName 动作名
/// \return 成功返回0或1(0表示动作未加载完成,1表示动作加载完成),失败返回-1
DECLARE_METHOD_1(int, Actor, FindAction, const char*);
/// method: bool SetActionTranslate(string pszName, int nTranslate)
/// \brief 设置指定的动作是否为位移动作
/// \param pszName 动作名
/// \param nTranslate 位移动作标志
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, SetActionTranslate, const char*, int);
/// method: int GetActionTranslate(string pszName)
/// \brief 获取指定的动作是否为位移动作
/// \param pszName 动作名
/// \return 是位移动作返回1或2,否则返回0
DECLARE_METHOD_1(int, Actor, GetActionTranslate, const char*);
/// method: [x, y, z] GetTranslatePosition(string pszName, [int nFrame])
/// \brief 获取在位移模式下，当前非位移根骨骼的相对位置
/// \param pszName 动作名
/// \param nFrame 指定帧（可选参数）
DECLARE_METHOD_N(Actor, GetTranslatePosition);
/// method: bool SetActionSpeed(string pszName, float fSpeed)
/// \brief 设置指定的动作的播放速度
///		[如果动作名为空字符串时，同SetSpeed(fSpeed)]
/// \param pszName 动作名
/// \param fSpeed 动作名
/// \return 是成功返回true,否则返回false
DECLARE_METHOD_2(bool, Actor, SetActionSpeed, const char*, float);
/// method: bool GetActionSpeed(string pszName)
///		[如果动作名为空字符串时，同GetSpeed()]
/// \brief 获取指定的动作的播放速度
/// \param pszName 动作名
/// \return 指定动作的播放速度
DECLARE_METHOD_1(bool, Actor, GetActionSpeed, const char*);
/// method: int AddAction(string pszName, string pszFileName, 
///		string pszActionFileName)
/// \brief 添加动作
/// \param pszName 动作名
/// \param pszFileName 动作文件名
/// \param pszActionFileName 所在动作集名
/// \return 动作的索引值
DECLARE_METHOD_3(int, Actor, AddAction, const char*, const char*, 
	const char*);
/// method: bool DeleteAction(string pszName)
/// \brief 删除动作
/// \param pszName 动作名
/// \return 是否删除成功
DECLARE_METHOD_1(bool, Actor, DeleteAction, const char*);
/// method: bool LoadAction(int action_index)
/// \brief 加载动作骨骼数据
/// \param action_index 动作索引
/// \return 是否加载成功
DECLARE_METHOD_1(bool, Actor, LoadAction, int);
/// method: bool GetActionLoaded(int action_index)
/// \brief 获得动作骨骼数据是否加载成功完成
/// \param action_index 动作索引
/// \return 是否加载成功完成
DECLARE_METHOD_1(bool, Actor, GetActionLoaded, int);

/// method: bool CreateFromIni(string pFileName)
/// \brief 加载组合模型
/// \param pFileName 组合模型配置文件名称
/// \return 加载成功返回true,失败返回false
DECLARE_METHOD_1(bool, Actor, CreateFromIni, const char*);
/// method: bool CreateFromIniEx(string pFileName, const char* append_path)
/// \brief 加载组合模型
/// \param pFileName 组合模型配置文件名称
/// \param append_path 扩展路径名
/// \return 加载成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, CreateFromIniEx, const char*, const char*);

/// method: table GetActionNodeList()
/// \brief 获得动作相关节点列表
DECLARE_METHOD_T(Actor, GetActionNodeList);
/// method: table GetHelperNodeList()
/// \brief 获得辅助点列表
DECLARE_METHOD_T(Actor, GetHelperNodeList);
/// method: table GetBoneNodeList()
/// \brief 获得骨骼节点列表
DECLARE_METHOD_T(Actor, GetBoneNodeList);
/// method: [x, y, z] GetNodePosition(string pNodeName)
/// \brief 取得结点的世界坐标
/// \param pNodeName 结点名
DECLARE_METHOD_N(Actor, GetNodePosition);
/// method: [x, y, z] GetNodeAngle(string pNodeName)
/// \brief 取得结点在世界坐标系的方向
/// \param pNodeName 结点名
DECLARE_METHOD_N(Actor, GetNodeAngle);
/// method: [x, y, z] GetNodeScale(string pNodeName)
/// \brief 取得结点在世界坐标系的缩放比例
/// \param pNodeName 结点名
DECLARE_METHOD_N(Actor, GetNodeScale);
/// method: bool NodeIsExist(string pNodeName)
/// \brief 判断节点是否存在
/// \param pNodeName 节点名称
/// \return 节点存在返回true,反之返回false
DECLARE_METHOD_1(bool, Actor, NodeIsExist, const char*);

/// method: bool AddChildAction(PERSISTID id)
/// \brief 添加子动作集
/// \param id 节点名称
/// \return 添加成功返回true,反之返回false
DECLARE_METHOD_1(bool, Actor, AddChildAction, PERSISTID);
/// method: bool AddParentAction(PERSISTID id)
/// \brief bool 添加父动作集
/// \param id 节点名称
/// \return 添加成功返回true,反之返回false
DECLARE_METHOD_1(bool, Actor, AddParentAction, PERSISTID);
/// method: bool ChildActionExist()
/// \brief bool 是否存在子动作集
/// \return 存在返回true,反之返回false
DECLARE_METHOD_0(bool, Actor, ChildActionExist);
/// method: bool ParentActionExist()
/// \brief bool 是否存在父动作集
/// \return 添加成功返回true,反之返回false
DECLARE_METHOD_0(bool, Actor, ParentActionExist);

/// method: bool AddSkin(string pszModelName, string pszModelFile)
/// \brief 添加蒙皮
/// \param pszModelName 蒙皮名
/// \param pszModelFile 蒙皮文件名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, AddSkin, const char*, const char*);
/// method: bool AddSkinEx(string pszModelName, string pszModelFile, 
///		string pszMaterialFile)
/// \brief 添加蒙皮
/// \param pszModelName 蒙皮名
/// \param pszModelFile 蒙皮文件名
/// \param pszMaterialFile 蒙皮材质文件
/// \return 成功返回true,失败返回false
DECLARE_METHOD_3(bool, Actor, AddSkinEx, const char*, const char*, 
	const char*);
/// method: bool AddEffectModel(string em_file, string append_path)
/// \brief 添加特效模型
/// \param em_file 特效模型名
/// \param append_path 附加路径
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, AddEffectModel, const char*, const char*);
/// method: bool DeleteSkin(string pModelName)
/// \brief 删除蒙皮
/// \param pModelName 蒙皮名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_1(bool, Actor, DeleteSkin, const char*);

/// method: bool SaveActionFile(string pszFile)
/// \brief 保存动作文件
/// \param pszFile 动作文件名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_1(bool, Actor, SaveActionFile, const char*);
/// method: bool SaveAction(string pszActionName)
/// \brief 保存指定动作的信息
/// \param pszActionName 动作名
/// \return 成功返回true,失败返回false
DECLARE_METHOD_1(bool, Actor, SaveAction, const char*);
/// method: bool AddActionTrigger(string pszName, 
///		string pszTriggerName, int nFrame, string pszString)
/// \brief 添加动作回调事件
/// \param pszName 动作名
/// \param pszTriggerName 事件名
/// \param nFrame 事件回调的桢号
/// \param pszString 动作回调事件附加信息
/// \return 成功返回动作回调的索引,失败-1
DECLARE_METHOD_4(bool, Actor, AddActionTrigger, const char*, const char*, 
	int, const char*);
/// method: bool DeleteActionTrigger(string pszName, 
///		string pszTriggerName, int nFrame)
/// \brief 删除动作回调事件
/// \param pszName 动作名
/// \param pszTriggerName 事件名
/// \param nFrame 事件回调的桢号
/// \return 成功返回true,失败返回false
DECLARE_METHOD_3(bool, Actor, DeleteActionTrigger, 
	const char*, const char*, int);
/// method: table GetActionTriggerList()
/// \brief 获得指定动作的触发事件列表
DECLARE_METHOD_T(Actor, GetActionTriggerList);
/// method: int GetActionTriggerFrame(const char* actionName, 
///		const char* triggerName);
/// \brief 取得动作回调事件触发桢数
/// \param actionName 动作名
/// \param triggerName 事件名
/// \return 成功返回true,失败返回false
/// \return 返回动作回调事件触发桢数,失败时返回0
DECLARE_METHOD_2(int, Actor, GetActionTriggerFrame, const char*, const char*);
/// method: const char* GetActionTriggerString(const char* actionName, 
///		const char* triggerName);
/// \brief 取得动作回调事件附加信息
/// \param actionName 动作名
/// \param triggerName 事件名
/// \return 返回动作回调事件附加信息,失败时返回空字符串
DECLARE_METHOD_2(const char*, Actor, GetActionTriggerString, const char*, 
	const char*);
/// method: bool SetActionStartTrigger(const char* actionName, 
///		bool bStartTrigger)
/// \brief 设置指定动作开始事件触发器是否开启
/// \param actionName 动作名
/// \param bStartTrigger true表示打开触发器,false表示关闭触发器
DECLARE_METHOD_2(bool, Actor, SetActionStartTrigger, const char*, bool);
/// method: bool GetActionStartTrigger(const char* actionName)
/// \brief 获取指定动作开始事件触发器是否开启
/// \param actionName 动作名
DECLARE_METHOD_1(bool, Actor, GetActionStartTrigger, const char*);
/// method: bool SetActionEndTrigger(const char* actionName, 
///										bool bStartTrigger)
/// \brief 设置指定动作结束事件触发器是否开启
/// \param actionName 动作名
/// \param bStartTrigger true表示打开触发器,false表示关闭触发器
DECLARE_METHOD_2(bool, Actor, SetActionEndTrigger, const char*, bool);
/// method: bool GetActionEndTrigger(const char* actionName)
/// \brief 获取指定动作结束事件触发器是否开启
/// \param actionName 动作名
DECLARE_METHOD_1(bool, Actor, GetActionEndTrigger, const char*);
/// method: bool SetActionLoopTrigger(const char* actionName, 
///		bool bStartTrigger)
/// \brief 设置指定动作循环事件触发器是否开启
/// \param actionName 动作名
/// \param bStartTrigger true表示打开触发器,false表示关闭触发器
DECLARE_METHOD_2(bool, Actor, SetActionLoopTrigger, const char*, bool);
/// method: bool GetActionLoopTrigger(const char* actionName)
/// \brief 获取指定动作循环事件触发器是否开启
/// \param actionName 动作名
DECLARE_METHOD_1(bool, Actor, GetActionLoopTrigger, const char*);
/// method: bool SetActionBreakTrigger(const char* actionName, 
///		bool bStartTrigger)
/// \brief 设置指定动作被打断事件触发器是否开启
/// \param actionName 动作名
/// \param bStartTrigger true表示打开触发器,false表示关闭触发器
DECLARE_METHOD_2(bool, Actor, SetActionBreakTrigger, const char*, bool);
/// method: bool GetActionBreakTrigger(const char* actionName)
/// \brief 获取指定动作被打断事件触发器是否开启
/// \param actionName 动作名
DECLARE_METHOD_1(bool, Actor, GetActionBreakTrigger, const char*);

/// method: bool LinkToPoint(string name, string point, 
///		const PERSISTID& id, bool bEsseObj)
/// \brief 将特效模型绑定到组合模型指定点上
/// \param name 绑定关系名
/// \param point 绑定点名
/// \param id 绑定对象ID
/// \param [可选参数,默认为true]bEsseObj 是否作为有效实体，即参与碰撞和剪裁
DECLARE_METHOD_N(Actor, LinkToPoint);
/// method: bool UnLink(string name, bool bDelete)
/// \brief 取消模型绑定的关系
/// \param name 绑定关系名
/// \param bDelete 是否删除模型
/// \return 成功返回true,失败返回false
DECLARE_METHOD_2(bool, Actor, UnLink, const char*, bool);
/// method: bool SetLinkPosition(string name, float x, float y, float z)
/// \brief 设置连接位置
/// \param name 连接关系名
/// \param x X坐标
/// \param y Y坐标
/// \param z Z坐标
/// \return 成功返回true,失败返回false
DECLARE_METHOD_4(bool, Actor, SetLinkPosition, const char*, float, float, 
	float);
/// method: bool SetLinkAngle(string name, float x, float y, float z)
/// \brief 设置连接方向
/// \param name 连接关系名
/// \param x X轴方向
/// \param y Y轴方向
/// \param z Z轴方向
/// \return 成功返回true,失败返回false
DECLARE_METHOD_4(bool, Actor, SetLinkAngle, const char*, float, float, float);
/// method: bool SetLinkScale(string name, float x, float y, float z)
/// \brief 设置连接缩放比例
/// \param name 连接关系名
/// \param x X轴缩放比例
/// \param y Y轴缩放比例
/// \param z Z轴缩放比例
/// \return 成功返回true,失败返回false
DECLARE_METHOD_4(bool, Actor, SetLinkScale, const char*, float, float, float);
/// method: [x, y, z] GetLinkPosition(string pLinkName)
/// \brief 取得连接点位置变化信息
/// \param pLinkName 连接关系名
DECLARE_METHOD_N(Actor, GetLinkPosition);
/// method: [x, y, z] GetLinkAngle(string pLinkName)
/// \brief 取得连接点方向变化信息
/// \param pLinkName 连接关系名
DECLARE_METHOD_N(Actor, GetLinkAngle);
/// method: [x, y, z] GetLinkScale(string pLinkName)
/// \brief 取得连接点缩放变化信息
/// \param pLinkName 连接关系名
DECLARE_METHOD_N(Actor, GetLinkScale);
/// method:PERSISTID GetLinkObject(string name)
/// \brief 取得连接点连接对象ID
/// \param name 连接关系名
/// \return 返回模型ID
DECLARE_METHOD_1(PERSISTID, Actor, GetLinkObject, const char*);
/// method:PERSISTID SetLinkAngleKeep(string name, bool value)
/// \brief 设置是否保持连接点方向保持不变
/// \param name 连接关系名
/// \param value 连接是否保持方向不变
/// \return 返回是否设置成功
DECLARE_METHOD_2(bool, Actor, SetLinkAngleKeep, const char*, bool);
/// method:PERSISTID GetLinkAngleKeep(string name)
/// \brief 取得是否保持连接点方向保持不变
/// \param name 连接关系名
/// \return 返回是否保持连接点方向不变
DECLARE_METHOD_1(bool, Actor, GetLinkAngleKeep, const char*);
/// method: table GetLinkObjList()
/// \brief 获得链接对象绑定关系名列表
DECLARE_METHOD_T(Actor, GetLinkObjList);

/// method: table GetVisualList()
/// \brief 获得所有节点的可见对象列表
DECLARE_METHOD_T(Actor, GetVisualList);

/// method:bool SetNodeBrokenShadow(string name, bool value)
/// \brief 设置节点连接对象残影是否打开
/// \param name 连接关系名
/// \param value true表示打开残影，false表示关闭残影
/// \return 返回设置是否成功
DECLARE_METHOD_2(bool, Actor, SetNodeBrokenShadow, const char*, bool);
/// \brief 取得节点连接对象残影是否打开
/// method:bool GetNodeBrokenShadow(string name)
/// \param name 连接关系名
/// \return 残影是否打开
DECLARE_METHOD_1(bool, Actor, GetNodeBrokenShadow, const char*);
/// \brief 取得节点连接对象残影的ID
/// method:PERSISTID GetNodeBrokenShadowID(string name)
/// \param name 连接关系名
/// \return 返回残影ID
DECLARE_METHOD_1(PERSISTID, Actor, GetNodeBrokenShadowID, const char*);

/// \brief 设置骨骼位移信息
/// method:bool SetBonePosition(string name, float x, float y, float z)
/// \param name 骨骼名
/// \param x, y, z 位置信息
/// \return 返回是否设置成功
DECLARE_METHOD_4(bool, Actor, SetBonePosition, const char*, float, float, 
	float);
/// method: [x, y, z] GetBonePosition(string name)
/// \brief 取得骨骼位移信息
/// \param name 骨骼名
/// \param x, y, z骨骼位移信息
DECLARE_METHOD_N(Actor, GetBonePosition);
/// \brief 设置骨骼旋转信息
/// method:bool SetBoneAngle(string name, float x, float y, float z)
/// \param name 骨骼名
/// \param x, y, z 旋转信息
/// \return 返回是否设置成功
DECLARE_METHOD_4(bool, Actor, SetBoneAngle, const char*, float, float, float);
/// \brief 设置骨骼旋转信息（无缩放）
/// method:bool SetBoneAngleNoScale(string name, float x, float y, float z)
/// \param name 骨骼名
/// \param x, y, z 旋转信息
/// \return 返回是否设置成功
DECLARE_METHOD_4(bool, Actor, SetBoneAngleNoScale, const char*, float, float, 
	float);
/// method: [x, y, z] GetBoneAngle(string name)
/// \brief 取得骨骼旋转信息
/// \param name 骨骼名
/// \param x, y, z骨骼旋转信息
DECLARE_METHOD_N(Actor, GetBoneAngle);
/// \brief 设置骨骼缩放信息
/// method:bool SetBoneScale(string name, float x, float y, float z)
/// \param name 骨骼名
/// \param x, y, z 缩放信息
/// \return 返回是否设置成功
DECLARE_METHOD_4(bool, Actor, SetBoneScale, const char*, float, float, float);
/// method: [x, y, z] GetBoneScale(string name)
/// \brief 取得骨骼缩放信息
/// \param name 骨骼名
/// \param x, y, z骨骼缩放信息
DECLARE_METHOD_N(Actor, GetBoneScale);

/// method:bool SetKinetics(string name, float fBaseValue, float fValue)
/// \brief 设置飘带系数
/// \param name 系数名
/// \param fBaseValue 初始系数
/// \param fValue 系数
/// \return 返回是否设置成功
DECLARE_METHOD_3(bool, Actor, SetKinetics, const char*, float, float);
/// method: [res, fBaseValue, fValue] GetKinetics(string name)
/// \brief 取得飘带系数
/// \param name 系数名
/// \param res 是否取得成功
/// \param fBaseValue 初始系数
/// \param fValue 系数
DECLARE_METHOD_N(Actor, GetKinetics);

/// method: bool DumpBoneNodeTree(string file_name)
/// \brief 导出骨骼结构到文本文件
/// \param file_name 文本文件名
DECLARE_METHOD_1(bool, Actor, DumpBoneNodeTree, const char*);*/

// 随机播放动作
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

	// 自动看着目标
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
			// 降低骨骼动画播放的频率
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
			// 更新残影
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
			// 描画残影
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

	// 显示骨骼和显示辅助点
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

	// 显示选择点
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

// 返回0到指定值之间的随机数
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
	// 修正角色模型为加载完成就updateboundingbox，导致出错的问题
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
		// 数值没变化
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
		// 数值没明显变化的时候优化性能
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
		// 没变化
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

	//取得绑定模型上的辅助点
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

// 分解节点名和点名
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
		// 解析点名和父节点名
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

	// 取得绑定模型上的辅助点
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

// 设置节点的当前的相对矩阵
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
        // 解析点名和父节点名
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

    // 取得绑定模型上的辅助点
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

	// 解析点名和父节点名
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

	// 取得绑定模型上的辅助点
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

// 通过世界矩阵设置节点位置
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

// 取得节点的世界矩阵
bool Actor::GetNodeWorldMatrix(const char* pNodeName, FmMat4& mat)
{
     return GetNodeWorldMatrix(NULL, pNodeName, &mat);
}

// 设置节点的世界矩阵
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
		// 没有动作文件的，作为特效模型创建
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
				// 添加子动作集
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

		// 连接到结点
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
		// 名字不能为空
		result.AddBool(false);
		return;
	}

	PERSISTID id = args.ObjectVal(2);

	if (id.IsNull())
	{
		result.AddBool(false);
		return;
	}

	// 取得要添加节点的父节点
	CActorNode* pPointNode = m_pRootNode;
	// 解析点名和父节点名
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

	// 添加节点
	CActorNode* pNode = m_pRootNode->GetNode(name);

	if (pNode)
	{
		CActorNode* pOldParentNode = pNode->GetParentNode();

		if (pOldParentNode != NULL)
		{
			pOldParentNode->DeleteChild(name);
		}
	}

	// 父节点已经确定，此处设置的点名已经去除前缀的节点名
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
		// 对象绑定前的附加色
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

/// \param name 绑定关系名
/// \param point 绑定点名
/// \param id 绑定对象ID
/// \param [可选参数,默认为true]bEsseObj 是否作为有效实体，即参与碰撞和剪裁
bool Actor::LinkToPoint(const char* name, const char* pointname, const PERSISTID& id, bool bEsseObj)
{
	if (0 == name[0])
	{
		// 名字不能为空
		return false;
	}

	if (id.IsNull())
	{
		return false;
	}

	// 取得要添加节点的父节点
	CActorNode* pPointNode = m_pRootNode;
	// 解析点名和父节点名
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

	// 添加节点
	CActorNode* pNode = m_pRootNode->GetNode(name);

	if (pNode)
	{
		CActorNode* pOldParentNode = pNode->GetParentNode();

		if (pOldParentNode != NULL)
		{
			pOldParentNode->DeleteChild(name);
		}
	}

	// 父节点已经确定，此处设置的点名已经去除前缀的节点名
	pNode = pPointNode->AddChild(name, pointName.c_str(), id);

	if (NULL == pNode)
	{
		CORE_TRACE("(Actor::LinkToPoint)add child failed");
		return false;
	}
	
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (pVisBase)
	{
		// 对象绑定前的附加色
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

