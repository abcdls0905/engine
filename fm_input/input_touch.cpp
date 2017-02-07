#include "input_touch.h"


//�¼�������   �¼�����  �¼�����ʱ��
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
 

//��ȡ�¼�������
PERSISTID CTouchEvent::GetEventSenderID()
{
	return  m_sender_id;
}

//��ȡʱ������
IVarList* CTouchEvent::GetEventContent()
{
	return m_pContent;
}

//�¼�����ʱ��
double CTouchEvent::GetEventGenTime()
{
	return m_dGenTime;
}
 
//�¼�����
InputEventType CTouchEvent::GetEventType()
{
	return EVENT_TOUCH;
}

//�����¼�����
TouchEventType CTouchEvent::GetTouchEventType()
{
    return m_TouchEventType;
}

//��ȡ��ѯ��ǰ���Ƶ�Ĵ���λ��
FmVec2 CTouchEvent::GetTouchPointPos(unsigned int index)
{
	if(index<m_nTouchArrayNum)
		return m_pVecTouchPointArray[index*2];
	return FmVec2();
}

//��ѯ��һ�����Ƶ�Ĵ���λ��
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