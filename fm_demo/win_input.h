//--------------------------------------------------------------------
// �ļ���:		win_input.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��6��20��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WIN_INPUT_H_
#define _WIN_INPUT_H_

#include "../public/module.h"
#include "../public/core_type.h"

#include "../visual/i_input.h"
#include "../visual/i_render.h"

class CInput: public IEntity
{
public:
	// ��ʼ��
	bool Init(const IVarList& args);

	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	bool Shut();

private:

	// �������
	bool EnableControl();
	// ȡ������
	bool DisableControl();

	IInputManager* m_InputManager;
	IRender* m_pRender;
};





#endif