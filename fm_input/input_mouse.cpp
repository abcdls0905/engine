#include "input_mouse.h"


// 构造一个鼠标事件
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

//事件类型
InputEventType CMouseEvent::GetEventType()
{
	return EVENT_MOUSE;
}


//获取事件发送者
PERSISTID CMouseEvent::GetEventSenderID()
{
	return m_sender_id;
}

//获取事件内容
IVarList* CMouseEvent::GetEventContent()
{
	return m_pContent;
}

//事件发起时间
double CMouseEvent::GetEventGenTime()
{
	return m_dGenTime;
}

//获取当前鼠标位置
FmVec2 CMouseEvent::GetMousePos()
{
	return m_vec2MousePos;
}
//获取当前滚轮坐标
//xy 为Horizontal position of the pointer
//zw 为Vertical position of the pointer
FmVec4 CMouseEvent::GetWheelPos()
{
	return m_vec4WheelPos;
}

//获取当前鼠标的事件类型
MouseEventType CMouseEvent::GetMouseEventType()
{
	return m_MouseEventType;
}