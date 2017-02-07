//--------------------------------------------------------------------
// 文件名:		input_touch.h
// 内  容:		
// 说  明:		多点触摸事件
// 创建日期:	2013年6月18日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _INPUT_TOUCH_H_
#define _INPUT_TOUCH_H_

#include "../visual/i_input.h"
#include "input_event.h"


// 触摸事件
class  CTouchEvent :public ITouchEvent
{
public:
  	CTouchEvent(PERSISTID Sender_ID,IVarList* pContent,double dGenTime,TouchEventType TouchEventType,const FmVec2* pVecTouchPoint,unsigned int nNum);

	virtual ~CTouchEvent();
   //事件发送者   事件内容  事件发起时间 触摸事件类型
	//事件类型
    virtual InputEventType GetEventType();

	//触摸事件类型
    TouchEventType GetTouchEventType();

	//获取事件发送者
	virtual PERSISTID GetEventSenderID();

	//获取事件内容
    virtual IVarList* GetEventContent();

	//事件发起时间
	virtual double GetEventGenTime();

    //获取查询当前手势点的触摸位置
    virtual FmVec2 GetTouchPointPos(unsigned int index) ;

    //查询上一次手势点的触摸位置
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