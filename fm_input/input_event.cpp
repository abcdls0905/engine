#include "input_event.h"


 
//获取事件发送者
PERSISTID CInputEvent::GetEventSenderID()
{
	return  m_sender_id;
}

//获取时间内容
IVarList* CInputEvent::GetEventContent()
{
	return m_pContent;
}

//事件发起时间
double CInputEvent::GetEventGenTime()
{
	return m_dGenTime;
}