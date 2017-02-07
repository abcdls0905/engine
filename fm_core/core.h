//--------------------------------------------------------------------
// �ļ���:		core.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// ������־
	bool SaveLog(const char* log, ...);
	bool SaveLogList(const char* log, va_list args);

	// ��ʼ��
	bool Initialize(const CCoreConfig& config);
	// ����
	bool Startup(const IVarList& args);
	// �ر�
	bool Shutdown();
	
	// ��ģ���ڴ����
	virtual void* MemoryCheckAllocDebug(size_t size, const char* file, 
		int line);
	virtual void* MemoryCheckAlloc(size_t size);
	virtual void MemoryCheckFree(void* ptr, size_t size);
	virtual void* MemoryAlloc(size_t size);
	virtual void MemoryFree(void* ptr);

	// ��ù���·��
	virtual const char* GetWorkPath();
	// ��ó�����Դ·��
	virtual const char* GetProgramPath();
	// �����Դ·��
	virtual const char* GetResourcePath();

	// ��ѭ������ʱ��
	virtual void SetSleep(int value);
	virtual int GetSleep();

	// ���Ƴ����˳�
	virtual void SetQuit(bool value);
	virtual bool GetQuit();

	// ��������Ϣ
	virtual bool ProcessMessage(unsigned int msg, size_t param1, 
		size_t param2, int& result);

	// ÿ֡����
	virtual void FrameCheckMessage();
	virtual void FrameExecute(double seconds);
	virtual void FrameSleep();

	// ������־���ļ�
	virtual bool TraceLog(const char* log);
	virtual bool TraceLogW(const wchar_t* log);
	// �ض�����־��ָ������
	virtual bool SetTraceFunc(TRACE_FUNC func, void* context);
	// ����������Ϣ���ļ�
	virtual bool DumpFile(const char* type, const char* file);
	
	// ����ļ�ϵͳ
	virtual IFileSys* GetFileSys();
	
	// ���������ļ���д�ӿ�
	virtual IIniFile* CreateIniFile(const char* filename, 
		int read_raw_file = 0);

	// ���ҹ��ܽӿ�
	virtual bool FindInterface(const char* name);
	// ��ù��ܽӿ�
	virtual IInterface* GetInterface(const char* name);
	// �ͷŹ��ܽӿ�
	virtual void ReleaseInterface(IInterface* pInterface);
	
	// ����ʵ������Ϣ
	virtual IEntInfo* GetEntInfo(const char* name);
	// �����ʵ��
	virtual IEntity* GetMainEntity();
	// ���ʵ��
	virtual IEntity* GetEntity(const PERSISTID& id);
	// ������е�ʵ�����ID
	virtual size_t GetAllEntity(IVarList& result);
	// �������ַ��ϵĵ�һ��ʵ��
	virtual IEntity* LookupEntity(const char* name);
	// �������ַ��ϵ�����ʵ��
	virtual size_t LookupEntityMore(const char* name, IVarList& result);
	// ����ʵ��
	virtual IEntity* CreateEntity(const char* name);
	// ����������ʵ��
	virtual IEntity* CreateEntityArgs(const char* name, 
		const IVarList& args);
	// ɾ��ʵ��
	virtual bool DeleteEntity(const PERSISTID& id);
	
	// ���ʵ������
	virtual bool GetProperty(IEntity* pEntity, const char* prop, 
		IVar& value);
	// ����ʵ������
	virtual bool SetProperty(IEntity* pEntity, const char* prop, 
		const IVar& value);

	// ��ӵ����ж���
	virtual bool AddExecute(IEntity* pEntity);
	// �����ж����Ƴ�
	virtual bool RemoveExecute(IEntity* pEntity);
	// ��Ӵ�����Ϣ����msg����С��0x1000��priorԽ�󼶱�Խ�ߣ��ȼ������ȣ�
	virtual bool AddMsgProc(IEntity* pEntity, unsigned int msg, 
		int prior = 0);
	// ���������������Ϣ��msg����С��0x1000��priorԽ�󼶱�Խ�ߣ��ȼ������ȣ�
	virtual bool AddMsgProcInstant(IEntity* pEntity, unsigned int msg, 
		int prior = 0);
	// �Ƴ�������Ϣ����
	virtual bool RemoveMsgProc(IEntity* pEntity, unsigned int msg);
	
	// ȫ�ֱ���
	virtual bool FindGlobalValue(const char* name);
	virtual bool RemoveGlobalValue(const char* name);
	virtual bool SetGlobalValue(const char* name, const IVar& value);
	virtual IVar& GetGlobalValue(const char* name);
	virtual size_t GetGlobalCount();
	virtual size_t GetGlobalList(IVarList& result);

	// ÿ��֡��
	virtual void SetFramePerSecond(int value) { m_nFramePerSecond = value; }
	virtual int GetFramePerSecond() { return m_nFramePerSecond; }

	// ��õ�ǰ֡ʱ��
	virtual double GetFrameSeconds() { return m_dFrameSeconds; }
	// ��õ�ǰ�ڲ���ʱ��ʱ��
	virtual double GetTotalSeconds() { return m_dTotalSeconds; }
	
	// �����Ƿ��¼�ļ�������־
	virtual void SetLogFileIO(bool value);

	// ���core���ܼ��
	virtual core_performance_info_t* GetPerformance();

	// �޸�·��
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
