#pragma once

#include "../public/persistid.h"
#include "../public/i_entity.h"
#include "../public/core_log.h"

#include "../fm_gui/gui.h"
#include "../fm_gui/form.h"
#include "../command.h"

#ifndef PRELOAD_FORM
#define PRELOAD_FORM(FormClass) \
	if (!CHelper::GetGlobalEntity(#FormClass))\
	{\
	FormClass* instance = (FormClass*)CHelper::CreateEntity(#FormClass);\
	if(instance)\
		{\
		instance->LoadAndHide();\
		}\
	}
#endif

// 注册控件事件
#ifndef REG_CONTROL_EVENT
#define REG_CONTROL_EVENT(ControlName, EventName, Proc)\
	do{\
	IControl* pCtrl = GetControl(ControlName);\
	if (pCtrl) {\
	event_connect(pCtrl->GetID(), EventName, EventDelegate(this, &Proc));\
	} else {\
	char szInfo[512];\
	sprintf(szInfo, "[Error-Logic] Control: %s Register Event: %s Fail", ControlName, EventName);\
	CORE_TRACE(szInfo);\
	}\
	} while (false)
#endif

// 封装gui相关函数
class GuiUtils
{
public:
	// 预加载一个FormBase
	static void gui_preloadform(const std::string& form_class, bool bStatic = false, bool bAsyncLoad = false, bool bCenter = false);
	// 清除一个全局的FormBase
	static void gui_unloadform(const std::string& form_class);

	// 载入一个界面文件
	static PERSISTID gui_loadform(const char* file, bool bStatic=false, bool bAsyncLoad=false);
	// 删除一个窗体
	static bool gui_removeform(const PERSISTID& form_id);

	// 根据窗口名和控件名获控件
	static IControl* gui_get_control(const std::string& form_name, const std::string& ctrl_name = "");
	// 根据窗口名和控件名获取3DUI控件
	static IControl* gui_get_3dui_control(const std::string& form_name, const std::string& ctrl_name = "");

	// 获取此控件下面的某个子对象（递归进行，层级之间用.分割）
	static IControl* gui_get_sub_control(const PERSISTID& form_id, const char* szPathName);

	// 关掉desktop的所有子窗体
	//static void gui_close_all_desktopform();
	// 关掉所有模态窗口
	//static void gui_close_all_modalform();

	//使一个界面相对于桌面居中
	static void gui_center(IControl* pControl);
	static void gui_center_h(IControl* pcontrol);
	static void gui_center_v(IControl* pcontrol);

	// 使一个界面最大化（充满整个窗口）
	static void gui_maximize_form(IControl* pControl);

	// 获得屏幕坐标转成UI坐标要乘的值
	static float gui_screen_radio();

	// 获取设备屏幕的宽高
	static int gui_render_width();
	static int gui_render_height();

	// 将一个控件移动到顶层/底层（ZOrder）
	static void gui_move_to_back(IControl* pControl);
	static void gui_move_to_front(IControl* pControl);

	// 递归克隆pControl控件加上名字后缀，并Add到parent上
	static IControl* gui_clone_recursion(IControl* parent, IControl* pControl, const std::string& name_add);

	// 移除此控件的所有子控件
	static IControl* gui_remove_all_child(IControl* pControl);


	static void gui_play_effect(IControl* pControl, const char* effect_name);
	static void gui_stop_effect(IControl* pControl, const char* effect_name);
	static void gui_play_effect_and_do(IControl* pControl, const char* effect_name, EventDelegate pCallback);
	static bool gui_is_effect_playing(IControl* pControl, const char* effect_name);

	static void gui_play_comic(const std::string& comic_name);
	static void gui_stop_comic(const std::string& comic_name);
	static void gui_play_comic_and_do(const std::string& comic_name, EventDelegate pCallback);


	// 是否当前只有大厅主界面显示
	static bool gui_only_main_hall_show();
};
