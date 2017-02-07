//--------------------------------------------------------------------
// 文件名:		input_mouse.h
// 内  容:		
// 说  明:		鼠标事件
// 创建日期:	2013年6月19日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _INPUT_MOUSE_H_
#define _INPUT_MOUSE_H_

#include "../visual/i_input.h"
#include "input_event.h"


class CMouseEvent :public IMouseEvent
{
public:
	// 构造一个鼠标事件
  	CMouseEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,MouseEventType MouseEventType,FmVec2 mousepos,FmVec4 wheelpos);

	virtual ~CMouseEvent();

	//事件类型
    virtual InputEventType GetEventType();

	//获取事件发送者
	virtual PERSISTID GetEventSenderID();

	//获取事件内容
    virtual IVarList* GetEventContent();

	//事件发起时间
	virtual double GetEventGenTime();

	//获取当前鼠标位置
	virtual FmVec2 GetMousePos();
	//获取当前滚轮坐标
	//xy 为Horizontal position of the pointer
	//zw 为Vertical position of the pointer
	virtual FmVec4 GetWheelPos();
	//获取当前鼠标的事件类型
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