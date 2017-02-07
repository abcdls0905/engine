//--------------------------------------------------------------------
// 文件名:		input_keyboard.h
// 内  容:		
// 说  明:		键盘事件
// 创建日期:	2013年6月19日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _INPUT_KEYBOARD_H_
#define _INPUT_KEYBOARD_H_

#include "../visual/i_input.h"
#include "input_event.h"



class CKeyBoardEvent :public IInputKeyBoardEvent
{
public:
	// 构造一个鼠标事件
  	CKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,unsigned char key,KeyBoardEventType keytype);

	virtual ~CKeyBoardEvent();

	//事件类型
    virtual InputEventType GetEventType();

	//获取事件发送者
	virtual PERSISTID GetEventSenderID();

	//获取事件内容
    virtual IVarList* GetEventContent();

	//事件发起时间
	virtual double GetEventGenTime();

	//获取当前触发的按键事件（主按键事件）
	virtual unsigned char GetCurrentEventKey() ;

	//获取当前触发的按键类型
	virtual KeyBoardEventType GetKeyBoardEventType();
protected:
    PERSISTID m_sender_id;
    IVarList* m_pContent;
	double    m_dGenTime;

private:
    KeyBoardEventType m_KeyBoardEventType;
	unsigned char  m_cKey;
};



#endif