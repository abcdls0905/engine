#pragma once

#include "../public/persistid.h"
#include "../public/i_var_list.h"

class form_gmcommand
{
	PERSISTID m_form_id;
	PERSISTID m_edit_id;
	PERSISTID m_listbox_id;
	void OnCreate();
	void OnDestroy();
public:
	void Show();
	void Close();
	
	//ÊÂ¼þ
	void on_enter(const char* event, const IVarList& arg);
	void on_custom(const char* event, const IVarList& arg);
};