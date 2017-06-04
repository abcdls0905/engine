
#ifndef _UTILS_GAME_TIMER_H
#define _UTILS_GAME_TIMER_H

#include "../public/macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_timer.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_timer.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_timer.h"
#endif // FX_SYSTEM_IOS

#endif // _UTILS_GAME_TIMER_H
