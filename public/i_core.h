//--------------------------------------------------------------------
// �ļ���:		i_core.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_CORE_H
#define _PUBLIC_I_CORE_H

#include "macros.h"
#include "persistid.h"
#include "i_var.h"
#include "i_var_list.h"
#include <stdarg.h>

// ���Ľӿڰ汾��
#define FM_CORE_VERSION 0x2000

class ICore;
class CCoreConfig;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ��֤�汾��
extern bool core_version(int version);
// ��������
extern ICore* core_initialize(const CCoreConfig& config);
// ��������
extern bool core_startup(ICore* pCore, const IVarList& args);
// �رպ���
extern bool core_shutdown(ICore* pCore);

//��־����
enum LOG_TYPE
{
	LOG_RENDER,//��Ⱦ���
	LOG_GUI,//GUi��ؼ��
	LOG_NET,//������ؼ��
	LOG_CORE,//Core��ؼ��
	LOG_PHYSICS,//������
	LOG_LOGIC,//�߼����
	LOG_FILEIO,//�ļ�IO���
	LOG_FRAME,//��Ⱦ֡����
	LOG_PERF,//���ܳ�ʱ��ؼ��
	LOG_TYPE_MAX,//���������
};

//д��һ����־ (֧��1024���ֽ�������Ч��
extern bool fm_tracelog(const char * info,LOG_TYPE log_type = LOG_LOGIC );

//��ȡһ����־
extern char* fm_logread(LOG_TYPE log_type = LOG_LOGIC );


#ifdef __cplusplus
}
#endif // __cplusplus

// ���Ľӿ�

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
	double dNeedExecPerFrameFuncTime;//����,Ŀǰ�ײ㲻���
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

	// ��ģ���ڴ����
	virtual void* MemoryCheckAllocDebug(size_t size, const char* file, 
		int line) = 0;
	virtual void* MemoryCheckAlloc(size_t size) = 0;
	virtual void MemoryCheckFree(void* ptr, size_t size) = 0;
	virtual void* MemoryAlloc(size_t size) = 0;
	virtual void MemoryFree(void* ptr) = 0;

	// ��ù���·��
	virtual const char* GetWorkPath() = 0;
	// ��ó�����Դ·��
	virtual const char* GetProgramPath() = 0;
	// �����Դ·��
	virtual const char* GetResourcePath() = 0;

	// ��ѭ������ʱ��
	virtual void SetSleep(int value) = 0;
	virtual int GetSleep() = 0;

	// ���Ƴ����˳�
	virtual void SetQuit(bool value) = 0;
	virtual bool GetQuit() = 0;

	// ��������Ϣ
	virtual bool ProcessMessage(unsigned int msg, size_t param1, size_t param2, 
		int& result) = 0;

	// ÿ֡����
	virtual void FrameCheckMessage() = 0;
	virtual void FrameExecute(double seconds) = 0;
	virtual void FrameSleep() = 0;

	// ������־���ļ�
	virtual bool TraceLog(const char* log) = 0;
	virtual bool TraceLogW(const wchar_t* log) = 0;
	// �ض�����־��ָ������
	virtual bool SetTraceFunc(TRACE_FUNC func, void* context) = 0;
	// ����������Ϣ���ļ�
	virtual bool DumpFile(const char* type, const char* file) = 0;

	// ����ļ�ϵͳ
	virtual IFileSys* GetFileSys() = 0;

	// ���������ļ���д�ӿ�
	virtual IIniFile* CreateIniFile(const char* filename, 
		int read_raw_file = 0) = 0;

	// ���ҹ��ܽӿ�
	virtual bool FindInterface(const char* name) = 0;
	// ��ù��ܽӿ�
	virtual IInterface* GetInterface(const char* name) = 0;
	// �ͷŹ��ܽӿ�
	virtual void ReleaseInterface(IInterface* pInterface) = 0;

	// ����ʵ������Ϣ
	virtual IEntInfo* GetEntInfo(const char* name) = 0;
	// �����ʵ��
	virtual IEntity* GetMainEntity() = 0;
	// ���ʵ��
	virtual IEntity* GetEntity(const PERSISTID& id) = 0;
	// ������е�ʵ�����ID
	virtual size_t GetAllEntity(IVarList& result) = 0;
	// �������ַ��ϵĵ�һ��ʵ��
	virtual IEntity* LookupEntity(const char* name) = 0;
	// �������ַ��ϵ�����ʵ��
	virtual size_t LookupEntityMore(const char* name, IVarList& result) = 0;
	// ����ʵ��
	virtual IEntity* CreateEntity(const char* name) = 0;
	// ����������ʵ��
	virtual IEntity* CreateEntityArgs(const char* name, 
		const IVarList& args) = 0;
	// ɾ��ʵ��
	virtual bool DeleteEntity(const PERSISTID& id) = 0;

	// ���ʵ������
	virtual bool GetProperty(IEntity* pEntity, const char* prop, 
		IVar& value) = 0;
	// ����ʵ������
	virtual bool SetProperty(IEntity* pEntity, const char* prop, 
		const IVar& value) = 0;

	// ��ӵ����ж���
	virtual bool AddExecute(IEntity* pEntity) = 0;
	// �����ж����Ƴ�
	virtual bool RemoveExecute(IEntity* pEntity) = 0;
	// ��Ӵ�����Ϣ����msg����С��0x1000��priorԽ�󼶱�Խ�ߣ��ȼ������ȣ�
	virtual bool AddMsgProc(IEntity* pEntity, unsigned int msg, 
		int prior = 0) = 0;
	// ���������������Ϣ��msg����С��0x1000��priorԽ�󼶱�Խ�ߣ��ȼ������ȣ�
	virtual bool AddMsgProcInstant(IEntity* pEntity, unsigned int msg, 
		int prior = 0) = 0;
	// �Ƴ�������Ϣ����
	virtual bool RemoveMsgProc(IEntity* pEntity, unsigned int msg) = 0;

	// ȫ�ֱ���
	virtual bool FindGlobalValue(const char* name) = 0;
	virtual bool RemoveGlobalValue(const char* name) = 0;
	virtual bool SetGlobalValue(const char* name, const IVar& value) = 0;
	virtual IVar& GetGlobalValue(const char* name) = 0;
	virtual size_t GetGlobalCount() = 0;
	virtual size_t GetGlobalList(IVarList& result) = 0;

	// ÿ��֡��
	virtual void SetFramePerSecond(int value) = 0;
	virtual int GetFramePerSecond() = 0;

	// ��õ�ǰ֡ʱ��
	virtual double GetFrameSeconds() = 0;
	// ��õ�ǰ�ڲ���ʱ��ʱ��
	virtual double GetTotalSeconds() = 0;

	// �����Ƿ��¼�ļ�������־
	virtual void SetLogFileIO(bool value) = 0;

	// ���core���ܼ��
	virtual core_performance_info_t* GetPerformance() = 0;

	// �޸�·��
	virtual void SetPathConfig(CCoreConfig& config) = 0;
};

inline ICore::~ICore() {}

#endif // _PUBLIC_I_CORE_H
