//--------------------------------------------------------------------
// �ļ���:		input_keyboard.h
// ��  ��:		
// ˵  ��:		�����¼�
// ��������:	2013��6��19��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _INPUT_KEYBOARD_H_
#define _INPUT_KEYBOARD_H_

#include "../visual/i_input.h"
#include "input_event.h"



class CKeyBoardEvent :public IInputKeyBoardEvent
{
public:
	// ����һ������¼�
  	CKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,unsigned char key,KeyBoardEventType keytype);

	virtual ~CKeyBoardEvent();

	//�¼�����
    virtual InputEventType GetEventType();

	//��ȡ�¼�������
	virtual PERSISTID GetEventSenderID();

	//��ȡ�¼�����
    virtual IVarList* GetEventContent();

	//�¼�����ʱ��
	virtual double GetEventGenTime();

	//��ȡ��ǰ�����İ����¼����������¼���
	virtual unsigned char GetCurrentEventKey() ;

	//��ȡ��ǰ�����İ�������
	virtual KeyBoardEventType GetKeyBoardEventType();
protected:
    PERSISTID m_sender_id;
    IVarList* m_pContent;
	double    m_dGenTime;

private:
    KeyBoardEventType m_KeyBoardEventType;
	unsigned char  m_cKey;
};



#endif