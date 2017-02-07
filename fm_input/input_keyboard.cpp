#include "input_keyboard.h"

// ����һ������¼�
CKeyBoardEvent::CKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,unsigned char key,KeyBoardEventType keytype)
{
	m_sender_id = Sender_ID;
	m_pContent = pContent;
	m_dGenTime = dGenTime;
	m_KeyBoardEventType = keytype;
	m_cKey = key;
}

CKeyBoardEvent::~CKeyBoardEvent()
{
}

//�¼�����
InputEventType CKeyBoardEvent::GetEventType()
{
	return EVENT_KEYBOARD;
}


//��ȡ�¼�������
PERSISTID CKeyBoardEvent::GetEventSenderID()
{
	return m_sender_id;
}

//��ȡ�¼�����
IVarList* CKeyBoardEvent::GetEventContent()
{
	return m_pContent;
}

//�¼�����ʱ��
double CKeyBoardEvent::GetEventGenTime()
{
	return m_dGenTime;
}

//��ȡ��ǰ�����¼�����
KeyBoardEventType CKeyBoardEvent::GetKeyBoardEventType()
{
	return m_KeyBoardEventType;
}

//��ȡ��ǰ�����İ����¼����������¼���
unsigned char CKeyBoardEvent::GetCurrentEventKey()
{
	return m_cKey;
}