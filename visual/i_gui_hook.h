//--------------------------------------------------------------------
// 文件名:		i_gui_hook.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年2月21日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_GUI_HOOK_H
#define _VISUAL_I_GUI_HOOK_H

#include "../public/i_var_list.h"

// 界面回调

class IEntity;

class IGuiHook
{
public:
	virtual ~IGuiHook() = 0;
	// 释放
	virtual void Release() = 0;
	// 事件
	virtual int OnEvent(IEntity* pEntity, const char* event, 
		const IVarList& args) = 0;
};

inline IGuiHook::~IGuiHook() {}

#endif // _VISUAL_I_GUI_HOOK_H
