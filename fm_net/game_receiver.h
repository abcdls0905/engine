//--------------------------------------------------------------------
// 文件名:		game_receiver.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 游戏数据接收
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
		unsigned int nCount;//统计属性变化次数
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
		unsigned int nRecordAddRowCount;//统计表格表格增加次数
		unsigned int nRecordClearCount;//统计表格清除次数
		unsigned int nRecordGridCount;//统计表格数据改变次数
		unsigned int nRecordDelRowCount;//统计表格数据删除行数次数
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
	
	// 游戏客户端对象
	void SetClientID(const PERSISTID& id);
	PERSISTID GetClientID() const;

	// 本地服务对象
	void SetStubID(const PERSISTID& id);
	PERSISTID GetStubID() const;

	// 游戏消息处理对象
	void SetGameMsgHanderID(const PERSISTID& id);
	PERSISTID GetGameMsgHandlerID() const;

	// 自定义消息处理器对象
	void SetCustomHandlerID(const PERSISTID& id);
	PERSISTID GetCustomHandlerID() const;
	
	// 自定义消息的最大参数数量
	void SetMaxCustomArguments(int value);
	int GetMaxCustomArguments() const;

	// 获得角色数量
	int GetRoleCount() const;
	// 获得每个角色的信息参数数量
	int GetRoleInfoCount() const;
	// 获得角色位置
	int GetRoleIndex(int index) const;
	// 获得角色标志
	int GetRoleSysFlags(int index) const;
	// 获得角色名称
	const wchar_t* GetRoleName(int index) const;
	// 获得角色参数
	const wchar_t* GetRolePara(int index) const;
	// 获得角色是否被删除
	int GetRoleDeleted(int index) const;
	// 获得角色删除时间
	double GetRoleDeleteTime(int index) const;
	// 获得所有的角色信息（可能包含附加信息）
	void GetRoleInfo(const IVarList& args, IVarList& ret) const;
	// 获得菜单项的数量
	int GetMenuCount() const;
	// 获得菜单项的类型
	int GetMenuType(int index) const;
	// 获得菜单项的选择标记
	int GetMenuMark(int index) const;
	// 获得菜单项的内容
	const wchar_t* GetMenuContent(int index) const;

	// 清除所有数据信息
	bool ClearAll(); 

	// 处理消息
	bool ProcessMessage(int src,const void* pdata, size_t len);

	// 导出消息统计数据
	bool DumpMsgStat(const char* file_name);

	// 获得属性数量
	int	GetPropertyCount() const;
	// 获得属性名
	const char* GetPropertyName(size_t index) const;

	virtual int GetPropertyType(size_t index) const;
	// 获得表格数量
	int GetRecordCount() const;
	// 获得表格名
	const char* GetRecordName(size_t index) const;
	// 获得表格列数
	int GetRecordCols(size_t index) const;

	ISoloStub* GetStub() const;

	//////////////////////////////////////////////////////////////////////////
	//tyz  2014/2/28
	// 获得表格列数
	virtual int GetRecordColsType(size_t index,size_t col) const;

private:

	// 获得场景中对象
	GameSceneObj* GetSceneObj(const char* ident);
	// 获得容器视窗对象
	GameView* GetView(const char* view_ident);
	// 获得容器视窗中对象
	GameViewObj* GetViewObj(const char* view_ident, const char* item_ident);

	// 接收属性数据
	bool RecvProperty(GameObject* pObj, CLoadArchive& ar, size_t count, 
		bool change);
	// 接收容器对像属数据
	bool RecvViewObjProperty(GameObject* pObj, CLoadArchive& ar, size_t count, 
		bool change, const char* view_ident, const char* item_ident);
	// 接收表行数据
	bool RecvRecordRow(CGameRecord* pRec, size_t index, CLoadArchive& ar, 
		size_t row, size_t count);
	// 接收表格数据
	bool RecvRecordGrid(CGameRecord* pRec, int is_viewobj, int nIdent, 
        int nSerial, size_t index, CLoadArchive& ar, size_t count);
	// 添加表格
	bool AddRecord(GameObject* pObj, size_t index);
	
	// 接收消息
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

