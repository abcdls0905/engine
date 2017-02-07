//--------------------------------------------------------------------
// 文件名:		core_log.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORE_LOG_H
#define _PUBLIC_CORE_LOG_H

#include "i_core.h"
#include "stdio.h"

extern ICore* g_pCore;

// 记录日志
inline void CORE_TRACE(const char* info)
{
	g_pCore->TraceLog(info);
}

inline void CORE_TRACE(const wchar_t* info)
{
	g_pCore->TraceLogW(info);
}

// 多参数LOG输出：
inline void CORE_TRACE_EX(const char* info, ...)
{
	va_list arglist;
	va_start(arglist, info);

	char sMsg[1536]="";
	vsnprintf(&sMsg[0], 1536-1, info, arglist);
	va_end(arglist);

	sMsg[1536-1] = '\0';
	CORE_TRACE(sMsg);
}

// 多参数LOG输出：
inline void CORE_TRACE_EX(const wchar_t* info, ...)
{
	va_list arglist;
	va_start(arglist, info);

	wchar_t wsMsg[1536]=L"";
	vswprintf(&wsMsg[0], 1536-1, info, arglist);
	va_end(arglist);

	wsMsg[1536-1] = 0;
	CORE_TRACE(wsMsg);
}

#endif // _PUBLIC_CORELOG_H
