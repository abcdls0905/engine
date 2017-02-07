#include "global.h"
#include "utility.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "formlogic/form_mbox.h"
#include "../public/core_log.h"

PERSISTID gui_loadform(const char* file)
{
	char buf[256];
	sprintf(buf, "gui load form %s", file);
	CORE_TRACE(buf);
	return g_designeder->LoadForm(g_core->GetWorkPath(), file);
}

//ȷ��
form_mbox s_form_mbox;
void gui_showmbox(const wchar_t* text)
{
	s_form_mbox.Show(text);
}

//ȷ��/ȡ���Ի���
void gui_showqbox()
{
	
}

//ʹһ������������������
void gui_center(IControl* control)
{
	Form* form_desktop = (Form*)g_gui->GetDesktopForm();

	int parent_width = form_desktop->GetWidth();
	int parent_height = form_desktop->GetHeight();

	int offset_x = (control->GetWidth() - parent_width)/2;
	int offset_y = (control->GetHeight() - parent_height)/2;
	control->SetLeft(-offset_x);
	control->SetTop(-offset_y);
}