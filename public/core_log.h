//--------------------------------------------------------------------
// �ļ���:		core_log.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��5��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORE_LOG_H
#define _PUBLIC_CORE_LOG_H

#include "i_core.h"
#include "stdio.h"

extern ICore* g_pCore;

// ��¼��־
inline void CORE_TRACE(const char* info)
{
	g_pCore->TraceLog(info);
}

inline void CORE_TRACE(const wchar_t* info)
{
	g_pCore->TraceLogW(info);
}

// �����LOG�����
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

// �����LOG�����
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
