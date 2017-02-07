//--------------------------------------------------------------------
// �ļ���:		game_receiver.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_receiver.h"
#include "game_client.h"
#include "custom_handler.h"
#include "../public/outer_msg.h"
#include "../visual/i_game_client.h"
#include "../visual/i_solo_stub.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../extern/quicklz/quicklz.h"
#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime
/// \file game_receiver.cpp
/// \brief ��Ϸ��Ϣ������

/// entity: IGameMsgHandler
/// \brief ��Ϸ��Ϣ����ʵ�����
DECLARE_ENTITY(IGameMsgHandler, NULL, IEntity);

/// entity: GameReceiver
/// \brief ��Ϸ��Ϣ������ʵ��
DECLARE_ENTITY(GameReceiver, NULL, IEntity);

/// property: object Client
/// \brief ��Ϸ�ͻ��˶���
DECLARE_PROPERTY(PERSISTID, GameReceiver, Client, GetClientID, SetClientID);
/// property: object Stub
/// \brief ���ط���������
DECLARE_PROPERTY(PERSISTID, GameReceiver, Stub, GetStubID, SetStubID);
/// property: object GameMsgHandler
/// \brief ��Ϸ��Ϣ�������
DECLARE_PROPERTY(PERSISTID, GameReceiver, GameMsgHandler, GetGameMsgHandlerID, 
	SetGameMsgHanderID);
/// property: object CustomHander
/// \brief �Զ�����Ϣ����������
DECLARE_PROPERTY(PERSISTID, GameReceiver, CustomHandler, 
	GetCustomHandlerID, SetCustomHandlerID);
/// property: int MaxCustomArguments
/// \brief �Զ�����Ϣ������������
DECLARE_PROPERTY(int, GameReceiver, MaxCustomArguments, 
	GetMaxCustomArguments, SetMaxCustomArguments);

/*
/// method: int GetRoleCount()
/// \brief ��ý�ɫ����
DECLARE_METHOD_0(int, GameReceiver, GetRoleCount);
/// method: int GetRoleInfoCount()
/// \brief ���ÿ����ɫ�Ĳ�������
DECLARE_METHOD_0(int, GameReceiver, GetRoleInfoCount);
/// method: int GetRoleIndex(int index)
/// \brief ���ָ����ɫ��λ������
/// \param index ��ɫ����
DECLARE_METHOD_1(int, GameReceiver, GetRoleIndex, int);
/// method: int GetRoleSysFlags(int index)
/// \brief ���ָ����ɫ��ϵͳ��־
/// \param index ��ɫ����
DECLARE_METHOD_1(int, GameReceiver, GetRoleSysFlags, int);
/// method: wstring GetRoleName(int index)
/// \brief ���ָ����ɫ������
/// \param index ��ɫ����
DECLARE_METHOD_1(const wchar_t*, GameReceiver, GetRoleName, int);
/// method: wstring GetRolePara(int index)
/// \brief ���ָ����ɫ�Ĳ���
/// \param index ��ɫ����
DECLARE_METHOD_1(const wchar_t*, GameReceiver, GetRolePara, int);
/// method: int GetRoleDeleted(int index)
/// \brief ���ָ����ɫ�Ƿ�ɾ��
/// \param index ��ɫ����
DECLARE_METHOD_1(int, GameReceiver, GetRoleDeleted, int);
/// method: double GetRoleDeleteTime(int index)
/// \brief ���ָ����ɫ��ɾ��ʱ��
/// \param index ��ɫ����
DECLARE_METHOD_1(double, GameReceiver, GetRoleDeleteTime, int);
/// method: [args] GetRoleInfo(int index)
/// \brief ���ָ����ɫ�����в���
/// \param index ��ɫλ��
DECLARE_METHOD_N(GameReceiver, GetRoleInfo);
/// method: int GetMenuCount()
/// \brief ��ò˵�����
DECLARE_METHOD_0(int, GameReceiver, GetMenuCount);
/// method: int GetMenuType(int index)
/// \brief ��ò˵�����
/// \param index �˵�����
DECLARE_METHOD_1(int, GameReceiver, GetMenuType, int);
/// method: int GetMenuMark(int index)
/// \brief ��ò˵����
/// \param index �˵�����
DECLARE_METHOD_1(int, GameReceiver, GetMenuMark, int);
/// method: wstring GetMenuContent(int index)
/// \brief ��ò˵�����
/// \param index �˵�����
DECLARE_METHOD_1(const wchar_t*, GameReceiver, GetMenuContent, int);

/// method: bool ClearAll()
/// \brief �����������
DECLARE_METHOD_0(bool, GameReceiver, ClearAll);

/// method: bool DumpMsgStat(string file_name)
/// \brief ������Ϣͳ�����ݵ��ļ�
/// \param file_name ����ļ���
DECLARE_METHOD_1(bool, GameReceiver, DumpMsgStat, const char*);

/// event: int on_set_verify(object self, int width, int height)
/// \brief �յ���֤��ͼƬ��Ϣ
/// \param width ͼƬ���
/// \param height ͼƬ�߶�
DECLARE_EVENT(on_set_verify);
/// event: int on_set_encode(object self)
/// \brief �յ����ü�����Ϣ 
DECLARE_EVENT(on_set_encode);
/// event: int on_error_code(object self, int error_code)
/// \brief �յ���������Ϣ
/// \param error_code ������
DECLARE_EVENT(on_error_code);
/// event: int on_login_succeed(object self, bool is_free, int points, 
///		int year, int month, int day, int hour, int minute, int second, int roles);
/// \brief �յ���¼�ɹ���Ϣ
/// \param is_free �Ƿ����
/// \param points �ʺŵĵ���
/// \param year,month,day,hour,minute,second ���½�ֹʱ��
/// \param roles ��ɫ����
DECLARE_EVENT(on_login_succeed);
/// event: int on_terminate(object self, int reason)
/// \brief �յ�ǿ�ƹر���Ϣ
/// \param reason ԭ��
DECLARE_EVENT(on_terminate);
/// event: int on_entry_scene(object self)
/// \brief �յ����볡������Ϣ
DECLARE_EVENT(on_entry_scene);
/// event: int on_entry_scene_ex(object self)
/// \brief �յ��������֡ͬ����������Ϣ
DECLARE_EVENT(on_entry_scene_ex);
/// event: int on_exit_scene(object self)
/// \brief �յ��˳���������Ϣ
DECLARE_EVENT(on_exit_scene);
/// event: int on_add_object(object self, object obj, int attrs)
/// \brief �յ���Ӷ�����Ϣ
/// \param obj �����
/// \param attrs ��������
DECLARE_EVENT(on_add_object);
/// event: int on_remove_object(object self, object obj)
/// \brief �յ��Ƴ�������Ϣ
/// \param obj �����
DECLARE_EVENT(on_remove_object);
/// event: int on_property_table(object self, int count)
/// \brief �յ����Ա���Ϣ
/// \param count ������������
DECLARE_EVENT(on_property_table);
/// event: int on_state_table(object self, int count)
/// \brief �յ�״̬����Ϣ
/// \param count ״̬����
DECLARE_EVENT(on_state_table);
/// event: int on_record_table(object self, int count)
/// \brief �յ���ṹ��Ϣ��Ϣ
/// \param count ���ӱ�����
DECLARE_EVENT(on_record_table);
/// event: int on_scene_property(object self, int count)
/// \brief �յ�����������Ϣ
/// \param count �������� 
DECLARE_EVENT(on_scene_property);
/// event: int on_object_property(object self, object obj, int count)
/// \brief �յ�����������Ϣ
/// \param obj �����
/// \param count �������� 
DECLARE_EVENT(on_object_property);
/// event: int on_speech(object self, object obj, wstring content)
/// \brief �յ���ͨ������Ϣ
/// \param obj �����
/// \param content ��������
DECLARE_EVENT(on_speech);
/// event: int on_system_info(object self, int type, wstring info)
/// \brief �յ�ϵͳ��Ϣ
/// \param type ��Ϣ����
/// \param info ��Ϣ����
DECLARE_EVENT(on_system_info);
/// event: int on_menu(object self, object obj, object async_id, int count)
/// \brief �յ��˵���Ϣ
/// \param obj �����
/// \param async_id �첽ID
/// \param count �˵�������
DECLARE_EVENT(on_menu);
/// event: int on_clear_menu(object self)
/// \brief �յ�����˵���Ϣ
DECLARE_EVENT(on_clear_menu);
/// event: int on_custom(object self, int arg_num, [args])
/// \brief �յ��Զ�����Ϣ
/// \param arg_num ��������
/// \param args ��Ϣ����
DECLARE_EVENT(on_custom);
/// event: int on_create_view(object self, int view_id, int capacity)
/// \brief �յ������Ӵ���Ϣ
/// \param view_id �Ӵ����
/// \param capacity ����
DECLARE_EVENT(on_create_view);
/// event: int on_delete_view(object self, int view_id)
/// \brief �յ�ɾ���Ӵ���Ϣ
/// \param view_id �Ӵ����
DECLARE_EVENT(on_delete_view);
/// event: int on_view_property(object self, int view_id, int count)
/// \brief �յ��Ӵ�������Ϣ
/// \param view_id �Ӵ����
/// \param count ��������
DECLARE_EVENT(on_view_property);
/// event: int on_view_add(object self, int view_id, int obj_index)
/// \brief �յ��Ӵ�����Ӷ�����Ϣ
/// \param view_id �Ӵ����
/// \param obj_index ������������1��ʼ�� 
DECLARE_EVENT(on_view_add);
/// event: int on_view_remove(object self, int view_id, int obj_index)
/// \brief �յ��Ӵ���ɾ��������Ϣ
/// \param view_id �Ӵ����
/// \param obj_index ������������1��ʼ�� 
DECLARE_EVENT(on_view_remove);
/// event: int on_view_object_property(object self, 
///		int view_id, int obj_index, int count)
/// \brief �յ��Ӵ��ж���������Ϣ
/// \param view_id �Ӵ����
/// \param obj_index ������������1��ʼ�� 
/// \param count ��������
DECLARE_EVENT(on_view_object_property);

/// event: int on_view_object_property_change(object self, 
///		int view_id, int obj_index, string name)
/// \brief �յ��Ӵ��ж���������Ϣ
/// \param view_id �Ӵ����
/// \param obj_index ������������1��ʼ�� 
/// \param count ��������
DECLARE_EVENT(on_view_object_property_change);

/// event: int on_record_add_row(object self, string name, int row, int row_num)
/// \brief �յ������������Ϣ
/// \param name ����
/// \param row ������к�
/// \param row_num ������
DECLARE_EVENT(on_record_add_row);
/// event: int on_record_remove_row(object self, string name, int row)
/// \brief �յ�����ɾ������Ϣ
/// \param name ����
/// \param row �к�
DECLARE_EVENT(on_record_remove_row);
/// event: int on_recrod_grid(object self, string name, int count)
/// \brief �յ���������ݸı���Ϣ
/// \param name ����
/// \param count ��������
DECLARE_EVENT(on_record_grid);
/// event: int on_record_single_grid(object self, string name, int row, int clomn)
/// \brief �յ���������ݸı���Ϣ
/// \param name ����
/// \param row ������
/// \param clomn ������
DECLARE_EVENT(on_record_single_grid);
/// event: int on_record_clear(object self, string name)
/// \brief �յ������������������Ϣ
/// \param name ����
DECLARE_EVENT(on_record_clear);
/// event: int on_location(object self)
/// \brief �յ�����λ����Ϣ 
DECLARE_EVENT(on_location);
/// event: int on_moving(object self)
/// \brief �յ������ƶ�Ŀ����Ϣ 
DECLARE_EVENT(on_moving);
/// event: int on_all_dest(object self, int count)
/// \brief �յ���Ұ�����ж����Ŀ�����Ϣ
/// \param count ��������
DECLARE_EVENT(on_all_dest);
/// event: int on_idle(object self)
/// \brief �յ�������Ϣ
DECLARE_EVENT(on_idle);
/// event: int on_warning(object self, int type, wstring info)
/// \brief �յ�������Ϣ
/// \param type ��Ϣ����
/// \param info ��Ϣ����
DECLARE_EVENT(on_warning);
/// event: int on_from_gmcc(object self, int id, wstring name, wstring info)
/// \brief �յ�����GM����Ϣ
/// \param id GM���
/// \param name GM����
/// \param info ��Ϣ����
DECLARE_EVENT(on_from_gmcc);
/// event: int on_server_info(object self, int count)
/// \brief �յ��������б���Ϣ
/// \param count ����������
DECLARE_EVENT(on_server_info);
/// event: int on_queue(object self, int queue, int position)
/// \brief �յ��Ŷ���Ϣ
/// \param queue ���б�ţ�Ϊ0��
/// \param position �����е�λ��
DECLARE_EVENT(on_queue);*/

// GameReceiver
#ifdef __FULL_MSG_INFO_PERF__

#ifdef FX_SYSTEM_WINDOWS
typedef SYSTEMTIME FM_SYSTEMTIME;
#else
typedef struct _FM_SYSTEMTIME {
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
} FM_SYSTEMTIME;
#endif

//�Ȱ�����ֱ�Ӷ�ȡ����
struct _base_msg_
{	
	FM_SYSTEMTIME stime;
	double     dperf;
	size_t     nlen;
	size_t     cplen;
};

struct _msg_ :public _base_msg_
{
	char*      p;
};

#endif
 

GameReceiver::GameReceiver()
{
	m_pClient = NULL;
	m_pSoloStub = NULL;
	m_pMsgHandler = NULL;
	m_pCustomHandler = NULL;
	m_nMaxCustomArguments = 0;
	m_pBuffer = (char*)CORE_ALLOC(BUFFER_LEN);
	memset(m_MsgStats, 0, sizeof(m_MsgStats));

#ifdef __FULL_MSG_INFO_PERF__
	m_fp_info = core_file::fopen("full_msg_perf.net","wb");
#endif
}

GameReceiver::~GameReceiver()
{
	ClearPropertyTable();
	ClearRecordTable();
	ClearRoles();
	ClearMenus();
	CORE_FREE(m_pBuffer, BUFFER_LEN);
#ifdef __FULL_MSG_INFO_PERF__
	core_file::fclose(m_fp_info);
#endif
}

bool GameReceiver::Init(const IVarList& args)
{
	m_pClient = (GameClient*)GetCore()->LookupEntity("GameClient");
	m_pSoloStub = dynamic_cast<ISoloStub*>(GetCore()->LookupEntity("SoloStub"));

	return true;
}

bool GameReceiver::Shut()
{
	return true;
}

void GameReceiver::SetClientID(const PERSISTID& id)
{
	GameClient* pClient = (GameClient*)GetCore()->GetEntity(id);

	if (NULL == pClient)
	{
		return;
	}

	if (!pClient->GetEntInfo()->IsKindOf("GameClient"))
	{
		return;
	}

	m_pClient = pClient;
}

PERSISTID GameReceiver::GetClientID() const
{
	if (NULL == m_pClient)
	{
		return PERSISTID();
	}

	return m_pClient->GetID();
}

void GameReceiver::SetStubID(const PERSISTID& id)
{
	ISoloStub* pSoloStub = dynamic_cast<ISoloStub*>(GetCore()->GetEntity(id));

	if (NULL == pSoloStub)
	{
		return;
	}

	if (!pSoloStub->GetEntInfo()->IsKindOf("SoloStub"))
	{
		return;
	}

	m_pSoloStub = pSoloStub;
}

PERSISTID GameReceiver::GetStubID() const
{
	if (NULL == m_pSoloStub)
	{
		return PERSISTID();
	}

	return m_pSoloStub->GetID();
}

void GameReceiver::SetGameMsgHanderID(const PERSISTID& id)
{
	IGameMsgHandler* p = (IGameMsgHandler*)GetCore()->GetEntity(id);

	if (NULL == p)
	{
		return;
	}

	if (!p->GetEntInfo()->IsKindOf("IGameMsgHandler"))
	{
		return;
	}

	m_pMsgHandler = p;
}

PERSISTID GameReceiver::GetGameMsgHandlerID() const
{
	if (NULL == m_pMsgHandler)
	{
		return PERSISTID();
	}

	return m_pMsgHandler->GetID();
}

void GameReceiver::SetCustomHandlerID(const PERSISTID& id)
{
	CustomHandler* p = (CustomHandler*)GetCore()->GetEntity(id);

	if (NULL == p)
	{
		return;
	}

	if (!p->GetEntInfo()->IsKindOf("CustomHandler"))
	{
		return;
	}

	m_pCustomHandler = p;
}

PERSISTID GameReceiver::GetCustomHandlerID() const
{
	if (NULL == m_pCustomHandler)
	{
		return PERSISTID();
	}

	return m_pCustomHandler->GetID();
}

void GameReceiver::SetMaxCustomArguments(int value)
{
	m_nMaxCustomArguments = value;
}

int GameReceiver::GetMaxCustomArguments() const
{
	return m_nMaxCustomArguments;
}

int GameReceiver::GetRoleCount() const 
{ 
	return (int)m_Roles.size(); 
}

int GameReceiver::GetRoleInfoCount() const
{
	if (m_Roles.empty())
	{
		return 0;
	}

	return (int)m_Roles[0]->Parameters.GetCount();
}

int GameReceiver::GetRoleIndex(int index) const
{
	if (size_t(index) >= m_Roles.size())
	{
		return 0;
	}

	return m_Roles[index]->nRoleIndex;
}

int GameReceiver::GetRoleSysFlags(int index) const
{
	if (size_t(index) >= m_Roles.size())
	{
		return 0;
	}

	return m_Roles[index]->nSysFlags;
}

const wchar_t* GameReceiver::GetRoleName(int index) const
{
	if (size_t(index) >= m_Roles.size())
	{
		return L"";
	}

	return m_Roles[index]->wsName.c_str();
}

const wchar_t* GameReceiver::GetRolePara(int index) const
{
	if (size_t(index) >= m_Roles.size())
	{
		return L"";
	}

	return m_Roles[index]->wsPara.c_str();
}

int GameReceiver::GetRoleDeleted(int index) const
{
	if (size_t(index) >= m_Roles.size())
	{
		return 0;
	}

	return m_Roles[index]->nDeleted;
}

double GameReceiver::GetRoleDeleteTime(int index) const
{
	if (size_t(index) >= m_Roles.size())
	{
		return 0.0;
	}

	return m_Roles[index]->dDeleteTime;
}

void GameReceiver::GetRoleInfo(const IVarList& args, IVarList& ret) const
{
	size_t index = args.IntVal(0);
	
	if (index >= m_Roles.size())
	{
		return;
	}

	ret << m_Roles[index]->Parameters;
}

int GameReceiver::GetMenuCount() const
{
	return (int)m_Menus.size();
}

int GameReceiver::GetMenuType(int index) const
{
	if (size_t(index)  >= m_Menus.size())
	{
		return 0;
	}

	return m_Menus[index]->nType;
}

int GameReceiver::GetMenuMark(int index) const
{
	if (size_t(index) >= m_Menus.size())
	{
		return 0;
	}

	return m_Menus[index]->nMark;
}

const wchar_t* GameReceiver::GetMenuContent(int index) const
{
	if (size_t(index) >= m_Menus.size())
	{
		return L"";
	}

	return m_Menus[index]->wsInfo.c_str();
}
int GameReceiver::GetPropertyCount() const
{
	return m_PropertyTable.size();
}
const char* GameReceiver::GetPropertyName(size_t index) const
{
	if (index >= m_PropertyTable.size())
	{
		return "";
	}

	return m_PropertyTable[index]->strName.c_str();
}
int GameReceiver::GetPropertyType(size_t index) const
{
	if (index >= m_PropertyTable.size())
	{
		return -1;
	}

	return m_PropertyTable[index]->nType;
}

int GameReceiver::GetRecordCount() const
{
	return m_RecordTable.size();
}
const char* GameReceiver::GetRecordName(size_t index) const
{
	if (index >= m_RecordTable.size())
	{
		return "";
	}

	return m_RecordTable[index]->strName.c_str();
}

int GameReceiver::GetRecordCols(size_t index) const
{
	if (index >= m_RecordTable.size())
	{
		return -1;
	}

	return m_RecordTable[index]->nCols;
}
ISoloStub* GameReceiver::GetStub() const
{
	return m_pSoloStub;
}

int GameReceiver::GetRecordColsType(size_t index,size_t col) const
{
	if (index >= m_RecordTable.size())
	{
		return -1;
	}

	if (col >= m_RecordTable[index]->ColTypes.size())
	{
		return -1;
	}

	return m_RecordTable[index]->ColTypes[col];
}
bool GameReceiver::ClearAll()
{
	if (m_pClient)
	{
		m_pClient->ClearAll();
	}

	ClearPropertyTable();
	ClearRecordTable();
	ClearRoles();
	ClearMenus();

	return true;
}

// ��ֹռ�ù����stack�ռ�
static qlz_state_decompress state_decompress;

bool GameReceiver::ProcessMessage(int src,const void* pdata, size_t len)
{
	Assert(pdata != NULL);
	Assert(len > 0);

	if (len < sizeof(server_msg_base_t))
	{
		CORE_TRACE("(GameReceiver::ProcessMessage)size error");
		return false;
	}
#ifdef __FULL_MSG_INFO_PERF__
    
	_base_msg_ _msg;
  #ifndef PERFORMANCE_DEBUG
 	double t1 = performance_time();
  #endif
   
  #ifdef FX_SYSTEM_WINDOWS
	GetSystemTime(&_msg.stime);
  #endif

#endif


#ifdef PERFORMANCE_DEBUG
	double t1 = performance_time();
#endif

	server_msg_base_t* pBase = (server_msg_base_t*)pdata;
	unsigned int msg_type = pBase->nMsgType;

    size_t cplen = len;
	bool is_compressed;

	switch (msg_type)
	{
	case SERVER_CP_CUSTOM:
		is_compressed = true;
		break;
	case SERVER_CP_ADD_OBJECT:
		is_compressed = true;
		break;
	case SERVER_CP_RECORD_ADDROW:
		is_compressed = true;
		break;
	case SERVER_CP_VIEW_ADD:
		is_compressed = true;
		break;
	case SERVER_CP_ALL_DEST:
		is_compressed = true;
		break;
	case SERVER_CP_ALL_DEST_EX:
		is_compressed = true;
		break;
	case SERVER_CP_ALL_PROP:
		is_compressed = true;
		break;
	case SERVER_CP_ADD_MORE_OBJECT:
		is_compressed = true;
		break;
	default:
		is_compressed = false;
		break;
	}

//	if (is_compressed)
//	{
//		pBase = (server_msg_base_t*)m_pBuffer;
//		pBase->nMsgType = msg_type;
//
//		const size_t header_len = sizeof(server_msg_base_t);
//		
//		int origin_size = fastlz_decompress((char*)pdata + header_len, 
//			(int)(len - header_len), m_pBuffer + header_len, 
//			BUFFER_LEN - header_len);
//
//		if (0 == origin_size)
//		{
//			CORE_TRACE("(GameReceiver::ProcessMessage)decompress failed");
//			return false;
//		}
//
//		pdata = m_pBuffer;
//		len = origin_size + header_len;
//	}
	if (is_compressed)
	{
		pBase = (server_msg_base_t*)m_pBuffer;
		pBase->nMsgType = msg_type;

		const size_t header_len = sizeof(server_msg_base_t);
		size_t origin_size = qlz_size_decompressed((char*)pdata + header_len);

		if (origin_size > (BUFFER_LEN - header_len))
		{
			CORE_TRACE("(GameReceiver::ProcessMessage)decompress size error");
			return false;
		}

		size_t decompress_size = qlz_decompress((char*)pdata + header_len, 
			m_pBuffer + header_len, &state_decompress);

		if (decompress_size != origin_size)
		{
			CORE_TRACE("(GameReceiver::ProcessMessage)decompress failed");
			return false;
		}

		pdata = m_pBuffer;
		len = origin_size + header_len;
	}

	bool result = false;

	switch (pBase->nMsgType)
	{
	case SERVER_SET_VERIFY:
		result = ServerSetVerify(pdata, len);
		break;
	case SERVER_SET_ENCODE:
		result = ServerSetEncode(pdata, len);
		break;
	case SERVER_ERROR_CODE:
		result = ServerErrorCode(pdata, len);
		break;
	case SERVER_LOGIN_SUCCEED:
		result = ServerLoginSucceed(pdata, len);
		break;
	case SERVER_WORLD_INFO:
		result = ServerWorldInfo(pdata, len);
		break;
	case SERVER_TERMINATE:
		result = ServerTerminate(pdata, len);
		break;
	case SERVER_ENTRY_SCENE:
		result = ServerEntryScene(pdata, len);
		break;
	case SERVER_ENTRY_SCENE_EX:
		result = ServerEntrySceneEx(pdata, len);
		break;
	case SERVER_EXIT_SCENE:
		result = ServerExitScene(pdata, len);
		break;
	case SERVER_ADD_OBJECT:
		result = ServerAddObject(pdata, len);
		break;
	case SERVER_REMOVE_OBJECT:
		result = ServerRemoveObject(pdata, len);
		break;
	case SERVER_PROPERTY_TABLE:
		result = ServerPropertyTable(pdata, len);
		break;
	case SERVER_RECORD_TABLE:
		result = ServerRecordTable(pdata, len);
		break;
	case SERVER_SCENE_PROPERTY:
		result = ServerSceneProperty(pdata, len);
		break;
	case SERVER_OBJECT_PROPERTY:
		result = ServerObjectProperty(src,pdata, len);
		break;
	case SERVER_SPEECH:
		result = ServerSpeech(pdata, len);
		break;
	case SERVER_SYSTEM_INFO:
		result = ServerSystemInfo(pdata, len);
		break;
	case SERVER_MENU:
		result = ServerMenu(pdata, len);
		break;
	case SERVER_CLEAR_MENU:
		result = ServerClearMenu(pdata, len);
		break;
	case SERVER_CUSTOM:
		result = ServerCustom(src,pdata, len);
		break;
	case SERVER_CREATE_VIEW:
		result = ServerCreateView(pdata, len);
		break;
	case SERVER_DELETE_VIEW:
		result = ServerDeleteView(pdata, len);
		break;
	case SERVER_VIEW_PROPERTY:
		result = ServerViewProperty(pdata, len);
		break;
	case SERVER_VIEW_ADD:
		result = ServerViewAdd(pdata, len);
		break;
	case SERVER_VIEW_REMOVE:
		result = ServerViewRemove(pdata, len);
		break;
	case SERVER_VIEW_CHANGE:
		result = ServerViewChange(pdata, len);
		break;
	case SERVER_RECORD_ADDROW:
		result = ServerRecordAddRow(src,pdata, len);
		break;
	case SERVER_RECORD_DELROW:
		result = ServerRecordDelRow(src,pdata, len);
		break;
	case SERVER_RECORD_GRID:
		result = ServerRecordGrid(src,pdata, len);
		break;
	case SERVER_RECORD_CLEAR:
		result = ServerRecordClear(src,pdata, len);
		break;
	case SERVER_LOCATION:
		result = ServerLocation(pdata, len);
		break;
	case SERVER_MOVING:
		result = ServerMoving(pdata, len);
		break;
	case SERVER_MOVING_EX:
		result = ServerMovingEx(pdata, len);
		break;
	case SERVER_ALL_DEST:
		result = ServerAllDest(pdata, len);
		break;
	case SERVER_ALL_DEST_EX:
		result = ServerAllDestEx(pdata, len);
		break;
	case SERVER_IDLE:
		result = ServerIdle(pdata, len);
		break;
	case SERVER_WARNING:
		result = ServerWarning(pdata, len);
		break;
	case SERVER_FROM_GMCC:
		result = ServerFromGmcc(pdata, len);
		break;
	case SERVER_SERVER_INFO:
		break;
	case SERVER_QUEUE:
		result = ServerQueue(pdata, len);
		break;
	case SERVER_LINK_TO:
		result = ServerLinkTo(pdata, len);
		break;
	case SERVER_UNLINK:
		result = ServerUnlink(pdata, len);
		break;
	case SERVER_LINK_MOVE:
		result = ServerLinkMove(pdata, len);
		break;
	case SERVER_CP_CUSTOM:
		result = ServerCustom(src,pdata, len);
		break;
	case SERVER_CP_ADD_OBJECT:
		result = ServerAddObject(pdata, len);
		break;
	case SERVER_CP_RECORD_ADDROW:
		result = ServerRecordAddRow(src,pdata, len);
		break;
	case SERVER_CP_VIEW_ADD:
		result = ServerViewAdd(pdata, len);
		break;
	case SERVER_CP_ALL_DEST:
		result = ServerAllDest(pdata, len);
		break;
	case SERVER_CP_ALL_DEST_EX:
		result = ServerAllDestEx(pdata, len);
		break;
	case SERVER_ALL_PROP:
		result = ServerAllProp(pdata, len);
		break;
	case SERVER_CP_ALL_PROP:
		result = ServerAllProp(pdata, len);
		break;
	case SERVER_ADD_MORE_OBJECT:
		result = ServerAddMoreObject(pdata, len);
		break;
	case SERVER_CP_ADD_MORE_OBJECT:
		result = ServerAddMoreObject(pdata, len);
		break;
	case SERVER_REMOVE_MORE_OBJECT:
		result = ServerRemoveMoreObject(pdata, len);
		break;
	case SERVER_CHARGE_VALIDSTRING:
		result = ServerChargeValidstring(pdata, len);
		break;
	case SERVER_MEASURE_DELAY:
		result = ServerMeasureDelay(pdata, len);
		break;
	case SERVER_FRAME_UPDATE:
		result = ServerFrameUpdate(pdata, len);
		break;
	case SERVER_FRAME_MOVING:
		result = ServerFrameMoving(pdata, len);
		break;
	case SERVER_FRAME_MOVING_DIFF:
		result = ServerFrameMovingDiff(pdata, len);
		break;
	case SERVER_MSG_TO_STUB:
		result = ServerMsgToStub(pdata, len);
		break;
	case STUB_MSG_TO_CLIENT:
		result = StubMsgToClient(pdata, len);
		break;
	default:
		CORE_TRACE("(GameReceiver::ProcessMessage)unknown message type");
		break;
	}

	if (msg_type < SERVER_MESSAGE_MAX)
	{
		m_MsgStats[msg_type].nMsgCount++;
		m_MsgStats[msg_type].nMsgBytes += (unsigned int)len;

		if (len > m_MsgStats[msg_type].nMaxSize)
		{
			m_MsgStats[msg_type].nMaxSize = (unsigned int)len;
			m_MsgStats[msg_type].nCPMaxSize = (unsigned int)cplen;
		}

#ifdef PERFORMANCE_DEBUG
		m_MsgStats[msg_type].dProcessTime += performance_time() - t1;	
#endif
	}

#ifdef __FULL_MSG_INFO_PERF__
	_msg.cplen = cplen;
	_msg.nlen = len;
	_msg.dperf = performance_time() - t1;
	core_file::fwrite(&_msg,sizeof(_msg),1,m_fp_info);
	core_file::fwrite(pdata,len,1,m_fp_info);
#endif

	return false;
}

const char* PropertyTableTypeName[] =
{
	"OUTER_TYPE_UNKNOWN",
	"OUTER_TYPE_BYTE",
    "OUTER_TYPE_WORD",
    "OUTER_TYPE_DWORD",
    "OUTER_TYPE_QWORD",
    "OUTER_TYPE_FLOAT",
    "OUTER_TYPE_DOUBLE",
    "OUTER_TYPE_STRING",
    "OUTER_TYPE_WIDESTR",
	"OUTER_TYPE_OBJECT",
	"OUTER_TYPE_MAX",
};

const char* MsgTableTypeName[] = 
{
	"SERVER_SERVER_INFO",
	"SERVER_SET_VERIFY",
	"SERVER_SET_ENCODE",
	"SERVER_ERROR_CODE",
	"SERVER_LOGIN_SUCCEED",
	"SERVER_WORLD_INFO",
	"SERVER_IDLE",
	"SERVER_QUEUE",
	"SERVER_TERMINATE",
	"SERVER_PROPERTY_TABLE",
	"SERVER_RECORD_TABLE",
	"SERVER_ENTRY_SCENE",
	"SERVER_EXIT_SCENE",
	"SERVER_ADD_OBJECT",
	"SERVER_REMOVE_OBJECT",
	"SERVER_SCENE_PROPERTY",
	"SERVER_OBJECT_PROPERTY",
	"SERVER_RECORD_ADDROW",
	"SERVER_RECORD_DELROW",
	"SERVER_RECORD_GRID",
	"SERVER_RECORD_CLEAR",
	"SERVER_CREATE_VIEW",
	"SERVER_DELETE_VIEW",
	"SERVER_VIEW_PROPERTY",
	"SERVER_VIEW_ADD",
	"SERVER_VIEW_REMOVE",
	"SERVER_SPEECH",
	"SERVER_SYSTEM_INFO",
	"SERVER_MENU",
	"SERVER_CLEAR_MENU",
	"SERVER_CUSTOM",
	"SERVER_LOCATION",
	"SERVER_MOVING",
	"SERVER_ALL_DEST",
	"SERVER_WARNING",
	"SERVER_FROM_GMCC",
	"SERVER_LINK_TO",
	"SERVER_UNLINK",
	"SERVER_LINK_MOVE",
	"SERVER_CP_CUSTOM",
	"SERVER_CP_ADD_OBJECT",
	"SERVER_CP_RECORD_ADDROW",
	"SERVER_CP_VIEW_ADD",
	"SERVER_VIEW_CHANGE",
	"SERVER_CP_ALL_DEST",
	"SERVER_ALL_PROP",
	"SERVER_CP_ALL_PROP",
	"SERVER_ADD_MORE_OBJECT",
	"SERVER_CP_ADD_MORE_OBJECT",
	"SERVER_REMOVE_MORE_OBJECT",
	"SERVER_CHARGE_VALIDSTRING",
	"SERVER_SECURITY_LOGIN",
	"SERVER_REP_OUTLINE_STAT",
	"SERVER_REP_DETAIL_STAT",
	"SERVER_REP_REMOTE_CTRL",
	"SERVER_MOVING_EX",
	"SERVER_ALL_DEST_EX",
	"SERVER_CP_ALL_DEST_EX",
	"SERVER_ENTRY_SCENE_EX",
	"SERVER_MEASURE_DELAY",
	"SERVER_FRAME_UPDATE",
	"SERVER_FRAME_MOVING",
	"SERVER_FRAME_MOVING_DIFF",
	"SERVER_MSG_TO_STUB",
};


bool GameReceiver::DumpMsgStat(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	unsigned int msg_count = 0;
	unsigned int msg_bytes = 0;
	
	for (size_t i = 0; i < SERVER_MESSAGE_MAX; ++i)
	{
		if (0 == m_MsgStats[i].nMsgCount)
		{
			continue;
		}

		core_file::fprintf(fp, 
			"message id:%s, count:%d, cpmax_size:%d, max_size:%d, bytes:%d, "
			"time:%f\r\n", 
			MsgTableTypeName[i], m_MsgStats[i].nMsgCount,
			m_MsgStats[i].nCPMaxSize, m_MsgStats[i].nMaxSize, 
			m_MsgStats[i].nMsgBytes, m_MsgStats[i].dProcessTime);
		msg_count += m_MsgStats[i].nMsgCount;
		msg_bytes += m_MsgStats[i].nMsgBytes;
	}

	core_file::fprintf(fp, "total count:%d, bytes:%d\r\n", msg_count, msg_bytes);
    
	// д�����Ա仯����ͳ����Ϣ
	for (size_t i = 0; i < m_PropertyTable.size(); i++)
	{
		if (0 == m_PropertyTable[i]->nCount)
		{
			continue;
		}
        
		int nType = m_PropertyTable[i]->nType;

		if (nType > OUTER_TYPE_OBJECT)
		{
			nType = OUTER_TYPE_MAX;
		}

		core_file::fprintf(fp,
			"messge property name:%s, type:%s, count:%d \r\n",
			m_PropertyTable[i]->strName.c_str(), PropertyTableTypeName[nType],
			m_PropertyTable[i]->nCount);
	}

	core_file::fprintf(fp, "total count:%d\r\n", m_PropertyTable.size());

	// д�����ͳ����Ϣ
	for(size_t i = 0; i< m_RecordTable.size();i++)
	{
 
		core_file::fprintf(fp,
			"recordtable name:%s , addrowcount:%d , addrowtime:%f, "
			"delrowcount:%d, delrowtime:%f, gridcount:%d , gridtime:%f, "
			"clearcount:%d, cleartime:%f  \r\n",
			m_RecordTable[i]->strName.c_str(),
			m_RecordTable[i]->nRecordAddRowCount,
			m_RecordTable[i]->dRecordAddRowTime,
			m_RecordTable[i]->nRecordDelRowCount,
			m_RecordTable[i]->dRecordDelRowTime,
			m_RecordTable[i]->nRecordGridCount,
			m_RecordTable[i]->dRecordGridTime,
			m_RecordTable[i]->nRecordClearCount,
			m_RecordTable[i]->dRecordClearTime);
	}
	
	core_file::fprintf(fp, "total count:%d\r\n", m_RecordTable.size());
	core_file::fclose(fp);
	
	return true;
}
