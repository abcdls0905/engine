//--------------------------------------------------------------------
// �ļ���:		input_mouse.h
// ��  ��:		
// ˵  ��:		����¼�
// ��������:	2013��6��19��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _INPUT_MOUSE_H_
#define _INPUT_MOUSE_H_

#include "../visual/i_input.h"
#include "input_event.h"


class CMouseEvent :public IMouseEvent
{
public:
	// ����һ������¼�
  	CMouseEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,MouseEventType MouseEventType,FmVec2 mousepos,FmVec4 wheelpos);

	virtual ~CMouseEvent();

	//�¼�����
    virtual InputEventType GetEventType();

	//��ȡ�¼�������
	virtual PERSISTID GetEventSenderID();

	//��ȡ�¼�����
    virtual IVarList* GetEventContent();

	//�¼�����ʱ��
	virtual double GetEventGenTime();

	//��ȡ��ǰ���λ��
	virtual FmVec2 GetMousePos();
	//��ȡ��ǰ��������
	//xy ΪHorizontal position of the pointer
	//zw ΪVertical position of the pointer
	virtual FmVec4 GetWheelPos();
	//��ȡ��ǰ�����¼�����
    virtual MouseEventType GetMouseEventType();
protected:
    PERSISTID m_sender_id;
    IVarList* m_pContent;
	double    m_dGenTime;

private:
    MouseEventType m_MouseEventType;
	FmVec2         m_vec2MousePos;
	FmVec4         m_vec4WheelPos;
};


#endif