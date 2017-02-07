#include "utility.h"
#include "global.h"
#include "form_gmcommand.h"
#include "../fm_gui/form.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_game_sock.h"
#include "../fm_gui/edit.h"
#include "../fm_gui/list_box.h"

void form_gmcommand::OnCreate()
{
	m_form_id = gui_loadform("res/skin/form_test/form_gm_command.xml");
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	IControl* pControl = form->FindChild("func_grpbox")->FindChild("param_edit");
	event_connect(pControl->GetID(), "on_enter", EventDelegate(this, &form_gmcommand::on_enter));
	event_connect(g_gamesock->GetReceiver(), "on_custom", EventDelegate(this, &form_gmcommand::on_custom));
	
	m_edit_id = pControl->GetID();

	m_listbox_id = form->FindChild("ListBox")->GetID();
}

void form_gmcommand::OnDestroy()
{
}

void form_gmcommand::on_custom(const char* event, const IVarList& arg)
{
	const char* type = arg.StringVal(1);
	if(stricmp(type, "sysinfo") == 0)
	{
		if(99 == arg.IntVal(2))
		{
			const char* p = arg.StringVal(3);
			if(stricmp(p, "gm_info") == 0)
			{
				const wchar_t* p = arg.WideStrVal(4);
				ListBox* plistbox = (ListBox*)g_core->GetEntity(m_listbox_id);
				plistbox->AddString(p);	
			}
		}
	}
}

void form_gmcommand::on_enter(const char* event, const IVarList& arg)
{
	Edit* pEdit = (Edit*)g_core->GetEntity(m_edit_id);
	const wchar_t* text = pEdit->GetText();
	if(text && text[0])
	{
		CVarList arg, result;
		arg.AddInt(2);
		arg.AddWideStr(text);
		g_gamesender->Custom(arg, result);
	}
	pEdit->SetText(L"");
}

void form_gmcommand::Show()
{
	if(m_form_id.nData64 == 0)
		OnCreate();
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	form->Show();
}

void form_gmcommand::Close()
{
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	form->Close();
	OnDestroy();
}