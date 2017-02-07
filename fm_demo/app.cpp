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
#include "game_main.h"
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
#include "../visual/i_soundsystem.h"
#include "../public/core_log.h"

#include "../visual/i_input.h"
#include "message.h"

ICore* g_pCorePtr = NULL;
IWorld* g_pWorld  = NULL;

IInputManager* g_pInputManager = NULL;

extern void lib_main_logic();
extern void lib_main_gui();
extern void lib_main_render();
extern void lib_main_world();
extern void lib_main_model();
extern void lib_main_terrain();
extern void lib_main_input();
extern void lib_main_movie();
extern void lib_main_geometry();
extern void lib_main_physics_bullet();

//extern void lib_main_net();

#ifdef __ANDROID__
extern void lib_jni_file();
#endif


#define UI_WIDTH 1024
#define UI_HEIGHT 768

// 加载核心配置文件
static bool load_core_config( CCoreConfig& config)
{
	// 主循环休眠时间
	config.m_nSleep = 0;

	config.m_strProgramPath = config.m_strWorkPath + "pro\\";

	//资源路径
	//result_string strResPath = config.m_strWorkPath;
	//size_t iIndex = strResPath.find("mobile");
	//strResPath = strResPath.substr(0, iIndex);
	//strResPath.append("client/");


	config.m_strResourcePath = config.m_strWorkPath + "res\\";


 
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
			LOGI("%s", info);
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

void App::Init(const char* workpath, const IVarList& args, int screen_width, int screen_height, float scale, const char* filesystem)
{
	if(m_initok)
		return ;
	m_width = screen_width;
	m_height = screen_height;
   	
	lib_main_gui();
	lib_main_render();
	lib_main_world();
	lib_main_model();
	lib_main_terrain();
	lib_main_logic();
    lib_main_input();
    lib_main_movie();
    lib_main_geometry();
    lib_main_physics_bullet();
//	lib_main_net();
    
#ifdef __ANDROID__
	lib_jni_file();
#endif
	

	ICore* core = NULL;
	IWorld* pWorld  = NULL;
	
	// 核心配置
	CCoreConfig config;
	
	config.m_strWorkPath = workpath;

//	config.m_nFlags |= CORE_FLAG_MEMORY_VALIDATE;
	config.m_strFileSystem = filesystem;

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
	
	char buf[257];
	sprintf(buf, "filesystem %s", config.m_strFileSystem.c_str());
	CORE_TRACE(buf);
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
	//开始创建逻辑_
    game_main();

    g_pInputManager = (IInputManager*)g_pCorePtr->GetInterface("CInputManager");

	//注。
	//1024，768 是界面的设计分变率
	//scale是缩放比率

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
//            hideKeyboard();
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

void App::OnFrameForPerfHUD_ES(bool bPause)
{
	double time = m_timer.GetElapseTime();
	if(bPause)
		time = 0.0;
	g_pCorePtr->FrameCheckMessage();
	g_pCorePtr->FrameExecute(time);
}

/*
#include <sys/time.h>
#include <time.h>
#include <unistd.h>*/

inline unsigned long _timeGetTime()
	{
	#if defined(_WIN32)
		return timeGetTime();
	#else
		struct timeval tv;
		gettimeofday(&tv, 0);
		return (((tv.tv_sec % 86400)*1000) + tv.tv_usec * 0.001);
	#endif
	}

class Fps
{
	int m_fpscount;
	int m_fps;
	
	long m_currframe;;
public:
	Fps()
		:m_fpscount(0)
		,m_fps(0)
	{
		m_currframe = _timeGetTime();
	}

	inline void OnFrame()
	{
		m_fpscount++;
		long frame = _timeGetTime() - m_currframe;
		if(frame > 1000)
		{
			m_currframe = _timeGetTime();
			m_fps = m_fpscount;
			m_fpscount = 0;
		}
	}

	inline long GetFps()
	{
		return m_fps;
	}
};

void App::TraceLog(const char* pInfo)
{
	g_pCorePtr->TraceLog(pInfo);
}


void App::OnRender()
{
    g_pWorld->Display(0.0F);
	char buf[256];
	static Fps fps;
	fps.OnFrame();
	sprintf(buf, "fps %d", fps.GetFps());
//	g_pCorePtr->TraceLog(buf);
}

FmVec2 last_pos;
bool bpress = false;

#include "../visual/i_camera.h"
extern ICamera* g_test_camera;

bool App::OnMsg(unsigned int msg, size_t param1, size_t param2, int& result)
{
	#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
	#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))

	int touchnum = 0;
	FmVec2 vec2TouchData[10];
	switch(msg)
	{
	case WM_SIZE:
		{
		}break;
	case FM_TOUCH_BEGIN:
		{
			bpress = true;
			vec2TouchData[0] = FmVec2( LOWORD(param2),HIWORD(param2));
			vec2TouchData[1] = vec2TouchData[0];

			last_pos = vec2TouchData[1];

			touchnum = 1;
			ITouchEvent* pTouchEvent = g_pInputManager->CreateTouchEvent(PERSISTID(),NULL, Touch_Began,vec2TouchData,touchnum);
			g_pInputManager->SendInputEvent(pTouchEvent);
			g_pInputManager->DeleteTouchEvent(pTouchEvent);
		}break;
	case FM_TOUCH_END:
		{
			bpress = false;
			vec2TouchData[0] = FmVec2(LOWORD(param2), HIWORD(param2));
			vec2TouchData[1] = last_pos;
			touchnum = 1;
			ITouchEvent* pTouchEvent = g_pInputManager->CreateTouchEvent(PERSISTID(),NULL, Touch_Ended,vec2TouchData,touchnum);
			g_pInputManager->SendInputEvent(pTouchEvent);
			g_pInputManager->DeleteTouchEvent(pTouchEvent);
			last_pos = vec2TouchData[0];
		}
		break;
	case FM_TOUCH_MOVE:
		{
			if (!bpress)
				break;
			vec2TouchData[0] = FmVec2( LOWORD(param2),HIWORD(param2));
			vec2TouchData[1] = last_pos;
			touchnum = 1;
			ITouchEvent* pTouchEvent = g_pInputManager->CreateTouchEvent(PERSISTID(),NULL, Touch_Moved,vec2TouchData,touchnum);
			g_pInputManager->SendInputEvent(pTouchEvent);
			g_pInputManager->DeleteTouchEvent(pTouchEvent);
			last_pos = vec2TouchData[0];
		}
		break;
	default:
		break;
	}


	Gui* pGui = (Gui*)g_pCorePtr->LookupEntity("Gui");
	if(pGui == NULL)
		return false;
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
