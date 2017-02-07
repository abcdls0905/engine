//--------------------------------------------------------------------
// �ļ���:		input_event.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��5��14��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __INPUT_EVENT_H__
#define __INPUT_EVENT_H__
#include "../visual/i_input.h"

//�¼�����
class CInputEvent:public IInputEvent
{
public:
	//��ȡ�¼�������
	virtual PERSISTID GetEventSenderID();

	//��ȡ�¼�����
    virtual IVarList* GetEventContent();

	//�¼�����ʱ��
	virtual double GetEventGenTime();
protected:
    PERSISTID m_sender_id;
    IVarList* m_pContent;
	double    m_dGenTime;
};


#endif