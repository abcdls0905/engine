

#include "form_login.h"

#include "../fm_gui/gui.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "../fm_gui/edit.h"
#include "../fm_gui/text.h"
#include "../fm_gui/group_box.h"
#include "../fm_gui/check_button.h"

#include "../visual/i_event_manager.h"
#include "../visual/i_scene.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_game_sock.h"

#include "../public/converts.h"
#include "../public/i_ini_file.h"

#include "../../fm_gui/string_util.h"

#include "../helper.h"
#include "../global.h"


#include "../interface/global_prop_define.h"
#include "../interface/iresourcemanager.h"


#include "../stagemanager/stageutils.h"

#include "../util/gui_utils.h"
#include "../util/logic_util.h"

#define  SHOW_ACTIVE_ACCOUNT 0

DECLARE_ENTITY(FormLogin, 0, IEntity);

FormLogin* FormLogin::Instance()
{
	FormLogin* pInstance = (FormLogin*)CHelper::GetGlobalEntity("FormLogin");
	if (pInstance == NULL)
	{
		pInstance = (FormLogin*)CHelper::CreateEntity("FormLogin");
	}
	return pInstance;
}

//
FormLogin::FormLogin()
{	
	m_ServerPort = 2001;
}

//
FormLogin::~FormLogin()
{
}

//
void FormLogin::OnCreate()
{
    Maximize(); 

	IControl* group_login_main = GetControl("group_login_main");
	IControl* group_account_main = GetControl("group_account_main");
	IControl* group_register_main = GetControl("group_register_main");

	IControl* group_account = GetControl("group_account");
	IControl* group_register = GetControl("group_register");
	
	NULL_RETURN_VOID(group_login_main&&group_account_main&&group_register_main);

	GuiUtils::gui_center(group_login_main);
	GuiUtils::gui_center(group_account_main);
	GuiUtils::gui_center(group_register_main);

	group_account_main->SetVisible(false);
	group_register_main->SetVisible(false);

	REG_CONTROL_EVENT("btn_select_account", "on_click",	FormLogin::OnBtnSelectAccountClick);
	REG_CONTROL_EVENT("btn_select_server",	"on_click",	FormLogin::OnBtnSelectServerClick);
	REG_CONTROL_EVENT("btn_enter_game",		"on_click",	FormLogin::OnBtnEnterGameClick);

	REG_CONTROL_EVENT("btn_register_new",	"on_click",	FormLogin::OnBtnRegisterClick);
	REG_CONTROL_EVENT("btn_login",			"on_click",	FormLogin::OnBtnLoginClick);
	REG_CONTROL_EVENT("btn_exit",			"on_click",	FormLogin::OnBtnAccountExitClick);

	REG_CONTROL_EVENT("btn_reg_cancel",		"on_click",	FormLogin::OnBtnRegisterCancelClick);
	REG_CONTROL_EVENT("btn_reg_login",		"on_click",	FormLogin::OnBtnRegisterLoginClick);
	REG_CONTROL_EVENT("btn_reg_exit",		"on_click",	FormLogin::OnBtnRegisterExitClick);

	//激活账号
	IControl* pGroupActive = GetControl("group_active");


#if SHOW_ACTIVE_ACCOUNT
	IControl* pGroupActive = GetControl("group_active");
	IControl* pBtnActiveAccount = GetControl("btn_active_account");
	if (pBtnActiveAccount && pGroupActive)
	{
		pBtnActiveAccount->SetVisible(true);
		REG_CONTROL_EVENT("btn_active_account",	"on_click",	FormLogin::OnBtnActiveAccClick);
	}
#else
	IControl* pBtnActiveAccount = GetControl("group_active");
	if (pGroupActive != NULL)
	{
		pGroupActive->SetVisible(false);
	}
#endif


	_readSystemSet();

	IControl* btn_account = GetControl("btn_select_account");
	IControl*  btn_server = GetControl("btn_select_server");
	NULL_RETURN_VOID(btn_account&&btn_server);

	if (!m_Account.empty())
	{
		btn_account->SetText(StringUtil::StringAsWideStr(m_Account.c_str()).c_str());
	}

	std::wstring server_name = StringUtil::UTF8StringAsWideStr(m_ServerName.c_str());
	btn_server->SetText(server_name.c_str());
}


void FormLogin::OnClosed()
{
}

// 角色选择界面调用的设置服务器接口
void FormLogin::SetServerAddrPort(const std::string& name, const std::string& addr, int port)
{
	m_ServerName = name;
	m_ServerAddr = addr;
	m_ServerPort = port;

	IControl* btn_server = GetControl("btn_select_server");
	NULL_RETURN_VOID(btn_server);

}

// 选择账号界面显示
void FormLogin::OnBtnSelectAccountClick(const char* event, const IVarList& arg)
{
	IControl* group_account_main = GetControl("group_account_main");
	IControl* group_account = GetControl("group_account");
	NULL_RETURN_VOID(group_account_main && group_account);

	group_account_main->SetVisible(true);
}

void FormLogin::OnAccountRealClosed(const char* event, const IVarList& arg)
{
	IControl* group_account_main = GetControl("group_account_main");
	NULL_RETURN_VOID(group_account_main);

	group_account_main->SetVisible(false);
}
void FormLogin::OnRegisterRealClosed(const char* event, const IVarList& arg)
{
	IControl* group_register_main = GetControl("group_register_main");
	NULL_RETURN_VOID(group_register_main );

	group_register_main->SetVisible(false);
}

// 输入账号界面关闭
void FormLogin::OnBtnAccountExitClick(const char* event, const IVarList& arg)
{
	IControl* group_account_main = GetControl("group_account_main");
	IControl* group_account = GetControl("group_account");
	NULL_RETURN_VOID(group_account_main && group_account);
}

// 选择服务器界面显示
void FormLogin::OnBtnSelectServerClick(const char* event, const IVarList& arg)
{
}

// 注册账号界面上点击关闭按钮
void FormLogin::OnBtnRegisterCancelClick(const char* event, const IVarList& arg)
{
	IControl* group_register_main = GetControl("group_register_main");
	IControl* group_register = GetControl("group_register");
	NULL_RETURN_VOID(group_register_main && group_register);

}

// 注册账号界面上点击取消按钮
void FormLogin::OnBtnRegisterExitClick(const char* event, const IVarList& arg)
{
	OnBtnRegisterCancelClick(event, arg);
}
// 注册账号界面上点击注册按钮
void FormLogin::OnBtnRegisterLoginClick(const char* event, const IVarList& arg)
{
#define MIN_PASSWORD_LEN 8
#define PHONE_NUM_LEN 11

	IControl* ipt_account = GetControl("ipt_reg_account");
	IControl* ipt_password = GetControl("ipt_reg_password");
	IControl* ipt_psw_again = GetControl("ipt_reg_password_confirm");
	IControl* ipt_phone_num = GetControl("ipt_phone_num");
	if (!ipt_account || !ipt_password || !ipt_psw_again || !ipt_phone_num)
	{
		return;
	}

	std::wstring account_text = ipt_account->GetText();
	std::wstring password_text = ipt_password->GetText();
	std::wstring psw_again_text = ipt_psw_again->GetText();
	std::wstring phone_num = ipt_phone_num->GetText();

	EventDelegate event_delegate(this, &FormLogin::OnBtnRegisterError);

	if (password_text.length()<MIN_PASSWORD_LEN)
	{
	}

	if (password_text!=psw_again_text)
	{
	}

	if (phone_num.length()!=PHONE_NUM_LEN)
	{
	}
}

// 注册失败
void FormLogin::OnBtnRegisterError(const char* event, const IVarList& arg)
{
	IControl* ipt_account = GetControl("ipt_reg_account");
	IControl* ipt_password = GetControl("ipt_reg_password");
	IControl* ipt_psw_again = GetControl("ipt_reg_password_confirm");
	IControl* ipt_phone_num = GetControl("ipt_phone_num");
	if (!ipt_account || !ipt_password || !ipt_psw_again || !ipt_phone_num)
	{
		return;
	}

	ipt_account->SetText(L"");
	ipt_password->SetText(L"");
	ipt_psw_again->SetText(L"");
	ipt_phone_num->SetText(L"");
}

//激活账号
void FormLogin::OnBtnActiveAccClick(const char* event, const IVarList& arg)
{

}

// 进入游戏
void FormLogin::OnBtnEnterGameClick(const char* event, const IVarList& arg)
{
//ios散包测试
#ifndef PACKAGE_SYSTEM
#ifdef FX_SYSTEM_IOS
	m_ServerAddr = "172.19.35.39";
#endif
#endif
}

// 注册账号
void FormLogin::OnBtnRegisterClick(const char* event, const IVarList& arg)
{
	IControl* group_register_main = GetControl("group_register_main");
	IControl* group_register = GetControl("group_register");
	NULL_RETURN_VOID(group_register_main && group_register);

	group_register_main->SetVisible(true);
}

// 选择账号界面，登陆游戏
void FormLogin::OnBtnLoginClick(const char* event, const IVarList& arg)
{
	IControl* ipt_account = GetControl("ipt_account");
	IControl* ipt_password = GetControl("ipt_password");
	IControl* group_account = GetControl("group_account_main");
	NULL_RETURN_VOID(ipt_account && ipt_password && group_account);

	m_Account = StringUtil::WideStrAsString(ipt_account->GetText());
	m_Password = StringUtil::WideStrAsString(ipt_password->GetText());

	group_account->SetVisible(false);

	IControl* btn_account =  GetControl("btn_select_account");
	NULL_RETURN_VOID(btn_account);

	btn_account->SetText(StringUtil::StringAsWideStr(m_Account.c_str()).c_str());

	OnBtnEnterGameClick(event, arg);
}


// 连接服务器成功
void FormLogin::OnConnected(const char* event, const IVarList& arg)
{
	_writeSystemSet();
}

// 连接服务器失败
void FormLogin::OnConnectFail(const char* szEventName, const IVarList& arg)
{
	{
	}
}

// 连接中断
void FormLogin::OnSockClosed(const char* event, const IVarList& arg)
{
}

// 读取之前保存的登录数据：服务器及账号信息
void FormLogin::_readSystemSet()
{
	std::string strIniPath = g_core->GetWorkPath();

	strIniPath += SYSTEM_SET;

	IIniFile* pConfIniFile = g_core->CreateIniFile(strIniPath.c_str());

	if (!pConfIniFile)
	{
		TraceLog(LOG_ERROR, "StageUtils::_readSystemSet file(%s) load error !", strIniPath.c_str());
		return;
	}

	pConfIniFile->LoadFromFile();

	std::string		  account = pConfIniFile->ReadString("main", "login_account",	"");
	std::string	     password = pConfIniFile->ReadString("main", "login_password",	"");
	std::string	  server_name = pConfIniFile->ReadString("main", "server_name",		"");
	std::string	  server_addr = pConfIniFile->ReadString("main", "server_addr",		"");
	std::string	strServerPort = pConfIniFile->ReadString("main", "server_port",		"");
	pConfIniFile->Release();

	int server_port = StringUtil::StringAsInt(strServerPort);

	if (server_name.empty())
	{
	}

	m_Account = account;

	m_ServerName = server_name;
	m_ServerAddr = server_addr;
	m_ServerPort = server_port;
}

// 将服务器及账号信息保存到配置文件中
void FormLogin::_writeSystemSet()
{
	std::string strIniPath = g_core->GetWorkPath();
	strIniPath += SYSTEM_SET;

	IIniFile* pConfIniFile = g_core->CreateIniFile(strIniPath.c_str());
	if (!pConfIniFile)
	{
		TraceLog(LOG_ERROR, "StageUtils::_writeSystemSet file(%s) load error !", strIniPath.c_str());
		return;
	}

	pConfIniFile->LoadFromFile();
	pConfIniFile->WriteString("main",	"login_account",	m_Account.c_str() );
	//pConfIniFile->WriteString("main",	"login_password",	StageUtils::EncodeString(m_Password, "").c_str());
	pConfIniFile->WriteString("main",	"server_name",		m_ServerName.c_str());
	pConfIniFile->WriteString("main",	"server_addr",		m_ServerAddr.c_str());
	pConfIniFile->WriteString("main",	"server_port",		StringUtil::IntAsString(m_ServerPort).c_str());
	bool res = pConfIniFile->SaveToFile();
	if (!res)
	{
		TraceLog(LOG_ERROR, "[FormLogin::_writeSystemSe] write file %s failed!", strIniPath.c_str());
	}
	pConfIniFile->Release();
}