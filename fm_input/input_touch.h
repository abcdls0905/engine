//--------------------------------------------------------------------
// �ļ���:		input_touch.h
// ��  ��:		
// ˵  ��:		��㴥���¼�
// ��������:	2013��6��18��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _INPUT_TOUCH_H_
#define _INPUT_TOUCH_H_

#include "../visual/i_input.h"
#include "input_event.h"


// �����¼�
class  CTouchEvent :public ITouchEvent
{
public:
  	CTouchEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,TouchEventType TouchEventType,const FmVec2* pVecTouchPoint,unsigned int nNum);

	virtual ~CTouchEvent();
   //�¼�������   �¼�����  �¼�����ʱ�� �����¼�����
	//�¼�����
    virtual InputEventType GetEventType();

	//�����¼�����
    TouchEventType GetTouchEventType();

	//��ȡ�¼�������
	virtual PERSISTID GetEventSenderID();

	//��ȡ�¼�����
    virtual IVarList* GetEventContent();

	//�¼�����ʱ��
	virtual double GetEventGenTime();

    //��ȡ��ѯ��ǰ���Ƶ�Ĵ���λ��
    virtual FmVec2 GetTouchPointPos(unsigned int index) ;

    //��ѯ��һ�����Ƶ�Ĵ���λ��
    virtual FmVec2 GetTouchPrePointPos(unsigned int index) ;

    unsigned int GetTouchNum() ;
protected:
    PERSISTID m_sender_id;
    IVarList* m_pContent;
	double    m_dGenTime;
    unsigned int m_nTouchArrayNum;
	const FmVec2* m_pVecTouchPointArray;
private:
	TouchEventType m_TouchEventType;
};


#endif