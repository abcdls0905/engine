#include "input_touch.h"


//事件发送者   事件内容  事件发起时间
CTouchEvent::CTouchEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,TouchEventType TouchEventType,const FmVec2* pVecTouchPoint,unsigned int nNum)
{
   m_sender_id = Sender_ID;
   m_pContent = pContent;
   m_dGenTime = dGenTime;
   m_TouchEventType = TouchEventType;
   m_nTouchArrayNum = nNum;
   m_pVecTouchPointArray = pVecTouchPoint;
}

CTouchEvent::~CTouchEvent()
{
}
 

//获取事件发送者
PERSISTID CTouchEvent::GetEventSenderID()
{
	return  m_sender_id;
}

//获取时间内容
IVarList* CTouchEvent::GetEventContent()
{
	return m_pContent;
}

//事件发起时间
double CTouchEvent::GetEventGenTime()
{
	return m_dGenTime;
}
 
//事件类型
InputEventType CTouchEvent::GetEventType()
{
	return EVENT_TOUCH;
}

//触摸事件类型
TouchEventType CTouchEvent::GetTouchEventType()
{
    return m_TouchEventType;
}

//获取查询当前手势点的触摸位置
FmVec2 CTouchEvent::GetTouchPointPos(unsigned int index)
{
	if(index<m_nTouchArrayNum)
		return m_pVecTouchPointArray[index*2];
	return FmVec2();
}

//查询上一次手势点的触摸位置
FmVec2 CTouchEvent::GetTouchPrePointPos(unsigned int index)
{
	if(index<m_nTouchArrayNum)
		return m_pVecTouchPointArray[index*2+1];
	return FmVec2();
}

unsigned int CTouchEvent::GetTouchNum()
{
	return m_nTouchArrayNum;
}