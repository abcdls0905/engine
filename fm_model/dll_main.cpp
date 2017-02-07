//--------------------------------------------------------------------
// 文件名:		dll_main.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifdef _DEBUG

#include "../utils/leak_checker.h"

const char* g_leak_checker_file = "mem_fx_model.log";
CLeakChecker g_LeakChecker;

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{ 
	switch (fdwReason) 
	{ 
	case DLL_PROCESS_ATTACH: 
		g_LeakChecker.Initialize();
		break; 
	case DLL_THREAD_ATTACH: 
		break; 
	case DLL_THREAD_DETACH: 
		break; 
	case DLL_PROCESS_DETACH: 
		g_LeakChecker.DumpMemory();
		g_LeakChecker.Shutdown();
		break; 
	default: 
		break; 
	} 
	
	return TRUE; 
} 

#endif // _DEBUG
*/

#include "../public/module.h"

//IEntCreator& Camera_EntCreator();
IIntCreator& ModelSystem_IntCreator();

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());
void lib_main_model()
{
	LinkIntCreator(ModelSystem_IntCreator());
	REG_ENTITY(CResManagerUpdate);
}