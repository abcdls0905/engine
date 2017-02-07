//--------------------------------------------------------------------
// 文件名:		input_event.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年5月14日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __INPUT_EVENT_H__
#define __INPUT_EVENT_H__
#include "../visual/i_input.h"

//事件基类
class CInputEvent:public IInputEvent
{
public:
	//获取事件发送者
	virtual PERSISTID GetEventSenderID();

	//获取事件内容
    virtual IVarList* GetEventContent();

	//事件发起时间
	virtual double GetEventGenTime();
protected:
    PERSISTID m_sender_id;
    IVarList* m_pContent;
	double    m_dGenTime;
};


#endif