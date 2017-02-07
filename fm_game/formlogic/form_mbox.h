#pragma once

#include <vector>
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "../fm_gui/label.h"
#include "global.h"
#include "utility.h"
class form_mbox
{
	struct Item
	{
		PERSISTID form;
		PERSISTID btn;
	};

	void on_btn_sure_click(const char* event, const IVarList& arg)
	{
		for(size_t i = 0; i < m_forms.size(); i++)
		{
			if(m_forms[i].btn == arg.ObjectVal(0))
			{
				Form* pForm = (Form*) g_core->GetEntity(m_forms[i].form);
				if(pForm)
				{
					pForm->Close();
					g_designeder->Delete(pForm->GetID());
					m_forms.erase(m_forms.begin() + i);
					return;
				}
			}
		}
	}
	std::vector<Item> m_forms;
public:
	form_mbox()
	{
	
	}

	void Show(const wchar_t* text, EventDelegate* callback = NULL)
	{
		PERSISTID id = gui_loadform("res/skin/form_common/form_error.xml");
		Form* pForm = (Form*) g_core->GetEntity(id);
		Label* pLabel = (Label*)pForm->FindChild("lbl_info");
		if(pLabel)
			pLabel->SetText(text);
		pForm->ShowModal();
		gui_center(pForm);
		IControl* btn_ok = pForm->FindChild("btn_ok");
		if(callback)
			event_connect(btn_ok->GetID(), "on_click", *callback);
		else
			event_connect(btn_ok->GetID(), "on_click", EventDelegate(this, &form_mbox::on_btn_sure_click));
		Item item;
		item.form = id;
		item.btn = btn_ok->GetID();
		m_forms.push_back(item);
	}
};