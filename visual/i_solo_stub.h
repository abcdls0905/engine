//--------------------------------------------------------------------
// �ļ���:		i_solo_stub.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��12��2��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------


#ifndef _VISUAL_I_SOLO_CLIENT_H
#define _VISUAL_I_SOLO_CLIENT_H

#include "../public/module.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"

enum MsgSort
{
	CLIENT_MSG,	//�ͻ�����Ϣ
	REMOTE_MSG,	//Զ�̷���Ϣ
	INNER_MSG,	//�����ڲ���Ϣ
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

	// ��ѭ��
	virtual void Execute(float seconds) = 0;

	//virtual ICollideManager* GetCollideManager() const  = 0;

	//virtual IKernel* GetKernel() const  = 0;

	// ��ǰ����ִ�г�ʼ�����߼�ģ������
	virtual const char* GetInitModuleName() const  = 0;

	// �Ƿ�ʹ���߳�
	virtual void SetUseThread(bool value)  = 0;
	virtual bool GetUseThread() const  = 0;

	// �Ƿ�ʹ�ÿ�����־ϵͳ
	virtual void SetUseQuickLog(bool value)  = 0;
	virtual bool GetUseQuickLog() const  = 0;

	// �Ƿ�ʹ��Ӧ�ý��̼�¼
	virtual void SetUseAppRecord(bool value)  = 0;
	virtual bool GetUseAppRecord() const  = 0;

	// ���̰�CPU����
	virtual void SetBindCore(int value) = 0;
	virtual int GetBindCore() const  = 0;

	// ������
	virtual void SetAppName(const char* value)  = 0;
	virtual const char* GetAppName() const  = 0;

	// ������־�����������
	virtual void SetLogLauncher(const char* value)  = 0;
	virtual const char* GetLogLauncher() const  = 0;

	// �������
	virtual void SetDistrictId(int value)  = 0;
	virtual int GetDistrictId() const  = 0;

	// ��Ϸ���������
	virtual void SetServerId(int value)  = 0;
	virtual int GetServerId() const  = 0;

	// �߼��������������ʱ�������
	virtual void SetMaxRunningTime(int value)  = 0;
	virtual int GetMaxRunningTime() const  = 0;

	// ������ʱ��ʱ��
	virtual void SetBeatTime(int value) = 0;
	virtual int GetBeatTime() const  = 0;

	// ɨ�趨ʱ��ʱ��
	virtual void SetScanTime(int value) = 0;
	virtual int GetScanTime() const  = 0;

	// �߼�����Ŀ�ɨ�趨ʱ��ʱ��
	virtual void SetFastScanTime(int value) = 0;
	virtual int GetFastScanTime() const  = 0;

	// �߼��������ɨ�趨ʱ��ʱ��
	virtual void SetSlowScanTime(int value) = 0;
	virtual int GetSlowScanTime() const  = 0;

	// �����ָ�Ĵ�С
	virtual void SetSceneGridSize(int value) = 0;
	virtual int GetSceneGridSize() const  = 0;

	// ���ִ����Ƿ���ʾ
	virtual void SetDebugHalt(bool value)  = 0;
	virtual bool GetDebugHalt() const  = 0;

	// ������볡�����Ƿ�������ʾ
	virtual void SetAppearAtOnce(bool value)  = 0;
	virtual bool GetAppearAtOnce() const  = 0;

	// �Ƿ񲻷���Ϊȱʡ��ֵ�Ķ�������
	virtual void SetHideEmptyProperty(bool value)  = 0;
	virtual bool GetHideEmptyProperty() const  = 0;

	// �Ƿ�ʹ���ڲ��ƶ�ɨ��
	virtual void SetIsUseInnerMoveableScan(bool value)  = 0;
	virtual bool GetIsUseInnerMoveableScan() const  = 0;

	// ��ײ�����ļ��ĸ�·��
	virtual void SetCollideRootPath(const char* value)  = 0;
	virtual const char* GetCollideRootPath() const  = 0;

	// �Ƿ�Ҫ�ϲ������ƶ���Ϣ 
	virtual void SetMergeMoving(bool value)  = 0;
	virtual bool GetMergeMoving() const  = 0;

	// �Ƿ�Ҫ�ϲ��������Ըı���Ϣ 
	virtual void SetMergeProperty(bool value)  = 0;
	virtual bool GetMergeProperty() const  = 0;

	// �Ƿ�Ҫ�ϲ���������Ƴ���Ϣ 
	virtual void SetMergeObjects(bool value) = 0;
	virtual bool GetMergeObjects() const  = 0;

	// �Ƿ��¼ÿ֡������ʱ��
	virtual void SetLogFrameTime(bool value)  = 0;
	virtual bool GetLogFrameTime() const  = 0;

	// �Ƿ��ʹ�ýű�
	virtual void SetUseLuaScript(bool value)  = 0;
	virtual bool GetUseLuaScript() const  = 0;

	// �Ƿ�ʹ���ڴ��
	virtual void SetEnableMemPool(bool value) = 0;
	virtual bool GetEnableMemPool() const = 0;

	// �Ƿ�ʹ�ö����
	virtual void SetEnableObjectPool(bool value) = 0;
	virtual bool GetEnableObjectPool() const = 0;

	// �ű�ϵͳ�Ƿ�ʹ���ڴ��
	virtual void SetScriptUseMemPool(bool value) = 0;
	virtual bool GetScriptUseMemPool() const = 0;

	// ������ײ��Ϣ������
	virtual void SetCollideManagerID(const PERSISTID& value) = 0;
	virtual PERSISTID GetCollideManagerID() const = 0;

	// ����
	virtual bool Start() = 0;
	//�Ƿ�����
	virtual bool IsRunning() = 0;
	// ֹͣ
	virtual bool Stop() = 0;

	// ���ÿ���̨�ַ���ɫ
	virtual bool SetConsoleColor(const char* color) = 0;
	// ����̨�����Ϣ
	virtual bool WriteLine(const char* info) = 0;

	// ��ʱ���
	void TimerBeat(float seconds);
	void TimerScan(float seconds);

	// ����ȫ�ֵ���Ϣ
	virtual bool LoadLocations(const char* file_name) = 0;

	// ����������ݵ��ļ�
	//bool DumpPlayers(const char* file_name);
	// ����������Ϣ���ļ�
	virtual bool DumpScene(int scene_id, const char* info_type) = 0;
	// �����߼�ģ������ͳ������
	virtual bool DumpPerformance(const char* file_name) = 0;
	// ����߼�ģ������ͳ������
	virtual bool ClearPerformance() = 0;

	// ����ڴ�����ڴ�����
	virtual double GetMemPoolSize() const = 0;
	// ����ڴ���п����ڴ���
	virtual double GetMemFreeSize() const = 0;

	/// \brief ���ָ�����ֵ��߼�ģ��
	/// \param name �߼�ģ����
	virtual ILogicModule* GetLogicModule(const char* name) = 0;

	/// \brief ��ӳ�����Ϣ(scenes.ini)
	/// \param scene_id ����id
	/// \param member �������������
	/// \param type ��������
	/// \param prototype ԭ�ͳ���
	/// \param script �ű�
	/// \param config �����ļ�
	/// \param group �����
	virtual bool AddScene(int scene_id, int member, int type, 
		int prototype, const char* script, const char* config,int group) = 0;
	/// \brief ��ó����߼��ű���
	/// \param scene_id ����id
	virtual const char* GetSceneScript(int scene_id) = 0;
	/// \brief ��ó�����������
	/// \param scene_id ����id
	virtual const char* GetSceneConfig(int scene_id) = 0;
	/// \brief ���ҳ���ID
	/// \param config ��������
	virtual int FindSceneId(const char* config) = 0;	
	/// \brief �������ظ���
	/// \param nProtoSceneId ԭ�ͳ���
	virtual bool CreateStubCloneScene(int nProtoSceneId) = 0;
	/// \brief �������
	/// \param pClient ���ؿͻ��˶���
	/// \param pMainPlayer �������Ƕ���
	virtual bool AddMainPlayer(IGameClient* pClient,IGameObject* pMainPlayer) = 0;
	/// \brief �˳���������
	/// \param pClient ���ؿͻ��˶���
	/// \param pMainPlayer �������Ƕ���
	virtual bool PlayerExitStubCloneScene(IGameClient* pClient,IGameObject* pMainPlayer) = 0;
	// ɾ����������
	virtual bool DeleteStubCloneScene() = 0;
	/// \brief ������Ϣ��Զ�̷�����
	/// \param pdata ��������
	/// \param len ���ݳ���
	virtual bool SendToServer(const void* pdata, size_t len) = 0;
	/// \brief ������Ϣ�����ط�����
	/// \param type ��Ϣ����
	/// \param pdata ��������
	/// \param len ���ݳ���
	/// \param src 0 Ϊ�ͻ��ˣ�1ΪԶ�̷�
	virtual bool PutMessage(int type, const void* pdata, size_t len,int src = 0) = 0;
	/// \brief ������Ϣ���ͻ���ģ��
	/// \param pdata ��������
	/// \param len ���ݳ���
	virtual bool SendToClient(const void* pdata, size_t len) = 0;
	virtual bool SendToClient2(const void* pdata1, size_t len1, const void* pdata2, size_t len2) = 0;
	/// \brief �������
	/// \param id �������id
	/// \param len ���ݳ���
	virtual void SetGameSockID(const PERSISTID& id) = 0;
	virtual PERSISTID GetGameSockID() const = 0;
	/// \brief ������ܶ���
	/// \param id �������id
	/// \param len ���ݳ���
	virtual void SetGameReceiverID(const PERSISTID& id) = 0;
	virtual PERSISTID GetGameReceiverID() const = 0;
};

#endif // _SoloWar_H
