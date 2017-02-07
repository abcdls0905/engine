//--------------------------------------------------------------------
// 文件名:		core.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CORE_H
#define _CORE_H

#include "../public/macros.h"
#include "../public/i_core.h"
#include "../public/i_entity.h"
#include "../public/i_interface.h"
#include "../public/i_var_table.h"
#include "../public/core_config.h"
#include "../public/fast_str.h"
#include "../public/var.h"
#include "../utils/array_pod.h"
#include "../utils/lock_util.h"

// CCore

class CIntManager;
class CEntManager;
class CEntFactory;
class CHookManager;
class CWinMsgBuf;
class CLogFile;




class CCore: public ICore
{
public:

public:
	CCore();
	virtual ~CCore();

	CEntManager* GetEntManager() const { return m_pEntManager; }
	CIntManager* GetIntManager() const { return m_pIntManager; }

	// 保存日志
	bool SaveLog(const char* log, ...);
	bool SaveLogList(const char* log, va_list args);

	// 初始化
	bool Initialize(const CCoreConfig& config);
	// 启动
	bool Startup(const IVarList& args);
	// 关闭
	bool Shutdown();
	
	// 跨模块内存管理
	virtual void* MemoryCheckAllocDebug(size_t size, const char* file, 
		int line);
	virtual void* MemoryCheckAlloc(size_t size);
	virtual void MemoryCheckFree(void* ptr, size_t size);
	virtual void* MemoryAlloc(size_t size);
	virtual void MemoryFree(void* ptr);

	// 获得工作路径
	virtual const char* GetWorkPath();
	// 获得程序资源路径
	virtual const char* GetProgramPath();
	// 获得资源路径
	virtual const char* GetResourcePath();

	// 主循环休眠时间
	virtual void SetSleep(int value);
	virtual int GetSleep();

	// 控制程序退出
	virtual void SetQuit(bool value);
	virtual bool GetQuit();

	// 处理窗口消息
	virtual bool ProcessMessage(unsigned int msg, size_t param1, 
		size_t param2, int& result);

	// 每帧运行
	virtual void FrameCheckMessage();
	virtual void FrameExecute(double seconds);
	virtual void FrameSleep();

	// 保存日志到文件
	virtual bool TraceLog(const char* log);
	virtual bool TraceLogW(const wchar_t* log);
	// 重定向日志到指定函数
	virtual bool SetTraceFunc(TRACE_FUNC func, void* context);
	// 导出程序信息到文件
	virtual bool DumpFile(const char* type, const char* file);
	
	// 获得文件系统
	virtual IFileSys* GetFileSys();
	
	// 创建配置文件读写接口
	virtual IIniFile* CreateIniFile(const char* filename, 
		int read_raw_file = 0);

	// 查找功能接口
	virtual bool FindInterface(const char* name);
	// 获得功能接口
	virtual IInterface* GetInterface(const char* name);
	// 释放功能接口
	virtual void ReleaseInterface(IInterface* pInterface);
	
	// 查找实体类信息
	virtual IEntInfo* GetEntInfo(const char* name);
	// 获得主实体
	virtual IEntity* GetMainEntity();
	// 获得实体
	virtual IEntity* GetEntity(const PERSISTID& id);
	// 获得所有的实体对象ID
	virtual size_t GetAllEntity(IVarList& result);
	// 查找名字符合的第一个实体
	virtual IEntity* LookupEntity(const char* name);
	// 查找名字符合的所有实体
	virtual size_t LookupEntityMore(const char* name, IVarList& result);
	// 创建实体
	virtual IEntity* CreateEntity(const char* name);
	// 带参数创建实体
	virtual IEntity* CreateEntityArgs(const char* name, 
		const IVarList& args);
	// 删除实体
	virtual bool DeleteEntity(const PERSISTID& id);
	
	// 获得实体属性
	virtual bool GetProperty(IEntity* pEntity, const char* prop, 
		IVar& value);
	// 设置实体属性
	virtual bool SetProperty(IEntity* pEntity, const char* prop, 
		const IVar& value);

	// 添加到运行队列
	virtual bool AddExecute(IEntity* pEntity);
	// 从运行队列移除
	virtual bool RemoveExecute(IEntity* pEntity);
	// 添加窗口消息处理（msg必须小于0x1000，prior越大级别越高，先加入优先）
	virtual bool AddMsgProc(IEntity* pEntity, unsigned int msg, 
		int prior = 0);
	// 添加立即处理窗口消息（msg必须小于0x1000，prior越大级别越高，先加入优先）
	virtual bool AddMsgProcInstant(IEntity* pEntity, unsigned int msg, 
		int prior = 0);
	// 移除窗口消息处理
	virtual bool RemoveMsgProc(IEntity* pEntity, unsigned int msg);
	
	// 全局变量
	virtual bool FindGlobalValue(const char* name);
	virtual bool RemoveGlobalValue(const char* name);
	virtual bool SetGlobalValue(const char* name, const IVar& value);
	virtual IVar& GetGlobalValue(const char* name);
	virtual size_t GetGlobalCount();
	virtual size_t GetGlobalList(IVarList& result);

	// 每秒帧数
	virtual void SetFramePerSecond(int value) { m_nFramePerSecond = value; }
	virtual int GetFramePerSecond() { return m_nFramePerSecond; }

	// 获得当前帧时间
	virtual double GetFrameSeconds() { return m_dFrameSeconds; }
	// 获得当前内部计时总时间
	virtual double GetTotalSeconds() { return m_dTotalSeconds; }
	
	// 设置是否记录文件访问日志
	virtual void SetLogFileIO(bool value);

	// 获得core性能监控
	virtual core_performance_info_t* GetPerformance();

	// 修改路径
	virtual void SetPathConfig(CCoreConfig& config);
private:
	CCore(const CCore&);
	CCore& operator=(const CCore&);

private:
	bool m_bQuit;
	int m_nSleep;
	int m_nFramePerSecond;
	double m_dFrameSeconds;
	double m_dTotalSeconds;
	TFastStr<char, 128> m_strWorkPath;
	TFastStr<char, 128> m_strProgramPath;
	TFastStr<char, 128> m_strResourcePath;
	TFastStr<char, 16> m_strFileSystem;
	TFastStr<char, 16> m_strFileConfig;
	TFastStr<char, 16> m_strMainEntity;
	TRACE_FUNC m_pTraceFunc;
	void* m_pTraceContext;
	IVarTable* m_pGlobals;
	IEntity* m_pMainEntity;
	IFileSys* m_pFileSys;
	CLogFile* m_pLogFile;
	CLockUtil m_LogLock;
	CIntManager* m_pIntManager;
	CEntManager* m_pEntManager;
	CEntFactory* m_pEntFactory;
	CHookManager* m_pHookManager;
	CWinMsgBuf* m_pWinMsgBuf;
public:
	core_performance_info_t m_CorePerformance;
};

#endif // _CORE_H
