//--------------------------------------------------------------------
// 文件名:		core.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "core.h"
#include "log_file.h"
#include "file_sys.h"
#include "ini_file.h"
#include "int_manager.h"
#include "ent_manager.h"
#include "ent_factory.h"
#include "ent_info.h"
#include "hook_manager.h"
#include "win_msg_buf.h"
#include "memory_pool.h"
#include "memory_check.h"
#include "../public/var_table.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../public/portable.h"
#include "../public/i_file_sys.h"
#include "../public/i_int_creator.h"
#include "../public/i_ent_creator.h"

#include "fm_mem_log.h"

#define performance_time Port_GetPerformanceTime

ASSERTION_FAIL_DEFINED;

// 核心指针
ICore* g_pCore = NULL;
// 实体创建器链表
IEntCreator* g_pEntCreator = NULL;
// 功能接口创建器链表
IIntCreator* g_pIntCreator = NULL;
// 记录文件操作
bool g_bLogFileIO = false;
// 是否在系统退出过程中
bool g_bClosing = false;
// 内部使用的内存池
CMemoryPool* g_pMemoryPool = NULL;
// 内存检查
CMemoryCheck* g_pMemoryCheck = NULL;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// 验证版本号
bool core_version(int version)
{
	return (version == FM_CORE_VERSION);
}

// 创建核心
ICore* core_initialize(const CCoreConfig& config)
{ 
	// 创建核心
	if (config.m_nFlags & CORE_FLAG_USE_MEMORY_POOL)
	{
		g_pMemoryPool = NEW CMemoryPool;
	}

	if (config.m_nFlags & CORE_FLAG_MEMORY_VALIDATE)
	{
		g_pMemoryCheck = NEW CMemoryCheck;
	}

	CCore* core = NEW CCore;

	if (!core->Initialize(config))
	{
		delete core;

		if (g_pMemoryPool)
		{
			delete g_pMemoryPool;
			g_pMemoryPool = NULL;
		}

		if (g_pMemoryCheck)
		{
			delete g_pMemoryCheck;
			g_pMemoryCheck = NULL;
		}

		return NULL;
	}

	return core; 
}

// 启动核心
bool core_startup(ICore* pCore, const IVarList& args)
{ 
	CCore* core = (CCore*)pCore;

	return core->Startup(args);
}

// 关闭核心
bool core_shutdown(ICore* pCore)
{ 
	CCore* core = (CCore*)pCore;

	// 核心关闭
	core->Shutdown();
	delete core;

	if (g_pMemoryPool)
	{
		delete g_pMemoryPool;
		g_pMemoryPool = NULL;
	}

	if (g_pMemoryCheck)
	{
		delete g_pMemoryCheck;
		g_pMemoryCheck = NULL;
	}

	return true;
}

//写入一行日志
bool fm_tracelog(const char * info,LOG_TYPE log_type )
{
	return CMemLog::instrance(log_type)->Write(info);
}


//读取一行日志
char* fm_logread(LOG_TYPE log_type )
{
	return CMemLog::instrance(log_type)->Read();
}

#ifdef __cplusplus
}
#endif // __cplusplus

// 内存池内存分配器

class CMemoryPoolAlloc
{
public:
	CMemoryPoolAlloc() {}
	~CMemoryPoolAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return g_pMemoryPool->Alloc(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { g_pMemoryPool->Free(ptr, size); }
	// 交换
	void Swap(CMemoryPoolAlloc& src) {}
};

// CCore

CCore::CCore()
{
	m_bQuit = false;
	m_nSleep = 0;
	m_nFramePerSecond = 0;
	m_dFrameSeconds = 0.0;
	m_dTotalSeconds = 0.0;
	m_pTraceFunc = NULL;
	m_pTraceContext = NULL;
	m_pMainEntity = NULL;
	m_pFileSys = NULL;
	m_pGlobals = NULL;
	m_pLogFile = NEW CLogFile("trace.log");
	m_pHookManager = NEW CHookManager;
	m_pWinMsgBuf = NEW CWinMsgBuf;
	m_pIntManager = NEW CIntManager(this);
	m_pEntManager = NEW CEntManager(this);
	m_pEntFactory = NEW CEntFactory(this, m_pEntManager, m_pHookManager);

	memset(&m_CorePerformance, 0, sizeof(m_CorePerformance));

	g_pCore = this;
}

CCore::~CCore()
{
	Shutdown();

	if (m_pGlobals)
	{
		delete m_pGlobals;
	}

	delete m_pEntFactory;
	delete m_pEntManager;
	delete m_pIntManager;
	delete m_pHookManager;
	delete m_pWinMsgBuf;
	delete m_pLogFile;
	g_pCore = NULL;
}

const char* CCore::GetWorkPath()
{
	return m_strWorkPath.c_str();
}

const char* CCore::GetProgramPath()
{
	return m_strProgramPath.c_str();
}

const char* CCore::GetResourcePath()
{
	return m_strResourcePath.c_str();
}

void* CCore::MemoryCheckAllocDebug(size_t size, const char* file, int line)
{
	if (0 == size)
	{
		return (void*)"";
	}

	void* ptr;

	if (g_pMemoryPool)
	{
		ptr = g_pMemoryPool->Alloc(size);
	}
	else
	{
		ptr = malloc(size);

		if (NULL == ptr)
		{
			// 申请内存失败
			SaveLog("(CCore::MemoryCheckAllocDebug)no enough memory, size:%d", 
				size);
			return NULL;
		}
	}

	if (g_pMemoryCheck)
	{
		if (!g_pMemoryCheck->Add(ptr, size, file, line))
		{
			SaveLog("(CCore::MemoryCheckAllocDebug)memory check failed");
			abort();
		}
	}

	return ptr;
}

void* CCore::MemoryCheckAlloc(size_t size)
{
	if (0 == size)
	{
		return (void*)"";
	}

	void* ptr;

	if (g_pMemoryPool)
	{
		ptr = g_pMemoryPool->Alloc(size);
	}
	else
	{
		ptr = malloc(size);

		if (NULL == ptr)
		{
			// 申请内存失败
			SaveLog("(CCore::MemoryCheckAlloc)no enough memory, size:%d", 
				size);
			return NULL;
		}
	}

	if (g_pMemoryCheck)
	{
		if (!g_pMemoryCheck->Add(ptr, size, "", 0))
		{
			SaveLog("(CCore::MemoryCheckAlloc)memory check failed");
			abort();
		}
	}

	return ptr;
}

void CCore::MemoryCheckFree(void* ptr, size_t size)
{
	if ((NULL == ptr) || (0 == size))
	{
		return;
	}

	if (g_pMemoryCheck)
	{
		if (!g_pMemoryCheck->Remove(ptr, size))
		{
			SaveLog("(CCore::MemoryCheckFree)memory check failed");
			abort();
		}
	}

	if (g_pMemoryPool)
	{
		g_pMemoryPool->Free(ptr, size);
	}
	else
	{
		free(ptr);
	}
}

void* CCore::MemoryAlloc(size_t size)
{
	return NEW char[size];
}

void CCore::MemoryFree(void* ptr)
{
	delete[] (char*)ptr;
}

void CCore::SetSleep(int value)
{
	m_nSleep = value;
}

int CCore::GetSleep()
{
	return m_nSleep;
}

void CCore::SetQuit(bool value)
{
	m_bQuit = value;
}

bool CCore::GetQuit()
{
	return m_bQuit;
}

bool CCore::TraceLog(const char* log)
{
#ifdef FX_SYSTEM_WINDOWS
	#ifdef	_DEBUG
	    OutputDebugStringA(log);
        OutputDebugStringA("\n");  
	#endif
#endif

#ifdef FX_SYSTEM_IOS
	#ifdef	DEBUG
		printf("%s\n",log);
		return true;
    #endif
#endif

	Assert(log != NULL);
	
	if (m_pTraceFunc)
	{
		return m_pTraceFunc(m_pTraceContext, log);
	}
	
	CAutoLock autolock(m_LogLock);

	if (NULL == m_pLogFile)
	{
		return false;
	}
	
	return m_pLogFile->SaveLog(log);
}

bool CCore::TraceLogW(const wchar_t* log)
{
	char buf[256];

	ToString(log, buf, sizeof(buf));

	return TraceLog(buf);
}

bool CCore::SetTraceFunc(TRACE_FUNC func, void* context)
{
	m_pTraceFunc = func;
	m_pTraceContext = context;
	
	return true;
}

bool CCore::DumpFile(const char* type, const char* file)
{
	Assert(type != NULL);
	Assert(file != NULL);

	if (stricmp(type, "memory") == 0)
	{
		if (g_pMemoryCheck)
		{
			// 内存信息
			return g_pMemoryCheck->Dump(file);
		}
	}
	else if (stricmp(type, "entinfo") == 0)
	{
		// 实体信息
		return m_pEntManager->Dump(file);
	}
	else if (stricmp(type, "entity") == 0)
	{
		// 实体对象
		return m_pEntFactory->Dump(file);
	}
	else if (stricmp(type, "interface") == 0)
	{
		// 功能接口信息
		return m_pIntManager->Dump(file);
	}
	
	return false;
}

IFileSys* CCore::GetFileSys()
{
	return m_pFileSys;
}

IIniFile* CCore::CreateIniFile(const char* filename, int read_raw_file)
{
	return CIniFile::NewInstance(filename, read_raw_file);
}

bool CCore::FindInterface(const char* name)
{
	Assert(name != NULL);
	
	return m_pIntManager->Find(name);
}

IInterface* CCore::GetInterface(const char* name)
{
	Assert(name != NULL);

	return m_pIntManager->GetReference(name);
}

void CCore::ReleaseInterface(IInterface* pInterface)
{
	m_pIntManager->ReleaseReference(pInterface);
}

IEntInfo* CCore::GetEntInfo(const char* name)
{
	Assert(name != NULL);
	
	return m_pEntManager->GetEntInfo(name);
}

IEntity* CCore::GetMainEntity()
{
	return m_pMainEntity;
}

IEntity* CCore::GetEntity(const PERSISTID& id)
{
	return m_pEntFactory->Get(id);
}

size_t CCore::GetAllEntity(IVarList& result)
{
	return m_pEntFactory->GetAll(result);
}

IEntity* CCore::LookupEntity(const char* name)
{
	Assert(name != NULL);
	
	return m_pEntFactory->Find(name);
}

size_t CCore::LookupEntityMore(const char* name, IVarList& result)
{
	Assert(name != NULL);
	
	return m_pEntFactory->FindMore(name, result); 
}

IEntity* CCore::CreateEntity(const char* name)
{
	Assert(name != NULL);
	
	return m_pEntFactory->Create(name, CVarList());
}

IEntity* CCore::CreateEntityArgs(const char* name, const IVarList& args)
{
	Assert(name != NULL);
	
	return m_pEntFactory->Create(name, args);
}

bool CCore::DeleteEntity(const PERSISTID& id)
{
	IEntity* pEntity = m_pEntFactory->Get(id);

	if (NULL == pEntity)
	{
		return false;
	}

	m_pEntFactory->SetDeleted(pEntity);

	return true;
}

typedef bool (*GET_FUNC)(IEntity*, IVar&);

bool CCore::GetProperty(IEntity* pEntity, const char* prop, IVar& value)
{
	Assert(pEntity != NULL);
	Assert(prop != NULL);

	const IPropInfo* pPropInfo = 
		pEntity->GetEntInfo()->FindPropertyInfo(prop);
	
	if (NULL == pPropInfo)
	{
		return false;
	}
	
	GET_FUNC get_func = (GET_FUNC)pPropInfo->GetGetFunc();

	if (NULL == get_func)
	{
		return false;
	}
	
	return get_func(pEntity, value);
}

typedef bool (*SET_FUNC)(IEntity*, const IVar&);

bool CCore::SetProperty(IEntity* pEntity, const char* prop, 
	const IVar& value)
{
	Assert(pEntity != NULL);
	Assert(prop != NULL);
	
	const IPropInfo* pPropInfo = 
		pEntity->GetEntInfo()->FindPropertyInfo(prop);
	
	if (NULL == pPropInfo)
	{
		return false;
	}
	
	SET_FUNC set_func = (SET_FUNC)pPropInfo->GetSetFunc();
	
	if (NULL == set_func)
	{
		return false;
	}
	
	return set_func(pEntity, value);
}

bool CCore::AddExecute(IEntity* pEntity)
{
	return m_pEntManager->AddFrameExecute(pEntity);
}

bool CCore::RemoveExecute(IEntity* pEntity)
{
	return m_pEntManager->RemoveFrameExecute(pEntity);
}

bool CCore::AddMsgProc(IEntity* pEntity, unsigned int msg, int prior)
{
	return m_pEntFactory->AddMsgProc(pEntity, msg, prior, false);
}

bool CCore::AddMsgProcInstant(IEntity* pEntity, unsigned int msg, int prior)
{
	return m_pEntFactory->AddMsgProc(pEntity, msg, prior, true);
}

bool CCore::RemoveMsgProc(IEntity* pEntity, unsigned int msg)
{
	return m_pEntFactory->RemoveMsgProc(pEntity, msg);
}

bool CCore::FindGlobalValue(const char* name)
{
	return m_pGlobals->Exists(name);
}

bool CCore::RemoveGlobalValue(const char* name)
{
	return m_pGlobals->Remove(name);
}

bool CCore::SetGlobalValue(const char* name, const IVar& value)
{
	IVar* pVar = m_pGlobals->GetValue(name);

	if (NULL == pVar)
	{
		return m_pGlobals->Add(name, value) != NULL;
	}

	pVar->Assign(value);

	return true;
}

IVar& CCore::GetGlobalValue(const char* name)
{
	IVar* pVar = m_pGlobals->GetValue(name);

	if (NULL == pVar)
	{
		pVar = m_pGlobals->Add(name, CVar());
	}

	return *pVar;
}

size_t CCore::GetGlobalCount()
{
	return m_pGlobals->GetCount();
}

size_t CCore::GetGlobalList(IVarList& result)
{
	m_pGlobals->GetKeyList(result);

	return result.GetCount();
}

bool CCore::SaveLog(const char* log, ...)
{
	va_list	args;
	
	va_start(args, log);
	
	return SaveLogList(log, args);
}

bool CCore::SaveLogList(const char* log, va_list args)
{
	Assert(log != NULL);
	
	char buf[256];
	
	SafeSprintList(buf, sizeof(buf), log, args);
	
	return TraceLog(buf);
}

bool CCore::Initialize(const CCoreConfig& config)
{
	m_nSleep = config.m_nSleep;
	m_strWorkPath = config.m_strWorkPath.c_str();
	m_strProgramPath = config.m_strProgramPath.c_str();
	m_strResourcePath = config.m_strResourcePath.c_str();
	m_strFileSystem = config.m_strFileSystem.c_str();
	m_strFileConfig = config.m_strFileConfig.c_str();
	m_strMainEntity = config.m_strMainEntity.c_str();

	if (g_pMemoryPool)
	{
		m_pGlobals = NEW TVarTable<CMemoryPoolAlloc>;
	}
	else
	{
		m_pGlobals = NEW TVarTable<>;
	}
	
	// 清空日志
	m_pLogFile->Clear();
	
	// 获得实体创建器
	IEntCreator* pEntCreator = g_pEntCreator;

	while (pEntCreator != NULL)
	{
		if (m_pEntManager->GetEntInfo(pEntCreator->GetName()))
		{
			SaveLog("(CCore::Initialize)repeat entity name %s", 
				pEntCreator->GetName());
		}
		else
		{
			m_pEntManager->AddCreator(pEntCreator);
		}

		pEntCreator = pEntCreator->GetNext();
	}

	// 获得功能接口创建器
	IIntCreator* pIntCreator = g_pIntCreator;

	while (pIntCreator != NULL)
	{
		if (m_pIntManager->Find(pIntCreator->GetName()))
		{
			SaveLog("(CCore::Initialize)repeat interface name %s", 
				pIntCreator->GetName());
		}
		else
		{
			m_pIntManager->AddCreator(pIntCreator);
		}

		pIntCreator = pIntCreator->GetNext();
	}

	// 设置实体类之间的关联信息
	m_pEntManager->SetAllParentInfo();

	return true;
}

bool CCore::Startup(const IVarList& args)
{
	// 文件系统
	if (!m_strFileSystem.empty())
	{
		m_pFileSys = (IFileSys*)GetInterface(m_strFileSystem.c_str());

		if (NULL == m_pFileSys)
		{
			SaveLog("(CCore::Startup)file system %s not found", 
				m_strFileSystem.c_str());
			return false;
		}

		if (!m_strFileConfig.empty())
		{
			// 加载文件系统配置信息
			if (!m_pFileSys->LoadConfig(m_strFileConfig.c_str()))
			{
				SaveLog("(CCore::Startup)file system %s load config %s fail", 
					m_strFileSystem.c_str(), m_strFileConfig.c_str());
			}
		}
	}
	else
	{
		m_pFileSys = CFileSys::NewInstance();
	}

	// 主实体
	PERSISTID main_id;
	
	if (!m_strMainEntity.empty())
	{
		// 创建主实体
		m_pMainEntity = CreateEntity(m_strMainEntity.c_str());
		
		if (NULL == m_pMainEntity)
		{
			SaveLog("(CCore::Startup)create main entity failed");
			SaveLog(m_strMainEntity.c_str());
			return false;
		}
		
		main_id = m_pMainEntity->GetID();
	}

	return true;
}

bool CCore::Shutdown()
{
	if (m_pMainEntity != NULL)
	{
		// 删除主实体
		m_pEntFactory->Destroy(m_pMainEntity);
		m_pMainEntity = NULL;

#ifdef _DEBUG
		if (m_pEntFactory->GetCount() > 0)
		{
			// 导出所有未删除的实体信息
			m_pEntFactory->Dump("entity_leak.log");
		}
#endif // _DEBUG
	}

	// 先清除所有已被删除的实体
	m_pEntFactory->ClearDeleted();

	// 以下如果有实体在析构函数中删除其他实体将立即崩溃
	g_bClosing = true;

	// 删除所有实体对象
	m_pEntFactory->ReleaseAll();
	// 删除所有功能接口
	m_pIntManager->ReleaseAll();
	
	if (m_strFileSystem.empty())
	{
		// 需要删除缺省的文件系统
		SAFE_RELEASE(m_pFileSys);
	}
	else
	{
		m_pFileSys = NULL;
	}

	return true;
}

bool CCore::ProcessMessage(unsigned int msg, size_t param1, size_t param2, 
	int& result)
{
	if (m_bQuit)
	{
		return false;
	}

	double dTime = performance_time();

	// 先尝试立即处理消息
	if (m_pHookManager->ProcessMsgInstant(msg, param1, param2, result))
	{
		m_CorePerformance.dProcessMessageFuncTime+= performance_time() - dTime;
		return true;
	}

	m_pWinMsgBuf->Put(msg, param1, param2);

	m_CorePerformance.dProcessMessageFuncTime+= performance_time() - dTime;

	// 这样返回以使DefWindowProc进行缺省处理
	return false;
}

void CCore::FrameCheckMessage()
{
	if (m_pWinMsgBuf->Empty())
	{
		return;
	}

	double dTime = performance_time();

	CWinMsgBuf::msg_data_t* p = m_pWinMsgBuf->Get();

	while (p)
	{
		int result;

		m_pHookManager->ProcessMsg(p->nMsg, p->nParam1, p->nParam2, 
			result);
		p = m_pWinMsgBuf->Get();
	}

	m_CorePerformance.dFrameCheckMessageFuncTime+= performance_time() - dTime;
}

void CCore::FrameExecute(double seconds)
{
	m_dFrameSeconds = seconds;
	m_dTotalSeconds += seconds;

	double dTime = performance_time();
	// 功能接口帧起始调用
	m_pIntManager->ExecuteFrameBegin();
	// 实体执行逻辑
	m_pEntManager->ExecuteFrame((float)m_dFrameSeconds);
	// 功能接口帧结束调用
	m_pIntManager->ExecuteFrameEnd();
	// 清理已删除的实体
	m_pEntFactory->ClearDeleted();

	m_CorePerformance.dFrameExecuteFuncTime += performance_time() - dTime;
}

void CCore::FrameSleep()
{
	// 固定时间的休眠
	if (m_nSleep >= 0)
	{
		Port_Sleep(m_nSleep);
	}
}

void CCore::SetLogFileIO(bool value)
{
	g_bLogFileIO = value;
}


// 获得core性能监控
core_performance_info_t* CCore::GetPerformance()
{
	return &m_CorePerformance;
}

// 修改路径
void CCore::SetPathConfig(CCoreConfig& config)
{
	m_nSleep = config.m_nSleep;
	m_strWorkPath = config.m_strWorkPath.c_str();
	m_strProgramPath = config.m_strProgramPath.c_str();
	m_strResourcePath = config.m_strResourcePath.c_str();
	m_strFileSystem = config.m_strFileSystem.c_str();
	m_strFileConfig = config.m_strFileConfig.c_str();
	m_strMainEntity = config.m_strMainEntity.c_str();
}