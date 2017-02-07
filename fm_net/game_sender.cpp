//--------------------------------------------------------------------
// 文件名:		game_sender.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "game_sender.h"
#include "game_sock.h"
#include "../public/outer_msg.h"
#include "../utils/store_archive.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../visual/i_solo_stub.h"
//#include "../network/member_msg.h"
// 拷贝宽字符串
inline void CopyWideStr(unsigned short* buf, size_t byte_size, const wchar_t* str)
{
#define FM_MIN(a,b)            (((a) < (b)) ? (a) : (b))
	if(sizeof(wchar_t) == 2)
	{
		CopyWideStr((wchar_t*)buf, byte_size, str);
	}
	else
	{
		size_t src_len = wcslen(str);
		size_t len = FM_MIN(byte_size/2, src_len);
		size_t i;
		for(i = 0; i< len; i++)
		{
			buf[i] = (unsigned short)str[i];
		}
		buf[i] = 0;
	}
#undef FM_MIN
}

/// \file game_sender.cpp
/// \brief 游戏消息发送器

/// entity: GameSender
/// \brief 游戏消息发送器实体
DECLARE_ENTITY(GameSender, NULL, IEntity);
/// property: object Stub
/// \brief 本地服务器对象
DECLARE_PROPERTY(PERSISTID, GameSender, Stub, GetStubID, SetStubID);
/*
/// method: bool GetVerify()
/// \brief 发送获得验证消息
DECLARE_METHOD_0(bool, GameSender, GetVerify);
/// method: bool Login(string account, string password)
/// \brief 发送登录消息
/// \param account 帐号
/// \param password 密码 
DECLARE_METHOD_2(bool, GameSender, Login, const char*, const char*);
/// method: bool LoginVerify(string account, string password, string verify)
/// \brief 发送带验证码的登录消息
/// \param account 帐号
/// \param password 密码 
/// \param verify 验证码
DECLARE_METHOD_3(bool, GameSender, LoginVerify, const char*, const char*, 
	const char*);
/// method: bool LoginByString(string account, string login_string)
/// \brief 使用登录串登录
/// \param account 帐号
/// \param login_string 登录串 
DECLARE_METHOD_2(bool, GameSender, LoginByString, const char*, const char*);
/// method: bool LoginByShield(string account, string password, string login_string)
/// \brief 使用密码保护盾登录
/// \param account 帐号
/// \param password 密码 
/// \param login_string 登录串 
DECLARE_METHOD_3(bool, GameSender, LoginByShield, const char*, const char*, 
	const char*);
/// method: bool CreateRole(int index, wstring role_name, [args])
/// \brief 发送创建角色消息
/// \param index 角色位置
/// \param role_name 角色名字
/// \param args 其他参数
DECLARE_METHOD_N(GameSender, CreateRole);
/// method: bool DeleteRole(wstring role_name)
/// \brief 发送删除角色消息
/// \param role_name 角色名字
DECLARE_METHOD_1(bool, GameSender, DeleteRole, const wchar_t*);
/// method: bool ReviveRole(wstring role_name)
/// \brief 发送恢复角色消息
/// \param role_name 角色名字
DECLARE_METHOD_1(bool, GameSender, ReviveRole, const wchar_t*);
/// method: bool ChooseRole(wstring role_name)
/// \brief 发送选择角色消息
/// \param role_name 角色名字
DECLARE_METHOD_1(bool, GameSender, ChooseRole, const wchar_t*);
/// method: bool ChooseRoleAndScene(wstring role_name, string scene_info)
/// \brief 发送选择角色消息并指定登录场景（测试用）
/// \param role_name 角色名字
/// \param scene_info 场景信息
DECLARE_METHOD_2(bool, GameSender, ChooseRoleAndScene, const wchar_t*,
	const char*);
/// method: bool ClientReady()
/// \brief 发送客户端就绪消息
DECLARE_METHOD_0(bool, GameSender, ClientReady);
/// method: bool GetWorldInfo(int type)
/// \brief 发送获得游戏世界信息消息
/// \param type 信息类型
DECLARE_METHOD_1(bool, GameSender, GetWorldInfo, int);
/// method: bool GetWorldInfo2(int type, wstring info)
/// \brief 发送获得游戏世界信息消息
/// \param type 信息类型
/// \param info 附加信息
DECLARE_METHOD_2(bool, GameSender, GetWorldInfo2, int, const wchar_t*);
/// method: bool Speech(wstring info)
/// \brief 发送普通聊天消息
/// \param info 聊天内容
DECLARE_METHOD_1(bool, GameSender, Speech, const wchar_t*);
/// method: bool Custom([args])
/// \brief 发送自定义消息
/// \param args 参数集合
DECLARE_METHOD_N(GameSender, Custom);
/// method: bool Select(string object_ident, int func_id)
/// \brief 发送选择对象消息
/// \param object_ident 游戏对象标识
/// \param func_id 功能号
DECLARE_METHOD_2(bool, GameSender, Select, const char*, int);
/// method: bool RequestMove(int mode, int arg_num, float args[], string info)
/// \brief 发送请求移动消息
/// \param mode 移动模式
/// \param arg_num 浮点参数数量
/// \param args[] 浮点参数数组
/// \param info 附加信息
DECLARE_METHOD_N(GameSender, RequestMove);

/// method: bool SendAny(...)
/// \brief 发送任意数据消息
DECLARE_METHOD_N(GameSender, SendAny);*/

// GameSender

GameSender::GameSender()
{
	m_pGameSock = NULL;
	m_pSoloStub = NULL;
}

GameSender::~GameSender()
{
}

bool GameSender::Init(const IVarList& args)
{
	m_pSoloStub = dynamic_cast<ISoloStub*>(GetCore()->LookupEntity("SoloStub"));
	return true;
}

bool GameSender::Shut()
{
	return true;
}

void GameSender::SetGameSock(IGameSock* value)
{
	Assert(value != NULL);
	
	m_pGameSock = (GameSock*)value;
}


void GameSender::SetStubID(const PERSISTID& id)
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

PERSISTID GameSender::GetStubID() const
{
	if (NULL == m_pSoloStub)
	{
		return PERSISTID();
	}

	return m_pSoloStub->GetID();
}


bool GameSender::GetVerify()
{
	client_msg_get_verify_t out;

	out.nMsgType = CLIENT_GET_VERIFY;

	return m_pGameSock->SendData(&out, sizeof(out));
}

inline IEntity* GetGlobalEntity(const char* name)
{
	IVar& v = g_pCore->GetGlobalValue(name);

	if (v.GetType() != VTYPE_OBJECT)
	{
		return NULL;
	}

	return g_pCore->GetEntity(v.ObjectVal());
}

static bool send_login(GameSock* pGameSock, char* buffer, size_t buffer_size, 
	const char* account, const char* password, const char* login_string, 
	int login_type)
{
	char* end = buffer + buffer_size;
	char* p = buffer + sizeof(client_msg_login_t);

	if ((p + sizeof(unsigned int)) > end)
	{
		CORE_TRACE("(send_login)account size no buffer");
		return false;
	}

	unsigned int account_len = (unsigned int)strlen(account);
	unsigned int account_size = account_len + 1;

	account_size = (account_size + 15) / 16 * 16;

	*(unsigned int*)p = account_size;
	p += sizeof(unsigned int);

	if ((p + account_size) > end)
	{
		CORE_TRACE("(send_login)account no buffer");
		return false;
	}

	memset(p, 0, account_size);
	memcpy(p, account, strlen(account));
	p += account_size;

	if ((p + sizeof(unsigned int)) > end)
	{
		CORE_TRACE("(send_login)password size no buffer");
		return false;
	}

	unsigned int password_len = (unsigned int)strlen(password);
	unsigned int password_size = password_len + 1;

	password_size = (password_size + 15) / 16 * 16;

	*(unsigned int*)p = password_size;
	p += sizeof(unsigned int);

	if ((p + password_size) > end)
	{
		CORE_TRACE("(send_login)password no buffer");
		return false;
	}

	memset(p, 0, password_size);
	memcpy(p, password, password_len);
	p += password_size;

	if ((p + sizeof(unsigned int)) > end)
	{
		CORE_TRACE("(send_login)login string size no buffer");
		return false;
	}

	unsigned int loginstr_len = (unsigned int)strlen(login_string);
	unsigned int loginstr_size = loginstr_len + 1;

	loginstr_size = (loginstr_size + 15) / 16 * 16;

	*(unsigned int*)p = loginstr_size;
	p += sizeof(unsigned int);

	if ((p + loginstr_size) > end)
	{
		CORE_TRACE("(send_login)login string no buffer");
		return false;
	}

	memset(p, 0, loginstr_size);
	memcpy(p, login_string, loginstr_len);
	p += loginstr_size;

	if ((p + sizeof(int)) > end)
	{
		CORE_TRACE("(send_login)login type no buffer");
		return false;
	}

	*(int*)p = login_type;
	p += sizeof(int);

	return pGameSock->SendData(buffer, p - buffer);
}

bool GameSender::Login(const char* account, const char* pswd)
{
	Assert(account != NULL);
	Assert(pswd != NULL);

	char buf[512];
	client_msg_login_t* pout = (client_msg_login_t*)buf;

	memset(pout, 0, sizeof(client_msg_login_t));
	pout->nMsgType = CLIENT_LOGIN;
	pout->nVersion = OUTER_PROTOCOL_VERSION;

	return send_login(m_pGameSock, buf, sizeof(buf), account, pswd, "", 1);
}

bool GameSender::LoginVerify(const char* account, const char* pswd, 
	const char* verify)
{
	Assert(account != NULL);
	Assert(pswd != NULL);
	Assert(verify != NULL);
	
	char buf[512];
	client_msg_login_t* pout = (client_msg_login_t*)buf;

	memset(pout, 0, sizeof(client_msg_login_t));
	pout->nMsgType = CLIENT_LOGIN;
	pout->nVersion = OUTER_PROTOCOL_VERSION;
	CopyString(pout->strVerify, sizeof(pout->strVerify), verify);

	return send_login(m_pGameSock, buf, sizeof(buf), account, pswd, "", 1);
}

bool GameSender::LoginByString(const char* account, const char* login_string)
{
	Assert(account != NULL);
	Assert(login_string != NULL);

	char buf[512];
	client_msg_login_t* pout = (client_msg_login_t*)buf;

	memset(pout, 0, sizeof(client_msg_login_t));
	pout->nMsgType = CLIENT_LOGIN;
	pout->nVersion = OUTER_PROTOCOL_VERSION;

	return send_login(m_pGameSock, buf, sizeof(buf), account, "", 
		login_string, 2);
}

bool GameSender::LoginByShield(const char* account, const char* pswd, 
	const char* login_string)
{
	Assert(account != NULL);
	Assert(login_string != NULL);

	char buf[512];
	client_msg_login_t* pout = (client_msg_login_t*)buf;

	memset(pout, 0, sizeof(client_msg_login_t));
	pout->nMsgType = CLIENT_LOGIN;
	pout->nVersion = OUTER_PROTOCOL_VERSION;

	return send_login(m_pGameSock, buf, sizeof(buf), account, pswd, 
		login_string, 4);
}

bool GameSender::AddMsgVarList(CStoreArchive& ar, const IVarList& args, 
	size_t beg, size_t end)
{
	try
	{
		for (size_t i = beg; i < end; ++i)
		{
			switch (args.GetType(i))
			{
			case VTYPE_INT:
				ar.WriteByte(VTYPE_INT);
				ar.WriteInt(args.IntVal(i));
				break;
			case VTYPE_INT64:
				ar.WriteByte(VTYPE_INT64);
				ar.WriteInt64(args.Int64Val(i));
				break;
			case VTYPE_FLOAT:
				ar.WriteByte(VTYPE_FLOAT);
				ar.WriteFloat(args.FloatVal(i));
				break;
			case VTYPE_DOUBLE:
				ar.WriteByte(VTYPE_DOUBLE);
				ar.WriteDouble(args.DoubleVal(i));
				break;
			case VTYPE_STRING:
				ar.WriteByte(VTYPE_STRING);
				ar.WriteString(args.StringVal(i));
				break;
			case VTYPE_WIDESTR:
				ar.WriteByte(VTYPE_WIDESTR);
				ar.WriteWideStr(args.WideStrVal(i));
				break;
			case VTYPE_OBJECT:
				ar.WriteByte(VTYPE_OBJECT);
				ar.WriteObject(args.ObjectVal(i));
				break;
			default:
				Assert(0);
				break;
			}
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameSender::AddMsgVarList)make message failed");
		CORE_TRACE(e.GetMsg());
		return false;
	}
	
	return true;
}

void GameSender::CreateRole(const IVarList& args, IVarList& ret)
{
	if ((args.GetCount() < 2) || (args.GetType(1) != VTYPE_WIDESTR))
	{
		CORE_TRACE("(GameSender::CreateRole)arguments error");
		ret << false;
		return;
	}

	char buf[256];
	client_msg_create_role_t* pout = (client_msg_create_role_t*)buf;

	pout->nMsgType = CLIENT_CREATE_ROLE;
	pout->nIndex = args.IntVal(0);
	CopyWideStr(pout->wsName, sizeof(pout->wsName), 
		args.WideStrVal(1));
	pout->nArgNum = (unsigned short)args.GetCount() - 2;

	CStoreArchive arStore(buf, sizeof(buf));

	arStore.Seek(sizeof(client_msg_create_role_t));
	
	if (!AddMsgVarList(arStore, args, 2, args.GetCount()))
	{
		CORE_TRACE("(GameSender::CreateRole)add parameters error");
		ret << false;
		return;
	}
	
	ret << m_pGameSock->SendData(arStore.GetData(), arStore.GetLength());
}

bool GameSender::DeleteRole(const wchar_t* name)
{
	Assert(name != NULL);
	
	client_msg_delete_role_t out;

	out.nMsgType = CLIENT_DELETE_ROLE;
	CopyWideStr(out.wsName, sizeof(out.wsName), name);

	return m_pGameSock->SendData(&out, sizeof(out));
}

bool GameSender::ReviveRole(const wchar_t* name)
{
	Assert(name != NULL);

	client_msg_revive_role_t out;

	out.nMsgType = CLIENT_REVIVE_ROLE;
	CopyWideStr(out.wsName, sizeof(out.wsName), name);

	return m_pGameSock->SendData(&out, sizeof(out));
}

bool GameSender::ChooseRole(const wchar_t* name)
{
	Assert(name != NULL);
	
	client_msg_choose_role_t out;

	memset(&out, 0, sizeof(out));
	out.nMsgType = CLIENT_CHOOSE_ROLE;
	CopyWideStr(out.wsName, sizeof(out.wsName), name);

	return m_pGameSock->SendData(&out, sizeof(out));
}

bool GameSender::ChooseRoleAndScene(const wchar_t* name, 
	const char* scene_info)
{
	Assert(name != NULL);
	Assert(scene_info != NULL);

	char buf[256];
	client_msg_choose_role_t* pout = (client_msg_choose_role_t*)buf;
	size_t msg_size = sizeof(client_msg_choose_role_t) + strlen(scene_info);

	if (msg_size > sizeof(buf))
	{
		CORE_TRACE("(GameSender::ChooseRoleAndScene)scene_info too long");
		return false;
	}

	memset(pout, 0, msg_size);
	pout->nMsgType = CLIENT_CHOOSE_ROLE;
	CopyWideStr(pout->wsName, sizeof(pout->wsName), name);
	CopyString(pout->strInfo, 
		sizeof(buf) - sizeof(client_msg_choose_role_t) + 1, scene_info);

	return m_pGameSock->SendData(pout, msg_size);
}

bool GameSender::ClientReady()
{
	client_msg_ready_t out;

	out.nMsgType = CLIENT_READY;

	if(m_pSoloStub && m_pSoloStub->IsRunning())
		return m_pSoloStub->PutMessage(out.nMsgType,&out,sizeof(out),CLIENT_MSG);
	else
		return m_pGameSock->SendData(&out, sizeof(out));
}

bool GameSender::GetWorldInfo(int type)
{
	client_msg_world_info_t out;

	out.nMsgType = CLIENT_WORLD_INFO;
	out.nInfoType = type;

	return m_pGameSock->SendData(&out, sizeof(out));
}

bool GameSender::GetWorldInfo2(int type, const wchar_t* info)
{
	Assert(info != NULL);

	if (wcslen(info) > 128)
	{
		CORE_TRACE("(GameSender::GetWorldInfo2)info too long");
		return false;
	}

	char buf[1024];
	client_msg_world_info_t* pout = (client_msg_world_info_t*)buf;
	wchar_t* dest_info = (wchar_t*)(buf + sizeof(client_msg_world_info_t));

	pout->nMsgType = CLIENT_WORLD_INFO;
	pout->nInfoType = type;
	CopyWideStr(dest_info, sizeof(buf) - sizeof(client_msg_world_info_t),
		info);

	return m_pGameSock->SendData(pout, sizeof(client_msg_world_info_t)
		+ (wcslen(dest_info) + 1) * sizeof(wchar_t));
}

bool GameSender::Speech(const wchar_t* content)
{
	Assert(content != NULL);

	char buf[256];
	client_msg_speech_t* pout = (client_msg_speech_t*)buf;

	pout->nMsgType = CLIENT_SPEECH;

	const size_t SIZE1 = wcslen(content) * sizeof(wchar_t);

	if (SIZE1 > (sizeof(buf) - sizeof(client_msg_speech_t)))
	{
		CORE_TRACE("(GameSender::Speech)content too long");
		return false;
	}

	memcpy(pout->wsContent, content, SIZE1 + sizeof(wchar_t));
	
	return m_pGameSock->SendData(pout, SIZE1 + sizeof(client_msg_speech_t));
}

void GameSender::Custom(const IVarList& args, IVarList& ret)
{
	char buf[256];
	client_msg_custom_t* pout = (client_msg_custom_t*)buf;
	
	memset(pout, 0, sizeof(client_msg_custom_t));
	pout->nMsgType = CLIENT_CUSTOM;
	pout->nArgNum = (unsigned short)args.GetCount();
	
	CStoreArchive arStore(buf, sizeof(buf));

	arStore.Seek(sizeof(client_msg_custom_t));
	
	if (!AddMsgVarList(arStore, args, 0, args.GetCount()))
	{
		CORE_TRACE("(GameSender::Custom)add parameters error");
		ret << false;
		return;
	}

	if(m_pSoloStub && m_pSoloStub->IsRunning())
		ret << m_pSoloStub->PutMessage(pout->nMsgType,arStore.GetData(), arStore.GetLength(),CLIENT_MSG);
	else
		ret << m_pGameSock->SendData(arStore.GetData(), arStore.GetLength());
}

bool GameSender::Select(const char* object_ident, int func_id)
{
	PERSISTID obj_id = StringToObject(object_ident);
	client_msg_select_t out;

	memset(&out, 0, sizeof(out));
	out.nMsgType = CLIENT_SELECT;
	out.ObjectId.nIdent = obj_id.nIdent;
	out.ObjectId.nSerial = obj_id.nSerial;
	out.nFunctionId = func_id;

	if(m_pSoloStub && m_pSoloStub->IsRunning())
		m_pSoloStub->PutMessage(out.nMsgType,&out, sizeof(out),CLIENT_MSG);
	else
		m_pGameSock->SendData(&out, sizeof(out));

	return true;
}

void GameSender::RequestMove(const IVarList& args, IVarList& ret)
{
	int mode = args.IntVal(0);
	int arg_num = args.IntVal(1);

	if (size_t(arg_num) > 256)
	{
		CORE_TRACE("(GameSender::RequestMove)more arguments");
		ret << false;
		return;
	}

	if (args.GetCount() < size_t(arg_num + 2))
	{
		CORE_TRACE("(GameSender::RequestMove)too few arguments");
		ret << false;
		return;
	}
	
	char buf[2048];
	client_msg_request_move_t* pout = (client_msg_request_move_t*)buf;

	pout->nMsgType = CLIENT_REQUEST_MOVE;
	pout->nMode = mode;
	pout->nArgNum = (unsigned short)arg_num;

	float* p = (float*)((char*)buf + sizeof(client_msg_request_move_t));

	for (int i = 0; i < arg_num; ++i)
	{
		*p++ = args.FloatVal(2 + i);
	}

	// 附加字符串信息
	size_t info_size = 0;

	if (args.GetCount() > size_t(arg_num + 2))
	{
		const char* info = args.StringVal(arg_num + 2);
		
		info_size = strlen(info) + 1;
		memcpy(p, info, info_size);		
	}

	if(m_pSoloStub && m_pSoloStub->IsRunning())
	{
		if (!m_pSoloStub->PutMessage(pout->nMsgType,pout, sizeof(client_msg_request_move_t)
			+ sizeof(len_t) * arg_num + info_size),CLIENT_MSG)
		{
			ret << false;
			return;
		}
	}
	else
	{
		if (!m_pGameSock->SendData(pout, sizeof(client_msg_request_move_t)
			+ sizeof(len_t) * arg_num + info_size))
		{
			ret << false;
			return;
		}
	}

	ret << true;
}

bool GameSender::InnerSendAny(const IVarList& args)
{
	size_t k = 0;
	size_t data_num = args.IntVal(k++);
	
	if (data_num > 256)
	{
		return false;
	}
	
	char buf[2048];
	char* p = buf;
	char* end = buf + sizeof(buf);
	
	for (size_t i = 0; i < data_num; ++i)
	{
		const char* data_type = args.StringVal(k++);
		
		if (stricmp(data_type, "byte") == 0)
		{
			unsigned char val = (unsigned char)(args.IntVal(k++));
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}

			memcpy(p, &val, sizeof(unsigned char));
			//*(unsigned char*)p = val;
			p += sizeof(val);
		}
		else if (stricmp(data_type, "word") == 0)
		{
			unsigned short val = (unsigned short)(args.IntVal(k++));
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}
			
			memcpy(p, &val, sizeof(unsigned short));
			//*(unsigned short*)p = val;
			p += sizeof(val);
		}
		else if (stricmp(data_type, "dword") == 0)
		{
			unsigned int val = args.IntVal(k++);
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}
			
			memcpy(p, &val, sizeof(unsigned int));
			//*(unsigned int*)p = val;
			p += sizeof(val);
		}
		else if (stricmp(data_type, "qword") == 0)
		{
			uint64_t val = args.Int64Val(k++);
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}
			
			memcpy(p, &val, sizeof(uint64_t));
			//*(uint64_t*)p = val;
			p += sizeof(val);
		}
		else if (stricmp(data_type, "float") == 0)
		{
			float val = args.FloatVal(k++);
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}
			memcpy(p, &val, sizeof(float));
			//*(float*)p = val;
			p += sizeof(val);
		}
		else if (stricmp(data_type, "double") == 0)
		{
			double val = args.DoubleVal(k++);
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}
			memcpy(p, &val, sizeof(double));
			//*(double*)p = val;
			p += sizeof(val);
		}
		else if (stricmp(data_type, "string") == 0)
		{
			const char* str = args.StringVal(k++);
			size_t len = strlen(str) + 1;

			if ((p + len) > end)
			{
				return false;
			}

			memcpy(p, str, len);
			p += len;
		}
		else if (stricmp(data_type, "len_string") == 0)
		{
			size_t len = args.IntVal(k++);
			const char* str = args.StringVal(k++);

			if ((p + len) > end)
			{
				return false;
			}

			size_t s_len = strlen(str);

			if (s_len > len)
			{
				s_len = len;
			}
			
			memset(p, 0, len);
			memcpy(p, str, s_len);
			p += len;
		}
		else if (stricmp(data_type, "widestr") == 0)
		{
			const wchar_t* str = args.WideStrVal(k++);
			size_t len = (wcslen(str) + 1) * sizeof(wchar_t);
			
			if ((p + len) > end)
			{
				return false;
			}
			
			memcpy(p, str, len);
			p += len;
		}
		else if (stricmp(data_type, "len_widestr") == 0)
		{
			size_t len = args.IntVal(k++) * sizeof(wchar_t);
			const wchar_t* str = args.WideStrVal(k++);
			
			if ((p + len) > end)
			{
				return false;
			}
			
			size_t s_len = wcslen(str) * sizeof(wchar_t);
			
			if (s_len > len)
			{
				s_len = len;
			}
			
			memset(p, 0, len);
			memcpy(p, str, s_len);
			p += len;
		}
		else if (stricmp(data_type, "object") == 0)
		{
			PERSISTID val = args.ObjectVal(k++);
			
			if ((p + sizeof(val)) > end)
			{
				return false;
			}
			
			*(PERSISTID*)p = val;
			p += sizeof(val);
		}
		else
		{
			return false;
		}
	}
	
	return m_pGameSock->SendData(buf, p - buf);
}

void GameSender::SendAny(const IVarList& args, IVarList& result)
{
	if(m_pSoloStub && m_pSoloStub->IsRunning())
		return;

	result << InnerSendAny(args);
}

void GameSender::MoveCustom(const IVarList& args, IVarList& ret)
{
	char buf[256];
	client_msg_custom_t* pout = (client_msg_custom_t*)buf;

	memset(pout, 0, sizeof(client_msg_custom_t));
	pout->nMsgType = CLIENT_CUSTOM;
	pout->nArgNum = (unsigned short)args.GetCount();

	if (args.IntVal(0) != 15)
	{
		// 只能15号消息类型(移动消息)
		ret << false;
		return;
	}

	CStoreArchive arStore(buf, sizeof(buf));

	arStore.Seek(sizeof(client_msg_custom_t));

	if (!AddMsgVarList(arStore, args, 0, args.GetCount()))
	{
		CORE_TRACE("(GameSender::Custom)add parameters error");
		ret << false;
		return;
	}

	if(m_pSoloStub && m_pSoloStub->IsRunning())
		ret << m_pSoloStub->PutMessage(pout->nMsgType,arStore.GetData(), arStore.GetLength(),CLIENT_MSG);
	else
		ret << m_pGameSock->SendData(arStore.GetData(), arStore.GetLength());
}

// 发送消息到stub服
void GameSender::SendMsgToStub(const IVarList& args, IVarList& ret)
{
	if(args.GetCount()<=1 || args.GetType(0) != VTYPE_INT)
	{
		// 第一个只能是整形
		ret << false;
		return;
	}

	char buf[256];

	CStoreArchive arStore(buf, sizeof(buf));

	if (!AddMsgVarList(arStore, args, 1, args.GetCount()))
	{
		CORE_TRACE("(GameSender::SendMsgToStub)add parameters error");
		ret << false;
		return;
	}

	if(m_pSoloStub)
		ret << m_pSoloStub->PutMessage(args.IntVal(0),arStore.GetData(), arStore.GetLength(),CLIENT_MSG);
	else
		ret <<false;

}

void GameSender::SendMsgToMember(const IVarList& args, IVarList& ret)
{
	char buf[256];
	client_msg_custom_t* pout = (client_msg_custom_t*)buf;

	memset(pout, 0, sizeof(client_msg_custom_t));
	pout->nMsgType = CLIENT_CUSTOM;
	pout->nArgNum = (unsigned short)args.GetCount();

	CStoreArchive arStore(buf, sizeof(buf));

	arStore.Seek(sizeof(client_msg_custom_t));

	if (!AddMsgVarList(arStore, args, 0, args.GetCount()))
	{
		CORE_TRACE("(GameSender::Custom)add parameters error");
		ret << false;
		return;
	}
	ret << m_pGameSock->SendData(arStore.GetData(), arStore.GetLength());
}