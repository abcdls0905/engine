//--------------------------------------------------------------------
// �ļ���:		thread.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��10��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_THREAD_H
#define _UTILS_THREAD_H

#include "../public/macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_thread.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_thread.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_thread.h"
#endif // FX_SYSTEM_IOS

#endif // _UTILS_THREAD_H
