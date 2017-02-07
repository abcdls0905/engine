//--------------------------------------------------------------------
// 文件名:		portable.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_PORTABLE_H
#define _PUBLIC_PORTABLE_H

#include "macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_portable.h"
	#include "../system/win_filesys.h"
	#include "../system/win_string.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_portable.h"
	#include "../system/linux_filesys.h"
	#include "../system/linux_string.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_portable.h"
	#include "../system/ios_filesys.h"
	#include "../system/ios_string.h"
#endif // FX_SYSTEM_IOS

#endif // _PUBLIC_PORTABLE_H
