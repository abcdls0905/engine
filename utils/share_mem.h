//--------------------------------------------------------------------
// 文件名:		share_mem.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年8月23日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

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

