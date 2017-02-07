#include "../fm_gui/gui.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "../fm_gui/edit.h"
#include "utility.h"
#include "global.h"
#include "form_login.h"
#include "../visual/i_event_manager.h"
#include "../public/converts.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_game_sock.h"

inline void gui_event_connect(Form* form, const char* child_name, const char* event_name, EventDelegate delegate)
{
	IControl* pControl = form->FindChild(child_name);
	if(!pControl)
		return;
	event_connect(pControl->GetID(), event_name, delegate);
}

void form_login::OnCreate()
{
    m_account[0]=0;
    m_pwd[0]=0;
	m_form_id = gui_loadform("res/skin/form_common/form_team_invite.xml");

	
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	IControl* p = form->GetParentControl();
	gui_center(form);

	//Form* pForm = (Form*)g_core->GetEntity(gui_loadform("res/skin/form_common/form_loading.xml"));
	//pForm->Show();
	//gui_center(pForm);
	gui_event_connect(form, "btn_sure", "on_click", EventDelegate(this, &form_login::on_btn_sure_click));
	event_connect(g_gamesock->GetID(), "on_connected", EventDelegate(this, &form_login::on_connected));
	event_connect(g_gamesock->GetID(), "on_connect_fail", EventDelegate(this, &form_login::on_connect_fail));

	form->Show();	
}

void form_login::OnDestroy()
{

}

void form_login::Show()
{
	if(m_form_id.nData64 == 0)
	{
		OnCreate();
	}
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	form->Show();
}

void form_login::Close()
{
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	form->Close();
	OnDestroy();
}

void form_login::on_btn_sure_click(const char* event, const IVarList& arg)
{
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	Edit* pEditAccount = (Edit*)form->FindChild("ipt_account");
	Edit* pEditPwd = (Edit*)form->FindChild("ipt_password");
	ToString(pEditAccount->GetText(), m_account, sizeof(m_account));
	ToString(pEditPwd->GetText(), m_pwd, sizeof(m_pwd));
	g_gamesock->Connect("172.19.34.83", 2001, false);
}

void form_login::on_connected(const char* event, const IVarList& arg)
{
    if(m_account[0])
	{
		if(m_pwd[0] == 0)
			g_gamesender->Login(m_account, m_account);
		else
			g_gamesender->Login(m_account, m_pwd);
	}
    else
	{
        g_gamesender->Login("hp0", "hp0");
	}
}

void form_login::on_connect_fail(const char* event, const IVarList& arg)
{
	//链接服务器出错
	int n = 3;
}