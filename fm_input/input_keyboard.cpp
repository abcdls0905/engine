#include "input_keyboard.h"

// 构造一个鼠标事件
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

//事件类型
InputEventType CKeyBoardEvent::GetEventType()
{
	return EVENT_KEYBOARD;
}


//获取事件发送者
PERSISTID CKeyBoardEvent::GetEventSenderID()
{
	return m_sender_id;
}

//获取事件内容
IVarList* CKeyBoardEvent::GetEventContent()
{
	return m_pContent;
}

//事件发起时间
double CKeyBoardEvent::GetEventGenTime()
{
	return m_dGenTime;
}

//获取当前鼠标的事件类型
KeyBoardEventType CKeyBoardEvent::GetKeyBoardEventType()
{
	return m_KeyBoardEventType;
}

//获取当前触发的按键事件（主按键事件）
unsigned char CKeyBoardEvent::GetCurrentEventKey()
{
	return m_cKey;
}