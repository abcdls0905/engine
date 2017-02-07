//--------------------------------------------------------------------
// �ļ���:		i_gui_hook.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��2��21��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_GUI_HOOK_H
#define _VISUAL_I_GUI_HOOK_H

#include "../public/i_var_list.h"

// ����ص�

class IEntity;

class IGuiHook
{
public:
	virtual ~IGuiHook() = 0;
	// �ͷ�
	virtual void Release() = 0;
	// �¼�
	virtual int OnEvent(IEntity* pEntity, const char* event, 
		const IVarList& args) = 0;
};

inline IGuiHook::~IGuiHook() {}

#endif // _VISUAL_I_GUI_HOOK_H
