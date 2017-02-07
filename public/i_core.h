//--------------------------------------------------------------------
// 文件名:		i_core.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_CORE_H
#define _PUBLIC_I_CORE_H

#include "macros.h"
#include "persistid.h"
#include "i_var.h"
#include "i_var_list.h"
#include <stdarg.h>

// 核心接口版本号
#define FM_CORE_VERSION 0x2000

class ICore;
class CCoreConfig;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// 验证版本号
extern bool core_version(int version);
// 创建核心
extern ICore* core_initialize(const CCoreConfig& config);
// 启动核心
extern bool core_startup(ICore* pCore, const IVarList& args);
// 关闭核心
extern bool core_shutdown(ICore* pCore);

//日志类型
enum LOG_TYPE
{
	LOG_RENDER,//渲染相关
	LOG_GUI,//GUi相关监控
	LOG_NET,//网络相关监控
	LOG_CORE,//Core相关监控
	LOG_PHYSICS,//物理监控
	LOG_LOGIC,//逻辑监控
	LOG_FILEIO,//文件IO监控
	LOG_FRAME,//渲染帧数据
	LOG_PERF,//性能超时相关监控
	LOG_TYPE_MAX,//最大监控类型
};

//写入一行日志 (支持1024个字节以内有效）
extern bool fm_tracelog(const char * info,LOG_TYPE log_type = LOG_LOGIC );

//读取一行日志
extern char* fm_logread(LOG_TYPE log_type = LOG_LOGIC );


#ifdef __cplusplus
}
#endif // __cplusplus

// 核心接口

class IEntity;
class IInterface;
class IEntInfo;
class IFileSys;
class IIniFile;

struct core_performance_common_info_t
{
	double dInitFuncTime;
	double dShutFuncTime;
};

struct core_performance_entity_info_t
{
	double dExecuteFuncTime;
	double dMsgProcFuncTime;
};

struct core_performance_interface_info_t
{
	double dNeedExecPerFrameFuncTime;//保留,目前底层不监控
	double dExecFrameBeginFuncTime;
	double dExecFrameEndFuncTime;
};

struct core_performance_info_t:public core_performance_entity_info_t,public core_performance_interface_info_t,public core_performance_common_info_t
{
	double dProcessMessageFuncTime;
	double dFrameCheckMessageFuncTime;
    double dFrameExecuteFuncTime;
};

class ICore
{
public:
	typedef bool (*TRACE_FUNC)(void* context, const char* info);

public:
	virtual ~ICore() = 0;

	// 跨模块内存管理
	virtual void* MemoryCheckAllocDebug(size_t size, const char* file, 
		int line) = 0;
	virtual void* MemoryCheckAlloc(size_t size) = 0;
	virtual void MemoryCheckFree(void* ptr, size_t size) = 0;
	virtual void* MemoryAlloc(size_t size) = 0;
	virtual void MemoryFree(void* ptr) = 0;

	// 获得工作路径
	virtual const char* GetWorkPath() = 0;
	// 获得程序资源路径
	virtual const char* GetProgramPath() = 0;
	// 获得资源路径
	virtual const char* GetResourcePath() = 0;

	// 主循环休眠时间
	virtual void SetSleep(int value) = 0;
	virtual int GetSleep() = 0;

	// 控制程序退出
	virtual void SetQuit(bool value) = 0;
	virtual bool GetQuit() = 0;

	// 处理窗口消息
	virtual bool ProcessMessage(unsigned int msg, size_t param1, size_t param2, 
		int& result) = 0;

	// 每帧运行
	virtual void FrameCheckMessage() = 0;
	virtual void FrameExecute(double seconds) = 0;
	virtual void FrameSleep() = 0;

	// 保存日志到文件
	virtual bool TraceLog(const char* log) = 0;
	virtual bool TraceLogW(const wchar_t* log) = 0;
	// 重定向日志到指定函数
	virtual bool SetTraceFunc(TRACE_FUNC func, void* context) = 0;
	// 导出程序信息到文件
	virtual bool DumpFile(const char* type, const char* file) = 0;

	// 获得文件系统
	virtual IFileSys* GetFileSys() = 0;

	// 创建配置文件读写接口
	virtual IIniFile* CreateIniFile(const char* filename, 
		int read_raw_file = 0) = 0;

	// 查找功能接口
	virtual bool FindInterface(const char* name) = 0;
	// 获得功能接口
	virtual IInterface* GetInterface(const char* name) = 0;
	// 释放功能接口
	virtual void ReleaseInterface(IInterface* pInterface) = 0;

	// 查找实体类信息
	virtual IEntInfo* GetEntInfo(const char* name) = 0;
	// 获得主实体
	virtual IEntity* GetMainEntity() = 0;
	// 获得实体
	virtual IEntity* GetEntity(const PERSISTID& id) = 0;
	// 获得所有的实体对象ID
	virtual size_t GetAllEntity(IVarList& result) = 0;
	// 查找名字符合的第一个实体
	virtual IEntity* LookupEntity(const char* name) = 0;
	// 查找名字符合的所有实体
	virtual size_t LookupEntityMore(const char* name, IVarList& result) = 0;
	// 创建实体
	virtual IEntity* CreateEntity(const char* name) = 0;
	// 带参数创建实体
	virtual IEntity* CreateEntityArgs(const char* name, 
		const IVarList& args) = 0;
	// 删除实体
	virtual bool DeleteEntity(const PERSISTID& id) = 0;

	// 获得实体属性
	virtual bool GetProperty(IEntity* pEntity, const char* prop, 
		IVar& value) = 0;
	// 设置实体属性
	virtual bool SetProperty(IEntity* pEntity, const char* prop, 
		const IVar& value) = 0;

	// 添加到运行队列
	virtual bool AddExecute(IEntity* pEntity) = 0;
	// 从运行队列移除
	virtual bool RemoveExecute(IEntity* pEntity) = 0;
	// 添加窗口消息处理（msg必须小于0x1000，prior越大级别越高，先加入优先）
	virtual bool AddMsgProc(IEntity* pEntity, unsigned int msg, 
		int prior = 0) = 0;
	// 添加立即处理窗口消息（msg必须小于0x1000，prior越大级别越高，先加入优先）
	virtual bool AddMsgProcInstant(IEntity* pEntity, unsigned int msg, 
		int prior = 0) = 0;
	// 移除窗口消息处理
	virtual bool RemoveMsgProc(IEntity* pEntity, unsigned int msg) = 0;

	// 全局变量
	virtual bool FindGlobalValue(const char* name) = 0;
	virtual bool RemoveGlobalValue(const char* name) = 0;
	virtual bool SetGlobalValue(const char* name, const IVar& value) = 0;
	virtual IVar& GetGlobalValue(const char* name) = 0;
	virtual size_t GetGlobalCount() = 0;
	virtual size_t GetGlobalList(IVarList& result) = 0;

	// 每秒帧数
	virtual void SetFramePerSecond(int value) = 0;
	virtual int GetFramePerSecond() = 0;

	// 获得当前帧时间
	virtual double GetFrameSeconds() = 0;
	// 获得当前内部计时总时间
	virtual double GetTotalSeconds() = 0;

	// 设置是否记录文件访问日志
	virtual void SetLogFileIO(bool value) = 0;

	// 获得core性能监控
	virtual core_performance_info_t* GetPerformance() = 0;

	// 修改路径
	virtual void SetPathConfig(CCoreConfig& config) = 0;
};

inline ICore::~ICore() {}

#endif // _PUBLIC_I_CORE_H
