#include "input_event.h"


 
//��ȡ�¼�������
PERSISTID CInputEvent::GetEventSenderID()
{
	return  m_sender_id;
}

//��ȡʱ������
IVarList* CInputEvent::GetEventContent()
{
	return m_pContent;
}

//�¼�����ʱ��
double CInputEvent::GetEventGenTime()
{
	return m_dGenTime;
}