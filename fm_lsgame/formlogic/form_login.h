
#pragma once

#include "../public/persistid.h"
#include "../public/i_var_list.h"

#include "form_base.h"

//
class FormLogin:public FormBase
{
public:
	FormLogin();
	~FormLogin();

	static FormLogin* Instance();

	void	SetServerAddrPort(const std::string& name, const std::string& addr, int port);
private:

	void	OnCreate();
	void	OnClosed();

	// 登陆界面事件响应
	void	OnBtnSelectAccountClick(const char* event, const IVarList& arg);
	void	OnBtnSelectServerClick(const char* event, const IVarList& arg);
	void	OnBtnEnterGameClick(const char* event, const IVarList& arg);

	// 输入账号界面事件响应
	void	OnBtnRegisterClick(const char* event, const IVarList& arg);
	void	OnBtnLoginClick(const char* event, const IVarList& arg);
	void	OnBtnAccountExitClick(const char* event, const IVarList& arg);

	// 注册账号界面响应
	void	OnBtnRegisterCancelClick(const char* event, const IVarList& arg);
	void	OnBtnRegisterLoginClick(const char* event, const IVarList& arg);
	void	OnBtnRegisterExitClick(const char* event, const IVarList& arg);

	void	OnBtnRegisterError(const char* event, const IVarList& arg);

	//激活账号
	void	OnBtnActiveAccClick(const char* event, const IVarList& arg);

	// 网络事件
	void	OnConnected(const char* event, const IVarList& arg);
	void	OnConnectFail(const char* event, const IVarList& arg);
	void    OnSockClosed(const char* event, const IVarList& arg);

	void    OnAccountRealClosed(const char* event, const IVarList& arg);
	void    OnRegisterRealClosed(const char* event, const IVarList& arg);

private:
	void	_readSystemSet();
	void	_writeSystemSet();

private:
	std::string m_Account;
	std::string m_Password;

	std::string m_ServerName;
	std::string m_ServerAddr;
	int m_ServerPort;
};