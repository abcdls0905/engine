#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define  LOG_TAG    "fm"
#endif

#include "app.h"
#include "../public/i_core.h"
#include "../visual/i_world.h"
#include "../public/core_config.h"
#include "../utils/share_mem.h"
#include "../fm_gui/i_control.h"

#ifdef FX_SYSTEM_WINDOWS
#include "../system/win_encoding.h"
#include "../system/win_string.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
#include "../system/linux_encoding.h"
#include "../system/linux_string.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
#include "../system/ios_encoding.h"
#include "../system/ios_string.h"
#endif // FX_SYSTEM_IOS


ICore* g_pCorePtr = NULL;
IWorld* g_pWorld  = NULL;

extern void lib_main_game();
extern void lib_main_gui();
extern void lib_main_render();
extern void lib_main_world();
extern void lib_main_model();
extern void lib_main_terrain();
extern void lib_main_net();
extern void lib_main_ui3d();

#define UI_WIDTH 1024
#define UI_HEIGHT 768

// 加载核心配置文件
static bool load_core_config( CCoreConfig& config)
{
	// 主循环休眠时间
	config.m_nSleep = 0;

	config.m_strProgramPath = config.m_strWorkPath + "pro\\";

	config.m_strResourcePath = config.m_strWorkPath + "res\\";
 
	// 文件系统
	config.m_strFileSystem = "";
	// 文件系统配置信息
	config.m_strFileConfig = "config.cfg";
	// 主实体名
	config.m_strMainEntity = "World";

	return true;
}

// 显示错误对话框
static void show_fatal_error(const char* info)
{
#ifdef _WIN32
	MessageBoxA(NULL, info, "fatal error", MB_OK | MB_ICONERROR);
#endif
}

#ifndef _WIN32
	#ifdef __ANDROID__
		bool _custom_trace(void* context, const char* info)
		{
			#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
			LOGI(info, "%s", info);
			return true;
		}
	#else
		bool _custom_trace(void* context, const char* info)
		{
			::printf(info);
			::printf("\n");
			return true;
		}
	#endif
#endif

void App::Init(const char* workpath, const IVarList& args, int screen_width, int screen_height, float scale)
{
	if(m_initok)
		return ;
	m_width = screen_width;
	m_height = screen_height;
  lib_main_game();
	lib_main_gui();
	lib_main_render();
	lib_main_world();
	lib_main_model();
	lib_main_terrain();
	lib_main_net();
#ifdef _WIN32
	lib_main_ui3d();
#endif


	ICore* core = NULL;
	IWorld* pWorld  = NULL;
	
	// 核心配置
	CCoreConfig config;
	
	config.m_strWorkPath = workpath;
//	config.m_nFlags |= CORE_FLAG_MEMORY_VALIDATE;

	if (!load_core_config(config))
	{
		show_fatal_error("Load core ini failed");
		abort();
	}
	
	// 核心初始化
	core = core_initialize(config);
	
#ifndef _WIN32
	core->SetTraceFunc(_custom_trace, this);
#endif
	if (NULL == core)
	{
		show_fatal_error("Core initialize failed");
		abort();
	}
	
	// 启动核心前必须设置
	g_pCorePtr = core;
	
	// 启动核心
	if (!core_startup(core, args))
	{
		show_fatal_error("Core startup failed");
		abort();
	}

	//传输设备分辨率
	//获取world指针
	pWorld = (IWorld*)core->GetMainEntity();
	pWorld->SetWidth(screen_width);
	pWorld->SetHeight(screen_height);
	pWorld->CreateDevice();
	g_pWorld =pWorld;

	//开始创建逻辑
#ifdef _DEBUG_RT_
    game_main();
#else
	//注。
	//1024，768 是界面的设计分变率
	//scale是缩放比率
	g_pCorePtr->CreateEntityArgs("Game", CVarList() << UI_WIDTH << UI_HEIGHT << screen_height/float(UI_HEIGHT));
#endif

	// 进程索引值
#ifdef _WIN32    
	CShareMem share_mem;
	bool exists = false;

	share_mem.Create("fx_main_share_process_count", sizeof(unsigned int), 
		&exists);

	unsigned int* pProcessCount = (unsigned int*)share_mem.GetMem();
	unsigned int process_index;

	if (exists)
	{
		process_index = *pProcessCount;
	}
	else
	{
		process_index = 0;
	}

	*pProcessCount = process_index + 1;


	SYSTEM_INFO si;

	GetSystemInfo(&si);

	unsigned int mask = 0x1 << (process_index % si.dwNumberOfProcessors);

	// 主线程与CPU核心绑定以保证计时器的正确性
	SetThreadAffinityMask(GetCurrentThread(), mask);
#endif

	m_timer.SetUseQPF(true);
	m_timer.Initialize();
	m_initok = true;
}

#ifndef _WIN32
extern void hideKeyboard();
void App::OnInput(const char* utf8)
{
    int result;
    wchar_t wtext[256];
    Port_UTF8ToWideStr(utf8, wtext, sizeof(wtext));
    for(int i = 0; wtext[i]!=0; i++)
    {
        if(wtext[i] == '\n' && wtext[i+1] == 0)
        {
            hideKeyboard();
            return;
        }
        OnMsg(WM_CHAR, wtext[i], 0, result);
    }
}
#endif

void App::OnFrame()
{
	double time = m_timer.GetElapseTime();
	g_pCorePtr->FrameCheckMessage();
	g_pCorePtr->FrameExecute(time);
	
}

void App::OnRender()
{
    g_pWorld->Display(0.0F);
}

#include "../fm_world/world.h"
bool App::OnMsg(unsigned int msg, size_t param1, size_t param2, int& result)
{
	Gui* pGui = (Gui*)g_pCorePtr->LookupEntity("Gui");
	IControl * pFocusControl = pGui->GetFocusControl();
	int n =0;
	if(pFocusControl)
	{
		n = pFocusControl->GetAbsTop();
	}
	return g_pCorePtr->ProcessMessage(msg, param1, param2, result);
}

void App::Shutdown()
{
	core_shutdown(g_pCorePtr);
	g_pCorePtr = NULL;
}

void App::OnShowKeyboard(int height)
{
	float scale = m_width/float(UI_WIDTH);
	Gui* pGui = (Gui*)g_pCorePtr->LookupEntity("Gui");
	IControl* pControl = pGui->GetDesktopForm();
    int key_top = UI_HEIGHT - height;
	if(pControl)
	{
		IControl * pFocusControl = pGui->GetFocusControl();
   		if(pFocusControl)
		{
            int focus_bottom = pFocusControl->GetAbsTop() + pFocusControl->GetHeight();
			if(focus_bottom > key_top)
			{
                int offset = focus_bottom - key_top;
                pControl->SetTop(pControl->GetTop() - offset);
			}
		}
	}
}

void App::OnHideKeyboard()
{
	Gui* pGui = (Gui*)g_pCorePtr->LookupEntity("Gui");
	IControl* pControl = pGui->GetDesktopForm();
	if(pControl)
	{
		pControl->SetTop(0);
	} 
}

const char* App::GetFocusText(char* buf, size_t size)
{
	Gui* pGui = (Gui*)g_pCorePtr->LookupEntity("Gui");
	if(!pGui)
	{
		buf[0] = 0;
		return buf;
	}

	IControl* pControl = pGui->GetFocusControl();
	if(!pControl)
	{
		buf[0] = 0;
		return buf;
	}

	const wchar_t* wtext = pControl->GetText();
	if(!wtext)
	{
		buf[0] = 0;
		return buf;
	}

	Port_WideStrToUTF8(wtext, buf, size);
	return buf;
}