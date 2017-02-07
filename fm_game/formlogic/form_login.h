#pragma once

#include "../public/persistid.h"
#include "../public/i_var_list.h"

class form_login
{
	char m_account[256], m_pwd[256];
	PERSISTID m_form_id;
	void OnCreate();
	void OnDestroy();
	
	//ÊÂ¼þ
	void on_btn_sure_click(const char* event, const IVarList& arg);
	void on_connected(const char* event, const IVarList& arg);
	void on_connect_fail(const char* event, const IVarList& arg);
public:
	void Show();
	void Close();
};