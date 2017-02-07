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

// ע��ؼ��¼�
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

// ��װgui��غ���
class GuiUtils
{
public:
	// Ԥ����һ��FormBase
	static void gui_preloadform(const std::string& form_class, bool bStatic = false, bool bAsyncLoad = false, bool bCenter = false);
	// ���һ��ȫ�ֵ�FormBase
	static void gui_unloadform(const std::string& form_class);

	// ����һ�������ļ�
	static PERSISTID gui_loadform(const char* file, bool bStatic=false, bool bAsyncLoad=false);
	// ɾ��һ������
	static bool gui_removeform(const PERSISTID& form_id);

	// ���ݴ������Ϳؼ�����ؼ�
	static IControl* gui_get_control(const std::string& form_name, const std::string& ctrl_name = "");
	// ���ݴ������Ϳؼ�����ȡ3DUI�ؼ�
	static IControl* gui_get_3dui_control(const std::string& form_name, const std::string& ctrl_name = "");

	// ��ȡ�˿ؼ������ĳ���Ӷ��󣨵ݹ���У��㼶֮����.�ָ
	static IControl* gui_get_sub_control(const PERSISTID& form_id, const char* szPathName);

	// �ص�desktop�������Ӵ���
	//static void gui_close_all_desktopform();
	// �ص�����ģ̬����
	//static void gui_close_all_modalform();

	//ʹһ������������������
	static void gui_center(IControl* pControl);
	static void gui_center_h(IControl* pcontrol);
	static void gui_center_v(IControl* pcontrol);

	// ʹһ��������󻯣������������ڣ�
	static void gui_maximize_form(IControl* pControl);

	// �����Ļ����ת��UI����Ҫ�˵�ֵ
	static float gui_screen_radio();

	// ��ȡ�豸��Ļ�Ŀ��
	static int gui_render_width();
	static int gui_render_height();

	// ��һ���ؼ��ƶ�������/�ײ㣨ZOrder��
	static void gui_move_to_back(IControl* pControl);
	static void gui_move_to_front(IControl* pControl);

	// �ݹ��¡pControl�ؼ��������ֺ�׺����Add��parent��
	static IControl* gui_clone_recursion(IControl* parent, IControl* pControl, const std::string& name_add);

	// �Ƴ��˿ؼ��������ӿؼ�
	static IControl* gui_remove_all_child(IControl* pControl);


	static void gui_play_effect(IControl* pControl, const char* effect_name);
	static void gui_stop_effect(IControl* pControl, const char* effect_name);
	static void gui_play_effect_and_do(IControl* pControl, const char* effect_name, EventDelegate pCallback);
	static bool gui_is_effect_playing(IControl* pControl, const char* effect_name);

	static void gui_play_comic(const std::string& comic_name);
	static void gui_stop_comic(const std::string& comic_name);
	static void gui_play_comic_and_do(const std::string& comic_name, EventDelegate pCallback);


	// �Ƿ�ǰֻ�д�����������ʾ
	static bool gui_only_main_hall_show();
};
