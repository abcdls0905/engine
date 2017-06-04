
#ifndef _UTILS_SHARE_MEM_H
#define _UTILS_SHARE_MEM_H

#include "../public/macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_sharemem.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_sharemem.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_sharemem.h"
#endif // FX_SYSTEM_IOS

#endif // _UTILS_SHARE_MEM_H

