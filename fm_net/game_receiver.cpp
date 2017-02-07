//--------------------------------------------------------------------
// 文件名:		game_receiver.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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
/// \brief 游戏消息接收器

/// entity: IGameMsgHandler
/// \brief 游戏消息处理实体基类
DECLARE_ENTITY(IGameMsgHandler, NULL, IEntity);

/// entity: GameReceiver
/// \brief 游戏消息接收器实体
DECLARE_ENTITY(GameReceiver, NULL, IEntity);

/// property: object Client
/// \brief 游戏客户端对象
DECLARE_PROPERTY(PERSISTID, GameReceiver, Client, GetClientID, SetClientID);
/// property: object Stub
/// \brief 本地服务器对象
DECLARE_PROPERTY(PERSISTID, GameReceiver, Stub, GetStubID, SetStubID);
/// property: object GameMsgHandler
/// \brief 游戏消息处理对象
DECLARE_PROPERTY(PERSISTID, GameReceiver, GameMsgHandler, GetGameMsgHandlerID, 
	SetGameMsgHanderID);
/// property: object CustomHander
/// \brief 自定义消息处理器对象
DECLARE_PROPERTY(PERSISTID, GameReceiver, CustomHandler, 
	GetCustomHandlerID, SetCustomHandlerID);
/// property: int MaxCustomArguments
/// \brief 自定义消息的最大参数数量
DECLARE_PROPERTY(int, GameReceiver, MaxCustomArguments, 
	GetMaxCustomArguments, SetMaxCustomArguments);

/*
/// method: int GetRoleCount()
/// \brief 获得角色数量
DECLARE_METHOD_0(int, GameReceiver, GetRoleCount);
/// method: int GetRoleInfoCount()
/// \brief 获得每个角色的参数数量
DECLARE_METHOD_0(int, GameReceiver, GetRoleInfoCount);
/// method: int GetRoleIndex(int index)
/// \brief 获得指定角色的位置索引
/// \param index 角色索引
DECLARE_METHOD_1(int, GameReceiver, GetRoleIndex, int);
/// method: int GetRoleSysFlags(int index)
/// \brief 获得指定角色的系统标志
/// \param index 角色索引
DECLARE_METHOD_1(int, GameReceiver, GetRoleSysFlags, int);
/// method: wstring GetRoleName(int index)
/// \brief 获得指定角色的名字
/// \param index 角色索引
DECLARE_METHOD_1(const wchar_t*, GameReceiver, GetRoleName, int);
/// method: wstring GetRolePara(int index)
/// \brief 获得指定角色的参数
/// \param index 角色索引
DECLARE_METHOD_1(const wchar_t*, GameReceiver, GetRolePara, int);
/// method: int GetRoleDeleted(int index)
/// \brief 获得指定角色是否被删除
/// \param index 角色索引
DECLARE_METHOD_1(int, GameReceiver, GetRoleDeleted, int);
/// method: double GetRoleDeleteTime(int index)
/// \brief 获得指定角色的删除时间
/// \param index 角色索引
DECLARE_METHOD_1(double, GameReceiver, GetRoleDeleteTime, int);
/// method: [args] GetRoleInfo(int index)
/// \brief 获得指定角色的所有参数
/// \param index 角色位置
DECLARE_METHOD_N(GameReceiver, GetRoleInfo);
/// method: int GetMenuCount()
/// \brief 获得菜单数量
DECLARE_METHOD_0(int, GameReceiver, GetMenuCount);
/// method: int GetMenuType(int index)
/// \brief 获得菜单类型
/// \param index 菜单索引
DECLARE_METHOD_1(int, GameReceiver, GetMenuType, int);
/// method: int GetMenuMark(int index)
/// \brief 获得菜单标记
/// \param index 菜单索引
DECLARE_METHOD_1(int, GameReceiver, GetMenuMark, int);
/// method: wstring GetMenuContent(int index)
/// \brief 获得菜单内容
/// \param index 菜单索引
DECLARE_METHOD_1(const wchar_t*, GameReceiver, GetMenuContent, int);

/// method: bool ClearAll()
/// \brief 清除所有数据
DECLARE_METHOD_0(bool, GameReceiver, ClearAll);

/// method: bool DumpMsgStat(string file_name)
/// \brief 导出消息统计数据到文件
/// \param file_name 输出文件名
DECLARE_METHOD_1(bool, GameReceiver, DumpMsgStat, const char*);

/// event: int on_set_verify(object self, int width, int height)
/// \brief 收到验证码图片消息
/// \param width 图片宽度
/// \param height 图片高度
DECLARE_EVENT(on_set_verify);
/// event: int on_set_encode(object self)
/// \brief 收到设置加密消息 
DECLARE_EVENT(on_set_encode);
/// event: int on_error_code(object self, int error_code)
/// \brief 收到错误码消息
/// \param error_code 错误码
DECLARE_EVENT(on_error_code);
/// event: int on_login_succeed(object self, bool is_free, int points, 
///		int year, int month, int day, int hour, int minute, int second, int roles);
/// \brief 收到登录成功消息
/// \param is_free 是否免费
/// \param points 帐号的点数
/// \param year,month,day,hour,minute,second 包月截止时间
/// \param roles 角色数量
DECLARE_EVENT(on_login_succeed);
/// event: int on_terminate(object self, int reason)
/// \brief 收到强制关闭消息
/// \param reason 原因
DECLARE_EVENT(on_terminate);
/// event: int on_entry_scene(object self)
/// \brief 收到进入场景的消息
DECLARE_EVENT(on_entry_scene);
/// event: int on_entry_scene_ex(object self)
/// \brief 收到进入兼容帧同步场景的消息
DECLARE_EVENT(on_entry_scene_ex);
/// event: int on_exit_scene(object self)
/// \brief 收到退出场景的消息
DECLARE_EVENT(on_exit_scene);
/// event: int on_add_object(object self, object obj, int attrs)
/// \brief 收到添加对象消息
/// \param obj 对象号
/// \param attrs 属性数量
DECLARE_EVENT(on_add_object);
/// event: int on_remove_object(object self, object obj)
/// \brief 收到移除对象消息
/// \param obj 对象号
DECLARE_EVENT(on_remove_object);
/// event: int on_property_table(object self, int count)
/// \brief 收到属性表消息
/// \param count 可视属性数量
DECLARE_EVENT(on_property_table);
/// event: int on_state_table(object self, int count)
/// \brief 收到状态表消息
/// \param count 状态数量
DECLARE_EVENT(on_state_table);
/// event: int on_record_table(object self, int count)
/// \brief 收到表结构信息消息
/// \param count 可视表数量
DECLARE_EVENT(on_record_table);
/// event: int on_scene_property(object self, int count)
/// \brief 收到场景属性消息
/// \param count 属性数量 
DECLARE_EVENT(on_scene_property);
/// event: int on_object_property(object self, object obj, int count)
/// \brief 收到对象属性消息
/// \param obj 对象号
/// \param count 属性数量 
DECLARE_EVENT(on_object_property);
/// event: int on_speech(object self, object obj, wstring content)
/// \brief 收到普通聊天消息
/// \param obj 对象号
/// \param content 聊天内容
DECLARE_EVENT(on_speech);
/// event: int on_system_info(object self, int type, wstring info)
/// \brief 收到系统信息
/// \param type 信息类型
/// \param info 信息内容
DECLARE_EVENT(on_system_info);
/// event: int on_menu(object self, object obj, object async_id, int count)
/// \brief 收到菜单消息
/// \param obj 对象号
/// \param async_id 异步ID
/// \param count 菜单项数量
DECLARE_EVENT(on_menu);
/// event: int on_clear_menu(object self)
/// \brief 收到清除菜单消息
DECLARE_EVENT(on_clear_menu);
/// event: int on_custom(object self, int arg_num, [args])
/// \brief 收到自定义消息
/// \param arg_num 参数数量
/// \param args 消息参数
DECLARE_EVENT(on_custom);
/// event: int on_create_view(object self, int view_id, int capacity)
/// \brief 收到创建视窗消息
/// \param view_id 视窗编号
/// \param capacity 容量
DECLARE_EVENT(on_create_view);
/// event: int on_delete_view(object self, int view_id)
/// \brief 收到删除视窗消息
/// \param view_id 视窗编号
DECLARE_EVENT(on_delete_view);
/// event: int on_view_property(object self, int view_id, int count)
/// \brief 收到视窗属性消息
/// \param view_id 视窗编号
/// \param count 属性数量
DECLARE_EVENT(on_view_property);
/// event: int on_view_add(object self, int view_id, int obj_index)
/// \brief 收到视窗中添加对象消息
/// \param view_id 视窗编号
/// \param obj_index 对象索引（从1开始） 
DECLARE_EVENT(on_view_add);
/// event: int on_view_remove(object self, int view_id, int obj_index)
/// \brief 收到视窗中删除对象消息
/// \param view_id 视窗编号
/// \param obj_index 对象索引（从1开始） 
DECLARE_EVENT(on_view_remove);
/// event: int on_view_object_property(object self, 
///		int view_id, int obj_index, int count)
/// \brief 收到视窗中对象属性消息
/// \param view_id 视窗编号
/// \param obj_index 对象索引（从1开始） 
/// \param count 属性数量
DECLARE_EVENT(on_view_object_property);

/// event: int on_view_object_property_change(object self, 
///		int view_id, int obj_index, string name)
/// \brief 收到视窗中对象属性消息
/// \param view_id 视窗编号
/// \param obj_index 对象索引（从1开始） 
/// \param count 属性数量
DECLARE_EVENT(on_view_object_property_change);

/// event: int on_record_add_row(object self, string name, int row, int row_num)
/// \brief 收到表中添加行消息
/// \param name 表名
/// \param row 插入的行号
/// \param row_num 行数量
DECLARE_EVENT(on_record_add_row);
/// event: int on_record_remove_row(object self, string name, int row)
/// \brief 收到表中删除行消息
/// \param name 表名
/// \param row 行号
DECLARE_EVENT(on_record_remove_row);
/// event: int on_recrod_grid(object self, string name, int count)
/// \brief 收到表格中数据改变消息
/// \param name 表名
/// \param count 格子数量
DECLARE_EVENT(on_record_grid);
/// event: int on_record_single_grid(object self, string name, int row, int clomn)
/// \brief 收到表格中数据改变消息
/// \param name 表名
/// \param row 格子行
/// \param clomn 格子列
DECLARE_EVENT(on_record_single_grid);
/// event: int on_record_clear(object self, string name)
/// \brief 收到清除表中所有数据消息
/// \param name 表名
DECLARE_EVENT(on_record_clear);
/// event: int on_location(object self)
/// \brief 收到对象位置信息 
DECLARE_EVENT(on_location);
/// event: int on_moving(object self)
/// \brief 收到对象移动目标信息 
DECLARE_EVENT(on_moving);
/// event: int on_all_dest(object self, int count)
/// \brief 收到视野内所有对象的目标点消息
/// \param count 对象数量
DECLARE_EVENT(on_all_dest);
/// event: int on_idle(object self)
/// \brief 收到空闲消息
DECLARE_EVENT(on_idle);
/// event: int on_warning(object self, int type, wstring info)
/// \brief 收到警告消息
/// \param type 信息类型
/// \param info 信息内容
DECLARE_EVENT(on_warning);
/// event: int on_from_gmcc(object self, int id, wstring name, wstring info)
/// \brief 收到来自GM的信息
/// \param id GM编号
/// \param name GM名称
/// \param info 信息内容
DECLARE_EVENT(on_from_gmcc);
/// event: int on_server_info(object self, int count)
/// \brief 收到服务器列表消息
/// \param count 服务器数量
DECLARE_EVENT(on_server_info);
/// event: int on_queue(object self, int queue, int position)
/// \brief 收到排队消息
/// \param queue 队列编号（为0）
/// \param position 队列中的位置
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

//先把数据直接读取出来
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

// 防止占用过多的stack空间
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
    
	// 写入属性变化次数统计信息
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

	// 写入表监控统计信息
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
