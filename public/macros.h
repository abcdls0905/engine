//--------------------------------------------------------------------
// 文件名:		macros.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_MACROS_H
#define _PUBLIC_MACROS_H

#ifdef _WIN32
	#define FX_SYSTEM_WINDOWS
	#ifdef _WIN64
		#define FX_SYSTEM_64BIT
	#else
		#define FX_SYSTEM_32BIT
	#endif // _WIN64
#endif // _WIN32

#ifdef __linux__
	#define FX_SYSTEM_LINUX
	#ifdef __ANDROID__
		#define FX_SYSTEM_32BIT
		#define __cdecl
	#else
		#define FX_SYSTEM_64BIT
	#endif
#elif __GNUC__
	#define FX_SYSTEM_IOS
	#define FX_SYSTEM_32BIT
#endif // __linux__

#ifdef FX_SYSTEM_WINDOWS
	#include <stddef.h>
	#pragma warning(disable:4786)
	#pragma warning(disable:4996)
	#pragma warning(disable:4819)
	typedef signed __int64 int64_t;
	typedef unsigned __int64 uint64_t;
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include <stddef.h>
	#include <stdint.h>
    #include <wchar.h>
	#define stricmp strcasecmp
#ifdef __ANDROID__
    inline int wcsicmp(const wchar_t *s1, const wchar_t *s2)
    {
        while (towlower(*s1) == towlower(*s2))
        {
            if (0 == *s1)
            {
                return 0;
            }
            s1++;
            s2++;
        }
        return(towlower(*s1) - towlower(*s2));
    }
#else
	#define wcsicmp wcscasecmp
#endif
    #define strnicmp strncasecmp
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include <stddef.h>
	#include <stdint.h>
	#define stricmp strcasecmp
	#define wcsicmp wcscasecmp
    #define strnicmp strncasecmp
#endif // FX_SYSTEM_IOS

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if (p) { (p)->Release(); (p) = NULL; } }
#endif // !SAFE_RELEASE

#include "debug.h"

#endif // _PUBLIC_MACROS_H
