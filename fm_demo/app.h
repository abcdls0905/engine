#pragma once

#include "../public/i_var_list.h"
#include "../public/var_list.h"
#include "../utils/game_timer.h"
#include "../fm_gui/gui_input.h"

class App
{
	// ¼ÆÊ±Æ÷
	CGameTimer m_timer;
	int m_width;
	int m_height;
	bool m_initok;
public:
	App()
	{ 
		m_initok = false;
	}
	void Init(const char* workpath, const IVarList& args, int screen_width, int screen_height, float scale, const char* filesystem="");

	void OnFrame();

	void OnFrameForPerfHUD_ES(bool bPause);

	void TraceLog(const char* pInfo);
    
    void OnRender();

	void Shutdown();

	bool OnMsg(unsigned int msg, size_t param1, size_t param2, int& result);
#ifndef _WIN32
    void OnInput(const char* utf8);
#endif
	void OnShowKeyboard(int height = 400.0f);

	void OnHideKeyboard();
    
	static App& Inst()
	{
		static App s_app;
		return s_app;
	}

	const char* GetFocusText(char* buf, size_t size);

};