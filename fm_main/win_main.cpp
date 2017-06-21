//--------------------------------------------------------------------
// �ļ���:		win_main.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#define _WIN32_WINNT 0x0400
#include <windows.h>
#include "../public/macros.h"
#include "../public/i_core.h"
#include "../public/core_config.h"
#include "../public/fast_str.h"
#include "../public/var_list.h"
#include "../public/inlines.h"


#include "../utils/ini_file.h"
#include "../utils/game_timer.h"
#include "../utils/share_mem.h"

#include "../visual/i_world.h"

#include "win_graphdevice.h"

#include "app.h"

//ASSERTION_FAIL_DEFINED;

// ����·��
char g_strWorkPath[256] = { 0 };
// ����ָ��
extern ICore* g_pCorePtr;

// ��ʼ���ڴ�С
const DWORD FACE_WIDTH = 1280;
const DWORD FACE_HEIGHT = 768;

// Ŀ¼�Ƿ����
static bool path_exists(const char* name)
{
	Assert(name != NULL);

	DWORD res = GetFileAttributesA(name);

	if (res == DWORD(-1))
	{
		return false;
	}

	return (res & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// ����ǰ��ո�
static result_string trim_cmd_line(const char* cmd)
{
	Assert(cmd != NULL);

	char buf[256];

	TrimString(cmd, buf, sizeof(buf));

	return result_string(buf);
}

// �������л�ÿ�ִ���ļ���
static result_string get_exe_name(const char* cmd)
{
	Assert(cmd != NULL);

	char buf[256];
	
	buf[0] = 0;

	if (cmd[0] == '\"')
	{
		const char* beg = cmd + 1;
		const char* end = strchr(beg, '\"');

		if (NULL == end)
		{
			return result_string(buf);
		}

		const size_t SIZE1 = end - beg;
		
		if (SIZE1 >= sizeof(buf))
		{
			return result_string(buf);
		}

		memcpy(buf, beg, SIZE1);
		buf[SIZE1] = 0;
	}
	else
	{
		CopyString(buf, sizeof(buf), cmd);

		for (char* s = buf; *s; ++s)
		{
			if ((*s == ' ') || (*s == '\t'))
			{
				*s = 0;
				break;
			}
		}
	}

	return result_string(buf);
}

// ��������ļ�������
static result_string get_ini_name(const char* exe_name)
{
	Assert(exe_name != NULL);

	char buf[256];
	char* file_part;

	Port_GetFullPathName(exe_name, buf, sizeof(buf), &file_part);
	
	if (NULL == file_part)
	{
		return result_string();
	}
	
	strlwr(file_part);
	
	char* p = strstr(file_part, ".exe");

	if (NULL == p)
	{
		return result_string();
	}

	strncpy(p, ".ini", 4);
	
	return result_string(file_part);
}

// �ָ�������
static size_t get_cmd_args(const char* cmd, char* buf, size_t size,
	const char* args[], size_t arg_max)
{
	Assert(cmd != NULL);
	Assert(buf != NULL);
	Assert(size > 0);
	
	size_t argc = 0;
	
	CopyString(buf, size, cmd);

	char* s = buf;
	
	if (*s == '\"')
	{
		char* beg = ++s;
		
		s = strchr(beg, '\"');
		
		if (NULL == s)
		{
			return 0;
		}
		
		*s++ = 0;
		
		if (argc < arg_max)
		{
			args[argc++] = beg;
		}
	}
	else
	{
		for (; *s; ++s)
		{
			if ((*s == ' ') || (*s == '\t'))
			{
				*s++ = 0;
				break;
			}
		}
		
		if (argc < arg_max)
		{
			args[argc++] = buf;
		}
	}

	char* arg = NULL;
	
	for (; *s; ++s)
	{
		if ((*s == ' ') || (*s == '\t'))
		{
			*s = 0;
			
			if (arg != NULL)
			{
				if (argc < arg_max)
				{
					args[argc++] = arg;
				}

				arg = NULL;
			}
		}
		else
		{
			if (NULL == arg)
			{
				arg = s;
			}
		}
	}
	
	if (arg != NULL)
	{
		if (argc < arg_max)
		{
			args[argc++] = arg;
		}
	}

	return argc;
}

// �������л�ù���Ŀ¼
static result_string get_work_path(const char* exe)
{
	Assert(exe != NULL);

	char buf[256];
	
	buf[0] = 0;

	const char* slash = strrchr(exe, '\\');

	if (NULL == slash)
	{
		slash = strrchr(exe, '/');

		if (NULL == slash)
		{
			return result_string(buf);
		}
	}
	
	const size_t SIZE1 = slash - exe + 1;

	if (SIZE1 >= sizeof(buf))
	{
		return result_string(buf);
	}

	memcpy(buf, exe, SIZE1);

	buf[SIZE1] = 0;
	
	return result_string(buf);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if(g_pCorePtr)
	{
		int result;
		if(App::Inst().OnMsg(iMsg, wParam, lParam, result))
			return result;
	}
		
	switch (iMsg)
	{
 	case WM_CLOSE:
		//�����Ƴ�
 		g_pCorePtr->SetQuit(true);
	 	DestroyWindow(hwnd);
		// ��ֹϵͳ�Զ��رմ���
 		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

    // ���F10��Alt����������
    if (iMsg == WM_SYSKEYUP && (wParam == VK_MENU || wParam == VK_F10))
    {
	    return 0L;
    }

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

int WinMainProc(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, 
	int iCmdShow)
{
#ifdef _DEBUG11
  AllocConsole();
  freopen("CONOUT$","w+t",stdout);
  freopen("CONIN$","r+t",stdin);
#endif
	ICore* core = NULL;
	IWorld* pWorld  = NULL;

	CGraphDevice graphedevice;

	// ��С��ջ
	if (true)
	{
		// ������
		result_string cmd_line = trim_cmd_line(GetCommandLineA());
		// ִ���ļ���
		result_string exe_name = get_exe_name(cmd_line.c_str());
		// ����Ŀ¼
		result_string work_path = get_work_path(exe_name.c_str());
#ifndef _DEBUG_RT_
		//work_path = "D:\\ɢ��������\\voyage\\";
		//work_path = "I:\\work\\MiniClient\\Res\\client\\";
		//work_path = "C:\\amber_client\\";
#endif
		if (work_path.empty())
		{
			char path[256];

			GetCurrentDirectoryA(sizeof(path), path);
			work_path = result_string(path) + "\\";
		}
		
		SetCurrentDirectoryA(work_path.c_str());
		strcpy(g_strWorkPath, work_path.c_str());
		
		// ��������
		char buf[256];
		const char* argv[32];

		size_t argc = get_cmd_args(cmd_line.c_str(), buf, sizeof(buf), argv, 
			32);

		// ��������ļ���
		result_string ini_name = get_ini_name(exe_name.c_str());

		if (ini_name.empty())
		{
			ini_name = "fm_main.ini";
		}

		if (argc > 1)
		{
			ini_name = argv[1];
		}
		
		// ��������
		CVarList args;

		if (argc > 2)
		{
			for (size_t i = 2; i < argc; ++i)
			{
				args.AddString(argv[i]);
			}
		}
		
		// ����Windows����
		WNDCLASSEX wndclass;

		wndclass.cbSize = sizeof(wndclass);
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;//sizeof(WORD);
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = TEXT("fm_main");
		wndclass.hIconSm = NULL;
		
		RegisterClassEx(&wndclass);

		RECT winRect;
		SetRect(&winRect,0,0,FACE_WIDTH,FACE_HEIGHT);
		AdjustWindowRectEx(&winRect, WS_CAPTION|WS_SYSMENU, false, 0);

		int x =  -winRect.left;
		winRect.left += x;
		winRect.right += x;
		int y =  -winRect.top;
		winRect.top += y;
		winRect.bottom += y;

		//WS_VISIBLE|WS_CAPTION|WS_SYSMENU
		HWND hwnd = CreateWindow(TEXT("fm_main"), TEXT("fm_main"), WS_OVERLAPPEDWINDOW,
			(GetSystemMetrics(SM_CXSCREEN) - FACE_WIDTH) / 2, 
			(GetSystemMetrics(SM_CYSCREEN) - FACE_HEIGHT) / 2, 
			winRect.right-winRect.left, winRect.bottom-winRect.top, NULL, NULL, hInstance, NULL);

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		::GetClientRect(hwnd, &winRect);

		graphedevice.InitDevice(hwnd);
	
		App::Inst().Init(work_path.c_str(), args, winRect.right-winRect.left, winRect.bottom-winRect.top, 1.0f);
	}

	// ��������ֵ
	/*
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

	// ���߳���CPU���İ��Ա�֤��ʱ������ȷ��
	SetThreadAffinityMask(GetCurrentThread(), mask);

	// ��ʱ��
	CGameTimer timer;

	timer.SetUseQPF(true);
	timer.Initialize();*/

	// ����Ϣѭ��
	MSG	msg;
	
	for (;;)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message) 
			{
				// ���Ĺر�
				App::Inst().Shutdown();
				graphedevice.ShutDevice();
				return 1;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
		}

		if (g_pCorePtr->GetQuit())
		{
			break;
		}

		App::Inst().OnFrame();
		App::Inst().OnRender();
		graphedevice.Present();
		g_pCorePtr->FrameSleep();
	}

	// ���Ĺر�
	App::Inst().Shutdown();
	graphedevice.ShutDevice();
	return 1;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, int iCmdShow)
{

#ifdef _DEBUG
	// Enable memory-leak reports
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
 //	_CrtSetBreakAlloc(1911);
#endif
	if (IsDebuggerPresent() == FALSE)
	{
//		InitMiniDump();
	}

	return WinMainProc(hInstance, hPrevInstance, lpCmdLine, iCmdShow);
}
