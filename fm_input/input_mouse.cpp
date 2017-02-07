#include "input_mouse.h"


// ����һ������¼�
CMouseEvent::CMouseEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,MouseEventType MouseEventType,FmVec2 mousepos,FmVec4 wheelpos)
{
    m_sender_id = Sender_ID;
    m_pContent = pContent;
    m_dGenTime = dGenTime;
    m_MouseEventType = MouseEventType;
    m_vec2MousePos = mousepos;
    m_vec4WheelPos = wheelpos;
}

CMouseEvent::~CMouseEvent()
{
}

//�¼�����
InputEventType CMouseEvent::GetEventType()
{
	return EVENT_MOUSE;
}


//��ȡ�¼�������
PERSISTID CMouseEvent::GetEventSenderID()
{
	return m_sender_id;
}

//��ȡ�¼�����
IVarList* CMouseEvent::GetEventContent()
{
	return m_pContent;
}

//�¼�����ʱ��
double CMouseEvent::GetEventGenTime()
{
	return m_dGenTime;
}

//��ȡ��ǰ���λ��
FmVec2 CMouseEvent::GetMousePos()
{
	return m_vec2MousePos;
}
//��ȡ��ǰ��������
//xy ΪHorizontal position of the pointer
//zw ΪVertical position of the pointer
FmVec4 CMouseEvent::GetWheelPos()
{
	return m_vec4WheelPos;
}

//��ȡ��ǰ�����¼�����
MouseEventType CMouseEvent::GetMouseEventType()
{
	return m_MouseEventType;
}