//--------------------------------------------------------------------
// 文件名:		i_solo_stub.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年12月2日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------


#ifndef _VISUAL_I_SOLO_CLIENT_H
#define _VISUAL_I_SOLO_CLIENT_H

#include "../public/module.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"

enum MsgSort
{
	CLIENT_MSG,	//客户端消息
	REMOTE_MSG,	//远程服消息
	INNER_MSG,	//本地内部消息
};

class ICollideManager;
class IKernel;
class IGameClient;
class IGameObject;
class ILogicModule;

class ISoloStub: public IEntity
{

public:

	virtual bool Init(const IVarList& args) = 0;
	virtual bool Shut() = 0;

	// 主循环
	virtual void Execute(float seconds) = 0;

	//virtual ICollideManager* GetCollideManager() const  = 0;

	//virtual IKernel* GetKernel() const  = 0;

	// 当前正在执行初始化的逻辑模块名称
	virtual const char* GetInitModuleName() const  = 0;

	// 是否使用线程
	virtual void SetUseThread(bool value)  = 0;
	virtual bool GetUseThread() const  = 0;

	// 是否使用快速日志系统
	virtual void SetUseQuickLog(bool value)  = 0;
	virtual bool GetUseQuickLog() const  = 0;

	// 是否使用应用进程记录
	virtual void SetUseAppRecord(bool value)  = 0;
	virtual bool GetUseAppRecord() const  = 0;

	// 进程绑定CPU核心
	virtual void SetBindCore(int value) = 0;
	virtual int GetBindCore() const  = 0;

	// 进程名
	virtual void SetAppName(const char* value)  = 0;
	virtual const char* GetAppName() const  = 0;

	// 启动日志服务的命令行
	virtual void SetLogLauncher(const char* value)  = 0;
	virtual const char* GetLogLauncher() const  = 0;

	// 分区编号
	virtual void SetDistrictId(int value)  = 0;
	virtual int GetDistrictId() const  = 0;

	// 游戏服务器编号
	virtual void SetServerId(int value)  = 0;
	virtual int GetServerId() const  = 0;

	// 逻辑函数的最大运行时间毫秒数
	virtual void SetMaxRunningTime(int value)  = 0;
	virtual int GetMaxRunningTime() const  = 0;

	// 心跳定时器时长
	virtual void SetBeatTime(int value) = 0;
	virtual int GetBeatTime() const  = 0;

	// 扫描定时器时长
	virtual void SetScanTime(int value) = 0;
	virtual int GetScanTime() const  = 0;

	// 逻辑对象的快扫描定时器时长
	virtual void SetFastScanTime(int value) = 0;
	virtual int GetFastScanTime() const  = 0;

	// 逻辑对象的慢扫描定时器时长
	virtual void SetSlowScanTime(int value) = 0;
	virtual int GetSlowScanTime() const  = 0;

	// 场景分格的大小
	virtual void SetSceneGridSize(int value) = 0;
	virtual int GetSceneGridSize() const  = 0;

	// 出现错误是否提示
	virtual void SetDebugHalt(bool value)  = 0;
	virtual bool GetDebugHalt() const  = 0;

	// 物体加入场景后是否立即显示
	virtual void SetAppearAtOnce(bool value)  = 0;
	virtual bool GetAppearAtOnce() const  = 0;

	// 是否不发送为缺省空值的对象属性
	virtual void SetHideEmptyProperty(bool value)  = 0;
	virtual bool GetHideEmptyProperty() const  = 0;

	// 是否使用内部移动扫描
	virtual void SetIsUseInnerMoveableScan(bool value)  = 0;
	virtual bool GetIsUseInnerMoveableScan() const  = 0;

	// 碰撞数据文件的根路径
	virtual void SetCollideRootPath(const char* value)  = 0;
	virtual const char* GetCollideRootPath() const  = 0;

	// 是否要合并对象移动消息 
	virtual void SetMergeMoving(bool value)  = 0;
	virtual bool GetMergeMoving() const  = 0;

	// 是否要合并对象属性改变消息 
	virtual void SetMergeProperty(bool value)  = 0;
	virtual bool GetMergeProperty() const  = 0;

	// 是否要合并对象添加移除消息 
	virtual void SetMergeObjects(bool value) = 0;
	virtual bool GetMergeObjects() const  = 0;

	// 是否记录每帧的运行时间
	virtual void SetLogFrameTime(bool value)  = 0;
	virtual bool GetLogFrameTime() const  = 0;

	// 是否可使用脚本
	virtual void SetUseLuaScript(bool value)  = 0;
	virtual bool GetUseLuaScript() const  = 0;

	// 是否使用内存池
	virtual void SetEnableMemPool(bool value) = 0;
	virtual bool GetEnableMemPool() const = 0;

	// 是否使用对象池
	virtual void SetEnableObjectPool(bool value) = 0;
	virtual bool GetEnableObjectPool() const = 0;

	// 脚本系统是否使用内存池
	virtual void SetScriptUseMemPool(bool value) = 0;
	virtual bool GetScriptUseMemPool() const = 0;

	// 场景碰撞信息管理器
	virtual void SetCollideManagerID(const PERSISTID& value) = 0;
	virtual PERSISTID GetCollideManagerID() const = 0;

	// 启动
	virtual bool Start() = 0;
	//是否运行
	virtual bool IsRunning() = 0;
	// 停止
	virtual bool Stop() = 0;

	// 设置控制台字符颜色
	virtual bool SetConsoleColor(const char* color) = 0;
	// 控制台输出信息
	virtual bool WriteLine(const char* info) = 0;

	// 定时检查
	void TimerBeat(float seconds);
	void TimerScan(float seconds);

	// 加载全局点信息
	virtual bool LoadLocations(const char* file_name) = 0;

	// 导出玩家数据到文件
	//bool DumpPlayers(const char* file_name);
	// 导出场景信息到文件
	virtual bool DumpScene(int scene_id, const char* info_type) = 0;
	// 导出逻辑模块性能统计数据
	virtual bool DumpPerformance(const char* file_name) = 0;
	// 清空逻辑模块性能统计数据
	virtual bool ClearPerformance() = 0;

	// 获得内存池中内存总数
	virtual double GetMemPoolSize() const = 0;
	// 获得内存池中可用内存数
	virtual double GetMemFreeSize() const = 0;

	/// \brief 获得指定名字的逻辑模块
	/// \param name 逻辑模块名
	virtual ILogicModule* GetLogicModule(const char* name) = 0;

	/// \brief 添加场景信息(scenes.ini)
	/// \param scene_id 场景id
	/// \param member 场景服务器编号
	/// \param type 场景类型
	/// \param prototype 原型场景
	/// \param script 脚本
	/// \param config 配置文件
	/// \param group 分组号
	virtual bool AddScene(int scene_id, int member, int type, 
		int prototype, const char* script, const char* config,int group) = 0;
	/// \brief 获得场景逻辑脚本名
	/// \param scene_id 场景id
	virtual const char* GetSceneScript(int scene_id) = 0;
	/// \brief 获得场景配置名称
	/// \param scene_id 场景id
	virtual const char* GetSceneConfig(int scene_id) = 0;
	/// \brief 查找场景ID
	/// \param config 配置名称
	virtual int FindSceneId(const char* config) = 0;	
	/// \brief 创建本地副本
	/// \param nProtoSceneId 原型场景
	virtual bool CreateStubCloneScene(int nProtoSceneId) = 0;
	/// \brief 添加主角
	/// \param pClient 本地客户端对象
	/// \param pMainPlayer 本地主角对象
	virtual bool AddMainPlayer(IGameClient* pClient,IGameObject* pMainPlayer) = 0;
	/// \brief 退出副本场景
	/// \param pClient 本地客户端对象
	/// \param pMainPlayer 本地主角对象
	virtual bool PlayerExitStubCloneScene(IGameClient* pClient,IGameObject* pMainPlayer) = 0;
	// 删除副本场景
	virtual bool DeleteStubCloneScene() = 0;
	/// \brief 发送消息到远程服务器
	/// \param pdata 发送数据
	/// \param len 数据长度
	virtual bool SendToServer(const void* pdata, size_t len) = 0;
	/// \brief 发送消息到本地服务器
	/// \param type 消息类型
	/// \param pdata 发送数据
	/// \param len 数据长度
	/// \param src 0 为客户端，1为远程服
	virtual bool PutMessage(int type, const void* pdata, size_t len,int src = 0) = 0;
	/// \brief 发送消息到客户端模块
	/// \param pdata 发送数据
	/// \param len 数据长度
	virtual bool SendToClient(const void* pdata, size_t len) = 0;
	virtual bool SendToClient2(const void* pdata1, size_t len1, const void* pdata2, size_t len2) = 0;
	/// \brief 网络对象
	/// \param id 网络对象id
	/// \param len 数据长度
	virtual void SetGameSockID(const PERSISTID& id) = 0;
	virtual PERSISTID GetGameSockID() const = 0;
	/// \brief 网络接受对象
	/// \param id 网络对象id
	/// \param len 数据长度
	virtual void SetGameReceiverID(const PERSISTID& id) = 0;
	virtual PERSISTID GetGameReceiverID() const = 0;
};

#endif // _SoloWar_H
