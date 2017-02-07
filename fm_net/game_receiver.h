//--------------------------------------------------------------------
// �ļ���:		game_receiver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_RECEIVER_H
#define _GAME_RECEIVER_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../public/var_list.h"
#include "../utils/load_archive.h"
#include "../utils/array_pod.h"
#include "../visual/i_game_receiver.h"

// ��Ϸ���ݽ���
//#define __FULL_MSG_INFO_PERF__

class GameClient;
class GameObject;
class GameScene;
class GameSceneObj;
class GameView;
class GameViewObj;
class CGameRecord;
class IGameMsgHandler;
class CustomHandler;
class ISoloStub;

class GameReceiver: public IGameReceiver
{
private:
	enum { BUFFER_LEN = 0x10000 };

	struct prop_data_t
	{
		core_string strName;
		int nType;
		unsigned int nCount;//ͳ�����Ա仯����
	};

	struct rec_data_t
	{
		rec_data_t(): nRecordAddRowCount(0), nRecordClearCount(0),
			nRecordGridCount(0), nRecordDelRowCount(0),
			dRecordAddRowTime(0.0F), dRecordDelRowTime(0.0F),
			dRecordGridTime(0.0F), dRecordClearTime(0.0F) {}
		
		core_string strName;
		size_t nCols;
		TArrayPod<int, 1, TCoreAlloc> ColTypes;
		unsigned int nRecordAddRowCount;//ͳ�Ʊ�������Ӵ���
		unsigned int nRecordClearCount;//ͳ�Ʊ���������
		unsigned int nRecordGridCount;//ͳ�Ʊ�����ݸı����
		unsigned int nRecordDelRowCount;//ͳ�Ʊ������ɾ����������
		double dRecordAddRowTime;
		double dRecordDelRowTime;
		double dRecordGridTime;
		double dRecordClearTime;
	};

	struct role_data_t
	{
		unsigned int nRoleIndex;
		unsigned int nSysFlags;
		core_wstring wsName;
		core_wstring wsPara;
		int nDeleted;
		double dDeleteTime;
		TVarList<8, 64, TCoreAlloc> Parameters;
	};

	struct menu_data_t
	{
		unsigned int nType;
		unsigned int nMark;
		core_wstring wsInfo;
	};

	struct msg_stat_t
	{
		unsigned int nMsgCount;
		unsigned int nMsgBytes;
		unsigned int nMaxSize;
		unsigned int nCPMaxSize;
		double dProcessTime;
	};

public:
	GameReceiver();
	virtual ~GameReceiver();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ��Ϸ�ͻ��˶���
	void SetClientID(const PERSISTID& id);
	PERSISTID GetClientID() const;

	// ���ط������
	void SetStubID(const PERSISTID& id);
	PERSISTID GetStubID() const;

	// ��Ϸ��Ϣ�������
	void SetGameMsgHanderID(const PERSISTID& id);
	PERSISTID GetGameMsgHandlerID() const;

	// �Զ�����Ϣ����������
	void SetCustomHandlerID(const PERSISTID& id);
	PERSISTID GetCustomHandlerID() const;
	
	// �Զ�����Ϣ������������
	void SetMaxCustomArguments(int value);
	int GetMaxCustomArguments() const;

	// ��ý�ɫ����
	int GetRoleCount() const;
	// ���ÿ����ɫ����Ϣ��������
	int GetRoleInfoCount() const;
	// ��ý�ɫλ��
	int GetRoleIndex(int index) const;
	// ��ý�ɫ��־
	int GetRoleSysFlags(int index) const;
	// ��ý�ɫ����
	const wchar_t* GetRoleName(int index) const;
	// ��ý�ɫ����
	const wchar_t* GetRolePara(int index) const;
	// ��ý�ɫ�Ƿ�ɾ��
	int GetRoleDeleted(int index) const;
	// ��ý�ɫɾ��ʱ��
	double GetRoleDeleteTime(int index) const;
	// ������еĽ�ɫ��Ϣ�����ܰ���������Ϣ��
	void GetRoleInfo(const IVarList& args, IVarList& ret) const;
	// ��ò˵��������
	int GetMenuCount() const;
	// ��ò˵��������
	int GetMenuType(int index) const;
	// ��ò˵����ѡ����
	int GetMenuMark(int index) const;
	// ��ò˵��������
	const wchar_t* GetMenuContent(int index) const;

	// �������������Ϣ
	bool ClearAll(); 

	// ������Ϣ
	bool ProcessMessage(int src,const void* pdata, size_t len);

	// ������Ϣͳ������
	bool DumpMsgStat(const char* file_name);

	// �����������
	int	GetPropertyCount() const;
	// ���������
	const char* GetPropertyName(size_t index) const;

	virtual int GetPropertyType(size_t index) const;
	// ��ñ������
	int GetRecordCount() const;
	// ��ñ����
	const char* GetRecordName(size_t index) const;
	// ��ñ������
	int GetRecordCols(size_t index) const;

	ISoloStub* GetStub() const;

	//////////////////////////////////////////////////////////////////////////
	//tyz  2014/2/28
	// ��ñ������
	virtual int GetRecordColsType(size_t index,size_t col) const;

private:

	// ��ó����ж���
	GameSceneObj* GetSceneObj(const char* ident);
	// ��������Ӵ�����
	GameView* GetView(const char* view_ident);
	// ��������Ӵ��ж���
	GameViewObj* GetViewObj(const char* view_ident, const char* item_ident);

	// ������������
	bool RecvProperty(GameObject* pObj, CLoadArchive& ar, size_t count, 
		bool change);
	// ������������������
	bool RecvViewObjProperty(GameObject* pObj, CLoadArchive& ar, size_t count, 
		bool change, const char* view_ident, const char* item_ident);
	// ���ձ�������
	bool RecvRecordRow(CGameRecord* pRec, size_t index, CLoadArchive& ar, 
		size_t row, size_t count);
	// ���ձ������
	bool RecvRecordGrid(CGameRecord* pRec, int is_viewobj, int nIdent, 
        int nSerial, size_t index, CLoadArchive& ar, size_t count);
	// ��ӱ��
	bool AddRecord(GameObject* pObj, size_t index);
	
	// ������Ϣ
	bool ServerSetVerify(const void* pdata, size_t len);
	bool ServerSetEncode(const void* pdata, size_t len);
	bool ServerErrorCode(const void* pdata, size_t len);
	bool ServerLoginSucceed(const void* pdata, size_t len);
	bool ServerWorldInfo(const void* pdata, size_t len);
	bool ServerTerminate(const void* pdata, size_t len);
	bool ServerEntryScene(const void* pdata, size_t len);
	bool ServerEntrySceneEx(const void* pdata, size_t len);
	bool ServerExitScene(const void* pdata, size_t len);
	bool ServerAddObject(const void* pdata, size_t len);
	bool ServerRemoveObject(const void* pdata, size_t len);
	bool ServerPropertyTable(const void* pdata, size_t len);
	bool ServerRecordTable(const void* pdata, size_t len);
	bool ServerSceneProperty(const void* pdata, size_t len);
	bool ServerObjectProperty(int src,const void* pdata, size_t len);
	bool ServerSpeech(const void* pdata, size_t len);
	bool ServerSystemInfo(const void* pdata, size_t len);
	bool ServerMenu(const void* pdata, size_t len);
	bool ServerClearMenu(const void* pdata, size_t len);
	bool ServerCustom(int src,const void* pdata, size_t len);
	bool ServerCreateView(const void* pdata, size_t len);
	bool ServerDeleteView(const void* pdata, size_t len);
	bool ServerViewProperty(const void* pdata, size_t len);
	bool ServerViewAdd(const void* pdata, size_t len);
	bool ServerViewRemove(const void* pdata, size_t len);
	bool ServerViewChange(const void* pdata, size_t len);
	bool ServerRecordAddRow(int src,const void* pdata, size_t len);
	bool ServerRecordDelRow(int src,const void* pdata, size_t len);
	bool ServerRecordGrid(int src,const void* pdata, size_t len);
	bool ServerRecordClear(int src,const void* pdata, size_t len);
	bool ServerLocation(const void* pdata, size_t len);
	bool ServerMoving(const void* pdata, size_t len);
	bool ServerMovingEx(const void* pdata, size_t len);
	bool ServerAllDest(const void* pdata, size_t len);
	bool ServerAllDestEx(const void* pdata, size_t len);
	bool ServerIdle(const void* pdata, size_t len);
	bool ServerWarning(const void* pdata, size_t len);
	bool ServerFromGmcc(const void* pdata, size_t len);
	bool ServerQueue(const void* pdata, size_t len);
	bool ServerLinkTo(const void* pdata, size_t len);
	bool ServerUnlink(const void* pdata, size_t len);
	bool ServerLinkMove(const void* pdata, size_t len);
	bool ServerAllProp(const void* pdata, size_t len);
	bool ServerAddMoreObject(const void* pdata, size_t len);
	bool ServerRemoveMoreObject(const void* pdata, size_t len);
	bool ServerChargeValidstring(const void* pdata, size_t len);
	bool ServerMeasureDelay(const void* pdata, size_t len);
	bool ServerFrameUpdate(const void* pdata, size_t len);
	bool ServerFrameMoving(const void* pdata, size_t len);
	bool ServerFrameMovingDiff(const void* pdata, size_t len);

	bool ServerMsgToStub(const void* pdata, size_t len);
	bool StubMsgToClient(const void* pdata, size_t len);

	void ClearPropertyTable()
	{
		for (size_t i = 0; i < m_PropertyTable.size(); ++i)
		{
			CORE_DELETE(m_PropertyTable[i]);
		}

		m_PropertyTable.clear();
	}

	void ClearRecordTable()
	{
		for (size_t i = 0; i < m_RecordTable.size(); ++i)
		{
			CORE_DELETE(m_RecordTable[i]);
		}

		m_RecordTable.clear();
	}

	void ClearRoles()
	{
		for (size_t i = 0; i < m_Roles.size(); ++i)
		{
			CORE_DELETE(m_Roles[i]);
		}

		m_Roles.clear();
	}

	void ClearMenus()
	{
		for (size_t i = 0; i < m_Menus.size(); ++i)
		{
			CORE_DELETE(m_Menus[i]);
		}

		m_Menus.clear();
	}
	
private:
	GameClient* m_pClient;
	ISoloStub * m_pSoloStub;
	IGameMsgHandler* m_pMsgHandler;
	CustomHandler* m_pCustomHandler;
	int m_nMaxCustomArguments;
	char* m_pBuffer;
	TArrayPod<prop_data_t*, 1, TCoreAlloc> m_PropertyTable;
	TArrayPod<rec_data_t*, 1, TCoreAlloc> m_RecordTable;
	TArrayPod<role_data_t*, 1, TCoreAlloc> m_Roles;
	TArrayPod<menu_data_t*, 1, TCoreAlloc> m_Menus;
	msg_stat_t m_MsgStats[256];
#ifdef __FULL_MSG_INFO_PERF__
	FILE* m_fp_info;
#endif
};

#endif // _GAME_RECEIVER_H

