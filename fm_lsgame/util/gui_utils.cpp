


#include "gui_utils.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"

#include "../global.h"
#include "../helper.h"

#include "../visual/i_render.h"
#include "../visual/i_ui3d.h"

#include "../formlogic/form_base.h"

#include <sstream>

// 预加载一个Form
void GuiUtils::gui_preloadform(const std::string& form_class, bool bStatic, bool bAsyncLoad, bool bCenter)
{
	if (!CHelper::GetGlobalEntity(form_class.c_str()))
	{
		FormBase* instance = (FormBase*)CHelper::CreateEntity(form_class.c_str());
		if(instance)
		{
			instance->LoadAndHide(bStatic, false, bAsyncLoad, bCenter);
		}
	}
}
// 清除一个全局的FormBase
void GuiUtils::gui_unloadform(const std::string& form_class)
{
	FormBase* form = (FormBase*)CHelper::GetGlobalEntity(form_class.c_str());
	if (form)
	{
		form->Close();
		g_core->DeleteEntity(form->GetID());
		CHelper::SetGlobalEntity(form_class.c_str(), NULL);
	}
}

// 载入一个界面文件
PERSISTID GuiUtils::gui_loadform(const char* file, bool bStatic, bool bAsyncLoad)
{
	if (bAsyncLoad)
	{
		return g_designeder->LoadFormAsync(g_core->GetResourcePath(), file, bStatic);
	}
	else
	{
		return g_designeder->LoadForm(g_core->GetResourcePath(), file, bStatic);
	}
}

// 删除一个窗体
bool GuiUtils::gui_removeform(const PERSISTID& form_id)
{
	return g_designeder->Delete(form_id);
}

//使一个界面相对于桌面居中
void GuiUtils::gui_center(IControl* pControl)
{
	if (pControl == NULL)
	{
		return;
	}

	int iDevWidth = g_render->GetDeviceWidth();
	int iDevHeight = g_render->GetDeviceHeight();
	float fRatio = g_gui->GetScaleRatio();

	int iFormWidth = (int)(iDevWidth/fRatio);
	int iFormHeight = (int)(iDevHeight/fRatio);

	int offset_x = (pControl->GetWidth() - iFormWidth)/2;
	int offset_y = (pControl->GetHeight() - iFormHeight)/2;

	pControl->SetLeft(-offset_x);
	pControl->SetTop(-offset_y);
}


void GuiUtils::gui_center_h(IControl* pControl)
{
	if (pControl == NULL)
	{
		return;
	}

	int iDevWidth = g_render->GetDeviceWidth();
	float fRatio = g_gui->GetScaleRatio();

	int iFormWidth = (int)(iDevWidth/fRatio);

	int offset_x = (pControl->GetWidth() - iFormWidth)/2;

	pControl->SetLeft(-offset_x);
}

void GuiUtils::gui_center_v(IControl* pControl)
{
	if (pControl == NULL)
	{
		return;
	}

	int iDevHeight = g_render->GetDeviceHeight();
	float fRatio = g_gui->GetScaleRatio();

	int iFormHeight = (int)(iDevHeight/fRatio);

	int offset_y = (pControl->GetHeight() - iFormHeight)/2;

	pControl->SetTop(-offset_y);
}


int GuiUtils::gui_render_width()
{
	int iDevWidth = g_gui->GetDeviceWidth();
	float fRatio = g_gui->GetScaleRatio();

	int iFormWidth = (int)(iDevWidth/fRatio + 0.5f);

	return iFormWidth;
}

int GuiUtils::gui_render_height()
{
	int iDevHeight = g_gui->GetDeviceHeight();
	float fRatio = g_gui->GetScaleRatio();

	int iFormHeight = (int)(iDevHeight/fRatio + 0.5f);

	return iFormHeight;
}

// 将控件移动到desktop的底层
void GuiUtils::gui_move_to_back(IControl* pControl)
{
	if (!pControl)
	{
		return;
	}

	g_gui->GetDesktopForm()->SendToBack(pControl);
}

// 将控件移动到desktop的顶层
void GuiUtils::gui_move_to_front(IControl* pControl)
{
	if (!pControl)
	{
		return;
	}

	g_gui->GetDesktopForm()->BringToFront(pControl);
}

// 使一个界面最大化（充满整个窗口）
void GuiUtils::gui_maximize_form(IControl* pControl)
{
	if (pControl == NULL)
	{
		return;
	}

	int iDevWidth = g_render->GetDeviceWidth();
	int iDevHeight = g_render->GetDeviceHeight();
	float fRatio = g_gui->GetScaleRatio();

	int iFormWidth = (int)(iDevWidth/fRatio + 1.5f);
	int iFormHeight = (int)(iDevHeight/fRatio + 1.5f);

	pControl->SetLeft(0);
	pControl->SetTop(0);
	pControl->SetWidth(iFormWidth);
	pControl->SetHeight(iFormHeight);
}

// 获得屏幕坐标对应的UI坐标
float GuiUtils::gui_screen_radio()
{
    if (g_gui->GetScaleEnable())
    {
        return (1.0f/g_gui->GetScaleRatio());
    }

    return 1.0f;
}

// 递归克隆pControl控件，并Add到parent上
IControl* GuiUtils::gui_clone_recursion(IControl* parent, IControl* pControl, const std::string& name_add )
{
	if (!parent || ! pControl)
	{
		return NULL;
	}

	PERSISTID designer_id = g_gui->GetDesigner();
	Designer* designer = (Designer*)g_core->GetEntity(designer_id);
	if (!designer)
	{
		return NULL;
	}

	IControl* father = designer->CloneControl(pControl);
	if (father == NULL)
	{
		return NULL;
	}

	//
	std::string name = pControl->GetName() + name_add;
	father->SetName(name.c_str());
	father->SetDesignMode(false);
	father->SetLeft(pControl->GetLeft());
	father->SetTop(pControl->GetTop());
	parent->AddChild(father);

	size_t child_count = pControl->GetChildCount();
	for (int i=0; i<child_count; i++)
	{
		IControl* child = pControl->GetChildByIndex(i);
		if (child)
		{
			gui_clone_recursion(father, child, name_add);
		}		
	}

	return father;
}

IControl* GuiUtils::gui_remove_all_child(IControl* pControl)
{
	if (!pControl)
	{
		return NULL;
	}

	if (!pControl->GetEntInfo()->IsKindOf("IContainer"))
	{
		return NULL;
	}

	pControl->DeleteAllChild();
/*
	size_t child_count = pControl->GetChildCount();
	for (int i=0; i<child_count;)
	{
		IControl* child = pControl->GetChildByIndex(i);
		if(!child)
		{	
			continue;
		}
		
		if(!pControl->RemoveChild(child))
		{	
			i++; 
		}
		child_count = pControl->GetChildCount();
	}
*/
	return pControl;
}



void GuiUtils::gui_play_effect(IControl* pControl, const char* effect_name)
{

}


void GuiUtils::gui_stop_effect(IControl* pControl, const char* effect_name)
{
}

void GuiUtils::gui_play_effect_and_do(IControl* pControl, const char* effect_name, EventDelegate pCallback)
{

}


bool GuiUtils::gui_is_effect_playing(IControl* pControl, const char* effect_name)
{
	return true;
}

void GuiUtils::gui_play_comic(const std::string& comic_name)
{

}

void GuiUtils::gui_stop_comic(const std::string& comic_name)
{

}

void GuiUtils::gui_play_comic_and_do(const std::string& comic_name, EventDelegate pCallback)
{

}

IControl* GuiUtils::gui_get_sub_control(const PERSISTID& form_id, const char* szPathName)
{
	if(StringEmpty(szPathName))
	{
		return NULL;
	}

	IControl* pControl = (IControl*) g_core->GetEntity(form_id);
	if (!pControl) {
		return NULL;
	}

	char szControlName[512];
	std::stringstream strmTemp; strmTemp<<szPathName;
	while ( !strmTemp.eof() )
	{
		strmTemp.getline(szControlName, sizeof(szControlName), '.');
		IControl* pChild = pControl->QueryControl(szControlName);
		if (NULL == pChild)
		{
			return NULL;
		}
		pControl = pChild;
	}

	return pControl;
}

/*
// 关掉所有模态窗口
void GuiUtils::gui_close_all_modalform()
{
	Form* form = (Form*)g_gui->GetModalForm();
	while(form)
	{
		g_gui->RemoveModalForm(form);

		form = (Form*) g_gui->GetModalForm();
	}
}

// 关掉desktop的所有子窗体
void GuiUtils::gui_close_all_desktopform()
{
	IControl* pDesktop = g_gui->GetDesktopForm();
	if (!pDesktop) {
		return;
	}

	int child_count = pDesktop->GetChildCount();
	for (int i=0; i<child_count; i++)
	{
		IControl* child = pDesktop->GetChildByIndex(i);
		if(!child->GetEntInfo()->IsKindOf("Form"))
		{
			continue;
		}

		Form* form = (Form*)child;
		form->SetVisible(false);
	}
}
*/

// 根据窗口名和空间名获控件
IControl* GuiUtils::gui_get_control(const std::string& form_name, const std::string& ctrl_name)
{
	IControl* pDesktop = g_gui->GetDesktopForm();
	if (!pDesktop) {
		return NULL;
	}

	int child_count = pDesktop->GetChildCount();
	for (int i=0; i<child_count; i++)
	{
		IControl* child = pDesktop->GetChildByIndex(i);
		if(!child->GetEntInfo()->IsKindOf("Form"))
		{
			continue;
		}

		Form* form = (Form*)child;
		std::string form_file_name = form->GetFormName();
		if (form_file_name!=form_name) {
			continue;
		}

		if (ctrl_name.empty()) {
			return child;
		}

		return child->QueryControl(ctrl_name.c_str());
	}

	return NULL;
}
// 根据窗口名和控件名获取3DUI控件
IControl* GuiUtils::gui_get_3dui_control(const std::string& form_name, const std::string& ctrl_name)
{
	IUI3DCore* ui3d_core = (IUI3DCore*)g_pCore->GetInterface("ui3d_core");
	if (!ui3d_core) {
		return NULL;
	}

	int form_count = ui3d_core->GetFormCount();
	for (int i=0; i<form_count; i++)
	{
		PERSISTID form_id = ui3d_core->GetForm(i);
		Form* form = (Form*)g_core->GetEntity(form_id);
		if (!form) {
			return NULL;
		}

		if(!(form->GetEntInfo()->IsKindOf("Form"))) {
			return NULL;
		}

		std::string form_file_name = form->GetFormName();
		if (form_file_name!=form_name) {
			continue;
		}

		if (ctrl_name.empty()) {
			return form;
		}

		return form->QueryControl(ctrl_name.c_str());
	}

	return NULL;
}


// 是否当前只有大厅主界面显示
bool GuiUtils::gui_only_main_hall_show()
{
	IControl* pDesktop = g_gui->GetDesktopForm();
	if (!pDesktop) {
		return false;
	}

	std::vector<std::string> forms;
	int child_count = pDesktop->GetChildCount();
	for (int i=0; i<child_count; i++)
	{
		IControl* child = pDesktop->GetChildByIndex(i);
		if(!child->GetEntInfo()->IsKindOf("Form")){
			continue;
		}

		Form* form = (Form*)child;
		std::string form_name = form->GetFormName();
		if (!form->GetVisible()) {
			continue;
		}

		if (form_name!="form_gaming" 
			&& form_name!="form_main_hall" 
			&& form_name!="form_prop_change" 
			&& form_name!="form_main_message"
			)
		{
			return false;
		}
	}

	return true;
}