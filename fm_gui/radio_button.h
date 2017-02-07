//--------------------------------------------------------------------
// �ļ���:		radio_button.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _RADIO_BUTTON_H
#define _RADIO_BUTTON_H

#include "i_control.h"
#include "check_button.h"

// ��ѡ��

class RadioButton : public CheckButton  
{
public:
	RadioButton();
	virtual ~RadioButton();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual bool SetCheckValue(bool value);
};

#endif // _RADIO_BUTTON_H

